#ifndef _Included_org_jwebtop_dll4csharp
#define _Included_org_jwebtop_dll4csharp
#include "common_dll/jwebtop_dll.h"
extern "C" {

	typedef char*(CALLBACK *CSharpCallBack)(long browserHWnd, char* s); // 定义一个回调函数，csharp端实现此回调函数
	EXPORT void WINAPI SetCSharpCallback(CSharpCallBack callBack);      // 设置回调函数
	EXPORT void WINAPI DelPtrInDLL(void *p);							// 用于（DLL调用者）删除dll中定义的指针
	
	// 以下方法包装jwebtop_dll对应方法
	EXPORT long  WINAPI nCreateJWebTop(LPTSTR processPath, LPTSTR configFile);
	EXPORT void  WINAPI nExitJWebTop();
	EXPORT long  WINAPI nCreateJWebTopBrowser(LPTSTR jWebTopConfigJSON);
	EXPORT void  WINAPI nCloseJWebTopBrowser(long browserHWnd);
	EXPORT char* WINAPI nJWebTopExecuteJSWait(long browserHWnd, LPTSTR script);
	EXPORT void  WINAPI nJWebTopExecuteJSNoWait(long browserHWnd, LPTSTR script);
	EXPORT char* WINAPI nJWebTopExecuteJSONWait(long browserHWnd, LPTSTR json);
	EXPORT void  WINAPI nJWebTopExecuteJSONNoWait(long browserHWnd, LPTSTR json);
}
#endif