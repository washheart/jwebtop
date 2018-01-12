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
#define APP_ERR_HasINITED		1	// �����Ѿ����������������´���
#define APP_ERR_NoJSHandler		2	// ��������ǰ����������jsonDispater
#define APP_ERR_NoBrowserExe	3	// �Ҳ�����ִ���ļ�
#define APP_ERR_NoAppCfgFile	4	// �Ҳ��������ļ�
#define APP_ERR_NotInited		5	// ��δ��app���г�ʼ��
/**
* ��JWebTop���̳ɹ���������ã�ֻ���ڴ�ʱ���֮�󣬲ſ��Թ��������
*/
class JWebTopAppInited {
public:
	virtual void onJWebTopAppInited() = 0;
};

/**
* ��ָ������������������
* @param browserHWnd �ɹ�������������Ĵ��ھ��
*/
class JWebTopBrowserCreated {
public:
	virtual void onJWebTopBrowserCreated(long browserHWnd)=0;
};

/**
* jsִ�к��ִ�н��
* @param uuid       ִ��JSʱ���ݵ�uuid�����ݴ���Ҫ����һ��JWebTopJSReturn�д�����JS���ؽ�������
* @param jsonString ִ�н������������������һ��JSON�ַ���
*/
class JWebTopJSReturn {
public:
	virtual void onJWebTopJSReturn(wstring jsonString) = 0;
};
class JWebtopJSONDispater{
public:
	/**
	* ���������������json���ݣ������ؽ��
	*
	* @param browserHWnd
	*            ��Ҫִ��json����������
	* @param json
	*            ������˷�����Ҫִ������
	* @return ���ش����Ľ��
	*/
	virtual wstring dispatcher(long browserHWnd, wstring json) = 0;
};

class JWebTopContext :public fastipc::RebuildedBlockListener
{
private:
	typedef map<string, JWebTopJSReturn * > DefJSReturnMap;			// ����һ���洢�������չ״̬��Map��
	typedef map<string, JWebTopBrowserCreated *> DefBrowserCreatedListenerMap;
	DefJSReturnMap jsReturnListners;
	DefBrowserCreatedListenerMap browserCreateListeners;
	std::wstring serverName;	// ���������
	DWORD blockSize;			// һ�δ���ʱ�����ݴ�С
	fastipc::Server* server;	// IPC�����
	fastipc::Client* client;	// IPC�ͻ���
	JWebtopJSONDispater* jsonDispater;	// ��Ӧ��������̷�����JSON���ݲ����д���
	JWebTopAppInited * appInitListener;

	DWORD _write(LONG userMsgType, LONG userValue,  wstring data){
		string _data = jw::str::w2s(data);
		return client->write(userMsgType, userValue, NULL, LPSTR(_data.c_str()), _data.size());
	}
	DWORD _write(LONG userMsgType, LONG userValue, string userShortStr, wstring data){
		string _data = jw::str::w2s(data);
		return client->write(userMsgType, userValue, LPSTR(userShortStr.c_str()), LPSTR(_data.c_str()), _data.size());
	}
	string createTaskId(){
		return jw::str::GenerateGuidA();
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

	// �������������ʱ��������
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

	// ���������
	int _createBrowser(int createType, string taskId, wstring cfgData)
	{
		string s = jw::str::w2s(cfgData);
		return client->write(createType, 0, LPSTR(taskId.c_str()), LPSTR(s.c_str()), s.length());
	}
public:
	JWebTopContext(){};
	~JWebTopContext(){ closeContext(); };

	/**
	 * ����JWebTop���̣��ɹ���ص�JWebTopAppInited������
	 * ����ֵ�μ���APP_*
	 * @param processPath
	 * @param cfgFile
	 * @param appInitListner
	 */
	int createJWebTopByCfgFile(wstring processPath, wstring cfgFile, JWebTopAppInited* appInitListner);

	/**
	 * ���������ļ�������������ɹ���ص�JWebTopBrowserCreated������
	 * ����ֵ�μ���BROWSER_*
	 * @param browserCfgFile
	 */
	int createBrowserByFile(wstring browserCfgFile, JWebTopBrowserCreated* createListener);

	/**
	* �������ö�����������������ɹ���ص�JWebTopBrowserCreated������
	* @param configs  ����������Ķ���
	*/
	int createBrowser(JWebTopConfigs* configs, JWebTopBrowserCreated* createListener);

	/**
	* ����JSON�����ַ���������������ɹ���ص�JWebTopBrowserCreated������
	* @param browerCfgJSON  JSON�����ַ���
	*/
	int createBrowserByJSON(wstring browerCfgJSON, JWebTopBrowserCreated* createListener);

	// �ر�ָ�������
	void closeBrowser(long browserHWnd);

	// �˳�JWebTop����
	void ExitJWebTop();

	// �رյ�ǰContext���е�FastIPC����ˡ��ͻ��ˣ��Լ����������
	void closeContext();

	/**
	* ����һ������ҳ��
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
	* ִ��js�����ͺ�ȴ����ݵķ��ء�<br/>
	* ���﷢���ǰ�װ��JSON���ݣ������JS�ű��е�invokeByDLL(jsonvalue)����
	*
	* @param browserHWnd
	*            ���ĸ������ִ��
	* @param jsonstring
	*            ��ִ�е�JSON����
	* @param jsReturn
	*            JSִ�н����������
	* @param uuid
	*            ִ��jsʱ���ڻ�ȡ���ؽ�������jsReturn�ǹ��õģ���ô���Ը��ݴ�uuid�����ֲ�ͬ�ĵ���
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
	* ִ��js�������ȴ����ݵķ��ء�<br/>
	* ���﷢���ǰ�װ��JSON���ݣ������JS�ű��е�invokeByDLL(jsonvalue)����
	*
	* @param jsonstring
	*/
	void executeJSON_NoWait(long browserHWnd, wstring jsonstring) {
		_write(JWM_JSON_EXECUTE_RETURN, browserHWnd,  jsonstring);
	};

	/**
	* ִ��js�����ȴ����ݵķ��ء�<br/>
	* �Ƽ�ʹ��executeJSON_Wait�������Լ����ַ���ת��������
	*
	* @param browserHWnd
	*            ���ĸ������ִ��
	* @param script
	*            ��ִ�е�JS�ű�
	* @param jsReturn
	*            JSִ�н����������
	* @param uuid
	*            ִ��jsʱ���ڻ�ȡ���ؽ�������jsReturn�ǹ��õģ���ô���Ը��ݴ�uuid�����ֲ�ͬ�ĵ���
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
	* ִ��js�������ȴ����ݵķ��ء� <br/>
	* �Ƽ�ʹ��executeJSON_NoWait�������Լ����ַ���ת��������
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