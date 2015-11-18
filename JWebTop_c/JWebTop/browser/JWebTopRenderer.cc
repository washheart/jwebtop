// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <string>

#include "JWebTopApp.h"
#include "JWebTop/jshandler/JWebTopJSHanlder.h"
#include "include/base/cef_logging.h"
#include "JWebTop/dllex/JWebTop_DLLEx.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif

namespace renderer {
	namespace {
		// Must match the value in client_handler.cc.
		const char kFocusedNodeChangedMessage[] = "ClientRenderer.FocusedNodeChanged";

		class ClientRenderDelegate : public JWebTopApp::Delegate {
		public:
			ClientRenderDelegate()
				: last_node_is_editable_(false) {
			}

			virtual void OnWebKitInitialized(CefRefPtr<JWebTopApp> app) OVERRIDE{
				// Create the renderer-side router for query handling.
				CefMessageRouterConfig config;
				message_router_ = CefMessageRouterRendererSide::Create(config);
			}

			virtual void OnContextCreated(CefRefPtr<JWebTopApp> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefV8Context> context) OVERRIDE{
				message_router_->OnContextCreated(browser, frame, context);
			}

			virtual void OnContextReleased(CefRefPtr<JWebTopApp> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefV8Context> context) OVERRIDE{
				message_router_->OnContextReleased(browser, frame, context);
			}
			 
			virtual void OnFocusedNodeChanged(CefRefPtr<JWebTopApp> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefDOMNode> node) OVERRIDE{
				bool is_editable = (node.get() && node->IsEditable());
				if (is_editable != last_node_is_editable_) {
					// Notify the browser of the change in focused element type.
					last_node_is_editable_ = is_editable;
					CefRefPtr<CefProcessMessage> message =
						CefProcessMessage::Create(kFocusedNodeChangedMessage);
					message->GetArgumentList()->SetBool(0, is_editable);
					browser->SendProcessMessage(PID_BROWSER, message);
				}
			}

			virtual bool OnProcessMessageReceived(
				CefRefPtr<JWebTopApp> app,
				CefRefPtr<CefBrowser> browser,
				CefProcessId source_process,
				CefRefPtr<CefProcessMessage> message) OVERRIDE{
				wstring name = message->GetName().ToWString();
				if (B2R_MSG_NAME == name){
					jw::dllex::render_processMsg(browser, source_process, message);
					return 1;
				}
				return message_router_->OnProcessMessageReceived(browser, source_process, message);
			}

		private:
			bool last_node_is_editable_;

			// Handles the renderer side of query routing.
			CefRefPtr<CefMessageRouterRendererSide> message_router_;

			IMPLEMENT_REFCOUNTING(ClientRenderDelegate);
		};

	}  // namespace

	void CreateDelegates(JWebTopApp::DelegateSet& delegates) {
		delegates.insert(new ClientRenderDelegate);
	}

}  // namespace renderer


void JWebTopApp::CreateDelegates__(DelegateSet& delegates) {
	renderer::CreateDelegates(delegates);
	//performance_test::CreateDelegates(delegates);
}

void JWebTopApp::OnRenderThreadCreated(
	CefRefPtr<CefListValue> extra_info) {
	CreateDelegates__(delegates_);

	DelegateSet::iterator it = delegates_.begin();
	for (; it != delegates_.end(); ++it)
		(*it)->OnRenderThreadCreated(this, extra_info);
#ifdef JWebTopLog
	writeLog("===JWebTopApp-------------------------OnRenderThreadCreated\r\n");
#endif
}


void JWebTopApp::OnWebKitInitialized() {
	DelegateSet::iterator it = delegates_.begin();
	for (; it != delegates_.end(); ++it)
		(*it)->OnWebKitInitialized(this);
}

void JWebTopApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
	DelegateSet::iterator it = delegates_.begin();
	for (; it != delegates_.end(); ++it)
		(*it)->OnBrowserCreated(this, browser);
#ifdef JWebTopLog
	writeLog("===JWebTopApp-------------------------OnBrowserCreated\r\n");
#endif
}

void JWebTopApp::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) {
	DelegateSet::iterator it = delegates_.begin();
	for (; it != delegates_.end(); ++it)
		(*it)->OnBrowserDestroyed(this, browser);
#ifdef JWebTopLog
	writeLog("===JWebTopApp-------------------------OnBrowserDestroyed\r\n");
#endif
}

CefRefPtr<CefLoadHandler> JWebTopApp::GetLoadHandler() {
	CefRefPtr<CefLoadHandler> load_handler;
	DelegateSet::iterator it = delegates_.begin();
	for (; it != delegates_.end() && !load_handler.get(); ++it)
		load_handler = (*it)->GetLoadHandler(this);

#ifdef JWebTopLog
	writeLog("===JWebTopApp-------------------------GetLoadHandler\r\n");
#endif
	return load_handler;
}

bool JWebTopApp::OnBeforeNavigation(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request,
	NavigationType navigation_type,
	bool is_redirect) {
	DelegateSet::iterator it = delegates_.begin();
	for (; it != delegates_.end(); ++it) {
		if ((*it)->OnBeforeNavigation(this, browser, frame, request,
			navigation_type, is_redirect)) {
			return true;
		}
	}
#ifdef JWebTopLog
	writeLog("===JWebTopApp-------------------------OnBeforeNavigation\r\n");
#endif
	return false;
}


void JWebTopApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context) {
	DelegateSet::iterator it = delegates_.begin();
	for (; it != delegates_.end(); ++it)
		(*it)->OnContextCreated(this, browser, frame, context);
#ifdef JWebTopLog
	writeLog("===JWebTopApp-------------------------OnContextCreated\r\n");
#endif
	regist(browser, frame, context);
}

void JWebTopApp::OnContextReleased(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context) {
	DelegateSet::iterator it = delegates_.begin();
	for (; it != delegates_.end(); ++it)
		(*it)->OnContextReleased(this, browser, frame, context);
#ifdef JWebTopLog
	writeLog("===JWebTopApp-------------------------OnContextReleased\r\n");
#endif
}

void JWebTopApp::OnUncaughtException(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context,
	CefRefPtr<CefV8Exception> exception,
	CefRefPtr<CefV8StackTrace> stackTrace) {
	DelegateSet::iterator it = delegates_.begin();
	for (; it != delegates_.end(); ++it) {
		(*it)->OnUncaughtException(this, browser, frame, context, exception,
			stackTrace);
	}
#ifdef JWebTopLog
	writeLog("===JWebTopApp-------------------------OnUncaughtException\r\n");
#endif
}


void JWebTopApp::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefDOMNode> node) {
	DelegateSet::iterator it = delegates_.begin();
	for (; it != delegates_.end(); ++it)
		(*it)->OnFocusedNodeChanged(this, browser, frame, node);
#ifdef JWebTopLog
	writeLog("===JWebTopApp-------------------------OnFocusedNodeChanged\r\n");
#endif
}

bool JWebTopApp::OnProcessMessageReceived(
	CefRefPtr<CefBrowser> browser,
	CefProcessId source_process,
	CefRefPtr<CefProcessMessage> message) {
	DCHECK_EQ(source_process, PID_BROWSER);

	bool handled = false;

	DelegateSet::iterator it = delegates_.begin();
	for (; it != delegates_.end() && !handled; ++it) {
		handled = (*it)->OnProcessMessageReceived(this, browser, source_process,
			message);
	}
#ifdef JWebTopLog
	writeLog("===JWebTopApp-------------------------OnProcessMessageReceived\r\n");
#endif

	return handled;
}
