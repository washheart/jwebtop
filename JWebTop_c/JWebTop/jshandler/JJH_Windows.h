#ifndef CEF_JWEBTOP_JJH_WINDOWS_H_
#define CEF_JWEBTOP_JJH_WINDOWS_H_

#include <iostream>  
#include <fstream>  

#include <sstream>
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
//选择文件夹对话框  
#include<Shlobj.h>  
//#pragma comment(lib,"Shell32.lib")  
// JJH=JWebTop JavaScriptHandler

//getPos(handler);//获得窗口位置，返回值为一object，格式如下{x:13,y:54}
class JJH_GetPos : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		POINT p = jw::getPos(getHWND(object, arguments, 0));
		retval = CefV8Value::CreateObject(NULL);
		retval->SetValue("x", CefV8Value::CreateInt(p.x), V8_PROPERTY_ATTRIBUTE_NONE);
		retval->SetValue("y", CefV8Value::CreateInt(p.y), V8_PROPERTY_ATTRIBUTE_NONE);
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_GetPos);
};

//setSize(x, y, handler);//设置窗口大小
class JJH_SetSize : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		if (arguments.size() < 2)return false;
		jw::setSize(getHWND(object, arguments, 2), arguments[0]->GetIntValue(), arguments[1]->GetIntValue());
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_SetSize);
};
//getSize(handler);//获得窗口大小，返回值为一object，格式如下{width:130,height:54}
class JJH_GetSize : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		POINT p = jw::getSize(getHWND(object, arguments, 0));
		retval = CefV8Value::CreateObject(NULL);
		retval->SetValue("width", CefV8Value::CreateInt(p.x), V8_PROPERTY_ATTRIBUTE_NONE);
		retval->SetValue("height", CefV8Value::CreateInt(p.y), V8_PROPERTY_ATTRIBUTE_NONE);
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_GetSize);
};
//getScreenSize();//获取屏幕大小，返回值为一object，格式如下{width:130,height:54}
class JJH_GetScreenSize : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		POINT p = jw::getScreenSize();
		retval = CefV8Value::CreateObject(NULL);
		retval->SetValue("width", CefV8Value::CreateInt(p.x), V8_PROPERTY_ATTRIBUTE_NONE);
		retval->SetValue("height", CefV8Value::CreateInt(p.y), V8_PROPERTY_ATTRIBUTE_NONE);
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_GetScreenSize);
};
//move(x, y, handler);//移动窗口
class JJH_Move : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		if (arguments.size() < 2)return false;
		jw::move(getHWND(object, arguments, 2), arguments[0]->GetIntValue(), arguments[1]->GetIntValue());
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_Move);
};
//setBound(x, y,w ,h, handler);// 同时设置窗口的坐标和大小
class JJH_SetBound : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		if (arguments.size() < 4)return false;
		jw::setBound(getHWND(object, arguments, 4), arguments[0]->GetIntValue(), arguments[1]->GetIntValue(), arguments[2]->GetIntValue(), arguments[3]->GetIntValue());
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_SetBound);
};
//getBound(handler);//获取窗口的位置和大小，返回值为一object，格式如下{x:100,y:100,width:130,height:54}
class JJH_GetBound : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		RECT rt = jw::getBound(getHWND(object, arguments, 0));
		retval = CefV8Value::CreateObject(NULL);
		retval->SetValue("x", CefV8Value::CreateInt(rt.left), V8_PROPERTY_ATTRIBUTE_NONE);
		retval->SetValue("y", CefV8Value::CreateInt(rt.top), V8_PROPERTY_ATTRIBUTE_NONE);
		retval->SetValue("width", CefV8Value::CreateInt(rt.right - rt.left), V8_PROPERTY_ATTRIBUTE_NONE);
		retval->SetValue("height", CefV8Value::CreateInt(rt.bottom - rt.top), V8_PROPERTY_ATTRIBUTE_NONE);
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_GetBound);
};
//setTitle(title, handler);// 设置窗口名称
class JJH_SetTitle : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		if (arguments.size() < 1)return false;
		jw::setTitle(getHWND(object, arguments, 1), arguments[0]->GetStringValue().ToWString());
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_SetTitle);
};
//getTitle(handler);// 获取窗口名称，返回值为一字符串
class JJH_GetTitle : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		std::wstring title = jw::getTitle(getHWND(object, arguments, 0));
		retval = CefV8Value::CreateString(CefString(title));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_GetTitle);
};
//close(handler);// 关闭窗口
class JJH_Close : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jb::close(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_Close);
};
// bringToTop(handler);//窗口移到最顶层
class JJH_BringToTop : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jw::bringToTop(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_BringToTop);
};
// focus(handler);//使窗口获得焦点
class JJH_Focus : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jw::focus(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_Focus);
};

//hide(handler);//隐藏窗口
class JJH_Hide : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jw::hide(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_Hide);
};
//max(handler);//最大化窗口
class JJH_Max : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jw::maxWin(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_Max);
};
//mini(hander);//最小化窗口
class JJH_Mini : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jw::mini(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_Mini);
};
//restore(handler);//还原窗口，对应于hide函数
class JJH_Restore : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jw::restore(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_Restore);
};
//setTopMost(handler);//窗口置顶，此函数跟bringToTop的区别在于此函数会使窗口永远置顶，除非有另外一个窗口调用了置顶函数
class JJH_SetTopMost : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jw::setTopMost(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_SetTopMost);
};
//setWindowStyle(style, handler);//高级函数，设置窗口额外属性，诸如置顶之类。
class JJH_SetWindowStyle : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		if (arguments.size() < 1)return false;
		jw::setWindowStyle(getHWND(object, arguments, 2), arguments[0]->GetIntValue());
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_SetWindowStyle);
};
//setWindowStyle(exStyle, handler);//高级函数，设置窗口额外属性，诸如置顶之类。
class JJH_SetWindowExStyle : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		if (arguments.size() < 1)return false;
		jw::setWindowExStyle(getHWND(object, arguments, 1), arguments[0]->GetIntValue());
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_SetWindowExStyle);
};

static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
		if (uMsg == BFFM_INITIALIZED)
	{
		std::wstring tmp = (const wchar_t  *)lpData;
		std::cout << "path: " << jw::str :: w2s(tmp) << std::endl;
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)lpData);
	}
	return 0;
}
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
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		if (arguments.size() < 1) return false;
		CefRefPtr<CefV8Value> params = arguments[0];
		int mode=params->GetValue("mode")->GetIntValue();
		HWND hwnd = getHWND(object, arguments, 1);// 在哪个窗口上进行文件对话框的弹出
		CefString title = params->GetValue("title")->GetStringValue();
		CefString dir = params->GetValue("dir")->GetStringValue();
		if (mode == 1){// 择文件夹对话框
			TCHAR szBuffer[MAX_PATH] = { 0 };
			BROWSEINFO bi = { 0 };
			if (!title.empty())bi.lpszTitle = LPCWSTR(title.ToWString().c_str());			
			bi.hwndOwner = hwnd;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄  
			std::wstring tmp = dir.ToWString();
			std::wstring wtmp(tmp.begin(), tmp.end());
			const wchar_t * psTmp = wtmp.c_str();
			if (!dir.empty()){// 需要设置初始化目录
				bi.lpfn = BrowseCallbackProc;// 回调函数：用于设置初始化目录
				bi.lParam = (LPARAM)psTmp;
			}
			bi.pszDisplayName = szBuffer;//接收文件夹的缓冲区  
			bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
			LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
			if (pidl != 0 && SHGetPathFromIDList(pidl, szBuffer)){
				IMalloc * imalloc = 0;
				if (SUCCEEDED(SHGetMalloc(&imalloc)))
				{
					imalloc->Free(pidl);
					imalloc->Release();
				}
				retval = CefV8Value::CreateString(szBuffer);
			}
		}else{// 打开或保存文件
			OPENFILENAME ofn = { 0 };
			if (!title.empty())ofn.lpstrTitle = title.ToWString().c_str();//标题  
			ofn.hwndOwner = hwnd;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄  
			if (!dir.empty())ofn.lpstrInitialDir = dir.ToWString().c_str();//初始目录为默认  
			CefRefPtr<CefV8Value> filters=params->GetValue("filters");
			// CefString filters = params->GetValue("filters")->GetStringValue();
			if (filters != NULL&&filters->IsArray()){
				int flen=filters->GetArrayLength();
				int len2=0;
				WCHAR tt[1000]; WCHAR *tt3 = tt;
				for (int i = 0; i < flen; i++){
					wstring tt2 = filters->GetValue(i)->GetStringValue().ToWString();
					len2 = tt2.length();
					for (int k = 0; k < len2; k++){
						(*tt3) = tt2.at(k);	tt3++;
					}
					(*tt3) = 0; tt3++;
				}
				(*tt3) = 0; tt3++;
				ofn.lpstrFilter = tt;
				ofn.nFilterIndex = 1;//过滤器索引  
			}
			WCHAR strFilename[1000 * MAX_PATH];//用于接收文件名 
			CefString file = params->GetValue("file")->GetStringValue();
			if (!file.empty())wcscpy_s(strFilename, file.ToWString().c_str());
			ofn.lpstrFile = strFilename;
			ofn.nMaxFile = sizeof(strFilename);//缓冲区长度  
			if (mode == 2){// 保存文件对话框
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;//目录必须存在，覆盖文件前发出警告
				// ofn.lpstrDefExt = suffix_w;//默认追加的扩展名  
				ofn.lStructSize = sizeof(OPENFILENAME);//结构体大小  
				if (GetSaveFileName(&ofn))
				{
					retval = CefV8Value::CreateString(strFilename);
				}
			}
			else{// 选择文件对话框（默认）
				ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT;//文件、目录必须存在，隐藏只读选项  s
				ofn.lStructSize = sizeof(OPENFILENAME);//结构体大小  
				if (GetOpenFileName(&ofn)){
					wstring str;
					str.append(ofn.lpstrFile);
					WCHAR *tmp = ofn.lpstrFile + ofn.nFileOffset;
					while (*tmp){
						str.append(L"\n").append(tmp);
						tmp += lstrlenW(tmp) + 1;
					}
					retval = CefV8Value::CreateString(str);
				}
			}
		}
		// 【测试验证用代码】将对象参数按全部是字符串取出并返回
		//string s;stringstream ss;vector<CefString> keys;arguments[0]->GetKeys(keys);vector<CefString>::iterator it = keys.begin();
		//while (it != keys.end()){s = (*it).ToString();
		//	ss << s << "=" << arguments[0]->GetValue((*it))->GetStringValue().ToString()/*注意这里都按字符串来取了*/<< "\r\n";
		//	it++;}
		//retval = CefV8Value::CreateString(ss.str());// 将组织好的字符串返回
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_SelectFile);
};

//loadUrl(url, handler);//加载网页，url为网页路径
class JJH_LoadUrl : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		if (arguments.size() < 1)return false;
		jb::loadUrl(getHWND(object, arguments, 1), arguments[0]->GetStringValue().ToWString());
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_LoadUrl);
};
//reload(handler);//重新加载当前页面
class JJH_Reload : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jb::reload(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_Reload);
};

//reloadIgnoreCache(handler);//重新加载当前页面并忽略缓存
class JJH_ReloadIgnoreCache : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jb::reloadIgnoreCache(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_ReloadIgnoreCache);
};
//showDev(handler);//打开开发者工具
class JJH_ShowDev : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jb::showDev(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_ShowDev);
};

//invokeRemote_CallBack(jsonstring,callback,[handler]);// 从JS调用远程进程代码
class JJH_InvokeRemote_CallBack : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		if (arguments.size() < 2){
			exception = (L"invokeRemote_CallBack(jsonstring,callback,[handler])需要至少两个参数");
			return true;
		}
		if (!arguments[1]->IsString()){
			exception = (L"第二个参数必须是字符串");
			return true;
		}
		string callback = arguments[1]->GetStringValue().ToString();
		int size = callback.size();
		if (size == 0){
			exception = (L"第二个参数不能是空字符串");
			return true;
		}
		if (size > 100){
			exception = (L"第二个参数长度不能超过100");
			return true;
		}
		jw::dllex::invokeRemote_CallBack(getHWND(object, arguments, 2), arguments[0]->GetStringValue(), callback);
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_InvokeRemote_CallBack);
};

//JJH_InvokeRemote_NoWait(jsonstring,[handler]);// 从JS调用远程进程代码
class JJH_InvokeRemote_NoWait : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		if (arguments.size() < 1)return false;
		jw::dllex::invokeRemote_NoWait(getHWND(object, arguments, 1), arguments[0]->GetStringValue());
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_InvokeRemote_NoWait);
};

//JJH_enableDrag(enable);// 允许进行拖动
class JJH_enableDrag : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jb::enableDrag(getHWND(object, arguments, 10000), arguments[0]->GetBoolValue());
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_enableDrag);
};

//JJH_startDrag();// 开始进行拖动
class JJH_startDrag : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jb::startDrag(getHWND(object, arguments, 10000));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_startDrag);
};
//JJH_stopDrag([handler]);// 停止拖动
class JJH_stopDrag : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jb::stopDrag(getHWND(object, arguments, 10000));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_stopDrag);
};


///*
//JJH_TestFunInJSON({params}, [handler]);// 测试回调通过JSON样式传入的JS函数。
//用法：JWebTop.selectFile({params}, [handler])
//params={
// str:	一个字符串
// fun:	需要进行回调的JavaScript函数
//}
//返回值：无
//*/
//class JJH_TestFunInJSON : public CefV8Handler {
//public:
//	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
//		if (arguments.size() < 1) return false;
//		CefRefPtr<CefV8Value> params = arguments[0];
//		CefString str = params->GetValue("str")->GetStringValue();
//		std::wstring ss = str.ToWString();
//		std::reverse(ss.begin(), ss.end());
//		// 函数回调
//		CefRefPtr<CefV8Value> fun = params->GetValue("fun");
//		CefV8ValueList args;
//		CefRefPtr<CefV8Value> v = CefV8Value::CreateString(ss);
//		args.push_back(v);
//		fun->ExecuteFunction(object, args);
//		return true;
//	}
//private:
//	IMPLEMENT_REFCOUNTING(JJH_TestFunInJSON);
//};
////invokeReflect(function(v){});// 测试回调js的function
//class JJH_invokeReflect : public CefV8Handler {
//public:
//	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
//		CefV8ValueList args;
//		std::string jsons = "{\"a\":\"a\",\"b\":1234}";
//		CefRefPtr<CefV8Value> v = CefV8Value::CreateString(jsons);
//		args.push_back(v);
//		CefRefPtr<CefV8Value> fun = arguments[0];
//		fun->ExecuteFunction(object, args);
//		return true;
//	}
//private:
//	IMPLEMENT_REFCOUNTING(JJH_invokeReflect);
//};

namespace jw{
	namespace js{
		namespace events{
			// 发送页面准备好事件：new CustomEvent('JWebTopReady')
			void sendReadey(const CefRefPtr<CefFrame> frame);

			// 页面内的子页面（iframe）准备好事件：new CustomEvent('JWebTopIFrameReady')
			void sendIFrameReady(const CefRefPtr<CefFrame> frame);

			// 发送窗口大小改变事件:new CustomEvent('JWebTopResize',{detail:{w:宽度数值,h:高度数值}})
			void sendSize(const CefRefPtr<CefFrame> frame, const int w, const int h);

			// 发送窗口位置改变事件:new CustomEvent('JWebTopMove',{detail:{x:X坐标值,y:Y坐标值}})
			void sendMove(const CefRefPtr<CefFrame> frame, const int x, const int y);

			// 发送窗口被激活事件:new CustomEvent('JWebTopWindowActive',{detail:{handler:被激活的窗口的句柄}})
			void sendWinowActive(const CefRefPtr<CefFrame> frame, const long handler, const DWORD state);

			// 发送应用（一个应用可能有多个窗口）被激活事件:new CustomEvent('JWebTopAppActive',{detail:{handler:除非此消息的窗口的句柄}})
			void sendAppActive(const CefRefPtr<CefFrame> frame, const long handler);

			// 在浏览器执行了文件操作：粘贴(JWebTopFilePasted)、拖入文件(JWebTopFileDroped)
			void sendFileEvent(const CefRefPtr<CefFrame> frame, vector<CefString> files, std::string eventName);
			
			// 发送页面加载失败事件
			void sendLoadError(const CefRefPtr<CefFrame> frame, int errorCode, const CefString& errorText, const CefString& failedUrl);
		}
	}
}
#endif