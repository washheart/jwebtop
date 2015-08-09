#include <windows.h> 
#include "jni_jdk1_6_24/jni.h"
#include "common/util/StrUtil.h"

JNIEnv* GetJNIEnv();
void DetachFromThread(bool *mustDetach);

//把java的字符串转换成windows的字符串   
char*   jstringToWindows(JNIEnv*   env, jstring   jstr);