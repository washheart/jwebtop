#ifndef CEF_JWEBTOP_WIN_CTRL_H_
#define CEF_JWEBTOP_WIN_CTRL_H_
#include <sstream>
#include <string>
//#include "include\cef_client.h"
#include "include/cef_browser.h"
#include "include/cef_resource_handler.h"
#include "include/cef_request.h"
#include "include/wrapper/cef_message_router.h"
#include "JWebTopConfigs.h"

class BrowserWindowInfo{
public:
	HWND hWnd;
	HWND bWnd;

	LONG oldProc;                       // 窗口之前的消息处理函数
	CefRefPtr<CefBrowser> browser;      // 窗口关联到的浏览器
	JWebTopConfigs  configs;            // 窗口相关的一些配置信息

	bool isDraging;                     // 是否正在拖动窗口
	LONG dragX, dragY;
};
typedef std::map<HWND, BrowserWindowInfo*> BrowserWindowInfoMap;// 定义一个存储BrowserWindowInfo的Map

// 对CEF浏览器窗口的消息进行拦截
LRESULT CALLBACK JWebTop_BrowerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// 根据配置信息(configs)对顶层窗口和实际浏览器窗口进行修饰
void renderBrowserWindow(CefRefPtr<CefBrowser> browser,JWebTopConfigs configs);
// 
namespace jw{
	void setTitle(HWND hWnd, std::wstring title);
	std::wstring getTitle(HWND hWnd);
	void move(HWND hWnd, int x, int y);
	void setSize(HWND hWnd, int w, int h);
	void setBound(HWND hWnd, int x, int y, int w, int h);
}
#endif