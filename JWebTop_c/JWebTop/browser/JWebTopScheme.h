#ifndef CEF_JWEBTOP_Scheme_H_
#define CEF_JWEBTOP_Scheme_H_
#include <set>
#include "include/cef_app.h"
#include "include/wrapper/cef_message_router.h"
namespace jw{
	namespace jb{// jb=jwebtop browser：浏览器相关的一些工具类和方法

		// 获取请求链接对应的mime类型
		std::string GetMimeType(const std::string& url);

		// 注册jwebtop://协议到浏览器，主要用于读取本地文件
		void RegisterSchemeHandlers();
	}
}
#endif  // CEF_JWEBTOP_APP_H_