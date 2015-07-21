// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "JWebTopHandler.h"

#include <sstream>
#include <string>

#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

#include "JWebTop\winctrl\JWebTopWinCtrl.h"
#include "JWebTop\winctrl\JWebTopConfigs.h"
#include "JWebTop/tests/TestUtil.h"
using namespace std;

JWebTopHandler::JWebTopHandler()
    : is_closing_(false) {
}

JWebTopHandler::~JWebTopHandler() {
}

extern HANDLE hFile;
extern DWORD filePos;

// 临时记录窗口配置信息，用于在JWebTopBrowser和JWebTopHandler传递参数，（因为JWebTopHandler是全局唯一实例）使用后置空
extern JWebTopConfigs  tmpConfigs;
void JWebTopHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	browser_list_.push_back(browser);// 记录下已经创建的窗口来
	renderBrowserWindow(browser, tmpConfigs);
}

bool JWebTopHandler::DoClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  // Closing the main window requires special handling. See the DoClose()
  // documentation in the CEF header for a detailed destription of this
  // process.
  if (browser_list_.size() == 1) {
    // Set a flag to indicate that the window close should be allowed.
    is_closing_ = true;
  }

  // Allow the close. For windowed browsers this will result in the OS close
  // event being sent.
  return false;
}

void JWebTopHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  // Remove from the list of existing browsers.
  BrowserList::iterator bit = browser_list_.begin();
  for (; bit != browser_list_.end(); ++bit) {
    if ((*bit)->IsSame(browser)) {
      browser_list_.erase(bit);
      break;
    }
  }
  if (browser_list_.empty()) {// 如果浏览器列表已经为空，那么退出浏览器的消息循环
    CefQuitMessageLoop();
  }
}

void JWebTopHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                ErrorCode errorCode,
                                const CefString& errorText,
                                const CefString& failedUrl) {
  CEF_REQUIRE_UI_THREAD();

  // Don't display an error for downloaded files.
  if (errorCode == ERR_ABORTED)
    return;

  // Display a load error message.
  std::stringstream ss;
  ss << "<html><body bgcolor=\"white\">"
        "<h2>Failed to load URL " << std::string(failedUrl) <<
        " with error " << std::string(errorText) << " (" << errorCode <<
        ").</h2></body></html>";
  frame->LoadString(ss.str(), failedUrl);
}

void JWebTopHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	int httpStatusCode){
	stringstream extensionCode;
	CefRefPtr<CefBrowserHost> host = browser->GetHost();
	HWND hwnd = host->GetWindowHandle();
	extensionCode << "if(JWebTop)JWebTop.handler=0x" << hwnd << ";";
	extensionCode << "var e = new CustomEvent('AlloyDesktopReady');"<<"setTimeout('dispatchEvent(e);',0);" << endl;
	extensionCode << "var handler=0x" << hwnd << ";";
	//frame->ExecuteJavaScript(ss.str(), "", 0);

#ifdef JWebTopLog
	writeLog(L"在OnContextCreated方法中执行JS\r\n\t");
	writeLog(extensionCode.str());
#endif
	frame->ExecuteJavaScript(CefString(extensionCode.str()), "", 0);
}
void JWebTopHandler::CloseAllBrowsers(bool force_close) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute on the UI thread.
    CefPostTask(TID_UI,
        base::Bind(&JWebTopHandler::CloseAllBrowsers, this, force_close));
    return;
  }

  if (browser_list_.empty())
    return;

  BrowserList::const_iterator it = browser_list_.begin();
  for (; it != browser_list_.end(); ++it)
    (*it)->GetHost()->CloseBrowser(force_close);
}