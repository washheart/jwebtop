#ifndef CEF_JWEBTOP_JS_HANDLER_H_
#define CEF_JWEBTOP_JS_HANDLER_H_

#include "include/cef_app.h" 

// 也可以通过inline或static关键字将getHWND方法实现在此头文件中。
// 获取函数对应的窗口句柄：默认从最后一个参数获取，如果没有知道最后一个参数，从object的handler中取
HWND getHWND(CefRefPtr<CefV8Value> object/*JS对象*/
	, const CefV8ValueList& arguments/*函数参数列表*/
	, CefV8ValueList::size_type lastIdx/*最后一个参数的地址*/);

// 注册JWebTop相关函数到V8Context
void regist(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context);
namespace jw{
	namespace js{
		CefRefPtr<CefListValue> parserCopyFile(HWND hWnd, CefRefPtr<CefDictionaryValue> json);
	}
}
#endif  // CEF_JWEBTOP_JS_HANDLER_H_