#ifndef CEF_JWEBTOP_COMMONS_H_
#define CEF_JWEBTOP_COMMONS_H_
#include <set>
#include "include\cef_client.h"
#include "include/wrapper/cef_message_router.h"
#include "JWebTop/config/JWebTopConfigs.h"

// 新建一个浏览器窗口
void createNewBrowser(JWebTopConfigs * configs);

bool excuteJSON(const CefString& request);

namespace jc/*jc=JWebTop Client*/{
	typedef std::set<CefMessageRouterBrowserSide::Handler*> MessageHandlerSet;
	void CreateMessageHandlers(MessageHandlerSet& handlers);

	void exitCef();
}
#endif