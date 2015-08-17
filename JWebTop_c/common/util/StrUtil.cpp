#include "StrUtil.h"
#include <windows.h> 
#include <time.h>
#include <ObjBase.h>

using namespace std;

wstring&   replace_allW(wstring&   str,const   wstring&   old_value,const   wstring&   new_value)   
{   
	while(true)   {   
		wstring::size_type   pos(0);   
		if(   (pos=str.find(old_value))!=wstring::npos   )   
			str.replace(pos,old_value.length(),new_value);   
		else   break;   
	}   
	return   str;   
}
wstring&   replace_all(wstring&   str, const   wstring&   old_value, const   wstring&   new_value)
{
	while (true)   {
		string::size_type   pos(0);
		if ((pos = str.find(old_value)) != string::npos)
			str.replace(pos, old_value.length(), new_value);
		else   break;
	}
	return   str;
}
string&   replace_all(string&   str,const   string&   old_value,const   string&   new_value)   
{   
	while(true)   {   
		string::size_type   pos(0);   
		if(   (pos=str.find(old_value))!=string::npos   )   
			str.replace(pos,old_value.length(),new_value);   
		else   break;   
	}   
	return   str;   
}   
/*wstring&   replace_all_distinctW(wstring&   str,const   wstring&   old_value,const   wstring&   new_value)   
{   
for(wstring::size_type   pos(0);   pos!=wstring::npos;   pos+=new_value.length())   {   
if(   (pos=str.find(old_value,pos))!=wstring::npos   )   
str.replace(pos,old_value.length(),new_value);   
else   break;   
}   
return   str;   
}*/
string&   replace_all_distinct(string&   str,const   string&   old_value,const   string&   new_value)   
{   
	for(string::size_type   pos(0);   pos!=string::npos;   pos+=new_value.length())   {   
		if(   (pos=str.find(old_value,pos))!=string::npos   )   
			str.replace(pos,old_value.length(),new_value);   
		else   break;   
	}   
	return   str;   
}

bool isGB(const char*gb,int len){
	for(int i=0;i<len;++i){
		if(gb[i]<0){
			return true;
		}
	}
	return false;
}

int IsTextUTF8(const char* str,long length)
{
	int i;
	long nBytes=0;//UFT8可用1-6个字节编码,ASCII用一个字节
	unsigned char chr;
	bool bAllAscii=true; //如果全部都是ASCII, 说明不是UTF-8
	for(i=0;i<length;i++)
	{
		chr= *(str+i);
		if( (chr&0x80) != 0 ) // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
			bAllAscii= false;
		if(nBytes==0) //如果不是ASCII码,应该是多字节符,计算字节数
		{
			if(chr>=0x80)
			{
				if(chr>=0xFC&&chr<=0xFD)
					nBytes=6;
				else if(chr>=0xF8)
					nBytes=5;
				else if(chr>=0xF0)
					nBytes=4;
				else if(chr>=0xE0)
					nBytes=3;
				else if(chr>=0xC0)
					nBytes=2;
				else
				{
					return false;
				}
				nBytes--;
			}
		}
		else //多字节符的非首字节,应为 10xxxxxx
		{
			if( (chr&0xC0) != 0x80 )
			{
				return false;
			}
			nBytes--;
		}
	}
	if( nBytes > 0 ) //违返规则
	{
		return false;
	}
	if( bAllAscii ) //如果全部都是ASCII, 说明不是UTF-8
	{
		return false;
	}
	return true;
}
char* U2G(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len+1];
	memset(wstr, 0, len+1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len+1];
	memset(str, 0, len+1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if(wstr) delete[] wstr;
	return str;
}
//GB2312到UTF-8的转换
char* G2U(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len+1];
	memset(wstr, 0, len+1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len+1];
	memset(str, 0, len+1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if(wstr) delete[] wstr;
	return str;
}
//GB2312到UTF-8的转换
char* B2U(const char* big5)
{
	int len = MultiByteToWideChar(950, 0, big5, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len+1];
	memset(wstr, 0, len+1);
	MultiByteToWideChar(950, 0, big5, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len+1];
	memset(str, 0, len+1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if(wstr) delete[] wstr;
	return str;
}


// 将char转换为wchar_t
wchar_t* chr2wch(const char* tmp)
{
	size_t len = strlen(tmp);
	size_t wlen = MultiByteToWideChar(CP_ACP, 0, (const char*)tmp, int(len), NULL, 0);
	wchar_t*  buffer = new wchar_t[wlen];
	MultiByteToWideChar(CP_ACP, 0, (const char*)tmp, int(len), buffer, int(wlen));
	buffer[wlen] = 0;
	return buffer;
}
// 将char转换为wchar_t
wchar_t* chr2wch(char* tmp)
{
	size_t len = strlen(tmp);
	size_t wlen = MultiByteToWideChar(CP_ACP, 0, (const char*)tmp, int(len), NULL, 0);
	wchar_t*  buffer = new wchar_t[wlen];
	MultiByteToWideChar(CP_ACP, 0, (const char*)tmp, int(len), buffer, int(wlen));
	buffer[wlen] = 0;
	return buffer;
}

// 将wchar_t转换为char
char* wch2chr(const wchar_t* wp)
{
	size_t wlen = wcslen(wp);
	size_t len = WideCharToMultiByte(CP_ACP, 0, wp, wlen, NULL, 0, NULL, NULL);
	char *m_char = new char[len];
	WideCharToMultiByte(CP_ACP, 0, wp, wlen, m_char, len, NULL, NULL);
	m_char[len] = 0;
	return m_char;
}
// 将wchar_t转换为char
char* wch2chr(wchar_t* wp)
{
	size_t wlen = wcslen(wp);
	size_t len = WideCharToMultiByte(CP_ACP, 0, wp, wlen, NULL, 0, NULL, NULL);
	char *m_char = new char[len];
	WideCharToMultiByte(CP_ACP, 0, wp, wlen, m_char, len, NULL, NULL);
	m_char[len] = 0;
	return m_char;
}


char* GenerateGuid()
{
	char* guidBuf;
	guidBuf = (char *)malloc(64);
	GUID guid;
	CoInitialize(NULL);
	if (S_OK == CoCreateGuid(&guid))
	{
		_snprintf(guidBuf, 64, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	}
	CoUninitialize();
	return guidBuf;
}

string GenerateGuidA()
{
	char* guidBuf = GenerateGuid();
	string guid(guidBuf);
	delete guidBuf;
	return guid;
}

wstring GenerateGuidW(){
	char* guidBuf = GenerateGuid();
	wstring guid(chr2wch(guidBuf));
	delete guidBuf;
	return guid;
}

string getDateTime(){
	time_t t = time(0);
	char tmp2[20];// 到9999-12-31的情况下都是19的字符，所以20长的数组就可以了。
	strftime(tmp2, sizeof(tmp2), "%Y_%m_%d %X", localtime(&t));
	return string(tmp2);
}