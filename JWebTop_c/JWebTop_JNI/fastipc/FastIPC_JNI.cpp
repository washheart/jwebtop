/// JNIʵ����
/// 
/// Դ�룺http://git.oschina.net/washheart/fastipc
/// Դ�룺https://github.com/washheart/fastipc
/// ˵����https://github.com/washheart/fastipc/wiki
/// @version	1.0 
/// @author		washheart@163.com
/// @date       2015-10-31  
////////////////////////////////////////////////////////////////////////// 
#include "org_fastipc_FastIPCNative.h"
#include <sstream>
#include "jni_jdk1_6_24/jni.h"
#include "common/fastipc/Server.h"
#include "common/fastipc/Client.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
#include "common/util/StrUtil.h"
#include "javautils.h"

JavaVM* g_jvm_server;               // ����ȫ�ֵ����������
class IPCReadListener :public fastipc::ReadListener{// ���ﲻ����RebuildedBlockListener�����ݵ���װ����java������ȥ��
private:
	jobject jlistener;
	jmethodID callBack;
public:
	int setListener(JNIEnv * env, jobject listener){
		jclass cls = env->GetObjectClass(listener);// ���Java��  
		this->callBack = env->GetMethodID(cls, "OnRead",
			// ��Ӧ��ǩ���ǣ�int,String,int,long,String,byte[]
			"(ILjava/lang/String;IJLjava/lang/String;[B)V");//��øûص��������
		if (this->callBack == NULL)return -1;
		this->jlistener = listener;
		return 0;
	}
	// ���з���˶�ȡ�����ݺ󣬻���ô˷���֪ͨ�ȴ��߽��д���
	// memBlock�ڷַ�����ɷ��������٣��ⲿ�����������������
	 void onRead(fastipc::MemBlock* memBlock) override{
#ifdef JWebTopLog
		 std::wstringstream wss;
		 wss << L"Readed "
			 << L" userMsgType=" << memBlock->userMsgType
			 << L" userValue=" << memBlock->userValue
			 << L" userShortStr=" << jw::str::s2w(memBlock->getUserShortStr())
			 << L" pBuff=" << jw::str::s2w(memBlock->getData())
			 << L"||\r\n";
		 writeLog(wss.str());
#endif
		 JNIEnv *env;
		 bool detachEnv = false;
		 if (g_jvm_server->GetEnv((void **)&env, JNI_VERSION_1_6) < 0){
			 if (g_jvm_server->AttachCurrentThread((void **)&env, NULL) < 0){
				 return ;// Ӧ���ǳ����ĳ���ط�������
			 }
			 else{
				 detachEnv = true;// �����Attach�ϵ�env����ô��Ҫdetach
			 }
		 }
		 jstring packId = NULL;
		 if (memBlock->msgType != MSG_TYPE_NORMAL){
			 string pid=memBlock->getPackId();
			 packId = env->NewStringUTF(pid.c_str());
		 }

		 int len = memBlock->dataLen;
		 jbyteArray bytes = NULL;  //����һϵ�в�����btPathת��jbyteArray ���س�ȥ
		 bytes = env->NewByteArray(len);
		 env->SetByteArrayRegion(bytes, 0, len, (jbyte *)memBlock->data);

		 jstring userShortValue = env->NewStringUTF(memBlock->userShortStr);
		 jclass cls = env->GetObjectClass(jlistener);// ���Java��  
		 //jmethodID m2 = env->GetMethodID(cls, "OnRead", "()V");//��øûص��������
		 //env->CallVoidMethod(this->jlistener, m2);
		 //jmethodID m3 = env->GetMethodID(cls, "OnRead", "(Ljava/lang/String;)V");//��øûص��������
		 //env->CallVoidMethod(this->jlistener, m3, userShortValue);
		 this->callBack = env->GetMethodID(cls, "OnRead",
			 // ��Ӧ��ǩ���ǣ�int,String,int,long,String,byte[]
			 "(ILjava/lang/String;IJLjava/lang/String;[B)V");//��øûص��������
		 env->CallVoidMethod(this->jlistener, this->callBack,
			 (jint)memBlock->msgType, packId, 
			 (jint)memBlock->userMsgType, (jlong)memBlock->userValue, userShortValue, bytes); //�ص��÷��������Ҵ��ݲ���ֵ		 
		 if (detachEnv)g_jvm_server->DetachCurrentThread();

		 // ͨ������ʽʵ�ֶ�̬�������ŵ㣺�ܶ�̬��ȱ�㣺�������ݶ���Ҫͨ�����䷽ʽ�����ܲ���
		 //this->callBack = env->GetMethodID(cls, "OnRead", "(Lorg/fastipc/FastIPCData;)V");//��øûص��������
		 //if (this->callBack == NULL)return -1;
		 //this->dataClass = env->FindClass("org/fastipc/FastIPCData"); // �õ����ڷ��������õ���  
		 //if (this->dataClass == NULL)return -2;
		 ////��õø����͵Ĺ��캯��  ������Ϊ <init> �������ͱ���Ϊ void �� V  
		 //this->dataConstroc = env->GetMethodID(this->dataClass, "<init>", "()V");
		 // �½�����
		 // ��������ֵ

		 // ͨ�����鷽ʽʵ�ֶ�̬�������ŵ㣺�ܶ�̬��ȱ�㣺intҲ��Ҫnew Object��ʽ�����ܲ���
		 //this->callBack = env->GetMethodID(cls, "OnRead", "([Ljava/lang/Object;)V");//��øûص��������
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
* ��Ӧorg.fastipc.FastIPCNative���nCreateServer����
* �÷������ڴ���һ��fastipc�����
* serverName	���������
* blockSize		�������ͻ���ͨ��ʱÿ�ε�������
* return		����һ��int[2]���顣0=�Ƿ񴴽��ɹ���1=������Server��ָ��
*/
JNIEXPORT jintArray JNICALL Java_org_fastipc_FastIPCNative_nCreateServer
(JNIEnv * env, jclass, jstring serverName, jint blockSize){
	if (g_jvm_server == NULL)env->GetJavaVM(&g_jvm_server);// ��ȡ��ǰ���������������������	
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
* ��Ӧorg.fastipc.FastIPCNative���nStartRead����
* �÷������ڿ���fastipc����˵Ķ�ȡ�������ɹ���������ڴ˴�
* nativeServer	ָ������ʵ����ָ��
* listener		��Ӧ��ȡ������ָ��
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
* ��Ӧorg.fastipc.FastIPCNative���nCloseServer����
* �÷������ڹر�fastipc����ˣ�ִ�к�nStartRead��������ֹͣ
* nativeServer	ָ������ʵ����ָ��
*/
JNIEXPORT void JNICALL Java_org_fastipc_FastIPCNative_nCloseServer
(JNIEnv *, jclass, jint nativeServer){
	fastipc::Server * server = (fastipc::Server *) nativeServer;
	server->close();
}

/*
* ��Ӧorg.fastipc.FastIPCNative���nCreateClient����
* �÷������ڴ���һ��fastipc�ͻ���
* serverName	�ͻ��˶�Ӧ�ķ���˵�����
* blockSize		�������ͻ���ͨ��ʱÿ�ε�������
* return		����һ��int[2]���顣0=�Ƿ񴴽��ɹ���1=������Client��ָ��
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
* ��Ӧorg.fastipc.FastIPCNative���nWriteClient����
* �÷���������fastipc�ͻ���д�����ݣ�����д��ɺ󣬻ᴥ������˵�����
* nativeClient	ָ��ͻ���ʵ����ָ��
* data			Ҫд�������
* return		�ɹ�д������ݳ���
*/
JNIEXPORT jint JNICALL Java_org_fastipc_FastIPCNative_nWriteClient
(JNIEnv * env, jclass, jint nativeClient, jint userMsgType, jlong userValue, jstring userShortStr, jstring data){
	fastipc::Client * client = (fastipc::Client *) nativeClient;
	char * shortStr = NULL;
	char * str = NULL;
	int len = 0;
	// data
	if (data != NULL){
		const char * tmp = env->GetStringUTFChars(data, false);
		len = lstrlenA(tmp);
		str = new char[len+1];
		memcpy(str, tmp, len);
		str[len] = '\0';
		env->ReleaseStringUTFChars(data, tmp);
	}
	// userShortStr
	if (userShortStr != NULL){
		const char * tmp2 = env->GetStringUTFChars(userShortStr, false);
		int len2 = lstrlenA(tmp2);
		shortStr = new char[len2+1];
		memcpy(shortStr, tmp2, len2);
		shortStr[len2] = '\0';
		env->ReleaseStringUTFChars(userShortStr, tmp2);
	}
	// д���ͻ��ˣ�������ǰ������ڴ�
#ifdef JWebTopLog
	std::wstringstream wss;
	wss << L"Writed "
		<< L" userMsgType=" << userMsgType
		<< L" userValue=" << userValue;
	if (shortStr != NULL)wss << L" userShortStr=" << shortStr;
	if (str != NULL)wss << L" data=" << str;
		wss<< L"||\r\n";
	writeLog(wss.str());
#endif
	client->write(userMsgType, userValue, shortStr, str, len);
	delete[] str; 
	delete[] shortStr;
	return 0;
}
/*
* ��Ӧorg.fastipc.FastIPCNative���nCloseClient����
* �÷������ڹر�fastipc�ͻ���
* nativeClient	ָ��ͻ���ʵ����ָ��
*/
JNIEXPORT void JNICALL Java_org_fastipc_FastIPCNative_nCloseClient
(JNIEnv *, jclass, jint nativeClient){
	fastipc::Client * client = (fastipc::Client *) nativeClient;
	client->close();
}