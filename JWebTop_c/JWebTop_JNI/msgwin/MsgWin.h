#ifndef jwebtop_multi_process_msgwin_H_
#define jwebtop_multi_process_msgwin_H_
#include <Windows.h>

// 创建窗口
int createWin(HINSTANCE hInstance, LPTSTR szCmdLine);

// 用于createWin进行回调
void onWindowHwndCreated(HWND hWnd, LPTSTR szCmdLine);

// 处理WM_COPYDATA消息
LRESULT onWmCopyData(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif