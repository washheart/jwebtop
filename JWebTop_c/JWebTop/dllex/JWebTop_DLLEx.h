#ifndef CEF_JWEBTOP_EXE_DLLEX_H_
#define CEF_JWEBTOP_EXE_DLLEX_H_
#include <string>
using namespace std;
namespace jw{
	namespace dllex{// 和DLL进行交互的相关扩展方法
		bool ex();
		void sendBrowserCreatedMessage(wstring taskId,long browserHWnd);
		// 创建一个线程用来监听远程进程是否终止以便结束当前程序
		void waitForRemoteProcessTerminate();

		// 处理来自浏览器窗口的WM_COPYDATA消息
		LRESULT onBrowserWinWmCopyData(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		//LRESULT onWmCopyData(BrowserWindowInfo * bwInfo, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		//void thread_executeWmCopyData(HWND hWnd, DWORD msgId, std::wstring json);
	}
}
#endif