#ifndef _Included_org_jwebtop_dll4csharp
#define _Included_org_jwebtop_dll4csharp
#include "common_dll/jwebtop_dll.h"
//ÉùÃ÷
typedef LPTSTR(CALLBACK *CSharpCallBack)(long browserHWnd, LPTSTR json);
extern "C" {
	EXPORT void WINAPI SetCSharpCallback(CSharpCallBack callBack);
	EXPORT LPTSTR WINAPI CovWString(LPTSTR v);
	EXPORT int WINAPI CovWString2(LPTSTR v);
	EXPORT int WINAPI CovWString3(LPTSTR v);
	EXPORT char* WINAPI CovWString4(LPTSTR v);
	EXPORT int WINAPI CovString(char* v);
	EXPORT char* WINAPI CovString2(char* v);
	EXPORT int WINAPI Calc(int v);
}
#endif