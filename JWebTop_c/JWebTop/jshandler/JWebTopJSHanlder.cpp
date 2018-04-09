#include "JWebTopJSHanlder.h"
#include "include/cef_app.h"
#include "include/cef_parser.h"
#include "include/cef_load_handler.h"
#include "JJH_Windows.h"
#include "JJH_Files.h"
#include "common/util/StrUtil.h"
#include <iostream>  
#include <fstream>  
#include "JWebTop/browser/JWebTopScheme.h"
#include "JWebTop/sqlite/ex_sqlite.h"
#include "common/tests/TestUtil.h"
namespace jw{
	namespace js{
		namespace events{
			// ����ҳ��׼�����¼���new CustomEvent('JWebTopReady')
			void sendReadey(const CefRefPtr<CefFrame> frame){
				CefString js_event(L"var e = new CustomEvent('JWebTopReady');dispatchEvent(e);");
				frame->ExecuteJavaScript(js_event, "", 0);
			}
			// ҳ���ڵ���ҳ�棨iframe��׼�����¼���new CustomEvent('JWebTopIFrameReady')
			void sendIFrameReady(const CefRefPtr<CefFrame> frame){
				CefString js_event(L"var e = new CustomEvent('JWebTopIFrameReady');dispatchEvent(e);");
				frame->ExecuteJavaScript(js_event, "", 0);
			}
			// ���ʹ��ڴ�С�ı��¼�:new CustomEvent('JWebTopResize',{detail:{w:�����ֵ,h:�߶���ֵ}})
			void sendSize(const CefRefPtr<CefFrame> frame, const int w, const int h){
				stringstream js_event;
				js_event << "var e = new CustomEvent('JWebTopResize',{"
					<< "	detail:{"
					<< "		width:" << w << ","
					<< "		height:" << h
					<< "	}"
					<< "});"
					<< "dispatchEvent(e);";
				frame->ExecuteJavaScript(js_event.str(), "", 0);
			}

			// ���ʹ���λ�øı��¼�:new CustomEvent('JWebTopMove',{detail:{x:X����ֵ,y:Y����ֵ}})
			void sendMove(const CefRefPtr<CefFrame> frame, const int x, const int y){
				stringstream js_event;
				js_event << "var e = new CustomEvent('JWebTopMove',{"
					<< "	detail:{"
					<< "		x:" << x << ","
					<< "		y:" << y
					<< "	}"
					<< "});"
					<< "dispatchEvent(e);";
				frame->ExecuteJavaScript(js_event.str(), "", 0);
			}

			// ���ʹ��ڱ������¼�:new CustomEvent('JWebTopWindowActive',{detail:{handler:������Ĵ��ڵľ��}})
			void sendWinowActive(const CefRefPtr<CefFrame> frame, const long handler, const DWORD state){
				stringstream js_event;
				js_event << "var e = new CustomEvent('JWebTopWindowActive',{"
					<< "	detail:{"
					<< "		handler:" << handler << ","
					<< "		state:" << state
					<< "	}"
					<< "});"
					<< "dispatchEvent(e);";
				frame->ExecuteJavaScript(js_event.str(), "", 0);
			}

			// ����Ӧ�ã�һ��Ӧ�ÿ����ж�����ڣ��������¼�:new CustomEvent('JWebTopAppActive',{detail:{handler:��������Ϣ�Ĵ��ڵľ��}})
			void sendAppActive(const CefRefPtr<CefFrame> frame, const long handler){
				stringstream js_event;
				js_event << "var e = new CustomEvent('JWebTopAppActive',{"
					<< "	detail:{"
					<< "		handler:" << handler
					<< "	}"
					<< "});"
					<< "dispatchEvent(e);";
				frame->ExecuteJavaScript(js_event.str(), "", 0);
			}
			string & convertFilePath(string & p){
				p = jw::str::replace_all(p, "\\", "/");
				return jw::str::replace_all(p, "\"", "\\\"");
			}
			void sendFileEvent(const CefRefPtr<CefFrame> frame, vector<CefString> files, const std::string eventName){
				if (files.size() == 0)return;
				stringstream js_event;
				vector<CefString>::iterator it = files.begin();
				string tmp = (*it).ToString();
				js_event << "var e = new CustomEvent('" << eventName << "',{"
					<< "	detail:{"
					<< "		data:[\"" << convertFilePath(tmp) << "\"";
				it++;
				while (it != files.end()){
					tmp = (*it).ToString();
					js_event << ",\"" << convertFilePath(tmp) << "\"";
					it++;
				}
				js_event << "]}"
					<< "});"
					<< "dispatchEvent(e);";
				frame->ExecuteJavaScript(js_event.str(), "", 0);
			}
			// ����ҳ�����ʧ���¼�
			void sendLoadError(const CefRefPtr<CefFrame> frame,
				int errorCode, const CefString& errorText, const CefString& failedUrl){
				stringstream js_event;
				js_event << "var e = new CustomEvent('JWebTopLoadError',{"
					<< "	detail:{"
					<< "		url:\"" << failedUrl.ToString() << "\","
					<< "		msg:\"" << errorText.ToString() << "\","
					<< "		code:"  << errorCode
					<< "	}"
					<< "});"
					<< "dispatchEvent(e);";
				frame->ExecuteJavaScript(js_event.str(), "", 0);
			}

			// ���������¼�
			void sendDownload(const CefRefPtr<CefFrame> frame,
				const int state, const  uint32 id, const int percentComplete, const  int64 totalBytes, const int64 receivedBytes, const int64 currentSpeed,
				const CefString& url, const CefString& originalUrl, const CefString& fullPath, const CefString& suggestedFileName, const CefString& mimeType,
				const CefString& contentDisposition, const CefTime startTime, const CefTime endTime){
				stringstream js_event;
				js_event << "var e = new CustomEvent('JWebTopDownLoad',{"
					<< "	detail:{"
					<< "		contentDisposition:\"" << contentDisposition.ToString() << "\","  // MIMEЭ����չ���ɷ�������ָ������ʱ���ļ�����
					<< "		mimeType:\"" << mimeType.ToString() << "\","                      // MIME
					<< "		suggestedFileName:\"" << suggestedFileName.ToString() << "\","    // ����ʱ�Ľ�������
					<< "		fullPath:\"" << fullPath.ToString() << "\","                      // �����������ص��ļ���ȫ·��
					<< "		originalUrl:\"" << originalUrl.ToString() << "\","                // ���κ��ض���֮ǰ�ĳ�ʼ����
					<< "		url:\"" << url.ToString() << "\","                                // ���ڱ����ص�����
					<< "		startTime:" << startTime.GetTimeT() << ","                        // ���ؿ�ʼʱ��
					<< "		endTime:" << endTime.GetTimeT() << ","                            // ���ؽ���ʱ��
					<< "		currentSpeed:" << currentSpeed << ","                             // ����һ�������ٶȣ�������
					<< "		receivedBytes:" << receivedBytes << ","                           // �����ص��ļ���С
					<< "		totalBytes:" << totalBytes << ","                                 // �����ļ��ܴ�С
					<< "		percentComplete:" << percentComplete << ","                       // ���ؽ���
					<< "		id:" << id << ","                                                 // ��Ե�ǰ���ص�һ��Ψһ�Ա�ʶ��
					<< "		state:" << state                                                  // ��ǰ����״̬��1=�����У�2=�����ꣻ3=ȡ�����أ���������ֵΪ����
					<< "	}"
					<< "});"
					<< "dispatchEvent(e);";
				frame->ExecuteJavaScript(js_event.str(), "", 0);
			}


		}// End ns-events
	}// End ns-js
}// End ns-jw

// ��ȡ������Ӧ�Ĵ��ھ����Ĭ�ϴ����һ��������ȡ�����û��֪�����һ����������object��handler��ȡ
HWND getHWND(CefRefPtr<CefV8Value> object/*JS����*/, const CefV8ValueList& arguments/*���������б�*/
	, CefV8ValueList::size_type lastIdx/*���һ�������ĵ�ַ*/){
	LONG hWnd = 0;
	if (arguments.size() > lastIdx){
		hWnd = static_cast<LONG>(arguments[lastIdx]->GetIntValue());
	}
	else{
		CefRefPtr<CefV8Value> v = object->GetValue("handler");
		hWnd = v->GetIntValue();
	}
	return HWND(hWnd);
}

void regist(const CefRefPtr<CefV8Value> jWebTop, const CefString& fn, CefRefPtr<CefV8Handler> handler){
	jWebTop->SetValue(fn, CefV8Value::CreateFunction(fn, handler), V8_PROPERTY_ATTRIBUTE_NONE);
}

extern CefSettings settings;              // CEFȫ������
void regist(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context){

	CefRefPtr<CefV8Value> object = context->GetGlobal();
	// ��������Զ������
	CefRefPtr<CefV8Accessor> accessor;// �б�Ҫ�Ļ���������չ����
	CefRefPtr<CefV8Value> jWebTop = object->CreateObject(accessor);
	object->SetValue("JWebTop", jWebTop, V8_PROPERTY_ATTRIBUTE_NONE);// �Ѵ����Ķ��󸽼ӵ�V8��������

	// 
	regist(jWebTop, "getPos", new JJH_GetPos());//getPos(handler);//��ô���λ�ã�����ֵΪһobject����ʽ����{x:13,y:54}
	regist(jWebTop, "setSize", new JJH_SetSize());//setSize(x, y, handler);//���ô��ڴ�С
	regist(jWebTop, "getSize", new JJH_GetSize());//getSize(handler);//��ô��ڴ�С������ֵΪһobject����ʽ����{width:130,height:54}
	regist(jWebTop, "getScreenSize", new JJH_GetScreenSize());//getScreenSize();//��ȡ��Ļ��С������ֵΪһobject����ʽ����{width:130,height:54}
	regist(jWebTop, "move", new JJH_Move());//move(x, y, handler);//�ƶ�����
	regist(jWebTop, "setBound", new JJH_SetBound());//setBound(x, y,w ,h, handler);// ͬʱ���ô��ڵ�����ʹ�С
	regist(jWebTop, "getBound", new JJH_GetBound());//getBound(handler);//��ȡ���ڵ�λ�úʹ�С������ֵΪһobject����ʽ����{x:100,y:100,width:130,height:54}
	regist(jWebTop, "setTitle", new JJH_SetTitle());//setTitle(title, handler);// ���ô�������
	regist(jWebTop, "getTitle", new JJH_GetTitle());//getTitle(handler);// ��ȡ�������ƣ�����ֵΪһ�ַ���

	regist(jWebTop, "bringToTop", new JJH_BringToTop());  // bringToTop(handler);//�����Ƶ����
	regist(jWebTop, "focus", new JJH_Focus());      	  // focus(handler);//ʹ���ڻ�ý���
	regist(jWebTop, "hide", new JJH_Hide());	          // hide(handler);//���ش���
	regist(jWebTop, "max", new JJH_Max());	              // max(handler);//��󻯴���
	regist(jWebTop, "mini", new JJH_Mini());	          // mini(hander);//��С������
	regist(jWebTop, "restore", new JJH_Restore());	      // restore(handler);//��ԭ���ڣ���Ӧ��hide����
	regist(jWebTop, "setTopMost", new JJH_SetTopMost());  // setTopMost(handler);//�����ö����˺�����bringToTop���������ڴ˺�����ʹ������Զ�ö�������������һ�����ڵ������ö�����
	regist(jWebTop, "setWindowStyle", new JJH_SetWindowStyle());	//setWindowStyle(exStyle, handler);//�߼����������ô��ڶ������ԣ������ö�֮�ࡣ
	regist(jWebTop, "setWindowExStyle", new JJH_SetWindowExStyle());

	regist(jWebTop, "invokeRemote_CallBack", new JJH_InvokeRemote_CallBack());// ��������render���̣�����
	regist(jWebTop, "invokeRemote_NoWait", new JJH_InvokeRemote_NoWait());   
	//regist(jWebTop, "testFunInJSON", new JJH_TestFunInJSON());


	CefRefPtr<CefV8Value> db = object->CreateObject(accessor);
	jWebTop->SetValue("db", db, V8_PROPERTY_ATTRIBUTE_NONE);// �Ѵ����Ķ��󸽼ӵ�JWebTop��������
	CefRefPtr<CefV8Value> dbType = object->CreateObject(accessor);
	// db����������
	db->SetValue("type", dbType, V8_PROPERTY_ATTRIBUTE_NONE);// �Ѵ����Ķ��󸽼ӵ�JWebTop.db��������
	dbType->SetValue("SQLITE_INTEGER", CefV8Value::CreateInt(SQLITE_INTEGER), V8_PROPERTY_ATTRIBUTE_NONE);
	dbType->SetValue("SQLITE_FLOAT", CefV8Value::CreateInt(SQLITE_FLOAT), V8_PROPERTY_ATTRIBUTE_NONE);
	dbType->SetValue("SQLITE_BLOB", CefV8Value::CreateInt(SQLITE_BLOB), V8_PROPERTY_ATTRIBUTE_NONE);
	dbType->SetValue("SQLITE_NULL", CefV8Value::CreateInt(SQLITE_NULL), V8_PROPERTY_ATTRIBUTE_NONE);
	dbType->SetValue("SQLITE_TEXT", CefV8Value::CreateInt(SQLITE_TEXT), V8_PROPERTY_ATTRIBUTE_NONE);
	// db�ĳ��ú���
	regist(db, "open", new jw::db::JJH_DB_open());
	regist(db, "close", new jw::db::JJH_DB_close());
	regist(db, "exec", new jw::db::JJH_DB_exec());
	regist(db, "queryCallback", new jw::db::JJH_DB_queryCallbcak());
	regist(db, "queryDataSet", new jw::db::JJH_DB_queryDataSet());
	regist(db, "batch", new jw::db::JJH_DB_batch());
	
	CefRefPtr<CefV8Value> file = object->CreateObject(accessor);
	jWebTop->SetValue("file", file, V8_PROPERTY_ATTRIBUTE_NONE);// �Ѵ����Ķ��󸽼ӵ�JWebTop��������
	regist(file, "selectFile", new JJH_SelectFile());
	regist(file, "rename", new JJH_RenameFiles());
	//regist(jWebTop, "invokeReflect", new JJH_invokeReflect());//����JS�ص�
	// ������ģʽ�£��ſ��Ը���HWNDֱ�ӻ�ȡBrowerWindowInfo
	// �����ģʽҪͨ����Ϣ�������ݣ��μ�JWebTopClient#OnLoadEnd������ʵ����JWebTopCommons#renderBrowserWindow��
	if (settings.single_process){
		regist(jWebTop, "close", new JJH_Close());            // close(handler);// �رմ���
		regist(jWebTop, "loadUrl", new JJH_LoadUrl());	      //loadUrl(url, handler);//������ҳ��urlΪ��ҳ·��
		regist(jWebTop, "reload", new JJH_Reload());	      //reload(handler);//���¼��ص�ǰҳ��
		regist(jWebTop, "reloadIgnoreCache", new JJH_ReloadIgnoreCache());	//reloadIgnoreCache(handler);//���¼��ص�ǰҳ�沢���Ի���
		regist(jWebTop, "showDev", new JJH_ShowDev());	      //showDev(handler);//�򿪿����߹���
		regist(jWebTop, "enableDrag", new JJH_enableDrag());  // enableDrag(true|false);
		regist(jWebTop, "startDrag", new JJH_startDrag());	  // startDrag();
		regist(jWebTop, "stopDrag", new JJH_stopDrag());	  // stopDrag();
	}
	// ���·���ֻ��ʵ��JWebTopClient#OnLoadEndʵ�֣�����ʵ����JWebTopCommons#renderBrowserWindow��
	//regist(jWebTop, "runApp", new JJH_RunApp());  //runApp(appName,handler);//����һ��app��appNameΪ.app�ļ�·����
}