#include <windows.h>
#include <ShellAPI.h> 
#include <thread>
#include <sstream>
#include "JWebTop_DLLEx.h"
#include "common/util/StrUtil.h"
#include "common/JWebTopMsg.h"
#include "common/task/Task.h"
#include "common/msgwin/MsgWin.h"
#include "JWebTop/config/JWebTopConfigs.h"
#include "JWebTop/browser/JWebTopClient.h"
#include "JWebTop/browser/JWebTopCommons.h"
#include "JWebTop/wndproc/JWebTopWndProc.h"

#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif

using namespace std;
extern HWND g_LocalWinHWnd;  // 本地创建的消息窗口HWND
extern HWND g_RemoteWinHWnd;  // 远程进程的消息窗口HWND
namespace jw{
	//wstring g_TaskId;                 // 创建消息窗口或浏览器时的任务id
	namespace dllex{
		bool ex(){ return g_RemoteWinHWnd != NULL; }

		void waitRemoteProcessTerminate(){
			try{
				DWORD lpdwProcessId;
				GetWindowThreadProcessId(g_RemoteWinHWnd, &lpdwProcessId);
				//HANDLE hHandle = GetProcessHandleFromHwnd(g_RemoteWinHWnd);// 此函数的头文件不确定是哪个(Header: Use LoadLibrary and GetProcAddress.  Library: Use Oleacc.dll.)
				HANDLE hHandle = OpenProcess(PROCESS_ALL_ACCESS, false, lpdwProcessId);
				WaitForSingleObject(hHandle, INFINITE);
			}
			catch (...){
			}
			jw::sendProcessMsg(g_LocalWinHWnd, WM_COPYDATA_EXIT, L"");// 通知本进程主窗口，程序需要关闭
		}
		// 创建一个线程用来监听远程进程是否终止以便结束当前程序
		void waitForRemoteProcessTerminate(){
			std::thread waitRemoteProcessThread(waitRemoteProcessTerminate);
			waitRemoteProcessThread.detach();// 在独立线程中等待远程进程的结束消息
		}

		void browserwin_thread_executeWmCopyData(HWND browserHWnd, DWORD msgId, wstring msg){
			BrowserWindowInfo * bwInfo = getBrowserWindowInfo(browserHWnd);
			switch (msgId)
			{
			case JWM_RESULT_RETURN:// 远程任务已完成，结果发回来了，需要通知本进程的等待线程去获取结果
			{
									   wstring taskId, result;
									   long remoteHWnd;
									   jw::parseMessageJSON(msg, ref(remoteHWnd), ref(taskId), ref(result));  // 从任务信息中解析出任务id和任务描述
									   jw::task::putTaskResult(taskId, result);	   					   // 通知等待线程，远程任务已完成，结果已去取回
									   break;
			}
			case JWM_JSON_EXECUTE_WAIT:// 远程进程发来一个任务，并且远程进程正在等待，任务完成后需要发送JWEBTOP_MSG_RESULT_RETURN消息给远程进程
			case JWM_JS_EXECUTE_WAIT:// 远程进程发来一个任务，并且远程进程正在等待，任务完成后需要发送JWEBTOP_MSG_RESULT_RETURN消息给远程进程
			{

										 CefRefPtr<CefProcessMessage> cefMsg = CefProcessMessage::Create("waitjs");
										 CefRefPtr<CefListValue> args = cefMsg->GetArgumentList();
										 args->SetInt(0, msgId);
										 args->SetString(1, msg);
										 bwInfo->browser->SendProcessMessage(PID_RENDERER, cefMsg);// 直接发送到render进程去执行
										 break;
			}
			case JWM_JSON_EXECUTE_RETURN:
				msg = L"invokeByDLL(" + msg + L")";// 包装json为js调用 
				bwInfo->browser->GetMainFrame()->ExecuteJavaScript(msg, "", 0);
				break;
			case JWM_JS_EXECUTE_RETURN:
				bwInfo->browser->GetMainFrame()->ExecuteJavaScript(msg, "", 0);
				break;
			case JWM_CLOSEBROWSER:
				jb::close((HWND)parseLong(msg));
				break;
			default:
				break;
			}
		}
		LRESULT onBrowserWinWmCopyData(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
			wstring msg;
			DWORD msgId = 0;
			jw::parseProcessMsg(lParam, ref(msgId), ref(msg));
			std::thread t(browserwin_thread_executeWmCopyData, hWnd, msgId, msg);// onWmCopyData是同步消息，为了防止另一进程的等待，这里在新线程中进行业务处理
			t.detach();// 从当前线程分离
			return JWEBTOP_MSG_SUCCESS;
		}

		void sendBrowserCreatedMessage(wstring taskId, long browserHWnd){
			wstringstream wss; wss << browserHWnd;
			wstring ws = wss.str();
			wstring wrapped = jw::wrapAsTaskJSON((long)g_RemoteWinHWnd, std::ref(taskId), std::ref(ws));
			sendProcessMsg(g_RemoteWinHWnd, JWM_RESULT_RETURN, LPTSTR(wrapped.c_str()));
		}

		bool sendJWebTopProcessMsg(HWND browserHWnd, DWORD msgId, LPTSTR msg){
			BrowserWindowInfo * bw = getBrowserWindowInfo(browserHWnd);
			if (bw == NULL)return false;
			if (g_RemoteWinHWnd == NULL)return false;
			return jw::sendProcessMsg(g_RemoteWinHWnd, msgId, msg);
		}

		void syncExecuteJS(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message){
			CefRefPtr<CefListValue> args = message->GetArgumentList();
			CefString msg = args->GetString(1);
			wstring remoteTaskId, taskInfo;
			long remoteHWnd;
			jw::parseMessageJSON(msg, ref(remoteHWnd), ref(remoteTaskId), ref(taskInfo));  // 从任务信息中解析出任务id和任务描述
			CefString rtn;
			CefRefPtr<CefV8Context>  v8 = browser->GetMainFrame()->GetV8Context();
			long browserHWnd = 0;
			if (v8 != NULL){
				int msgId = args->GetInt(0);
				if (v8->Enter()){
					CefRefPtr<CefV8Value> reval;
					CefRefPtr<CefV8Exception> exception;
					if (msgId == JWM_JSON_EXECUTE_WAIT){
						taskInfo = L"invokeByDLL(" + taskInfo + L")";// 包装json为js调用 
					}
					//taskInfo = L"testInvoke()";// FIXME：调试用
					if (v8->Eval(taskInfo, reval, exception)){// 执行JS
						rtn = reval->GetStringValue();
					}
					CefRefPtr<CefV8Value> global = v8->GetGlobal();
					CefRefPtr<CefV8Value> jwebtop = global->GetValue(CefString("JWebTop"));
					CefRefPtr<CefV8Value> handler = jwebtop->GetValue("handler");
					browserHWnd = handler->GetIntValue();// 从JWebTop.handler获取窗口句柄
					v8->Exit();
				}
			}
			wstring result = rtn.ToWString();
			wstring wrapped = jw::wrapAsTaskJSON(browserHWnd, std::ref(remoteTaskId), std::ref(result)); // 包装结果任务
			//FIXME：多进程时下面的方法会失败，因为获取不到browserHWnd对应的BrowserWindowInfo
			sendJWebTopProcessMsg((HWND)browserHWnd, JWM_RESULT_RETURN, LPTSTR(wrapped.c_str())); // 发送结果到远程进程
		}

		CefString invokeRemote_Wait(HWND browserHWnd, CefString json){
			wstring taskId = jw::task::createTaskId();			         // 生成任务id
			// taskId附加到json字符串上
			wstring newjson = json.ToWString();
			wstring wrapped = jw::wrapAsTaskJSON((long)browserHWnd, std::ref(taskId), std::ref(newjson));
			jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // 放置任务到任务池
			if (sendJWebTopProcessMsg(browserHWnd, JWM_DLL_EXECUTE_WAIT, LPTSTR(wrapped.c_str()))){ // 发送任务到远程进程
				lock->wait();		             		 		             // 等待任务完成
				wstring result = lock->result;   		 		             // 取回执行结果
				return CefString(result);									 // 返回数据
			}
			else{
				jw::task::removeTask(taskId);								// 消息发送失败移除现有消息
				return CefString();											// 返回数据：注意这里是空字符串
			}
		}

		void invokeRemote_NoWait(HWND browserHWnd, CefString json){
			sendJWebTopProcessMsg(browserHWnd, JWM_DLL_EXECUTE_RETURN, LPTSTR(json.ToWString().c_str())); // 发送任务到远程进程
		}
	}

	// 处理消息窗口的WM_COPYDATA消息
	void msgwin_thread_executeWmCopyData(DWORD msgId, std::wstring msg){
		if (msgId == JWM_CREATEBROWSER){
			wstring taskId, result;
			long remoteHWnd;
			jw::parseMessageJSON(msg, ref(remoteHWnd), ref(taskId), ref(result));  // 从任务信息中解析出任务id和任务描述
			createNewBrowser(JWebTopConfigs::parseCreateBrowserCmdLine(result),taskId);
		}
		// 暂时不需要其他任何处理
	}
	// 用于createWin进行回调
	void onWindowHwndCreated(HWND hWnd, LPTSTR szCmdLine){
		if (g_RemoteWinHWnd != NULL){// 如果有设置远程窗口的句柄，那么向其发送当前窗口的句柄
			wstring taskId = wstring(szCmdLine);
			wstringstream wss; wss << ((long)hWnd);
			wstring ws = wss.str();
			wstring wrapped = jw::wrapAsTaskJSON((long)g_RemoteWinHWnd, std::ref(taskId), std::ref(ws));
			sendProcessMsg(g_RemoteWinHWnd, JWM_RESULT_RETURN, LPTSTR(wrapped.c_str()));
		}
	}
}