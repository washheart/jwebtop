#ifndef CEF_JWEBTOP_EXE_DLLEX_H_
#define CEF_JWEBTOP_EXE_DLLEX_H_
#include <string>
#include "include/wrapper/cef_message_router.h"
using namespace std;
namespace jw{
	namespace dllex{// 和DLL进行交互的相关扩展方法
		// 是否是dll扩展
		bool ex();
		
		// 发送浏览器已创建消息到客户端
		void sendBrowserCreatedMessage(wstring taskId,long browserHWnd);

		// 创建一个线程用来监听远程进程是否终止以便结束当前程序
		void waitForRemoteProcessTerminate();

		// 处理来自浏览器窗口的WM_COPYDATA消息
		LRESULT onBrowserWinWmCopyData(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		// js调用dll的方法，并等待返回结果
		CefString invokeRemote_Wait(HWND browserHWnd, CefString json);
		
		// js调用dll的方法，但不等待返回结果
		void invokeRemote_NoWait(HWND browserHWnd, CefString json);

		// dll按同步方式调用js，并将js的执行结果通过消息方式发送到dll端
		void syncExecuteJS(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message);
	}
}
#endif