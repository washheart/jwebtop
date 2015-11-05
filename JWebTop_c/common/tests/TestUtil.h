#ifdef JWebTopLog
#ifndef __TESTS_H__
#define __TESTS_H__
#include <sstream>
#include <string>

_declspec(dllexport) void setLogFileName(std::wstring logFileName);
_declspec(dllexport) void writeLog(std::wstring str)	;
_declspec(dllexport) void writeLog(std::string s);
#endif
#endif