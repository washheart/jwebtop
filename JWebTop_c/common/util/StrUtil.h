#ifndef __MYUTIL__str_H__
#define __MYUTIL__str_H__
#include <xstring>
#include <windows.h>
using namespace std;

namespace jw {
	namespace str {
		_declspec(dllexport) string& trim(string &s);
		_declspec(dllexport) wstring& trim(wstring &s);

		_declspec(dllexport) wstring&   replace_allW(wstring& str, const wstring& old_value, const wstring& new_value);
		_declspec(dllexport) string&    replace_all(string&  str, const string&  old_value, const string&  new_value);

		_declspec(dllexport) bool startswith(const std::wstring& str, const std::wstring& start);
		_declspec(dllexport) bool startswith(const std::string& str, const std::string& start);
		_declspec(dllexport)bool endswith(const std::wstring& str, const std::wstring& end);
		_declspec(dllexport)bool endswith(const std::string& str, const std::string& end);

		//string&   replace_all_distinct(string&   str, const   string&   old_value, const   string&   new_value);

		//wstring&   replace_all(wstring&   str, const   wstring&   old_value, const   wstring&   new_value);
		//bool isGB(const char*gb, int len);
		//int IsTextUTF8(const char* str, long length);
		//char* U2G(const char* utf8);
		//char* G2U(const char* gb2312);
		//char* B2U(const char* big5);

		// ������ĸ������ᵼ���ڴ�й©��Ӧ�ð�xxx(yyy * from,zzz * to)�ķ�ʽ��д����������������ȡֵ�������ڴ�й©
		//// ��charת��Ϊwchar_t
		//wchar_t* chr2wch(const char* tmp);
		//// ��charת��Ϊwchar_t
		//wchar_t* chr2wch(char* tmp);
		//// ��wchar_tת��Ϊchar
		//char* wch2chr(const wchar_t* wp);
		//// ��wchar_tת��Ϊchar
		//char* wch2chr(wchar_t* wp);

		_declspec(dllexport) string w2s(const wstring &ws);
		_declspec(dllexport) wstring s2w(const string &s);

		_declspec(dllexport) wstring URLEncode(const wstring &s);

		// ����Ψһ���ַ���
		_declspec(dllexport) char* GenerateGuid();
		_declspec(dllexport) std::string GenerateGuidA();
		_declspec(dllexport) std::wstring GenerateGuidW();

		// ��ȡ��ǰ�����ں�ʱ�䣬��ʽ��yyyy-mm-dd hh:MM:ss
		_declspec(dllexport) std::string getDateTime();

		_declspec(dllexport) long parseLong64(const wstring &ws);
		_declspec(dllexport) long parseLong(const wstring &ws);
		_declspec(dllexport) int parseInt(const wstring &ws);

		// ��ȡ�ļ����ݵ�result�У�append��ʽ������ȡ�ɹ�����1��ʧ�ܷ���0
		_declspec(dllexport) int readfile(wstring file, string &result);
	}
}
#endif