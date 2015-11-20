#include "DLLExState.h"

#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif 

namespace jw{
	namespace dllex{// 和DLL进行交互的相关扩展方法
		wstring DLLExState::getAppendJS(){
			return this->appendJS;
		}
		void DLLExState::setAppendJS(wstring js){
			this->appendJS = js;// 记录下浏览器句柄与附加JS的关联			
		}		

		// 静态变量与方法
		DLLExState::DefBrowserSateMap DLLExState::browserStateMap;			// 保存浏览器扩展状态（静态变量）

		DLLExState * DLLExState::findWithoutCreate(HWND browserHWnd){
			DLLExState * rtn = NULL;
			DefBrowserSateMap::iterator it = browserStateMap.find(browserHWnd);
			if (browserStateMap.end() != it) {
				rtn = it->second;
			}
			return rtn;
		}

		DLLExState * DLLExState::findOrCreateExState(HWND browserHWnd){
			DLLExState * rtn = NULL;
			DefBrowserSateMap::iterator it = browserStateMap.find(browserHWnd);
			if (browserStateMap.end() != it) {
				rtn = it->second;
			}
			else{
				rtn = new DLLExState();
				browserStateMap.insert(pair<HWND, DLLExState* >(browserHWnd, rtn));// 记录下浏览器句柄与扩展状态的关联
			}
			return rtn;
		}

		void DLLExState::unlockBrowserLocks(HWND browserHwnd){
			DLLExState *  exState = findWithoutCreate(browserHwnd);
			if (exState == NULL)return;
		}
		void DLLExState::removeBrowserSetting(HWND browserHwnd){
			DLLExState *  exState = findWithoutCreate(browserHwnd);
			if (exState == NULL)return;
			unlockBrowserLocks(browserHwnd);
			browserStateMap.erase(browserHwnd);
		}

		void DLLExState::appendBrowserJS(HWND browserHwnd, const CefRefPtr<CefFrame> frame){
			DLLExState *  exState = findWithoutCreate(browserHwnd);
			if (exState == NULL)return;
			wstring js = exState->getAppendJS();
#ifdef JWebTopLog
			writeLog(L"附加的JS====="); writeLog(js); writeLog(L"\r\n");
#endif
			if (!js.empty())frame->ExecuteJavaScript(js, "", 0);
		}
	}// End dllex namespace
}// End jw namespace