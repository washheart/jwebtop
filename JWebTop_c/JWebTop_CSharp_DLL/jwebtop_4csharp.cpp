#include "jwebtop_4csharp.h"

CSharpCallBack g_CSharpCallBack;  //回复信息的回调函数

/************************************************************************/
/*设置回调函数
/************************************************************************/
EXPORT void WINAPI SetCSharpCallback(CSharpCallBack callBack)
{
	g_CSharpCallBack = callBack;
}

// 回调c#程序
std::wstring invokeByBrowser(long browserHWnd, wstring json){
	return g_CSharpCallBack(browserHWnd, LPTSTR(json.c_str()));
}