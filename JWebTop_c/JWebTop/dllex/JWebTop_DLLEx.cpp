#include <windows.h>
#include <ShellAPI.h> 
#include <thread>
#include <sstream>
#include "JWebTop_DLLEx.h"
#include "common/util/StrUtil.h"
#include "common/JWebTopMsg.h"
#include "common/task/Task.h"
#include "JWebTop/config/JWebTopConfigs.h"
#include "JWebTop/browser/JWebTopClient.h"
#include "JWebTop/browser/JWebTopCommons.h"
#include "JWebTop/browser/JWebTopContext.h"
#include "JWebTop/wndproc/JWebTopWndProc.h"
#include "common/fastipc/Server.h"
#include "common/fastipc/Client.h"
#include "include/cef_app.h"

#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif

using namespace std;
namespace jw{
	namespace dllex{
		bool isEx = false;

		fastipc::Server server;
		fastipc::Client client;

		bool ex(){ return isEx; }
		void setAsEx(){ isEx = true; }

		void closeWebTopEx();

		void __onRead(LONG userMsgType, LONG userValue, std::string taskIds, std::string datas){
			std::wstring taskId = jw::s2w(taskIds);
			std::wstring data = jw::s2w(datas);
			switch (userMsgType){
			case JWM_RESULT_RETURN:
				jw::task::putTaskResult(taskId, data); // 通知等待线程，远程任务已完成，结果已取回
				return;
			case JWM_JSON_EXECUTE_WAIT:// 远程进程发来一个任务，并且远程进程正在等待，任务完成后需要发送JWEBTOP_MSG_RESULT_RETURN消息给远程进程
			case JWM_JS_EXECUTE_WAIT:// 远程进程发来一个任务，并且远程进程正在等待，任务完成后需要发送JWEBTOP_MSG_RESULT_RETURN消息给远程进程
			{
										 BrowserWindowInfo * bwInfo = getBrowserWindowInfo((HWND)userValue);
										 CefRefPtr<CefProcessMessage> cefMsg = CefProcessMessage::Create("waitjs");
										 CefRefPtr<CefListValue> args = cefMsg->GetArgumentList();
										 args->SetInt(0, userMsgType);
										 args->SetString(1, data);
										 args->SetString(2, taskId);
										 bwInfo->browser->SendProcessMessage(PID_RENDERER, cefMsg);// 直接发送到render进程去执行
										 break;
			}
			case JWM_JSON_EXECUTE_RETURN:{
											 BrowserWindowInfo * bwInfo = getBrowserWindowInfo((HWND)userValue);
											 data = L"invokeByDLL(" + data + L")";// 包装json为js调用 
											 bwInfo->browser->GetMainFrame()->ExecuteJavaScript(data, "", 0);
											 break; }
			case JWM_JS_EXECUTE_RETURN:{
										   BrowserWindowInfo * bwInfo = getBrowserWindowInfo((HWND)userValue);
										   bwInfo->browser->GetMainFrame()->ExecuteJavaScript(data, "", 0);
										   break;
			}
			case JWM_CLOSEBROWSER:
				jb::close((HWND)userValue);
			case JWM_CREATEBROWSER_JSON:
				createNewBrowser(JWebTopConfigs::loadAsJSON(data), taskId);
				return;
			case JWM_CREATEBROWSER_FILE:
				createNewBrowser(JWebTopConfigs::loadConfigs(data), taskId);
				return;
			case JWM_CFGJWEBTOP_FILE:
				jw::ctx::startJWebTopByFile(data);
				return;
			case JWM_CEF_ExitAPP:
				closeWebTopEx();
			}
		}

		class EXEReadListener :public fastipc::RebuildedBlockListener{
			void onRebuildedRead(fastipc::MemBlock* memBlock){
#ifdef JWebTopLog
				std::wstringstream wss;
				wss << L"Readed "
					<< L" userMsgType=" << memBlock->userMsgType
					<< L" userValue=" << memBlock->userValue
					<< L" userShortStr=" << jw::s2w(memBlock->getUserShortStr())
					<< L" pBuff=" << jw::s2w(memBlock->getData())
					<< L"||\r\n";
				writeLog(wss.str());
#endif
				thread t(__onRead, memBlock->userMsgType, memBlock->userValue, memBlock->getUserShortStr(), memBlock->getData());
				t.detach();// 在一个分离线程中完成其他操作，避免阻塞数据的收发
			}
		};

		void __startIPCServer(){
			server.startRead();
		}
		void __waitProcess(DWORD processId){
			// 等待远程进程结束
			try{
				HANDLE hHandle = OpenProcess(SYNCHRONIZE, false, processId);// 降低权限，否则有些情况下OpendProcess失败（比如xp）
				WaitForSingleObject(hHandle, INFINITE);// 等待远程进程结束
				closeWebTopEx();
			}
			catch (...){
#ifdef JWebTopLog
				writeLog("等待远程进程结束出错。。。。。\r\n");
#endif
			}
		}
		int startIPCServer(wstring &serverName, DWORD blockSize, DWORD processId){
			int r = client.create(serverName, blockSize);
			if (r != 0)return r;
			r = server.create(fastipc::genServerName(serverName), blockSize);
			if (r != 0)return r;
			server.setListener(new EXEReadListener());// 设置侦听器
			thread startServer(__startIPCServer);
			startServer.detach();								// 在新线程中启动exe端的FastIPC监听
			client.write(JWM_IPC_CLIENT_OK, 0, NULL, NULL, 0);	// 通知dll端，子进程已创建，可以进行浏览器的初始化了
			thread waitProcess(__waitProcess, processId);		// 在新线程中启动等待远程进程结束的监听
			waitProcess.detach();
			return 0;
		}
		void closeWebTopEx(){
			isEx = false;					// 标记为非ex模式，以便退出cef侦听
			jw::ctx::CloseAllBrowsers(true);// 关闭所有浏览器
			server.close();					// 关闭ipc侦听
		}
		void OnContextInitialized(){
			client.write(JWM_CEF_APP_INITED, 0, NULL, NULL, 0); // CEF浏览器已初始完成
		}

		void sendBrowserCreatedMessage(wstring taskId, long browserHWnd){
#ifdef JWebTopLog
			std::wstringstream wss;
			wss << L"Writed "
				<< L" sendBrowserCreatedMessage"
				<< L" taskId=" << taskId
				<< L" browserHWnd=" << browserHWnd
				<< L"||\r\n";
			writeLog(wss.str());
#endif
			client.write(JWM_BROWSER_CREATED, browserHWnd, LPSTR(jw::w2s(taskId).c_str()), NULL, 0);
		}

		bool sendJWebTopProcessMsg(HWND browserHWnd, DWORD msgId, wstring msg, long senderHWND, wstring taskId){
			BrowserWindowInfo * bw = getBrowserWindowInfo(browserHWnd);
			if (bw == NULL)return false;
#ifdef JWebTopLog
			std::wstringstream wss;
			wss << L"Writed "
				<< L" sendJWebTopProcessMsg"
				<< L" taskId=" << taskId
				<< L" browserHWnd=" << browserHWnd
				<< L" msgId=" << msgId
				<< L" msg=" << msg
				<< L"||\r\n";
			writeLog(wss.str());
#endif
			string tmp = jw::w2s(msg);
			return client.write(msgId, (long)browserHWnd, LPSTR(jw::w2s(taskId).c_str()), LPSTR(tmp.c_str()), tmp.size()) == 0;
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
}