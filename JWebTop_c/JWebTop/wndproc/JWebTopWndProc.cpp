#include "JWebTopWndProc.h"

#include "JWebTop/browser/JWebTopCommons.h"

#include "common/JWebTopMsg.h"
#include "common/util/StrUtil.h"
#include "common/process/MultiProcess.h"
#include "common/winctrl/JWebTopWinCtrl.h"
#include "common/task/Task.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif

using namespace std;
BrowserWindowInfoMap BrowserWindowInfos;// 在静态变量中缓存所有已创建的窗口信息

extern JWebTopConfigs * g_configs;
extern JWebTopConfigs * tmpConfigs;

BrowserWindowInfo * getBrowserWindowInfo(HWND hWnd){
	BrowserWindowInfoMap::iterator it = BrowserWindowInfos.find(hWnd);
	if (BrowserWindowInfos.end() != it) {
		return it->second;
	}
	return NULL;
}

extern HWND g_RemoteWinHWnd;
namespace jb{
	bool sendJWebTopProcessMsg(HWND browserHWnd, DWORD msgId, LPTSTR msg){
		BrowserWindowInfo * bw = getBrowserWindowInfo(browserHWnd);
		if (bw == NULL)return false;
		if (g_RemoteWinHWnd == NULL)return false;
		return jw::sendProcessMsg(g_RemoteWinHWnd, msgId, msg);
	}

	CefString invokeRemote_Wait(HWND browserHWnd, CefString json){
		wstring taskId = jw::task::createTaskId();			         // 生成任务id
		// taskId附加到json字符串上
		wstring newjson = json.ToWString();
		wstring wrapped = jw::wrapAsTaskJSON((long)browserHWnd, std::ref(taskId), std::ref(newjson));
		jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // 放置任务到任务池
		if (sendJWebTopProcessMsg(browserHWnd, JWM_DLL_EXECUTE_WAIT, LPTSTR(wrapped.c_str()))){ // 发送任务到远程进程
			lock->wait();		             		 		             // 等待任务完成
			wstring result = lock->result;   		 		             // 取回执行结果
			return CefString(result);									 // 返回数据
		}
		else{
			jw::task::removeTask(taskId);								// 消息发送失败移除现有消息
			return CefString();											// 返回数据：注意这里是空字符串
		}
	}

	void invokeRemote_NoWait(HWND browserHWnd, CefString json){
		sendJWebTopProcessMsg(browserHWnd, JWM_DLL_EXECUTE_RETURN, LPTSTR(json.ToWString().c_str())); // 发送任务到远程进程
	}
	//close(handler);// 关闭窗口
	void close(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL)bw->browser->GetHost()->CloseBrowser(true);
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
	void CreateNewBrowserThread(){
		createNewBrowser(tmpConfigs);
	}
	void runApp(HWND hWnd, std::wstring appDefFile, long parentWin){
#ifdef JWebTopLog 
		std::wstringstream log;
		log << L"run app=" << appDefFile << L",parentWin=" << parentWin << L"\r\n";
		writeLog(log.str());
#endif
		//if (tmpConfigs != g_configs)delete tmpConfigs;
		tmpConfigs = JWebTopConfigs::loadConfigs(JWebTopConfigs::getAppDefFile(appDefFile.c_str()));
		tmpConfigs->parentWin = parentWin;
		/*createNewBrowser(tmpConfigs);*/
		//DWORD  dwThreadId = 0; // 记录线程的id
		////LPTSTR str = L"abcd";
		//HANDLE threadHandle = // 记录线程的handler
		//CreateThread(NULL, 0, CreateNewBrowserThread, NULL, NULL, &dwThreadId);
		////Sleep(200);
		//CloseHandle(threadHandle);
		thread t(CreateNewBrowserThread);
		t.detach();
	}


	void browserwin_thread_executeWmCopyData(HWND browserHWnd, DWORD msgId, wstring msg){
		BrowserWindowInfo * bwInfo = getBrowserWindowInfo(browserHWnd);		
		switch (msgId)
		{
		case JWM_RESULT_RETURN:// 远程任务已完成，结果发回来了，需要通知本进程的等待线程去获取结果
		{
								   wstring taskId, result;
								   long remoteHWnd;
								   jw::parseMessageJSON(msg, ref(remoteHWnd), ref(taskId), ref(result));  // 从任务信息中解析出任务id和任务描述
								   jw::task::putTaskResult(taskId, result);	   					   // 通知等待线程，远程任务已完成，结果已去取回
								   break;
		}
		case JWM_JSON_EXECUTE_WAIT:// 远程进程发来一个任务，并且远程进程正在等待，任务完成后需要发送JWEBTOP_MSG_RESULT_RETURN消息给远程进程
		case JWM_JS_EXECUTE_WAIT:// 远程进程发来一个任务，并且远程进程正在等待，任务完成后需要发送JWEBTOP_MSG_RESULT_RETURN消息给远程进程
		{

									 CefRefPtr<CefProcessMessage> cefMsg = CefProcessMessage::Create("waitjs");
									 CefRefPtr<CefListValue> args = cefMsg->GetArgumentList();
									 args->SetInt(0, msgId);
									 args->SetString(1, msg);
									 bwInfo->browser->SendProcessMessage(PID_RENDERER, cefMsg);// 直接发送到render进程去执行
									 break;
		}
		case JWM_JSON_EXECUTE_RETURN:
			msg = L"invokeByDLL(" + msg + L")";// 包装json为js调用 
			bwInfo->browser->GetMainFrame()->ExecuteJavaScript(msg, "", 0);
			break;
		case JWM_JS_EXECUTE_RETURN:
			bwInfo->browser->GetMainFrame()->ExecuteJavaScript(msg, "", 0);
			break;
		default:
			break;
		}
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
	return (HICON)::LoadImage(NULL, path.ToWString().data(), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
}
LRESULT onBrowserWinWmCopyData(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	wstring msg;
	DWORD msgId = 0;
	jw::parseProcessMsg(lParam, ref(msgId), ref(msg));
	std::thread t(jb::browserwin_thread_executeWmCopyData, hWnd, msgId, msg);// onWmCopyData是同步消息，为了防止另一进程的等待，这里在新线程中进行业务处理
	t.detach();// 从当前线程分离
	return JWEBTOP_MSG_SUCCESS;
}
// 拦截主窗口的消息：有些消息（比如关闭窗口、移动窗口等）只有在主窗口才能侦听到
LRESULT CALLBACK JWebTop_WindowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	BrowserWindowInfo * bwInfo = getBrowserWindowInfo(hWnd);
	// 可以对必要的信息进行预先处理，需要拦截的消息就可以不用发给浏览器了stringstream s;
	switch (message) {
	case WM_COPYDATA:{
						 return	 onBrowserWinWmCopyData(hWnd, message, wParam, lParam);

	}
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
							sss << "BrowerWndProc hWnd=" << hWnd << " WM_PARENTNOTIFY[wParam=" << wParam << ",lParam=" << lParam << "]" << "\r\n";
							writeLog(sss.str());
#endif
							UINT msg2 = LOWORD(wParam);
							// 如果有多个的话可以考虑用switch方式
							if (msg2 == WM_LBUTTONDOWN){
								if (bwInfo->enableDrag){// 是否允许拖动
									POINT pt;
									GetCursorPos(&pt);
									bwInfo->isDraging = true;
									bwInfo->dragX = LOWORD(lParam);
									bwInfo->dragY = HIWORD(lParam);
								}
							}
							else if (msg2 == WM_DESTROY){
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
	case WM_KEYUP:{
					  if (wParam == VK_F1){//
						  //if (bwInfo->configs.enableDebug){
						  CefPoint(pp);
						  pp.x = 300;
						  pp.y = 300;
						  CefWindowInfo windowInfo;
						  CefBrowserSettings settings;
						  windowInfo.SetAsPopup(NULL, "cef_debug");
						  CefRefPtr<CefBrowserHost> host = bwInfo->browser->GetHost();
						  host->ShowDevTools(windowInfo, new DEBUG_Handler(), settings, pp);
						  //}
					  }
					  else if (wParam == VK_F11){// 
						  createNewBrowser(NULL);
					  }
					  break;
	}break;
	}// End switch-message
	return CallWindowProc((WNDPROC)bwInfo->oldBrowserProc, hWnd, message, wParam, lParam);
}

// 根据配置信息(configs)对顶层窗口和实际浏览器窗口进行修饰
void renderBrowserWindow(CefRefPtr<CefBrowser> browser, JWebTopConfigs * p_configs){
	JWebTopConfigs configs = (*p_configs);
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
		changed = true;
	}
	if (configs.max){// 需要按最大化的方式来显示
		jw::max(hWnd);
	}
	else if (configs.parentWin != 0){// 没有指定父窗口时，在桌面正中显示窗口
		if (configs.x == -1 || configs.y == -1){
			RECT rc = winInfo.rcWindow;
			jw::setCenter(hWnd
				, configs.x == -1 ? rc.left : configs.x
				, configs.y == -1 ? rc.top : configs.y
				, rc.right - rc.left, rc.bottom - rc.top);
		}
	}
	else if (changed){// 如果窗口风格有改变，重绘下窗口
		RECT rc = winInfo.rcWindow;
		SetWindowPos(hWnd, HWND_TOPMOST, rc.left, rc.top, rc.right, rc.bottom, SWP_ASYNCWINDOWPOS | SWP_FRAMECHANGED | SWP_NOCOPYBITS | SWP_NOZORDER);
	}
	HWND bWnd = GetNextWindow(hWnd, GW_CHILD);// 得到真实的浏览器窗口
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
		bwInfo->enableDrag = configs.enableDrag;
		BrowserWindowInfos.insert(pair<HWND, BrowserWindowInfo*>(bWnd, bwInfo));// 在map常量中记录下hWnd和之前WndProc的关系
		BrowserWindowInfos.insert(pair<HWND, BrowserWindowInfo*>(hWnd, bwInfo));// 在map常量中记录下hWnd和之前WndProc的关系
		if (configs.msgWin != 0) {
			std::wstringstream wss;
			wss << L"@{\"action\":\"browser\",\"method\":\"setBrowserHwnd\",\"msg\":\"浏览器已创建\",\"value\":{\"hwnd\":" << (LONG)hWnd << L"}}";
			jb::sendJWebTopProcessMsg(hWnd, 0, LPTSTR(wss.str().c_str()));
		}
	}
#ifdef JWebTopLog
	stringstream ss;
	ss << "hWnd===" << hWnd << ",bWnd=" << bWnd << ",rWnd=<<" << GetNextWindow(bWnd, GW_CHILD) //
		<< ",parentWin=" << configs.parentWin
		<< ",msgWin=" << configs.msgWin
		<< "\r\n";
	writeLog(ss.str());
#endif 
}