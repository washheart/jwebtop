#include "JWebTopConfigs.h"

#include <Windows.h>
#include <ShellAPI.h> 
#include <iostream> 
#include <sstream> 
#include <strsafe.h>
#include "include/cef_parser.h"
#include "common/util/StrUtil.h"
#include "common/os/OS.h"
#include "JWebTop/dllex/JWebTop_DLLEx.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
using namespace std;
wstring errorUrl;
void JWebTopConfigs::setErrorURL(wstring url){
	errorUrl = url;
}
 wstring JWebTopConfigs::getErrorURL(){
	 return errorUrl;
}

// 根据相对路径获取绝对路径
// 如果relativePath已经是绝对路径，则直接返回
// 否则将返回appPath+relativePath
 CefString JWebTopConfigs::getAbsolutePath(std::wstring relativePath){
	 wstring _appPath = appPath.ToWString();
	 if (relativePath.find(L":") == -1){// 如果指定的路径是相对路径
		 _appPath.append(relativePath);
		 return jw::URLEncode(_appPath);
	 }
	 return relativePath;
 }
// 根据传入的参数获取配置文件路径(目前主要用于处理传入参数为NULL或空字符串的情况)
std::wstring JWebTopConfigs::getAppDefFile(LPCTSTR lpCmdLine){
	if (lpCmdLine == NULL || lstrlen(lpCmdLine) == 0){// 未指定启动文件，检查下是否通过启动参数指定了appDefFile
		return jw::os::file::GetExePath() + L"index.app";// 取程序所在目录下的index.app文件;
	}
	else{// 取启动参数作为appDefFile路径
		std::wstring tmp(lpCmdLine);
		if (tmp.find(L":") == -1){// 如果指定的路径是相对路径
			return jw::os::file::GetExePath() + lpCmdLine;
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
	configs->appPath = jw::os::file::GetFilePath(appDefFile);
	TCHAR url[1000], appendJs[1000], name[100], iconPath[1000];
	GetPrivateProfileString(L"BASE", L"url", NULL, url, 1000, appDefFile.data());
	configs->url = CefString(url);                                                                // 需要打开的地址 
	GetPrivateProfileString(L"BASE", L"appendJs", NULL, appendJs, 1000, appDefFile.data());
	configs->appendJs = CefString(appendJs);                                                                // 需要附加的JS文件 
	
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
	
	TCHAR proxyServer_[1000];
	GetPrivateProfileString(L"CEF", L"proxyServer", configs->proxyServer.ToWString().c_str(), proxyServer_, 1000, appDefFile.data());
	configs->proxyServer = CefString(proxyServer_);									// 设置代理服务器地址。设置一个http代理服务器：  设置多个代理服务器：--proxy-server="https=proxy1:80;http=socks4://baz:1080"
	TCHAR proxyAuthUser_[1000];
	GetPrivateProfileString(L"CEF", L"proxyAuthUser", configs->proxyAuthUser.ToWString().c_str(), proxyAuthUser_, 1000, appDefFile.data());
	configs->proxyAuthUser = CefString(proxyAuthUser_);								// 登录代理服务器时需要的用户名（注意：不管proxyServer设置了多少代理服务器，这里暂时值支持一组用户名和密码）
	TCHAR proxyAuthPwd_[1000];
	GetPrivateProfileString(L"CEF", L"proxyAuthPwd", configs->proxyAuthPwd.ToWString().c_str(), proxyAuthPwd_, 1000, appDefFile.data());
	configs->proxyAuthPwd = CefString(proxyAuthPwd_);									// 登录代理服务器时需要的密码

	return configs;
}

// 根据命令行执行创建浏览器的参数的解析
JWebTopConfigs * JWebTopConfigs::loadAsJSON(wstring jsonString){
	if (jsonString.length() == 0)return loadConfigs(JWebTopConfigs::getAppDefFile(NULL));
	CefRefPtr<CefValue> v = CefParseJSON(CefString(jsonString), JSON_PARSER_RFC);  // 进行解析
	if (v == NULL)return loadConfigs(JWebTopConfigs::getAppDefFile(NULL));
	CefRefPtr<CefDictionaryValue> value = v->GetDictionary();// 按JSON字典来获取
	CefRefPtr<CefValue> tmp= value->GetValue("appDefFile");
	JWebTopConfigs * configs = loadConfigs(JWebTopConfigs::getAppDefFile((tmp != NULL ? LPCTSTR(tmp->GetString().ToWString().c_str()) : NULL)));
	tmp = value->GetValue("parentWinS");
	if (tmp != NULL)configs->parentWin = jw::parseLong64(tmp->GetString().ToWString());
	// url,appendJs,name,icon,dwStyle,dwExStyle
	tmp = value->GetValue("url");
	if (tmp != NULL)configs->url = tmp->GetString();
	tmp = value->GetValue("appendJs");
	if (tmp != NULL)configs->appendJs = tmp->GetString();
	tmp = value->GetValue("name");
	if (tmp != NULL)configs->name = tmp->GetString();
	tmp = value->GetValue("icon");
	if (tmp != NULL)configs->icon = tmp->GetString();
	tmp = value->GetValue("dwStyleS");
	if (tmp != NULL)configs->dwStyle = jw::parseLong64(tmp->GetString().ToWString());
	tmp = value->GetValue("dwExStyleS");
	if (tmp != NULL)configs->dwExStyle = jw::parseLong64(tmp->GetString().ToWString());
	// x,y,w,h,max
	tmp = value->GetValue("x");
	if (tmp != NULL)configs->x = tmp->GetInt();
	tmp = value->GetValue("y");
	if (tmp != NULL)configs->y = tmp->GetInt();
	tmp = value->GetValue("h");
	if (tmp != NULL)configs->h = tmp->GetInt();
	tmp = value->GetValue("w");
	if (tmp != NULL)configs->w = tmp->GetInt();
	tmp = value->GetValue("max");
	if (tmp != NULL)configs->max = tmp->GetInt();
	// enableDebug,enableResize,disableRefresh,enableDrag
	tmp = value->GetValue("enableDebug");
	if (tmp != NULL)configs->enableDebug = (tmp->GetInt()!=0);
	tmp = value->GetValue("enableResize");
	if (tmp != NULL)configs->enableResize = (tmp->GetInt() != 0);
	tmp = value->GetValue("disableRefresh");
	if (tmp != NULL)configs->disableRefresh = (tmp->GetInt() != 0);
	tmp = value->GetValue("enableDrag");
	if (tmp != NULL)configs->enableDrag = (tmp->GetInt() != 0);

	// [CEF]小节：cef配置参数（cef相关参数只在应用启动时起作用），配置在[CEF]小节下
	tmp = value->GetValue("single_process");
	if (tmp != NULL)configs->single_process = tmp->GetInt();
	tmp = value->GetValue("user_data_path");
	if (tmp != NULL)configs->user_data_path = tmp->GetString();
	tmp = value->GetValue("cache_path");
	if (tmp != NULL)configs->cache_path = tmp->GetString();
	tmp = value->GetValue("persist_session_cookies");
	if (tmp != NULL)configs->persist_session_cookies = tmp->GetInt();
	tmp = value->GetValue("user_agent");
	if (tmp != NULL)configs->user_agent = tmp->GetString();
	tmp = value->GetValue("locale");
	if (tmp != NULL)configs->locale = tmp->GetString();
	tmp = value->GetValue("log_severity");
	if (tmp != NULL)configs->log_severity = tmp->GetInt();
	tmp = value->GetValue("log_file");
	if (tmp != NULL)configs->log_file = tmp->GetString();
	tmp = value->GetValue("resources_dir_path");
	if (tmp != NULL)configs->resources_dir_path = tmp->GetString();

	tmp = value->GetValue("locales_dir_path");
	if (tmp != NULL)configs->locales_dir_path = tmp->GetString();
	tmp = value->GetValue("ignore_certificate_errors");
	if (tmp != NULL)configs->ignore_certificate_errors = tmp->GetInt();
	tmp = value->GetValue("remote_debugging_port");
	if (tmp != NULL)configs->remote_debugging_port = tmp->GetInt();
	

	tmp = value->GetValue("proxyServer");
	if (tmp != NULL)configs->proxyServer = tmp->GetString();
	tmp = value->GetValue("proxyAuthUser");
	if (tmp != NULL)configs->proxyAuthUser = tmp->GetString();
	tmp = value->GetValue("proxyAuthPwd");
	if (tmp != NULL)configs->proxyAuthPwd = tmp->GetString();
	tmp = value->GetValue("no_sandbox");
	if (tmp != NULL)configs->no_sandbox = tmp->GetInt();

	return configs;
}