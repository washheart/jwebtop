#ifdef JWebTopLog
#ifndef __TESTS_H__
#define __TESTS_H__
#include <sstream>
#include <string>
using namespace std;

_declspec(dllexport) void writeLog(wstring str)	;
_declspec(dllexport) void writeLog(string s);
#endif
#endif