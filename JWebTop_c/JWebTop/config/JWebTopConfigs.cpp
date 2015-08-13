#include "JWebTopConfigs.h"

#include <Windows.h>
#include <ShellAPI.h> 
#include <iostream> 
#include <sstream> 
#include <strsafe.h>
#include "common/util/StrUtil.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
using namespace std;

// 根据文件名获取问路径
wstring GetFilePath(wstring appDefFile){
	const int BUFSIZE = 4096;
	TCHAR  buffer[BUFSIZE] = TEXT("");
	GetFullPathName(LPCWSTR(appDefFile.data()), BUFSIZE, buffer, NULL);// 获取文件的绝对路径
	wstring pp(buffer);
	int p = pp.find_last_of('\\');
	if (p != -1)pp = pp.substr(0, p + 1);
	return pp;
}
CefString JWebTopConfigs::getAbsolutePath(std::wstring relativePath){
	if (relativePath.find(L":") == -1){// 如果指定的路径是相对路径
		std::wstring _path(appPath);
		relativePath = _path.append(relativePath);
	}
	return CefString(relativePath);
}

// 得到程序启动目录
std::wstring JWebTopConfigs::GetExePath(){
	TCHAR   szPath[1000];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	std::wstring path(szPath);
	path = path.substr(0, path.find_last_of('\\') + 1);
	return path;
}

// 根据传入的参数获取配置文件路径(目前主要用于处理传入参数为NULL或空字符串的情况)
std::wstring JWebTopConfigs::getAppDefFile(LPCTSTR lpCmdLine){
	if (lpCmdLine == NULL || lstrlen(lpCmdLine) == 0){// 未指定启动文件，检查下是否通过启动参数指定了appDefFile
		return GetExePath() + L"index.app";// 取程序所在目录下的index.app文件;
	}
	else{// 取启动参数作为appDefFile路径
		std::wstring tmp(lpCmdLine);
		if (tmp.find(L":") == -1){// 如果指定的路径是相对路径
			return GetExePath() + lpCmdLine;
		}
		return wstring(lpCmdLine);
	}
}

JWebTopConfigs * JWebTopConfigs::loadConfigs(std::wstring appDefFile){
#ifdef JWebTopLog
	writeLog(L"appDefFile=====" + appDefFile + L"\r\n");
#endif
	JWebTopConfigs * configs =new JWebTopConfigs();
	if (appDefFile.size() == 0)return configs;                                                   // 如果未指定配置文件（null，空指针） 
	LPCTSTR path = appDefFile.data();
	if (_waccess(path, 0) != 0)return configs;                                                   // 如果文件不存在
	configs->appDefFile = CefString(appDefFile);
	configs->appPath = GetFilePath(appDefFile);
	TCHAR url[1000], name[100], iconPath[1000];
	GetPrivateProfileString(L"BASE", L"url", NULL, url, 1000, appDefFile.data());
	configs->url = CefString(url);                                                                // 需要打开的地址 
	GetPrivateProfileString(L"BASE", L"name", NULL, name, 100, appDefFile.data());
	configs->name = CefString(name);                                                              // 设置浏览器窗口名称 
	GetPrivateProfileString(L"BASE", L"icon", NULL, iconPath, 1000, appDefFile.data());
	configs->icon = CefString(iconPath);                                                          // 设置浏览器图标 
	// 其实这里可以先通过获取字符串再转换为int的方式来判断
	configs->w = GetPrivateProfileInt(L"BASE", L"width", -1, appDefFile.data());
	configs->h = GetPrivateProfileInt(L"BASE", L"height", -1, appDefFile.data());
	configs->x = GetPrivateProfileInt(L"BASE", L"x", -1, appDefFile.data());
	configs->y = GetPrivateProfileInt(L"BASE", L"y", -1, appDefFile.data());
	configs->enableDrag = 1 == GetPrivateProfileInt(L"BASE", L"enableDrag", 1, appDefFile.data());        // 默认可以通过页面进行拖动
	configs->disableRefresh = 0 == GetPrivateProfileInt(L"BASE", L"disableRefresh", 0, appDefFile.data());// 默认禁止刷新
	configs->enableDebug = 0 == GetPrivateProfileInt(L"BASE", L"disableDevelop", 1, appDefFile.data());   // 默认不可以调试
	configs->enableResize = 1 == GetPrivateProfileInt(L"BASE", L"enableResize", 1, appDefFile.data());    // 默认可以改变大小
	//disableTransparent = 1==GetPrivateProfileInt(L"BASE", L"disableTransparent", 0, appDefFile.data());
	//hasBorder = 1==GetPrivateProfileInt(L"BASE", L"hasBorder", 0, appDefFile.data());
	configs->max = GetPrivateProfileInt(L"BASE", L"max", 0, appDefFile.data());
	configs->dwStyle = GetPrivateProfileInt(L"BASE", L"style", 0, appDefFile.data());
	configs->dwExStyle = GetPrivateProfileInt(L"BASE", L"exStyle", 0, appDefFile.data());

	// cef
	configs->single_process = GetPrivateProfileInt(L"CEF", L"single_process", configs->single_process, appDefFile.data());             // 是否使用单进程模式：JWebTop默认使用。CEF默认不使用单进程模式
	TCHAR user_data_path_[1000];
	GetPrivateProfileString(L"CEF", L"user_data_path", configs->user_data_path.ToWString().c_str(), user_data_path_, 1000, appDefFile.data());
	configs->user_data_path = CefString(user_data_path_); // 用户数据保存目录
	TCHAR cache_path_[1000];
	GetPrivateProfileString(L"CEF", L"cache_path", configs->cache_path.ToWString().c_str(), cache_path_, 1000, appDefFile.data());
	configs->cache_path = CefString(cache_path_); // 浏览器缓存数据的保存目录
	configs->persist_session_cookies = GetPrivateProfileInt(L"CEF", L"single_process", configs->persist_session_cookies, appDefFile.data());             // 是否需要持久化用户cookie数据（若要设置为true，需要同时指定cache_path）
	TCHAR user_agent_[1000];
	GetPrivateProfileString(L"CEF", L"user_agent", configs->user_agent.ToWString().c_str(), user_agent_, 1000, appDefFile.data());
	configs->user_agent = CefString(user_agent_);  // HTTP请求中的user_agent,CEF默认是Chorminum的user agent
	TCHAR locale_[1000];
	GetPrivateProfileString(L"CEF", L"locale", configs->locale.ToWString().c_str(), locale_, 1000, appDefFile.data());
	configs->locale = CefString(locale_);  // CEF默认是en-US
	configs->log_severity = GetPrivateProfileInt(L"CEF", L"log_severity", configs->log_severity, appDefFile.data());// 指定日志输出级别，默认不输出(disable),其他取值：verbose,info,warning,error,error-report
	TCHAR log_file_[1000];
	GetPrivateProfileString(L"CEF", L"log_file_", configs->log_file.ToWString().c_str(), log_file_, 1000, appDefFile.data());
	configs->log_file = CefString(log_file_);  // 指定调试时的日志文件，默认为"debug.log"。如果关闭日志，则不输出日志
	TCHAR resources_dir_path_[1000];
	GetPrivateProfileString(L"CEF", L"resources_dir_path", configs->resources_dir_path.ToWString().c_str(), resources_dir_path_, 1000, appDefFile.data());
	configs->resources_dir_path = CefString(resources_dir_path_);  // 指定cef资源文件（ cef.pak、devtools_resources.pak）的目录，默认从程序运行目录取
	TCHAR locales_dir_path_[1000];
	GetPrivateProfileString(L"CEF", L"locales_dir_path", configs->locales_dir_path.ToWString().c_str(), locales_dir_path_, 1000, appDefFile.data());
	configs->locales_dir_path = CefString(locales_dir_path_);// 指定cef本地化资源(locales)目录，默认去程序运行目录下的locales目录
	configs->ignore_certificate_errors = GetPrivateProfileInt(L"CEF", L"ignore_certificate_errors", configs->ignore_certificate_errors, appDefFile.data());             // 是否忽略SSL证书错误
	configs->remote_debugging_port = GetPrivateProfileInt(L"CEF", L"remote_debugging_port", configs->remote_debugging_port, appDefFile.data());                 // 远程调试端口，取值范围[1024-65535]
	return configs;
}

// 从命令行读取
JWebTopConfigs * JWebTopConfigs::parseCmdLine(LPTSTR szCmdLine){
	if (szCmdLine == NULL || lstrlen(szCmdLine) == 0){
		szCmdLine = LPTSTR(JWebTopConfigs::getAppDefFile(szCmdLine).c_str());// 没有指定命令行参数时按默认配置文件查找
	}
	if (szCmdLine[0] != ':'){// 不以:开头，认为是普通的文件
		return JWebTopConfigs::loadConfigs(JWebTopConfigs::getAppDefFile(szCmdLine));
	}
	int argc = 0;
	LPTSTR * args = CommandLineToArgvW(szCmdLine, &argc);
	JWebTopConfigs * configs = loadConfigs(JWebTopConfigs::getAppDefFile(args[7]));
	//// args[0]===特殊符号“:”
	configs->msgWin = atol(wch2chr(args[1]));
	configs->parentWin = atol(wch2chr(args[2]));
	int tmpi = atoi(wch2chr(args[3]));
	if (tmpi != -1)configs->x = tmpi;
	tmpi = atoi(wch2chr(args[4]));
	if (tmpi != -1)configs->y =tmpi;
	tmpi = atoi(wch2chr(args[5]));
	if (tmpi != -1)configs->w = tmpi;
	tmpi = atoi(wch2chr(args[6]));
	if (tmpi != -1)configs->h = tmpi;
	LPTSTR  url = args[8];
	if (url[0] != ':')configs->url = CefString(url);
	url = args[9];
	if (url[0] != ':')configs->icon = CefString(url);
	if (argc > 10)configs->name = CefString(args[10]);
	return configs;
}