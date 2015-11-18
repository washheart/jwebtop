// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "JWebTopClient.h"

#include <sstream>
#include <string>
#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "common/util/StrUtil.h"
#include "JWebTop/jshandler/JJH_Windows.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
#include "JWebTopContext.h"
#include "JWebTopCommons.h"
#include "JWebTop/dllex/DLLExState.h"

using namespace std;
namespace jw{
	extern JWebTopConfigs * g_configs;  // 应用启动时的第一个配置变量
}
JWebTopClient::JWebTopClient()
: isClosed(false), dialog_handler_(new JSDialogHandler()){
}

JWebTopClient::~JWebTopClient() {}

extern CefSettings settings;              // CEF全局设置

void JWebTopClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	jw::ctx::addBrowser(browser);// 记录下已经创建的窗口来
	renderBrowserWindow(browser, this->configs);
	if (jw::dllex::ex()) {
		jw::dllex::sendBrowserCreatedMessage(this->taskId, (LONG)browser->GetHost()->GetWindowHandle());
	}
	if (!message_router_) {
		CefMessageRouterConfig config;
		message_router_ = CefMessageRouterBrowserSide::Create(config);
		// Register handlers with the router.
		jc::CreateBrowserSideMessageHandlers(message_handler_set_);
		MessageHandlerSet::const_iterator it = message_handler_set_.begin();
		for (; it != message_handler_set_.end(); ++it)
			message_router_->AddHandler(*(it), false);
	}
}

bool JWebTopClient::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request,
	bool is_redirect) {
#ifdef JWebTopLog
	writeLog(L"■■■■■■■■■■■■CefRequestHandler---OnBeforeBrowse---\r\n");
#endif 
	CEF_REQUIRE_UI_THREAD();
	message_router_->OnBeforeBrowse(browser, frame);
	return false;
}

void JWebTopClient::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
	TerminationStatus status) {
#ifdef JWebTopLog
	writeLog(L"■■■■■■■■■■■■CefRequestHandler---OnRenderProcessTerminated---\r\n");
#endif 
	CEF_REQUIRE_UI_THREAD();
	message_router_->OnRenderProcessTerminated(browser);
}

bool JWebTopClient::GetAuthCredentials(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	bool isProxy,
	const CefString& host,
	int port,
	const CefString& realm,
	const CefString& scheme,
	CefRefPtr<CefAuthCallback> callback) {
#ifdef JWebTopLog
	writeLog(L"■■■■■■■■■■■■CefRequestHandler---GetAuthCredentials---\r\n");
#endif 
	CEF_REQUIRE_IO_THREAD();
	if (isProxy) {// 对于代理，如果需要认证，提供用户名和密码
		//if(frame->GetURL...)// 注意这里没有区分哪个代理，哪个url
		callback->Continue(jw::g_configs->proxyAuthUser, jw::g_configs->proxyAuthPwd);
		return true;
	}
	return false;
}
void JWebTopClient::OnRenderViewReady(CefRefPtr<CefBrowser> browser){
	jw::dllex::sendIPCServerInfo(browser->GetHost()->GetWindowHandle());// 发送消息构建render进程的ipc客户端
}
bool JWebTopClient::OnProcessMessageReceived(
	CefRefPtr<CefBrowser> browser,
	CefProcessId source_process,
	CefRefPtr<CefProcessMessage> message) {
	CEF_REQUIRE_UI_THREAD();
	if (message_router_->OnProcessMessageReceived(browser, source_process,
		message)) {
		return true;
	}
	//// Check for messages from the client renderer.
	//std::string message_name = message->GetName();
	//if (message_name == kFocusedNodeChangedMessage) {
	//	// A message is sent from ClientRenderDelegate to tell us whether the
	//	// currently focused DOM node is editable. Use of |focus_on_editable_field_|
	//	// is redundant with CefKeyEvent.focus_on_editable_field in OnPreKeyEvent
	//	// but is useful for demonstration purposes.
	//	focus_on_editable_field_ = message->GetArgumentList()->GetBool(0);
	//	return true;
	//}
	return false;
}
bool JWebTopClient::DoClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();
	HWND hWnd = browser->GetHost()->GetWindowHandle();
	if (hWnd != NULL || getBrowserWindowInfo(hWnd) == NULL){
		OnBeforeClose(browser);
	}
	// Allow the close. For windowed browsers this will result in the OS close
	// event being sent.
	return false;
}

void JWebTopClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
	if (isClosed)return;
	CEF_REQUIRE_UI_THREAD();
	jw::ctx::removeBrowser(browser);
	if (jw::dllex::ex()) {
		jw::dllex::DLLExState::removeBrowserSetting(browser->GetHost()->GetWindowHandle());
		jw::dllex::invokeRemote_NoWait(browser->GetHost()->GetWindowHandle(), "{\"action\":\"window\",\"method\":\"browserClosed\"}");
	}
	isClosed = true;
}
void JWebTopClient::OnLoadError(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	ErrorCode errorCode,
	const CefString& errorText,
	const CefString& failedUrl) {
	CEF_REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED) return;
	CefString errorUrl(JWebTopConfigs::getErrorURL());
	if (!errorUrl.empty() && 0 != errorUrl.compare(failedUrl)){
		frame->LoadURL(errorUrl);
	}
	else{
		frame->LoadString(L"<html><body><h2>出错了（地址：" + failedUrl.ToWString() + L"）！</h2></body></html>", failedUrl);
	}
}

// 判断附加的js是否需要以引用方式添加
bool isReference(const wstring &appendFile){
	wstring start = appendFile.substr(0, 10);
	int len = start.length();
	if (len < 7/*7=="http://".length()*/ || start.find(L":") == -1)return 0;
	transform(start.begin(), start.end(), start.begin(), ::tolower);
	return start.find(L"http://") != -1 || start.find(L"https://") != -1 || start.find(L"jwebtop://") != -1;
}

void JWebTopClient::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode){
#ifdef JWebTopLog
	if (frame->IsMain()){
		writeLog(L"MAIN  ====");
	}
	else{
		writeLog(L"FRAME ====");
	} writeLog(L"OnLoadEnd URL===="); writeLog(frame->GetURL().ToWString()); writeLog(L"\r\n");
#endif	
	if (frame->IsMain()){// 只在主窗口上附加JWebTop JS对象
		CefRefPtr<CefBrowserHost> host = browser->GetHost();
		HWND hWnd = host->GetWindowHandle();
		// 添加JWebTop对象的handler属性和close方法（放到OnAfterCreated中，页面重新加载后函数和变量会丢失）
		stringstream extensionCode;
		extensionCode << "if(!JWebTop)JWebTop={};";
		extensionCode << "(function() {";// 将OnLoad后，JWebTop动态添加的JS脚本包起来，避免和其他用户JS文件的冲突
		extensionCode << "JWebTop.handler=" << (LONG)hWnd << ";" << endl;
		extensionCode << "JWebTop.cefQuery = function(ajson){ window.cefQuery({ request:JSON.stringify(ajson) }) }; " << endl;// 包装下window.cefQuery参数
		if (!settings.single_process){	// 多进程模式下，需要按发送消息的方式注册需要根据HWND获取Borwser的函数到JWebTop对象
			// close(handler);// 关闭窗口
			extensionCode << "JWebTop.close=function(handler){JWebTop.cefQuery({m:'close',handler:(handler?handler:JWebTop.handler)})};" << endl;
			//loadUrl(url, handler);//加载网页，url为网页路径
			extensionCode << "JWebTop.loadUrl=function(url,handler){JWebTop.cefQuery({m:'loadUrl',url:url,handler:(handler?handler:JWebTop.handler)})};" << endl;
			//reload(handler);//重新加载当前页面
			extensionCode << "JWebTop.reload=function(handler){JWebTop.cefQuery({m:'reload',handler:(handler?handler:JWebTop.handler)})};" << endl;
			//reloadIgnoreCache(handler);//重新加载当前页面并忽略缓存
			extensionCode << "JWebTop.reloadIgnoreCache=function(handler){JWebTop.cefQuery({m:'reloadIgnoreCache',handler:(handler?handler:JWebTop.handler)})};" << endl;
			//showDev(handler);//打开开发者工具
			extensionCode << "JWebTop.showDev=function(handler){JWebTop.cefQuery({m:'showDev',handler:(handler?handler:JWebTop.handler)})};" << endl;

			// enableDrag(true|false);// 允许进行拖动
			extensionCode << "JWebTop.enableDrag=function(enable){JWebTop.cefQuery({m:'enableDrag',enable:enable,handler:JWebTop.handler})};" << endl;
			// startDrag();// 开始进行拖动
			extensionCode << "JWebTop.startDrag=function(){JWebTop.cefQuery({m:'startDrag',handler:JWebTop.handler})};" << endl;
			// stopDrag();// 停止拖动
			extensionCode << "JWebTop.stopDrag=function(){JWebTop.cefQuery({m:'stopDrag',handler:JWebTop.handler})};" << endl;
		}
		// runApp(appName,handler);//运行一个app，appName为.app文件路径。
		extensionCode << "JWebTop.runApp=function(app,parentWin,handler){JWebTop.cefQuery({m:'runApp',app:app,parentWin:(parentWin?parentWin:0),handler:(handler?handler:JWebTop.handler)})};" << endl;
		extensionCode << "JWebTop.runAppMore=function(app,parentWin,url,name,icon,x,y,w,h,handler){" \
			"var morejson={m:'runAppMore',app:app,parentWin:(parentWin?parentWin:0),handler:(handler?handler:JWebTop.handler)};" \
			"if(url!=null)morejson.url=url;" \
			"if(name!=null)morejson.name=name;" \
			"if(icon!=null)morejson.icon=icon;" \
			"if(x!=null)morejson.x=x;" \
			"if(y!=null)morejson.y=y;" \
			"if(w!=null)morejson.w=w;" \
			"if(h!=null)morejson.h=h;" \
			"JWebTop.cefQuery(morejson)};";
		if (!configs->appendJs.empty()){// 需要附加一个js文件
			wstring appendFile = configs->appendJs.ToWString();
			//下面这种方式会有跨域问题，所以采用读入文件的方式
			if (isReference(ref(appendFile))){// 通过docuemnt.wirte的方式来写
				appendFile = jw::replace_allW(ref(appendFile), L"\\", L"/");// 替换文件中的换行符号	
				extensionCode
					<< "\r\n var scriptLet = document.createElement('SCRIPT');"
					<< "\r\n scriptLet.type = 'text/javascript';"
					<< "\r\n scriptLet.src = '" << jw::w2s(appendFile) << "';"
					<< "\r\n addEventListener(\"JWebTopReady\",function(){document.body.appendChild(scriptLet);});"
					<< "\r\n";
			}
			else{
				appendFile = configs->getAbsolutePath(appendFile);
				string appendJS;
				if (jw::readfile(appendFile, ref(appendJS))){
					extensionCode << "\r\n" << appendJS << "\r\n";
				}
			}
		}
		extensionCode << "})()\r\n";// 结束对脚本的包围
#ifdef JWebTopLog
		writeLog(extensionCode.str());
#endif
		frame->ExecuteJavaScript(CefString(extensionCode.str()), "", 0);// 附加JWebTop对象
		jw::dllex::DLLExState::appendBrowserJS(hWnd, frame);			// 每次主页面重新加载之后，重新执行需要附加的JS
		jw::js::events::sendReadey(frame);								// 发送页面已准备好事件
		if (configs->enableResize)jb::checkAndSetResizeAblity(hWnd);
	}
	else{
		jw::js::events::sendIFrameReady(browser->GetMainFrame());// 在【主    】页面发送iframe页面已准备好事件
		jw::js::events::sendIFrameReady(frame);					 // 在【iframe】页面送iframe页面已准备好事件
	}
}