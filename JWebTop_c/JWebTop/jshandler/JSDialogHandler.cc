// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "JSDialogHandler.h"

#include <Windows.h>
//#include <sys/stat.h>
#include <string>
#include <thread>
#include "include/cef_browser.h"
#include "include/cef_parser.h"
#include "include/wrapper/cef_helpers.h"
#include "common/ui/dlgs/InputDialog.h"
// #include "cefclient/browser/root_window.h"

#include "common/tests/TestUtil.h"
using namespace std;
JSDialogHandler::JSDialogHandler()  {
}

void alert(HWND parent,wstring  message_text ){
	MessageBox(parent, LPTSTR(message_text.c_str()), L"警告", 0);
}

bool JSDialogHandler::OnJSDialog(
    CefRefPtr<CefBrowser> browser,
    const CefString& origin_url,
    const CefString& accept_lang,
    JSDialogType dialog_type,
    const CefString& message_text,
    const CefString& default_prompt_text,
    CefRefPtr<CefJSDialogCallback> callback,
    bool& suppress_message) {
  CEF_REQUIRE_UI_THREAD();
  wstring rtn;
  bool success = true;
  HWND parent = browser->GetHost()->GetWindowHandle();
  switch (dialog_type) {
  case JSDIALOGTYPE_ALERT:
	  MessageBox(parent, LPTSTR(message_text.c_str()), L"警告", MB_OK);
	  break;
  case JSDIALOGTYPE_CONFIRM:
	  success = IDOK == MessageBox(parent, LPTSTR(message_text.c_str()), L"确认", MB_OKCANCEL);
	  break;
  case JSDIALOGTYPE_PROMPT:
  {
							  wstring mtxt = message_text.ToWString();
							  wstring ptxt = default_prompt_text.ToWString();
							  success = IDOK == jw::ui::Dlgs::InputDialog::ShowInputDialog(
								  parent, ref(rtn)//
								  , L"输入", ref(mtxt), ref(ptxt));
  }
      break;
  }
  js_dialog_callback_ = callback;
  suppress_message = false;
  callback->Continue(success, CefString(rtn));
  return true;
}
void JSDialogHandler::OnDialogClosed(CefRefPtr<CefBrowser> browser){
	//if (js_dialog_callback_ )js_dialog_callback_->Continue(false, CefString(L""));
}
bool JSDialogHandler::OnBeforeUnloadDialog(
    CefRefPtr<CefBrowser> browser,
    const CefString& message_text,
    bool is_reload,
    CefRefPtr<CefJSDialogCallback> callback) {
  CEF_REQUIRE_UI_THREAD();
  bool suppress_message = false;
  return OnJSDialog(browser, CefString(), CefString(), JSDIALOGTYPE_CONFIRM,
	  message_text, CefString(), callback, suppress_message);
}

void JSDialogHandler::OnResetDialogState(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();
  js_dialog_callback_ = NULL;
}
namespace jw{
	namespace js{
		namespace events{
			// 发送页面准备好事件：new CustomEvent('JWebTopReady')
			void sendReadey(const CefRefPtr<CefFrame> frame){
				CefString js_event(L"var e = new CustomEvent('JWebTopReady');dispatchEvent(e);");
				frame->ExecuteJavaScript(js_event, "", 0);
			}
			// 页面内的子页面（iframe）准备好事件：new CustomEvent('JWebTopIFrameReady')
			void sendIFrameReady(const CefRefPtr<CefFrame> frame){
				CefString js_event(L"var e = new CustomEvent('JWebTopIFrameReady');dispatchEvent(e);");
				frame->ExecuteJavaScript(js_event, "", 0);
			}

			// 发送窗口大小改变事件:new CustomEvent('JWebTopResize',{detail:{w:宽度数值,h:高度数值}})
			void sendSize(const CefRefPtr<CefFrame> frame, const int w, const int h){
				stringstream js_event;
				js_event << "var e = new CustomEvent('JWebTopResize',{"
					<< "	detail:{"
					<< "		width:" << w << ","
					<< "		height:" << h
					<< "	}"
					<< "});"
					<< "dispatchEvent(e);";
				frame->ExecuteJavaScript(js_event.str(), "", 0);
			}

			// 发送窗口位置改变事件:new CustomEvent('JWebTopMove',{detail:{x:X坐标值,y:Y坐标值}})
			void sendMove(const CefRefPtr<CefFrame> frame, const int x, const int y){
				stringstream js_event;
				js_event << "var e = new CustomEvent('JWebTopMove',{"
					<< "	detail:{"
					<< "		x:" << x << ","
					<< "		y:" << y
					<< "	}"
					<< "});"
					<< "dispatchEvent(e);";
				frame->ExecuteJavaScript(js_event.str(), "", 0);
			}

			// 发送窗口被激活事件:new CustomEvent('JWebTopWindowActive',{detail:{handler:被激活的窗口的句柄}})
			void sendWinowActive(const CefRefPtr<CefFrame> frame, const long handler){
				stringstream js_event;
				js_event << "var e = new CustomEvent('JWebTopWindowActive',{"
					<< "	detail:{"
					<< "		handler:" << handler
					<< "	}"
					<< "});"
					<< "dispatchEvent(e);";
				frame->ExecuteJavaScript(js_event.str(), "", 0);
			}

			// 发送应用（一个应用可能有多个窗口）被激活事件:new CustomEvent('JWebTopAppActive',{detail:{handler:触发此消息的窗口的句柄}})
			void sendAppActive(const CefRefPtr<CefFrame> frame, const long handler){
				stringstream js_event;
				js_event << "var e = new CustomEvent('JWebTopAppActive',{"
					<< "	detail:{"
					<< "		handler:" << handler
					<< "	}"
					<< "});"
					<< "dispatchEvent(e);";
				frame->ExecuteJavaScript(js_event.str(), "", 0);
			}
		}// End ns-events
	}// End ns-js
}// End ns-jw