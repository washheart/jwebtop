#include "JWebTopJSHanlder.h"
#include "JWebTop/tests/TestUtil.h"
#include "JWebTop/winctrl/JWebTopWinCtrl.h"

bool JWebTopJSHandler::Execute(const CefString& name,
	CefRefPtr<CefV8Value> object,
	const CefV8ValueList& arguments,
	CefRefPtr<CefV8Value>& retval,
	CefString& exception) {
	//
	std::wstringstream rs;
#ifdef JWebTopLog
	std::wstringstream  wss;
	wss << L"JS调用。 name：" << name.ToWString().c_str() << L" 参数长度：" << arguments.size() << L"。\r\n";
	int i = 0, size = arguments.size();
	for (; i < size; i++){
		wss << L"参数[" << i << "]=" << arguments[0]->GetStringValue().ToWString();
	}
	writeLog(wss.str());
#endif // JWebTop
	//TransparentWnd* winHandler = (TransparentWnd*)static_cast<long>(arguments[0]->GetIntValue());
	if (name == L"setSize"){
		//int w = static_cast<int>(arguments[1]->GetIntValue());
		//int h = static_cast<int>(arguments[2]->GetIntValue());
		setSize((HWND)arguments[0]->GetIntValue(), arguments[1]->GetIntValue(), arguments[2]->GetIntValue());
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