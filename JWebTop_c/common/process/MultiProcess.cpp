#include "MultiProcess.h"

#include <Windows.h>
#include <string>
#include <strsafe.h>

// 创建一个新进程，返回的数据为进程中主线程的id
DWORD createSubProcess(LPTSTR subProcess, LPTSTR szCmdLine){
	STARTUPINFO sui;
	ZeroMemory(&sui, sizeof(STARTUPINFO)); // 对一个内存区清零，最好用ZeroMemory, 它的速度要快于memset
	sui.cb = sizeof(STARTUPINFO);
	sui.dwFlags = STARTF_USESTDHANDLES;

	PROCESS_INFORMATION pi; // 保存了所创建子进程的信息
	std::wstring cmd = L" "; cmd += szCmdLine;// 追加一个空格在命令行参数的前面，否则新进程的szCmdLine会取不到参数
	if (CreateProcess(subProcess, LPTSTR(cmd.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &sui, &pi))
	{
		CloseHandle(pi.hProcess); // 子进程的进程句柄
		CloseHandle(pi.hThread);  // 子进程的线程句柄，windows中进程就是一个线程的容器，每个进程至少有一个线程在执行
		return pi.dwThreadId;
	}
	//else{// else的处理主要是为了方便在VS环境下调试程序，因为VS下启动的程序默认的工作目录是$(ProjectDir)，如果改为$(SolutionDir)$(Configuration)\则不需要else的处理
	//	TCHAR   szPath[1000];
	//	GetModuleFileName(NULL, szPath, MAX_PATH);
	//	std::wstring path(szPath);
	//	path = path.substr(0, path.find_last_of('\\') + 1);
	//	path += subProcess;
	//	if (CreateProcess(path.c_str(), LPTSTR(cmd.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &sui, &pi))
	//	{
	//		CloseHandle(pi.hProcess); // 子进程的进程句柄
	//		CloseHandle(pi.hThread);  // 子进程的线程句柄，windows中进程就是一个线程的容器，每个进程至少有一个线程在执行
	//		return pi.dwThreadId;
	//	}
	//}
	return 0;
}

// 想指定窗口发送WM_COPYDATA消息。
// WM_COPYDATA可以跨进程发送，不过此方法是同步方法，对于耗时任务接收到消息的进程应开启新线程处理。
// 在JWebTop中对于接收的WM_COPYDATA消息都是开启新线程处理
BOOL sendProcessMsg(HWND hWnd, DWORD msgId, LPTSTR msg){
	COPYDATASTRUCT copyData;
	int len = lstrlen(msg);
	if (len > MPMSG_LARGE_LEN)return false;
	if (len > MPMSG_MINI_LEN){
		MPMSG_LARGE mpMsg;
		copyData.dwData = WM_COPYDATA_LARGE;         // 表示是大数据
		copyData.cbData = sizeof(mpMsg);             // 待发送的数据结构的大小
		copyData.lpData = &mpMsg;                    // 待发送的数据结构
		mpMsg.msgId = msgId;
		StringCbCopy(mpMsg.msg, sizeof(mpMsg.msg), msg);
		return ::SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&copyData);
	}
	else{
		MPMSG_MINI mpMsg;
		copyData.dwData = WM_COPYDATA_MINI;          // 表示是小数据
		copyData.cbData = sizeof(mpMsg);             // 待发送的数据结构的大小
		copyData.lpData = &mpMsg;                    // 待发送的数据结构
		mpMsg.msgId = msgId;
		if (msgId == WM_COPYDATA_HWND)copyData.dwData = WM_COPYDATA_HWND;
		StringCbCopy(mpMsg.msg, sizeof(mpMsg.msg), msg);
		return ::SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&copyData);
	}
}