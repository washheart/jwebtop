#ifndef jwebtop_multi_process_H_
#define jwebtop_multi_process_H_

#include <Windows.h>
#include <string>

#define WM_COPYDATA_MINI  1      // 定义WM_COPYDATA消息中dwData的数值：表示是小数据
#define WM_COPYDATA_LARGE 2      // 定义WM_COPYDATA消息中dwData的数值：表示是大数据
#define WM_COPYDATA_EXIT  3      // 定义WM_COPYDATA消息中dwData的数值：表示是退出窗口的消息循环
#define MPMSG_USER        100    // 定义MPMSG_MINI和MPMSG_LARGE中msgId的起始值

#define TASKID_LEN        100	 // 任务ID的长度

#define LARGE_MSG_START   20000	 // 通过此值来区分是大数据还是小数据，大数据时msgId=dwData-LARGE_MSG_START
#define MPMSG_MINI_LEN    1024   // 定义小数据结构时字符串的长度
#define MPMSG_LARGE_LEN   65535  // 定义大数据结构时字符串的长度

using namespace std;
namespace jw{
	typedef	struct tagMPMSG_MINI
	{
		TCHAR msg[MPMSG_MINI_LEN];
		TCHAR taskId[TASKID_LEN];
	}MPMSG_MINI;

	typedef	struct tagMPMSG_LARGE
	{
		TCHAR msg[MPMSG_LARGE_LEN];
		TCHAR taskId[TASKID_LEN];
	}MPMSG_LARGE;

	// 创建新进程并返回进程中对应的线程id（可以通过PostThreadMessage发送通知类消息）
	DWORD createSubProcess(LPTSTR subProcess, LPTSTR szCmdLine);

	// 发送跨进程消息
	// receiverHWnd	:接收消息的窗口的句柄
	// msgId		:大于MPMSG_USER的数值
	// msg			:待发送的字符串消息
	// senderHWND	:发送消息的窗口
	// taskId		:如果是需等待任务，此值对应等待任务的ID
	bool sendProcessMsg(const HWND receiverHWnd, const DWORD msgId, const wstring msg, const long senderHWND, const wstring taskId);

	LRESULT parseProcessMsg(const  LPARAM lParam, DWORD &msgId, wstring &msg, std::wstring &taskId);
}
#endif