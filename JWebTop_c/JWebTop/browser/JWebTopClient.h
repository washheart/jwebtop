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
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
using namespace std;

class JWebTopClient : public CefClient,
	public CefDisplayHandler,
	public CefLifeSpanHandler,
	public CefLoadHandler,
	public CefRequestHandler,
	public CefContextMenuHandler,
	public CefDragHandler,
	public CefDownloadHandler {
private:

	JWebTopConfigs * configs = NULL; // ���������ʱ��������Ϣ
	wstring taskId;                // DLL��ʽʱ��Ӧ���������������ID
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
	virtual CefRefPtr<CefDragHandler> GetDragHandler() OVERRIDE{
		return this;
	}
	virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler() {
		return this;
	}
	// CefDownloadHandler------------------------------------------------------------------------------------
	virtual void OnBeforeDownload(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDownloadItem> download_item,
		const CefString& suggested_name,
		CefRefPtr<CefBeforeDownloadCallback> callback)OVERRIDE;
	virtual void OnDownloadUpdated(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDownloadItem> download_item,
		CefRefPtr<CefDownloadItemCallback> callback)OVERRIDE;


	// CefDisplayHandler------------------------------------------------------------------------------------
	virtual bool OnDragEnter(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDragData> dragData,
		DragOperationsMask mask) OVERRIDE{
		return true;// ����true����ֹĬ�ϵ��϶��ļ�����ϵͳ����Ϊ
	}

	bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message) OVERRIDE;
	// CefDisplayHandler methods:
	//virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;

	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

	// CefDisplayHandler------------------------------------------------------------------------------------
	//virtual void OnAddressChange(CefRefPtr<CefBrowser> browser,
	//	CefRefPtr<CefFrame> frame,
	//	const CefString& url) {
	//	writeLog(L"������������������������CefDisplayHandler---OnAddressChange---\r\n");
	//}
	//virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
	//	const CefString& title) {
	//	writeLog(L"������������������������CefDisplayHandler---OnTitleChange---\r\n");
	//}
	//virtual void OnFaviconURLChange(CefRefPtr<CefBrowser> browser,
	//	const std::vector<CefString>& icon_urls) {
	//	writeLog(L"������������������������CefDisplayHandler---OnFaviconURLChange---\r\n");
	//}
	//virtual bool OnTooltip(CefRefPtr<CefBrowser> browser, CefString& text) {
	//	writeLog(L"������������������������CefDisplayHandler---OnTooltip---\r\n");
	//	return false;
	//}
	//virtual void OnStatusMessage(CefRefPtr<CefBrowser> browser,
	//	const CefString& value) {
	//	writeLog(L"������������������������CefDisplayHandler---OnStatusMessage---\r\n");
	//}
	//virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line) {
	//	writeLog(L"������������������������CefDisplayHandler---OnConsoleMessage---\r\n");
	//	return false;
	//}

	// CefLoadHandler------------------------------------------------------------------------------
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) OVERRIDE;
	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode);
	//virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) {
	//	writeLog(L"������������������������CefLoadHandler---OnLoadingStateChange---\r\n");
	//}
	//virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) {
	//	writeLog(L"������������������������CefLoadHandler---OnLoadStart---\r\n");
	//}

	// CefRequestHandler-------------------------------------------------------------------------------
	bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_redirect) OVERRIDE;
	void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status) OVERRIDE;
	bool GetAuthCredentials(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, bool isProxy, const CefString& host, int port, const CefString& realm, const CefString& scheme, CefRefPtr<CefAuthCallback> callback) OVERRIDE;
	//virtual ReturnValue OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback) {
	//	writeLog(L"������������������������CefRequestHandler---OnBeforeResourceLoad---\r\n");
	//	return RV_CONTINUE;
	//}
	//virtual void OnResourceRedirect(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefString& new_url) {
	//	writeLog(L"������������������������CefRequestHandler---OnResourceRedirect---\r\n");
	//}
	//virtual bool OnResourceResponse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefResponse> response) {
	//	writeLog(L"������������������������CefRequestHandler---OnResourceResponse---\r\n");
	//	return false;
	//}
	void OnRenderViewReady(CefRefPtr<CefBrowser> browser)OVERRIDE;


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
#ifdef JWebTopLog
	void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) OVERRIDE;
	bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags) OVERRIDE;
	void ShowDevTools(CefRefPtr<CefBrowser> browser, const CefPoint& inspect_element_at);
#else
	void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) OVERRIDE{
		model->Clear();// �Ƴ�����Ĭ�ϲ˵�
	}
#endif

	// CefLifeSpanHandler------------------------------------------------------------------------------
	bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& target_url,
		const CefString& target_frame_name,
		CefLifeSpanHandler::WindowOpenDisposition target_disposition,
		bool user_gesture,
		const CefPopupFeatures& popupFeatures,
		CefWindowInfo& windowInfo,
		CefRefPtr<CefClient>& client,
		CefBrowserSettings& settings,
		bool* no_javascript_access) OVERRIDE;
	
private:
	// ��render���̽���ͨ�ŵ���ر���
	typedef std::set<CefMessageRouterBrowserSide::Handler*> MessageHandlerSet;
	CefRefPtr<CefMessageRouterBrowserSide> message_router_;
	MessageHandlerSet message_handler_set_;

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(JWebTopClient);
};

#endif  // CEF_JWEBTOP_BROWSER_HANDLER_H_
