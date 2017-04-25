#ifndef CEF_JWEBTOP_ex_H_
#define CEF_JWEBTOP_ex_H_
#pragma once
#include "common/fastipc/server.h"
#include "common/fastipc/client.h"
#include "common/util/StrUtil.h"
#include "common/JWebTopMsg.h"
#include "JWebTop\config\JWebTopConfigs.h"
#include <string>
#include <map>
#include <sstream>

#define APP_SUCCESS				0
#define APP_ERR_HasINITED		1	// 服务已经创建，不会再重新创建
#define APP_ERR_NoJSHandler		2	// 创建服务前，必须设置jsonDispater
#define APP_ERR_NoBrowserExe	3	// 找不到可执行文件
#define APP_ERR_NoAppCfgFile	4	// 找不到配置文件
#define APP_ERR_NotInited		5	// 尚未对app进行初始化
/**
* 当JWebTop进程成功启动后调用，只有在此时间点之后，才可以构建浏览器
*/
class JWebTopAppInited {
public:
	virtual void onJWebTopAppInited() = 0;
};

/**
* 当指定浏览器被创建后调用
* @param browserHWnd 成功创建的浏览器的窗口句柄
*/
class JWebTopBrowserCreated {
public:
	virtual void onJWebTopBrowserCreated(long browserHWnd)=0;
};

/**
* js执行后的执行结果
* @param uuid       执行JS时传递的uuid，根据此主要用于一个JWebTopJSReturn中处理多个JS返回结果的情况
* @param jsonString 执行结果，绝大多数情况下是一个JSON字符串
*/
class JWebTopJSReturn {
public:
	virtual void onJWebTopJSReturn(wstring jsonString) = 0;
};
class JWebtopJSONDispater{
public:
	/**
	* 处理浏览器发来的json内容，并返回结果
	*
	* @param browserHWnd
	*            需要执行json的浏览器句柄
	* @param json
	*            浏览器端发来的要执行内容
	* @return 本地处理后的结果
	*/
	virtual wstring dispatcher(long browserHWnd, wstring json) = 0;
};

class JWebTopContext :public fastipc::RebuildedBlockListener
{
private:
	typedef map<string, JWebTopJSReturn * > DefJSReturnMap;			// 定义一个存储浏览器扩展状态的Map类
	typedef map<string, JWebTopBrowserCreated *> DefBrowserCreatedListenerMap;
	DefJSReturnMap jsReturnListners;
	DefBrowserCreatedListenerMap browserCreateListeners;
	std::wstring serverName;	// 服务端名称
	DWORD blockSize;			// 一次传输时的数据大小
	fastipc::Server* server;	// IPC服务端
	fastipc::Client* client;	// IPC客户端
	JWebtopJSONDispater* jsonDispater;	// 响应浏览器进程发来的JSON数据并进行处理
	JWebTopAppInited * appInitListener;

	DWORD _write(LONG userMsgType, LONG userValue,  wstring data){
		string _data=jw::w2s(data);
		return client->write(userMsgType, userValue, NULL, LPSTR(_data.c_str()), _data.size());
	}
	DWORD _write(LONG userMsgType, LONG userValue, string userShortStr, wstring data){
		string _data = jw::w2s(data);
		return client->write(userMsgType, userValue, LPSTR(userShortStr.c_str()), LPSTR(_data.c_str()), _data.size());
	}
	string createTaskId(){
		return jw::GenerateGuidA();
	}


	void onRebuildedRead(fastipc::MemBlock* memBlock) override;
	void __onRead(int userMsgType, int userValue, string userShortStr, wstring data);

	JWebTopJSReturn * findJSONDispater(string uuid){
		JWebTopJSReturn * rtn = NULL;	
		DefJSReturnMap::iterator it = jsReturnListners.find(uuid);
		if (jsReturnListners.end() != it) {
			rtn = it->second;
		}
		return rtn;
	}
	void removeJSONDispater(string uuid){
		jsReturnListners.erase(uuid);
	}

	// 管理浏览器创建时的侦听器
	JWebTopBrowserCreated * findBrowserCreatedListener(string uuid){
		JWebTopBrowserCreated * rtn = NULL;
		DefBrowserCreatedListenerMap::iterator it = browserCreateListeners.find(uuid);
		if (browserCreateListeners.end() != it) {
			rtn = it->second;
		}
		return rtn;
	}
	void addBrowserCreatedListener(JWebTopBrowserCreated* listener, string uuid) {
		string taskId = (uuid.length() == 0) ? createTaskId() : uuid;
		browserCreateListeners.insert(pair<string, JWebTopBrowserCreated* >(taskId, listener));
	};
	void removeBrowserCreatedListener(string uuid){
		browserCreateListeners.erase(uuid);
	}

	// 创建浏览器
	int _createBrowser(int createType, string taskId, wstring cfgData)
	{
		string s = jw::w2s(cfgData);
		return client->write(createType, 0, LPSTR(taskId.c_str()), LPSTR(s.c_str()), s.length());
	}
public:
	JWebTopContext(){};
	~JWebTopContext(){ closeContext(); };

	/**
	 * 创建JWebTop进程，成功后回调JWebTopAppInited侦听器
	 * 返回值参见：APP_*
	 * @param processPath
	 * @param cfgFile
	 * @param appInitListner
	 */
	int createJWebTopByCfgFile(wstring processPath, wstring cfgFile, JWebTopAppInited* appInitListner);

	/**
	 * 根据配置文件创建浏览器，成功后回调JWebTopBrowserCreated侦听器
	 * 返回值参见：BROWSER_*
	 * @param browserCfgFile
	 */
	int createBrowserByFile(wstring browserCfgFile, JWebTopBrowserCreated* createListener);

	/**
	* 根据配置对象来创建浏览器，成功后回调JWebTopBrowserCreated侦听器
	* @param configs  配置浏览器的对象
	*/
	int createBrowser(JWebTopConfigs* configs, JWebTopBrowserCreated* createListener);

	/**
	* 根据JSON配置字符串创建浏览器，成功后回调JWebTopBrowserCreated侦听器
	* @param browerCfgJSON  JSON配置字符串
	*/
	int createBrowserByJSON(wstring browerCfgJSON, JWebTopBrowserCreated* createListener);

	// 关闭指定浏览器
	void closeBrowser(long browserHWnd);

	// 退出JWebTop进程
	void ExitJWebTop();

	// 关闭当前Context持有的FastIPC服务端、客户端，以及浏览器进程
	void closeContext();

	/**
	* 设置一个错误页面
	*
	* @param url
	*/
	void setErrorUrl(wstring url) {
		_write(JWM_SET_ERR_URL, 0, url);
	};

	bool isStable() {
		return server != NULL && server->isStable();
	};

	void setUrl(long browserHWnd, wstring url) {
		_write(JWM_M_SETURL, browserHWnd,  url);
	};

	void setAppendJS(long browserHWnd, wstring js) {
		_write(JWM_M_APPEND_JS, browserHWnd, js);
	};

	/**
	* 执行js，发送后等待数据的返回。<br/>
	* 这里发送是包装的JSON数据，其调用JS脚本中的invokeByDLL(jsonvalue)方法
	*
	* @param browserHWnd
	*            在哪个浏览器执行
	* @param jsonstring
	*            待执行的JSON数据
	* @param jsReturn
	*            JS执行结果的侦听器
	* @param uuid
	*            执行js时用于获取返回结果，如果jsReturn是共用的，那么可以根据此uuid来区分不同的调用
	*/
	void executeJSON_Wait(long browserHWnd, wstring jsonstring, JWebTopJSReturn* jsReturn, string uuid) {
		if (jsReturn != NULL){
			string taskId = (uuid.length() == 0) ? createTaskId() : uuid;
			jsReturnListners.insert(pair<string, JWebTopJSReturn* >(taskId, jsReturn));
			_write(JWM_JSON_EXECUTE_WAIT, browserHWnd, taskId, jsonstring);
		} else{
			_write(JWM_JSON_EXECUTE_WAIT, browserHWnd, uuid, jsonstring);
		}
	};

	/**
	* 执行js，但不等待数据的返回。<br/>
	* 这里发送是包装的JSON数据，其调用JS脚本中的invokeByDLL(jsonvalue)方法
	*
	* @param jsonstring
	*/
	void executeJSON_NoWait(long browserHWnd, wstring jsonstring) {
		_write(JWM_JSON_EXECUTE_RETURN, browserHWnd,  jsonstring);
	};

	/**
	* 执行js，并等待数据的返回。<br/>
	* 推荐使用executeJSON_Wait方法，以减少字符串转换的问题
	*
	* @param browserHWnd
	*            在哪个浏览器执行
	* @param script
	*            待执行的JS脚本
	* @param jsReturn
	*            JS执行结果的侦听器
	* @param uuid
	*            执行js时用于获取返回结果，如果jsReturn是共用的，那么可以根据此uuid来区分不同的调用
	*/
	void executeJS_Wait(long browserHWnd, wstring script, JWebTopJSReturn* jsReturn, string uuid) {
		if (jsReturn != NULL){
			string taskId = (uuid.length() == 0) ? createTaskId() : uuid;
			jsReturnListners.insert(pair<string, JWebTopJSReturn* >(taskId, jsReturn));
			_write(JWM_JS_EXECUTE_WAIT, browserHWnd, taskId, script);
		} else{
			_write(JWM_JS_EXECUTE_WAIT, browserHWnd, uuid, script);
		}
	};

	/**
	* 执行js，但不等待数据的返回。 <br/>
	* 推荐使用executeJSON_NoWait方法，以减少字符串转换的问题
	*
	* @param script
	* @return
	*/
	void executeJS_NoWait(long browserHWnd, wstring script) {
		_write(JWM_JS_EXECUTE_RETURN, browserHWnd, script);
	};

	wstring getServerName(){ return this->serverName; };
	void setServerName(wstring serverName) { this->serverName = serverName; };
	int getBlockSize() { return blockSize; };
	void setBlockSize(int blockSize) { this->blockSize = blockSize; };
	JWebtopJSONDispater* getjsonDispater(){ return this->jsonDispater; };
	void setjsonDispater(JWebtopJSONDispater* jsonDispater) { this->jsonDispater = jsonDispater; };
};
#endif