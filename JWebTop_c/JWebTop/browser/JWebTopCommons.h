#ifndef CEF_JWEBTOP_COMMONS_H_
#define CEF_JWEBTOP_COMMONS_H_
#include <set>
#include <string>
#include "include\cef_client.h"
#include "include/wrapper/cef_message_router.h"
#include "JWebTop/config/JWebTopConfigs.h"

// 新建一个浏览器窗口
void createNewBrowser(JWebTopConfigs * configs);
void createNewBrowser(JWebTopConfigs * configs, std::wstring taskId);
namespace jc/*jc=JWebTop Client*/{
	typedef std::set<CefMessageRouterBrowserSide::Handler*> MessageHandlerSet;
	void CreateBrowserSideMessageHandlers(MessageHandlerSet& handlers);
}
#endif