#ifndef CEF_JWEBTOP_APP_H_
#define CEF_JWEBTOP_APP_H_
#include <set>

#include "include/cef_app.h"
#include "JWebTopRenderer.h"

class JWebTopApp : public CefApp,
	public CefBrowserProcessHandler {
public:
	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
		OVERRIDE{ return this; }


	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() {
		return new JWebTopRender();
	}
	virtual void OnBeforeCommandLineProcessing(
		const CefString& process_type,
		CefRefPtr<CefCommandLine> command_line)OVERRIDE;
	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() OVERRIDE;

private:
	 IMPLEMENT_REFCOUNTING(JWebTopApp);
};

#endif  // CEF_JWEBTOP_APP_H_