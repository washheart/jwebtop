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
#include "common/util/StrUtil.h"
#include "browser/JWebTopContext.h"
#include "browser/JWebTopCommons.h"
#include "dllex/JWebTop_DLLEx.h"
#if defined(CEF_USE_SANDBOX)
// The cef_sandbox.lib static library is currently built with VS2013. It may not
// link successfully with other VS versions.
#pragma comment(lib, "cef_sandbox.lib")
#endif

CefSettings settings;                      // CEFȫ������
// Ӧ�ó������
int startJWebTop(HINSTANCE hInstance/*��ǰӦ�õ�ʵ��*/, LPTSTR lpCmdLine) {
	//MessageBox(NULL, L"���ڸ��ӽ��̵��ж�", L"�ж�", 0);
	if ((lpCmdLine[0] == ':')){
		int argc = 0;
		LPTSTR * args = CommandLineToArgvW(lpCmdLine, &argc);
		if (argc > 4){// ��dll�����Ĵ���
			jw::dllex::setAsEx();
			DWORD	processId = jw::str::parseLong(args[1]);
			DWORD	blockSize = jw::str::parseLong(args[2]);
			wstring serverName = (args[3]);
			int r = jw::dllex::startIPCServer(serverName, blockSize, processId);
			if (r != 0)return r;
			jw::ctx::startJWebTopByFile(args[4]);
			return 0;
		}
		MessageBox(NULL, L"�޷�������������̣��������ԣ���", L"��Ϣ", 0); return -1;
	}
	else{
		std::wstring cfgFile(lpCmdLine);
		jw::str::replace_allW(cfgFile, L"\"", L"");
		jw::ctx::startJWebTopByFile(cfgFile);
	}
	return 0;
}

// Ӧ�ó������
int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow) {
	SetProcessDPIAware();// ����dpi���������ڰ汾����������api˵�����ᵽ�����ܻᵼ��δ֪���
	//SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);// win8.1��sdk���д�api
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	startJWebTop(hInstance, lpCmdLine);
	return 0;
}