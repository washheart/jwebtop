#include "jwebtop_4csharp.h"
#include "common/util/StrUtil.h"
#include "common/fastipc/Server.h"
#include "common/fastipc/Client.h"
#include "common/os/OS.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
using namespace std;

// 创建一个新进程，返回的数据为进程中主线程的id
EXPORT long WINAPI nCreateSubProcess(LPTSTR subProcess, LPTSTR szCmdLine, int waitFor){
	DWORD processId = jw::os::process::createSubProcess(subProcess, szCmdLine);
	if (waitFor == 1){
		HANDLE hHandle = OpenThread(SYNCHRONIZE, false, processId);// 降低权限，否则有些情况下OpendProcess失败（比如xp）
		WaitForSingleObject(hHandle, INFINITE);// 等待远程进程结束
		return 0;
	} else{
		return processId;
	}
}

// 把wstring转为char*返回给csharp端
char* wstring2char(wstring ws){
	string s = jw::w2s(ws);
	char* c;
	const int len = s.length();
	c = new char[len + 1];
	strcpy(c, s.c_str());
	return c;
}

class CSharpIPCReadListener :public fastipc::ReadListener{// 这里不采用RebuildedBlockListener，数据的组装留给csharp侦听器去做
private:
	CSharpCallBack g_CSharpCallBack;  //回复信息的回调函数
public:
	void setListener(CSharpCallBack listener){
		g_CSharpCallBack = listener;
	}
	// 当有服务端读取到数据后，会调用此方法通知等待者进行处理
	// memBlock在分发后会由服务器销毁，外部调用者无需清理操作
	void onRead(fastipc::MemBlock* memBlock) override{
		if (!g_CSharpCallBack)return;
		// FIXME:packId、userShortValue是否也可以不进行新建，直接传出去用？
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
		// FIXME:data是否也可以不进行新建，把len作为传出参数
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
* 该方法用于创建一个fastipc服务端
* serverName	服务端名称
* blockSize		服务端与客户端通信时每次的数据量
* result		（传出参数）是否创建成功，0=成功
* return		创建的Server的指针
*/
EXPORT long WINAPI nCreateServer(LPTSTR serverName, int blockSize, int& result){
	fastipc::Server * server = new fastipc::Server();
	result = server->create(serverName, blockSize);
	return (int)server;
}

/*
* 该方法用于开启fastipc服务端的读取侦听，成功后会阻塞在此处
* nativeServer	指向服务端实例的指针
* listener		响应读取操作的指针
*/
EXPORT void WINAPI nStartRead(int nativeServer, CSharpCallBack callBack){
	fastipc::Server * server = (fastipc::Server *) nativeServer;
	CSharpIPCReadListener * ipcListener = new CSharpIPCReadListener();
	ipcListener->setListener(callBack);
	server->setListener(ipcListener);
	server->startRead();
}
/*
* 该方法用于关闭fastipc服务端，执行后nStartRead的阻塞会停止
* nativeServer	指向服务端实例的指针
*/
EXPORT void WINAPI nCloseServer(int nativeServer){
	fastipc::Server * server = (fastipc::Server *) nativeServer;
	server->close();
}

/*
* 该方法用于创建一个fastipc客户端
* serverName	客户端对应的服务端的名称
* blockSize		服务端与客户端通信时每次的数据量
* result		（传出参数）是否创建成功，0=成功
* return		创建的Client的指针
*/
EXPORT int WINAPI nCreateClient(LPTSTR serverName, int blockSize, int& result){
	fastipc::Client * client = new fastipc::Client();
	result = client->create(serverName, blockSize);
	return (int)client;
}

/*
* 该方法用于早fastipc客户端写入数据，数据写完成后，会触发服务端的侦听
* nativeClient	指向客户端实例的指针
* data			要写入的数据
* return		成功写入的数据长度
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
	// 写到客户端，并清理当前申请的内存
	client->write(userMsgType, userValue, shortStr, str, len);
	delete[] str;
	if (shortStr)delete[] shortStr;
	return 0;
}

/*
* 该方法用于关闭fastipc客户端
* nativeClient	指向客户端实例的指针
*/
EXPORT void WINAPI nCloseClient(int nativeClient){
	fastipc::Client * client = (fastipc::Client *) nativeClient;
	client->close();
}