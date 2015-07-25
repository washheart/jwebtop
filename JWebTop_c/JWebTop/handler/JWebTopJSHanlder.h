#ifndef CEF_JWEBTOP_JS_HANDLER_H_
#define CEF_JWEBTOP_JS_HANDLER_H_

#include <sstream>
#include <string>
#include "include/cef_app.h" 

// Client app implementation for the renderer process.
class JWebTopJSHandler : public CefV8Handler {
public:
	virtual bool Execute(const CefString& name,
		CefRefPtr<CefV8Value> object,
		const CefV8ValueList& arguments,
		CefRefPtr<CefV8Value>& retval,
		CefString& exception) OVERRIDE;

private:
	IMPLEMENT_REFCOUNTING(JWebTopJSHandler);
};

void regist(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context);
#endif  // CEF_JWEBTOP_JS_HANDLER_H_