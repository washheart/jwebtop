#include "JWebTopContext.h"

#include <Windows.h>
#include <sstream>
#include <string>
#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
#include "JWebTopApp.h"
#include "JWebTopCommons.h"
#include "JWebTop/dllex/JWebTop_DLLEx.h"

using namespace std;
extern CefSettings settings;        // CEFȫ������
namespace jw{

	DWORD			 mainThreadId;	// ��¼�����̵߳�id���Ա�������˳���Ϣ
	JWebTopConfigs * g_configs;		// Ӧ������ʱ�ĵ�һ�����ñ���

	namespace ctx{
		BrowserList browser_list_;

		JWebTopConfigs * getDefaultConfigs(){
			return g_configs;
		}
		void setDefaultConfigs(JWebTopConfigs * configs){
			g_configs = configs;
		}

		int getBrowserCount(){
			return browser_list_.size();
		}
		void addBrowser(CefRefPtr<CefBrowser> browser) {
			browser_list_.push_back(browser);// ��¼���Ѿ������Ĵ�����
		}

		bool removeBrowser(CefRefPtr<CefBrowser> browser) {
#ifdef JWebTopLog
			wstringstream log;
			log << L"JWebTopContext::removeBrowser �д�[" << browser_list_.size() << L"]��,"
				<< L"���ڹرյ�URL��ַ��" << browser->GetMainFrame()->GetURL().ToWString().c_str()
				<< L"\r\n";
			writeLog(log.str());
#endif
			// Remove from the list of existing browsers.
			bool success = false;
			BrowserList::iterator bit = browser_list_.begin();
			for (; bit != browser_list_.end(); ++bit) {
				if ((*bit)->IsSame(browser)) {
					browser_list_.erase(bit);
					success = true;
					break;
				}
			}
			if (!jw::dllex::ex() && browser_list_.empty()) {
				// All browser windows have closed. Quit the application message loop.
				CefQuitMessageLoop();
			}
			return success;
		}

		void CloseAllBrowsers(bool force_close) {
#ifdef JWebTopLog
			wstringstream log;
			log << L"�ر�ȫ���������Ŀǰ��[" << browser_list_.size() << L"]��\r\n";
			writeLog(log.str());
#endif
			if (browser_list_.empty()){
				if (!jw::dllex::ex()){
					//PostQuitMessage(0);						// ���ʹ���Ϣ������
					PostAppMessage(mainThreadId, WM_QUIT, 0, 0);// �����˳�
					CefQuitMessageLoop();
				}
				return;
			}
			BrowserList::const_iterator it = browser_list_.begin();
			for (; it != browser_list_.end(); ++it)
				(*it)->GetHost()->CloseBrowser(force_close);
		}
		void startJWebTopByCfg(JWebTopConfigs * tmpConfigs){
			mainThreadId = GetCurrentThreadId();
			jw::ctx::setDefaultConfigs(tmpConfigs);
			// ��CEF����һЩ����
			settings.single_process = tmpConfigs->single_process;                      // �Ƿ�ʹ�õ�����ģʽ��JWebTopĬ��ʹ�á�CEFĬ�ϲ�ʹ�õ�����ģʽ
			CefString(&settings.user_data_path) = tmpConfigs->user_data_path;          // �û����ݱ���Ŀ¼
			CefString(&settings.cache_path) = tmpConfigs->cache_path;                  // ������������ݵı���Ŀ¼
			settings.persist_session_cookies = tmpConfigs->persist_session_cookies;    // �Ƿ���Ҫ�־û��û�cookie���ݣ���Ҫ����Ϊtrue����Ҫͬʱָ��cache_path��
			CefString(&settings.user_agent) = tmpConfigs->user_agent;                  // HTTP�����е�user_agent,CEFĬ����Chorminum��user agent
			CefString(&settings.locale) = tmpConfigs->locale;                          // CEFĬ����en-US
			settings.log_severity = cef_log_severity_t(tmpConfigs->log_severity);      // ָ����־�������Ĭ�ϲ����(disable),����ȡֵ��verbose,info,warning,error,error-report
			CefString(&settings.log_file) = tmpConfigs->log_file;                      // ָ������ʱ����־�ļ���Ĭ��Ϊ"debug.log"������ر���־���������־
			CefString(&settings.resources_dir_path) = tmpConfigs->resources_dir_path;  // ָ��cef��Դ�ļ��� cef.pak��devtools_resources.pak����Ŀ¼��Ĭ�ϴӳ�������Ŀ¼ȡ
			CefString(&settings.locales_dir_path) = tmpConfigs->locales_dir_path;      // ָ��cef���ػ���Դ(locales)Ŀ¼��Ĭ��ȥ��������Ŀ¼�µ�localesĿ¼
			settings.ignore_certificate_errors = tmpConfigs->ignore_certificate_errors;// �Ƿ����SSL֤�����
			settings.remote_debugging_port = tmpConfigs->remote_debugging_port;        // Զ�̵��Զ˿ڣ�ȡֵ��Χ[1024-65535]
			CefString(&settings.accept_language_list) = L"zh-CN,cn;q=0.8";
#ifdef JWebTopLog
			 //settings.single_process = 1;// ����ʱ�����õ�����
			settings.log_severity = LOGSEVERITY_VERBOSE;
#endif
			void* sandbox_info = NULL;

#if defined(CEF_USE_SANDBOX)
			// Manage the life span of the sandbox information object. This is necessary
			// for sandbox support on Windows. See cef_sandbox_win.h for complete details.
			CefScopedSandboxInfo scoped_sandbox;
			sandbox_info = scoped_sandbox.sandbox_info();
#else
			settings.no_sandbox = tmpConfigs->no_sandbox;
#endif	
			CefRefPtr<JWebTopApp> app(new JWebTopApp);// �������ڼ����Ķ�������ͨ����app��OnContextInitialized���������ʵ��

			CefMainArgs main_args(GetModuleHandle(NULL));  // �ṩCEF�����в���
			// CEF applications have multiple sub-processes (render, plugin, GPU, etc)
			// that share the same executable. This function checks the command-line and,
			// if this is a sub-process, executes the appropriate logic.
			int exit_code = CefExecuteProcess(main_args, app.get(), sandbox_info);
#ifdef JWebTopLog 
			std::stringstream log;
			log << "CefExecuteProcess exit_code ==" << exit_code << "\r\n";
			writeLog(log.str());
#endif
			if (exit_code >= 0){
#ifdef JWebTopLog 
				if (settings.single_process != 1)setLogFileName(L"jwebtop_render.log");
#endif
				return;	// �ӽ�����������ɣ�������							
			}
			CefInitialize(main_args, settings, app.get(), sandbox_info);// ��ʼ��cef
			CefRunMessageLoop();// ����CEF��Ϣ������ֱ��CefQuitMessageLoop()����������
			closeJWebtopContext();
		}

		// �����ļ������ú�����JWebTop
		void startJWebTopByFile(std::wstring cfgFile){
			startJWebTopByCfg(JWebTopConfigs::loadConfigs(JWebTopConfigs::getAppDefFile(LPTSTR(cfgFile.c_str()))));
		}
		void closeJWebtopContext(){
#ifdef JWebTopLog 
			writeLog(L"���˳���Ϣѭ����ִ�������������\r\n");
#endif
			CefShutdown();      // �ر�CEF
		}
	}// End ctx namespace
}// End jw namespace
