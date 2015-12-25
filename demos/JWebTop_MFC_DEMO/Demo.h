//////////////////////////////////////////////////////////////////////////  
/// 定义测试fatipc服务器与客户端通信时需要的窗口、变量、以及公共类
///  
/// 源码：https://github.com/washheart/jwebtop
/// 说明：https://github.com/washheart/jwebtop/wiki
/// @version	1.0 
/// @author		washheart@163.com
/// @date       2015-10-22  
////////////////////////////////////////////////////////////////////////// 
#pragma once
#include <Windows.h>
#include <string>
#include <sstream>
#include "jwebtopex/jwebtopex.h"

#define BTN_ADD_NOTE		101
#define BTN_DEL_NOTE		102

#define MARGIN				10
#define BAR_HEIGHT			24
#define BTN_WIDTH			120

// 创建窗口函数
void createWin(HINSTANCE hInstance);

class DemoCtrl :JWebTopAppInited, JWebTopBrowserCreated, JWebTopJSReturn, JWebtopJSONDispater {
private:
	JWebTopContext * ctx = NULL;
	HWND listBrowser = NULL;
	HWND detailBrowser = NULL;

	void _startJWebTop();
public :
	void init();

	wstring getListAppFile();	// 得到配置文件
	wstring getDetailAppFile();	// 得到配置文件
	wstring getJWebTopExe();	// 得到JWebTop.exe的路径
	HWND getListBrowser(){ return this->listBrowser; };
	HWND getDetailBrowser(){ return this->detailBrowser; };
	void addNote(wstring note);
	void delNote();

	void onJWebTopAppInited();
	void onJWebTopBrowserCreated(long browserHWnd);
	void onJWebTopJSReturn(wstring jsonString);
	wstring onDispatcher(long browserHWnd, wstring json);

};