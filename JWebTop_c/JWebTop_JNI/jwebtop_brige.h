#include "include/cef_client.h"

int startJWebTop(HINSTANCE hInstance/*当前应用的实例*/, LPTSTR lpCmdLine);

// 供浏览器回调java程序
CefString  invokeJavaMethod(CefString json);
