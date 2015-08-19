#ifndef jwebtop_multi_process_jwebtop_msg_H_
#define jwebtop_multi_process_jwebtop_msg_H_
#include <string>

#include "include/cef_parser.h"
#include "common/process/MultiProcess.h"

#define JWEBTOP_MSG_SUCCESS         (WM_USER+321)	    // 表示消息成功被接收进程收到


// JWM=JWEBTOP_MSG
#define JWM_JS_EXECUTE_WAIT		(MPMSG_USER+201)  // DLL调用CEF端：需要执行并等待的JS任务 
#define JWM_JS_EXECUTE_RETURN	(MPMSG_USER+202)  // 
#define JWM_JSON_EXECUTE_WAIT	(MPMSG_USER+211)  // DLL调用CEF端：需要执行并等待的JSON任务 
#define JWM_JSON_EXECUTE_RETURN	(MPMSG_USER+212)  // 
#define JWM_DLL_EXECUTE_WAIT	(MPMSG_USER+221)  // CEF调用DLL端：需要执行并等待的任务 
#define JWM_DLL_EXECUTE_RETURN	(MPMSG_USER+222)  // CEF调用DLL端：需要执行但CEF无需等待的任务 
#define JWM_RESULT_RETURN	    (MPMSG_USER+231)  
#define JWM_STARTJWEBTOP		(MPMSG_USER+301)  // 启动JWebTop进程的消息
#define JWM_CREATEBROWSER		(MPMSG_USER+302)  // 创建浏览器的消息
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