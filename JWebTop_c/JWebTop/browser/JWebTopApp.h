#ifndef CEF_JWEBTOP_APP_H_
#define CEF_JWEBTOP_APP_H_
#include <set>
#include "include/cef_app.h"
#include "include/wrapper/cef_message_router.h"

class JWebTopApp : public CefApp,
	public CefBrowserProcessHandler,
	public CefRenderProcessHandler{
public:
	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()	OVERRIDE{ return this; }
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler()	OVERRIDE{ return this; }

	virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)OVERRIDE;

	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() OVERRIDE;

	// CefRenderProcessHandler----start---------------------------------------------------------------------------------------------
public:
	class Delegate : public virtual CefBase {
	public:
		virtual void OnRenderThreadCreated(CefRefPtr<JWebTopApp> app, CefRefPtr<CefListValue> extra_info) {}
		virtual void OnWebKitInitialized(CefRefPtr<JWebTopApp> app) {}
		virtual void OnBrowserCreated(CefRefPtr<JWebTopApp> app, CefRefPtr<CefBrowser> browser) {}
		virtual void OnBrowserDestroyed(CefRefPtr<JWebTopApp> app, CefRefPtr<CefBrowser> browser) {}
		virtual CefRefPtr<CefLoadHandler> GetLoadHandler(CefRefPtr<JWebTopApp> app) {
			return NULL;
		}
		virtual bool OnBeforeNavigation(CefRefPtr<JWebTopApp> app, CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, cef_navigation_type_t navigation_type, bool is_redirect) {
			return false;
		}
		virtual void OnContextCreated(CefRefPtr<JWebTopApp> app, CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {}
		virtual void OnContextReleased(CefRefPtr<JWebTopApp> app, CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {}
		virtual void OnUncaughtException(CefRefPtr<JWebTopApp> app, CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Exception> exception, CefRefPtr<CefV8StackTrace> stackTrace) {}
		virtual void OnFocusedNodeChanged(CefRefPtr<JWebTopApp> app, CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefDOMNode> node) {}
		// Called when a process message is received. Return true if the message was
		// handled and should not be passed on to other handlers. Delegates
		// should check for unique message names to avoid interfering with each
		// other.
		virtual bool OnProcessMessageReceived(CefRefPtr<JWebTopApp> app, CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
			return false;
		}
	};
	typedef std::set<CefRefPtr<Delegate> > DelegateSet;
private:
	// Creates all of the Delegate objects. Implemented by cefclient in
	// client_app_delegates_renderer.cc
	static void CreateDelegates__(DelegateSet& delegates);

	// CefRenderProcessHandler methods.
	void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) OVERRIDE;
	void OnWebKitInitialized() OVERRIDE;
	void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) OVERRIDE;
	CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;
	bool OnBeforeNavigation(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, NavigationType navigation_type, bool is_redirect) OVERRIDE;
	void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;
	void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;
	void OnUncaughtException(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Exception> exception, CefRefPtr<CefV8StackTrace> stackTrace) OVERRIDE;
	void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefDOMNode> node) OVERRIDE;
	bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) OVERRIDE;

private:
	// Set of supported Delegates.
	DelegateSet delegates_;
	CefRefPtr<CefMessageRouterRendererSide> message_router_;
	// CefRenderProcessHandler---end------------------------------------------------------------------------------------------------


private:
	IMPLEMENT_REFCOUNTING(JWebTopApp);
};

#endif  // CEF_JWEBTOP_APP_H_