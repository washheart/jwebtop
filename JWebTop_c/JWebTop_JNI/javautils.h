#include <windows.h> 
#include "jni_jdk1_6_24/jni.h"
#include "JWebTop/util/StrUtil.h"

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