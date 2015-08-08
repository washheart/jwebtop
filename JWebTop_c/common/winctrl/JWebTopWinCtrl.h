#ifndef CEF_JWEBTOP_WIN_CTRL_H_
#define CEF_JWEBTOP_WIN_CTRL_H_
#include <Windows.h>
#include <sstream>
#include <string>


namespace jw{
	void setCenter(HWND hWnd);                             // setToCenter(handler);// 让窗口显示在屏幕正中
	void setCenter(HWND hWnd, int x, int y, int w, int h); // setToCenter(handler);// 让窗口显示在屏幕正中
	POINT getPos(HWND hWnd);                               // getPos(handler);//获得窗口位置，返回值为一object，格式如下{x:13,y:54}
	void setSize(HWND hWnd, int w, int h);                 // setSize(x, y, handler);//设置窗口大小
	POINT getSize(HWND hWnd);                              // getSize(handler);//获得窗口大小，返回值为一object，格式如下{width:130,height:54}
	POINT getScreenSize();                                 // getScreenSize();//获取屏幕大小，返回值为一object，格式如下{width:130,height:54}
	void move(HWND hWnd, int x, int y);                    // move(x, y, handler);//移动窗口
	void setBound(HWND hWnd, int x, int y, int w, int h);  // setBound(x, y,w ,h, handler);// 同时设置窗口的坐标和大小
	RECT getBound(HWND hWnd);                              // getBound(handler);//获取窗口的位置和大小，返回值为一object，格式如下{x:100,y:100,width:130,height:54}
	void setTitle(HWND hWnd, std::wstring title);          // setTitle(title, handler);// 设置窗口名称
	std::wstring getTitle(HWND hWnd);                      // getTitle(handler);// 获取窗口名称，返回值为一字符串

	void bringToTop(HWND hWnd);	// bringToTop(handler);//窗口移到最顶层
	void focus(HWND hWnd);	    // focus(handler);//使窗口获得焦点
	void hide(HWND hWnd);	    // hide(handler);//隐藏窗口
	void max(HWND hWnd);	    // max(handler);//最大化窗口
	void mini(HWND hWnd);	    // mini(hander);//最小化窗口
	void restore(HWND hWnd);	// restore(handler);//还原窗口，对应于hide函数
	void setTopMost(HWND hWnd);	// setTopMost(handler);//窗口置顶，此函数跟bringToTop的区别在于此函数会使窗口永远置顶，除非有另外一个窗口调用了置顶函数

	void setWindowStyle(HWND hWnd, int exStyle);	//setWindowStyle(exStyle, handler);//高级函数，设置窗口额外属性，诸如置顶之类。
}
#endif