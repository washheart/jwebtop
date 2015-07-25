// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <string>

#include "JWebTopRenderer.h"
#include "JWebTop/handler/JWebTopJSHanlder.h"
#include "include/base/cef_logging.h"
#include "JWebTop/tests/TestUtil.h"

JWebTopRender::JWebTopRender() {
}

void JWebTopRender::OnRenderThreadCreated(
	CefRefPtr<CefListValue> extra_info) {
	CreateDelegates(delegates_);

	DelegateSet::iterator it = delegates_.begin();
	for (; it != delegates_.end(); ++it)
		(*it)->OnRenderThreadCreated(this, extra_info);
#ifdef JWebTopLog
	writeLog("===JWebTopRender-------------------------OnRenderThreadCreated\r\n");
#endif
}


void JWebTopRender::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
	DelegateSet::iterator it = delegates_.begin();
	for (; it != delegates_.end(); ++it)
		(*it)->OnBrowserCreated(this, browser);
#ifdef JWebTopLog
	writeLog("===JWebTopRender-------------------------OnBrowserCreated\r\n");
#endif
}

void JWebTopRender::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) {
	DelegateSet::iterator it = delegates_.begin();
	for (; it != delegates_.end(); ++it)
		(*it)->OnBrowserDestroyed(this, browser);
#ifdef JWebTopLog
	writeLog("===JWebTopRender-------------------------OnBrowserDestroyed\r\n");
#endif
}

CefRefPtr<CefLoadHandler> JWebTopRender::GetLoadHandler() {
	CefRefPtr<CefLoadHandler> load_handler;
	DelegateSet::iterator it = delegates_.begin();
	for (; it != delegates_.end() && !load_handler.get(); ++it)
		load_handler = (*it)->GetLoadHandler(this);

#ifdef JWebTopLog
	writeLog("===JWebTopRender-------------------------GetLoadHandler\r\n");
#endif
	return load_handler;
}

bool JWebTopRender::OnBeforeNavigation(CefRefPtr<CefBrowser> browser,
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
	writeLog("===JWebTopRender-------------------------OnBeforeNavigation\r\n");
#endif
	return false;
}


void JWebTopRender::OnWebKitInitialized() {
	DelegateSet::iterator it = delegates_.begin();
	for (; it != delegates_.end(); ++it)
		(*it)->OnWebKitInitialized(this);
}


void JWebTopRender::OnContextCreated(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context) {
	DelegateSet::iterator it = delegates_.begin();
	for (; it != delegates_.end(); ++it)
		(*it)->OnContextCreated(this, browser, frame, context);
#ifdef JWebTopLog
	writeLog("===JWebTopRender-------------------------OnContextCreated\r\n");
#endif
	regist(browser, frame, context);
}

void JWebTopRender::OnContextReleased(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context) {
	DelegateSet::iterator it = delegates_.begin();
	for (; it != delegates_.end(); ++it)
		(*it)->OnContextReleased(this, browser, frame, context);
#ifdef JWebTopLog
	writeLog("===JWebTopRender-------------------------OnContextReleased\r\n");
#endif
}

void JWebTopRender::OnUncaughtException(
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
	writeLog("===JWebTopRender-------------------------OnUncaughtException\r\n");
#endif
}

void JWebTopRender::CreateDelegates(JWebTopRender::DelegateSet& delegates) {
	//delegates.insert(new Delegate());
	delegates.insert(new JWebTopRender::Delegate);
#ifdef JWebTopLog
	writeLog("===JWebTopRender-------------------------CreateDelegates\r\n");
#endif
}
void JWebTopRender::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefDOMNode> node) {
	DelegateSet::iterator it = delegates_.begin();
	for (; it != delegates_.end(); ++it)
		(*it)->OnFocusedNodeChanged(this, browser, frame, node);
#ifdef JWebTopLog
	writeLog("===JWebTopRender-------------------------OnFocusedNodeChanged\r\n");
#endif
}

bool JWebTopRender::OnProcessMessageReceived(
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
	writeLog("===JWebTopRender-------------------------OnProcessMessageReceived\r\n");
#endif

	return handled;
}
