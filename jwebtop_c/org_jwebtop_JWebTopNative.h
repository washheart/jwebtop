/* DO NOT EDIT THIS FILE - it is machine generated */
#include "jni_jdk1_6_24\jni.h"
#include "jni_jdk1_6_24\jni_md.h"
#include "jni_jdk1_6_24\jawt_md.h"
/* Header for class org_jwebtop_JWebTopNative */

#ifndef _Included_org_jwebtop_JWebTopNative
#define _Included_org_jwebtop_JWebTopNative
#ifdef __cplusplus
extern "C" {
#endif

 // dll入口函数
 BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwReason, LPVOID lpReserved);
 
/*
 * 对应org.jwebtop.JWebTopNative类的createJWebTop方法
 * 该方法用于创建一个浏览器窗口
 * appfile 浏览器根据此配置文件进行初始化
 * parentWin 创建的浏览器的父窗口是哪个
 */
 JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nCreateJWebTop
(JNIEnv *, jclass, jstring appfile, jlong parentWin);

/*
 * 对应org.jwebtop.JWebTopNative类的executeJs方法
 * 该方法用于执行浏览器js脚本，由于cef不能直接返回脚本值，所以返回值通过回调java方式去传递
 * browserHWnd 要执行js的浏览器，browserHWnd在浏览器创建后通过回调方式传递给java程序    
 * json 需要执行的js脚本，这里是一个json字符串
 */
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJs
(JNIEnv *, jclass, jlong browserHWnd, jstring json);

// jni方法：设置窗口大小
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetSize
(JNIEnv *, jclass, jlong browserHWnd, jint w, jint h);

// jni方法：设置窗口位置
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetLocation
(JNIEnv *, jclass, jlong browserHWnd, jint x, jint y);

#ifdef __cplusplus
}
#endif
#endif
