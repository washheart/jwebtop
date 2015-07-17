// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <string>

#include "JWebTopRenderer.h"
#include "JWebTop/handler/JWebTopJSHanlder.h"
#include "include/base/cef_logging.h"

JWebTopRender::JWebTopRender() {
}

void JWebTopRender::OnRenderThreadCreated(
    CefRefPtr<CefListValue> extra_info) {
  CreateDelegates(delegates_);

  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it)
    (*it)->OnRenderThreadCreated(this, extra_info);
}

void initCallBack(){
	std::string extensionCode =
		"var jwebtop;if(!jwebtop)jwebtop = {}; "
		"(function() {"
		" jwebtop.myval = 'My Value!'; "
		"})(); ";
	//std::string extensionCode = "var jwebtop;if(!jwebtop)jwebtop = {};";
	CefRegisterExtension("jwebtop", extensionCode, NULL);
}
void JWebTopRender::OnWebKitInitialized() {
  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it)
    (*it)->OnWebKitInitialized(this);
  initCallBack();
}

void JWebTopRender::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it)
    (*it)->OnBrowserCreated(this, browser);
}

void JWebTopRender::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) {
  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it)
    (*it)->OnBrowserDestroyed(this, browser);
}

CefRefPtr<CefLoadHandler> JWebTopRender::GetLoadHandler() {
  CefRefPtr<CefLoadHandler> load_handler;
  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end() && !load_handler.get(); ++it)
    load_handler = (*it)->GetLoadHandler(this);

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

  return false;
}
void JWebTopRender::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         CefRefPtr<CefV8Context> context) {
  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it)
	  (*it)->OnContextCreated(this, browser, frame, context);
  CefRefPtr<CefV8Value> object = context->GetGlobal();
  CefRefPtr<CefV8Handler> handler2 = new JWebTopJSHandler();
  CefRefPtr<CefV8Value> func2 = CefV8Value::CreateFunction("ctrlwin", handler2);
  object->SetValue("ctrlwin", func2, V8_PROPERTY_ATTRIBUTE_NONE);
  object->SetValue("myjs", CefV8Value::CreateFunction("myjs", handler2), V8_PROPERTY_ATTRIBUTE_NONE);
  object->SetValue("showDevTools", CefV8Value::CreateFunction("showDevTools", handler2), V8_PROPERTY_ATTRIBUTE_NONE);
  //initCallBack();// 在这里加入，会导致程序崩溃
}

void JWebTopRender::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                          CefRefPtr<CefFrame> frame,
                                          CefRefPtr<CefV8Context> context) {
  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it)
    (*it)->OnContextReleased(this, browser, frame, context);
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
}

void JWebTopRender::CreateDelegates(JWebTopRender::DelegateSet& delegates) {
	//delegates.insert(new Delegate());
	delegates.insert(new JWebTopRender::Delegate);
}
void JWebTopRender::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                                             CefRefPtr<CefFrame> frame,
                                             CefRefPtr<CefDOMNode> node) {
  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it)
    (*it)->OnFocusedNodeChanged(this, browser, frame, node);
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

  return handled;
}
