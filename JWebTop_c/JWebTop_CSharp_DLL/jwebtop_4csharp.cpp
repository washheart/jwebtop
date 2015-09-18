#include "jwebtop_4csharp.h"
#include "common/util/StrUtil.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
using namespace std;
CSharpCallBack g_CSharpCallBack;  //回复信息的回调函数

// 把wstring转为char*返回给csharp端
char* wstring2char(wstring ws){
	string s = jw::w2s(ws);
	char* c;
	const int len = s.length();
	c = new char[len + 1];
	strcpy(c, s.c_str());
	return c;
}

/************************************************************************/
/*设置回调函数
/************************************************************************/
EXPORT void WINAPI SetCSharpCallback(CSharpCallBack callBack)
{
	g_CSharpCallBack = callBack;
}

// 回调c#程序
std::wstring invokeByBrowser(long browserHWnd, wstring json){
	//writeLog(L"CSharpCallBack params="); writeLog(json); writeLog(L"\r\n");
	LPSTR param = wstring2char(json);
	//writeLog(L"               params="); writeLog(param); writeLog(L"\r\n");
	LPSTR rtns = g_CSharpCallBack(browserHWnd, param);
	delete param;
	//writeLog(L"CSharpCallBack rtns ="); writeLog(rtns); writeLog(L"\r\n");
	wstring rntws = jw::s2w(rtns);
	//writeLog(L"               rtns ="); writeLog(rntws); writeLog(L"\r\n");
	return rntws;
}


EXPORT void WINAPI DelPtrInDLL(void *p){
	//writeLog(L"准备删除指针\r\n");
	delete p;
	//writeLog(L"指针已经删除\r\n");
}
EXPORT long WINAPI nCreateJWebTop(LPTSTR processPath, LPTSTR configFile){
	//writeLog(L"processPath="); writeLog(processPath); writeLog(L"\r\n");
	//writeLog(L"configFile ="); writeLog(configFile); writeLog(L"\r\n");
	return CreateJWebTop(processPath, configFile);
}
EXPORT void WINAPI nExitJWebTop(){
	ExitJWebTop();
}
EXPORT long WINAPI nCreateJWebTopBrowser(LPTSTR jWebTopConfigJSON){
	return CreateJWebTopBrowser(jWebTopConfigJSON);
}
EXPORT void WINAPI nCloseJWebTopBrowser(long browserHWnd){
	CloseJWebTopBrowser(browserHWnd);
}
EXPORT char* WINAPI nJWebTopExecuteJSWait(long browserHWnd, LPTSTR script){
	wstring v = JWebTopExecuteJSWait(browserHWnd, script);
	return wstring2char(v);
}
EXPORT void WINAPI nJWebTopExecuteJSNoWait(long browserHWnd, LPTSTR script){
	JWebTopExecuteJSNoWait(browserHWnd, script);
}
EXPORT char* WINAPI nJWebTopExecuteJSONWait(long browserHWnd, LPTSTR json){
	wstring v = JWebTopExecuteJSONWait(browserHWnd, json);
	return wstring2char(v);
}
EXPORT void WINAPI nJWebTopExecuteJSONNoWait(long browserHWnd, LPTSTR json){
	JWebTopExecuteJSONNoWait(browserHWnd, json);
}