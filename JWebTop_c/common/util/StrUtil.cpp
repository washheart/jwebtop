#include "StrUtil.h"
#include <windows.h> 
#include <ObjBase.h>
#include <codecvt>
#include <fstream>

#define _jw_IS_SPACE(c) (0 <=c && c<=32)
using namespace std;
namespace jw {
	namespace str {
		template<class STR> inline STR& _trim(STR &s) {	// ȥ�����ҿհ��ַ��ĺ���
			if (s.empty())return s;
			STR::iterator it = s.begin();
			for (; it != s.end();) {						// ���ǰ��Ŀհ��ַ�
				if (_jw_IS_SPACE(*it)) { it++; continue; }
				break;
			}
			if (it == s.end()) {
				s.erase(s.begin(), s.end());			// ȫ�����ǿհ��ַ�
				return s;
			} else {
				s.erase(s.begin(), it);					// ����ǰ��Ŀհ��ַ�
			}
			int len = s.size() - 1;
			for (; len >= 0;) {							// ������Ŀհ��ַ�
				if (_jw_IS_SPACE(s.at(len))) { len--; continue; }
				break;
			}
			s.erase(s.begin() + len + 1, s.end());		// ��������Ŀհ��ַ�
			return s;
		}

		string&  trim(string  &s) { return _trim(s); }
		wstring& trim(wstring &s) { return _trim(s); }

		wstring&   replace_allW(wstring&   str, const   wstring&   old_value, const   wstring&   new_value) {
			while (true) {
				wstring::size_type   pos(0);
				if ((pos = str.find(old_value)) != wstring::npos)
					str.replace(pos, old_value.length(), new_value);
				else   break;
			}
			return   str;
		}
		//wstring&   replace_all(wstring&   str, const   wstring&   old_value, const   wstring&   new_value)
		//{
		//	while (true)   {
		//		string::size_type   pos(0);
		//		if ((pos = str.find(old_value)) != string::npos)
		//			str.replace(pos, old_value.length(), new_value);
		//		else   break;
		//	}
		//	return   str;
		//}
		string&   replace_all(string&   str, const   string&   old_value, const   string&   new_value) {
			while (true) {
				string::size_type   pos(0);
				if ((pos = str.find(old_value)) != string::npos)
					str.replace(pos, old_value.length(), new_value);
				else   break;
			}
			return   str;
		}
		//wstring&   replace_all_distinctW(wstring&   str,const   wstring&   old_value,const   wstring&   new_value)
		//{
		//for(wstring::size_type   pos(0);   pos!=wstring::npos;   pos+=new_value.length())   {
		//if(   (pos=str.find(old_value,pos))!=wstring::npos   )
		//str.replace(pos,old_value.length(),new_value);
		//else   break;
		//}
		//return   str;
		//}
		//string&   replace_all_distinct(string&   str, const   string&   old_value, const   string&   new_value)
		//{
		//	for (string::size_type pos(0); pos != string::npos; pos += new_value.length())   {
		//		if ((pos = str.find(old_value, pos)) != string::npos)
		//			str.replace(pos, old_value.length(), new_value);
		//		else   break;
		//	}
		//	return   str;
		//}

		bool startswith(const std::wstring& str, const std::wstring& start) {
			unsigned int startlen = start.size();
			return (str.size() >= startlen) && (str.compare(0, startlen, start) == 0);
		}
		bool startswith(const std::string& str, const std::string& start) {
			unsigned int startlen = start.size();
			return (str.size() >= startlen) && str.compare(0, startlen, start) == 0;
		}
		bool endswith(const std::wstring& str, const std::wstring& end) {
			unsigned int endlen = end.size();
			unsigned int srclen = str.size();
			return (srclen >= endlen) && str.compare(srclen - endlen, endlen, end) == 0;
		}
		bool endswith(const std::string& str, const std::string& end) {
			unsigned int endlen = end.size();
			unsigned int srclen = str.size();
			return (srclen >= endlen) && str.compare(srclen - endlen, endlen, end) == 0;
		}

		//bool isGB(const char*gb, int len){
		//	for (int i = 0; i < len; ++i){
		//		if (gb[i] < 0){
		//			return true;
		//		}
		//	}
		//	return false;
		//}
		//int IsTextUTF8(const char* str, long length)
		//{
		//	int i;
		//	long nBytes = 0;//UFT8����1-6���ֽڱ���,ASCII��һ���ֽ�
		//	unsigned char chr;
		//	bool bAllAscii = true; //���ȫ������ASCII, ˵������UTF-8
		//	for (i = 0; i < length; i++)
		//	{
		//		chr = *(str + i);
		//		if ((chr & 0x80) != 0) // �ж��Ƿ�ASCII����,�������,˵���п�����UTF-8,ASCII��7λ����,����һ���ֽڴ�,���λ���Ϊ0,o0xxxxxxx
		//			bAllAscii = false;
		//		if (nBytes == 0) //�������ASCII��,Ӧ���Ƕ��ֽڷ�,�����ֽ���
		//		{
		//			if (chr >= 0x80)
		//			{
		//				if (chr >= 0xFC && chr <= 0xFD)
		//					nBytes = 6;
		//				else if (chr >= 0xF8)
		//					nBytes = 5;
		//				else if (chr >= 0xF0)
		//					nBytes = 4;
		//				else if (chr >= 0xE0)
		//					nBytes = 3;
		//				else if (chr >= 0xC0)
		//					nBytes = 2;
		//				else
		//				{
		//					return false;
		//				}
		//				nBytes--;
		//			}
		//		}
		//		else //���ֽڷ��ķ����ֽ�,ӦΪ 10xxxxxx
		//		{
		//			if ((chr & 0xC0) != 0x80)
		//			{
		//				return false;
		//			}
		//			nBytes--;
		//		}
		//	}
		//	if (nBytes > 0) //Υ������
		//	{
		//		return false;
		//	}
		//	if (bAllAscii) //���ȫ������ASCII, ˵������UTF-8
		//	{
		//		return false;
		//	}
		//	return true;
		//}
		//char* U2G(const char* utf8)
		//{
		//	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
		//	wchar_t* wstr = new wchar_t[len + 1];
		//	memset(wstr, 0, len + 1);
		//	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
		//	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
		//	char* str = new char[len + 1];
		//	memset(str, 0, len + 1);
		//	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
		//	if (wstr) delete[] wstr;
		//	return str;
		//}
		////GB2312��UTF-8��ת��
		//char* G2U(const char* gb2312)
		//{
		//	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
		//	wchar_t* wstr = new wchar_t[len + 1];
		//	memset(wstr, 0, len + 1);
		//	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
		//	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
		//	char* str = new char[len + 1];
		//	memset(str, 0, len + 1);
		//	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
		//	if (wstr) delete[] wstr;
		//	return str;
		//}
		////GB2312��UTF-8��ת��
		//char* B2U(const char* big5)
		//{
		//	int len = MultiByteToWideChar(950, 0, big5, -1, NULL, 0);
		//	wchar_t* wstr = new wchar_t[len + 1];
		//	memset(wstr, 0, len + 1);
		//	MultiByteToWideChar(950, 0, big5, -1, wstr, len);
		//	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
		//	char* str = new char[len + 1];
		//	memset(str, 0, len + 1);
		//	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
		//	if (wstr) delete[] wstr;
		//	return str;
		//}
		// ��wstringת��Ϊstring(ת������UTF-8��
		string w2s(const wstring &ws) {
			std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
			std::u16string u16_cvt_str(ws.begin(), ws.end());
			std::string u8_cvt_str = cvt.to_bytes(u16_cvt_str);
			return u8_cvt_str;
		}
		wstring s2w(const string &s) {
			std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
			std::u16string u16_cvt_str = cvt.from_bytes(s);
			return wstring(u16_cvt_str.begin(), u16_cvt_str.end());
		}
		//// ��wstringת��Ϊstring(ת������GBK��
		//string w2s_gbk(const wstring &ws){
		//	const wchar_t * wp = ws.c_str();
		//	size_t wlen = wcslen(wp);
		//	size_t len = WideCharToMultiByte(CP_ACP, 0, wp, wlen, NULL, 0, NULL, NULL);
		//	char *m_char = new char[len + 1];
		//	memset(m_char, 0, len + 1);// ��ʼ��һ���ڴ�飨�˴������ַ������ַ�Ϊ0��
		//	WideCharToMultiByte(CP_ACP, 0, wp, wlen, m_char, len, NULL, NULL);
		//	string rtn(m_char);
		//	delete[] m_char;// ɾ��������Ŀռ䣨wpָ����ڴ�ռ���ws��ͬ������ɾ����
		//	return rtn;
		//}
		//wstring s2w(const string &s){
		//	const char* tmp = s.c_str();
		//	size_t len = strlen(tmp);
		//	size_t wlen = MultiByteToWideChar(CP_ACP, 0, (const char*)tmp, int(len), NULL, 0);
		//	wchar_t*  buffer = new wchar_t[wlen];
		//	memset(buffer, 0, wlen + 1);// ��ʼ��һ���ڴ�飨�˴������ַ������ַ�Ϊ0��
		//	MultiByteToWideChar(CP_ACP, 0, (const char*)tmp, int(len), buffer, int(wlen));
		//	wstring ws(buffer);
		//	delete[] buffer;
		//	return buffer;
		//}


		wstring URLEncode(const wstring &s) {
			int size = s.size();
			wchar_t * to = NULL;
			LPTSTR from = LPTSTR(s.c_str());
			to = (wchar_t *)malloc(sizeof(wchar_t)*(size * 3 + 1));
			int offset = 0;
			int i = 0;
			for (; i < size; i++) {
				if (from[i] == L'+') {			// 1. + URL �� + �ű�ʾ�ո� %2B
					to[i + offset++] = L'%'; to[i + offset++] = L'2'; to[i + offset] = L'B';
				} else if (from[i] == L' ') {	// 2. �ո� URL�еĿո������ + �Ż��߱��� %20
					to[i + offset++] = L'%'; to[i + offset++] = L'2'; to[i + offset] = L'0';
				} else if (from[i] == L'/') {	//	3. / �ָ�Ŀ¼����Ŀ¼ %2F
					to[i + offset++] = L'%'; to[i + offset++] = L'2'; to[i + offset] = L'F';
				} else if (from[i] == L'?') {	//	4. ? �ָ�ʵ�ʵ� URL �Ͳ��� %3F
					to[i + offset++] = L'%'; to[i + offset++] = L'3'; to[i + offset] = L'F';
				} else if (from[i] == L'%') {	//	5. % ָ�������ַ� %25
					to[i + offset++] = L'%'; to[i + offset++] = L'2'; to[i + offset] = L'5';
				} else if (from[i] == L'#') {	//	6. # ��ʾ��ǩ %23
					to[i + offset++] = L'%'; to[i + offset++] = L'2'; to[i + offset] = L'3';
				} else if (from[i] == L'&') {	//	7. & URL ��ָ���Ĳ�����ķָ��� %26
					to[i + offset++] = L'%'; to[i + offset++] = L'2'; to[i + offset] = L'6';
				} else if (from[i] == L'=') {	//	8. = URL ��ָ��������ֵ %3D
					to[i + offset++] = L'%'; to[i + offset++] = L'3'; to[i + offset] = L'D';
				} else {
					to[i + offset] = from[i];
				}
			}
			to[i + offset] = L'\0';
			wstring rtn = wstring(to);
			return rtn;
		}

		char* GenerateGuid() {
			char* guidBuf;
			guidBuf = (char *)malloc(64);
			GUID guid;
			CoInitialize(NULL);
			if (S_OK == CoCreateGuid(&guid)) {
				//_snprintf(guidBuf, 64, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
				_snprintf(guidBuf, 64, "%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
					guid.Data1, guid.Data2, guid.Data3,
					guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
					guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
			}
			CoUninitialize();
			return guidBuf;
		}

		string GenerateGuidA() {
			char* guidBuf = GenerateGuid();
			string guid(guidBuf);
			delete guidBuf;
			return guid;
		}

		wstring GenerateGuidW() {
			char* guidBuf = GenerateGuid();
			string tmp(guidBuf);
			wstring guid = s2w(tmp);
			delete guidBuf;
			return guid;
		}

		string getDateTime() {
			time_t t = time(0);
			char tmp2[20];// ��9999-12-31������¶���19���ַ�������20��������Ϳ����ˡ�
			strftime(tmp2, sizeof(tmp2), "%Y_%m_%d %X", localtime(&t));
			return string(tmp2);
		}
		long parseLong64(const wstring &ws) {
			return _wtoi64(LPTSTR(ws.c_str()));
		}
		long parseLong(const wstring &ws) {
			return _wtol(LPTSTR(ws.c_str()));
		}
		int parseInt(const wstring &ws) {
			return _wtoi(LPTSTR(ws.c_str()));
		}

		int readfile(wstring file, string &result) {
			ifstream myfile(file);
			if (!myfile)return 0;
			result.append((std::istreambuf_iterator<char>(myfile)), std::istreambuf_iterator<char>());
			myfile.close();
			return 1;
		}
	}
}