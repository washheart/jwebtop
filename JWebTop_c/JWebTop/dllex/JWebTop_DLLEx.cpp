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
	namespace dllex{
		bool ex(){ return g_RemoteWinHWnd != NULL; }

		void waitRemoteProcessTerminate(){
			try{
				DWORD lpdwProcessId;
				GetWindowThreadProcessId(g_RemoteWinHWnd, &lpdwProcessId);
				//HANDLE hHandle = GetProcessHandleFromHwnd(g_RemoteWinHWnd);// 此函数的头文件不确定是哪个(Header: Use LoadLibrary and GetProcAddress.  Library: Use Oleacc.dll.)
				HANDLE hHandle = OpenProcess(SYNCHRONIZE, false, lpdwProcessId);// 降低权限，否则有些情况下OpendProcess失败（比如xp）
				WaitForSingleObject(hHandle, INFINITE);// 等待进程结束
			}
			catch (...){
			}
			::SendMessage(g_LocalWinHWnd, WM_DESTROY, 0, 0);// 因为是同一进程，直接发送销毁窗口的消息即可，不用WM_COPYDATA来中转
		}
		// 创建一个线程用来监听远程进程是否终止以便结束当前程序
		void waitForRemoteProcessTerminate(){
			std::thread waitRemoteProcessThread(waitRemoteProcessTerminate);
			waitRemoteProcessThread.detach();// 在独立线程中等待远程进程的结束消息
		}

		void browserwin_thread_executeWmCopyData(HWND browserHWnd, DWORD msgId, wstring msg,long senderHWnd,wstring taskId){
			BrowserWindowInfo * bwInfo = getBrowserWindowInfo(browserHWnd);
			switch (msgId)
			{
			case JWM_RESULT_RETURN:// 远程任务已完成，结果发回来了，需要通知本进程的等待线程去获取结果
			{
									   jw::task::putTaskResult(taskId, msg);	   					   // 通知等待线程，远程任务已完成，结果已去取回
									   break;
			}
			case JWM_JSON_EXECUTE_WAIT:// 远程进程发来一个任务，并且远程进程正在等待，任务完成后需要发送JWEBTOP_MSG_RESULT_RETURN消息给远程进程
			case JWM_JS_EXECUTE_WAIT:// 远程进程发来一个任务，并且远程进程正在等待，任务完成后需要发送JWEBTOP_MSG_RESULT_RETURN消息给远程进程
			{

										 CefRefPtr<CefProcessMessage> cefMsg = CefProcessMessage::Create("waitjs");
										 CefRefPtr<CefListValue> args = cefMsg->GetArgumentList();
										 args->SetInt(0, msgId);
										 args->SetString(1, msg);
										 args->SetString(2, taskId);
										 //args->SetInt(3, (long)senderHWnd);
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
			wstring msg,taskId;
			DWORD msgId = 0;
			jw::parseProcessMsg(lParam, ref(msgId), ref(msg), ref(taskId));
			long senderHWnd = wParam;
			std::thread t(browserwin_thread_executeWmCopyData, hWnd, msgId, msg, senderHWnd, taskId);// onWmCopyData是同步消息，为了防止另一进程的等待，这里在新线程中进行业务处理
			t.detach();// 从当前线程分离
			return JWEBTOP_MSG_SUCCESS;
		}

		void sendBrowserCreatedMessage(wstring taskId, long browserHWnd){
			wstringstream wss; wss << browserHWnd; wstring ws = wss.str();
			sendProcessMsg(g_RemoteWinHWnd, JWM_RESULT_RETURN, ws.c_str(), browserHWnd, taskId);
		}

		bool sendJWebTopProcessMsg(HWND browserHWnd, DWORD msgId, wstring msg, long senderHWND, wstring taskId){
			BrowserWindowInfo * bw = getBrowserWindowInfo(browserHWnd);
			if (bw == NULL)return false;
			if (g_RemoteWinHWnd == NULL)return false;
			return jw::sendProcessMsg(g_RemoteWinHWnd, msgId, msg, senderHWND, taskId);
		}

		void syncExecuteJS(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message){
			CefRefPtr<CefListValue> args = message->GetArgumentList();
			wstring taskInfo = args->GetString(1).ToWString();
			wstring remoteTaskId = args->GetString(2);
			//long remoteHWnd = args->GetInt(3);
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
			sendJWebTopProcessMsg((HWND)browserHWnd, JWM_RESULT_RETURN, result, (long)browserHWnd, remoteTaskId); // 发送结果到远程进程
		}

		CefString invokeRemote_Wait(HWND browserHWnd, CefString json){
			wstring taskId = jw::task::createTaskId();			         // 生成任务id
			wstring newjson = json.ToWString();
			jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // 放置任务到任务池
			if (sendJWebTopProcessMsg(browserHWnd, JWM_DLL_EXECUTE_WAIT, newjson, (long)browserHWnd, taskId)){ // 发送任务到远程进程
				wstring result = lock->wait();     		 		             // 等待任务完成并取回执行结果
				return CefString(result);									 // 返回数据
			}
			else{
				jw::task::removeTask(taskId);								// 消息发送失败移除现有消息
				return CefString();											// 返回数据：注意这里是空字符串
			}
		}

		void invokeRemote_NoWait(HWND browserHWnd, CefString json){
			wstring newjson = json.ToWString();
			sendJWebTopProcessMsg(browserHWnd, JWM_DLL_EXECUTE_RETURN, newjson, (long)browserHWnd, wstring()); // 发送任务到远程进程
		}
	}

	// 处理消息窗口的WM_COPYDATA消息
	void msgwin_thread_executeWmCopyData(DWORD msgId, wstring msg, long senderHWnd, wstring taskId){
		if (msgId == JWM_CREATEBROWSER){
			createNewBrowser(JWebTopConfigs::parseCreateBrowserCmdLine(msg), taskId);
		}
		// 暂时不需要其他任何处理
	}
	// 用于createWin进行回调
	void onWindowHwndCreated(HWND hWnd, LPTSTR szCmdLine){
		wstringstream wss;
		if (g_RemoteWinHWnd != NULL){// 如果有设置远程窗口的句柄，那么向其发送当前窗口的句柄
			wstring taskId = wstring(szCmdLine);
			wstringstream wss; wss << ((long)hWnd);	
			sendProcessMsg(g_RemoteWinHWnd, JWM_RESULT_RETURN, wss.str(), (long)hWnd, taskId);
		}
	}
}