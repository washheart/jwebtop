#include "JWebTopJSHanlder.h"
#include "JWebTop/winctrl/JWebTopWinCtrl.h"
#include "include/cef_app.h"

bool JWebTopJSHandler::Execute(const CefString& name,
	CefRefPtr<CefV8Value> object,
	const CefV8ValueList& arguments,
	CefRefPtr<CefV8Value>& retval,
	CefString& exception) {

	std::wstringstream rs;

	if (name == L"setSize"){
		//jw::setSize((HWND)arguments[0]->GetIntValue(), arguments[1]->GetIntValue(), arguments[2]->GetIntValue());
		int pcount = arguments.size();
		int w = static_cast<int>(arguments[0]->GetIntValue());
		int h = static_cast<int>(arguments[1]->GetIntValue());
		LONG hWnd = 0;
		if (pcount > 2){
			hWnd = static_cast<LONG>(arguments[2]->GetIntValue());
		}
		else{
			CefRefPtr<CefV8Value> v = object->GetValue("handler");
			hWnd = v->GetIntValue();
		}
		const HWND hhh = HWND(hWnd);
		jw::setSize(hhh, w, h);
	}
	//else	if (name == L"setSize"){
	//	int pcount = arguments.size();
	//	LONG hWnd = 0;
	//	if (pcount > 1){
	//		hWnd = static_cast<LONG>(arguments[1]->GetIntValue());
	//	}
	//	else{
	//		CefRefPtr<CefV8Value> v = object->GetValue("handler");
	//		hWnd = v->GetIntValue();
	//	}
	//	jw::setTitle((HWND)hWnd, arguments[2]->GetStringValue().ToWString());
	//}
	else if (name == L"move"){
		//jw::move((HWND)arguments[0]->GetIntValue(), arguments[1]->GetIntValue(), arguments[2]->GetIntValue());
	}
	else if (name == L"setBound"){
		jw::setBound((HWND)arguments[0]->GetIntValue(), arguments[1]->GetIntValue(), arguments[2]->GetIntValue(), arguments[3]->GetIntValue(), arguments[4]->GetIntValue());
	}
	if (name == "showDevTools"){
		//CefWindowInfo windowInfo;
		//CefBrowserSettings settings;
		//CefPoint(pp);
		//pp.x = 300;
		//pp.y = 300;
		//windowInfo.SetAsPopup(NULL, "cef_debug");
		//
		//browser->GetHost()->ShowDevTools(windowInfo, this, settings, pp);
	}
	retval = CefV8Value::CreateString(CefString(rs.str()));
	return true;
}


void regist(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context){

	//#ifdef JWebTopLog
	//	writeLog("===JWebTopRender-------------------------OnContextCreated\r\n");
	//#endif
	CefRefPtr<CefV8Value> object = context->GetGlobal();
	// 尝试添加自定义对象
	CefRefPtr<CefV8Accessor> accessor;// 有必要的话，可以扩展该类
	CefRefPtr<CefV8Value> jWebTop = object->CreateObject(accessor);
	object->SetValue("JWebTop", jWebTop, V8_PROPERTY_ATTRIBUTE_NONE);// 把创建的对象附加到V8根对象上
	CefRefPtr<CefV8Handler> handler = new JWebTopJSHandler();
	jWebTop->SetValue("setSize", CefV8Value::CreateFunction("setSize", handler), V8_PROPERTY_ATTRIBUTE_NONE);
	jWebTop->SetValue("move", CefV8Value::CreateFunction("move", handler), V8_PROPERTY_ATTRIBUTE_NONE);
	jWebTop->SetValue("setTitle", CefV8Value::CreateFunction("setTitle", handler), V8_PROPERTY_ATTRIBUTE_NONE);

	//jWebTop->SetValue("showDev", CefV8Value::CreateFunction("showDev", handler), V8_PROPERTY_ATTRIBUTE_NONE);
}