// JNI主体实现
#include "jni_jdk1_6_24\jni.h"
#include <Shlwapi.h>
#include "common/task/Task.h"
#include "common/JWebTopMsg.h"
#include "common/util/StrUtil.h"
#include "common/winctrl/JWebTopWinCtrl.h"

#include "org_jwebtop_JWebTopNative.h"
#include "javautils.h"
#include "common/msgwin/MsgWin.h"
typedef jboolean(JNICALL *GETAWT)(JNIEnv*, JAWT*);
typedef BOOL(WINAPI *lpfnSetLayeredWindowAttributes)(HWND hWnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);
using namespace std;
extern JavaVM* g_jvm;// 保存全局的虚拟机环境
jclass g_nativeClass;// 记录全局的本地类变量
jmethodID g_invokeByJS;// 从C端回调Java的方法
extern HWND g_RemoteWinHWnd;

//extern HINSTANCE g_instance;
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved){
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//HINSTANCE exe_hInstance = ::GetModuleHandle(NULL);
		//g_instance = hModule;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		// 是不是这里要做些清理操作？比如关闭所有浏览器
		break;
	case DLL_PROCESS_DETACH:
		//jw::sendProcessMsg(g_RemoteWinHWnd, WM_COPYDATA_EXIT, L"");// 进程异常结束时走不到此处，所以这里没用
		break;
	}
	return TRUE;
}
// 用于回调java程序的方法（定义于jwebtop_brige.h）
wstring invoke_Wait(DWORD msgId, wstring json){
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
	jstring str = (jstring)env->CallStaticObjectMethod(g_nativeClass, g_invokeByJS, sss);
	env->DeleteLocalRef(sss);
	char * tmp = jstringToWindows(env, str);
	CefString result(env->GetStringUTFChars(str, false));
	env->DeleteLocalRef(str);
	free(tmp);
	if (detachEnv)g_jvm->DetachCurrentThread();
	return result.ToWString();
}

void jw::msgwin_thread_executeWmCopyData( DWORD msgId, wstring json){
	if (msgId == JWM_RESULT_RETURN){// 远程任务已完成，结果发回来了，需要通知本进程的等待线程去获取结果
		wstring taskId, result;
		long browserHWnd;
		jw::parseMessageJSON(json, ref(browserHWnd), ref(taskId), ref(result));  // 从任务信息中解析出任务id和任务描述
		jw::task::putTaskResult(taskId, result);						   // 通知等待线程，远程任务已完成，结果已去取回
	}
	else if (msgId == JWM_DLL_EXECUTE_WAIT){// 远程进程发来一个任务，并且远程进程正在等待，任务完成后需要发送JWEBTOP_MSG_RESULT_RETURN消息给远程进程
		wstring remoteTaskId, taskInfo;
		long browserHWnd;
		jw::parseMessageJSON(json, ref(browserHWnd), ref(remoteTaskId), ref(taskInfo));        // 从任务信息中解析出任务id和任务描述
		wstring result = invoke_Wait(msgId, taskInfo); 		         // 取回执行结果
		wstring wrapped = jw::wrapAsTaskJSON(browserHWnd,ref(remoteTaskId), ref(result));      // 包装结果任务
		sendProcessMsg((HWND)browserHWnd,JWM_RESULT_RETURN, LPTSTR(wrapped.c_str())); // 发送结果到远程进程
	}
	else{// 其他情况按远程发来的无需等待任务执行
		invoke_Wait(msgId, json);
	}
}

// 用于createWin进行回调
void jw::onWindowHwndCreated(HWND hWnd, LPTSTR szCmdLine){
	LPTSTR exe = L"D:\\c\\jwebtop\\JWebTop_c\\JWebTop\\Release\\JWebTop.exe";
	//LPTSTR exe = L"JWebTop.exe";
	wstringstream cmd;
	cmd << L": ";// 标记为特殊命令
	cmd << (long)hWnd << " ";// 追加msgWin的HWND
	cmd << szCmdLine;// 其他命令行参数
	createSubProcess(exe, LPTSTR(cmd.str().c_str()));
}
void appendCMd(JNIEnv * env, wstring * cmd, jstring url){
	if (url != NULL){
		LPTSTR s = LPTSTR(jw::s2w(jstringToWindows(env, url)).c_str());
		if (s[0] == L'"'){// 如果url中已以双引号开头，那么这里就不再添加双引号
			cmd->append(L" ");
			cmd->append(s);
		}
		else{
			cmd->append(L" \"");
			cmd->append(s);
			cmd->append(L"\"");
		}
	}
	else{
		cmd->append(L" :");// 冒号作为特殊符号不会出现在文件路径中
	}
}
// jni方法：创建浏览器
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nCreateJWebTop
(JNIEnv * env, jclass nativeClass, jstring appfile, jlong parentWin
// 一下参数会替换appfile中的形影参数
, jstring url       // 要打开的链接地址
, jstring title     // 窗口名称
, jstring icon      // 窗口图标
, jint x, jint y    // 窗口左上角坐标,当值为-1时不启用此变量		 
, jint w, jint h    // 窗口的宽、高，当值为-1时不启用此变量		
){
	//if (appfile == NULL)return -1;
	wstringstream cmd;
	cmd << " " << parentWin;
	cmd << " " << x << " " << y << " " << w << " " << h;// xywh不可能是null，只可能是-1，所以优先传递
	wstring cmds = cmd.str();
	appendCMd(env, &cmds, appfile);
	appendCMd(env, &cmds, url);
	appendCMd(env, &cmds, icon);
	if (title != NULL){// title放最后
		cmds.append(L" ").append(jw::s2w(jstringToWindows(env, url)).c_str());
	}
	if (g_invokeByJS == NULL){// 第一次被java端调用
		env->GetJavaVM(&g_jvm);// 获取当前的虚拟机环境并保存下来
		g_nativeClass = (jclass)(env->NewGlobalRef(nativeClass));// 将一个对象设置为全局对象，此处将nativeClasss设置为全局对象
		g_invokeByJS = env->GetStaticMethodID(g_nativeClass, "invokeByJS", "(Ljava/lang/String;)Ljava/lang/String;");// 取出要调用的方法
		// 创建浏览器(创建过程中需要回调java，以便传递创建后的浏览器句柄到java端
		jw::createWin(::GetModuleHandle(NULL), LPTSTR(cmds.c_str()));// createWin会开启新的线程
		//startJWebTop(g_instance/*可以在dll attach的时候获取到*/, LPTSTR(result.ToWString().c_str()), parentWin, url_, title_, icon_, x, y, w, h);
	}
	else{	 //这样再次创建浏览器窗口不行，难道是在不同线程的原因？？用js执行RunApp反而可以
		//jw::runApp(LPTSTR(result.ToWString().c_str()), parentWin);
	}
}
// jni方法：执行脚本且不等待返回结果
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJs
(JNIEnv * env, jclass, jlong browserHWnd, jstring json){
	string js;
	// js.clear();	// 可以考虑讲js作为一个全局变量，每次使用前/后都clear一下，这样效率是否更高？
	js = "invokeByJava(";
	const char * tmp = env->GetStringUTFChars(json, false);
	js += tmp;
	env->ReleaseStringUTFChars(json, tmp);
	js += ")";
	//jw::ExecJS((HWND)browserHWnd, js);
	jw::sendProcessMsg((HWND)browserHWnd, JWM_JSON_EXECUTE_RETURN, LPTSTR(jw::s2w(js).c_str()));
}

// jni方法：设置窗口大小
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetSize
(JNIEnv * env, jclass, jlong browserHWnd, jint w, jint h){
	jw::setSize((HWND)browserHWnd, w, h);
}

// jni方法：设置窗口位置
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetLocation
(JNIEnv * env, jclass, jlong browserHWnd, jint x, jint y){
	jw::move((HWND)browserHWnd, x, y);
}

// jni方法：设置窗口位置和大小
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetBound
(JNIEnv *, jclass, jlong browserHWnd, jint x, jint y, int w, int h){
	jw::setBound((HWND)browserHWnd, x, y, w, h);
}

// jni方法：设置新的网页地址
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nSetUrl
(JNIEnv * env, jclass, jlong browserHWnd, jstring url){	
	jw::sendProcessMsg((HWND)browserHWnd, JWEBTOP_MSG_LOADURL, LPTSTR(jw::s2w(jstringToWindows(env, url)).c_str()));
	//jw::loadUrl((HWND)browserHWnd, chr2wch(jstringToWindows(env, url)));
}

// jni方法：退出JWebTop进程
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nExit
(JNIEnv *, jclass){
	jw::sendProcessMsg(g_RemoteWinHWnd, WM_COPYDATA_EXIT,L"");
}