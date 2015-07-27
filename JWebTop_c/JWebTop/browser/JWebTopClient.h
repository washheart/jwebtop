// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#ifndef CEF_JWEBTOP_BROWSER_HANDLER_H_
#define CEF_JWEBTOP_BROWSER_HANDLER_H_

#include <list>
#include <set>
#include "include/cef_client.h"
#include "include/wrapper/cef_message_router.h"
#include "JWebTop/winctrl/JWebTopConfigs.h"

class JWebTopClient : public CefClient,
	public CefDisplayHandler,
	public CefLifeSpanHandler,
	public CefLoadHandler,
	public CefRequestHandler {
public:
	JWebTopClient();
	~JWebTopClient();

	// CefClient methods:
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE{
		return this;
	}
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE{
		return this;
	}
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE{
		return this;
	}
	virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE{
		return this;
	}
	// 替换默认的一些对话框
	// virtual CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() OVERRIDE{
	//	 return dialog_handler_;
	// }

	bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message) OVERRIDE;
	// CefDisplayHandler methods:
	//virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;

	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

	// CefLoadHandler methods:
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl) OVERRIDE;  
	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		int httpStatusCode);

	// CefRequestHandler-------------------------------------------------------------------------------
	bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		bool is_redirect) OVERRIDE;
	void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
		TerminationStatus status) OVERRIDE;

	// Request that all existing browser windows close.
	void CloseAllBrowsers(bool force_close);

	bool IsClosing() const { return is_closing_; }

	//// CefKeyboardHandler-------------------------------------------------------------------------------
	//virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
	// const CefKeyEvent& event,
	// CefEventHandle os_event,
	// bool* is_keyboard_shortcut)OVERRIDE;
	//CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE{
	// return this;
	//}
	//// CefKeyboardHandler-------------------------------------------------------------------------------
private:
	// List of existing browser windows. Only accessed on the CEF UI thread.
	typedef std::list<CefRefPtr<CefBrowser> > BrowserList;
	BrowserList browser_list_;
	bool is_closing_;

	// 和render进程进行通信的相关变量
	typedef std::set<CefMessageRouterBrowserSide::Handler*> MessageHandlerSet;
	CefRefPtr<CefMessageRouterBrowserSide> message_router_;
	MessageHandlerSet message_handler_set_;

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(JWebTopClient);
};

#endif  // CEF_JWEBTOP_BROWSER_HANDLER_H_
