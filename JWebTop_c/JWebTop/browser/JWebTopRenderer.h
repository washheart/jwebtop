// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#ifndef CEF_JWEBTOP_APP_RENDERER_H_
#define CEF_JWEBTOP_APP_RENDERER_H_

#include <set>
#include "include/cef_app.h"

// Client app implementation for the renderer process.
class JWebTopRender : public CefRenderProcessHandler {
public:
	// Interface for renderer delegates. All Delegates must be returned via
	// CreateDelegates. Do not perform work in the Delegate
	// constructor. See CefRenderProcessHandler for documentation.
	class Delegate : public CefBase {
	public:
		virtual void OnRenderThreadCreated(CefRefPtr<JWebTopRender> app,
			CefRefPtr<CefListValue> extra_info) {}

		virtual void OnWebKitInitialized(CefRefPtr<JWebTopRender> app) {}

		virtual void OnBrowserCreated(CefRefPtr<JWebTopRender> app,
			CefRefPtr<CefBrowser> browser) {}

		virtual void OnBrowserDestroyed(CefRefPtr<JWebTopRender> app,
			CefRefPtr<CefBrowser> browser) {}

		virtual CefRefPtr<CefLoadHandler> GetLoadHandler(
			CefRefPtr<JWebTopRender> app) {
			return NULL;
		}

		virtual bool OnBeforeNavigation(CefRefPtr<JWebTopRender> app,
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request,
			cef_navigation_type_t navigation_type,
			bool is_redirect) {
			return false;
		}

		virtual void OnContextCreated(CefRefPtr<JWebTopRender> app,
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context) {}

		virtual void OnContextReleased(CefRefPtr<JWebTopRender> app,
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context) {}

		virtual void OnUncaughtException(CefRefPtr<JWebTopRender> app,
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context,
			CefRefPtr<CefV8Exception> exception,
			CefRefPtr<CefV8StackTrace> stackTrace) {}

		virtual void OnFocusedNodeChanged(CefRefPtr<JWebTopRender> app,
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefDOMNode> node) {}

		// Called when a process message is received. Return true if the message was
		// handled and should not be passed on to other handlers. Delegates
		// should check for unique message names to avoid interfering with each
		// other.
		virtual bool OnProcessMessageReceived(
			CefRefPtr<JWebTopRender> app,
			CefRefPtr<CefBrowser> browser,
			CefProcessId source_process,
			CefRefPtr<CefProcessMessage> message) {
			return false;
		}
		IMPLEMENT_REFCOUNTING(Delegate);
	};

	typedef std::set<CefRefPtr<Delegate> > DelegateSet;

	JWebTopRender();

private:
	// Creates all of the Delegate objects. Implemented by cefclient in
	// client_app_delegates_renderer.cc
	static void CreateDelegates(DelegateSet& delegates);

	//// CefApp methods.
	//CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE {
	//  return this;
	//}

	// CefRenderProcessHandler methods.
	void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) OVERRIDE;
	void OnWebKitInitialized() OVERRIDE;
	void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) OVERRIDE;
	CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;
	bool OnBeforeNavigation(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		NavigationType navigation_type,
		bool is_redirect) OVERRIDE;
	void OnContextCreated(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context) OVERRIDE;
	void OnContextReleased(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context) OVERRIDE;
	void OnUncaughtException(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context,
		CefRefPtr<CefV8Exception> exception,
		CefRefPtr<CefV8StackTrace> stackTrace) OVERRIDE;
	void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefDOMNode> node) OVERRIDE;
	bool OnProcessMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message) OVERRIDE;

private:
	// Set of supported Delegates.
	DelegateSet delegates_;

	IMPLEMENT_REFCOUNTING(JWebTopRender);
	DISALLOW_COPY_AND_ASSIGN(JWebTopRender);
};


#endif  // CEF_JWEBTOP_APP_RENDERER_H_
