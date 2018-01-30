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


//打开保存文件对话框  
#include<Commdlg.h>  
#include "common/util/StrUtil.h"
#include "common/file/FileUtil.h"
//选择文件夹对话框  
#include<Shlobj.h>  
//#pragma comment(lib,"Shell32.lib")  
// JJH=JWebTop JavaScriptHandler


/*
 JJH_SelectFile({params}, [handler]);//高级函数，打开选择文件对话框。
 用法：JWebTop.selectFile({params}, [handler])
 params={
 mode:	0=选择文件对话框（默认）；1=选择文件夹对话框；2=保存文件对话框
 title:	选择文件的提示信息，可以为空
 dir:	对话框初始化时的目录，可以为空
 file:	对话框初始化时的文件，可以为空
 filters:过滤器规则，一个JSON数组[名称、规则，名称、规则、... ...]，例如：['所有文件','*.*','C/C++ Flie','*.cpp;*.c;*.h']
 }
 返回值：字符串
 当字符串为null时表示用户执行的是取消操作
 mode=0，返回以换行符分隔的多个文件。第一行是文件路径，后续每行为文件名称。
 mode=1，返回选中的文件夹路径
 mode=2，返回选中的文件路径
 */
class JJH_SelectFile : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);
private:
	IMPLEMENT_REFCOUNTING(JJH_SelectFile);
};


/* 用法：JWebTop.file.reanme({params}, [handler]) */
class JJH_RenameFiles : public CefV8Handler {
private:
	list<wstring> settingList;

	static 	class FindedFile {// 根据要替换的后缀检索出的结果
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