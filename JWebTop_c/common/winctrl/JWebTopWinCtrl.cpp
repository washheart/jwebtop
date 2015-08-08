#include "JWebTopWinCtrl.h"

#include "common/util/StrUtil.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
using namespace std;

namespace jw{
	// setToCenter(handler);// 让窗口显示在屏幕正中
	void setCenter(HWND hWnd){
		RECT rt;
		GetWindowRect(hWnd, &rt);
		MoveWindow(hWnd, rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top, FALSE);
	}

	// setToCenter(handler);// 让窗口显示在屏幕正中
	void setCenter(HWND hWnd, int x, int y, int w, int h){
		int sw = GetSystemMetrics(SM_CXFULLSCREEN);
		int sh = GetSystemMetrics(SM_CYFULLSCREEN);// +GetSystemMetrics(SM_CYCAPTION);
		if (sw <= w){
			x = 0;
			w = sw;
		}
		else{
			x = (sw - w) / 2;
		}
		if (sh < h){
			y = 0;
			h = sh;
		}
		else{
			y = (sh - h) / 2;
		}
		MoveWindow(hWnd, x, y, w, h, FALSE);
	}
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
		//setBound(hWnd, 0, 0, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN) + GetSystemMetrics(SM_CYCAPTION));
		ShowWindow(hWnd, SW_MAXIMIZE);
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
	
	//setWindowStyle(exStyle, handler);//高级函数，设置窗口额外属性，诸如置顶之类。
	void setWindowStyle(HWND hWnd, int exStyle){
		HWND hWndTemp = hWnd;
		DWORD dwExStyle = GetWindowLong(hWndTemp, GWL_EXSTYLE);//获取旧样式  
		exStyle |= dwExStyle;//按位与将旧扩展样式去掉  
		SetWindowLong(hWndTemp, GWL_EXSTYLE, exStyle);//设置新的扩展样式  
	}
}