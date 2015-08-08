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
	HWND hWnd;
	HWND bWnd;

	LONG oldMainProc;                   // 浏览器所在主窗口之前的消息处理函数
	LONG oldBrowserProc;                // 浏览器窗口之前的消息处理函数
	CefRefPtr<CefBrowser> browser;      // 窗口关联到的浏览器
	JWebTopConfigs  configs;            // 窗口相关的一些配置信息

	bool isDraging;                     // 是否正在拖动窗口
	LONG dragX, dragY;
};
typedef std::map<HWND, BrowserWindowInfo*> BrowserWindowInfoMap;// 定义一个存储BrowserWindowInfo的Map

// 对CEF浏览器窗口的消息进行拦截
LRESULT CALLBACK JWebTop_BrowerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// 根据配置信息(configs)对顶层窗口和实际浏览器窗口进行修饰
void renderBrowserWindow(CefRefPtr<CefBrowser> browser, JWebTopConfigs * configs);
// 
namespace jb{
	//void setCenter(HWND hWnd);                             // setToCenter(handler);// 让窗口显示在屏幕正中
	//void setCenter(HWND hWnd, int x, int y, int w, int h); // setToCenter(handler);// 让窗口显示在屏幕正中
	//POINT getPos(HWND hWnd);                               // getPos(handler);//获得窗口位置，返回值为一object，格式如下{x:13,y:54}
	//void setSize(HWND hWnd, int w, int h);                 // setSize(x, y, handler);//设置窗口大小
	//POINT getSize(HWND hWnd);                              // getSize(handler);//获得窗口大小，返回值为一object，格式如下{width:130,height:54}
	//POINT getScreenSize();                                 // getScreenSize();//获取屏幕大小，返回值为一object，格式如下{width:130,height:54}
	//void move(HWND hWnd, int x, int y);                    // move(x, y, handler);//移动窗口
	//void setBound(HWND hWnd, int x, int y, int w, int h);  // setBound(x, y,w ,h, handler);// 同时设置窗口的坐标和大小
	//RECT getBound(HWND hWnd);                              // getBound(handler);//获取窗口的位置和大小，返回值为一object，格式如下{x:100,y:100,width:130,height:54}
	//void setTitle(HWND hWnd, std::wstring title);          // setTitle(title, handler);// 设置窗口名称
	//std::wstring getTitle(HWND hWnd);                      // getTitle(handler);// 获取窗口名称，返回值为一字符串

	//void bringToTop(HWND hWnd);	// bringToTop(handler);//窗口移到最顶层
	//void focus(HWND hWnd);	    // focus(handler);//使窗口获得焦点
	//void hide(HWND hWnd);	    // hide(handler);//隐藏窗口
	//void max(HWND hWnd);	    // max(handler);//最大化窗口
	//void mini(HWND hWnd);	    // mini(hander);//最小化窗口
	//void restore(HWND hWnd);	// restore(handler);//还原窗口，对应于hide函数
	//void setTopMost(HWND hWnd);	// setTopMost(handler);//窗口置顶，此函数跟bringToTop的区别在于此函数会使窗口永远置顶，除非有另外一个窗口调用了置顶函数

	void close(HWND hWnd);      // close(handler);// 关闭窗口

	void setWindowStyle(HWND hWnd, int exStyle);	//setWindowStyle(exStyle, handler);//高级函数，设置窗口额外属性，诸如置顶之类。
	
	void loadUrl(HWND hWnd, std::wstring url);	//loadUrl(url, handler);//加载网页，url为网页路径
	void reload(HWND hWnd);	//reload(handler);//重新加载当前页面
	void reloadIgnoreCache(HWND hWnd);	//reloadIgnoreCache(handler);//重新加载当前页面并忽略缓存
	void showDev(HWND hWnd);	//showDev(handler);//打开开发者工具

	void ExecJS(HWND hWnd, std::string js);
	void ExecJS(HWND hWnd, std::wstring js);

	void runApp(std::wstring appDefFile, long parentWin);

#ifdef JWebTopJNI // 只有在JWebTop_JNI项目下，下面的代码才会编译
	CefString invokeJavaMethod(CefString json);// 用途：从JS调用Java代码。用法：invokeJava(jsonstring);
#endif
}
#endif