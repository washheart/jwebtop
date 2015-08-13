#ifndef jwebtop_multi_process_jwebtop_msg_H_
#define jwebtop_multi_process_jwebtop_msg_H_
#include <string>

#include "include/cef_parser.h"
#include "common/process/MultiProcess.h"

#define JWEBTOP_MSG_EXECJS			MPMSG_USER+1    // JWebTop进程与DLL通信时的消息值：执行JS脚本
#define JWEBTOP_MSG_EXECJSON		MPMSG_USER+2    // JWebTop进程与DLL通信时的消息值：执行JSON
#define JWEBTOP_MSG_LOADURL			MPMSG_USER+11   // 定义MPMSG_MINI和MPMSG_LARGE中msgId的起始值

#define JWEBTOP_MSG_EXECUTE_WAIT	MPMSG_USER+201  // 需要执行并等待的任务 
#define JWEBTOP_MSG_EXECUTE_RETURN	MPMSG_USER+202  // 
#define JWEBTOP_MSG_RESULT_RETURN	MPMSG_USER+203  
namespace jw{
	// 从传入的JSON字符串(jsonString)中解析出任务id(taskId)和具体的JSON字符串(parsedJO)，已经回复消息的窗口句柄
	// jsonString的结构如下：{"id":"task id string","hWnd":123456,"jo":"parsed json string"}
	// hWnd、taskId和parsedJsonString为传入参数，值直接返回。解析成功后返回true，解析失败会返回false
	bool parseMessageJSON(std::wstring jsonString,long &hWnd, std::wstring &taskId, std::wstring &parsedJsonString);

	// 将传入的待接收消息的窗口句柄、任务id(taskId)和JSON字符串(someJsonString)包装为新的json字符串
	// 返回json的结构如下：{"id":"task id string","hWnd":123456,"jo":"some json string"}
	// taskId和someJsonString使用的是传入参数方式，目的只是减少性能开销，在方法中不应修改其值
	std::wstring wrapAsTaskJSON(long hWnd, std::wstring &taskId, std::wstring &someJsonString);
}
#endif