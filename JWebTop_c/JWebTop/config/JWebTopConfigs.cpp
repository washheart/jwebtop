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

// �������·����ȡ����·��
// ���relativePath�Ѿ��Ǿ���·������ֱ�ӷ���
// ���򽫷���appPath+relativePath
 CefString JWebTopConfigs::getAbsolutePath(std::wstring relativePath){
	 wstring _appPath = appPath.ToWString();
	 if (relativePath.find(L":") == -1){// ���ָ����·�������·��
		 _appPath.append(relativePath);
		 return jw::str::URLEncode(_appPath);
	 }
	 return relativePath;
 }
// ���ݴ���Ĳ�����ȡ�����ļ�·��(Ŀǰ��Ҫ���ڴ��������ΪNULL����ַ��������)
std::wstring JWebTopConfigs::getAppDefFile(LPCTSTR lpCmdLine){
	if (lpCmdLine == NULL || lstrlen(lpCmdLine) == 0){// δָ�������ļ���������Ƿ�ͨ����������ָ����appDefFile
		return jw::os::file::GetExePath() + L"index.app";// ȡ��������Ŀ¼�µ�index.app�ļ�;
	}
	else{// ȡ����������ΪappDefFile·��
		std::wstring tmp(lpCmdLine);
		if (tmp.find(L":") == -1){// ���ָ����·�������·��
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
	if (appDefFile.size() == 0)return configs;                                                   // ���δָ�������ļ���null����ָ�룩 
	LPCTSTR path = appDefFile.data();
	if (_waccess(path, 0) != 0)return configs;                                                   // ����ļ�������
	configs->appDefFile = CefString(appDefFile);
	configs->appPath = jw::os::file::GetFilePath(appDefFile);
	TCHAR url[1000], appendJs[1000], name[100], iconPath[1000];
	GetPrivateProfileString(L"BASE", L"url", NULL, url, 1000, path);
	configs->url = CefString(url);                                                                // ��Ҫ�򿪵ĵ�ַ 
	GetPrivateProfileString(L"BASE", L"appendJs", NULL, appendJs, 1000, path);
	configs->appendJs = CefString(appendJs);                                                                // ��Ҫ���ӵ�JS�ļ� 
	
	GetPrivateProfileString(L"BASE", L"name", NULL, name, 100, path);
	configs->name = CefString(name);                                                              // ����������������� 
	GetPrivateProfileString(L"BASE", L"icon", NULL, iconPath, 1000, path);
	configs->icon = CefString(iconPath);                                                          // ���������ͼ�� 
	// ��ʵ���������ͨ����ȡ�ַ�����ת��Ϊint�ķ�ʽ���ж�
	configs->w = GetPrivateProfileInt(L"BASE", L"width", -1, path);
	configs->h = GetPrivateProfileInt(L"BASE", L"height", -1, path);
	configs->x = GetPrivateProfileInt(L"BASE", L"x", -1, path);
	configs->y = GetPrivateProfileInt(L"BASE", L"y", -1, path);
	configs->enableDrag = 1 == GetPrivateProfileInt(L"BASE", L"enableDrag", 1, path);        // Ĭ�Ͽ���ͨ��ҳ������϶�
	configs->disableRefresh = 0 == GetPrivateProfileInt(L"BASE", L"disableRefresh", 0, path);// Ĭ�Ͻ�ֹˢ��
	configs->enableDebug = 0 == GetPrivateProfileInt(L"BASE", L"disableDevelop", 1, path);   // Ĭ�ϲ����Ե���
	configs->enableResize = 1 == GetPrivateProfileInt(L"BASE", L"enableResize", 1, path);    // Ĭ�Ͽ��Ըı��С
	//disableTransparent = 1==GetPrivateProfileInt(L"BASE", L"disableTransparent", 0, path);
	//hasBorder = 1==GetPrivateProfileInt(L"BASE", L"hasBorder", 0, path);
	configs->max = GetPrivateProfileInt(L"BASE", L"max", 0, path);
	configs->dwStyle = GetPrivateProfileInt(L"BASE", L"style", 0, path);
	configs->dwExStyle = GetPrivateProfileInt(L"BASE", L"exStyle", 0, path);

	// cef
	configs->single_process = GetPrivateProfileInt(L"CEF", L"single_process", configs->single_process, path);             // �Ƿ�ʹ�õ�����ģʽ��JWebTopĬ��ʹ�á�CEFĬ�ϲ�ʹ�õ�����ģʽ
	TCHAR user_data_path_[1000];
	GetPrivateProfileString(L"CEF", L"user_data_path", configs->user_data_path.ToWString().c_str(), user_data_path_, 1000, path);
	configs->user_data_path = CefString(user_data_path_); // �û����ݱ���Ŀ¼
	TCHAR cache_path_[1000];
	GetPrivateProfileString(L"CEF", L"cache_path", configs->cache_path.ToWString().c_str(), cache_path_, 1000, path);
	configs->cache_path = CefString(cache_path_); // ������������ݵı���Ŀ¼
	configs->persist_session_cookies = GetPrivateProfileInt(L"CEF", L"single_process", configs->persist_session_cookies, path);             // �Ƿ���Ҫ�־û��û�cookie���ݣ���Ҫ����Ϊtrue����Ҫͬʱָ��cache_path��
	TCHAR user_agent_[1000];
	GetPrivateProfileString(L"CEF", L"user_agent", configs->user_agent.ToWString().c_str(), user_agent_, 1000, path);
	configs->user_agent = CefString(user_agent_);  // HTTP�����е�user_agent,CEFĬ����Chorminum��user agent
	TCHAR locale_[1000];
	GetPrivateProfileString(L"CEF", L"locale", configs->locale.ToWString().c_str(), locale_, 1000, path);
	configs->locale = CefString(locale_);  // CEFĬ����en-US
	configs->log_severity = GetPrivateProfileInt(L"CEF", L"log_severity", configs->log_severity, path);// ָ����־�������Ĭ�ϲ����(disable),����ȡֵ��verbose,info,warning,error,error-report
	TCHAR log_file_[1000];
	GetPrivateProfileString(L"CEF", L"log_file_", configs->log_file.ToWString().c_str(), log_file_, 1000, path);
	configs->log_file = CefString(log_file_);  // ָ������ʱ����־�ļ���Ĭ��Ϊ"debug.log"������ر���־���������־
	TCHAR resources_dir_path_[1000];
	GetPrivateProfileString(L"CEF", L"resources_dir_path", configs->resources_dir_path.ToWString().c_str(), resources_dir_path_, 1000, path);
	configs->resources_dir_path = CefString(resources_dir_path_);  // ָ��cef��Դ�ļ��� cef.pak��devtools_resources.pak����Ŀ¼��Ĭ�ϴӳ�������Ŀ¼ȡ
	TCHAR locales_dir_path_[1000];
	GetPrivateProfileString(L"CEF", L"locales_dir_path", configs->locales_dir_path.ToWString().c_str(), locales_dir_path_, 1000, path);
	configs->locales_dir_path = CefString(locales_dir_path_);// ָ��cef���ػ���Դ(locales)Ŀ¼��Ĭ��ȥ��������Ŀ¼�µ�localesĿ¼
	configs->ignore_certificate_errors = GetPrivateProfileInt(L"CEF", L"ignore_certificate_errors", configs->ignore_certificate_errors, path);             // �Ƿ����SSL֤�����
	configs->remote_debugging_port = GetPrivateProfileInt(L"CEF", L"remote_debugging_port", configs->remote_debugging_port, path);                 // Զ�̵��Զ˿ڣ�ȡֵ��Χ[1024-65535]
	
	TCHAR proxyServer_[1000];
	GetPrivateProfileString(L"CEF", L"proxyServer", configs->proxyServer.ToWString().c_str(), proxyServer_, 1000, path);
	configs->proxyServer = CefString(proxyServer_);									// ���ô����������ַ������һ��http�����������  ���ö�������������--proxy-server="https=proxy1:80;http=socks4://baz:1080"
	TCHAR proxyAuthUser_[1000];
	GetPrivateProfileString(L"CEF", L"proxyAuthUser", configs->proxyAuthUser.ToWString().c_str(), proxyAuthUser_, 1000, path);
	configs->proxyAuthUser = CefString(proxyAuthUser_);								// ��¼���������ʱ��Ҫ���û�����ע�⣺����proxyServer�����˶��ٴ����������������ʱֵ֧��һ���û��������룩
	TCHAR proxyAuthPwd_[1000];
	GetPrivateProfileString(L"CEF", L"proxyAuthPwd", configs->proxyAuthPwd.ToWString().c_str(), proxyAuthPwd_, 1000, path);
	configs->proxyAuthPwd = CefString(proxyAuthPwd_);									// ��¼���������ʱ��Ҫ������

	return configs;
}

// ����������ִ�д���������Ĳ����Ľ���
JWebTopConfigs * JWebTopConfigs::loadAsJSON(wstring jsonString){
	if (jsonString.length() == 0)return loadConfigs(JWebTopConfigs::getAppDefFile(NULL));
	CefRefPtr<CefValue> v = CefParseJSON(CefString(jsonString), JSON_PARSER_RFC);  // ���н���
	if (v == NULL)return loadConfigs(JWebTopConfigs::getAppDefFile(NULL));
	CefRefPtr<CefDictionaryValue> value = v->GetDictionary();// ��JSON�ֵ�����ȡ
	CefRefPtr<CefValue> tmp= value->GetValue("appDefFile");
	JWebTopConfigs * configs = loadConfigs(JWebTopConfigs::getAppDefFile((tmp != NULL ? LPCTSTR(tmp->GetString().ToWString().c_str()) : NULL)));
	tmp = value->GetValue("parentWinS");
	if (tmp != NULL)configs->parentWin = jw::str::parseLong64(tmp->GetString().ToWString());
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
	if (tmp != NULL)configs->dwStyle = jw::str::parseLong64(tmp->GetString().ToWString());
	tmp = value->GetValue("dwExStyleS");
	if (tmp != NULL)configs->dwExStyle = jw::str::parseLong64(tmp->GetString().ToWString());
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

	// [CEF]С�ڣ�cef���ò�����cef��ز���ֻ��Ӧ������ʱ�����ã���������[CEF]С����
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