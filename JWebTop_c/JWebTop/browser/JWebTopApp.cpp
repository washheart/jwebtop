#include "JWebTopApp.h"
#include "JWebTopScheme.h"
#include "JWebTopCommons.h"
#include "JWebTopContext.h"
#include "JWebTop/config/JWebTopConfigs.h"
#include "JWebTop/dllex/JWebTop_DLLEx.h"

namespace jw{
	extern JWebTopConfigs * g_configs;  // 应用启动时的第一个配置变量
}
void JWebTopApp::OnBeforeCommandLineProcessing(
	const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line) {
	command_line->AppendSwitch("--disable-gpu");// 禁止gpu（开启gpu会增加一个新进程，而且在有的电脑上反而会降低性能）
	//command_line->AppendSwitch("--allow-file-access-from-files");
	command_line->AppendSwitch("--disable-web-security");
	//command_line->AppendSwitch("--allow-file-access");
	command_line->AppendSwitch("--enable-npapi");// 运行npapi，主要是支持flash
	// 下面ppapi方式注册的flash，无法正确的获取flash的name
	//command_line->AppendSwitch("--ppapi-out-of-process ");
	//command_line->AppendSwitchWithValue("--register-pepper-plugins", "pepflashplayer.dll;application/x-shockwave-flash");
	if (!jw::g_configs->proxyServer.empty()){
		command_line->AppendSwitchWithValue("proxy-server", jw::g_configs->proxyServer);// 设置代理服务器（也可以）
	}
}

void JWebTopApp::OnContextInitialized() {
	jw::jb::RegisterSchemeHandlers();// 注册jwebtop://协议到浏览器
	if (jw::dllex::ex()){// 如果是从dll调用，不创建浏览器
		jw::dllex::OnContextInitialized();
		//createNewBrowser(jw::ctx::getDefaultConfigs());
	}
	else{// 如果不是从dll调用，那么直接创建浏览器
		createNewBrowser(jw::ctx::getDefaultConfigs());
	}
}