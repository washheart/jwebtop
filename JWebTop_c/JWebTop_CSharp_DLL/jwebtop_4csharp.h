#ifndef _Included_org_jwebtop_dll4csharp
#define _Included_org_jwebtop_dll4csharp
#include <string>
#include <Windows.h>

#define EXPORT __declspec(dllexport)

extern "C" {

	/*
	* 对应org.fastipc.FastIPCNative类的nCreateServer方法
	* 该方法用于创建一个fastipc服务端
	* serverName	服务端名称
	* blockSize		服务端与客户端通信时每次的数据量
	* result		（传出参数）是否创建成功，0=成功
	* return		创建的Server的指针
	*/
	EXPORT HWND WINAPI nCreateServer(LPTSTR serverName, int blockSize,int& result);

	/*
	* 对应org.fastipc.FastIPCNative类的nStartRead方法
	* 该方法用于开启fastipc服务端的读取侦听，成功后会阻塞在此处
	* nativeServer	指向服务端实例的指针
	* listener		响应读取操作的指针
	*/
	EXPORT void WINAPI nStartRead(int nativeServer, jobject listener);

	/*
	* 对应org.fastipc.FastIPCNative类的nCloseServer方法
	* 该方法用于关闭fastipc服务端，执行后nStartRead的阻塞会停止
	* nativeServer	指向服务端实例的指针
	*/
	EXPORT void WINAPI nCloseServer(int nativeServer);

	/*
	* 对应org.fastipc.FastIPCNative类的nCreateClient方法
	* 该方法用于创建一个fastipc客户端
	* serverName	客户端对应的服务端的名称
	* blockSize		服务端与客户端通信时每次的数据量
	* result		（传出参数）是否创建成功，0=成功
	* return		创建的Client的指针
	*/
	EXPORT HWND WINAPI nCreateClient(LPTSTR serverName, int blockSize,int& result);

	/*
	* 对应org.fastipc.FastIPCNative类的nWriteClient方法
	* 该方法用于早fastipc客户端写入数据，数据写完成后，会触发服务端的侦听
	* nativeClient	指向客户端实例的指针
	* data			要写入的数据
	* return		成功写入的数据长度
	*/
	EXPORT int WINAPI nWriteClient(int nativeClient, int userMsgType, long userValue, LPTSTR userShortStr, LPTSTR data);

	/*
	* 对应org.fastipc.FastIPCNative类的nCloseClient方法
	* 该方法用于关闭fastipc客户端
	* nativeClient	指向客户端实例的指针
	*/
	EXPORT void WINAPI nCloseClient(int nativeClient);

	//typedef char*(CALLBACK *CSharpCallBack)(long browserHWnd, char* s); // 定义一个回调函数，csharp端实现此回调函数
	//EXPORT void WINAPI SetCSharpCallback(CSharpCallBack callBack);      // 设置回调函数
	//EXPORT void WINAPI DelPtrInDLL(void *p);							// 用于（DLL调用者）删除dll中定义的指针
	//
	//// 以下方法包装jwebtop_dll对应方法
	//EXPORT long  WINAPI nCreateJWebTop(LPTSTR processPath, LPTSTR configFile);
	//EXPORT void  WINAPI nExitJWebTop();
	//EXPORT long  WINAPI nCreateJWebTopBrowser(LPTSTR jWebTopConfigJSON);
	//EXPORT void  WINAPI nCloseJWebTopBrowser(long browserHWnd);
	//EXPORT char* WINAPI nJWebTopExecuteJSWait(long browserHWnd, LPTSTR script);
	//EXPORT void  WINAPI nJWebTopExecuteJSNoWait(long browserHWnd, LPTSTR script);
	//EXPORT char* WINAPI nJWebTopExecuteJSONWait(long browserHWnd, LPTSTR json);
	//EXPORT void  WINAPI nJWebTopExecuteJSONNoWait(long browserHWnd, LPTSTR json);
}
#endif