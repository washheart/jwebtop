#pragma once
#ifndef CEF_JWEBTOP_WIN_CTRL_H_
#define CEF_JWEBTOP_WIN_CTRL_H_
#include <Windows.h>
#include <string>

namespace jw{
	_declspec(dllexport) void  setCenter(HWND hWnd);                             // setToCenter(handler);// 让窗口显示在屏幕正中
	_declspec(dllexport) void  setCenter(HWND hWnd, int x, int y, int w, int h); // setToCenter(handler);// 让窗口显示在屏幕正中
	_declspec(dllexport) POINT getPos(HWND hWnd);                                // getPos(handler);//获得窗口位置，返回值为一object，格式如下{x:13,y:54}
	_declspec(dllexport) void  setSize(HWND hWnd, int w, int h);                 // setSize(x, y, handler);//设置窗口大小
	_declspec(dllexport) POINT getSize(HWND hWnd);                               // getSize(handler);//获得窗口大小，返回值为一object，格式如下{width:130,height:54}
	_declspec(dllexport) POINT getScreenSize();                                  // getScreenSize();//获取屏幕大小，返回值为一object，格式如下{width:130,height:54}
	_declspec(dllexport) void  move(HWND hWnd, int x, int y);                    // move(x, y, handler);//移动窗口
	_declspec(dllexport) void  setBound(HWND hWnd, int x, int y, int w, int h);  // setBound(x, y,w ,h, handler);// 同时设置窗口的坐标和大小
	_declspec(dllexport) RECT  getBound(HWND hWnd);                              // getBound(handler);//获取窗口信息，返回值为的左上角和右下角的屏幕坐标
	_declspec(dllexport) void  setTitle(HWND hWnd, std::wstring title);          // setTitle(title, handler);// 设置窗口名称
	_declspec(dllexport) std::wstring getTitle(HWND hWnd);                       // getTitle(handler);// 获取窗口名称，返回值为一字符串

	_declspec(dllexport) void bringToTop(HWND hWnd);		// bringToTop(handler);//窗口移到最顶层
	_declspec(dllexport) void focus(HWND hWnd);				// focus(handler);//使窗口获得焦点
	_declspec(dllexport) void showWindow(HWND hWnd, int);	// 设置窗口显示状态
	_declspec(dllexport) void hide(HWND hWnd);				// hide(handler);//隐藏窗口
	_declspec(dllexport) void maxWin(HWND hWnd);		    // max(handler);//最大化窗口
	_declspec(dllexport) void mini(HWND hWnd);				// mini(hander);//最小化窗口
	_declspec(dllexport) void restore(HWND hWnd);			// restore(handler);//还原窗口，对应于hide函数
	_declspec(dllexport) BOOL isVisible(HWND hWnd);			// 窗口是否在显示
	_declspec(dllexport) void setTopMost(HWND hWnd);		// setTopMost(handler);//窗口置顶，此函数跟bringToTop的区别在于此函数会使窗口永远置顶，除非有另外一个窗口调用了置顶函数

	_declspec(dllexport) void setWindowStyle(HWND hWnd, int style);	// setWindowStyle(exStyle, handler);//高级函数，设置窗口额外属性，诸如置顶之类。
	_declspec(dllexport) void setWindowExStyle(HWND hWnd, int exStyle);  // setWindowExStyle(exStyle, handler)
}
#endif