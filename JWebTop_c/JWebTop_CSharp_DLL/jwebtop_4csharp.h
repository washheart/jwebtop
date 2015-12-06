#ifndef _Included_org_jwebtop_dll4csharp
#define _Included_org_jwebtop_dll4csharp
#include <string>
#include <Windows.h>

#define EXPORT __declspec(dllexport)

extern "C" {
	// 创建一个新进程，返回的数据为进程中主线程的id
	// 创建一个新进程，返回的数据为进程中主线程的id
	EXPORT long WINAPI nCreateSubProcess(LPTSTR subProcess, LPTSTR szCmdLine, int waitFor);

	/*
	* 该方法用于创建一个fastipc服务端
	* serverName	服务端名称
	* blockSize		服务端与客户端通信时每次的数据量
	* result		（传出参数）是否创建成功，0=成功
	* return		创建的Server的指针
	*/
	EXPORT long WINAPI nCreateServer(LPTSTR serverName, int blockSize, int& result);

	// 定义一个回调函数，csharp端实现此回调函数
	typedef void *(CALLBACK *CSharpCallBack)(int msgType, char* packId,
		int userMsgType, long userValue, char* userShortStr,
		char* data, int dataLen);

	/*
	* 该方法用于开启fastipc服务端的读取侦听，成功后会阻塞在此处
	* nativeServer	指向服务端实例的指针
	* listener		响应读取操作的指针
	*/
	EXPORT void WINAPI nStartRead(int nativeServer, CSharpCallBack callBack);

	/*
	* 该方法用于关闭fastipc服务端，执行后nStartRead的阻塞会停止
	* nativeServer	指向服务端实例的指针
	*/
	EXPORT void WINAPI nCloseServer(int nativeServer);

	/*
	* 该方法用于创建一个fastipc客户端
	* serverName	客户端对应的服务端的名称
	* blockSize		服务端与客户端通信时每次的数据量
	* result		（传出参数）是否创建成功，0=成功
	* return		创建的Client的指针
	*/
	EXPORT int WINAPI nCreateClient(LPTSTR serverName, int blockSize, int& result);

	/*
	* 该方法用于早fastipc客户端写入数据，数据写完成后，会触发服务端的侦听
	* nativeClient	指向客户端实例的指针
	* data			要写入的数据
	* return		成功写入的数据长度
	*/
	EXPORT int WINAPI nWriteClient(int nativeClient, int userMsgType, int userValue, LPTSTR userShortStr, LPTSTR data);

	/*
	* 该方法用于关闭fastipcnWriteClient
	* nativeClient	指向客户端实例的指针
	*/
	EXPORT void WINAPI nCloseClient(int nativeClient);
}
#endif