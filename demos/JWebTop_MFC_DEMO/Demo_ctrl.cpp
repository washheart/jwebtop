#include "Demo.h"
#include "include/cef_parser.h"
#include "common/fastipc/Server.h"
#include "common/fastipc/Client.h"
#include "common/util/StrUtil.h"
#include "common/os/OS.h"
#include <thread>
#include <string>
#include <sstream> 
#include <iostream>  
#include <fstream>  
#include <direct.h>  
#include "jwebtopex/jwebtopex.h"

extern std::wstring serverName;	// 服务端名称
extern fastipc::Server server;  // IPC服务端
extern DWORD blockSize;			// 一次传输时的数据大小
extern HWND g_hWnd;
wstring getNotesFile() {
	LPTSTR fn = L"data/note/list.txt";
	if (_waccess(fn, 0) != 0) {
		_mkdir("data/note");
	}
	return fn;
}
wstring getNoteFile(wstring name) {
	wstring fn(L"data/note/");
	fn.append(name);
	fn.append(L".txt");
	return fn;
}

wstring toJSON(CefRefPtr<CefDictionaryValue> value){
	CefRefPtr<CefValue> node = CefValue::Create();
	node->SetDictionary(value);
	CefString result = CefWriteJSON(node, JSON_WRITER_DEFAULT);
	return result.ToWString();
}

// 得到配置文件
wstring DemoCtrl::getListAppFile(){
	LPTSTR path = L"res/list/index.app";
	if (_waccess(path, 0) != 0){// 按开发环境来处理
		path = L"../../res/list/index.app";
	}
	wchar_t fullpath[1024];
	_wfullpath(fullpath, path, 1024);
	wstring rtn(fullpath);
	delete[] & fullpath;
	return rtn;
}

// 得到配置文件
wstring DemoCtrl::getDetailAppFile(){
	LPTSTR path = L"res/detail/index.app";
	if (_waccess(path, 0) != 0){// 按开发环境来处理
		path = L"../../res/detail/index.app";
	}
	wchar_t fullpath[1024];
	_wfullpath(fullpath, path, 1024);
	wstring rtn(fullpath);
	delete[] & fullpath;
	return rtn;
}
// 得到JWebTop.exe的路径
wstring DemoCtrl::getJWebTopExe(){
	wchar_t fullpath[1024];
	_wfullpath(fullpath, L"JWebTop.exe", 1024);
	wstring rtn(fullpath);
	delete[] & fullpath;
	return rtn;
}

void DemoCtrl::_startJWebTop(){
	wstring appFile = this->getListAppFile();
	ctx->createJWebTopByCfgFile(this->getJWebTopExe(), appFile, this);
}

void DemoCtrl::init(){
	if (ctx != NULL)return;
	ctx = new JWebTopContext();
	ctx->setjsonDispater(this);
	thread th(std::mem_fn(&DemoCtrl::_startJWebTop), this);
	th.detach(); //分离线程  
}

void DemoCtrl::onJWebTopAppInited(){
	wstring appFile = this->getListAppFile();
	JWebTopConfigs * configs = new JWebTopConfigs();
	configs->appDefFile = appFile;
	configs->name = L"列表页";
	configs->parentWin = (long)g_hWnd;
	ctx->createBrowser(configs, this);
}
void DemoCtrl::onJWebTopBrowserCreated(long browserHWnd){
	this->listBrowser = (HWND)browserHWnd;
}

void DemoCtrl::initNames() {
	lock.lock();
	try{
		if (names != NULL) return;
		names = new NameList();
		wstring fn = getNotesFile();
		ifstream in(fn);
		wstring s = L" sd中文fad ";
		if (in.is_open()){
			while (!in.eof()){
				string line;
				std::getline(in, line, '\n');
				line=jw::trim(line);
				if (line.length() == 0)continue;
				names->push_back(jw::s2w(line));
			}
		}
		in.clear();
		in.close();
	} catch (...){
		lock.unlock();
	}
}
char* G2U(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}
void DemoCtrl::showDetail(wstring note) {
	CefRefPtr<CefDictionaryValue> rtn = CefDictionaryValue::Create();
	rtn->SetString(CefString("method"), CefString("showDetail"));
	wstring detail;
	bool hasData = true;
	if (jw::trim(note).length() == 0) {// 如果note无效，默认取第一个note进行显示
		if (names == NULL) initNames();
		if (names == NULL || names->size() == 0) {
			hasData = false;
			detail.append( L"请使用“添加日记”按钮新建日记");
		} else {
			note = *(names->begin());
		}
	}
	if (hasData) {
		detail.append(L"在这里输入【");
		detail.append(note);
		detail.append(L"】日记的内容");
		wstring fn = getNoteFile(note);
		// _access找不到？？？
		if (_waccess(fn.c_str(), 0) == 0) {
			ifstream in(fn);
			string tmp;
			if (in.is_open()){
				while (!in.eof()){
					std::getline(in, tmp);
				}
			}
			in.clear();
			in.close();
			detail = jw::s2w(tmp);
		}
	}
	//detail = jw::s2w(G2U(detail.c_str()));
	rtn->SetString(CefString("value"), CefString(detail));
	rtn->SetString(CefString("note"), CefString(note));
	this->ctx->executeJSON_NoWait((long)detailBrowser, toJSON(rtn));
}
 void DemoCtrl::saveNote(wstring note, wstring content) {
	 if (jw::trim(note).length() > 0) {
		wstring fn = getNoteFile(note);
		ofstream os(fn);
		os << jw::w2s(content);
		os.clear();
		os.close();
	}
}
 void DemoCtrl::saveNotes() {
	wstring fn = getNotesFile();
	ofstream os(fn);
	NameList::iterator it;
	for (it = names->begin(); it != names->end(); it++)  {
		os << jw::w2s(*(it)) << "\r\n";
	}
	os.clear();
	os.close();
}

 wstring DemoCtrl::addNote(wstring name) {
	if (getNoteIdx(name)!=-1) return L"日记名称不能重复";
	this->names->push_back(name);
	saveNotes();
	CefRefPtr<CefDictionaryValue> rtn = CefDictionaryValue::Create();
	rtn->SetString(CefString("method"), CefString("noteAdded"));
	rtn->SetString(CefString("value"), CefString(name));
	ctx->executeJSON_NoWait((long)listBrowser, toJSON(rtn));
	return L"";
}

  void DemoCtrl::delNote() {
	  int idx = getNoteIdx(currentNote);
	  if (idx!=-1){
		  this->names->remove(currentNote);
		  saveNotes();
		  CefRefPtr<CefDictionaryValue> rtn = CefDictionaryValue::Create();
		  rtn->SetString(CefString("method"), CefString("noteRemoved"));
		  rtn->SetString(CefString("value"), CefString(currentNote));
		  this->currentNote = L"";
		  if (idx > 0) idx = idx - 1;
		  if (idx < this->names->size()) {
			  rtn->SetString(CefString("newSel"), CefString(getNoteIdx(idx)));
		  } else {
			  showDetail(L"");
		  }
		  ctx->executeJSON_NoWait((long)listBrowser, toJSON(rtn));
	  }
}

  wstring DemoCtrl::dispatcher(long browserHWnd, wstring json){
	if (json.length() == 0) return L"";
	CefRefPtr<CefValue> v = CefParseJSON(CefString(json), JSON_PARSER_RFC);  // 进行解析
	if (v == NULL)return L"";
	CefRefPtr<CefDictionaryValue> jo = v->GetDictionary();// 按JSON字典来获取
	wstring method = jo->GetString("method").ToWString();
	if (L"initList" == (method)) {
		initNames();
		CefRefPtr<CefDictionaryValue> value = CefDictionaryValue::Create();
		value->SetString(CefString("method"), CefString(method));
		CefRefPtr<CefListValue>	ceflist = CefListValue::Create();
		NameList::iterator it;
		int idx = 0;
		for (it = names->begin(); it != names->end(); it++)  {
			ceflist->SetString(idx++, CefString(*it));
		}
		value->SetList(CefString("value"), ceflist);
		return toJSON(value);
	} else if (L"getDetailAppFile" == (method)) {
		CefRefPtr<CefDictionaryValue> value = CefDictionaryValue::Create();
		value->SetString(CefString("value"), CefString(getDetailAppFile()));
		return toJSON(value);
	} else if (L"setDetailHandler" == (method)) {
		this->detailBrowser = (HWND)jo->GetInt("value");
		showDetail(getNoteIdx(0));
		relayout();
	} else if (L"showDetail" == (method)) {
		wstring name = jo->GetString("value").ToWString();
		this->currentNote = name;
		showDetail(name);
	} else if (L"saveNote" == (method)) {
		saveNote(jo->GetString("note").ToWString(), jo->GetString("value").ToWString());
	} else if (L"showdetail" == (method)) {
		wstring name = jo->GetString("value").ToWString();
		this->currentNote = name;
		showDetail(name);
	} else if (L"javaWindowHwnd" == (method)) {
		wstringstream wss;;
		wss<<L"{\"value\":" << (long)g_hWnd << L"}";
		return  wss.str();
	} else if (L"getDetailRect" == (method)) {
		RECT	rect;
		GetClientRect(g_hWnd, &rect);
		wstringstream sb;
		sb << L"{x:" << rect.left;
		sb << L",y:" << rect.top;
		sb << L",w:" << rect.right;
		sb << L",h:" << rect.bottom;
		sb << L"}";
		return sb.str();
	} else if (L"browserClosed" == (method)) {
		// 
	}
	return L"";
}