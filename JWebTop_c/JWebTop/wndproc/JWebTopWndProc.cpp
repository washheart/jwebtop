#include "JWebTopWndProc.h"

#include <thread>
#include <WindowsX.h>
#include "JWebTop/browser/JWebTopCommons.h"
#include "common/util/StrUtil.h"
#include <comdef.h>
#include <algorithm>
#include <gdiplus.h>
#include "JWebTop/jshandler/JJH_Windows.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif

#pragma comment(lib,"GdiPlus.lib")
using namespace std;
BrowserWindowInfoMap BrowserWindowInfos;// 在静态变量中缓存所有已创建的窗口信息

BrowserWindowInfo * getBrowserWindowInfo(HWND hWnd){
	BrowserWindowInfoMap::iterator it = BrowserWindowInfos.find(hWnd);
	if (BrowserWindowInfos.end() != it) {
		return it->second;
	}
	return NULL;
}

namespace jb{
	//close(handler);// 关闭窗口
	void close(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL){
			if (bw->isWsChild){
				// DestroyWindow(hWnd);// 不能关闭浏览器进程
				SetParent(hWnd, NULL); // 所以需要先把父窗口设置为NULL，然后再关闭
			}
			bw->browser->GetHost()->CloseBrowser(true);
		}
	}

	//loadUrl(url, handler);//加载网页，url为网页路径
	void loadUrl(HWND hWnd, std::wstring url){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL)bw->browser->GetMainFrame()->LoadURL(url);
	}
	//reload(handler);//重新加载当前页面
	void reload(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL)bw->browser->Reload();
	}

	// 为当前网页动态记载一个js文件(url, handler);//加载网页，url为网页路径
	void appendJSFile(HWND hWnd, std::wstring jsfile){

	}
	//reloadIgnoreCache(handler);//重新加载当前页面并忽略缓存
	void reloadIgnoreCache(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL)bw->browser->ReloadIgnoreCache();
	}
	void __showDev(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL){
			CefWindowInfo windowInfo;
			windowInfo.SetAsPopup(NULL, "cef_debug");
			bw->browser->GetHost()->ShowDevTools(windowInfo, new DEBUG_Handler(), CefBrowserSettings(), CefPoint());
		}
	}
	//showDev(handler);//打开开发者工具
	void showDev(HWND hWnd){
		thread t(__showDev, hWnd);
		t.detach();// 开启新线程启动开发者工具，避免和JS线程锁在一起
	}

	void ExecJS(HWND hWnd, string js){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL){
			bw->browser->GetMainFrame()->ExecuteJavaScript(CefString(js), "", 0);
		}
	}
	void ExecJS(HWND hWnd, wstring js){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL){
			bw->browser->GetMainFrame()->ExecuteJavaScript(CefString(js), "", 0);
		}
	}
	void CreateNewBrowserThread(JWebTopConfigs *tmpConfigs){
		createNewBrowser(tmpConfigs);
	}
	void runApp(HWND hWnd, wstring appDefFile, long parentWin, const LPTSTR url, const LPTSTR &title, const LPTSTR &icon, const  int x, const  int y, const  int w, const  int h){
#ifdef JWebTopLog 
		std::wstringstream log;
		log << L"run app=" << appDefFile << L",parentWin=" << parentWin << L"\r\n";
		writeLog(log.str());
#endif
		//if (tmpConfigs != g_configs)delete tmpConfigs;
		JWebTopConfigs *tmpConfigs = JWebTopConfigs::loadConfigs(JWebTopConfigs::getAppDefFile(appDefFile.c_str()));
		tmpConfigs->parentWin = parentWin;

		if (url != NULL)tmpConfigs->url = CefString(url);
		if (title != NULL)tmpConfigs->url = CefString(title);
		if (icon != NULL)tmpConfigs->url = CefString(icon);
		if (x != -1)tmpConfigs->x = x;
		if (y != -1)tmpConfigs->y = y;
		if (h != -1)tmpConfigs->h = h;
		if (w != -1)tmpConfigs->w = w;
		thread t(CreateNewBrowserThread, tmpConfigs);
		t.detach();
	}

	void enableDrag(HWND hWnd, bool enable){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw == NULL)return;
		bw->enableDrag = enable;
	}
	void startDrag(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw == NULL || !bw->enableDrag)return;
		POINT pt;
		RECT rt;
		GetWindowRect(hWnd, &rt);
		GetCursorPos(&pt);
		// 计算并设置相对坐标
		bw->dragX = pt.x - rt.left;
		bw->dragY = pt.y - rt.top;
		bw->isDraging = true;// 标记为开始拖动
	}
	void stopDrag(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw == NULL)return;
		bw->isDraging = false;// 停止拖动
	}
}

HICON GetIcon(CefString url, CefString path){
	if (path.ToWString().find(L":") == -1){// 如果指定的路径是相对路径
		wstring _path;
		_path = url.ToWString();
		jw::replace_allW(_path, L"\\", L"/");
		_path = _path.substr(0, _path.find_last_of('/') + 1);
		path = _path.append(path);
	}
	Gdiplus::GdiplusStartupInput StartupInput;
	ULONG_PTR m_gdiplusToken;
	Gdiplus::Status sResult = Gdiplus::GdiplusStartup(&m_gdiplusToken, &StartupInput, NULL);
	if (sResult == Gdiplus::Ok){
		Gdiplus::Bitmap * bb = Gdiplus::Bitmap::FromFile(path.c_str(), false);
		HICON hIcon = NULL;
		bb->GetHICON(&hIcon);
		delete bb;
		Gdiplus::GdiplusShutdown(m_gdiplusToken);// 关闭gdi
		return hIcon;
	}
	return NULL;
}
const LONG border_width = 5; // 定义边框的宽度
LRESULT transparentNCHITTEST(HWND hWnd, LPARAM lParam){
	RECT wRect;
	GetWindowRect(hWnd, &wRect);
	long x = GET_X_LPARAM(lParam);
	long y = GET_Y_LPARAM(lParam);
	if (x >= wRect.left + border_width && y >= wRect.top + border_width
		&&x <= wRect.right - border_width &&y <= wRect.bottom - border_width){
		return HTCLIENT;// 如果在（重新计算的）客户区内
	}
	return HTTRANSPARENT;
}
LRESULT topWinNCHITTEST(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	BrowserWindowInfo * bwInfo = getBrowserWindowInfo(hWnd);
	RECT wRect;
	GetWindowRect(hWnd, &wRect);
	long x = GET_X_LPARAM(lParam), xLT = wRect.left + border_width, xRB = wRect.right - border_width;
	long y = GET_Y_LPARAM(lParam), yLT = wRect.top + border_width, yRB = wRect.bottom - border_width;
	if (y >= wRect.top && y < yLT)	{
		if (x < wRect.right && x >= xRB) return HTTOPRIGHT;		// 右上角		
		if (x >= wRect.left && x < xLT)  return HTTOPLEFT;		// 左上角
		return HTTOP;											// 上边
	}
	if (y < wRect.bottom && y >= yRB){
		if (x < wRect.right && x >= xRB) return HTBOTTOMRIGHT;	// 右下角
		if (x >= wRect.left && x < xLT)  return HTBOTTOMLEFT;	// 左下角
		return                                  HTBOTTOM;		// 下边
	}
	if (x >= wRect.left && x < xLT)      return HTLEFT;			// 左边
	if (x < wRect.right && x >= xRB)	 return HTRIGHT;		// 右边
	return CallWindowProc((WNDPROC)bwInfo->oldMainProc, hWnd, message, wParam, lParam);
}

LRESULT CALLBACK JWebTop_RenderWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	BrowserWindowInfo * bwInfo = getBrowserWindowInfo(hWnd);
	switch (message){
	case WM_NCHITTEST:
		return transparentNCHITTEST(hWnd, lParam);
	}
	if (bwInfo == NULL)return DefWindowProc(hWnd, message, wParam, lParam);
	//return ss(L"Render", 1, hWnd, message, wParam, lParam);
	return CallWindowProc((WNDPROC)bwInfo->oldRenderProc, hWnd, message, wParam, lParam);
}
// 拦截主窗口的消息：有些消息（比如关闭窗口、移动窗口等）只有在主窗口才能侦听到
LRESULT CALLBACK JWebTop_WindowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	BrowserWindowInfo * bwInfo = getBrowserWindowInfo(hWnd);
	// 可以对必要的信息进行预先处理，需要拦截的消息就可以不用发给浏览器了stringstream s;
	switch (message) {
	case WM_SIZE:
		jw::js::events::sendSize(bwInfo->browser->GetMainFrame(), LOWORD(lParam), HIWORD(lParam));
		break;// End case-WM_SIZE
	case WM_MOVE:
		jw::js::events::sendMove(bwInfo->browser->GetMainFrame(), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;// End case-WM_MOVE
	case WM_ACTIVATE:
		jw::js::events::sendWinowActive(bwInfo->browser->GetMainFrame(), (long)hWnd, LOWORD(wParam));
		break;// End case-WM_ACTIVATE
	case WM_ACTIVATEAPP:
		jw::js::events::sendAppActive(bwInfo->browser->GetMainFrame(), (long)hWnd);
		break;// End case-WM_ACTIVATEAPP
	case WM_NCHITTEST:
		if (bwInfo->rWnd != NULL)return topWinNCHITTEST(hWnd, message, wParam, lParam);
		break;
	case WM_NCDESTROY:
		BrowserWindowInfos.erase(hWnd);// 清理掉在map中的数据
		break;
	}// End switch-message
	if (bwInfo == NULL)return DefWindowProc(hWnd, message, wParam, lParam);
	return CallWindowProc((WNDPROC)bwInfo->oldMainProc, hWnd, message, wParam, lParam);
}
// 拦截浏览器窗口的消息
LRESULT CALLBACK JWebTop_BrowerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	BrowserWindowInfo * bwInfo = getBrowserWindowInfo(hWnd);
	// 可以对必要的信息进行预先处理，需要拦截的消息就可以不用发给浏览器了stringstream s;
	switch (message) {
	case WM_KILLFOCUS:
		bwInfo->isDraging = false; // 失去焦点时，停止拖动
		break;
#ifdef JWebTopLog
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_F12:
			jb::showDev(hWnd);
			break;
		}
		break;
#endif
	case WM_MOUSEMOVE:{
						  if (bwInfo->isDraging){
							  POINT pt;
							  GetCursorPos(&pt);
							  LONG x = pt.x - bwInfo->dragX;
							  LONG y = pt.y - bwInfo->dragY;
							  SetWindowPos(bwInfo->hWnd, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);// 移动窗口，但不改变窗口大小和窗口所在层次
						  }
	}
		break;
	case WM_NCHITTEST:
		if (bwInfo->rWnd != NULL)return transparentNCHITTEST(hWnd, lParam);
		break;
	case WM_NCDESTROY:
		BrowserWindowInfos.erase(hWnd);// 清理掉在map中的数据
		break;
	case WM_PARENTNOTIFY:
	{
							UINT msg2 = LOWORD(wParam);
							if (msg2 == WM_DESTROY){
								if (bwInfo == NULL){
									DefWindowProc(hWnd, message, wParam, lParam);
								}
								else{
									SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)bwInfo->oldBrowserProc);// 设置回原来的处理函数
									BrowserWindowInfos.erase(hWnd);// 清理掉在map中的数据
								}
							}
	}
		break;
	}// End switch-message
	if (bwInfo == NULL)return DefWindowProc(hWnd, message, wParam, lParam);
	return CallWindowProc((WNDPROC)bwInfo->oldBrowserProc, hWnd, message, wParam, lParam);
}

// 根据配置信息(configs)对顶层窗口和实际浏览器窗口进行修饰
void renderBrowserWindow(CefRefPtr<CefBrowser> browser, JWebTopConfigs * p_configs){
	JWebTopConfigs configs = (*p_configs);
	HWND hWnd = browser->GetHost()->GetWindowHandle();// 浏览器所在窗口的handle
	WINDOWINFO winInfo;
	GetWindowInfo(hWnd, &winInfo);// 获取窗口信息
	////根据配置信息对窗口重新进行装饰(url、title、style、dwStyle在createBrower时处理了）
	if (!configs.icon.empty()){
		HICON hIcon = GetIcon(configs.url, configs.icon);
		if (hIcon)SetClassLong(hWnd, GCL_HICON, (LONG)hIcon);
	}
	
	bool showMax = false;
	if (configs.max){// 需要按最大化的方式来显示
		jw::maxWin(hWnd); 
		showMax = true;
	}
	else{// 检查坐标和宽高是否与设定相同，如果不同则重新设
		RECT rc = winInfo.rcWindow;
		if ((p_configs->x != -1 && p_configs->x != rc.left)
			|| (p_configs->y != -1 && p_configs->y != rc.top)
			|| (p_configs->w != -1 && p_configs->w != (rc.right - rc.left))
			|| (p_configs->h != -1 && p_configs->h != (rc.bottom - rc.top))
			){
			jw::setBound(hWnd
				, (p_configs->x != -1 ? p_configs->x : rc.left)
				, (p_configs->y != -1 ? p_configs->y : rc.top)
				, (p_configs->w != -1 ? p_configs->w : (rc.right - rc.left))
				, (p_configs->h != -1 ? p_configs->h : (rc.bottom - rc.top))
				);
		}
	}
	HWND bWnd = GetNextWindow(hWnd, GW_CHILD);// 得到真实的浏览器窗口
	LONG preWndProc = GetWindowLongPtr(bWnd, GWLP_WNDPROC);
	if (preWndProc != (LONG)JWebTop_BrowerWndProc){
		SetWindowLongPtr(bWnd, GWLP_WNDPROC, (LONG)JWebTop_BrowerWndProc);
		BrowserWindowInfo * bwInfo = new BrowserWindowInfo();
		bwInfo->isWsChild = (WS_CHILD&p_configs->dwStyle) !=0;
		bwInfo->hWnd = hWnd;
		bwInfo->bWnd = bWnd;
		bwInfo->oldBrowserProc = preWndProc;
		preWndProc = GetWindowLongPtr(hWnd, GWLP_WNDPROC);
		bwInfo->oldMainProc = preWndProc;
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)JWebTop_WindowWndProc);// 替换主窗口的消息处理函数
		bwInfo->browser = browser;
		bwInfo->enableDrag = configs.enableDrag;
		BrowserWindowInfos.insert(pair<HWND, BrowserWindowInfo*>(bWnd, bwInfo));// 在map常量中记录下hWnd和之前WndProc的关系
		BrowserWindowInfos.insert(pair<HWND, BrowserWindowInfo*>(hWnd, bwInfo));// 在map常量中记录下hWnd和之前WndProc的关系
	}
#ifdef JWebTopLog
	GetWindowInfo(hWnd, &winInfo);// 获取窗口信息
	RECT rc = winInfo.rcWindow;
	wstringstream wss;
	wss << L"窗口句柄信息 windowWnd  =" << hWnd << L"\r\n"
		<< L"           browserWnd =" << bWnd << L"\r\n"
		<< L"           renderWnd  =" << GetNextWindow(bWnd, GW_CHILD) << L"\r\n"  //
		<< L"           parentWin  =" << configs.parentWin << L"\r\n"
		<< L"           winRect    =" << L"x:" << rc.left << L",y:" << rc.top << L",w:" << (rc.right - rc.left) << L",h:" << (rc.bottom - rc.top) << L"\r\n";
	writeLog(wss.str());
#endif 
	if (!showMax)ShowWindow(hWnd, SW_NORMAL);
}
namespace jb{

	void checkAndSetResizeAblity(HWND hWnd){
		DWORD style = GetWindowLong(hWnd, GWL_STYLE);
		if (style & WS_BORDER){// 窗口自带了边框，不用特殊的处理
			return;
		}
		if (style & WS_CHILD){// 作为子窗口嵌入，不用特殊的处理
			return;
		}
		BrowserWindowInfo * bwInfo = getBrowserWindowInfo(hWnd);
		if (bwInfo->rWnd != NULL) return;// 已进行过特殊处理
		HWND rWnd = GetNextWindow(bwInfo->bWnd, GW_CHILD);// 得到真绘制器窗口
		if (rWnd == NULL)return;
		BrowserWindowInfos.insert(pair<HWND, BrowserWindowInfo*>(rWnd, bwInfo));// 在map常量中记录下hWnd和之前WndProc的关系
		LONG preWndProc = GetWindowLongPtr(rWnd, GWLP_WNDPROC);
		bwInfo->oldRenderProc = preWndProc;
		SetWindowLongPtr(rWnd, GWLP_WNDPROC, (LONG)JWebTop_RenderWndProc);// 替换主窗口的消息处理函数
		bwInfo->rWnd = rWnd;
	}
}