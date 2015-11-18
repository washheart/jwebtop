#ifndef CEF_JWEBTOP_EXE_DLLEX_H_
#define CEF_JWEBTOP_EXE_DLLEX_H_
#include <string>
#include "include/cef_browser.h"
#define B2R_MSG_NAME L"jwebtop_b2r"
using namespace std;
namespace jw{
	namespace dllex{// 和DLL进行交互的相关扩展方法
		// 是否是dll扩展
		bool ex();
		void setAsEx();

		int startIPCServer(wstring &serverName,DWORD blockSize,DWORD processId);
		void closeIPCServer();
		void OnContextInitialized();

		// 发送浏览器已创建消息到客户端
		void sendBrowserCreatedMessage(wstring taskId,long browserHWnd);
		
		// js调用dll的方法，结果返回后，会执行响应的callback
		void invokeRemote_CallBack(HWND browserHWnd, CefString json, CefString callBackFunName);

		// 多进程时lock会阻塞整个render的前进，所以屏蔽
		//CefString invokeRemote_Wait(HWND browserHWnd, CefString json);
		
		// js调用dll的方法，但不等待返回结果
		void invokeRemote_NoWait(HWND browserHWnd, CefString json);

		// 在render端按同步方式调用js，并将js的执行结果通过消息方式发送到dll端
		void render_processMsg(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message);

		// 当多进程模式时发送FastIPCServerInfo到render进程，以便创建render端的IPCClient
		void sendIPCServerInfo(HWND browserHWnd);
	}
}
#endif