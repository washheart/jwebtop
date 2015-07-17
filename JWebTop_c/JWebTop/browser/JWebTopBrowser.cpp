#include <string>

#include "JWebTopBrowser.h"
#include "JWebTopCommons.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"

void JWebTopBrowser::OnBeforeCommandLineProcessing(
	const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line) {
	command_line->AppendSwitch("--disable-gpu");// 禁止gpu（开启gpu会增加一个新进程，而且在有的电脑上反而会降低性能）
}
void JWebTopBrowser::OnContextInitialized() {
	createNewBrowser((LPCTSTR)NULL);
}