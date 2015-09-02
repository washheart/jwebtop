#ifndef jwebtop_multi_process_msgwin_H_
#define jwebtop_multi_process_msgwin_H_
#include <Windows.h>
namespace jw{
	// 创建窗口：窗口创建后会进行侦听，此时会阻塞进程，所以应该在独立线程中调用此方法
	int createWin(HINSTANCE hInstance, LPTSTR szCmdLine);

	// createWin创建窗口，生成窗口的句柄(HWND)后，会回调此方法
	void onWindowHwndCreated(HWND hWnd, LPTSTR szCmdLine);

	// 窗口解析WM_COPYDATA消息后，会在新的独立线程中调用此方法
	void msgwin_thread_executeWmCopyData(DWORD msgId, std::wstring msg, long senderHWnd, std::wstring taskId);
}
#endif