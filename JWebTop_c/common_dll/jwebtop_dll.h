#ifndef _Included_org_jwebtop_JWebTopdll
#define _Included_org_jwebtop_JWebTopdll
#include <string>
#include <Windows.h>

#define EXPORT __declspec(dllexport)
using namespace std;
/*
 * 用于调用本地接口（java、c#等）程序，并返回执行结果
 */
wstring invokeByBrowser(long browserHWnd, wstring json);
extern "C" {
	/*
	 * 该方法用于创建一个浏览器窗口
	 * processPath 待执行的JWebTop.exe的全路径。比如d:\c\JWebTop.exe，当然JWebTop可以命名为其他名字。
	 * configFile  此配置文件用于启动JWebTop进程（但，不立刻创建浏览器窗口，即使指定了url参数）
	 *
	 * return 内被用于通信的隐藏窗口的句柄
	 */
	EXPORT long WINAPI CreateJWebTop(wstring processPath, wstring configFile);

	/*
	 * 退出JWebTop进程
	 */
	EXPORT void WINAPI ExitJWebTop();

	/*
	* 该方法用于创建一个浏览器窗口
	* jWebTopConfigJSON 浏览器配置信息JSON(JWebTopConfigs.h)
	*
	* return 返回创建的浏览器窗口的句柄
	*/
	EXPORT long WINAPI CreateJWebTopBrowser(wstring jWebTopConfigJSON);

	/*
	 * 该方法用于关闭一个浏览器窗口
	 * browserHWnd  浏览器窗口句柄
	 */
	EXPORT void WINAPI CloseJWebTopBrowser(long browserHWnd);

	/*
	 * 下面四个方法用于执行浏览器脚本
	 */
	EXPORT LPTSTR WINAPI JWebTopExecuteJSWait(long browserHWnd, wstring script);
	EXPORT void WINAPI JWebTopExecuteJSNoWait(long browserHWnd, wstring script);
	EXPORT LPTSTR WINAPI JWebTopExecuteJSONWait(long browserHWnd, wstring json);
	EXPORT void WINAPI JWebTopExecuteJSONNoWait(long browserHWnd, wstring json);
}
#endif
