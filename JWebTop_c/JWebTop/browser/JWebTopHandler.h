// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#ifndef CEF_JWEBTOP_BROWSER_HANDLER_H_
#define CEF_JWEBTOP_BROWSER_HANDLER_H_

#include "include/cef_client.h"
#include "JWebTop/winctrl/JWebTopConfigs.h"
#include <list>

class JWebTopHandler : public CefClient,
	public CefDisplayHandler,
	public CefLifeSpanHandler,
	//public CefKeyboardHandler,
	public CefLoadHandler {
public:
	JWebTopHandler();
	~JWebTopHandler();

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

	// Request that all existing browser windows close.
	void CloseAllBrowsers(bool force_close);

	bool IsClosing() const { return is_closing_; }

	CefRefPtr<CefBrowser> GetBrowser();
	HWND GetBrowserHwnd();

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

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(JWebTopHandler);
};

#endif  // CEF_JWEBTOP_BROWSER_HANDLER_H_
