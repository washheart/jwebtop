#include "JWebTopJSHanlder.h"

bool JWebTopJSHandler::Execute(const CefString& name,
	CefRefPtr<CefV8Value> object,
	const CefV8ValueList& arguments,
	CefRefPtr<CefV8Value>& retval,
	CefString& exception) {
	//
	std::wstringstream  wss;
	wss << L"JS调用。 name：" << name.ToWString().c_str() << L" 参数长度：" << arguments.size() << L"。";
	// MessageBox(NULL, wss.str().c_str(), L"被JS调用", 1);
	//MessageBox(NULL, L"---", L"被JS调用", 1);
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
	retval = CefV8Value::CreateString(CefString(wss.str()));
	return true;
}