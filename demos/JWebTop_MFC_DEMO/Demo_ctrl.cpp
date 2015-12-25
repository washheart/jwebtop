#include "Demo.h"
#include "common/fastipc/Server.h"
#include "common/fastipc/Client.h"
#include "common/util/StrUtil.h"
#include "common/os/OS.h"
#include <thread>
#include <string>
#include <sstream>
#include "jwebtopex/jwebtopex.h"

extern std::wstring serverName;	// 服务端名称
extern fastipc::Server server;  // IPC服务端
extern DWORD blockSize;			// 一次传输时的数据大小

// 得到配置文件
wstring DemoCtrl::getListAppFile(){
	LPTSTR path = L"res/list/index.app";
	if (_waccess(path, 0) != 0){// 按开发环境来处理
		path = L"../../res/list/index.app";
	}
	wchar_t fullpath[1024];
	_wfullpath(fullpath, path, 1024);
	wstring rtn(fullpath);
	delete[] fullpath;
	return rtn;
}	

// 得到配置文件
wstring DemoCtrl::getDetailAppFile(){
	LPTSTR path = L"res/deatil/index.app";
	if (_waccess(path, 0) != 0){// 按开发环境来处理
		path = L"../../res/deatil/index.app";
	}
	wchar_t fullpath[1024];
	_wfullpath(fullpath, path, 1024);
	wstring rtn(fullpath);
	delete[]fullpath;
	return rtn;
}
// 得到JWebTop.exe的路径
wstring DemoCtrl::getJWebTopExe(){
	return L"JWebTop.exe";
}

void DemoCtrl::_startJWebTop(){
	wstring appFile = this->getListAppFile();
	ctx->createJWebTopByCfgFile(this->getJWebTopExe(), appFile, this);
}

void DemoCtrl::init(){
	if (ctx != NULL)return;
	ctx = new JWebTopContext();
	thread th(std::mem_fn(&DemoCtrl::_startJWebTop), this);
	th.detach(); //分离线程  
}

void DemoCtrl::onJWebTopAppInited(){
	wstring appFile = this->getListAppFile();
	JWebTopConfigs * configs = new JWebTopConfigs();
	configs->appDefFile = appFile;
	configs->name = L"列表页";
	ctx->createBrowser(configs, this);
}

void DemoCtrl::onJWebTopBrowserCreated(long browserHWnd){
	this->listBrowser = (HWND)browserHWnd;
}

//DWORD startJWebTopProcess(std::wstring clientExe, std::wstring serverName, DWORD blockSize){
//	STARTUPINFO sui;
//	ZeroMemory(&sui, sizeof(STARTUPINFO));
//	sui.cb = sizeof(STARTUPINFO);
//	PROCESS_INFORMATION pi; // 保存了所创建子进程的信息
//	DWORD dwCreationFlags = CREATE_NEW_PROCESS_GROUP;
//	// 生成命令行
//	std::wstringstream wss; wss << clientExe << L" " << serverName << L" " << blockSize;
//	if (CreateProcess(NULL, LPTSTR(wss.str().c_str()), // 第一个参数置空，可执行文件和命令行放到一起避免恶意程序问题
//		NULL, NULL, FALSE, dwCreationFlags, NULL, NULL, &sui, &pi)){
//		CloseHandle(pi.hProcess);
//		CloseHandle(pi.hThread);
//		return pi.dwThreadId;
//	}
//	return 0;
//}
//
//
//int startJWebTop(){
//	std::wstring clientExee = L"JWebTop.exe";// 浏览器进程的路径
//	if (_waccess(LPTSTR(clientExee.c_str()), 0) != 0){
//		serverName.append(L"文件找不到！");
//		MessageBox(NULL, LPTSTR(serverName.c_str()), L"消息", 0);// 如果文件不存在
//		return 2;
//	} else{
//		if (startJWebTopProcess(clientExee, serverName, blockSize) == 0){// 创建子进程，失败后返回
//			return 3;
//		}
//	}
//	return 0;
//}
//
//int initJWebTop(){
//	blockSize = 2048;// 
//	blockSize = 2000;// 指定一个缓存区空间，用于和浏览器进程交互数据
//	int r = startIPCServer();
//	if (r != 0)return r;// 第一步启动主进程的FastIPCServer
//	r = startJWebTop();
//	if (r != 0){
//		server.close();
//		return r;
//	}
//	return 0;
//}