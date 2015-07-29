#include "include/cef_client.h"

int startJWebTop(HINSTANCE hInstance/*当前应用的实例*/, LPTSTR lpCmdLine);

//// 供浏览器回调java程序
//CefString  invokeJavaMethod(CefString json);// 定义和实现在JWebTopJSHanlder和JJH_Windows中，通过宏“JWebTop_JNI”来控制