#ifndef CEF_JWEBTOP_COMMONS_H_
#define CEF_JWEBTOP_COMMONS_H_
#include <set>
#include "include\cef_client.h"
#include "include/wrapper/cef_message_router.h"
#include "JWebTop/winctrl/JWebTopConfigs.h"
// 一些浏览器相关的通用和变量方法

// 新建一个浏览器窗口
void createNewBrowser(LPCTSTR appDefFile);

// 新建一个浏览器窗口
void createNewBrowser(JWebTopConfigs configs);

namespace jc/*jc=JWebTop Client*/{
	typedef std::set<CefMessageRouterBrowserSide::Handler*> MessageHandlerSet;
	void CreateMessageHandlers(MessageHandlerSet& handlers);
}
#endif