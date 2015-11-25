#ifndef CEF_JWEBTOP_WIN_WNDPROC_H_
#define CEF_JWEBTOP_WIN_WNDPROC_H_
#include <sstream>
#include <string>
#include "include/cef_browser.h"
#include "JWebTop/config/JWebTopConfigs.h"

using namespace std;
// 用于调试工具窗口的cefclient（如果传入JWebTopHandler会把对JWebTop的窗口设置在DEBUG上也应用一次）
class DEBUG_Handler : public CefClient{ IMPLEMENT_REFCOUNTING(DEBUG_Handler); };
class BrowserWindowInfo{
public:
	HWND hWnd = NULL;					// CEF浏览器的外部窗口
	HWND bWnd = NULL;					// CEF内部浏览器窗口
	HWND rWnd = NULL;					// CEF内部绘制器窗口
	bool isWsChild = false;				// 是否有WS_CHILD风格

	LONG oldMainProc;                   // 浏览器所在主窗口之前的消息处理函数
	LONG oldBrowserProc;                // 浏览器窗口之前的消息处理函数
	LONG oldRenderProc;                 // 内部绘制窗口之前的消息处理函数

	CefRefPtr<CefBrowser> browser;      // 窗口关联到的浏览器

	bool enableDrag = true;				// 是否允许窗口拖动
	bool isDraging = false;			    // 是否正在拖动窗口
	LONG dragX = 0, dragY = 0;			// 窗口拖动开始时的x、y坐标位置
};
BrowserWindowInfo * getBrowserWindowInfo(HWND hWnd);

typedef map<HWND, BrowserWindowInfo*> BrowserWindowInfoMap;// 定义一个存储BrowserWindowInfo的Map

// 对CEF浏览器窗口的消息进行拦截
LRESULT CALLBACK JWebTop_BrowerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// 根据配置信息(configs)对顶层窗口和实际浏览器窗口进行修饰
void renderBrowserWindow(CefRefPtr<CefBrowser> browser, JWebTopConfigs * configs);


// 
namespace jb{
	void close(HWND hWnd);      // close(handler);// 关闭窗口

	void setWindowStyle(HWND hWnd, int exStyle);	//setWindowStyle(exStyle, handler);//高级函数，设置窗口额外属性，诸如置顶之类。

	void loadUrl(HWND hWnd, wstring url);	//loadUrl(url, handler);//加载网页，url为网页路径
	void reload(HWND hWnd);	//reload(handler);//重新加载当前页面
	void reloadIgnoreCache(HWND hWnd);	//reloadIgnoreCache(handler);//重新加载当前页面并忽略缓存
	void showDev(HWND hWnd);	//showDev(handler);//打开开发者工具

	void ExecJS(HWND hWnd, string js);
	void ExecJS(HWND hWnd, wstring js);

	void enableDrag(HWND hWnd, bool enable);
	void startDrag(HWND hWnd);
	void stopDrag(HWND hWnd);

	void runApp(HWND hWnd, wstring appDefFile, long parentWin, const LPTSTR url, const LPTSTR &title, const LPTSTR &icon, const  int x, const  int y, const  int w, const  int h);

	void checkAndSetResizeAblity(HWND hWnd);// 检查窗口是否能拖动改变大小
}
#endif