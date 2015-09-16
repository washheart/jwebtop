#include "jwebtop_4csharp.h"
#include "common/util/StrUtil.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
using namespace std;
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

// 把wstring转为char*返回给csharp端
char* wstring2char(wstring ws){
	string s = jw::w2s(ws);
	char* c;
	const int len = s.length();
	c = new char[len + 1];
	strcpy(c, s.c_str());
	return c;
}

EXPORT void WINAPI DelPtrInDLL(void *p){
	//writeLog(L"准备删除指针\r\n");
	delete p;
	//writeLog(L"指针已经删除\r\n");
}
EXPORT long WINAPI nCreateJWebTop(LPTSTR processPath, LPTSTR configFile){
	writeLog(L"processPath="); writeLog(processPath); writeLog(L"\r\n");
	writeLog(L"configFile ="); writeLog(configFile); writeLog(L"\r\n");
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
	wstring v=JWebTopExecuteJSWait(browserHWnd, script);
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

//char* string2char(string s){
//	char* c;
//	const int len = s.length();
//	c = new char[len + 1];
//	strcpy(c, s.c_str());
//	return c;
//}
//EXPORT LPTSTR WINAPI CovWString(LPTSTR v){
//	wstring ws(v);
//	ws.append(L"___在DLL中处理过了");
//	return LPTSTR(ws.c_str());
//}
//EXPORT int WINAPI CovWString2(LPTSTR v){
//	MessageBox(NULL, v, L"CovWString2", 0);
//	return lstrlenW(v);
//}
//
//EXPORT int WINAPI CovWString3(LPTSTR v1, LPTSTR v2){
//	//MessageBox(NULL, v, L"CovWString3", 0);
//	return lstrlenW(v1) + lstrlenW(v2);
//}
//EXPORT char* WINAPI CovWString4(LPTSTR v){
//	writeLog(L"传入数据="); writeLog(v); writeLog(L"\r\n");// 由于c#端定义了字符集为unicode，所以v是utf16数据
//	wstring ws(v);
//	ws.append(L"__按wstring进行了处理");
//	string s = jw::w2s(ws);// 转换为utf-8的话，直接就是乱码
//	return string2char(s);
//}
//EXPORT void WINAPI CovWString5(
//	LPTSTR outsb,
//	LPTSTR param){
//	realloc(outsb, 100);
//	lstrcpy(outsb, param);
//}
//EXPORT int WINAPI CovString(char* v){
//	return 1234;
//}
//EXPORT char* WINAPI CovString2(char* v){
//	writeLog(L"v  ="); writeLog(v); writeLog(L"\r\n");
//	string s(v);
//	s.append("__来自DLL的数据");
//	return string2char(s);
//}
//EXPORT int WINAPI Calc(int v){
//	return v * 3;
//}