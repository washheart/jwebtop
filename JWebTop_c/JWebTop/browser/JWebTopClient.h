// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#ifndef CEF_JWEBTOP_BROWSER_HANDLER_H_
#define CEF_JWEBTOP_BROWSER_HANDLER_H_

#include <list>
#include <set>
#include <string>
#include "include/cef_client.h"
#include "include/wrapper/cef_message_router.h"
#include "JWebTop/config/JWebTopConfigs.h"
#include "JWebTop/jshandler/JSDialogHandler.h"
using namespace std;
class JWebTopClient : public CefClient,
	public CefDisplayHandler,
	public CefLifeSpanHandler,
	public CefLoadHandler,
	public CefRequestHandler,
	public CefContextMenuHandler{
private:
	JWebTopConfigs * configs = NULL; // 构建浏览器时的配置信息
	wstring taskId;                // DLL方式时对应创建浏览器的任务ID
	CefRefPtr<JSDialogHandler> dialog_handler_;
public:
	JWebTopClient();
	~JWebTopClient();
	void setJWebTopConfigs(JWebTopConfigs * configs){ this->configs = configs; }
	void setTaskId(wstring taskId){ this->taskId = taskId; }
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
	virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE{
		return this;
	}
	virtual CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() OVERRIDE{
		//return this;
		return dialog_handler_;
	}

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

	bool GetAuthCredentials(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		bool isProxy,
		const CefString& host,
		int port,
		const CefString& realm,
		const CefString& scheme,
		CefRefPtr<CefAuthCallback> callback) OVERRIDE;

	//// CefKeyboardHandler-------------------------------------------------------------------------------
	//virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
	// const CefKeyEvent& event,
	// CefEventHandle os_event,
	// bool* is_keyboard_shortcut)OVERRIDE;
	//CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE{
	// return this;
	//}
	//// CefKeyboardHandler-------------------------------------------------------------------------------

	// CefContextMenuHandler------------------------------------------------------------------------------
	void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		CefRefPtr<CefMenuModel> model) OVERRIDE{
		model->Clear();// 移除所有默认菜单
	}
private:
	// 和render进程进行通信的相关变量
	typedef std::set<CefMessageRouterBrowserSide::Handler*> MessageHandlerSet;
	CefRefPtr<CefMessageRouterBrowserSide> message_router_;
	MessageHandlerSet message_handler_set_;

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(JWebTopClient);
};

#endif  // CEF_JWEBTOP_BROWSER_HANDLER_H_
