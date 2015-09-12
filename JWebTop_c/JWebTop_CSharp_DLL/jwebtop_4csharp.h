#ifndef _Included_org_jwebtop_dll4csharp
#define _Included_org_jwebtop_dll4csharp
#include "common_dll/jwebtop_dll.h"
//ÉùÃ÷
typedef LPTSTR(CALLBACK *CSharpCallBack)(long browserHWnd, LPTSTR json);
extern "C" {
	EXPORT void WINAPI SetCSharpCallback(CSharpCallBack callBack);
}
#endif