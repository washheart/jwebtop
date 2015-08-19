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


std::string jstring2string(JNIEnv * env, jstring jstr){
	const char * tmp = env->GetStringUTFChars(jstr, false);
	string js(tmp);
	env->ReleaseStringUTFChars(jstr, tmp);
	return js;
}
std::wstring jstring2wstring(JNIEnv * env, jstring jstr){
	const char * tmp = env->GetStringUTFChars(jstr, false);
	string js(tmp);
	env->ReleaseStringUTFChars(jstr, tmp);
	return jw::s2w(js);
}