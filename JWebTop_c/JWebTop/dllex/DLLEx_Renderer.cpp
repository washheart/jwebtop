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

using namespace std;

namespace jw{
	namespace dllex{
		extern fastipc::Client * client;
		fastipc::Client * client_render;

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
			if (bw == NULL)writeLog(L"��Ϣ�����쳣��sendJWebTopProcessMsgʱbw==null,taskId"); writeLog(taskId); writeLog(L"\r\n");
#endif
			string tmp = jw::str::w2s(msg);
			if (client == NULL){// ����Ƕ����ģʽ����ôrender�����Լ��Ľ����У�������������Ϣ��¼�������ڴ棬�Ա�render���̻�ȡ�������Ϣ
				static int step = 100;
				while (client_render == NULL){// �ȴ�client_render�����ɹ�				
					Sleep(step);
					step += step;
				}
				return client_render->write(msgId, (long)browserHWnd, LPSTR(jw::str::w2s(taskId).c_str()), LPSTR(tmp.c_str()), tmp.size()) == 0;
			} else{
				return client->write(msgId, (long)browserHWnd, LPSTR(jw::str::w2s(taskId).c_str()), LPSTR(tmp.c_str()), tmp.size()) == 0;
			}
		}

		void executeJSCallBack(CefRefPtr<CefFrame> frame, wstring callback, wstring result){
			//wstring code = L"try{" + callback + L"(" + result + L")" + L"}catch(e){alert(e)}";
			wstring code = callback + L"(" + result + L");";
			frame->ExecuteJavaScript(code, "", 0);
		}
		void render_processMsg(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message){
			CefRefPtr<CefListValue> args = message->GetArgumentList();
			DWORD userMsgType = args->GetInt(0);
			if (userMsgType == JWM_B2R_TASKRESULT){
				wstring callBack = args->GetString(1).ToWString();
				wstring result = args->GetString(2).ToWString();
				if (callBack.empty()){// �����ˣ�����
				} else{
					executeJSCallBack(browser->GetMainFrame(), callBack, result);
				}
			}
			else if (userMsgType == JWM_B2R_SERVERINFO){
				DWORD blockSize = args->GetInt(1);
				wstring serverName = args->GetString(2).ToWString();
				client_render = new fastipc::Client();
				client_render->create(serverName, blockSize);
			}
			else{
				wstring taskInfo = args->GetString(1).ToWString();
				wstring remoteTaskId = args->GetString(2);
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
								taskInfo = L"invokeByDLL(" + taskInfo + L")";// ��װjsonΪjs���� 
							}
							if (v8->Eval(taskInfo, reval, exception)){// ִ��JS
								rtn = reval->GetStringValue();
							}
							CefRefPtr<CefV8Value> global = v8->GetGlobal();
							CefRefPtr<CefV8Value> jwebtop = global->GetValue(CefString("JWebTop"));
							CefRefPtr<CefV8Value> handler = jwebtop->GetValue("handler");
							browserHWnd = handler->GetIntValue();// ��JWebTop.handler��ȡ���ھ��
							v8->Exit();
						}
					}
				}
				catch (...){}// ���ܷ���ʲô���������Ҫ��taskId���ػ�ȥ����������ϵͳ�����������ĵȴ�
				wstring result = rtn.ToWString();
				sendJWebTopProcessMsg((HWND)browserHWnd, JWM_RESULT_RETURN, result, remoteTaskId); // ���ͽ����Զ�̽���
			}
		}
		void invokeRemote_CallBack(HWND browserHWnd, CefString json, CefString callBackFunName){
			wstring newjson = json.ToWString();
			wstring callBack = callBackFunName.ToWString();
			sendJWebTopProcessMsg(browserHWnd, JWM_DLL_EXECUTE_WAIT, newjson, callBack);
		}

		void invokeRemote_NoWait(HWND browserHWnd, CefString json){
			wstring newjson = json.ToWString(), empty;
			sendJWebTopProcessMsg(browserHWnd, JWM_DLL_EXECUTE_RETURN, newjson, empty); // ��������Զ�̽���
		}
	}// End dllex namespace
}// End jw namespace