#include "JWebTopWinCtrl.h"

#include <sstream>
#include <string>
#include "JWebTop/browser/JWebTopCommons.h"
#include "JWebTop/util/StrUtil.h"
#include "JWebTop/tests/TestUtil.h"
using namespace std;
BrowserWindowInfoMap BrowserWindowInfos;// 在静态变量中缓存所有已创建的窗口信息

HICON GetIcon(CefString url, CefString path){
	if (path.ToWString().find(L":") == -1){// 如果指定的路径是相对路径
		wstring _path;
		_path = url.ToWString();
		replace_allW(_path, L"\\", L"/");
		_path = _path.substr(0, _path.find_last_of('/') + 1);
		path = _path.append(path);
	}
	return (HICON)::LoadImage(NULL, path.ToWString().data(), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
}

BrowserWindowInfo * getBrowserWindowInfo(HWND hWnd){
	BrowserWindowInfoMap::iterator it = BrowserWindowInfos.find(hWnd);
	if (BrowserWindowInfos.end() != it) {
		return it->second;
	}
	return NULL;
}
// 用于调试工具窗口的cefclient（如果传入JWebTopHandler会把对JWebTop的窗口设置在DEBUG上也应用一次）
class DEBUG_Handler : public CefClient{ IMPLEMENT_REFCOUNTING(DEBUG_Handler); };
// 拦截浏览器窗口的消息
LRESULT CALLBACK JWebTop_BrowerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	BrowserWindowInfo * bwInfo = getBrowserWindowInfo(hWnd);
	// 可以对必要的信息进行预先处理，需要拦截的消息就可以不用发给浏览器了stringstream s;
	stringstream sss;
	sss << "mid [x=" << message << ",bwInfo=" << bwInfo << "]" << "\r\n";
	writeLog(sss.str());
	switch (message) {
		//case WM_CLOSE:case WM_RBUTTONDOWN:case WM_LBUTTONDOWN:break;// 窗口创建/销毁、鼠标左右键按下监听不到，需要通过PARENTNOTIFY方式
	case WM_PARENTNOTIFY:
	{

							UINT msg2 = LOWORD(wParam);
							// 如果有多个的话可以考虑用switch方式
							if (msg2 == WM_LBUTTONDOWN){
								if (bwInfo->configs.enableDrag){// 是否允许拖动
									POINT pt;
									GetCursorPos(&pt);
									bwInfo->isDraging = true;
									bwInfo->dragX = LOWORD(lParam);
									bwInfo->dragY = HIWORD(lParam);
								}
							}
							else if (msg2 == WM_DESTROY){
								SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)bwInfo->oldProc);// 设置回原来的处理函数
								BrowserWindowInfos.erase(hWnd);// 清理掉在map中的数据
							}
	}
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		bwInfo->isDraging = false;
		break;
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
	case WM_KEYUP:
		if (wParam == VK_F12){// 按下F12时打开调试工具(有时会触发异常)
			if (bwInfo->configs.enableDebug){
				CefPoint(pp);
				pp.x = 300;
				pp.y = 300;
				CefWindowInfo windowInfo;
				CefBrowserSettings settings;
				windowInfo.SetAsPopup(NULL, "cef_debug");
				CefRefPtr<CefBrowserHost> host = bwInfo->browser->GetHost();
				host->ShowDevTools(windowInfo, new DEBUG_Handler(), settings, pp);
			}
		}
		else if (wParam == VK_F11){// 按下F12时打开调试工具
			createNewBrowser(NULL);
		}
		break;
	}
	return CallWindowProc((WNDPROC)bwInfo->oldProc, hWnd, message, wParam, lParam);
}

// 根据配置信息(configs)对顶层窗口和实际浏览器窗口进行修饰
void renderBrowserWindow(CefRefPtr<CefBrowser> browser, JWebTopConfigs configs){
	HWND hWnd = browser->GetHost()->GetWindowHandle();// 浏览器所在窗口的handle
	WINDOWINFO winInfo;
	GetWindowInfo(hWnd, &winInfo);// 获取窗口信息
	bool changed = false;
#ifdef WebTopLog
	stringstream ss;
	DWORD dd = WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	ss << "dwStyle=====" << dd << "\r\n";
	DWORD dx = WS_EX_TOOLWINDOW | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
	ss << "dwExStyle===" << dx << "\r\n";
	writeLog(ss.str());
#endif 
	//根据配置信息对窗口重新进行装饰(url、title在createBrower时处理了）
	if (configs.dwStyle != 0){
		DWORD dwStyle = winInfo.dwStyle;
		dwStyle &= configs.dwStyle;//按位与将旧样式去掉  
		SetWindowLong(hWnd, GWL_STYLE, dwStyle);//设置成新的样式
		changed = true;
	}
	if (configs.dwExStyle != 0){
		DWORD  dwExStyle = winInfo.dwExStyle;
		dwExStyle &= configs.dwExStyle;//按位与将旧扩展样式去掉  
		SetWindowLong(hWnd, GWL_EXSTYLE, dwExStyle);//设置新的扩展样式  
		changed = true;
	}
	if (!configs.icon.empty()){
		HICON hIcon = GetIcon(configs.url, configs.icon);
		SetClassLong(hWnd, GCL_HICON, (LONG)hIcon);
	}
	if (configs.x != -1 || configs.y != -1 || configs.w != -1 || configs.h != -1){
		RECT rc = winInfo.rcWindow;
		SetWindowPos(hWnd, HWND_TOPMOST,
			configs.x == -1 ? rc.left : configs.x, configs.y == -1 ? rc.top : configs.y,
			configs.w == -1 ? rc.right : configs.w, configs.h == -1 ? rc.bottom : configs.h,
			SWP_ASYNCWINDOWPOS | SWP_FRAMECHANGED | SWP_NOCOPYBITS | SWP_NOZORDER);
		changed = true;
	}
	HWND bWnd = GetNextWindow(hWnd, GW_CHILD);// 得到真实的浏览器窗口
	//if (changed){// 如果窗口的状态发生了改变
	//	//RECT rc;
	//	//GetWindowRect(hWnd, &rc);// 获取窗口大小（如果窗口有边框和标题栏，则会带上的）
	//	//MoveWindow(bWnd, 0, 0, rc.right - rc.left, rc.bottom - rc.top, true);// 这样移动之后有可能会造成浏览器的实际大小“稍稍”大于窗口可视面积，但影响不大
	//}
	LONG preWndProc = GetWindowLongPtr(bWnd, GWLP_WNDPROC);
	if (preWndProc != (LONG)JWebTop_BrowerWndProc){
		SetWindowLongPtr(bWnd, GWLP_WNDPROC, (LONG)JWebTop_BrowerWndProc);
		BrowserWindowInfo * bwInfo = new BrowserWindowInfo();
		bwInfo->hWnd = hWnd;
		bwInfo->bWnd = bWnd;
		bwInfo->oldProc = preWndProc;
		bwInfo->browser = browser;
		bwInfo->configs = configs;
		BrowserWindowInfos.insert(pair<HWND, BrowserWindowInfo*>(bWnd, bwInfo));// 在map常量中记录下hWnd和之前WndProc的关系
	}
}