#ifndef CEF_JWEBTOP_JJH_Files_H_
#define CEF_JWEBTOP_JJH_Files_H_

#include <iostream>  
#include <fstream>  
#include <sstream>
#include <list>
#include <string>
#include "include/cef_urlrequest.h"
#include "include/cef_app.h" 
#include "JWebTopJSHanlder.h"
#include "common/winctrl/JWebTopWinCtrl.h"
#include "JWebTop/dllex/JWebTop_DLLEx.h"
#include "JWebTop/wndproc/JWebTopWndProc.h"

#include <Shellapi.h>
#include <oleidl.h>
#include "common/tests/TestUtil.h"


//�򿪱����ļ��Ի���  
#include<Commdlg.h>  
#include "common/util/StrUtil.h"
#include "common/file/FileUtil.h"
//ѡ���ļ��жԻ���  
#include<Shlobj.h>  
//#pragma comment(lib,"Shell32.lib")  
// JJH=JWebTop JavaScriptHandler


/*
 JJH_SelectFile({params}, [handler]);//�߼���������ѡ���ļ��Ի���
 �÷���JWebTop.selectFile({params}, [handler])
 params={
 mode:	0=ѡ���ļ��Ի���Ĭ�ϣ���1=ѡ���ļ��жԻ���2=�����ļ��Ի���
 title:	ѡ���ļ�����ʾ��Ϣ������Ϊ��
 dir:	�Ի����ʼ��ʱ��Ŀ¼������Ϊ��
 file:	�Ի����ʼ��ʱ���ļ�������Ϊ��
 filters:����������һ��JSON����[���ơ��������ơ�����... ...]�����磺['�����ļ�','*.*','C/C++ Flie','*.cpp;*.c;*.h']
 }
 ����ֵ���ַ���
 ���ַ���Ϊnullʱ��ʾ�û�ִ�е���ȡ������
 mode=0�������Ի��з��ָ��Ķ���ļ�����һ�����ļ�·��������ÿ��Ϊ�ļ����ơ�
 mode=1������ѡ�е��ļ���·��
 mode=2������ѡ�е��ļ�·��
 */
class JJH_SelectFile : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);
private:
	IMPLEMENT_REFCOUNTING(JJH_SelectFile);
};


/* �÷���JWebTop.file.reanme({params}, [handler]) */
class JJH_RenameFiles : public CefV8Handler {
private:
	list<wstring> settingList;

	static 	class FindedFile {// ����Ҫ�滻�ĺ�׺�������Ľ��
	public:
		wstring file, oldEnd, newEnd;
		FindedFile() {}
		FindedFile(const FindedFile& p) {
			file = p.file;
			oldEnd = p.oldEnd;
			newEnd = p.newEnd;
		   }
	};
	list<FindedFile> findedFiles;
	inline void __feedFile(wstring filename, int level);
	static void feedFile(void* ctx, const wchar_t * dir, wchar_t * name, bool isDir, int level);
	inline void howto(CefRefPtr<CefV8Value>& retval, int a);
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);
private:
	IMPLEMENT_REFCOUNTING(JJH_RenameFiles);
};

#endif