#include "JWebTopApp.h"
#include "JWebTopCommons.h"

extern JWebTopConfigs * tmpConfigs;
void JWebTopApp::OnBeforeCommandLineProcessing(
	const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line) {
	command_line->AppendSwitch("--disable-gpu");// 禁止gpu（开启gpu会增加一个新进程，而且在有的电脑上反而会降低性能）
}
void JWebTopApp::OnContextInitialized() {
	createNewBrowser(tmpConfigs);
}