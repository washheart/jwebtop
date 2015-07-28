#include "JWebTopCommons.h"
#include "JWebTopClient.h"
#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"
#include <string>
#include "JWebTop/winctrl/JWebTopWinCtrl.h"
#include  "JWebTop/json/json.h"
#include "JWebTop/util/StrUtil.h"

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
			stringstream jsons;
			jsons << wch2chr(request.ToWString().c_str());

			Json::Reader reader;
			Json::Value value;
			if (!reader.parse(jsons, value))return false;
			string methodName = value["m"].asString();
			if (methodName == "close"){
				Json::Value handler=value["handler"];
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
			else if (methodName == "showDev"){
				Json::Value handler = value["handler"];
				if (handler == NULL)return false; 
				jw::showDev((HWND)handler.asInt());
			}
			string name = value["name"].asString();
			//CefRefPtr<CefValue> json = CefParseJSON(request, NULL);
			//// 解析以逗号分隔的其他参数
			//const std::string& message_name = request;
			//std::vector<int> vec;
			//const std::string& vals = message_name.substr(sizeof(kTestMessageName));std::stringstream ss(vals);int i;
			//while (ss >> i) {vec.push_back(i);if (ss.peek() == ',')ss.ignore();}
			//jw::close((HWND)vec[0]);// 这种方式，获取到的BrowerWindowInfo也是NULL（很奇怪）
			
			return true;
		}
	};
	void CreateMessageHandlers(MessageHandlerSet& handlers) {
		handlers.insert(new Handler());
	}
}

CefRefPtr<JWebTopClient> g_handler;// 全局保留一个JWebTopClient即可
// 临时记录窗口配置信息，用于在JWebTopBrowser和JWebTopClient传递参数，（因为JWebTopClient是全局唯一实例）使用后置空
JWebTopConfigs  tmpConfigs;

void createNewBrowser(JWebTopConfigs configs){
	CEF_REQUIRE_UI_THREAD();
	// 用于构建本地窗口的配置信息（注：有些信息[style、exStyle]设置了不太管用，可以在Handler的OnAfterCreated中对窗口进行修饰）
	CefWindowInfo window_info;

#if defined(OS_WIN)
	// On Windows we need to specify certain flags that will be passed to
	window_info.SetAsPopup(NULL, configs.name);
#endif
	if (g_handler == NULL)g_handler = new JWebTopClient();
	CefBrowserSettings browser_settings;
	//window_info.window_name = UUID::Data1();// 用UUID作为窗口名称，建立窗口名与JWebTopConfigs的关联？？？
	tmpConfigs = configs;
	// 创建浏览器窗口
	CefBrowserHost::CreateBrowser(window_info, g_handler.get(), configs.getAbsolutePath(configs.url.ToWString()), browser_settings, NULL);
}