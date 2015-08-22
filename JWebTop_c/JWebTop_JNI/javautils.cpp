#include "javautils.h"

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