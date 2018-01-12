#include "jwebtop_4csharp.h"
#include "common/util/StrUtil.h"
#include "common/fastipc/Server.h"
#include "common/fastipc/Client.h"
#include "common/os/OS.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
using namespace std;

// ����һ���½��̣����ص�����Ϊ���������̵߳�id
EXPORT long WINAPI nCreateSubProcess(LPTSTR subProcess, LPTSTR szCmdLine, int waitFor){
	DWORD processId = jw::os::process::createSubProcess(subProcess, szCmdLine);
	if (waitFor == 1){
		HANDLE hHandle = OpenThread(SYNCHRONIZE, false, processId);// ����Ȩ�ޣ�������Щ�����OpendProcessʧ�ܣ�����xp��
		WaitForSingleObject(hHandle, INFINITE);// �ȴ�Զ�̽��̽���
		return 0;
	} else{
		return processId;
	}
}

// ��wstringתΪchar*���ظ�csharp��
char* wstring2char(wstring ws){
	string s = jw::str::w2s(ws);
	char* c;
	const int len = s.length();
	c = new char[len + 1];
	strcpy(c, s.c_str());
	return c;
}

class CSharpIPCReadListener :public fastipc::ReadListener{// ���ﲻ����RebuildedBlockListener�����ݵ���װ����csharp������ȥ��
private:
	CSharpCallBack g_CSharpCallBack;  //�ظ���Ϣ�Ļص�����
public:
	void setListener(CSharpCallBack listener){
		g_CSharpCallBack = listener;
	}
	// ���з���˶�ȡ�����ݺ󣬻���ô˷���֪ͨ�ȴ��߽��д���
	// memBlock�ڷַ�����ɷ��������٣��ⲿ�����������������
	void onRead(fastipc::MemBlock* memBlock) override{
		if (!g_CSharpCallBack)return;
		// FIXME:packId��userShortValue�Ƿ�Ҳ���Բ������½���ֱ�Ӵ���ȥ�ã�
		char * packId = NULL;
		if (memBlock->msgType != MSG_TYPE_NORMAL){
			packId = (char *)malloc(PACK_ID_LEN + 1);
			memcpy(packId, memBlock->packId, PACK_ID_LEN);
			packId[PACK_ID_LEN] = '\0';
		}
		LPSTR userShortValue = NULL;
		userShortValue = (char *)malloc(PACK_ID_LEN + 1);
		memcpy(userShortValue, memBlock->userShortStr, PACK_ID_LEN);
		userShortValue[PACK_ID_LEN] = '\0';
		// FIXME:data�Ƿ�Ҳ���Բ������½�����len��Ϊ��������
		int len = memBlock->dataLen;
		char * data = NULL;
		data = (char *)malloc(len);
		memcpy(data, memBlock->data, len);

		g_CSharpCallBack(memBlock->msgType, packId, 
			memBlock->userMsgType, memBlock->userValue,
			userShortValue, data, len);
		delete data;
		delete userShortValue;
		if (packId)delete packId;
	}
};


/*
* �÷������ڴ���һ��fastipc�����
* serverName	���������
* blockSize		�������ͻ���ͨ��ʱÿ�ε�������
* result		�������������Ƿ񴴽��ɹ���0=�ɹ�
* return		������Server��ָ��
*/
EXPORT long WINAPI nCreateServer(LPTSTR serverName, int blockSize, int& result){
	fastipc::Server * server = new fastipc::Server();
	result = server->create(serverName, blockSize);
	return (int)server;
}

/*
* �÷������ڿ���fastipc����˵Ķ�ȡ�������ɹ���������ڴ˴�
* nativeServer	ָ������ʵ����ָ��
* listener		��Ӧ��ȡ������ָ��
*/
EXPORT void WINAPI nStartRead(int nativeServer, CSharpCallBack callBack){
	fastipc::Server * server = (fastipc::Server *) nativeServer;
	CSharpIPCReadListener * ipcListener = new CSharpIPCReadListener();
	ipcListener->setListener(callBack);
	server->setListener(ipcListener);
	server->startRead();
}
/*
* �÷������ڹر�fastipc����ˣ�ִ�к�nStartRead��������ֹͣ
* nativeServer	ָ������ʵ����ָ��
*/
EXPORT void WINAPI nCloseServer(int nativeServer){
	fastipc::Server * server = (fastipc::Server *) nativeServer;
	server->close();
}

/*
* �÷������ڴ���һ��fastipc�ͻ���
* serverName	�ͻ��˶�Ӧ�ķ���˵�����
* blockSize		�������ͻ���ͨ��ʱÿ�ε�������
* result		�������������Ƿ񴴽��ɹ���0=�ɹ�
* return		������Client��ָ��
*/
EXPORT int WINAPI nCreateClient(LPTSTR serverName, int blockSize, int& result){
	fastipc::Client * client = new fastipc::Client();
	result = client->create(serverName, blockSize);
	return (int)client;
}

/*
* �÷���������fastipc�ͻ���д�����ݣ�����д��ɺ󣬻ᴥ������˵�����
* nativeClient	ָ��ͻ���ʵ����ָ��
* data			Ҫд�������
* return		�ɹ�д������ݳ���
*/
EXPORT int WINAPI nWriteClient(int nativeClient, int userMsgType, int userValue, LPTSTR userShortStr, LPTSTR data){
	fastipc::Client * client = (fastipc::Client *) nativeClient;
	char * shortStr = NULL;
	char * str = NULL;
	int len = 0;
	// data
	if (data != NULL){
		char*  tmp = wstring2char(data);
		len = lstrlenA(tmp);
		str = new char[len + 1];
		memcpy(str, tmp, len);
		delete tmp;
		str[len] = '\0';
	}
	// userShortStr
	if (userShortStr != NULL){
		char * tmp2 = wstring2char(userShortStr);
		int len2 = lstrlenA(tmp2);
		shortStr = new char[len2 + 1];
		memcpy(shortStr, tmp2, len2);
		delete tmp2;
		shortStr[len2] = '\0';
	}
	// д���ͻ��ˣ�������ǰ������ڴ�
	client->write(userMsgType, userValue, shortStr, str, len);
	delete[] str;
	if (shortStr)delete[] shortStr;
	return 0;
}

/*
* �÷������ڹر�fastipc�ͻ���
* nativeClient	ָ��ͻ���ʵ����ָ��
*/
EXPORT void WINAPI nCloseClient(int nativeClient){
	fastipc::Client * client = (fastipc::Client *) nativeClient;
	client->close();
}