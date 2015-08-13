#include "javautils.h"

JavaVM* g_jvm;// 保存全局的虚拟机环境

JNIEnv* GetJNIEnv() {
	JNIEnv *env = NULL;
	if (g_jvm->GetEnv((void**)&env, JNI_VERSION_1_6) == JNI_EDETACHED &&
		g_jvm->AttachCurrentThread((void**)&env, NULL) != JNI_OK) {
		return NULL;
	}
	return env;
}

void DetachFromThread(bool *mustDetach) {
	if (!g_jvm) {
		return;
	}
	if (*mustDetach)
		g_jvm->DetachCurrentThread();
}

//把java的字符串转换成windows的字符串   
char*   jstringToWindows(JNIEnv*   env, jstring   jstr)
{
	int   length = env->GetStringLength(jstr);
	const   jchar*   jcstr = env->GetStringChars(jstr, 0);
	char*   rtn = (char*)malloc(length * 2 + 1);
	int   size = 0;
	size = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)jcstr, length, rtn,
		(length * 2 + 1), NULL, NULL);
	if (size <= 0)
		return   NULL;
	env->ReleaseStringChars(jstr, jcstr);
	rtn[size] = 0;
	return   rtn;
}