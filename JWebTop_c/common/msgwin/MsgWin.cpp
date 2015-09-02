#include <Windows.h>
#include <string>
#include <sstream> 
#include <strsafe.h>
#include <thread>
#include "common/process/MultiProcess.h"
#include "common/util/StrUtil.h"
#include "common/JWebTopMsg.h"
#include "MsgWin.h"

using namespace std;
HWND g_LocalWinHWnd = NULL;  // 本地创建的消息窗口HWND
HWND g_RemoteWinHWnd = NULL;  // 远程进程的消息窗口HWND
namespace jw{
	// 处理WM_COPYDATA消息
	LRESULT onWmCopyData(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
		wstring msg, taskId;
		DWORD msgId = 0;
		long senderHWnd;
		jw::parseProcessMsg(lParam, ref(msgId), ref(msg), ref(senderHWnd), ref(taskId));
		if (msgId == WM_COPYDATA_EXIT){
			PostQuitMessage(0);
			return JWEBTOP_MSG_SUCCESS;
		}
		std::thread t(msgwin_thread_executeWmCopyData, msgId, msg, senderHWnd, taskId);// onWmCopyData是同步消息，为了防止另一进程的等待，这里在新线程中进行业务处理
		t.detach();// 从当前线程分离
		return JWEBTOP_MSG_SUCCESS;
	}

	// 窗口消息处理
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_COPYDATA:{
							 return	 onWmCopyData(hWnd, message, wParam, lParam);

		}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	// 创建窗口
	int createWin(HINSTANCE hInstance, LPTSTR szCmdLine){
		static TCHAR szAppName[] = TEXT("jwebtop_msgwin_cls");
		HWND hWnd;
		MSG msg;
		WNDCLASS wndclass;

		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = WndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInstance;
		// 加载 图标供程序使用 IDI-图示ID
		wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		// 加载 鼠标光标供程序使用  IDC-游标ID
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		//
		wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = szAppName;

		// 注册窗口类
		if (!RegisterClass(&wndclass)){
			MessageBox(NULL, TEXT("无法创建窗口，操作系统版本过低！"), szAppName, MB_ICONERROR);
			return 0;
		}

		hWnd = CreateWindow(szAppName, // window class name
			L"jwebtop_msgwin", // window caption
			WS_EX_TOOLWINDOW,// 没有边框，没有标题栏，仅有客户区的窗口
			0, 0, 0, 0,// 大小为0的窗口
			NULL, // parent window handle
			NULL, // window menu handle
			hInstance,//program instance handle
			NULL); // creation parameters
		g_LocalWinHWnd = hWnd;
		onWindowHwndCreated(hWnd, szCmdLine);
		ShowWindow(hWnd, SW_HIDE);
		UpdateWindow(hWnd);
		while (GetMessage(&msg, NULL, 0, 0)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return msg.wParam;
	}
}