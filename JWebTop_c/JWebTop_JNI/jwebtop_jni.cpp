// JNI主体实现
#include "jni_jdk1_6_24\jni.h"
#include "common/util/StrUtil.h"
#include "common/winctrl/JWebTopWinCtrl.h"

#include "org_jwebtop_JWebTopNative.h"
#include "javautils.h"
#include "common/os/OS.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
//typedef jboolean(JNICALL *GETAWT)(JNIEnv*, JAWT*);
using namespace std;

JNIEXPORT jlong JNICALL Java_org_jwebtop_JWebTopNative_nGetForegroundWindow(JNIEnv * env, jclass){
	return (jlong)GetForegroundWindow();
}

JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nShowWindow(JNIEnv * env, jclass, jlong hWnd, jint nCmdShow){
	jw::showWindow((HWND)hWnd, nCmdShow);
}
// 创建一个新进程，返回的数据为进程中主线程的id
JNIEXPORT jlong JNICALL Java_org_jwebtop_JWebTopNative_nCreateSubProcess(JNIEnv * env, jclass, jstring subProcess, jstring szCmdLine, jboolean waitFor){
#ifdef JWebTopLog
	setLogFileName(L"jwebtop_dll.log");
	writeLog(L"JNI开始启动新进程\r\n");
#endif
	wstring exe = jstring2wstring(env, subProcess), cmd = jstring2wstring(env, szCmdLine);
	DWORD processId = jw::os::process::createSubProcess(LPTSTR(exe.c_str()), LPTSTR(cmd.c_str()));
	if (waitFor){		
		HANDLE hHandle = OpenThread(SYNCHRONIZE, false, processId);// 降低权限，否则有些情况下OpendProcess失败（比如xp）
		WaitForSingleObject(hHandle, INFINITE);// 等待远程进程结束
		return 0;
	} else{
		return processId;
	}
}

/*
* 获取当前进程id
*
* return 返回当前进程id
*/
JNIEXPORT jlong JNICALL Java_org_jwebtop_JWebTopNative_nGetProcessID
(JNIEnv * env, jclass){
	return GetCurrentProcessId();
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
(JNIEnv *, jclass, jlong browserHWnd, jint style){
	jw::setWindowStyle((HWND)browserHWnd, style);
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