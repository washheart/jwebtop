#include <Windows.h>
#include <string>
#include <sstream> 
#include <strsafe.h>

#include "MsgWin.h"

#define BTN_MAX 101
using namespace std;

// 窗口消息处理
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COPYDATA:{
		return	 onWmCopyData(hwnd, message, wParam, lParam);

	}
	case   WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

// 创建窗口
int createWin(HINSTANCE hInstance, LPTSTR szCmdLine){
	static TCHAR szAppName[] = TEXT("jwebtop_msgwin_cls");
	HWND hwnd;
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
		MessageBox(NULL, TEXT("无法创建窗口，操作系统版本过低！"),
			szAppName, MB_ICONERROR);
		return 0;
	}
	
	hwnd = CreateWindow(szAppName, // window class name
		L"jwebtop_msgwin", // window caption
		WS_EX_TOOLWINDOW,// 没有边框，没有标题栏，仅有客户区的窗口
		0, 0, 0, 0,// 大小为0的窗口
		NULL, // parent window handle
		NULL, // window menu handle
		hInstance,//program instance handle
		NULL); // creation parameters

	onWindowHwndCreated(hwnd,szCmdLine);

	int  margin = 10, x = margin;
	CreateWindow(TEXT("button"),//必须为：button    
		TEXT("发送跨进程消息"),//按钮上显示的字符    
		WS_CHILD | WS_VISIBLE, x, 10, 120, 25, hwnd,
		(HMENU)BTN_MAX, hInstance, NULL);

	ShowWindow(hwnd, SW_HIDE);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

