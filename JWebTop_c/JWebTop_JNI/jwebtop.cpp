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

JavaVM* g_jvm;               // 保存全局的虚拟机环境
jclass g_nativeClass;        // 记录全局的本地类变量
jmethodID g_invokeByJS;      // 从C端回调Java的方法
extern HWND g_RemoteWinHWnd;
wstring processPath;
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
	CefString result(env->GetStringUTFChars(str, false));
	env->DeleteLocalRef(str);
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
	wstringstream cmd;
	cmd << L": ";// 标记为特殊命令
	cmd << (long)hWnd << " ";// 追加msgWin的HWND
	cmd << szCmdLine;// 其他命令行参数
	createSubProcess(LPTSTR(processPath.c_str()), LPTSTR(cmd.str().c_str()));
}
void appendCMd(JNIEnv * env, wstring * cmd, const jstring &url){
	if (url != NULL){
		wstring s = jstring2wstring(env, url);
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
void thread_CreateJWebTop(std::wstring cmds)
{
	jw::createWin(::GetModuleHandle(NULL), LPTSTR(cmds.c_str()));// createWin会开启新的线程
}
// jni方法：创建JWebTop进程
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nCreateJWebTop
(JNIEnv * env, jclass nativeClass, jstring jprocessPath, jstring configFile){
	if (g_RemoteWinHWnd != NULL)return /*(jlong)g_RemoteWinHWnd*/;
	if (g_invokeByJS == NULL){// 第一次被java端调用
		env->GetJavaVM(&g_jvm);// 获取当前的虚拟机环境并保存下来		
		g_nativeClass = (jclass)(env->NewGlobalRef(nativeClass));// 将一个对象设置为全局对象，此处将nativeClasss设置为全局对象
		g_invokeByJS = env->GetStaticMethodID(g_nativeClass, "invokeByJS", "(Ljava/lang/String;)Ljava/lang/String;");// 取出要调用的方法
	}
	processPath = jstring2wstring(env, jprocessPath);
	wstring taskId = jw::task::createTaskId();			         // 生成任务id	
	wstring cmds(taskId);                                        // 将任务id放到启动参数上 
	wstring cfgFile = jstring2wstring(env, configFile);
	if (cfgFile[0] != L'\"')cfgFile = L"\"" + cfgFile + L"\"";   // 用双引号将路径包含起来
	cmds.append(L" ").append(cfgFile);	                         // 其他任务参数：configFile
	jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // 放置任务到任务池
	thread t(thread_CreateJWebTop, cmds); t.detach();			 // 在新线程中完成启动操作
	lock->wait();		             		 		             // 等待任务完成
	wstring result = lock->result;   		 		             // 取回执行结果
	long tmp = jw::parseLong(result);
	if (tmp != 0) g_RemoteWinHWnd = (HWND)tmp;
	//return tmp;
}

/*
* 对应org.jwebtop.JWebTopNative类的nCreateBrowser方法
* 该方法用于创建一个浏览器窗口
* appFile    浏览器根据此配置文件进行初始化
* parentWin  创建的浏览器的父窗口是哪个
*/
JNIEXPORT jlong JNICALL Java_org_jwebtop_JWebTopNative_nCreateBrowser
(JNIEnv * env, jclass, jstring appFile, jlong parentWin
// 以下参数会替换appfile中的相应参数
, jstring url       // 要打开的链接地址
, jstring title     // 窗口名称
, jstring icon      // 窗口图标
, jint x, jint y    // 窗口左上角坐标,当值为-1时不启用此变量		 
, jint w, jint h    // 窗口的宽、高，当值为-1时不启用此变量		
){
	wstringstream cmd;
	cmd << parentWin;
	cmd << " " << x << " " << y << " " << w << " " << h;// xywh不可能是null，只可能是-1，所以优先传递
	wstring cmds = cmd.str();
	appendCMd(env, &cmds, appFile);
	appendCMd(env, &cmds, url);
	appendCMd(env, &cmds, icon);
	if (title != NULL){// title放最后
		cmds.append(L" ").append(jstring2wstring(env, title).c_str());
	}
	wstring taskId = jw::task::createTaskId();			         // 生成任务id
	jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // 放置任务到任务池
	wstring wrapped = jw::wrapAsTaskJSON((long)g_RemoteWinHWnd, std::ref(taskId), std::ref(cmds));
	jw::sendProcessMsg(g_RemoteWinHWnd, JWM_CREATEBROWSER, LPTSTR(wrapped.c_str()));
	lock->wait();		             		 		             // 等待任务完成
	wstring result = lock->result;   		 		             // 取回执行结果
	return jw::parseLong(result);
}

/*
 * 对应org.jwebtop.JWebTopNative类的nCloseBrowser方法
 * 该方法用于关闭一个浏览器窗口
 * browserHWnd  浏览器窗口句柄
 */
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nCloseBrowser
(JNIEnv * env, jclass, jlong browserHWnd){
	wstringstream wss; wss << browserHWnd;
	jw::sendProcessMsg((HWND)browserHWnd, JWM_CLOSEBROWSER, LPTSTR(wss.str().c_str()));
}

jstring exeRemoteAndWait(JNIEnv * env, jlong browserHWnd, string msg, DWORD msgId){
	wstring taskId = jw::task::createTaskId();			         // 生成任务id
	// taskId附加到json字符串上
	wstring newmsg = jw::s2w(msg);
	wstring wrapped = jw::wrapAsTaskJSON((long)browserHWnd, std::ref(taskId), std::ref(newmsg));
	jw::task::ProcessMsgLock * lock = jw::task::addTask(taskId); // 放置任务到任务池
	if (jw::sendProcessMsg((HWND)browserHWnd, msgId, LPTSTR(wrapped.c_str()))){ // 发送任务到远程进程
		lock->wait();		             		 		             // 等待任务完成
		wstring result = lock->result;   		 		             // 取回执行结果
		jstring sss = env->NewStringUTF(CefString(result).ToString().c_str());
		return sss;									 // 返回数据
	}
	else{
		jw::task::removeTask(taskId);								// 消息发送失败移除现有消息
		return NULL;											// 返回数据：注意这里是空字符串
	}
}
JNIEXPORT jstring JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJSWait
(JNIEnv * env, jclass, jlong browserHWnd, jstring script){
	return exeRemoteAndWait(env, browserHWnd, jstring2string(env,script), JWM_JS_EXECUTE_WAIT);
}

JNIEXPORT jstring JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJSONWait
(JNIEnv * env, jclass, jlong browserHWnd, jstring json){
	return exeRemoteAndWait(env, browserHWnd, jstring2string(env, json), JWM_JSON_EXECUTE_WAIT);
}

JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJSNoWait
(JNIEnv * env, jclass, jlong browserHWnd, jstring script){
	jw::sendProcessMsg((HWND)browserHWnd, JWM_JS_EXECUTE_RETURN, LPTSTR(jstring2wstring(env, script).c_str()));
}

// jni方法：执行脚本且不等待返回结果
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nExecuteJSONNoWait
(JNIEnv * env, jclass, jlong browserHWnd, jstring json){
	jw::sendProcessMsg((HWND)browserHWnd, JWM_JSON_EXECUTE_RETURN, LPTSTR(jstring2wstring(env, json).c_str()));
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

// jni方法：退出JWebTop进程
JNIEXPORT void JNICALL Java_org_jwebtop_JWebTopNative_nExit
(JNIEnv *, jclass){
	jw::sendProcessMsg(g_RemoteWinHWnd, WM_COPYDATA_EXIT,L"");
}

JNIEXPORT jintArray Java_org_jwebtop_JWebTopNative_nGetWindowClient(JNIEnv * env, jclass, jlong hWnd){
	WINDOWINFO winInfo;
	GetWindowInfo((HWND)hWnd, &winInfo);// 获取窗口信息
	RECT rc = winInfo.rcClient;
	jintArray rtn= env->NewIntArray(4);
	jint * tmp = new jint[4];
	tmp[0] = rc.left;
	tmp[1] = rc.top;
	tmp[2] = rc.right;
	tmp[3] = rc.bottom;
	delete[] tmp;
	env->SetIntArrayRegion(rtn, 0, 4, tmp);
	return rtn;
}