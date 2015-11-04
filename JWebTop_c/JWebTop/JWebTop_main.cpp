// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#include <Windows.h>
#include <ShellAPI.h> 
#include "browser/JWebTopApp.h"
#include "include/cef_sandbox_win.h"
#include "JWebTop/config/JWebTopConfigs.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
#include "common/task/Task.h"
#include "common/util/StrUtil.h"
#include "browser/JWebTopContext.h"
#include "browser/JWebTopCommons.h"
#include "dllex/JWebTop_DLLEx.h"
#if defined(CEF_USE_SANDBOX)
// The cef_sandbox.lib static library is currently built with VS2013. It may not
// link successfully with other VS versions.
#pragma comment(lib, "cef_sandbox.lib")
#endif

CefSettings settings;                      // CEF全局设置
// 应用程序入口
int startJWebTop(HINSTANCE hInstance/*当前应用的实例*/, LPTSTR lpCmdLine) {
	MessageBox(NULL, L"用于附加进程的中断", L"中断", 0);
	if ((lpCmdLine[0] == '\1')){
		int argc = 0;
		LPTSTR * args = CommandLineToArgvW(lpCmdLine, &argc);
		if (argc > 3){// 从dll发来的处理
			jw::dllex::setAsEx();
			DWORD	processId = jw::parseLong(args[1]);
			DWORD	blockSize = jw::parseLong(args[2]);
			wstring serverName = (args[3]);
			jw::dllex::startIPCServer(serverName, blockSize, processId);
			return 0;
		}
		MessageBox(NULL, L"创建IPC时必须在启动参数上指定服务端的名称！", L"消息", 0); return -1;
	}
	else{
		jw::ctx::startJWebTopByFile(lpCmdLine);
	}
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