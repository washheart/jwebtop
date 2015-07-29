// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <windows.h>

#include "browser/JWebTopApp.h"
#include "include/cef_sandbox_win.h"
#include "JWebTop/winctrl/JWebTopConfigs.h"

#if defined(CEF_USE_SANDBOX)
// The cef_sandbox.lib static library is currently built with VS2013. It may not
// link successfully with other VS versions.
#pragma comment(lib, "cef_sandbox.lib")
#endif

HINSTANCE g_hInstance;
LPTSTR    g_lpCmdLine;
bool g_single_process;
JWebTopConfigs * g_configs;  // 应用启动时的第一个配置变量
JWebTopConfigs * tmpConfigs; // 创建过程中在多个上下文中共享的变量
// 应用程序入口
int startJWebTop(HINSTANCE hInstance/*当前应用的实例*/,LPTSTR    lpCmdLine) {

	g_hInstance = hInstance;
	g_lpCmdLine = lpCmdLine;
	// Specify CEF global settings here.
	CefSettings settings;
	// 读取程序配置信息
	JWebTopConfigs tmp = JWebTopConfigs::loadConfigs(JWebTopConfigs::getAppDefFile(lpCmdLine));
	tmpConfigs = &tmp;
	if (g_configs == NULL)g_configs = tmpConfigs;
	// 对CEF进行一些设置
	settings.single_process = tmpConfigs->single_process;                      // 是否使用单进程模式：JWebTop默认使用。CEF默认不使用单进程模式
	CefString(&settings.user_data_path) = tmpConfigs->user_data_path; // 用户数据保存目录
	CefString(&settings.cache_path) = tmpConfigs->cache_path ;    // 浏览器缓存数据的保存目录
	settings.persist_session_cookies = tmpConfigs->persist_session_cookies;               // 是否需要持久化用户cookie数据（若要设置为true，需要同时指定cache_path）
	CefString(&settings.user_agent) = tmpConfigs->user_agent ;      // HTTP请求中的user_agent,CEF默认是Chorminum的user agent
	CefString(&settings.locale) = tmpConfigs->locale;                // CEF默认是en-US
	settings.log_severity = cef_log_severity_t(tmpConfigs->log_severity);       // 指定日志输出级别，默认不输出(disable),其他取值：verbose,info,warning,error,error-report
	//settings.log_severity = LOGSEVERITY_DISABLE;       // 配置日志级别：关闭日志
	CefString(&settings.log_file) = tmpConfigs->log_file;                        // 指定调试时的日志文件，默认为"debug.log"。如果关闭日志，则不输出日志
	CefString(&settings.resources_dir_path) = tmpConfigs->resources_dir_path;              // 指定cef资源文件（ cef.pak、devtools_resources.pak）的目录，默认从程序运行目录取
	CefString(&settings.locales_dir_path) = tmpConfigs->locales_dir_path;                // 指定cef本地化资源(locales)目录，默认去程序运行目录下的locales目录
	settings.ignore_certificate_errors = tmpConfigs->ignore_certificate_errors;             // 是否忽略SSL证书错误
	settings.remote_debugging_port = tmpConfigs->remote_debugging_port;                 // 远程调试端口，取值范围[1024-65535]


	//CefString(&settings.cache_path) = L"data/browser"; // 配置缓存目录（包括WebSQL和IndexDB的数据库位置）
	//settings.single_process = tmpConfigs->single_process;				   // 设置后browser和render会合并为一个进程（不稳定？）
	//g_single_process = settings.single_process == 1;

	void* sandbox_info = NULL;

#if defined(CEF_USE_SANDBOX)
	// Manage the life span of the sandbox information object. This is necessary
	// for sandbox support on Windows. See cef_sandbox_win.h for complete details.
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#endif

	// Provide CEF with command-line arguments.
	CefMainArgs main_args(hInstance);

	// SimpleApp implements application-level callbacks. It will create the first
	// browser instance in OnContextInitialized() after CEF has initialized.
	CefRefPtr<JWebTopApp> app(new JWebTopApp);

	// CEF applications have multiple sub-processes (render, plugin, GPU, etc)
	// that share the same executable. This function checks the command-line and,
	// if this is a sub-process, executes the appropriate logic.
	//app->setMainArgs(hInstance);
	int exit_code = CefExecuteProcess(main_args, app.get(), sandbox_info);
	if (exit_code >= 0) {
		// The sub-process has completed so return here.
		return exit_code;
	}


#if !defined(CEF_USE_SANDBOX)
	settings.no_sandbox = tmpConfigs->no_sandbox;
#endif	
	CefInitialize(main_args, settings, app.get(), sandbox_info);// 初始化cef
	// Run the CEF message loop. This will block until CefQuitMessageLoop() is called.
	CefRunMessageLoop();
	// Shut down CEF.
	CefShutdown();
	return 0;
}

// 应用程序入口
int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	startJWebTop(hInstance, lpCmdLine);
	return 0;
}