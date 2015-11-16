#ifndef CEF_JWEBTOP_EXE_DLLEX_H_
#define CEF_JWEBTOP_EXE_DLLEX_H_
#include <string>
#include "include/wrapper/cef_message_router.h"
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
		
		// js调用dll的方法，并等待返回结果
		CefString invokeRemote_Wait(HWND browserHWnd, CefString json);
		
		// js调用dll的方法，但不等待返回结果
		void invokeRemote_NoWait(HWND browserHWnd, CefString json);

		// dll按同步方式调用js，并将js的执行结果通过消息方式发送到dll端
		void syncExecuteJS(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message);
	
		// 每次页面OnLoad后附加一些JS
		void appendBrowserJS(HWND browserHwndconst ,const CefRefPtr<CefFrame> frame);

		// 浏览器关闭时清除所有等待的锁，此方法会先调用unlockBrowserLocks解锁然后再清除
		void removeBrowserSetting(HWND browserHwnd);
	}
}
#endif