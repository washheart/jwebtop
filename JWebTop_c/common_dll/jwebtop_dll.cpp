#include "jwebtop_dll.h"
#include <Shlwapi.h>
#include "common/task/Task.h"
#include "common/JWebTopMsg.h"
#include "common/util/StrUtil.h"
#include "common/winctrl/JWebTopWinCtrl.h"

#include "common/msgwin/MsgWin.h"
typedef BOOL(WINAPI *lpfnSetLayeredWindowAttributes)(HWND hWnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

extern HWND g_RemoteWinHWnd;
extern HWND g_LocalWinHWnd;
wstring processPath;

namespace jw{
	void msgwin_thread_executeWmCopyData(DWORD msgId, wstring json, long senderHWnd, std::wstring taskId){
		if (msgId == JWM_RESULT_RETURN){// 远程任务已完成，结果发回来了，需要通知本进程的等待线程去获取结果
			jw::task::putTaskResult(taskId, json);						   // 通知等待线程，远程任务已完成，结果已去取回
		}
		else{
			wstring result = invokeByBrowser(senderHWnd, json); 		         // 取回执行结果
			if (msgId == JWM_DLL_EXECUTE_WAIT){// 远程进程发来一个任务，并且远程进程正在等待，任务完成后需要发送JWEBTOP_MSG_RESULT_RETURN消息给远程进程
				sendProcessMsg((HWND)senderHWnd, JWM_RESULT_RETURN, result, (long)g_LocalWinHWnd, taskId); // 发送结果到远程进程
			}
		}
	}

	// 用于createWin进行回调
	void onWindowHwndCreated(HWND hWnd, LPTSTR szCmdLine){
		wstringstream cmd;
		cmd << L": ";// 标记为特殊命令
		cmd << (long)hWnd << " ";// 追加msgWin的HWND
		cmd << szCmdLine;// 其他命令行参数
		createSubProcess(LPTSTR(processPath.c_str()), LPTSTR(cmd.str().c_str()));
	}
}

EXPORT void thread_CreateJWebTop(std::wstring cmds)
{
	jw::createWin(::GetModuleHandle(NULL), LPTSTR(cmds.c_str()));// createWin会开启新的线程
}

// 创建JWebTop进程
EXPORT long CreateJWebTop(wstring jprocessPath, wstring cfgFile){
	if (g_RemoteWinHWnd != NULL)return (long)g_RemoteWinHWnd;
	processPath = jprocessPath;
	wstring taskId = jw::task::createTaskId();			         // 生成任务id	
	wstring cmds(taskId);                                        // 将任务id放到启动参数上 
	if (cfgFile[0] != L'\"')cfgFile = L"\"" + cfgFile + L"\"";   // 用双引号将路径包含起来
	cmds.append(L" ").append(cfgFile);	                         // 其他任务参数：configFile
	jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // 放置任务到任务池
	thread t(thread_CreateJWebTop, cmds); t.detach();			 // 在新线程中完成启动操作
	lock->wait();		             		 		             // 等待任务完成
	wstring result = lock->result;   		 		             // 取回执行结果
	long tmp = jw::parseLong(result);
	if (tmp != 0) g_RemoteWinHWnd = (HWND)tmp;
	return tmp;
}

/*
* 该方法用于创建一个浏览器窗口
* jWebTopConfigJSON 浏览器配置信息JSON
*/
EXPORT long CreateJWebTopBrowser(wstring jWebTopConfigJSON){
	wstring taskId = jw::task::createTaskId();			         // 生成任务id
	jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // 放置任务到任务池
	jw::sendProcessMsg(g_RemoteWinHWnd, JWM_CREATEBROWSER, jWebTopConfigJSON, (long)g_LocalWinHWnd, taskId);
	lock->wait();		             		 		             // 等待任务完成
	wstring result = lock->result;   		 		             // 取回执行结果
	return jw::parseLong(result);
}

/*
 * 该方法用于关闭一个浏览器窗口
 * browserHWnd  浏览器窗口句柄
 */
EXPORT void CloseJWebTopBrowser(long browserHWnd){
	wstringstream wss; wss << browserHWnd;
	jw::sendProcessMsg((HWND)browserHWnd, JWM_CLOSEBROWSER, wss.str(), (long)g_LocalWinHWnd, wstring());
}

LPTSTR exeRemoteAndWait(long browserHWnd, wstring msg, DWORD msgId){
	wstring taskId = jw::task::createTaskId();			         // 生成任务id
	jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // 放置任务到任务池
	if (jw::sendProcessMsg((HWND)browserHWnd, msgId, msg, (long)g_LocalWinHWnd, taskId)){ // 发送任务到远程进程
		lock->wait();		             		 		             // 等待任务完成
		return LPTSTR(lock->result.c_str());   		 		             // 取回执行结果
	}
	else{
		jw::task::removeTask(taskId);								// 消息发送失败移除现有消息
		return NULL;											// 返回数据：注意这里是空字符串
	}
}
EXPORT LPTSTR JWebTopExecuteJSWait(long browserHWnd, wstring script){
	return exeRemoteAndWait(browserHWnd, script, JWM_JS_EXECUTE_WAIT);
}

EXPORT LPTSTR JWebTopExecuteJSONWait(long browserHWnd, wstring json){
	return exeRemoteAndWait(browserHWnd, json, JWM_JSON_EXECUTE_WAIT);
}

void JWebTopExecuteJSNoWait(long browserHWnd, wstring script){
	jw::sendProcessMsg((HWND)browserHWnd, JWM_JS_EXECUTE_RETURN, script, (long)g_LocalWinHWnd, wstring());
}

// jni方法：执行脚本且不等待返回结果
void JWebTopExecuteJSONNoWait(long browserHWnd, wstring json){
	jw::sendProcessMsg((HWND)browserHWnd, JWM_JSON_EXECUTE_RETURN, json, (long)g_LocalWinHWnd, wstring());
}

// jni方法：退出JWebTop进程
void ExitJWebTop(){
	jw::sendProcessMsg(g_RemoteWinHWnd, WM_COPYDATA_EXIT, wstring(), (long)g_LocalWinHWnd, wstring());
}