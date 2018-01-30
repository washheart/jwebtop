#ifndef __jw_UTIL__file_H__
#define __jw_UTIL__file_H__
#include <xstring>
#include <windows.h>

using namespace std;

namespace jw {
	namespace file {
		/**
		 * �ж��ļ����ļ����Ƿ����
		 * true=�ļ����ļ��д����ҿɷ��ʣ������������false
		 */
		_declspec(dllexport) bool isExsit(const char* fileOrDictionary);
		_declspec(dllexport) bool isExsit(const wchar_t* fileOrDictionary);

		/**
		 * ��ȡ�ļ����ȣ�����޷��ɹ����ļ�����-1
		 */
		_declspec(dllexport) __int64 getLength(const char* file);
		_declspec(dllexport) __int64 getLength(const wchar_t* file);

		_declspec(dllexport) bool delFile(const char* file);
		_declspec(dllexport) bool mkdirs(const char *path);
		_declspec(dllexport) bool createFile(const char* file);

		// ����һ������ָ�룬����listFiles�еĻص�
		typedef	void(__cdecl  FeedFile) (void* ctx/*�������*/, const wchar_t * dir/*��ǰ��ɨ���Ŀ¼*/, wchar_t * name/*��ǰɨ�赽��Ŀ¼���ļ�*/, bool isDir/*�Ƿ���Ŀ¼*/, int level/*��ԭʼĿ¼��ʼ����ǰɨ�赽�ڼ���*/);        
		/*
		 * ����dir�µ�ÿ���ļ������ҽ��ļ����͵�FeedFile�С�FeedFile�����еĵ�һ������ΪlistFiles�����е����һ������
		 */
		_declspec(dllexport) void listFiles(const wchar_t * dir/*������Ŀ¼*/, FeedFile f/*�ص�����*/, void* feedfile_ctx/*����FeedFile�ص������ĵ�һ������*/);
	}
}
#endif