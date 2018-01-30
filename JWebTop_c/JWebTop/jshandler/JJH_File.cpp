

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
#include "JJH_Files.h"

//打开保存文件对话框  
#include<Commdlg.h>  
#include "common/util/StrUtil.h"
#include "common/file/FileUtil.h"
//选择文件夹对话框  
#include<Shlobj.h>  
//#pragma comment(lib,"Shell32.lib")  
// JJH=JWebTop JavaScriptHandler

static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
	if (uMsg == BFFM_INITIALIZED) {
		std::wstring tmp = (const wchar_t  *)lpData;
		std::cout << "path: " << jw::str::w2s(tmp) << std::endl;
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)lpData);
	}
	return 0;
}

//class JJH_SelectFile : public CefV8Handler {
//public:
bool JJH_SelectFile::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
	if (arguments.size() < 1) return false;
	CefRefPtr<CefV8Value> params = arguments[0];
	int mode = params->GetValue("mode")->GetIntValue();
	HWND hwnd = getHWND(object, arguments, 1);// 在哪个窗口上进行文件对话框的弹出
	CefString title = params->GetValue("title")->GetStringValue();
	CefString dir = params->GetValue("dir")->GetStringValue();
	if (mode == 1) {// 择文件夹对话框
		TCHAR szBuffer[MAX_PATH] = { 0 };
		BROWSEINFO bi = { 0 };
		if (!title.empty())bi.lpszTitle = LPCWSTR(title.ToWString().c_str());
		bi.hwndOwner = hwnd;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄  
		std::wstring tmp = dir.ToWString();
		std::wstring wtmp(tmp.begin(), tmp.end());
		const wchar_t * psTmp = wtmp.c_str();
		if (!dir.empty()) {// 需要设置初始化目录
			bi.lpfn = BrowseCallbackProc;// 回调函数：用于设置初始化目录
			bi.lParam = (LPARAM)psTmp;
		}
		bi.pszDisplayName = szBuffer;//接收文件夹的缓冲区  
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
		if (pidl != 0 && SHGetPathFromIDList(pidl, szBuffer)) {
			IMalloc * imalloc = 0;
			if (SUCCEEDED(SHGetMalloc(&imalloc))) {
				imalloc->Free(pidl);
				imalloc->Release();
			}
			retval = CefV8Value::CreateString(szBuffer);
		}
	} else {// 打开或保存文件
		OPENFILENAME ofn = { 0 };
		if (!title.empty())ofn.lpstrTitle = title.ToWString().c_str();//标题  
		ofn.hwndOwner = hwnd;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄  
		if (!dir.empty())ofn.lpstrInitialDir = dir.ToWString().c_str();//初始目录为默认  
		CefRefPtr<CefV8Value> filters = params->GetValue("filters");
		// CefString filters = params->GetValue("filters")->GetStringValue();
		if (filters != NULL&&filters->IsArray()) {
			int flen = filters->GetArrayLength();
			int len2 = 0;
			WCHAR tt[1000]; WCHAR *tt3 = tt;
			for (int i = 0; i < flen; i++) {
				wstring tt2 = filters->GetValue(i)->GetStringValue().ToWString();
				len2 = tt2.length();
				for (int k = 0; k < len2; k++) {
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
		if (mode == 2) {// 保存文件对话框
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;//目录必须存在，覆盖文件前发出警告
			// ofn.lpstrDefExt = suffix_w;//默认追加的扩展名  
			ofn.lStructSize = sizeof(OPENFILENAME);//结构体大小  
			if (GetSaveFileName(&ofn)) {
				retval = CefV8Value::CreateString(strFilename);
			}
		} else {// 选择文件对话框（默认）
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT;//文件、目录必须存在，隐藏只读选项  s
			ofn.lStructSize = sizeof(OPENFILENAME);//结构体大小  
			if (GetOpenFileName(&ofn)) {
				wstring str;
				str.append(ofn.lpstrFile);
				WCHAR *tmp = ofn.lpstrFile + ofn.nFileOffset;
				while (*tmp) {
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


class FindedFile {// 根据要替换的后缀检索出的结果
public:
	string file, oldEnd, newEnd;
	FindedFile() {}
	FindedFile(const FindedFile& p) {
		file = p.file;
		oldEnd = p.oldEnd;
		newEnd = p.newEnd;
	}
};
//list<FindedFile> findedFiles;
inline void JJH_RenameFiles::__feedFile(wstring filename, int level) {
	wstringstream dd;
	dd << L"settingList.size==" << settingList.size() << L" ";
	//writeLog(L"---__feedFile"); writeLog(dd.str());  writeLog(L"\r\n");
	// 如果检索文件，统计好个数，然后再重命名，体验会比较好，可以知道明确的进度
	int i = 0;
	list<wstring>::iterator it = settingList.begin();
	while (it != settingList.end()) {
		//for (list<string>::iterator it = settingList.begin(); it != settingList.end(); ++it) {
		if (jw::str::endswith(filename, *it)) {
			FindedFile ff;
			ff.file = filename;
			ff.oldEnd = *it;
			++it;
			ff.newEnd = *it;
			++it;
			std::wstringstream ss;
			ss << L"iter[" << i << L"]" << filename;
			//writeLog(L"\t\t"); writeLog(ss.str()); writeLog(L"\r\n");
			findedFiles.push_back(ff);
		} else {
			++it;
			++it;
		}
		i++;
	}
	//writeLog(L"++++__feedFile");  writeLog(L"\r\n");
}
void	JJH_RenameFiles::feedFile(void* ctx, const wchar_t * dir, wchar_t * name,bool isDir, int level) {
	if (isDir)return;
	JJH_RenameFiles * a = (JJH_RenameFiles *)ctx;
	wstring ws(dir);	ws.append(L"\\");	ws.append(name);
	a->__feedFile(ws, level);
}

inline void JJH_RenameFiles::howto(CefRefPtr<CefV8Value>& retval, int a) {
	retval->SetValue("errno", CefV8Value::CreateInt(errno), V8_PROPERTY_ATTRIBUTE_NONE);
	retval->SetValue("msg", CefV8Value::CreateString(L"用法：\r\nJWebTop.file.reanme({\r\n"\
		L"   dirs:        必填参数，数组[dir1,dir2,...]，所有需要进行重命名的文件夹,\r\n"\
		L"   settings:	 必填参数，二维数组[[old1,new1],[old2,new2],...]，要重命名的新旧后缀对照,\r\n"\
		L"   scanedFun:   可选参数，回调函数：function(filesCount)。filesCount：扫描出的需重命名的文件数量；返回值：true=继续处理，false=不进行后面的重命名操作。,\r\n"\
		L"   canRenameFun:可选参数，回调函数：function(idx,fileName)。idx：当前正在处理第几个文件，第一个文件时idx=0；fileName：将要被重命名后的文件；返回值：true=继续处理，false=不进行后面的重命名操作。\r\n"\
		L"   renamedFun:  可选参数，回调函数：function(idx,isRenamed,fileName)。idx：当前正在处理第几个文件，第一个文件时idx=0；isRenamed：重命名后是否成功；；fileName：重命名后的文件。\r\n"\
		L"})\r\n"\
		L"返回值：{success:true|false，msg:success=false时的错误信息}"), V8_PROPERTY_ATTRIBUTE_NONE);
}

bool JJH_RenameFiles::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
	retval = CefV8Value::CreateObject(NULL);
	retval->SetValue("success", CefV8Value::CreateBool(false), V8_PROPERTY_ATTRIBUTE_NONE);
	if (arguments.size() < 1) {
		howto(retval, 1);
		return true;
	}
	CefRefPtr<CefV8Value> params = arguments[0];
	// 检查目录参数
	CefRefPtr<CefV8Value>  dirs = params->GetValue("dirs");
	int dirCount;
	if (dirs == NULL) {
		howto(retval, 21);
		return true;
	}
	if (!dirs->IsArray()) {
		howto(retval, 22);
		return true;
	}
	if ((dirCount = dirs->GetArrayLength()) == 0) {
		howto(retval, 23);
		return true;
	}
	// 检查新旧后缀对照
	CefRefPtr<CefV8Value>  settings = params->GetValue("settings");
	int settingCount;
	if (settings == NULL || !settings->IsArray() || (settingCount = settings->GetArrayLength()) == 0) {
		howto(retval, 3);
		return true;
	}
	CefRefPtr<CefV8Value>  tmp = NULL,tmp2=NULL;
	int i = 0;
	for (i = 0; i < settingCount; i++) {
		tmp = settings->GetValue(i);
		if (tmp == NULL || !tmp->IsArray() || (settingCount = tmp->GetArrayLength()) == 0)continue;
		tmp2 = tmp->GetValue(0);
		if (tmp2 == NULL)continue;
		// 把数组中的两个对象放进去，相邻的两个是旧和新的替换对应
		settingList.push_back(tmp2->GetStringValue().ToWString());
		if (settingCount > 1) {
			tmp2 = tmp->GetValue(1);
			settingList.push_back(tmp2 == NULL ? L"" : tmp2->GetStringValue().ToWString());
		} else {
			settingList.push_back(L"");
		}
	}
	// 检查两个回调函数
	CefRefPtr<CefV8Value> scanedFun = arguments[0]->GetValue("scanedFun");
	if (scanedFun != NULL&&!scanedFun->IsFunction()) {
		howto(retval, 4);
		return true;
	}
	CefRefPtr<CefV8Value> canRenameFun = arguments[0]->GetValue("canRenameFun");
	if (canRenameFun != NULL&&!canRenameFun->IsFunction()) {
		howto(retval, 5);
		return true;
	}
	CefRefPtr<CefV8Value> renamedFun = arguments[0]->GetValue("renamedFun");
	if (renamedFun != NULL&&!renamedFun->IsFunction()) {
		howto(retval, 6);
		return true;
	}
	// 开始遍历文件
	for (i = 0; i < dirCount; i++) {
		tmp = dirs->GetValue(i);
		if (tmp != NULL) {
			std::wstringstream ss;
			ss << L"scan[" << i << L"]" << tmp->GetStringValue().ToWString().c_str();
			//writeLog(L"---"); writeLog(ss.str()); writeLog(L"\r\n");
			jw::file::listFiles(tmp->GetStringValue().ToWString().c_str(), feedFile, this);
		}
	}
	if (scanedFun != NULL) {
		CefV8ValueList args;
		args.push_back(CefV8Value::CreateInt(findedFiles.size()));
		tmp = scanedFun->ExecuteFunction(object, args);// 调用传入的JS函数，检查是否绩效进行处理
		if (tmp != NULL && !tmp->GetBoolValue()) {
			retval->SetValue("msg", CefV8Value::CreateString(L"用户在检索文件后，终止了文件重命名操作"), V8_PROPERTY_ATTRIBUTE_NONE);
			return true;
		}
	}
	// 进行重命名
	i = 0;
	int r = -1;
	for (list<FindedFile>::iterator it = findedFiles.begin(); it != findedFiles.end(); ++it) {
		FindedFile ff = *it;
		if (canRenameFun != NULL) {// 检查js是否允许继续重命名
			CefV8ValueList args;
			args.push_back(CefV8Value::CreateInt(i));
			args.push_back(CefV8Value::CreateString(ff.file));
			tmp = canRenameFun->ExecuteFunction(object, args);
			if (tmp != NULL && !tmp->GetBoolValue()) {
				retval->SetValue("msg", CefV8Value::CreateString(L"用户终止了文件重命名操作"), V8_PROPERTY_ATTRIBUTE_NONE);
				return true;
			}
		}
		wstring f2(ff.file, 0, ff.file.length() - ff.oldEnd.length());
		f2.append(ff.newEnd);
		r = _wrename(ff.file.c_str(), f2.c_str());
		if (renamedFun != NULL) {
			CefV8ValueList args;
			args.push_back(CefV8Value::CreateInt(i));
			args.push_back(CefV8Value::CreateBool(r == 0));
			args.push_back(CefV8Value::CreateString(ff.file));
			renamedFun->ExecuteFunction(object, args);
		}
		i++;
	}
	retval->SetValue("success", CefV8Value::CreateBool(true), V8_PROPERTY_ATTRIBUTE_NONE);
	return true;
}
