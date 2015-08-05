#ifdef JWebTopLog
#include "TestUtil.h"
#include <Windows.h>
#include <string>
#include "JWebTop/util/StrUtil.h"
using namespace std;

HANDLE hFile;// 调试时用于输出日志文件
DWORD filePos = 0;// 调试时用于输出日志文件
void checFile(){
	if (hFile == NULL){
		hFile = CreateFile(L"jwebtop.log", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
	}
}
void writeLog(wstring str){
	checFile();
	std::string s = wch2chr(str.c_str());
	WriteFile(hFile, s.c_str(), s.size(), &filePos, NULL);
	FlushFileBuffers(hFile);
}
void writeLog(string s){
	checFile();
	WriteFile(hFile, s.c_str(), s.size(), &filePos, NULL);
	FlushFileBuffers(hFile);
}
#endif