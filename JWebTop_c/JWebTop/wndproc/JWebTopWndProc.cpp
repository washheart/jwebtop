#include "JWebTopWndProc.h"

#include <thread>
#include <WindowsX.h>
#include "JWebTop/browser/JWebTopCommons.h"
#include "common/util/StrUtil.h"
#include <comdef.h>
#include <algorithm>
#include <gdiplus.h>
#include "JWebTop/jshandler/JJH_Windows.h"
#include "JWebTop/jshandler/JJH_Files.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif

#pragma comment(lib,"GdiPlus.lib")
using namespace std;
BrowserWindowInfoMap BrowserWindowInfos;// �ھ�̬�����л��������Ѵ����Ĵ�����Ϣ

BrowserWindowInfo * getBrowserWindowInfo(HWND hWnd){
	BrowserWindowInfoMap::iterator it = BrowserWindowInfos.find(hWnd);
	if (BrowserWindowInfos.end() != it) {
		return it->second;
	}
	return NULL;
}

namespace jb{
	//close(handler);// �رմ���
	void close(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL){
			if (bw->isWsChild){
				// DestroyWindow(hWnd);// ���ܹر����������
				SetParent(hWnd, NULL); // ������Ҫ�ȰѸ���������ΪNULL��Ȼ���ٹر�
			}
			bw->browser->GetHost()->CloseBrowser(true);
		}
	}

	//loadUrl(url, handler);//������ҳ��urlΪ��ҳ·��
	void loadUrl(HWND hWnd, std::wstring url){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL)bw->browser->GetMainFrame()->LoadURL(url);
	}
	//reload(handler);//���¼��ص�ǰҳ��
	void reload(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw != NULL)bw->browser->Reload();
	}

	//reloadIgnoreCache(handler);//���¼��ص�ǰҳ�沢���Ի���
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
	//showDev(handler);//�򿪿����߹���
	void showDev(HWND hWnd){
		thread t(__showDev, hWnd);
		t.detach();// �������߳����������߹��ߣ������JS�߳�����һ��
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
		// ���㲢�����������
		bw->dragX = pt.x - rt.left;
		bw->dragY = pt.y - rt.top;
		bw->isDraging = true;// ���Ϊ��ʼ�϶�
	}
	void stopDrag(HWND hWnd){
		BrowserWindowInfo * bw = getBrowserWindowInfo(hWnd);
		if (bw == NULL)return;
		bw->isDraging = false;// ֹͣ�϶�
	}
}

HICON GetIcon(CefString url, CefString path){
	if (path.ToWString().find(L":") == -1){// ���ָ����·�������·��
		wstring _path;
		_path = url.ToWString();
		jw::str::replace_allW(_path, L"\\", L"/");
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
		Gdiplus::GdiplusShutdown(m_gdiplusToken);// �ر�gdi
		return hIcon;
	}
	return NULL;
}

void parseCopyFile(HWND hWnd, HDROP hDrop, string eventName){
	UINT cFiles = 0;
	// ��ȡ���а�����
	cFiles = DragQueryFile(hDrop, (UINT)-1, NULL, 0);
	if (cFiles > 0){			
		TCHAR szFile[MAX_PATH];
		std::vector<CefString> files;
		for (UINT count = 0; count < cFiles; count++){
			DragQueryFile(hDrop, count, szFile, sizeof(szFile));
			CefString file = wstring(szFile);
			files.push_back(file);
		}// End for-count:cFiles
		jw::js::events::sendFileEvent(getBrowserWindowInfo(hWnd)->browser->GetMainFrame(), files, eventName);
	}// End if:���а������ļ�
}// End-method:parseCopyFile

// �����а����Ƿ����ļ�����������������¼�
void parseCopyFile(HWND hWnd){	
	if (OpenClipboard(hWnd)){
		HDROP hDrop = HDROP(GetClipboardData(CF_HDROP));
		if (hDrop){
			parseCopyFile(hWnd, hDrop, "JWebTopFilePasted");
		}// End if:�ܷ��ȡ���а�����
		CloseClipboard();
	}// End if:���а��Ƿ�������
}// End-method:parseCopyFile

const LONG border_width = 5; // ����߿�Ŀ��
LRESULT transparentNCHITTEST(HWND hWnd, LPARAM lParam){
	RECT wRect;
	GetWindowRect(hWnd, &wRect);
	long x = GET_X_LPARAM(lParam);
	long y = GET_Y_LPARAM(lParam);
	if (x >= wRect.left + border_width && y >= wRect.top + border_width
		&&x <= wRect.right - border_width &&y <= wRect.bottom - border_width){
		return HTCLIENT;// ����ڣ����¼���ģ��ͻ�����
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
		if (x < wRect.right && x >= xRB) return HTTOPRIGHT;		// ���Ͻ�		
		if (x >= wRect.left && x < xLT)  return HTTOPLEFT;		// ���Ͻ�
		return HTTOP;											// �ϱ�
	}
	if (y < wRect.bottom && y >= yRB){
		if (x < wRect.right && x >= xRB) return HTBOTTOMRIGHT;	// ���½�
		if (x >= wRect.left && x < xLT)  return HTBOTTOMLEFT;	// ���½�
		return                                  HTBOTTOM;		// �±�
	}
	if (x >= wRect.left && x < xLT)      return HTLEFT;			// ���
	if (x < wRect.right && x >= xRB)	 return HTRIGHT;		// �ұ�
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
// ���������ڵ���Ϣ����Щ��Ϣ������رմ��ڡ��ƶ����ڵȣ�ֻ���������ڲ���������
LRESULT CALLBACK JWebTop_WindowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	BrowserWindowInfo * bwInfo = getBrowserWindowInfo(hWnd);
	// ���ԶԱ�Ҫ����Ϣ����Ԥ�ȴ�����Ҫ���ص���Ϣ�Ϳ��Բ��÷����������stringstream s;
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
		BrowserWindowInfos.erase(hWnd);// �������map�е�����
		break;
	}// End switch-message
	if (bwInfo == NULL)return DefWindowProc(hWnd, message, wParam, lParam);
	return CallWindowProc((WNDPROC)bwInfo->oldMainProc, hWnd, message, wParam, lParam);
}
// ������������ڵ���Ϣ
LRESULT CALLBACK JWebTop_BrowerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	BrowserWindowInfo * bwInfo = getBrowserWindowInfo(hWnd);
	// ���ԶԱ�Ҫ����Ϣ����Ԥ�ȴ�����Ҫ���ص���Ϣ�Ϳ��Բ��÷����������stringstream s;
	switch (message) {
	case WM_KILLFOCUS:
		bwInfo->isDraging = false; // ʧȥ����ʱ��ֹͣ�϶�
		break;
	case WM_DROPFILES:
		parseCopyFile(hWnd, (HDROP)wParam, "JWebTopFileDroped");
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case 0x56:
			if (0x8000 & GetKeyState(VK_CONTROL))parseCopyFile(hWnd);
			break;
#ifdef JWebTopLog
		case VK_F12:
			jb::showDev(hWnd);
			break;
#endif
		}
		break;
	case WM_MOUSEMOVE:{
						  if (bwInfo->isDraging){
							  POINT pt;
							  GetCursorPos(&pt);
							  LONG x = pt.x - bwInfo->dragX;
							  LONG y = pt.y - bwInfo->dragY;
							  SetWindowPos(bwInfo->hWnd, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);// �ƶ����ڣ������ı䴰�ڴ�С�ʹ������ڲ��
						  }
	}
		break;
	case WM_NCHITTEST:
		if (bwInfo->rWnd != NULL)return transparentNCHITTEST(hWnd, lParam);
		break;
	case WM_NCDESTROY:
		BrowserWindowInfos.erase(hWnd);// �������map�е�����
		break;
	case WM_PARENTNOTIFY:
	{
							UINT msg2 = LOWORD(wParam);
							if (msg2 == WM_DESTROY){
								if (bwInfo == NULL){
									DefWindowProc(hWnd, message, wParam, lParam);
								} else{
									SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)bwInfo->oldBrowserProc);// ���û�ԭ���Ĵ�����
									BrowserWindowInfos.erase(hWnd);// �������map�е�����
								}
							}
	}
		break;
	}// End switch-message
	if (bwInfo == NULL)return DefWindowProc(hWnd, message, wParam, lParam);
	return CallWindowProc((WNDPROC)bwInfo->oldBrowserProc, hWnd, message, wParam, lParam);
}

// ����������Ϣ(configs)�Զ��㴰�ں�ʵ����������ڽ�������
void renderBrowserWindow(CefRefPtr<CefBrowser> browser, JWebTopConfigs * p_configs){
	JWebTopConfigs configs = (*p_configs);
	HWND hWnd = browser->GetHost()->GetWindowHandle();// ��������ڴ��ڵ�handle
	WINDOWINFO winInfo;
	GetWindowInfo(hWnd, &winInfo);// ��ȡ������Ϣ
	////����������Ϣ�Դ������½���װ��(url��title��style��dwStyle��createBrowerʱ�����ˣ�
	if (!configs.icon.empty()){
		HICON hIcon = GetIcon(configs.url, configs.icon);
		if (hIcon)SetClassLong(hWnd, GCL_HICON, (LONG)hIcon);
	}

	bool showMax = false;
	if (configs.max){// ��Ҫ����󻯵ķ�ʽ����ʾ
		jw::maxWin(hWnd);
		showMax = true;
	} else{// �������Ϳ���Ƿ����趨��ͬ�������ͬ��������
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
	HWND bWnd = GetNextWindow(hWnd, GW_CHILD);// �õ���ʵ�����������
	LONG preWndProc = GetWindowLongPtr(bWnd, GWLP_WNDPROC);
	if (preWndProc != (LONG)JWebTop_BrowerWndProc){
		SetWindowLongPtr(bWnd, GWLP_WNDPROC, (LONG)JWebTop_BrowerWndProc);
		BrowserWindowInfo * bwInfo = new BrowserWindowInfo();
		bwInfo->isWsChild = (WS_CHILD&p_configs->dwStyle) != 0;
		bwInfo->hWnd = hWnd;
		bwInfo->bWnd = bWnd;
		bwInfo->oldBrowserProc = preWndProc;
		preWndProc = GetWindowLongPtr(hWnd, GWLP_WNDPROC);
		bwInfo->oldMainProc = preWndProc;
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)JWebTop_WindowWndProc);// �滻�����ڵ���Ϣ������
		bwInfo->browser = browser;
		bwInfo->enableDrag = configs.enableDrag;
		BrowserWindowInfos.insert(pair<HWND, BrowserWindowInfo*>(bWnd, bwInfo));// ��map�����м�¼��hWnd��֮ǰWndProc�Ĺ�ϵ
		BrowserWindowInfos.insert(pair<HWND, BrowserWindowInfo*>(hWnd, bwInfo));// ��map�����м�¼��hWnd��֮ǰWndProc�Ĺ�ϵ
		DragAcceptFiles(bwInfo->bWnd, TRUE);// ����Ϊ���Խ����Ͻ����ļ�
		//DropFileFix();
	}
#ifdef JWebTopLog
	GetWindowInfo(hWnd, &winInfo);// ��ȡ������Ϣ
	RECT rc = winInfo.rcWindow;
	wstringstream wss;
	wss << L"���ھ����Ϣ windowWnd  =" << hWnd << L"\r\n"
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
		if (style & WS_BORDER){// �����Դ��˱߿򣬲�������Ĵ���
			return;
		}
		if (style & WS_CHILD){// ��Ϊ�Ӵ���Ƕ�룬��������Ĵ���
			return;
		}
		BrowserWindowInfo * bwInfo = getBrowserWindowInfo(hWnd);
		if (bwInfo->rWnd != NULL) return;// �ѽ��й����⴦��
		HWND rWnd = GetNextWindow(bwInfo->bWnd, GW_CHILD);// �õ������������
		if (rWnd == NULL)return;
		BrowserWindowInfos.insert(pair<HWND, BrowserWindowInfo*>(rWnd, bwInfo));// ��map�����м�¼��hWnd��֮ǰWndProc�Ĺ�ϵ
		LONG preWndProc = GetWindowLongPtr(rWnd, GWLP_WNDPROC);
		bwInfo->oldRenderProc = preWndProc;
		SetWindowLongPtr(rWnd, GWLP_WNDPROC, (LONG)JWebTop_RenderWndProc);// �滻�����ڵ���Ϣ������
		bwInfo->rWnd = rWnd;
	}
}