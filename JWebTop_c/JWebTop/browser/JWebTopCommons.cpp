#include "JWebTopCommons.h"
#include "JWebTopClient.h"
#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"
#include <string>
#include "JWebTop/winctrl/JWebTopWinCtrl.h"
#include  "JWebTop/json/json.h"
#include "JWebTop/util/StrUtil.h"
#ifdef JWebTopLog
#include "JWebTop/tests/TestUtil.h"
#endif
using namespace std;

const char kTestMessageName[] = "close";
namespace jc/*jc=JWebTop Client*/{
	class Handler : public CefMessageRouterBrowserSide::Handler {
	public:
		Handler() {}

		// 响应render进程发送过来的消息
		virtual bool OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id,
			const CefString& request,// request为消息JSON对象{request:msg}的msg部分（可以扩展一个JSON工具来进行解析）【解析代码：cef_message_router.cc=》CefMessageRouterRendererSideImpl】
			bool persistent, CefRefPtr<Callback> callback) OVERRIDE{
#ifdef JWebTopLog
			wstringstream log;
			log << L"JWebTopCommons#Handler#OnQuery jsons=" << request.ToWString() << L"\r\n";
			writeLog(log.str());
#endif
			stringstream jsons;
			jsons << wch2chr(request.ToWString().c_str());

			Json::Reader reader;
			Json::Value value;
			if (!reader.parse(jsons, value))return false;
			string methodName = value["m"].asString();
			if (methodName == "close"){
				Json::Value handler = value["handler"];
				if (handler == NULL)return false;
				jw::close((HWND)handler.asInt());
			}
			else if (methodName == "loadUrl"){
				Json::Value handler = value["handler"];
				if (handler == NULL)return false;
				Json::Value url = value["url"];
				if (url == NULL)return false;
				jw::loadUrl((HWND)handler.asInt(), chr2wch(url.asString().c_str()));
			}
			else if (methodName == "reload"){
				Json::Value handler = value["handler"];
				if (handler == NULL)return false;
				jw::reload((HWND)handler.asInt());
			}
			else if (methodName == "reloadIgnoreCache"){
				Json::Value handler = value["handler"];
				if (handler == NULL)return false;
				jw::reloadIgnoreCache((HWND)handler.asInt());
			}
			else if (methodName == "runApp"){
				Json::Value app = value["app"];
				Json::Value parentWin = value["parentWin"];
				jw::runApp(chr2wch(app.asString().c_str()), parentWin == NULL ? 0 : parentWin.asInt());
			}
			else if (methodName == "showDev"){
				Json::Value handler = value["handler"];
				if (handler == NULL)return false; 
				jw::showDev((HWND)handler.asInt());
			}			
			return true;
		}
	};
	void CreateMessageHandlers(MessageHandlerSet& handlers) {
		handlers.insert(new Handler());
	}
}

CefRefPtr<JWebTopClient> g_handler;// 全局保留一个JWebTopClient即可

extern JWebTopConfigs * tmpConfigs; // 创建过程中在多个上下文中共享的变量
extern JWebTopConfigs * g_configs;  // 应用启动时的第一个配置变量

void createNewBrowser(JWebTopConfigs * configs){
	CEF_REQUIRE_UI_THREAD();
	if (configs == NULL){
		tmpConfigs = g_configs;
	}
	else if (tmpConfigs != configs){
		if (configs != g_configs) delete tmpConfigs;
		tmpConfigs = configs;
	}
	// 用于构建本地窗口的配置信息（注：有些信息[style、exStyle]设置了不太管用，可以在Handler的OnAfterCreated中对窗口进行修饰）
	CefWindowInfo window_info;

#ifdef JWebTopLog
	wstringstream ss;
	ss << L"JWebTopCommons createNewBrowser parentWin=" << tmpConfigs->parentWin << L"\r\n";
	writeLog(ss.str());
#endif
#if defined(OS_WIN)
	// On Windows we need to specify certain flags that will be passed to
	window_info.SetAsPopup((HWND)tmpConfigs->parentWin, tmpConfigs->name);
#endif
	if (g_handler == NULL)g_handler = new JWebTopClient();
	CefBrowserSettings browser_settings;
	//window_info.window_name = UUID::Data1();// 用UUID作为窗口名称，建立窗口名与JWebTopConfigs的关联？？？
	// 创建浏览器窗口
	CefBrowserHost::CreateBrowser(window_info, g_handler.get(), tmpConfigs->getAbsolutePath(tmpConfigs->url.ToWString()), browser_settings, NULL);
}