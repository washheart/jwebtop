#include <windows.h>
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

//#define FLAG_TASK_TIMER 100
using namespace std;
namespace jw{
	namespace dllex{
		// 
		typedef list<jw::task::ProcessMsgLock *> DefLockedTaskList;// 定义一个存储ProcessMsgLock的List
		typedef map<HWND, DefLockedTaskList * > DefLockedTaskMap;  // map<key,value>,key=浏览器句柄，value=浏览器页面周期内的任务锁
		typedef map<HWND, wstring> DefBrowserAppendJSMap;		   // map<key,value>,key=浏览器句柄，value=浏览器页面周期内的每次OnLoad后需要附加的JS
		DefLockedTaskMap browserLockedTasks;
		DefBrowserAppendJSMap browserAppendJS;
		DefLockedTaskList * getTaskList(HWND taskId){
			DefLockedTaskList * rtn = NULL;
			DefLockedTaskMap::iterator it = browserLockedTasks.find(taskId);
			if (browserLockedTasks.end() != it) {
				rtn = it->second;
			}
			return rtn;
		}
		// 记录下所有的任务锁
		void addLockedTask(HWND browserHwnd, jw::task::ProcessMsgLock * lock){
			DefLockedTaskList *  taskList = getTaskList(browserHwnd);
			if (taskList == NULL){
				taskList = new DefLockedTaskList();
				pair<DefLockedTaskMap::iterator, bool> v = browserLockedTasks.insert(pair<HWND, DefLockedTaskList* >(browserHwnd, taskList));// 记录下浏览器句柄与锁列表的关联
			}
			taskList->push_back(lock);
		}
		wstring getAppendJS(HWND taskId){
			DefBrowserAppendJSMap::iterator it = browserAppendJS.find(taskId);
			if (browserAppendJS.end() != it)return it->second;
			return wstring();
		}
		void addAppendJS(HWND browserHwnd, wstring js){
			browserAppendJS.insert(pair<HWND, wstring>(browserHwnd, js));// 记录下浏览器句柄与附加JS的关联			
		}
		void appendBrowserJS(HWND browserHwnd, const CefRefPtr<CefFrame> frame){
			wstring js=getAppendJS(browserHwnd);
#ifdef JWebTopLog
			writeLog(L"附加的JS====="); writeLog(js); writeLog(L"\r\n");
#endif
			if(!js.empty())frame->ExecuteJavaScript(js, "", 0);
		}
		void unlockBrowserLocks(HWND browserHwnd){
			DefLockedTaskList *  taskList = getTaskList(browserHwnd);
			if (taskList == NULL)return;
			DefLockedTaskList tmpTasks(taskList->begin(), taskList->end());
			taskList->clear();
			DefLockedTaskList::iterator it = tmpTasks.begin();
			for (; it != tmpTasks.end(); it++){
				(*it)->notify(L"");
			}
		}
		void removeBrowserSetting(HWND browserHwnd){
			browserAppendJS.erase(browserHwnd);
			DefLockedTaskList *  taskList = getTaskList(browserHwnd);
			if (taskList == NULL)return;
			unlockBrowserLocks(browserHwnd);
			browserLockedTasks.erase(browserHwnd);
		}

		// 添加任务
		void removeLockedTask(HWND browserHwnd, jw::task::ProcessMsgLock * lock){
			DefLockedTaskList *  taskList = getTaskList(browserHwnd);
			if (taskList == NULL)return;
			DefLockedTaskList::iterator it = find(taskList->begin(), taskList->end(), lock);
			if (it != taskList->end()){
				taskList->erase(it);
				// if(taskList->empty())browserLockedTasks.erase(browserHwnd);// 因为要重复利用，所以除非浏览器关闭，否则不清除
			}
		}

		bool isEx = false;

		fastipc::Server server;
		fastipc::Client client;

		bool ex(){ return isEx; }
		void setAsEx(){ isEx = true; }

		void closeWebTopEx();
		bool sendJWebTopProcessMsg(HWND browserHWnd, DWORD msgId, wstring& msg, wstring& taskId);
		//wstring getFlagTaskId(wstring  taskId){ return taskId + L"_Flag"; }

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
			case JWM_RESULT_RETURN:
				jw::task::putTaskResult(taskId, data); // 通知等待线程，远程任务已完成，结果已取回
				break;
			case JWM_JSON_EXECUTE_WAIT:// 远程进程发来一个任务，并且远程进程正在等待，任务完成后需要发送JWEBTOP_MSG_RESULT_RETURN消息给远程进程
			case JWM_JS_EXECUTE_WAIT:// 远程进程发来一个任务，并且远程进程正在等待，任务完成后需要发送JWEBTOP_MSG_RESULT_RETURN消息给远程进程
				try{
					BrowserWindowInfo * bwInfo = getBrowserWindowInfo((HWND)userValue);
					CefRefPtr<CefProcessMessage> cefMsg = CefProcessMessage::Create("waitjs");
					CefRefPtr<CefListValue> args = cefMsg->GetArgumentList();
					args->SetInt(0, userMsgType);
					args->SetString(1, data);
					args->SetString(2, taskId);
					wstring flagTaskId = taskId + L"_flag";
					//jw::task::ProcessMsgLock * flagTask = jw::task::addTask(flagTaskId);// 
					if (!bwInfo->browser->SendProcessMessage(PID_RENDERER, cefMsg)){// 直接发送到render进程去执行
#ifdef JWebTopLog
						writeLog(L"发送JS到render进程失败，taskId="); writeLog(taskId); writeLog(L"\r\n");
#endif
						wstring empty;
						sendJWebTopProcessMsg((HWND)userValue, JWM_RESULT_RETURN, empty, taskId); // 发送结果到远程进程
						//					}
						//					else{
						//						wstring  flagResult = flagTask->wait(FLAG_TASK_TIMER);// 等待FLAG_TASK_TIMER毫秒
						//						if (L"1" != flagResult){// 此任务没有被正常处理，正常情况下FLAG_TASK_TIMER毫秒足够render进程接收任务了
						//#ifdef JWebTopLog
						//							writeLog(L"发送到render进程的JS未正常处理，taskId="); writeLog(taskId); writeLog(L"\r\n");
						//#endif
						//							sendJWebTopProcessMsg((HWND)userValue, JWM_RESULT_RETURN, wstring(), taskId); // 发送结果到远程进程
						//						}
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
				addAppendJS((HWND)userValue, data);// 将JS保存下来，每次OnLoadEnd后执行
				break;
			case JWM_M_SETURL:{
				jw::dllex::unlockBrowserLocks((HWND)userValue);// 每次主页面重新加载之后，都解锁之前页面绑定的JS任务
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
			case JWM_SET_TASK_WAIT_TIME:
				jw::task::setDefaultTaskWiatTime(userValue);
				break;
			case JWM_CEF_ExitAPP:
				closeWebTopEx();
				break;
			default:
				MessageBox(NULL,L"用户定义消息值不对！",L"错误",0);
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
#ifdef JWebTopLog
			writeLog(L"closeWebTopEx............\r\n");
#endif
			isEx = false;						 // 标记为非ex模式，以便退出cef侦听
			if (client.isStable())client.close();// 关闭ipc侦听
			if (server.isStable())server.close();// 关闭ipc侦听
			jw::ctx::CloseAllBrowsers(true);	 // 关闭所有浏览器
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

		bool sendJWebTopProcessMsg(HWND browserHWnd, DWORD msgId, wstring& msg, wstring& taskId){
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
			BrowserWindowInfo * bw = getBrowserWindowInfo(browserHWnd);
			if(bw==NULL)writeLog(L"消息出现异常，sendJWebTopProcessMsg时bw==null,taskId"); writeLog(taskId); writeLog(L"\r\n");
#endif
			string tmp = jw::w2s(msg);
			return client.write(msgId, (long)browserHWnd, LPSTR(jw::w2s(taskId).c_str()), LPSTR(tmp.c_str()), tmp.size()) == 0;
		}

		void syncExecuteJS(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message){
			CefRefPtr<CefListValue> args = message->GetArgumentList();
			wstring taskInfo = args->GetString(1).ToWString();
			wstring remoteTaskId = args->GetString(2);
			//jw::task::putTaskResult(getFlagTaskId(remoteTaskId), L"1");// 通知等待任务，js已被正常接收
			//long remoteHWnd = args->GetInt(3);
			long browserHWnd = 0;
			CefString rtn;
			try{
				CefRefPtr<CefV8Context>  v8 = browser->GetMainFrame()->GetV8Context();
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
			}
			catch (...){}// 不管发生什么情况，都需要把taskId返回回去，否则会造成系统在那里死死的等待
			wstring result = rtn.ToWString();
			sendJWebTopProcessMsg((HWND)browserHWnd, JWM_RESULT_RETURN, result, remoteTaskId); // 发送结果到远程进程
		}

		CefString invokeRemote_Wait(HWND browserHWnd, CefString json){
			BrowserWindowInfo * bwInfo = getBrowserWindowInfo(browserHWnd);
			if (bwInfo == NULL)return CefString();
			wstring taskId = jw::task::createTaskId();			         // 生成任务id
			wstring newjson = json.ToWString();
			jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // 放置任务到任务池		
			if (sendJWebTopProcessMsg(browserHWnd, JWM_DLL_EXECUTE_WAIT, newjson, taskId)){ // 发送任务到远程进程
				addLockedTask(browserHWnd, lock);							// 记录当前浏览器已经创建的锁
				wstring result = lock->wait();   	 						// 等待任务完成并取回执行结果
				removeLockedTask(browserHWnd, lock);						// 移除当前浏览器已经解开的锁
				delete lock;
				return CefString(result);									// 返回数据
			}
			else{
				jw::task::removeTask(taskId);								// 消息发送失败移除现有消息
				return CefString();											// 返回数据：注意这里是空字符串
			}
		}

		void invokeRemote_NoWait(HWND browserHWnd, CefString json){
			wstring newjson = json.ToWString(), empty;
			sendJWebTopProcessMsg(browserHWnd, JWM_DLL_EXECUTE_RETURN, newjson, empty); // 发送任务到远程进程
		}
	}
}