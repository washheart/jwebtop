// JNI主体实现
#include "jni_jdk1_6_24\jni.h"
#include <Shlwapi.h>
#include "common/task/Task.h"
#include "common/JWebTopMsg.h"
#include "common/util/StrUtil.h"
#include "common/msgwin/MsgWin.h"
#include "common/winctrl/JWebTopWinCtrl.h"

#include "org_jwebtop_JWebTopNative.h"
#include "common_dll/jwebtop_dll.h"
#include "javautils.h"
typedef jboolean(JNICALL *GETAWT)(JNIEnv*, JAWT*);
using namespace std;

JavaVM* g_jvm;               // 保存全局的虚拟机环境
jclass g_nativeClass;        // 记录全局的本地类变量
jmethodID g_invokeByJS;      // 从C端回调Java的方法
extern HWND g_RemoteWinHWnd;

// 实现回调java程序的方法
wstring invokeByBrowser(long browserHWnd, wstring json){
	JNIEnv *env;
	bool detachEnv = false;
	if (g_jvm->GetEnv((void **)&env, JNI_VERSION_1_6) < 0){
		if (g_jvm->AttachCurrentThread((void **)&env, NULL) < 0){
			return wstring();
		}
		else{
			detachEnv = true;// 如果是Attach上的env，那么需要detach
		}
	}
	jstring sss = env->NewStringUTF(CefString(json).ToString().c_str());
	jstring str = (jstring)env->CallStaticObjectMethod(g_nativeClass, g_invokeByJS, (jlong)browserHWnd, sss);
	env->DeleteLocalRef(sss);
	CefString result(env->GetStringUTFChars(str, false));
	env->DeleteLocalRef(str);
	if (detachEnv)g_jvm->DetachCurrentThread();
	return result.ToWString();
}

// jni方法：创建JWebTop进程
// 这里暂时没有办法用JSON的方式来传递配置：因为用cmd方式发送数据时，系统会默认把参数中的双引号给去掉（或者，先创建窗口，再创建浏览器？）
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nCreateJWebTop
(JNIEnv * env, jclass nativeClass, jstring jprocessPath, jstring configFile){
	if (g_RemoteWinHWnd != NULL)return /*(jlong)g_RemoteWinHWnd*/;
	if (g_invokeByJS == NULL){// 第一次被java端调用
		env->GetJavaVM(&g_jvm);// 获取当前的虚拟机环境并保存下来		
		g_nativeClass = (jclass)(env->NewGlobalRef(nativeClass));// 将一个对象设置为全局对象，此处将nativeClasss设置为全局对象
		g_invokeByJS = env->GetStaticMethodID(g_nativeClass, "invokeByJS", "(JLjava/lang/String;)Ljava/lang/String;");// 取出要调用的方法
	}
	wstring processPath = jstring2wstring(env, jprocessPath); 
	wstring cfgFile = jstring2wstring(env, configFile);
	CreateJWebTop(processPath, cfgFile);
}
// jni方法：退出JWebTop进程
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nExit(JNIEnv *, jclass){
	ExitJWebTop();
}

/*
* 对应org.jwebtop.JWebTopNative类的nCreateBrowser方法
* 该方法用于创建一个浏览器窗口
* jWebTopConfigJSON 浏览器配置信息JSON
*/
JNIEXPORT jlong JNICALL Java_org_jwebtop_JWebTopNative_nCreateBrowser
(JNIEnv * env, jclass, jstring jWebTopConfigJSON){
	wstring cmds = jstring2wstring(env, jWebTopConfigJSON);
	return CreateJWebTopBrowser(cmds);
}

/*
 * 对应org.jwebtop.JWebTopNative类的nCloseBrowser方法
 * 该方法用于关闭一个浏览器窗口
 * browserHWnd  浏览器窗口句柄
 */
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nCloseBrowser
(JNIEnv * env, jclass, jlong browserHWnd){
	CloseJWebTopBrowser(browserHWnd);
}

JNIEXPORT jstring JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJSWait
(JNIEnv * env, jclass, jlong browserHWnd, jstring script){
	return wstring2jstring(env, JWebTopExecuteJSWait(browserHWnd, jstring2wstring(env, script)));
}

JNIEXPORT jstring JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJSONWait
(JNIEnv * env, jclass, jlong browserHWnd, jstring json){
	return wstring2jstring(env, JWebTopExecuteJSONWait(browserHWnd, jstring2wstring(env, json)));
}

JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJSNoWait
(JNIEnv * env, jclass, jlong browserHWnd, jstring script){
	JWebTopExecuteJSNoWait(browserHWnd, jstring2wstring(env, script));
}

// jni方法：执行脚本且不等待返回结果
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJSONNoWait
(JNIEnv * env, jclass, jlong browserHWnd, jstring json){
	JWebTopExecuteJSONNoWait(browserHWnd, jstring2wstring(env, json));
}

// jni方法：设置窗口大小
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetSize
(JNIEnv * env, jclass, jlong browserHWnd, jint w, jint h){
	jw::setSize((HWND)browserHWnd, w, h);
}
JNIEXPORT jintArray JNICALL Java_org_jwebtop_JWebTopNative_nGetSize
(JNIEnv * env, jclass, jlong browserHWnd){
	POINT p = jw::getSize((HWND)browserHWnd);
	jintArray rtn = env->NewIntArray(2);
	jint * tmp = new jint[2];
	tmp[0] = p.x;
	tmp[1] = p.y;
	delete[] tmp;
	env->SetIntArrayRegion(rtn, 0, 2, tmp);
	return rtn;
}
JNIEXPORT jintArray JNICALL Java_org_jwebtop_JWebTopNative_nGetScreenSize
(JNIEnv * env, jclass){
	POINT p = jw::getScreenSize();
	jintArray rtn = env->NewIntArray(2);
	jint * tmp = new jint[2];
	tmp[0] = p.x;
	tmp[1] = p.y;
	delete[] tmp;
	env->SetIntArrayRegion(rtn, 0, 2, tmp);
	return rtn;
}
// jni方法：设置窗口位置
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetLocation
(JNIEnv * env, jclass, jlong browserHWnd, jint x, jint y){
	jw::move((HWND)browserHWnd, x, y);
}
JNIEXPORT jintArray JNICALL Java_org_jwebtop_JWebTopNative_nGetLocation
(JNIEnv * env, jclass, jlong browserHWnd){
	POINT p = jw::getPos((HWND)browserHWnd);
	jintArray rtn = env->NewIntArray(2);
	jint * tmp = new jint[2];
	tmp[0] = p.x;
	tmp[1] = p.y;
	delete[] tmp;
	env->SetIntArrayRegion(rtn, 0, 2, tmp);
	return rtn;
}

// jni方法：设置窗口位置和大小
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetBound
(JNIEnv *, jclass, jlong browserHWnd, jint x, jint y, int w, int h){
	jw::setBound((HWND)browserHWnd, x, y, w, h);
}
JNIEXPORT jintArray JNICALL Java_org_jwebtop_JWebTopNative_nGetBound
(JNIEnv * env, jclass, jlong browserHWnd){
	RECT rc = jw::getBound((HWND)browserHWnd);
	jintArray rtn = env->NewIntArray(4);
	jint * tmp = new jint[4];
	tmp[0] = rc.left;
	tmp[1] = rc.top;
	tmp[2] = rc.right - rc.left;
	tmp[3] = rc.bottom - rc.top;
	delete[] tmp;
	env->SetIntArrayRegion(rtn, 0, 4, tmp);
	return rtn;
}

//窗口移到最顶层
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nBringToTop
(JNIEnv *, jclass, jlong browserHWnd){
	jw::bringToTop((HWND)browserHWnd);
}
//使窗口获得焦点
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nFocus
(JNIEnv *, jclass, jlong browserHWnd){
	jw::focus((HWND)browserHWnd);
}
//隐藏窗口
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nHide
(JNIEnv *, jclass, jlong browserHWnd){
	jw::hide((HWND)browserHWnd);
}
//最大化窗口
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nMax
(JNIEnv *, jclass, jlong browserHWnd){
	jw::max((HWND)browserHWnd);
}
//最小化窗口
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nMini
(JNIEnv *, jclass, jlong browserHWnd){
	jw::mini((HWND)browserHWnd);
}
//还原窗口，对应于hide函数
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nRestore
(JNIEnv *, jclass, jlong browserHWnd){
	jw::restore((HWND)browserHWnd);
}

// 窗口是否显示
JNIEXPORT jboolean JNICALL Java_org_jwebtop_JWebTopNative_nIsVisible(JNIEnv *, jclass, jlong browserHWnd){
	return jw::isVisible((HWND)browserHWnd);
}
//窗口置顶，此函数跟bringToTop的区别在于此函数会使窗口永远置顶，除非有另外一个窗口调用了置顶函数
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetTopMost
(JNIEnv *, jclass, jlong browserHWnd){
	jw::setTopMost((HWND)browserHWnd);
}

JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetWindowStyle
(JNIEnv *, jclass, jlong browserHWnd,jint style){
	jw::setWindowStyle((HWND)browserHWnd,style);
}
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetWindowExStyle
(JNIEnv *, jclass, jlong browserHWnd, jint exStyle){
	jw::setWindowExStyle((HWND)browserHWnd, exStyle);
}

JNIEXPORT jintArray Java_org_jwebtop_JWebTopNative_nGetWindowClient(JNIEnv * env, jclass, jlong hWnd){
	WINDOWINFO winInfo;
	GetWindowInfo((HWND)hWnd, &winInfo);// 获取窗口信息
	RECT rc = winInfo.rcClient;
	jintArray rtn = env->NewIntArray(4);
	jint * tmp = new jint[4];
	tmp[0] = rc.left;
	tmp[1] = rc.top;
	tmp[2] = rc.right;
	tmp[3] = rc.bottom;
	delete[] tmp;
	env->SetIntArrayRegion(rtn, 0, 4, tmp);
	return rtn;
}