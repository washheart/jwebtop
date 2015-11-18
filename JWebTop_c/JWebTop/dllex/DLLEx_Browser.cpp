#include <windows.h>
#include <thread>
#include <sstream>
#include "JWebTop_DLLEx.h"
#include "common/util/StrUtil.h"
#include "common/JWebTopMsg.h"
#include "JWebTop/config/JWebTopConfigs.h"
#include "JWebTop/browser/JWebTopClient.h"
#include "JWebTop/browser/JWebTopCommons.h"
#include "JWebTop/browser/JWebTopContext.h"
#include "JWebTop/wndproc/JWebTopWndProc.h"
#include "common/fastipc/Server.h"
#include "common/fastipc/Client.h"
#include "include/cef_app.h"
#include "DLLExState.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif

//#define FLAG_TASK_TIMER 100
using namespace std;
extern CefSettings settings;                      // CEF全局设置
namespace jw{
	namespace dllex{
		bool isEx = false;
		fastipc::Server * server;
		fastipc::Client * client;

		wstring g_serverName;
		DWORD	g_blockSize = 0;

		bool ex(){ return isEx; }
		void setAsEx(){ isEx = true; }

		void closeWebTopEx();
		bool sendJWebTopProcessMsg(HWND browserHWnd, DWORD msgId, wstring& msg, wstring& taskId);
		void executeJSCallBack(CefRefPtr<CefFrame> frame, wstring callback, wstring result);

		bool setMsgToRender(HWND browserHWnd, CefRefPtr<CefProcessMessage> cefMsg){
			BrowserWindowInfo * bw = getBrowserWindowInfo((HWND)browserHWnd);
			if (bw == NULL)return false;
			return bw->browser->SendProcessMessage(PID_RENDERER, cefMsg);
		}
		void sendIPCServerInfo(HWND browserHWnd){
			if (settings.single_process)return;// 没有开启独立的render进程
			CefRefPtr<CefProcessMessage> tmp = CefProcessMessage::Create(B2R_MSG_NAME);
			CefRefPtr<CefListValue> args = tmp->GetArgumentList();
			args->SetInt(0, JWM_B2R_SERVERINFO);
			args->SetInt(1, g_blockSize);
			args->SetString(2, g_serverName);
			BrowserWindowInfo * bw = getBrowserWindowInfo((HWND)browserHWnd);
			bw->browser->SendProcessMessage(PID_RENDERER, tmp);
		}
		void __onRead(LONG userMsgType, LONG userValue, std::string& taskIds, std::string& datas){
			std::wstring taskId = jw::s2w(taskIds);
			std::wstring data = jw::s2w(datas);
#ifdef JWebTopLog
			std::wstringstream wss;
			wss << L"start__onRead "
				<< L" userMsgType=" << userMsgType
				<< L" userValue=" << userValue
				<< L" userShortStr=" << taskId
				<< L" pBuff=" << data
				<< L"||\r\n";
			writeLog(wss.str());
#endif
			switch (userMsgType){
			case JWM_RESULT_RETURN:{
									   wstring callBack = DLLExState::findAndRemoveCallBack((HWND)userValue, taskId);
									   if (callBack.empty()){
										   CefRefPtr<CefProcessMessage> cefMsg = CefProcessMessage::Create(B2R_MSG_NAME);
										   CefRefPtr<CefListValue> args = cefMsg->GetArgumentList();
										   args->SetInt(0, JWM_B2R_TASKRESULT);
										   args->SetString(1, taskId);
										   args->SetString(2, data);
										   args->SetInt(3, userValue);
										   setMsgToRender((HWND)userValue, cefMsg);
									   }
									   else{
										   BrowserWindowInfo * bwInfo = getBrowserWindowInfo((HWND)userValue);
										   executeJSCallBack(bwInfo->browser->GetMainFrame(), callBack, data);
									   }
									   break;
			}
			case JWM_JSON_EXECUTE_WAIT:// 远程进程发来一个任务，并且远程进程正在等待，任务完成后需要发送JWEBTOP_MSG_RESULT_RETURN消息给远程进程
			case JWM_JS_EXECUTE_WAIT:// 远程进程发来一个任务，并且远程进程正在等待，任务完成后需要发送JWEBTOP_MSG_RESULT_RETURN消息给远程进程
				try{
					CefRefPtr<CefProcessMessage> cefMsg = CefProcessMessage::Create(B2R_MSG_NAME);
					CefRefPtr<CefListValue> args = cefMsg->GetArgumentList();
					args->SetInt(0, userMsgType);
					args->SetString(1, data);
					args->SetString(2, taskId);
					if (!setMsgToRender((HWND)userValue, cefMsg)){// 直接发送到render进程去执行
#ifdef JWebTopLog
						writeLog(L"发送JS到render进程失败，taskId="); writeLog(taskId); writeLog(L"\r\n");
#endif
						wstring empty;
						sendJWebTopProcessMsg((HWND)userValue, JWM_RESULT_RETURN, empty, taskId); // 发送结果到远程进程
					}
				}
				catch (...){
#ifdef JWebTopLog
					writeLog(L"执行JS出现异常，taskId="); writeLog(taskId); writeLog(L"\r\n");
#endif
					wstring empty;
					sendJWebTopProcessMsg((HWND)userValue, JWM_RESULT_RETURN, empty, taskId); // 发送结果到远程进程
				}
				break;
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
			case JWM_M_APPEND_JS:
				DLLExState::findOrCreateExState((HWND)userValue)->setAppendJS(data);// 将JS保存下来，每次OnLoadEnd后执行
				break;
			case JWM_M_SETURL:{
								  DLLExState::unlockBrowserLocks((HWND)userValue);// 每次主页面重新加载之后，都解锁之前页面绑定的JS任务
								  BrowserWindowInfo * bwInfo = getBrowserWindowInfo((HWND)userValue);
								  bwInfo->browser->GetMainFrame()->LoadURL(data);
								  break;
			}
			case JWM_CLOSEBROWSER:
				jb::close((HWND)userValue);
				break;
			case JWM_CREATEBROWSER_JSON:
				createNewBrowser(JWebTopConfigs::loadAsJSON(data), taskId);
				break;
			case JWM_CREATEBROWSER_FILE:
				createNewBrowser(JWebTopConfigs::loadConfigs(data), taskId);
				break;
			case JWM_CFGJWEBTOP_FILE:
				jw::ctx::startJWebTopByFile(data);
				break;
			case JWM_SET_ERR_URL:
				JWebTopConfigs::setErrorURL(data);
				break;
			//case JWM_SET_TASK_WAIT_TIME:
			//	jw::task::setDefaultTaskWiatTime(userValue);
			//	break;
			case JWM_CEF_ExitAPP:
				closeWebTopEx();
				break;
			default:
				MessageBox(NULL, L"用户定义消息值不对！", L"错误", 0);
				break;
			}
#ifdef JWebTopLog
			wss.str(L"");
			wss << L"end__onRead "
				<< L" userMsgType=" << userMsgType
				<< L" userValue=" << userValue
				<< L" userShortStr=" << taskId
				<< L" pBuff=" << data
				<< L"||\r\n";
			writeLog(wss.str());
#endif
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
			server->startRead();
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
			client = new fastipc::Client();
			int r = client->create(serverName, blockSize);
			if (!settings.single_process){// 如果是多进程模式，将服务器的信息记录到共享内存，以便render进程获取到相关信息
				g_serverName = serverName;
				g_blockSize = blockSize;
			}
			if (r != 0)return r;
			server = new fastipc::Server();
			r = server->create(fastipc::genServerName(serverName), blockSize);
			if (r != 0)return r;
			server->setListener(new EXEReadListener());			// 设置侦听器
			thread startServer(__startIPCServer);
			startServer.detach();								// 在新线程中启动exe端的FastIPC监听
			client->write(JWM_IPC_CLIENT_OK, 0, NULL, NULL, 0);	// 通知dll端，子进程已创建，可以进行浏览器的初始化了
			thread waitProcess(__waitProcess, processId);		// 在新线程中启动等待远程进程结束的监听
			waitProcess.detach();
			return 0;
		}
		void closeWebTopEx(){
#ifdef JWebTopLog
			writeLog(L"closeWebTopEx............\r\n");
#endif
			isEx = false;
			if (server != NULL){
				if (server->isStable()){
					server->close();				// 关闭ipc侦听
				}
				delete server;
			}
			if (client != NULL){
				if (client->isStable()){
					client->close();				 // 关闭ipc侦听
				}
				delete client;
			}
			jw::ctx::CloseAllBrowsers(true);		// 关闭所有浏览器
		}
		void OnContextInitialized(){
			client->write(JWM_CEF_APP_INITED, 0, NULL, NULL, 0); // CEF浏览器已初始完成
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
			client->write(JWM_BROWSER_CREATED, browserHWnd, LPSTR(jw::w2s(taskId).c_str()), NULL, 0);
		}

	}// End dllex namespace
}// End jw namespac