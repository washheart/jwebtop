#ifndef CEF_JWEBTOP_EXE_DLLEX_STATE_H_
#define CEF_JWEBTOP_EXE_DLLEX_STATE_H_
#include <Windows.h>
#include <string>
#include <list>
#include <map>
#include "include/cef_browser.h"

using namespace std;
namespace jw{
	namespace dllex{// 和DLL进行交互的相关扩展方法
		class DLLExState{
		private:
			typedef map<HWND, DLLExState * > DefBrowserSateMap;			// 定义一个存储浏览器扩展状态的Map类
			static DefBrowserSateMap browserStateMap;					// 保存浏览器扩展状态（静态变量）

			wstring appendJS;											// 浏览器需要附加的JS
			static DLLExState * findWithoutCreate(HWND browserHWnd);	// 查找浏览器状态，如果找不到返回null
		public:

			wstring getAppendJS();										// 得到当前浏览器需要附加的JS	
			void setAppendJS(wstring js);								// 设置当前浏览器需要附加的JS

			static void unlockBrowserLocks(HWND browserHWnd);			// 解锁当前所有的任务锁
			static DLLExState * findOrCreateExState(HWND browserHWnd);	// 查找当前浏览器扩展状态，如果找不到会自动创建（一般切换网页时调用）
			static void removeBrowserSetting(HWND browserHwnd);			// 清理保存的浏览器扩展状态（一般关闭时调用），此方法会先调用unlockBrowserLocks解锁然后再清除
			static void appendBrowserJS(HWND browserHwnd, const CefRefPtr<CefFrame> frame);// 每次页面OnLoad后附加一些JS
		};// End DLLExState class
	}// End dllex namespace
}// End jw namespace
#endif