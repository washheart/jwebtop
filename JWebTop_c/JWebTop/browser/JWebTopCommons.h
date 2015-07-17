#ifndef CEF_JWEBTOP_COMMONS_H_
#define CEF_JWEBTOP_COMMONS_H_
#include "include\cef_client.h"
#include "JWebTop/winctrl/JWebTopConfigs.h"
// 一些浏览器相关的通用和变量方法

// 新建一个浏览器窗口
void createNewBrowser(LPCTSTR appDefFile);

// 新建一个浏览器窗口
void createNewBrowser(JWebTopConfigs configs);

#endif