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

#define BTN_SEND			101
#define BTN_CLEAR_SEND		102
#define BTN_CLEAR_RECEIVE	103

#define TXT_Msg				201
#define TXT_Sended			202
#define TXT_Received		203

#define MARGIN				10
#define BAR_HEIGHT			24
#define BTN_WIDTH			120

int startIPCServer();
void _onRead();


// 以下方法实现在【wincomm.cpp】文件-----------------------------------------------
// 获取窗口的标题
LPTSTR getWindowTitle();
// 创建窗口函数
BOOL createWin(HINSTANCE hInstance);
// 在接收消息的文本框中追加收到消息
void appendReceied(std::wstring received);
// 在发送消息的文本框中追加发送了的消息
void appendSended(std::wstring sended);


// 以下方法实现在【ipccom.cpp】文件------------------------------------------------
// 发送消息
DWORD sendMsg(std::wstring msg);
// 用于窗口创建后执行其他操作。比如启动服务器
void windowCreated(HWND hWnd);