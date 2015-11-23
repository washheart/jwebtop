//////////////////////////////////////////////////////////////////////////  
/// 定义fastipc的对应的jni相关方法
/// 
/// 源码：http://git.oschina.net/washheart/fastipc
/// 源码：https://github.com/washheart/fastipc
/// 说明：https://github.com/washheart/fastipc/wiki
/// @version	1.0 
/// @author		washheart@163.com
/// @date       2015-10-31  
////////////////////////////////////////////////////////////////////////// 
#include "jni_jdk1_6_24/jni.h"
//#include "jni_jdk1_6_24\jni_md.h"
//#include "jni_jdk1_6_24\jawt_md.h"

#ifndef _Included_org_fastipc_FastIPCNative
#define _Included_org_fastipc_FastIPCNative

#ifdef __cplusplus
extern "C" {
#endif

	/*
	* 对应org.fastipc.FastIPCNative类的nCreateServer方法
	* 该方法用于创建一个fastipc服务端
	* serverName	服务端名称
	* blockSize		服务端与客户端通信时每次的数据量
	* return		返回一个int[2]数组。0=是否创建成功；1=创建的Server的指针
	*/
	JNIEXPORT jintArray JNICALL Java_org_fastipc_FastIPCNative_nCreateServer
		(JNIEnv *, jclass, jstring serverName, jint blockSize);

	/*
	* 对应org.fastipc.FastIPCNative类的nStartRead方法
	* 该方法用于开启fastipc服务端的读取侦听，成功后会阻塞在此处
	* nativeServer	指向服务端实例的指针
	* listener		响应读取操作的指针
	*/
	JNIEXPORT void JNICALL Java_org_fastipc_FastIPCNative_nStartRead
		(JNIEnv *, jclass, jint nativeServer, jobject listener);

	/*
	* 对应org.fastipc.FastIPCNative类的nCloseServer方法
	* 该方法用于关闭fastipc服务端，执行后nStartRead的阻塞会停止
	* nativeServer	指向服务端实例的指针
	*/
	JNIEXPORT void JNICALL Java_org_fastipc_FastIPCNative_nCloseServer
		(JNIEnv *, jclass, jint nativeServer);

	/*
	* 对应org.fastipc.FastIPCNative类的nCreateClient方法
	* 该方法用于创建一个fastipc客户端
	* serverName	客户端对应的服务端的名称
	* blockSize		服务端与客户端通信时每次的数据量
	* return		返回一个int[2]数组。0=是否创建成功；1=创建的Client的指针
	*/
	JNIEXPORT jintArray JNICALL Java_org_fastipc_FastIPCNative_nCreateClient
		(JNIEnv *, jclass, jstring serverName, jint blockSize);

	/*
	* 对应org.fastipc.FastIPCNative类的nWriteClient方法
	* 该方法用于早fastipc客户端写入数据，数据写完成后，会触发服务端的侦听
	* nativeClient	指向客户端实例的指针
	* data			要写入的数据
	* return		成功写入的数据长度
	*/
	JNIEXPORT jint JNICALL Java_org_fastipc_FastIPCNative_nWriteClient
		(JNIEnv * env, jclass, jint nativeClient, jint userMsgType, jlong userValue, jstring userShortStr, jstring data);

	/*
	* 对应org.fastipc.FastIPCNative类的nCloseClient方法
	* 该方法用于关闭fastipc客户端
	* nativeClient	指向客户端实例的指针
	*/
	JNIEXPORT void JNICALL Java_org_fastipc_FastIPCNative_nCloseClient
		(JNIEnv *, jclass, jint nativeClient);
#ifdef __cplusplus
}
#endif
#endif
