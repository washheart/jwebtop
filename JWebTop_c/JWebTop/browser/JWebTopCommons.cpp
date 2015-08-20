#include "JWebTopCommons.h"
#include <string>
#include "JWebTopClient.h"
#include "include/cef_app.h"
#include "include/cef_parser.h"
#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"
#include "JWebTop/wndproc/JWebTopWndProc.h"
#include "JWebTopContext.h"
#include "common/util/StrUtil.h"
#include "common/task/Task.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
using namespace std;
const char kTestMessageName[] = "close";
bool excuteJSON(const CefString& request){
	CefRefPtr<CefValue> v = CefParseJSON(request, JSON_PARSER_RFC);          // 进行解析
	if (v == NULL){
		//callback->Failure(1, CefString(L"错误的JSON格式"));
		return false;
	}
#ifdef JWebTopLog 
	wstringstream log;
	log << L"JWebTopCommons#Handler#OnQuery jsons=" << request.ToWString() << L"\r\n";
	writeLog(log.str());
#endif
	CefRefPtr<CefDictionaryValue> value = v->GetDictionary();// 按JSON字典来获取
	wstring methodName = value->GetString("m").ToWString();  // 取出某字符串 
	 if (methodName == L"loadUrl"){
		CefRefPtr<CefValue> handler = value->GetValue("handler");
		if (handler == NULL)return false;
		CefRefPtr<CefValue> url = value->GetValue("url");
		if (url == NULL)return false;
		jb::loadUrl((HWND)handler->GetInt(), url->GetString().ToWString());
	}
	else if (methodName == L"reload"){
		CefRefPtr<CefValue> handler = value->GetValue("handler");
		if (handler == NULL)return false;
		jb::reload((HWND)handler->GetInt());
	}
	else if (methodName == L"reloadIgnoreCache"){
		CefRefPtr<CefValue> handler = value->GetValue("handler");
		if (handler == NULL)return false;
		jb::reloadIgnoreCache((HWND)handler->GetInt());
	}
	else if (methodName == L"runApp"){
		CefRefPtr<CefValue> handler = value->GetValue("handler");
		if (handler == NULL)return false;
		CefRefPtr<CefValue> app = value->GetValue("app");
		CefRefPtr<CefValue> parentWin = value->GetValue("parentWin");
		jb::runApp((HWND)handler->GetInt(), app->GetString().ToWString(), parentWin == NULL ? 0 : parentWin->GetInt()
			,NULL,NULL,NULL,-1,-1,-1,-1);
	}
	else if (methodName == L"runAppMore"){
		CefRefPtr<CefValue> handler = value->GetValue("handler");
		if (handler == NULL)return false;
		CefRefPtr<CefValue> app = value->GetValue("app");
		CefRefPtr<CefValue> parentWin = value->GetValue("parentWin");
		CefRefPtr<CefValue> url = value->GetValue("url");
		CefRefPtr<CefValue> title = value->GetValue("title");
		CefRefPtr<CefValue> icon = value->GetValue("icon");
		CefRefPtr<CefValue> x = value->GetValue("x");
		CefRefPtr<CefValue> y = value->GetValue("y");
		CefRefPtr<CefValue> w = value->GetValue("w");
		CefRefPtr<CefValue> h = value->GetValue("h");
		jb::runApp((HWND)handler->GetInt(), app->GetString().ToWString(), parentWin == NULL ? 0 : parentWin->GetInt()
			, (url == NULL ? NULL : LPTSTR(url->GetString().ToWString().c_str()))
			, (title == NULL ? NULL : LPTSTR(title->GetString().ToWString().c_str()))
			, (icon == NULL ? NULL : LPTSTR(icon->GetString().ToWString().c_str()))
			, (x == NULL ? -1 : x->GetInt())
			, (y == NULL ? -1 : y->GetInt())
			, (w == NULL ? -1 : w->GetInt())
			, (h == NULL ? -1 : h->GetInt()));
	}
	else if(methodName == L"close"){
		CefRefPtr<CefValue> handler = value->GetValue("handler");
		if (handler == NULL)return false;
		jb::close((HWND)handler->GetInt());
	}
	else if (methodName == L"showDev"){
		CefRefPtr<CefValue> handler = value->GetValue("handler");
		if (handler == NULL)return false;
		jb::showDev((HWND)handler->GetInt());
	}
	else{
		return false;
	}
	return true;
}
namespace jc/*jc=JWebTop Client*/{
	class Handler : public CefMessageRouterBrowserSide::Handler {
	public:
		Handler() {}

		// 响应render进程发送过来的消息
		virtual bool OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id,
			const CefString& request,// request为消息JSON对象{request:msg}的msg部分（可以扩展一个JSON工具来进行解析）【解析代码：cef_message_router.cc=》CefMessageRouterRendererSideImpl】
			bool persistent, CefRefPtr<Callback> callback) OVERRIDE{
			return excuteJSON(request);
		}
	};
	void CreateMessageHandlers(MessageHandlerSet& handlers) {
		handlers.insert(new Handler());
	}
}
namespace jw{
	extern JWebTopConfigs * g_configs;  // 应用启动时的第一个配置变量
}

void createNewBrowser(JWebTopConfigs * configs){
	createNewBrowser(configs, wstring());
}
void createNewBrowser(JWebTopConfigs * configs,wstring taskId){
	CEF_REQUIRE_UI_THREAD();
	if (configs == NULL)configs = jw::ctx::getDefaultConfigs();
	// 用于构建本地窗口的配置信息
	CefWindowInfo window_info;
#ifdef JWebTopLog 
	std::wstringstream log;
	log << L"processId=" << GetCurrentProcessId() << L" threadId=" << GetCurrentThreadId() << " title=" << configs->name.ToWString() << L"\r\n";
	log << L"JWebTopCommons createNewBrowser parentWin=" << configs->parentWin << L"\r\n";
	writeLog(log.str());
#endif
#if defined(OS_WIN)
	// On Windows we need to specify certain flags that will be passed to
	window_info.SetAsPopup((HWND)configs->parentWin, configs->name);
#endif
	if (configs->dwStyle != 0)window_info.style = configs->dwStyle;
	if (configs->dwExStyle != 0)window_info.style = configs->dwExStyle;
	if (configs->x != -1)window_info.x = configs->x;
	if (configs->y != -1)window_info.y = configs->y;
	if (configs->w != -1)window_info.width = configs->w;
	if (configs->h != -1)window_info.height = configs->h;
	CefRefPtr<JWebTopClient>  handler = new JWebTopClient();
	handler->setJWebTopConfigs(configs);
	handler->setTaskId(taskId);
	CefBrowserSettings browser_settings;
	// 创建浏览器窗口
	CefBrowserHost::CreateBrowser(window_info, handler.get(), configs->getAbsolutePath(configs->url.ToWString()), browser_settings, NULL);
}