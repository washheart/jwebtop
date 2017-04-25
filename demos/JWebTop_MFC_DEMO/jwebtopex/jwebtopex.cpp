#include "jwebtopex.h"
#include <thread>
#include "include/cef_parser.h"
#include "common/os/OS.h"
#include "common/tests/TestUtil.h"
/**
* 创建JWebTop进程，成功后回调JWebTopAppInited侦听器
*
* @param processPath
* @param cfgFile
* @param appInitListner
*/
int JWebTopContext::createJWebTopByCfgFile(wstring processPath, wstring cfgFile, JWebTopAppInited* appInitListener){
	if (this->server != NULL) return APP_ERR_HasINITED;
	this->appInitListener = appInitListener;
	if (jsonDispater == NULL) return APP_ERR_NoJSHandler;
	LPTSTR exePath = LPTSTR(processPath.c_str());
	if (_waccess(exePath, 0) != 0) return APP_ERR_NoBrowserExe;
	if (_waccess(LPTSTR(cfgFile.c_str()), 0) != 0) return APP_ERR_NoAppCfgFile;
	this->server = new fastipc::Server();
	this->serverName = fastipc::genServerName();
	this->blockSize = 2048;
	this->server->create(serverName, blockSize);
	server->setListener(this);
	//thread t(std::mem_fn(&JWebTopContext::__startIPCServer), this);//server.startRead();
	thread t(std::mem_fn(&fastipc::Server::startRead), this->server);
	t.detach(); //启动线程  

	std::wstringstream cmds;
	 cmds << L" :"// “:”作为特殊符号告诉JWebTop主程序，还有其他参数要解析。因为“:”不可能出现在文件路径的开通
		 << L" " << GetCurrentProcessId()// 将当前进程id传递过去，以便当前进程结束后，JWebTop主程序自己退出
		 << L" " << blockSize// 通过FastIPC进行通信时，缓存区的大小
		 << L" " << serverName // 通过FastIPC进行通信时，双方的交互标记
		 << L" \"" << cfgFile << L"\"" // 配置文件的路径
		// << L" " + JWebTopContext.WIN_HWND//
		;
	jw::os::process::createSubProcess(exePath,LPTSTR( cmds.str().c_str()));
	return APP_SUCCESS;
}

void JWebTopContext::onRebuildedRead(fastipc::MemBlock* memBlock){
	thread t(std::mem_fn(&JWebTopContext::__onRead), this//
		, memBlock->userMsgType, memBlock->userValue, memBlock->getUserShortStr(), jw::s2w(memBlock->getData()));
	t.detach(); //启动线程  
}
void JWebTopContext::__onRead(int userMsgType, int userValue, string userShortStr, wstring data){
	switch (userMsgType) {
	case JWM_DLL_EXECUTE_RETURN:
		this->jsonDispater->dispatcher(userValue, data);
		break;
	case JWM_DLL_EXECUTE_WAIT:{
		wstring result = this->jsonDispater->dispatcher(userValue, data); // 取回执行结果
		string s = jw::w2s(result);
		this->client->write(JWM_RESULT_RETURN, userValue, LPSTR(userShortStr.c_str()), LPSTR(s.c_str()), s.length()); // 发送结果到远程进程
		}
		break;
	case JWM_RESULT_RETURN:
		 {
			JWebTopJSReturn* tmp = this->findJSONDispater(userShortStr);
			if (tmp != NULL) {
				this->removeJSONDispater(userShortStr);
				tmp->onJWebTopJSReturn(data);
			}
		}
		break;
	case JWM_BROWSER_CREATED: 
		{
			JWebTopBrowserCreated *tmp = this->findBrowserCreatedListener(userShortStr);
			if (tmp != NULL) {
				this->removeBrowserCreatedListener(userShortStr);
				tmp->onJWebTopBrowserCreated(userValue);
			}
		}
		break;
	case JWM_IPC_CLIENT_OK:
		this->client = new fastipc::Client();// 启动JWebTop程序
		this->client->create(fastipc::genServerName(this->serverName), this->blockSize);
		break;
	case JWM_CEF_APP_INITED:
		this->appInitListener->onJWebTopAppInited();
		this->appInitListener = NULL;
		break;
	//default:
		//throw new JWebTopException("无法处理的消息（" + userMsgType + "）");
	}
}

int JWebTopContext::createBrowserByFile(wstring browerCfgFile, JWebTopBrowserCreated* browserCreateListener) {
	if (client == NULL)return APP_ERR_HasINITED;
	string taskId = createTaskId();
	addBrowserCreatedListener(browserCreateListener,taskId);
	return _createBrowser(JWM_CREATEBROWSER_FILE, taskId, browerCfgFile);
}

wstring wrapAsTaskJSON(JWebTopConfigs* configs){
	CefRefPtr<CefDictionaryValue> value = CefDictionaryValue::Create();
	JWebTopConfigs tmpcfgs;
	// [BASE]小节：JWebTop相关配置，配置在[BASE]小节下
	/**
	* 指定一个配置文件，所有配置信息从配置文件加载<br/>
	* 注意：配置类中的配置项的优先级高于配置文件中的
	*/
	//if (configs->appDefFile != tmp.appDefFile)
	value->SetString(CefString("appDefFile"), CefString(configs->appDefFile));
	if (configs->url != tmpcfgs.url)value->SetString(CefString("url"), CefString(configs->url));
	value->SetString(CefString("appendJs"), CefString(configs->appendJs));
	value->SetString(CefString("name"), CefString(configs->name));
	value->SetString(CefString("icon"), CefString(configs->icon));
	// value->SetString(CefString("errorUrl"), CefString(configs->errorUrl));
	stringstream tmp("");
	if (configs->parentWin != tmpcfgs.parentWin){
		tmp << configs->parentWin;
		value->SetString(CefString("parentWinS"), CefString(tmp.str()));
	}
	if (configs->dwStyle != tmpcfgs.dwStyle){
		tmp.str(""); tmp << configs->dwStyle;
		value->SetString(CefString("dwStyleS"), CefString(tmp.str()));
	}
	if (configs->dwExStyle != tmpcfgs.dwExStyle){
		tmp.str(""); tmp << configs->dwExStyle;
		value->SetString(CefString("dwExStyleS"), CefString(tmp.str()));
	}
	if (configs->x != tmpcfgs.x)value->SetInt(CefString("x"), configs->x);
	if (configs->y != tmpcfgs.y)value->SetInt(CefString("y"), configs->y);
	if (configs->w != tmpcfgs.w)value->SetInt(CefString("w"), configs->w);
	if (configs->h != tmpcfgs.h)value->SetInt(CefString("h"), configs->h);
	if (configs->max != -1)value->SetInt(CefString("max"), configs->max);

	if (configs->enableDebug != tmpcfgs.enableDebug)value->SetInt(CefString("enableDebug"), configs->enableDebug);
	if (configs->enableResize != tmpcfgs.enableResize)value->SetInt(CefString("enableResize"), configs->enableResize);
	if (configs->disableRefresh != tmpcfgs.disableRefresh)value->SetInt(CefString("disableRefresh"), configs->disableRefresh);
	if (configs->enableDrag != tmpcfgs.enableDrag)value->SetInt(CefString("enableDrag"), configs->enableDrag);

	// [CEF]小节：cef配置参数（cef相关参数只在应用启动时起作用），配置在[CEF]小节下
	if (configs->single_process != 1)value->SetInt(CefString("single_process"), configs->single_process);

	value->SetString(CefString("user_data_path"), CefString(configs->user_data_path));
	value->SetString(CefString("cache_path"), CefString(configs->cache_path));
	if (configs->persist_session_cookies != -1)value->SetInt(CefString("persist_session_cookies"), configs->persist_session_cookies);
	value->SetString(CefString("locale"), CefString(configs->locale));
	value->SetString(CefString("user_agent"), CefString(configs->user_agent));

	if (configs->log_severity != -1)value->SetInt(CefString("log_severity"), configs->log_severity);
	value->SetString(CefString("log_file"), CefString(configs->log_file));
	value->SetString(CefString("resources_dir_path"), CefString(configs->resources_dir_path));
	value->SetString(CefString("locales_dir_path"), CefString(configs->locales_dir_path));
	if (configs->ignore_certificate_errors != -1)value->SetInt(CefString("ignore_certificate_errors"), configs->ignore_certificate_errors);
	if (configs->remote_debugging_port != -1)value->SetInt(CefString("remote_debugging_port"), configs->remote_debugging_port);
	value->SetString(CefString("proxyServer"), CefString(configs->proxyServer));
	value->SetString(CefString("proxyAuthUser"), CefString(configs->proxyAuthUser));
	value->SetString(CefString("proxyAuthPwd"), CefString(configs->proxyAuthPwd));

	CefRefPtr<CefValue> node = CefValue::Create();
	node->SetDictionary(value);
	CefString result = CefWriteJSON(node, JSON_WRITER_DEFAULT);
	return result.ToWString();
}
int JWebTopContext::createBrowser(JWebTopConfigs* configs, JWebTopBrowserCreated* browserCreateListener) {
	if (client == NULL) return APP_ERR_HasINITED;
	return createBrowserByJSON(wrapAsTaskJSON(configs), browserCreateListener);
}

int JWebTopContext::createBrowserByJSON(wstring browerCfgJSON, JWebTopBrowserCreated* browserCreateListener) {
	if (client == NULL) return APP_ERR_HasINITED;
	string taskId = createTaskId();
	addBrowserCreatedListener(browserCreateListener, taskId);
	return _createBrowser(JWM_CREATEBROWSER_JSON, taskId, browerCfgJSON);
}