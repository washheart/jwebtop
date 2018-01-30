

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

//�򿪱����ļ��Ի���  
#include<Commdlg.h>  
#include "common/util/StrUtil.h"
#include "common/file/FileUtil.h"
//ѡ���ļ��жԻ���  
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
	HWND hwnd = getHWND(object, arguments, 1);// ���ĸ������Ͻ����ļ��Ի���ĵ���
	CefString title = params->GetValue("title")->GetStringValue();
	CefString dir = params->GetValue("dir")->GetStringValue();
	if (mode == 1) {// ���ļ��жԻ���
		TCHAR szBuffer[MAX_PATH] = { 0 };
		BROWSEINFO bi = { 0 };
		if (!title.empty())bi.lpszTitle = LPCWSTR(title.ToWString().c_str());
		bi.hwndOwner = hwnd;//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������  
		std::wstring tmp = dir.ToWString();
		std::wstring wtmp(tmp.begin(), tmp.end());
		const wchar_t * psTmp = wtmp.c_str();
		if (!dir.empty()) {// ��Ҫ���ó�ʼ��Ŀ¼
			bi.lpfn = BrowseCallbackProc;// �ص��������������ó�ʼ��Ŀ¼
			bi.lParam = (LPARAM)psTmp;
		}
		bi.pszDisplayName = szBuffer;//�����ļ��еĻ�����  
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
	} else {// �򿪻򱣴��ļ�
		OPENFILENAME ofn = { 0 };
		if (!title.empty())ofn.lpstrTitle = title.ToWString().c_str();//����  
		ofn.hwndOwner = hwnd;//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������  
		if (!dir.empty())ofn.lpstrInitialDir = dir.ToWString().c_str();//��ʼĿ¼ΪĬ��  
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
			ofn.nFilterIndex = 1;//����������  
		}
		WCHAR strFilename[1000 * MAX_PATH];//���ڽ����ļ��� 
		CefString file = params->GetValue("file")->GetStringValue();
		if (!file.empty())wcscpy_s(strFilename, file.ToWString().c_str());
		ofn.lpstrFile = strFilename;
		ofn.nMaxFile = sizeof(strFilename);//����������  
		if (mode == 2) {// �����ļ��Ի���
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;//Ŀ¼������ڣ������ļ�ǰ��������
			// ofn.lpstrDefExt = suffix_w;//Ĭ��׷�ӵ���չ��  
			ofn.lStructSize = sizeof(OPENFILENAME);//�ṹ���С  
			if (GetSaveFileName(&ofn)) {
				retval = CefV8Value::CreateString(strFilename);
			}
		} else {// ѡ���ļ��Ի���Ĭ�ϣ�
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT;//�ļ���Ŀ¼������ڣ�����ֻ��ѡ��  s
			ofn.lStructSize = sizeof(OPENFILENAME);//�ṹ���С  
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
	// ��������֤�ô��롿�����������ȫ�����ַ���ȡ��������
	//string s;stringstream ss;vector<CefString> keys;arguments[0]->GetKeys(keys);vector<CefString>::iterator it = keys.begin();
	//while (it != keys.end()){s = (*it).ToString();
	//	ss << s << "=" << arguments[0]->GetValue((*it))->GetStringValue().ToString()/*ע�����ﶼ���ַ�����ȡ��*/<< "\r\n";
	//	it++;}
	//retval = CefV8Value::CreateString(ss.str());// ����֯�õ��ַ�������
	return true;
}


class FindedFile {// ����Ҫ�滻�ĺ�׺�������Ľ��
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
	// ��������ļ���ͳ�ƺø�����Ȼ�����������������ȽϺã�����֪����ȷ�Ľ���
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
	retval->SetValue("msg", CefV8Value::CreateString(L"�÷���\r\nJWebTop.file.reanme({\r\n"\
		L"   dirs:        �������������[dir1,dir2,...]��������Ҫ�������������ļ���,\r\n"\
		L"   settings:	 �����������ά����[[old1,new1],[old2,new2],...]��Ҫ���������¾ɺ�׺����,\r\n"\
		L"   scanedFun:   ��ѡ�������ص�������function(filesCount)��filesCount��ɨ����������������ļ�����������ֵ��true=��������false=�����к����������������,\r\n"\
		L"   canRenameFun:��ѡ�������ص�������function(idx,fileName)��idx����ǰ���ڴ���ڼ����ļ�����һ���ļ�ʱidx=0��fileName����Ҫ������������ļ�������ֵ��true=��������false=�����к����������������\r\n"\
		L"   renamedFun:  ��ѡ�������ص�������function(idx,isRenamed,fileName)��idx����ǰ���ڴ���ڼ����ļ�����һ���ļ�ʱidx=0��isRenamed�����������Ƿ�ɹ�����fileName������������ļ���\r\n"\
		L"})\r\n"\
		L"����ֵ��{success:true|false��msg:success=falseʱ�Ĵ�����Ϣ}"), V8_PROPERTY_ATTRIBUTE_NONE);
}

bool JJH_RenameFiles::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
	retval = CefV8Value::CreateObject(NULL);
	retval->SetValue("success", CefV8Value::CreateBool(false), V8_PROPERTY_ATTRIBUTE_NONE);
	if (arguments.size() < 1) {
		howto(retval, 1);
		return true;
	}
	CefRefPtr<CefV8Value> params = arguments[0];
	// ���Ŀ¼����
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
	// ����¾ɺ�׺����
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
		// �������е���������Ž�ȥ�����ڵ������Ǿɺ��µ��滻��Ӧ
		settingList.push_back(tmp2->GetStringValue().ToWString());
		if (settingCount > 1) {
			tmp2 = tmp->GetValue(1);
			settingList.push_back(tmp2 == NULL ? L"" : tmp2->GetStringValue().ToWString());
		} else {
			settingList.push_back(L"");
		}
	}
	// ��������ص�����
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
	// ��ʼ�����ļ�
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
		tmp = scanedFun->ExecuteFunction(object, args);// ���ô����JS����������Ƿ�Ч���д���
		if (tmp != NULL && !tmp->GetBoolValue()) {
			retval->SetValue("msg", CefV8Value::CreateString(L"�û��ڼ����ļ�����ֹ���ļ�����������"), V8_PROPERTY_ATTRIBUTE_NONE);
			return true;
		}
	}
	// ����������
	i = 0;
	int r = -1;
	for (list<FindedFile>::iterator it = findedFiles.begin(); it != findedFiles.end(); ++it) {
		FindedFile ff = *it;
		if (canRenameFun != NULL) {// ���js�Ƿ��������������
			CefV8ValueList args;
			args.push_back(CefV8Value::CreateInt(i));
			args.push_back(CefV8Value::CreateString(ff.file));
			tmp = canRenameFun->ExecuteFunction(object, args);
			if (tmp != NULL && !tmp->GetBoolValue()) {
				retval->SetValue("msg", CefV8Value::CreateString(L"�û���ֹ���ļ�����������"), V8_PROPERTY_ATTRIBUTE_NONE);
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
