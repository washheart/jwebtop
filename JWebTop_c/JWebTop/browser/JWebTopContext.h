// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#ifndef CEF_JWEBTOP_BROWSER_CONTEXT_H_
#define CEF_JWEBTOP_BROWSER_CONTEXT_H_

#include <list>
#include <set>
#include "include/cef_client.h"
#include "include/wrapper/cef_message_router.h"
#include "JWebTop/config/JWebTopConfigs.h"
namespace jw{
	namespace ctx{
		//// 根据JSON来配置和启动JWebTop
		//void startJWebTopByJSON(std::wstring cfgJson);
		// 根据文件来配置和启动JWebTop
		void startJWebTopByFile(std::wstring cfgFile);
		// 关闭cef相关的设置
		void closeJWebtopContext();
		
		JWebTopConfigs * getDefaultConfigs();
					 
		void setDefaultConfigs(JWebTopConfigs *);

		int getBrowserCount();
		void addBrowser(CefRefPtr<CefBrowser> browser);

		void removeBrowser(CefRefPtr<CefBrowser> browser);

		// 关闭所有打开的浏览器
		void CloseAllBrowsers(bool force_close);

		// 保存所有创建了的浏览器
		typedef std::list<CefRefPtr<CefBrowser> > BrowserList;
	}
}
#endif  // CEF_JWEBTOP_BROWSER_CONTEXT_H_
