#include "JWebTopCommons.h"
#include "JWebTopClient.h"
#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"
#include <string>
#include "JWebTop/winctrl/JWebTopWinCtrl.h"
#include "JWebTop/util/StrUtil.h"
#include "include/cef_parser.h"
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
			cef_json_parser_options_t options = cef_json_parser_options_t();// 解析时的配置
			CefRefPtr<CefValue> v= CefParseJSON(request, options);          // 进行解析
			if (v == NULL){
				callback->Failure(1, CefString(L"错误的JSON格式"));
				return false;
			}
#ifdef JWebTopLog
			wstringstream log;
			log << L"JWebTopCommons#Handler#OnQuery jsons=" << request.ToWString() << L"\r\n";
			writeLog(log.str());
#endif
			CefRefPtr<CefDictionaryValue> value = v->GetDictionary();// 按JSON字典来获取
			wstring methodName = value->GetString("m").ToWString();  // 取出某字符串 
			if (methodName == L"close"){
				CefRefPtr<CefValue> handler = value->GetValue("handler");
				if (handler == NULL)return false;
				jw::close((HWND)handler->GetInt());
			}
			else if (methodName == L"loadUrl"){
				CefRefPtr<CefValue> handler = value->GetValue("handler");
				if (handler == NULL)return false;
				CefRefPtr<CefValue> url = value->GetValue("url");
				if (url == NULL)return false;
				jw::loadUrl((HWND)handler->GetInt(), url->GetString().ToWString());
			}
			else if (methodName == L"reload"){
				CefRefPtr<CefValue> handler = value->GetValue("handler");
				if (handler == NULL)return false;
				jw::reload((HWND)handler->GetInt());
			}
			else if (methodName == L"reloadIgnoreCache"){
				CefRefPtr<CefValue> handler = value->GetValue("handler");
				if (handler == NULL)return false;
				jw::reloadIgnoreCache((HWND)handler->GetInt());
			}
			else if (methodName == L"runApp"){
				CefRefPtr<CefValue> app = value->GetValue("app");
				CefRefPtr<CefValue> parentWin = value->GetValue("parentWin");
				jw::runApp(app->GetString().ToWString(), parentWin == NULL ? 0 : parentWin->GetInt());
			}
			else if (methodName == L"showDev"){
				CefRefPtr<CefValue> handler = value->GetValue("handler");
				if (handler == NULL)return false; 
				jw::showDev((HWND)handler->GetInt());
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