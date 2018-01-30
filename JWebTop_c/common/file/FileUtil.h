#ifndef __jw_UTIL__file_H__
#define __jw_UTIL__file_H__
#include <xstring>
#include <windows.h>

using namespace std;

namespace jw {
	namespace file {
		/**
		 * 判断文件或文件夹是否存在
		 * true=文件或文件夹存在且可访问，其他情况返回false
		 */
		_declspec(dllexport) bool isExsit(const char* fileOrDictionary);
		_declspec(dllexport) bool isExsit(const wchar_t* fileOrDictionary);

		/**
		 * 获取文件长度，如果无法成功打开文件返回-1
		 */
		_declspec(dllexport) __int64 getLength(const char* file);
		_declspec(dllexport) __int64 getLength(const wchar_t* file);

		_declspec(dllexport) bool delFile(const char* file);
		_declspec(dllexport) bool mkdirs(const char *path);
		_declspec(dllexport) bool createFile(const char* file);

		// 定义一个函数指针，用于listFiles中的回调
		typedef	void(__cdecl  FeedFile) (void* ctx/*传入参数*/, const wchar_t * dir/*当前在扫描的目录*/, wchar_t * name/*当前扫描到的目录或文件*/, bool isDir/*是否是目录*/, int level/*从原始目录开始，当前扫描到第几层*/);        
		/*
		 * 遍历dir下的每个文件，并且将文件传送到FeedFile中。FeedFile函数中的第一个参数为listFiles函数中的最后一个参数
		 */
		_declspec(dllexport) void listFiles(const wchar_t * dir/*待检测的目录*/, FeedFile f/*回调函数*/, void* feedfile_ctx/*用作FeedFile回调函数的第一个参数*/);
	}
}
#endif