// ²Î¿¼dialog_handler_gtk
#ifndef CEF_JWEBTOP_JAVASCRIPT_DIALOG_H_
#define CEF_JWEBTOP_JAVASCRIPT_DIALOG_H_
#pragma once

#include <Windows.h>

#include "include/cef_jsdialog_handler.h"

class JSDialogHandler : public CefJSDialogHandler {
public:
	JSDialogHandler();

	bool OnJSDialog(CefRefPtr<CefBrowser> browser,
		const CefString& origin_url,
		const CefString& accept_lang,
		JSDialogType dialog_type,
		const CefString& message_text,
		const CefString& default_prompt_text,
		CefRefPtr<CefJSDialogCallback> callback,
		bool& suppress_message) OVERRIDE;
	bool OnBeforeUnloadDialog(
		CefRefPtr<CefBrowser> browser,
		const CefString& message_text,
		bool is_reload,
		CefRefPtr<CefJSDialogCallback> callback) OVERRIDE;
	void OnResetDialogState(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnDialogClosed(CefRefPtr<CefBrowser> browser) OVERRIDE;
private:
	//content::JavaScriptMessageType message_type_;
	HWND dialog_win_;
	HWND parent_win_;
	base::string16 message_text_;
	base::string16 default_prompt_text_;
	static INT_PTR CALLBACK DialogProc(HWND dialog, UINT message, WPARAM wparam,
		LPARAM lparam);

	// Since the message loop we expect to run in isn't going to be nicely
	// calling IsDialogMessage(), we need to hook the wnd proc and call it
	// ourselves. See http://support.microsoft.com/kb/q187988/
	static bool InstallMessageHook();
	static bool UninstallMessageHook();
	static LRESULT CALLBACK GetMsgProc(int code, WPARAM wparam, LPARAM lparam);
	static HHOOK msg_hook_;
	static int msg_hook_user_count_;

	CefRefPtr<CefJSDialogCallback> js_dialog_callback_;

	IMPLEMENT_REFCOUNTING(JSDialogHandler);
	//DISALLOW_COPY_AND_ASSIGN(JSDialogHandler);
};

#endif  // CEF_TESTS_CEFCLIENT_BROWSER_DIALOG_HANDLER_GTK_H_
