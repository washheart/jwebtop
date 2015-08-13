#ifndef CEF_JWEBTOP_WIN_WNDPROC_H_
#define CEF_JWEBTOP_WIN_WNDPROC_H_
#include <sstream>
#include <string>
#include "include/cef_browser.h"
#include "include/cef_resource_handler.h"
#include "include/cef_request.h"
#include "include/wrapper/cef_message_router.h"
#include "JWebTop/config/JWebTopConfigs.h"

// 用于调试工具窗口的cefclient（如果传入JWebTopHandler会把对JWebTop的窗口设置在DEBUG上也应用一次）
class DEBUG_Handler : public CefClient{ IMPLEMENT_REFCOUNTING(DEBUG_Handler); };
class BrowserWindowInfo{
public:
	HWND hWnd;      // CEF浏览器的外部窗口
	HWND bWnd;      // CEF内部浏览器窗口
	HWND msgWin;    // 用户跨进程通信的远程进程窗口（只有通过DLL调用JWebTop时才有此参数） 

	LONG oldMainProc;                   // 浏览器所在主窗口之前的消息处理函数
	LONG oldBrowserProc;                // 浏览器窗口之前的消息处理函数
	CefRefPtr<CefBrowser> browser;      // 窗口关联到的浏览器
	JWebTopConfigs  configs;            // 窗口相关的一些配置信息

	bool isDraging;                     // 是否正在拖动窗口
	LONG dragX, dragY;
};
typedef std::map<HWND, BrowserWindowInfo*> BrowserWindowInfoMap;// 定义一个存储BrowserWindowInfo的Map
// 处理WM_COPYDATA消息
LRESULT onWmCopyData(BrowserWindowInfo * bwInfo, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void thread_executeWmCopyData(HWND hWnd, DWORD msgId, std::wstring json);


// 对CEF浏览器窗口的消息进行拦截
LRESULT CALLBACK JWebTop_BrowerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// 根据配置信息(configs)对顶层窗口和实际浏览器窗口进行修饰
void renderBrowserWindow(CefRefPtr<CefBrowser> browser, JWebTopConfigs * configs);


// 
namespace jb{
	void close(HWND hWnd);      // close(handler);// 关闭窗口

	void setWindowStyle(HWND hWnd, int exStyle);	//setWindowStyle(exStyle, handler);//高级函数，设置窗口额外属性，诸如置顶之类。
	
	void loadUrl(HWND hWnd, std::wstring url);	//loadUrl(url, handler);//加载网页，url为网页路径
	void reload(HWND hWnd);	//reload(handler);//重新加载当前页面
	void reloadIgnoreCache(HWND hWnd);	//reloadIgnoreCache(handler);//重新加载当前页面并忽略缓存
	void showDev(HWND hWnd);	//showDev(handler);//打开开发者工具

	void ExecJS(HWND hWnd, std::string js);
	void ExecJS(HWND hWnd, std::wstring js);

	void runApp(HWND hWnd, std::wstring appDefFile, long parentWin);


	// 调用另外的进程并等待
	CefString invokeRemote_Wait(HWND hWnd, CefString json);
	void invokeRemote_NoWait(HWND hWnd, CefString json);
}
#endif