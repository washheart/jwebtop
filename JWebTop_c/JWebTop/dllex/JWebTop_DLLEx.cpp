#include <windows.h>
#include <thread>
#include "JWebTop_DLLEx.h"
#include "common/util/StrUtil.h"
#include "JWebTop/config/JWebTopConfigs.h"
#include "common/JWebTopMsg.h"
#include "common/task/Task.h"
#include "common/msgwin/MsgWin.h"
#include "JWebTop/browser/JWebTopClient.h"

extern HWND g_LocalWinHWnd;  // 本地创建的消息窗口HWND
extern HWND g_RemoteWinHWnd;  // 远程进程的消息窗口HWND

namespace jw{
	namespace dllex{
		void waitRemoteProcessTerminate(){
			try{
				DWORD lpdwProcessId;
				GetWindowThreadProcessId(g_RemoteWinHWnd, &lpdwProcessId);
				//HANDLE hHandle = GetProcessHandleFromHwnd(g_RemoteWinHWnd);// 此函数的头文件不确定是哪个(Header: Use LoadLibrary and GetProcAddress.  Library: Use Oleacc.dll.)
				HANDLE hHandle = OpenProcess(PROCESS_ALL_ACCESS, false, lpdwProcessId);
				WaitForSingleObject(hHandle, INFINITE);
			}
			catch (...){
			}
			jw::sendProcessMsg(g_LocalWinHWnd, WM_COPYDATA_EXIT, L"");// 通知本进程主窗口，程序需要关闭
		}
		// 创建一个线程用来监听远程进程是否终止以便结束当前程序
		void waitForRemoteProcessTerminate(){
			std::thread waitRemoteProcessThread(waitRemoteProcessTerminate);
			waitRemoteProcessThread.detach();// 在独立线程中等待远程进程的结束消息
		}
	}
	// 处理消息窗口的WM_COPYDATA消息
	void msgwin_thread_executeWmCopyData(DWORD msgId, std::wstring msg){
		// 暂时不需要做任何处理
	}
	// 用于createWin进行回调
	void onWindowHwndCreated(HWND hWnd, LPTSTR szCmdLine){
		// 暂时不需要做任何处理
	}

}