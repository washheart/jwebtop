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
extern CefSettings settings;                      // CEFȫ������
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


		void sendIPCServerInfo(HWND browserHWnd){
			if (settings.single_process)return;	// û�п���������render����
			if (!ex())return;					// ���Ǵ������̵���
			CefRefPtr<CefProcessMessage> tmp = CefProcessMessage::Create(B2R_MSG_NAME);
			CefRefPtr<CefListValue> args = tmp->GetArgumentList();
			args->SetInt(0, JWM_B2R_SERVERINFO);
			args->SetInt(1, g_blockSize);
			args->SetString(2, g_serverName);
			BrowserWindowInfo * bw = getBrowserWindowInfo((HWND)browserHWnd);
			bw->browser->SendProcessMessage(PID_RENDERER, tmp);
		}

		void __onRead(LONG userMsgType, LONG userValue, std::string& taskIds, std::string& datas){
			std::wstring taskId = jw::str::s2w(taskIds);
			std::wstring data = jw::str::s2w(datas);
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
			if (userValue != 0){// �������ǲ��������ҳ�桢ִ�л����JS��
				BrowserWindowInfo * bwInfo = getBrowserWindowInfo((HWND)userValue);
				if (bwInfo != NULL){
					switch (userMsgType){
					case JWM_RESULT_RETURN:
						if (settings.single_process){// ������ģʽ��ֱ��ִ��callback����
							executeJSCallBack(bwInfo->browser->GetMainFrame(), taskId, data);
						} else{
							CefRefPtr<CefProcessMessage> cefMsg = CefProcessMessage::Create(B2R_MSG_NAME);
							CefRefPtr<CefListValue> args = cefMsg->GetArgumentList();
							args->SetInt(0, JWM_B2R_TASKRESULT);
							args->SetString(1, taskId);
							args->SetString(2, data);
							//args->SetInt(3, userValue);
							bwInfo->browser->SendProcessMessage(PID_RENDERER, cefMsg);
						}
						break;
					case JWM_JSON_EXECUTE_WAIT:// Զ�̽��̷���һ�����񣬲���Զ�̽������ڵȴ���������ɺ���Ҫ����JWEBTOP_MSG_RESULT_RETURN��Ϣ��Զ�̽���
					case JWM_JS_EXECUTE_WAIT:// Զ�̽��̷���һ�����񣬲���Զ�̽������ڵȴ���������ɺ���Ҫ����JWEBTOP_MSG_RESULT_RETURN��Ϣ��Զ�̽���
						try{
							CefRefPtr<CefProcessMessage> cefMsg = CefProcessMessage::Create(B2R_MSG_NAME);
							CefRefPtr<CefListValue> args = cefMsg->GetArgumentList();
							args->SetInt(0, userMsgType);
							args->SetString(1, data);
							args->SetString(2, taskId);
							if (!bwInfo->browser->SendProcessMessage(PID_RENDERER, cefMsg)){// ֱ�ӷ��͵�render����ȥִ��
#ifdef JWebTopLog
								writeLog(L"����JS��render����ʧ�ܣ�taskId="); writeLog(taskId); writeLog(L"\r\n");
#endif
								wstring empty;
								sendJWebTopProcessMsg((HWND)userValue, JWM_RESULT_RETURN, empty, taskId); // ���ͽ����Զ�̽���
							}
						} catch (...){
#ifdef JWebTopLog
							writeLog(L"ִ��JS�����쳣��taskId="); writeLog(taskId); writeLog(L"\r\n");
#endif
							wstring empty;
							sendJWebTopProcessMsg((HWND)userValue, JWM_RESULT_RETURN, empty, taskId); // ���ͽ����Զ�̽���
						}
						break;
					case JWM_JSON_EXECUTE_RETURN:
						data = L"invokeByDLL(" + data + L")";// ��װjsonΪjs���� 
						bwInfo->browser->GetMainFrame()->ExecuteJavaScript(data, "", 0);
						break;
					case JWM_JS_EXECUTE_RETURN:
						bwInfo->browser->GetMainFrame()->ExecuteJavaScript(data, "", 0);
						break;
					case JWM_M_APPEND_JS:
						DLLExState::findOrCreateExState((HWND)userValue)->setAppendJS(data);// ��JS����������ÿ��OnLoadEnd��ִ��
						break;
					case JWM_M_SETURL:
						DLLExState::unlockBrowserLocks((HWND)userValue);// ÿ����ҳ�����¼���֮�󣬶�����֮ǰҳ��󶨵�JS����
						bwInfo->browser->GetMainFrame()->LoadURL(data);
						break;
					case JWM_CLOSEBROWSER:
						jb::close((HWND)userValue);
						break;
					default:
#ifdef JWebTopLog
						writeLog(L"������__OnRead �ҵ�������������޷������Ӧ��Ϣֵ��\r\n");
#endif
						break;
					}
				} else{
#ifdef JWebTopLog
					writeLog(L"������__OnRead �޷�����ָ�������������ҵ���Ӧ���������\r\n");
#endif
				}
			} else{
				switch (userMsgType){
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
				case JWM_CEF_ExitAPP:
					closeWebTopEx();
					break;
				default:
#ifdef JWebTopLog
					writeLog(L"������__OnRead �û�������Ϣֵ���ԡ�\r\n");
#endif
					break;
				}
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
					<< L" userShortStr=" << jw::str::s2w(memBlock->getUserShortStr())
					<< L" pBuff=" << jw::str::s2w(memBlock->getData())
					<< L"||\r\n";
				writeLog(wss.str());
#endif
				thread t(__onRead, memBlock->userMsgType, memBlock->userValue, memBlock->getUserShortStr(), memBlock->getData());
				t.detach();// ��һ�������߳���������������������������ݵ��շ�
			}
		};

		void __startIPCServer(){
			server->startRead();
		}
		void __waitProcess(DWORD processId){
			// �ȴ�Զ�̽��̽���
			try{
				HANDLE hHandle = OpenProcess(SYNCHRONIZE, false, processId);// ����Ȩ�ޣ�������Щ�����OpendProcessʧ�ܣ�����xp��
				WaitForSingleObject(hHandle, INFINITE);// �ȴ�Զ�̽��̽���
				closeWebTopEx();
			} catch (...){
#ifdef JWebTopLog
				writeLog("�ȴ�Զ�̽��̽���������������\r\n");
#endif
			}
		}
		int startIPCServer(wstring &serverName, DWORD blockSize, DWORD processId){
			client = new fastipc::Client();
			int r = client->create(serverName, blockSize);
			if (!settings.single_process){// ����Ƕ����ģʽ��������������Ϣ��¼�������ڴ棬�Ա�render���̻�ȡ�������Ϣ
				g_serverName = serverName;
				g_blockSize = blockSize;
			}
			if (r != 0)return r;
			server = new fastipc::Server();
			r = server->create(fastipc::genServerName(serverName), blockSize);
			if (r != 0)return r;
			server->setListener(new EXEReadListener());			// ����������
			thread startServer(__startIPCServer);
			startServer.detach();								// �����߳�������exe�˵�FastIPC����
			client->write(JWM_IPC_CLIENT_OK, 0, NULL, NULL, 0);	// ֪ͨdll�ˣ��ӽ����Ѵ��������Խ���������ĳ�ʼ����
			thread waitProcess(__waitProcess, processId);		// �����߳��������ȴ�Զ�̽��̽����ļ���
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
					server->close();				// �ر�ipc����
				}
				delete server;
			}
			if (client != NULL){
				if (client->isStable()){
					client->close();				 // �ر�ipc����
				}
				delete client;
			}
			jw::ctx::CloseAllBrowsers(true);		// �ر����������
		}
		void OnContextInitialized(){
			client->write(JWM_CEF_APP_INITED, 0, NULL, NULL, 0); // CEF������ѳ�ʼ���
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
			client->write(JWM_BROWSER_CREATED, browserHWnd, LPSTR(jw::str::w2s(taskId).c_str()), NULL, 0);
		}

	}// End dllex namespace
}// End jw namespac