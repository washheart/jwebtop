/// JNI实现类
/// 
/// 源码：http://git.oschina.net/washheart/fastipc
/// 源码：https://github.com/washheart/fastipc
/// 说明：https://github.com/washheart/fastipc/wiki
/// @version	1.0 
/// @author		washheart@163.com
/// @date       2015-10-31  
////////////////////////////////////////////////////////////////////////// 
#include "org_fastipc_FastIPCNative.h"
#include "jni_jdk1_6_24/jni.h"
#include "common/fastipc/Server.h"
#include "common/fastipc/Client.h"
#include "javautils.h"

JavaVM* g_jvm_server;               // 保存全局的虚拟机环境
class IPCReadListener :public fastipc::ReadListener{// 这里不采用RebuildedBlockListener，数据的组装留给java侦听器去做
private:
	jobject jlistener;
	jmethodID callBack;
public:
	int setListener(JNIEnv * env, jobject listener){
		jclass cls = env->GetObjectClass(listener);// 获得Java类  
		this->callBack = env->GetMethodID(cls, "OnRead",
			//	LONG			msgType;
			//	char			packId[PACK_ID_LEN];
			//	DWORD			dataLen;
			//	char*			data = NULL;
			//  对应的签名是：int,String,String
			"(ILjava/lang/String;[B)V");//或得该回调方法句柄
		if (this->callBack == NULL)return -1;
		this->jlistener = listener;
		return 0;
	}
	// 当有服务端读取到数据后，会调用此方法通知等待者进行处理
	// memBlock在分发后会由服务器销毁，外部调用者无需清理操作
	 void onRead(fastipc::MemBlock* memBlock) override{
		 JNIEnv *env;
		 bool detachEnv = false;
		 if (g_jvm_server->GetEnv((void **)&env, JNI_VERSION_1_6) < 0){
			 if (g_jvm_server->AttachCurrentThread((void **)&env, NULL) < 0){
				 return ;// 应该是程序的某个地方出错了
			 }
			 else{
				 detachEnv = true;// 如果是Attach上的env，那么需要detach
			 }
		 }
		 jstring packId = NULL;
		 if (memBlock->msgType != MSG_TYPE_NORMAL){
			 string pid=memBlock->getPackId();
			 packId = env->NewStringUTF(pid.c_str());
		 }

		 int len = memBlock->dataLen;
		 jbyteArray bytes = NULL;  //下面一系列操作把btPath转成jbyteArray 返回出去
		 bytes = env->NewByteArray(len);
		 env->SetByteArrayRegion(bytes, 0, len, (jbyte *)memBlock->data);
		 env->CallVoidMethod(this->jlistener, this->callBack,
			 memBlock->msgType, packId, bytes); //回调该方法，并且传递参数值		 
		 if (detachEnv)g_jvm_server->DetachCurrentThread();

		 // 通过对象方式实现动态参数。优点：很动态；缺点：设置数据都需要通过反射方式，性能不好
		 //this->callBack = env->GetMethodID(cls, "OnRead", "(Lorg/fastipc/FastIPCData;)V");//或得该回调方法句柄
		 //if (this->callBack == NULL)return -1;
		 //this->dataClass = env->FindClass("org/fastipc/FastIPCData"); // 得到用于返回数据用的类  
		 //if (this->dataClass == NULL)return -2;
		 ////获得得该类型的构造函数  函数名为 <init> 返回类型必须为 void 即 V  
		 //this->dataConstroc = env->GetMethodID(this->dataClass, "<init>", "()V");
		 // 新建对象
		 // 反射设置值

		 // 通过数组方式实现动态参数。优点：很动态；缺点：int也需要new Object方式，性能不好
		 //this->callBack = env->GetMethodID(cls, "OnRead", "([Ljava/lang/Object;)V");//或得该回调方法句柄
		 //jclass objClass = env->FindClass("java/lang/Object");
		 //jobjectArray rtn = env->NewObjectArray(4, objClass, NULL);
		 //jclass cls = env->FindClass("java/lang/Integer");
		 //jmethodID valueOf = env->GetStaticMethodID(cls, "valueOf", "(I)Ljava/lang/Integer;");
		 //jobject oInteger = env->CallStaticObjectMethod(cls, valueOf, 123);
		 //env->SetObjectArrayElement(rtn, 0, oInteger);
		 //jstring oString = env->NewStringUTF("abc");
		 //env->SetObjectArrayElement(rtn, 1, oString);
	 }
};
/*
* 对应org.fastipc.FastIPCNative类的nCreateServer方法
* 该方法用于创建一个fastipc服务端
* serverName	服务端名称
* blockSize		服务端与客户端通信时每次的数据量
* return		返回一个int[2]数组。0=是否创建成功；1=创建的Server的指针
*/
JNIEXPORT jintArray JNICALL Java_org_fastipc_FastIPCNative_nCreateServer
(JNIEnv * env, jclass, jstring serverName, jint blockSize){
	if (g_jvm_server == NULL)env->GetJavaVM(&g_jvm_server);// 获取当前的虚拟机环境并保存下来	
	fastipc::Server * server = new fastipc::Server();
	wstring _serverName = jstring2wstring(env, serverName);
	int result=server->create(_serverName, blockSize);
	jintArray rtn = env->NewIntArray(4);
	jint * tmp = new jint[2];
	tmp[0] = result;
	tmp[1] = (int)server;
	env->SetIntArrayRegion(rtn, 0, 4, tmp);
	delete[] tmp;
	return rtn;
}

/*
* 对应org.fastipc.FastIPCNative类的nStartRead方法
* 该方法用于开启fastipc服务端的读取侦听，成功后会阻塞在此处
* nativeServer	指向服务端实例的指针
* listener		响应读取操作的指针
*/
JNIEXPORT void JNICALL Java_org_fastipc_FastIPCNative_nStartRead
(JNIEnv * env, jclass, jint nativeServer, jobject listener){
	fastipc::Server * server = (fastipc::Server *) nativeServer;
	IPCReadListener * ipcListener = new IPCReadListener();
	if (ipcListener->setListener(env, listener) != 0)return;
	server->setListener(ipcListener);
	server->startRead();
}

/*
* 对应org.fastipc.FastIPCNative类的nCloseServer方法
* 该方法用于关闭fastipc服务端，执行后nStartRead的阻塞会停止
* nativeServer	指向服务端实例的指针
*/
JNIEXPORT void JNICALL Java_org_fastipc_FastIPCNative_nCloseServer
(JNIEnv *, jclass, jint nativeServer){
	fastipc::Server * server = (fastipc::Server *) nativeServer;
	server->close();
}

/*
* 对应org.fastipc.FastIPCNative类的nCreateClient方法
* 该方法用于创建一个fastipc客户端
* serverName	客户端对应的服务端的名称
* blockSize		服务端与客户端通信时每次的数据量
* return		返回一个int[2]数组。0=是否创建成功；1=创建的Client的指针
*/
JNIEXPORT jintArray JNICALL Java_org_fastipc_FastIPCNative_nCreateClient
(JNIEnv * env, jclass, jstring serverName, jint blockSize){
	fastipc::Client * client = new fastipc::Client();
	wstring _serverName = jstring2wstring(env, serverName);
	int result = client->create(_serverName, blockSize);
	jintArray rtn = env->NewIntArray(4);
	jint * tmp = new jint[2];
	tmp[0] = result;
	tmp[1] = (int)client;
	env->SetIntArrayRegion(rtn, 0, 4, tmp);
	delete[] tmp;
	return rtn;
}
/*
* 对应org.fastipc.FastIPCNative类的nWriteClient方法
* 该方法用于早fastipc客户端写入数据，数据写完成后，会触发服务端的侦听
* nativeClient	指向客户端实例的指针
* data			要写入的数据
* return		成功写入的数据长度
*/
JNIEXPORT jint JNICALL Java_org_fastipc_FastIPCNative_nWriteClient
(JNIEnv * env, jclass, jint nativeClient, jint userMsgType, jlong userValue, jstring userShortStr, jstring data){
	fastipc::Client * client = (fastipc::Client *) nativeClient;
	// data
	const char * tmp = env->GetStringUTFChars(data, false);
	int len = lstrlenA(tmp);
	char * str = new char[len];
	memcpy(str, tmp, len);
	env->ReleaseStringUTFChars(data, tmp);
	// userShortStr
	const char * tmp2 = env->GetStringUTFChars(userShortStr, false);
	int len2 = lstrlenA(tmp);
	char * shortStr = new char[len2];
	memcpy(shortStr, tmp2, len);
	env->ReleaseStringUTFChars(userShortStr, tmp2);
	//
	client->write(userMsgType,userValue,shortStr,str,len);
	delete[] str; 
	delete[] userShortStr;
	return 0;
}
/*
* 对应org.fastipc.FastIPCNative类的nCloseClient方法
* 该方法用于关闭fastipc客户端
* nativeClient	指向客户端实例的指针
*/
JNIEXPORT void JNICALL Java_org_fastipc_FastIPCNative_nCloseClient
(JNIEnv *, jclass, jint nativeClient){
	fastipc::Client * client = (fastipc::Client *) nativeClient;
	client->close();
}