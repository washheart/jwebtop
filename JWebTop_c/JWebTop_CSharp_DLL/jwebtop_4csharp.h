#ifndef _Included_org_jwebtop_dll4csharp
#define _Included_org_jwebtop_dll4csharp
#include "common_dll/jwebtop_dll.h"
//ÉùÃ÷
typedef LPTSTR(CALLBACK *CSharpCallBack)(long browserHWnd, LPTSTR json);
extern "C" {
	//EXPORT LPTSTR WINAPI CovWString(LPTSTR v);
	//EXPORT int WINAPI CovWString2(LPTSTR v);
	//EXPORT int WINAPI CovWString3(LPTSTR v1,LPTSTR v2);
	//EXPORT char* WINAPI CovWString4(LPTSTR v);
	//EXPORT void WINAPI CovWString5(LPTSTR, LPTSTR);
	//EXPORT int WINAPI CovString(char* v);
	//EXPORT char* WINAPI CovString2(char* v);
	//EXPORT int WINAPI Calc(int v);

	EXPORT void WINAPI DelPtrInDLL(void *p);
	EXPORT void WINAPI SetCSharpCallback(CSharpCallBack callBack);
	EXPORT long WINAPI nCreateJWebTop(LPTSTR processPath, LPTSTR configFile);
	EXPORT void WINAPI nExitJWebTop();
	EXPORT long WINAPI nCreateJWebTopBrowser(LPTSTR jWebTopConfigJSON);
	EXPORT void WINAPI nCloseJWebTopBrowser(long browserHWnd);
	EXPORT char* WINAPI nJWebTopExecuteJSWait(long browserHWnd, LPTSTR script);
	EXPORT void WINAPI nJWebTopExecuteJSNoWait(long browserHWnd, LPTSTR script);
	EXPORT char* WINAPI nJWebTopExecuteJSONWait(long browserHWnd, LPTSTR json);
	EXPORT void WINAPI nJWebTopExecuteJSONNoWait(long browserHWnd, LPTSTR json);
}
#endif