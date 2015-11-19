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
extern CefSettings settings;        // CEF全局设置
namespace jw{

	DWORD			 mainThreadId;	// 记录下主线程的id，以便最后发送退出消息
	JWebTopConfigs * g_configs;		// 应用启动时的第一个配置变量

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
			browser_list_.push_back(browser);// 记录下已经创建的窗口来
		}

		void removeBrowser(CefRefPtr<CefBrowser> browser) {
#ifdef JWebTopLog
			wstringstream log;
			log << L"JWebTopClient::OnBeforeClose 尚存[" << browser_list_.size() << L"]个,"
				<< L"正在关闭的URL地址：" << browser->GetMainFrame()->GetURL().ToWString().c_str()
				<< L"\r\n";
			writeLog(log.str());
#endif
			// Remove from the list of existing browsers.
			BrowserList::iterator bit = browser_list_.begin();
			for (; bit != browser_list_.end(); ++bit) {
				if ((*bit)->IsSame(browser)) {
					browser_list_.erase(bit);
					break;
				}
			}
			if (!jw::dllex::ex() && browser_list_.empty()) {
				// All browser windows have closed. Quit the application message loop.
				CefQuitMessageLoop();
			}
		}

		void CloseAllBrowsers(bool force_close) {
#ifdef JWebTopLog
			wstringstream log;
			log << L"关闭全部浏览器，目前有[" << browser_list_.size() << L"]个\r\n";
			writeLog(log.str());
#endif
			if (browser_list_.empty()){
				if (!jw::dllex::ex()){
					//PostQuitMessage(0);						// 发送此消息不管用
					PostAppMessage(mainThreadId, WM_QUIT, 0, 0);// 可以退出
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
			// 对CEF进行一些设置
			settings.single_process = tmpConfigs->single_process;                      // 是否使用单进程模式：JWebTop默认使用。CEF默认不使用单进程模式
			CefString(&settings.user_data_path) = tmpConfigs->user_data_path;          // 用户数据保存目录
			CefString(&settings.cache_path) = tmpConfigs->cache_path;                  // 浏览器缓存数据的保存目录
			settings.persist_session_cookies = tmpConfigs->persist_session_cookies;    // 是否需要持久化用户cookie数据（若要设置为true，需要同时指定cache_path）
			CefString(&settings.user_agent) = tmpConfigs->user_agent;                  // HTTP请求中的user_agent,CEF默认是Chorminum的user agent
			CefString(&settings.locale) = tmpConfigs->locale;                          // CEF默认是en-US
			settings.log_severity = cef_log_severity_t(tmpConfigs->log_severity);      // 指定日志输出级别，默认不输出(disable),其他取值：verbose,info,warning,error,error-report
			CefString(&settings.log_file) = tmpConfigs->log_file;                      // 指定调试时的日志文件，默认为"debug.log"。如果关闭日志，则不输出日志
			CefString(&settings.resources_dir_path) = tmpConfigs->resources_dir_path;  // 指定cef资源文件（ cef.pak、devtools_resources.pak）的目录，默认从程序运行目录取
			CefString(&settings.locales_dir_path) = tmpConfigs->locales_dir_path;      // 指定cef本地化资源(locales)目录，默认去程序运行目录下的locales目录
			settings.ignore_certificate_errors = tmpConfigs->ignore_certificate_errors;// 是否忽略SSL证书错误
			settings.remote_debugging_port = tmpConfigs->remote_debugging_port;        // 远程调试端口，取值范围[1024-65535]
#ifdef JWebTopLog
			// settings.single_process = 1;// 测试时，采用单进程
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
			CefRefPtr<JWebTopApp> app(new JWebTopApp);// 创建用于监听的顶级程序，通过此app的OnContextInitialized创建浏览器实例

			CefMainArgs main_args(GetModuleHandle(NULL));  // 提供CEF命令行参数
			CefInitialize(main_args, settings, app.get(), sandbox_info);// 初始化cef
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
				return;	// 子进程在这里完成，并返回							
			}
			CefRunMessageLoop();// 运行CEF消息监听，直到CefQuitMessageLoop()方法被调用
			closeJWebtopContext();
		}

		// 根据文件来配置和启动JWebTop
		void startJWebTopByFile(std::wstring cfgFile){
			startJWebTopByCfg(JWebTopConfigs::loadConfigs(JWebTopConfigs::getAppDefFile(LPTSTR(cfgFile.c_str()))));
		}
		void closeJWebtopContext(){
#ifdef JWebTopLog 
			writeLog(L"已退出消息循环，执行最后的清理操作\r\n");
#endif
			CefShutdown();      // 关闭CEF
		}
	}// End ctx namespace
}// End jw namespace
