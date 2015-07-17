#include "JWebTopConfigs.h"
#include "JWebTop/tests/TestUtil.h"
using namespace std;

// 根据文件名获取问路径
wstring GetFilePath(wstring appDefFile){
	const int BUFSIZE = 4096;
	TCHAR  buffer[BUFSIZE] = TEXT("");
	GetFullPathName(LPCWSTR(appDefFile.data()),	BUFSIZE,buffer,	NULL);// 获取文件的绝对路径
	wstring pp(buffer);
	int p = pp.find_last_of('\\');
	if (p != -1)pp = pp.substr(0, p + 1);
	return pp;
}
CefString JWebTopConfigs::getAbsolutePath(std::wstring relativePath){
	if (relativePath.find(L":") == -1){// 如果指定的路径是相对路径
		wstring _path(appPath);
		relativePath = _path.append(relativePath);
	}
	return CefString(relativePath);
}

JWebTopConfigs loadConfigs(std::wstring appDefFile){
#ifdef WebTopLog
	writeLog(L"appDefFile=====" + appDefFile + L"\r\n");
#endif
	JWebTopConfigs configs = JWebTopConfigs();
	if (appDefFile.size() == 0)return configs;                                                   // 如果未指定配置文件（null，空指针） 
	LPCTSTR path = appDefFile.data();
	if (_waccess(path, 0) != 0)return configs;                                                   // 如果文件不存在
	configs.appDefFile = CefString(appDefFile);
	configs.appPath = GetFilePath(appDefFile);
	TCHAR url[1000], name[100], iconPath[1000];
	GetPrivateProfileString(L"BASE", L"url", NULL, url, 1000, appDefFile.data());
	configs.url = CefString(url);                                                                // 需要打开的地址 
	GetPrivateProfileString(L"BASE", L"name", NULL, name, 100, appDefFile.data());
	configs.name = CefString(name);                                                              // 设置浏览器窗口名称 
	GetPrivateProfileString(L"BASE", L"icon", NULL, iconPath, 1000, appDefFile.data());
	configs.icon = CefString(iconPath);                                                          // 设置浏览器图标 
	// 其实这里可以先通过获取字符串再转换为int的方式来判断
	configs.w = GetPrivateProfileInt(L"BASE", L"width", -1, appDefFile.data());
	configs.h = GetPrivateProfileInt(L"BASE", L"height", -1, appDefFile.data());
	configs.x = GetPrivateProfileInt(L"BASE", L"x", -1, appDefFile.data());
	configs.y = GetPrivateProfileInt(L"BASE", L"y", -1, appDefFile.data());
	//enableDrag = GetPrivateProfileInt(L"BASE", L"enableDrag", 0, appDefFile.data());
	configs.disableRefresh = 1 == GetPrivateProfileInt(L"BASE", L"disableRefresh", 0, appDefFile.data());
	configs.enableDebug = 0 == GetPrivateProfileInt(L"BASE", L"disableDevelop", 1, appDefFile.data());
	configs.enableResize = 1 == GetPrivateProfileInt(L"BASE", L"enableResize", 0, appDefFile.data());
	//disableTransparent = 1==GetPrivateProfileInt(L"BASE", L"disableTransparent", 0, appDefFile.data());
	//hasBorder = 1==GetPrivateProfileInt(L"BASE", L"hasBorder", 0, appDefFile.data());
	configs.max = GetPrivateProfileInt(L"BASE", L"max", 0, appDefFile.data());
	configs.dwStyle = GetPrivateProfileInt(L"BASE", L"style", 0, appDefFile.data());
	configs.dwExStyle = GetPrivateProfileInt(L"BASE", L"exStyle", 0, appDefFile.data());
	return configs;
}