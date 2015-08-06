// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <windows.h>

#include "browser/JWebTopApp.h"
#include "include/cef_sandbox_win.h"
#include "JWebTop/winctrl/JWebTopConfigs.h"
#ifdef JWebTopLog
#include "JWebTop/tests/TestUtil.h"
#endif


#define IDC_CEFCLIENT                   109

#if defined(CEF_USE_SANDBOX)
// The cef_sandbox.lib static library is currently built with VS2013. It may not
// link successfully with other VS versions.
#pragma comment(lib, "cef_sandbox.lib")
#endif

bool g_single_process;
HINSTANCE g_instance;
JWebTopConfigs * g_configs;  // 应用启动时的第一个配置变量
JWebTopConfigs * tmpConfigs; // 创建过程中在多个上下文中共享的变量

// 应用程序入口
int startJWebTop(HINSTANCE hInstance/*当前应用的实例*/, LPTSTR appDefFile, long parentWin
	// 以下参数会替换appfile中的相应参数
	, LPTSTR url       // 要打开的链接地址
	, LPTSTR title     // 窗口名称
	, LPTSTR icon      // 窗口图标
	, int x, int y     // 窗口左上角坐标,当值为-1时不启用此变量		 
	, int w, int h     // 窗口的宽、高，当值为-1时不启用此变量	
	) {
	g_instance = hInstance;
	#ifdef JWebTopJNI
		CefMainArgs main_args(::GetModuleHandle(NULL));
	#else
		CefMainArgs main_args(g_instance); // 提供CEF命令行参数
	#endif
	CefSettings settings;             // CEF全局设置
	// 读取程序配置信息
	tmpConfigs = JWebTopConfigs::loadConfigs(JWebTopConfigs::getAppDefFile(appDefFile));
	if (g_configs == NULL)g_configs = tmpConfigs;
	if (url != NULL){
		tmpConfigs->url = CefString(url);
	}
	if (title != NULL)tmpConfigs->name = CefString(title);
	if (icon != NULL)tmpConfigs->icon = CefString(icon);
	if (x != -1)tmpConfigs->x = x;
	if (y != -1)tmpConfigs->y = y;
	if (w != -1)tmpConfigs->w = w;
	if (h != -1)tmpConfigs->h = h;

	tmpConfigs->parentWin = parentWin;
	// 对CEF进行一些设置
	settings.single_process = tmpConfigs->single_process;                      // 是否使用单进程模式：JWebTop默认使用。CEF默认不使用单进程模式
	CefString(&settings.user_data_path) = tmpConfigs->user_data_path;          // 用户数据保存目录
	CefString(&settings.cache_path) = tmpConfigs->cache_path;                 // 浏览器缓存数据的保存目录
	settings.persist_session_cookies = tmpConfigs->persist_session_cookies;    // 是否需要持久化用户cookie数据（若要设置为true，需要同时指定cache_path）
	CefString(&settings.user_agent) = tmpConfigs->user_agent;                 // HTTP请求中的user_agent,CEF默认是Chorminum的user agent
	CefString(&settings.locale) = tmpConfigs->locale;                          // CEF默认是en-US
	settings.log_severity = cef_log_severity_t(tmpConfigs->log_severity);      // 指定日志输出级别，默认不输出(disable),其他取值：verbose,info,warning,error,error-report
	CefString(&settings.log_file) = tmpConfigs->log_file;                      // 指定调试时的日志文件，默认为"debug.log"。如果关闭日志，则不输出日志
	CefString(&settings.resources_dir_path) = tmpConfigs->resources_dir_path;  // 指定cef资源文件（ cef.pak、devtools_resources.pak）的目录，默认从程序运行目录取
	CefString(&settings.locales_dir_path) = tmpConfigs->locales_dir_path;      // 指定cef本地化资源(locales)目录，默认去程序运行目录下的locales目录
	settings.ignore_certificate_errors = tmpConfigs->ignore_certificate_errors;// 是否忽略SSL证书错误
	settings.remote_debugging_port = tmpConfigs->remote_debugging_port;        // 远程调试端口，取值范围[1024-65535]

	void* sandbox_info = NULL;

#if defined(CEF_USE_SANDBOX)
	// Manage the life span of the sandbox information object. This is necessary
	// for sandbox support on Windows. See cef_sandbox_win.h for complete details.
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#else
	settings.no_sandbox = tmpConfigs->no_sandbox;
#endif	
	settings.multi_threaded_message_loop = 1;
	CefRefPtr<JWebTopApp> app(new JWebTopApp);// 创建用于监听的顶级程序，通过此app的OnContextInitialized创建浏览器实例

	// CEF applications have multiple sub-processes (render, plugin, GPU, etc)
	// that share the same executable. This function checks the command-line and,
	// if this is a sub-process, executes the appropriate logic.
	int exit_code = CefExecuteProcess(main_args, app.get(), sandbox_info);
#ifdef JWebTopLog 
	std::stringstream log;
	log << "CefExecuteProcess exit_code ==" << exit_code << "\r\n";
	writeLog(log.str());
#endif
	if (exit_code >= 0) {
		// The sub-process has completed so return here.
		return exit_code;
	}
	if (settings.multi_threaded_message_loop){// 各自有独立的消息循环
		MSG msg;
		HACCEL	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CEFCLIENT));
		// Run the application message loop.
		while (GetMessage(&msg, NULL, 0, 0)) {
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	else{
		CefInitialize(main_args, settings, app.get(), sandbox_info);// 初始化cef
		CefRunMessageLoop();// 运行CEF消息监听，知道CefQuitMessageLoop()方法被调用
		// 下面的这种是自己侦听消息循环的方式，性能比CefRunMessageLoop()略差，但是好处是能自己控制线程间的锁定（但是依然无法解决java线程与cef消息冲突的问题）
		//while (runable){
		//	CefDoMessageLoopWork();
		//	Sleep(30);
		//}
	}
	CefShutdown();      // 关闭CEF
	return 0;
}

// 应用程序入口
int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	startJWebTop(hInstance, lpCmdLine, 0, NULL, NULL, NULL, -1, -1, -1, -1);
	return 0;
}