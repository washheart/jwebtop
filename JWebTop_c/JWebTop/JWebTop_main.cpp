// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <windows.h>
#include "browser/JWebTopApp.h"
#include "include/cef_sandbox_win.h"
#include "JWebTop/config/JWebTopConfigs.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
#include "common/task/Task.h"
#include "common/msgwin/MsgWin.h"
#include "browser/JWebTopContext.h"
#include "dllex/JWebTop_DLLEx.h"
#if defined(CEF_USE_SANDBOX)
// The cef_sandbox.lib static library is currently built with VS2013. It may not
// link successfully with other VS versions.
#pragma comment(lib, "cef_sandbox.lib")
#endif

extern HWND g_RemoteWinHWnd;               // 远程进程的消息窗口HWND
CefSettings settings;                      // CEF全局设置

// 应用程序入口
int startJWebTop(HINSTANCE hInstance/*当前应用的实例*/, LPTSTR lpCmdLine) {
	//MessageBox(NULL, L"用于附加进程的中断", L"中断", 0);
	CefMainArgs main_args(hInstance);  // 提供CEF命令行参数
	wstring taskId;
	// 读取程序配置信息
	JWebTopConfigs * tmpConfigs = JWebTopConfigs::parseCreateJWebTopCmdLine(lpCmdLine, std::ref(taskId));
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

	//settings.single_process = 1;
	if (g_RemoteWinHWnd != NULL){
		jw::dllex::waitForRemoteProcessTerminate();// 创建一个线程用来监听远程进程是否终止以便结束当前程序
		settings.multi_threaded_message_loop = 1;
	}
//#ifdef JWebTopLog
//	settings.log_severity = LOGSEVERITY_VERBOSE;
//#endif
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
	if (exit_code >= 0) {
		// The sub-process has completed so return here.
		return exit_code;
	}
	if (settings.multi_threaded_message_loop == 1){// 如果是被DLL调用，这种方式只能建立一个进程		
		jw::createWin(hInstance, LPTSTR(taskId.c_str()));       // 创建隐藏窗口并阻塞当前线程
		// 在MsgWin中PostQuitMessage(0)之后，下面的代码根本没有机会执行了？？？进程直接退出
		jw::ctx::CloseAllBrowsers(true);
		CefQuitMessageLoop();
	}
	else{
		CefRunMessageLoop();// 运行CEF消息监听，知道CefQuitMessageLoop()方法被调用
	}
	jw::task::unlockAndClearAll();
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
	startJWebTop(hInstance, lpCmdLine);
	return 0;
}