#include "JWebTopApp.h"
#include "JWebTopCommons.h"
#include "JWebTopContext.h"
#include "JWebTop/dllex/JWebTop_DLLEx.h"

void JWebTopApp::OnBeforeCommandLineProcessing(
	const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line) {
	command_line->AppendSwitch("--disable-gpu");// 禁止gpu（开启gpu会增加一个新进程，而且在有的电脑上反而会降低性能）
}

void JWebTopApp::OnContextInitialized() {
	if (jw::dllex::ex()){// 如果是从dll调用，不创建浏览器
		//if (!g_configs->url.empty())createNewBrowser(g_configs);
	}
	else{// 如果不是从dll调用，那么直接创建浏览器
		createNewBrowser(jw::ctx::getDefaultConfigs());
	}
}