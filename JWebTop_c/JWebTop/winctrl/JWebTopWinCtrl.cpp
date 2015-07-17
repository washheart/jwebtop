#include "JWebTopWinCtrl.h"

#include <sstream>
#include <string>
#include "JWebTop/browser/JWebTopCommons.h"
#include "JWebTop/util/StrUtil.h"
#include "JWebTop/tests/TestUtil.h"
using namespace std;
BrowerWindowInfoMap BrowerWindowInfos;// 在静态变量中缓存所有已创建的窗口信息

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

BrowerWindowInfo getBrowserWindowInfo(HWND hWnd){
	BrowerWindowInfo bwInfo;
	BrowerWindowInfoMap::iterator it = BrowerWindowInfos.find(hWnd);
	if (BrowerWindowInfos.end() != it) {
		bwInfo = it->second;
	}
	return bwInfo;
}
// 用于调试工具窗口的cefclient（如果传入JWebTopHandler会把对JWebTop的窗口设置在DEBUG上也应用一次）
class DEBUG_Handler : public CefClient{ IMPLEMENT_REFCOUNTING(DEBUG_Handler); };
LRESULT CALLBACK JWebTop_BrowerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	BrowerWindowInfo bwInfo = getBrowserWindowInfo(hWnd);
	// 可以对必要的信息进行预先处理，需要拦截的消息就可以不用发给浏览器了
	switch (message) {
	case WM_CLOSE:
		BrowerWindowInfos.erase(hWnd);// 当窗口关闭时，清理数据
		break;
	case WM_LBUTTONDOWN:// 鼠标左键
		bwInfo.isDraging = true;
		bwInfo.dragX = LOWORD(lParam);
		bwInfo.dragY = HIWORD(lParam);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		bwInfo.isDraging = false;
		break;
	case WM_MOUSEMOVE:{
						  POINT pt;
						  GetCursorPos(&pt);
						  RECT rc,hrc;
						  GetWindowRect(hWnd, &rc);
						  GetWindowRect(bwInfo.hWnd, &hrc);
						  int x = pt.x - bwInfo.dragX+hrc.left;
						  int y = pt.y - bwInfo.dragY+hrc.top;
#ifdef WebTopLog
						  stringstream s;
						  s << "   mv[x=" << x << ",y=" << y << "] cased_mid:" << message << "\r\n";
						  writeLog(s.str());
#endif
						  // 下面两种方式都可以拖动
						  //MoveWindow(bwInfo.hWnd, x, y, rc.right, rc.bottom, true);
						  SetWindowPos(bwInfo.hWnd, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
						  break;
	}
	case WM_KEYUP:
		if (wParam == VK_F12){// 按下F12时打开调试工具(有时会触发异常)
			if (bwInfo.configs.enableDebug){
				CefPoint(pp);
				pp.x = 300;
				pp.y = 300;
				CefWindowInfo windowInfo;
				CefBrowserSettings settings;
				windowInfo.SetAsPopup(NULL, "cef_debug");
				CefRefPtr<CefBrowserHost> host = bwInfo.browser->GetHost();
				host->ShowDevTools(windowInfo, new DEBUG_Handler(), settings, pp);
			}
		}
		else if (wParam == VK_F11){// 按下F12时打开调试工具
			createNewBrowser(NULL);
		}
		break;
	case WM_PARENTNOTIFY:
		break;
	}
	return CallWindowProc((WNDPROC)getBrowserWindowInfo(hWnd).oldProc, hWnd, message, wParam, lParam);
}

// 根据配置信息(configs)对顶层窗口和实际浏览器窗口进行修饰
void renderBrowserWindow(CefRefPtr<CefBrowser> browser, JWebTopConfigs configs){
	HWND hWnd = browser->GetHost()->GetWindowHandle();// 浏览器所在窗口的handle
	WINDOWINFO winInfo;
	GetWindowInfo(hWnd, &winInfo);// 获取窗口信息
	bool changed = false;
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
	if (changed){// 如果窗口的状态发生了改变
		//RECT rc;
		//GetWindowRect(hWnd, &rc);// 获取窗口大小（如果窗口有边框和标题栏，则会带上的）
		//MoveWindow(bWnd, 0, 0, rc.right - rc.left, rc.bottom - rc.top, true);// 这样移动之后有可能会造成浏览器的实际大小“稍稍”大于窗口可视面积，但影响不大
	}
	LONG preWndProc = GetWindowLongPtr(bWnd, GWLP_WNDPROC);
	if (preWndProc != (LONG)JWebTop_BrowerWndProc){
		SetWindowLongPtr(bWnd, GWLP_WNDPROC, (LONG)JWebTop_BrowerWndProc);
		//SetWindowLongPtr(hWnd, GWLP_USERDATA, preWndProc);// 估计CEF有使用GWLP_USERDATA，所以这样设置不管用，反而会导致出错
		BrowerWindowInfo bwInfo;
		bwInfo.hWnd = hWnd;
		bwInfo.bWnd = bWnd;
		bwInfo.oldProc = preWndProc;
		bwInfo.browser = browser;
		bwInfo.configs = configs;
		BrowerWindowInfos.insert(std::pair<HWND, BrowerWindowInfo>(bWnd, bwInfo));// 在map常量中记录下hWnd和之前WndProc的关系
	}
}