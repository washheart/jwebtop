#ifdef JWebTopLog
#include "TestUtil.h"
#include <Windows.h>
#include <string>
#include "common/util/StrUtil.h"

HANDLE hFile;// 调试时用于输出日志文件
DWORD filePos = 0;// 调试时用于输出日志文件
LPTSTR LOG_FILE_NAME ;

void setLogFileName(std::wstring logFileName){
	LOG_FILE_NAME = LPTSTR(logFileName.c_str());
}

void checFile(){
	if (hFile == NULL){
		if (LOG_FILE_NAME == NULL) {
			wstringstream wss; wss << L"jwebtop" <<  GetCurrentProcessId()<<".log";
			LOG_FILE_NAME = LPTSTR(wss.str().c_str());
		}
		hFile = CreateFile(LOG_FILE_NAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
	}
}
void writeLog(std::wstring str){
	checFile();
	std::string s = jw::str::w2s(str);
	WriteFile(hFile, s.c_str(), s.size(), &filePos, NULL);
	FlushFileBuffers(hFile);
}
void writeLog(std::string s){
	checFile();
	WriteFile(hFile, s.c_str(), s.size(), &filePos, NULL);
	FlushFileBuffers(hFile);
}
#endif