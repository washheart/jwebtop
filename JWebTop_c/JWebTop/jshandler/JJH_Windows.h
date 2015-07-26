#ifndef CEF_JWEBTOP_JJH_WINDOWS_H_
#define CEF_JWEBTOP_JJH_WINDOWS_H_

#include <sstream>
#include <string>
#include "include/cef_app.h" 
#include "JWebTopJSHanlder.h"
#include "JWebTop/winctrl/JWebTopWinCtrl.h"

// JJH=JWebTop JavaScriptHandler
class JJH_SetSize : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		if (arguments.size() < 2)return false;
		jw::setSize(getHWND(object, arguments, 2), arguments[0]->GetIntValue(), arguments[1]->GetIntValue());
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_SetSize);
};

class JJH_Move : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		if (arguments.size() < 2)return false;
		jw::move(getHWND(object, arguments, 2), arguments[0]->GetIntValue(), arguments[1]->GetIntValue());
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_Move);
};
class JJH_SetBound : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		if (arguments.size() < 4)return false;
		jw::setBound(getHWND(object, arguments, 4), arguments[1]->GetIntValue(), arguments[2]->GetIntValue(), arguments[3]->GetIntValue(), arguments[4]->GetIntValue());
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_SetBound);
};
class JJH_GetBound : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		RECT rt=jw::getBound(getHWND(object, arguments, 0));
		retval = CefV8Value::CreateObject(NULL);
		retval->SetValue("x", CefV8Value::CreateInt(rt.left), V8_PROPERTY_ATTRIBUTE_NONE);
		retval->SetValue("y", CefV8Value::CreateInt(rt.top), V8_PROPERTY_ATTRIBUTE_NONE);
		retval->SetValue("width", CefV8Value::CreateInt(rt.right), V8_PROPERTY_ATTRIBUTE_NONE);
		retval->SetValue("height", CefV8Value::CreateInt(rt.bottom), V8_PROPERTY_ATTRIBUTE_NONE);
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_GetBound);
};

class JJH_SetTitle : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		if (arguments.size() < 1)return false;
		jw::setTitle(getHWND(object, arguments, 1), arguments[0]->GetStringValue().ToWString());
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_SetTitle);
};
class JJH_GetTitle : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		std::wstring title= jw::getTitle(getHWND(object, arguments, 0));
		retval = CefV8Value::CreateString(CefString(title));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_GetTitle);
};

class JJH_Close : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jw::close(getHWND(object, arguments, 0));// 如果是多进程方式，直接调用取不到BrowserWindowInfo，需要采用消息通信方式
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_Close);
};
#endif