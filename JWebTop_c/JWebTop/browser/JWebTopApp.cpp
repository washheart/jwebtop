#include "JWebTopApp.h"
#include "JWebTopScheme.h"
#include "JWebTopCommons.h"
#include "JWebTopContext.h"
#include "JWebTop/config/JWebTopConfigs.h"
#include "JWebTop/dllex/JWebTop_DLLEx.h"
namespace jw{
	extern JWebTopConfigs * g_configs;  // Ӧ������ʱ�ĵ�һ�����ñ���
}
void JWebTopApp::OnBeforeCommandLineProcessing(
	const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line) {
	command_line->AppendSwitch("--disable-gpu");// ��ֹgpu������gpu������һ���½��̣��������еĵ����Ϸ����ή�����ܣ�
	command_line->AppendSwitch("--allow-file-access-from-files");
	command_line->AppendSwitch("--disable-web-security");
	command_line->AppendSwitch("--allow-file-access");
	command_line->AppendSwitch("--enable-npapi");// ����npapi����Ҫ��֧��flash

	//command_line->AppendSwitch("--enable-use-zoom-for-dsf");// zoom����
	command_line->AppendSwitchWithValue("high-dpi-support", "0");
	command_line->AppendSwitchWithValue("force-device-scale-factor", "1");
	// ����ppapi��ʽע���flash���޷���ȷ�Ļ�ȡflash��name
	// command_line->AppendSwitch("--ppapi-out-of-process ");
	// command_line->AppendSwitchWithValue("--register-pepper-plugins", "pepflashplayer.dll;application/x-shockwave-flash");
	if (!jw::g_configs->proxyServer.empty()){
		command_line->AppendSwitchWithValue("proxy-server", jw::g_configs->proxyServer);// ���ô����������Ҳ���ԣ�
	}else{
		command_line->AppendSwitchWithValue("no-proxy-server","1");//  �رմ���(���Լ��ٴ����⣬�ӿ첿���ٶ�)
	}
}

void JWebTopApp::OnContextInitialized() {
	jw::jb::RegisterSchemeHandlers();// ע��jwebtop://Э�鵽�����
	if (jw::dllex::ex()){// ����Ǵ�dll���ã������������
		jw::dllex::OnContextInitialized();
		//createNewBrowser(jw::ctx::getDefaultConfigs());
	} else {// ������Ǵ�dll���ã���ôֱ�Ӵ��������
		createNewBrowser(jw::ctx::getDefaultConfigs());
	}
}