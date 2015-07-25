#include "JWebTopJSHanlder.h"
#include "include/cef_app.h"
#include "JJH_Windows.h"

// 获取函数对应的窗口句柄：默认从最后一个参数获取，如果没有知道最后一个参数，从object的handler中取
HWND getHWND(CefRefPtr<CefV8Value> object/*JS对象*/, const CefV8ValueList& arguments/*函数参数列表*/
	, CefV8ValueList::size_type lastIdx/*最后一个参数的地址*/){
	LONG hWnd = 0;
	if (arguments.size() > lastIdx){
		hWnd = static_cast<LONG>(arguments[lastIdx]->GetIntValue());
	}
	else{
		CefRefPtr<CefV8Value> v = object->GetValue("handler");
		hWnd = v->GetIntValue();
	}
	return HWND(hWnd);
}

void regist(const CefRefPtr<CefV8Value> jWebTop, const CefString& fn, CefRefPtr<CefV8Handler> handler){
	jWebTop->SetValue(fn, CefV8Value::CreateFunction(fn, handler), V8_PROPERTY_ATTRIBUTE_NONE);
}
void regist(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context){
	CefRefPtr<CefV8Value> object = context->GetGlobal();
	// 尝试添加自定义对象
	CefRefPtr<CefV8Accessor> accessor;// 有必要的话，可以扩展该类
	CefRefPtr<CefV8Value> jWebTop = object->CreateObject(accessor);
	object->SetValue("JWebTop", jWebTop, V8_PROPERTY_ATTRIBUTE_NONE);// 把创建的对象附加到V8根对象上
	// 
	regist(jWebTop, "setSize", new JJH_SetSize());
	regist(jWebTop, "move", new JJH_Move());
	regist(jWebTop, "setTitle", new JJH_SetTitle());
	regist(jWebTop, "getTitle", new JJH_GetTitle());
}