#include "include/cef_client.h"

int startJWebTop(HINSTANCE hInstance/*当前应用的实例*/, LPTSTR lpCmdLine, long hwnd
	// 以下参数会替换appfile中的相应参数
	, LPTSTR url       // 要打开的链接地址
	, LPTSTR title     // 窗口名称
	, LPTSTR icon      // 窗口图标
	, int x, int y      // 窗口左上角坐标,当值为-1时不启用此变量		 
	, int w, int h      // 窗口的宽、高，当值为-1时不启用此变量	
	);

//// 供浏览器回调java程序
//CefString  invokeJavaMethod(CefString json);// 定义和实现在JWebTopJSHanlder和JJH_Windows中，通过宏“JWebTop_JNI”来控制