// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <windows.h>

#include "browser/JWebTopApp.h"
#include "include/cef_sandbox_win.h"

#if defined(CEF_USE_SANDBOX)
// The cef_sandbox.lib static library is currently built with VS2013. It may not
// link successfully with other VS versions.
#pragma comment(lib, "cef_sandbox.lib")
#endif

HINSTANCE g_hInstance;
LPTSTR    g_lpCmdLine;
// 应用程序入口
int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	g_hInstance = hInstance;
	g_lpCmdLine = lpCmdLine;
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

	// Specify CEF global settings here.
	CefSettings settings;

#if !defined(CEF_USE_SANDBOX)
	settings.no_sandbox = true;
#endif
	// 对CEF进行一些设置
	CefString(&settings.cache_path) = L"data/browser"; // 配置缓存目录（包括WebSQL和IndexDB的数据库位置）
	settings.log_severity = LOGSEVERITY_DISABLE;       // 配置日志级别：关闭日志
	settings.single_process = true;				   // 设置后browser和render会合并为一个进程（不稳定？）
	CefInitialize(main_args, settings, app.get(), sandbox_info);// 初始化cef
	// Run the CEF message loop. This will block until CefQuitMessageLoop() is called.
	CefRunMessageLoop();
	// Shut down CEF.
	CefShutdown();
	return 0;
}
