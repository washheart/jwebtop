#include "MultiProcess.h"

#include <Windows.h>
#include <string>
#include <sstream>
#include <strsafe.h>
#include "common/util/StrUtil.h"
#include "common/JWebTopMsg.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
using namespace std;
namespace jw{
	// 创建一个新进程，返回的数据为进程中主线程的id
	DWORD createSubProcess(LPTSTR subProcess, LPTSTR szCmdLine){
		std::wstring cmd;
		if (subProcess[0] == L'"'){// 如果url中已以双引号开头，那么这里就不再添加双引号
			cmd.append(subProcess);
		}
		else{// 把子进程用双引号包含起来，避免恶意程序问题
			cmd.append(L"\"");
			cmd.append(subProcess);
			cmd.append(L"\"");
		}
		cmd += L" "; cmd += szCmdLine;// 追加一个空格在命令行参数的前面，否则新进程的szCmdLine会取不到参数

		STARTUPINFO sui;
		ZeroMemory(&sui, sizeof(STARTUPINFO)); // 对一个内存区清零，最好用ZeroMemory, 它的速度要快于memset
		sui.cb = sizeof(STARTUPINFO);

		PROCESS_INFORMATION pi; // 保存了所创建子进程的信息
		//DWORD dwCreationFlags = DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP;
		DWORD dwCreationFlags = CREATE_NEW_PROCESS_GROUP;
		if (CreateProcess(NULL, LPTSTR(cmd.c_str()), // 第一个参数置空，可执行文件和命令行放到一起避免恶意程序问题
			NULL, NULL, FALSE,//句柄不继承 
			dwCreationFlags, //使用正常优先级 
			NULL, //使用父进程的环境变量 
			NULL, //指定工作目录 
			&sui, &pi))
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

	// 向指定窗口发送WM_COPYDATA消息。
	// WM_COPYDATA可以跨进程发送，不过此方法是同步方法，对于耗时任务接收到消息的进程应开启新线程处理。
	// 在JWebTop中对于接收的WM_COPYDATA消息都是开启新线程处理
	bool sendProcessMsg(const HWND receiverHWnd, const DWORD msgId, const wstring msg, const  long senderHWND, const wstring taskId){
#ifdef JWebTopLog
		wstringstream wss;
		wss<<L"发送WM_COPYDATA消息，msgId=" << msgId << L"，msg=" << msg
			<< ",senderHWnd=" << senderHWND << ",taskId=" << taskId << L"\r\n";
		writeLog(wss.str());
#endif
		COPYDATASTRUCT copyData;
		int len = msg.length();
		if (len > MPMSG_LARGE_LEN) return false;         // 太大了（超过65535）不给发了		
		if (len > MPMSG_MINI_LEN){
			MPMSG_LARGE mpMsg;
			StringCbCopy(mpMsg.msg, sizeof(mpMsg.msg), LPTSTR(msg.c_str()));
			StringCbCopy(mpMsg.taskId, sizeof(mpMsg.taskId), LPTSTR(taskId.c_str()));
			copyData.dwData = msgId + LARGE_MSG_START;
			copyData.cbData = sizeof(mpMsg);             // 待发送的数据结构的大小
			copyData.lpData = &mpMsg;                    // 待发送的数据结构
		}
		else{
			MPMSG_MINI  mpMsg;
			StringCbCopy(mpMsg.msg, sizeof(mpMsg.msg), LPTSTR(msg.c_str()));
			StringCbCopy(mpMsg.taskId, sizeof(mpMsg.taskId), LPTSTR(taskId.c_str()));
			copyData.dwData = msgId;
			copyData.cbData = sizeof(mpMsg);             // 待发送的数据结构的大小
			copyData.lpData = &mpMsg;                    // 待发送的数据结构
		}
		return ::SendMessage(receiverHWnd, WM_COPYDATA, senderHWND, (LPARAM)(LPVOID)&copyData) == JWEBTOP_MSG_SUCCESS;
	}

	// 解析WM_COPYDATA消息
	LRESULT parseProcessMsg(const LPARAM lParam, DWORD &msgId, wstring &msg, std::wstring &taskId){
		COPYDATASTRUCT* copyData = (COPYDATASTRUCT*)lParam;
		if (copyData->dwData >LARGE_MSG_START){
			MPMSG_LARGE * mpMsg = ((MPMSG_LARGE *)(copyData->lpData));
			msg = wstring(mpMsg->msg);
			taskId = wstring(mpMsg->taskId);
			msgId = copyData->dwData - LARGE_MSG_START;
		}
		else{
			MPMSG_MINI * mpMsg = ((MPMSG_MINI *)(copyData->lpData));
			msg = wstring(mpMsg->msg);
			taskId = wstring(mpMsg->taskId);
			msgId = copyData->dwData ;
		}
#ifdef JWebTopLog
		wstringstream wss;
		wss << L"接收WM_COPYDATA消息，msgId=" << msgId << L"，msg=" << msg 
			<< ",senderHWnd=" << senderHWnd << ",taskId=" << taskId << L"\r\n";
		writeLog(wss.str());
#endif
		return JWEBTOP_MSG_SUCCESS;
	}
}