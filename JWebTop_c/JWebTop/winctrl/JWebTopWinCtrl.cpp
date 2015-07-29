#include "JWebTopWinCtrl.h"

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
// 拦截主窗口的消息：有些消息（比如关闭窗口、移动窗口等）只有在主窗口才能侦听到
LRESULT CALLBACK JWebTop_WindowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	BrowserWindowInfo * bwInfo = getBrowserWindowInfo(hWnd);
	// 可以对必要的信息进行预先处理，需要拦截的消息就可以不用发给浏览器了stringstream s;
	switch (message) {
	case WM_SIZE:
	{
					stringstream js_event;
					js_event << "var e = new CustomEvent(' ',{"// AlloyDesktopWindowResize
						<< "	detail:{"
						<< "		width:" << LOWORD(lParam) << ","
						<< "		height:" << HIWORD(lParam)
						<< "	}"
						<< "});"
						<< "dispatchEvent(e);";
					bwInfo->browser->GetMainFrame()->ExecuteJavaScript(js_event.str(), "", 0);
					break;
	}// End case-WM_SIZE
	case WM_MOVE:
	{
					stringstream js_event;
					js_event << "var e = new CustomEvent('JWebTopMove',{"// AlloyDesktopWindowMove
						<< "	detail:{"
						<< "		x:" << LOWORD(lParam) << ","
						<< "		y:" << HIWORD(lParam)
						<< "	}"
						<< "});"
						<< "dispatchEvent(e);";
					bwInfo->browser->GetMainFrame()->ExecuteJavaScript(js_event.str(), "", 0);
					break;
	}// End case-WM_MOVE
	case WM_ACTIVATE:
	{
						stringstream js_event;
						js_event << "var e = new CustomEvent('JWebTopWindowActive',{"// AlloyDesktopWindowActive
							<< "	detail:{"
							<< "		handler:0x" << bwInfo->hWnd
							<< "       ,w:'main'"
							<< "	}"
							<< "});"
							<< "dispatchEvent(e);";
						bwInfo->browser->GetMainFrame()->ExecuteJavaScript(js_event.str(), "", 0);
						break;
	}// End case-WM_ACTIVATE
	case WM_ACTIVATEAPP:
	{
						   stringstream js_event;
						   js_event << "var e = new CustomEvent('JWebTopActive',{"
							   << "	detail:{"
							   << "		handler:0x" << bwInfo->hWnd
							   << "       ,w:'main'"
							   << "	}"
							   << "});"
							   << "dispatchEvent(e);";
						   bwInfo->browser->GetMainFrame()->ExecuteJavaScript(js_event.str(), "", 0);
						   break;
	}// End case-WM_ACTIVATEAPP

	}// End switch-message
	return CallWindowProc((WNDPROC)bwInfo->oldMainProc, hWnd, message, wParam, lParam);
}
// 拦截浏览器窗口的消息
LRESULT CALLBACK JWebTop_BrowerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	BrowserWindowInfo * bwInfo = getBrowserWindowInfo(hWnd);
	// 可以对必要的信息进行预先处理，需要拦截的消息就可以不用发给浏览器了stringstream s;
	switch (message) {
	case WM_PARENTNOTIFY:
	{
#ifdef JWebTopLog
							stringstream sss;
							sss << "mid WM_PARENTNOTIFY[wParam=" << wParam << ",lParam=" << lParam << "]" << "\r\n";
							writeLog(sss.str());
#endif
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
								SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)bwInfo->oldBrowserProc);// 设置回原来的处理函数
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
		if (wParam == VK_F1){// 按下F12时打开调试工具(F12有时会触发异常，暂时改为F1)
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
	}// End switch-message
	return CallWindowProc((WNDPROC)bwInfo->oldBrowserProc, hWnd, message, wParam, lParam);
}

// 根据配置信息(configs)对顶层窗口和实际浏览器窗口进行修饰
void renderBrowserWindow(CefRefPtr<CefBrowser> browser, JWebTopConfigs * p_configs){
	JWebTopConfigs configs = (* p_configs);
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
		bwInfo->oldBrowserProc = preWndProc;
		preWndProc = GetWindowLongPtr(hWnd, GWLP_WNDPROC);
		bwInfo->oldMainProc = preWndProc;
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)JWebTop_WindowWndProc);// 替换主窗口的消息处理函数
		bwInfo->browser = browser;
		bwInfo->configs = configs;
		BrowserWindowInfos.insert(pair<HWND, BrowserWindowInfo*>(bWnd, bwInfo));// 在map常量中记录下hWnd和之前WndProc的关系
		BrowserWindowInfos.insert(pair<HWND, BrowserWindowInfo*>(hWnd, bwInfo));// 在map常量中记录下hWnd和之前WndProc的关系
	}
#ifdef JWebTopLog
	stringstream ss;
	ss << "hWnd===" << hWnd << ",bWnd=" << bWnd << ",rWnd=<<" << GetNextWindow(bWnd, GW_CHILD) //
		<< ",pWnd=" << ::GetAncestor(browser->GetHost()->GetWindowHandle(), GA_ROOT)
		<< ",aWnd1=" << ::GetAncestor(hWnd, GA_ROOT)
		<< ",aWnd2=" << ::GetAncestor(bWnd, GA_ROOT)
		<< "\r\n";
	writeLog(ss.str());
#endif 
}

namespace jw{
	// getPos(handler);//获得窗口位置，返回值为一object，格式如下{x:13,y:54}
	POINT getPos(HWND hWnd){
		RECT rt = getBound(hWnd);
		POINT p;
		p.x = rt.left;
		p.y = rt.top;
		return p;
	}
	// setSize(x, y, handler);//设置窗口大小
	void setSize(HWND hWnd, int w, int h){
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER);// 只改变窗口大小，不改变窗口坐标和窗口所在层次
	}
	// getSize(handler);//获得窗口大小，返回值为一object，格式如下{width:130,height:54}
	POINT getSize(HWND hWnd){
		RECT rt = getBound(hWnd);
		POINT p;
		p.x = rt.right;
		p.y = rt.bottom;
		return p;
	}
	// getScreenSize();//获取屏幕大小，返回值为一object，格式如下{width:130,height:54}
	POINT getScreenSize(){
		POINT p;
		p.x = GetSystemMetrics(SM_CXFULLSCREEN);
		p.y = GetSystemMetrics(SM_CYFULLSCREEN) + GetSystemMetrics(SM_CYCAPTION);
		return p;
	}
	// move(x, y, handler);//移动窗口
	void move(HWND hWnd, int x, int y){
		::SetWindowPos(hWnd, HWND_TOPMOST, x, y, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOSIZE | SWP_NOZORDER | SWP_NOSENDCHANGING);// 移动窗口，但不改变窗口大小和窗口所在层次
	}
	// setBound(x, y,w ,h, handler);// 同时设置窗口的坐标和大小
	void setBound(HWND hWnd, int x, int y, int w, int h){
		::MoveWindow(hWnd, x, y, w, h, false);
	}
	// 获取窗口的位置和大小，返回值为一object，格式如下{x:100,y:100,width:130,height:54}
	RECT getBound(HWND hWnd){
		RECT rt;
		GetWindowRect(hWnd, &rt);
		return rt;
	}
	// setTitle(title, handler);// 设置窗口名称
	void setTitle(HWND hWnd, wstring title){
		SetWindowText(hWnd, title.c_str());
	}
	// getTitle(handler);// 获取窗口名称，返回值为一字符串
	wstring getTitle(HWND hWnd){
		TCHAR title[1000];// 声明并初始化
		::GetWindowText(hWnd, title, sizeof(title));// 获取窗口名称
		return title;
	}
	// bringToTop(handler);//窗口移到最顶层
	void bringToTop(HWND hWnd){
		SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	}
	// focus(handler);//使窗口获得焦点
	void focus(HWND hWnd){
		SetFocus(hWnd);
	}
	// hide(handler);//隐藏窗口
	void hide(HWND hWnd){
		ShowWindow(hWnd, SW_HIDE);
	}
	// max(handler);//最大化窗口
	void max(HWND hWnd){
		setBound(hWnd, 0, 0, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN) + GetSystemMetrics(SM_CYCAPTION));
	}
	// mini(hander);//最小化窗口
	void mini(HWND hWnd){
		if (GetWindowLong(hWnd, GWL_EXSTYLE)&WS_EX_TOOLWINDOW){
			ShowWindow(hWnd, SW_HIDE);
		}
		else{
			ShowWindow(hWnd, SW_MINIMIZE);
		}
	}
	// restore(handler);//还原窗口，对应于hide函数
	void restore(HWND hWnd){
		ShowWindow(hWnd, SW_RESTORE);
	}
	// setTopMost(handler);//窗口置顶，此函数跟bringToTop的区别在于此函数会使窗口永远置顶，除非有另外一个窗口调用了置顶函数
	void setTopMost(HWND hWnd){
		DWORD dwExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);//获取旧样式  
		DWORD dwNewExStyle = WS_EX_TOPMOST;
		dwNewExStyle |= dwExStyle;//按位与将旧扩展样式去掉  
		SetWindowLong(hWnd, GWL_EXSTYLE, dwNewExStyle);//设置新的扩展样式
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	//close(handler);// 关闭窗口
	void close(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL)bw->browser->GetHost()->CloseBrowser(true);
	}

	//setWindowStyle(exStyle, handler);//高级函数，设置窗口额外属性，诸如置顶之类。
	void setWindowStyle(HWND hWnd, int exStyle){
		HWND hWndTemp = hWnd;
		DWORD dwExStyle = GetWindowLong(hWndTemp, GWL_EXSTYLE);//获取旧样式  
		exStyle |= dwExStyle;//按位与将旧扩展样式去掉  
		SetWindowLong(hWndTemp, GWL_EXSTYLE, exStyle);//设置新的扩展样式  
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
	//reloadIgnoreCache(handler);//重新加载当前页面并忽略缓存
	void reloadIgnoreCache(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL)bw->browser->ReloadIgnoreCache();
	}
	//showDev(handler);//打开开发者工具
	void showDev(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL){
			CefWindowInfo windowInfo;
			windowInfo.SetAsPopup(NULL, "cef_debug");
			bw->browser->GetHost()->ShowDevTools(windowInfo, new DEBUG_Handler(), CefBrowserSettings(), CefPoint());
		}
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
}