#include <windows.h> 
#include "jni_jdk1_6_24/jni.h"
#include "common/util/StrUtil.h"

std::string jstring2string(JNIEnv * env, jstring jstr);
std::wstring jstring2wstring(JNIEnv * env, jstring jstr);
//jstring wstring2jstring(JNIEnv * env, wstring wstr);