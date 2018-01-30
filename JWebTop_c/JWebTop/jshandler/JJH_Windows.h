#ifndef CEF_JWEBTOP_JJH_WINDOWS_H_
#define CEF_JWEBTOP_JJH_WINDOWS_H_

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

#include "common/tests/TestUtil.h"

#include "common/util/StrUtil.h"

// JJH=JWebTop JavaScriptHandler

//getPos(handler);//��ô���λ�ã�����ֵΪһobject����ʽ����{x:13,y:54}
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

//setSize(x, y, handler);//���ô��ڴ�С
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
//getSize(handler);//��ô��ڴ�С������ֵΪһobject����ʽ����{width:130,height:54}
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
//getScreenSize();//��ȡ��Ļ��С������ֵΪһobject����ʽ����{width:130,height:54}
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
//move(x, y, handler);//�ƶ�����
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
//setBound(x, y,w ,h, handler);// ͬʱ���ô��ڵ�����ʹ�С
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
//getBound(handler);//��ȡ���ڵ�λ�úʹ�С������ֵΪһobject����ʽ����{x:100,y:100,width:130,height:54}
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
//setTitle(title, handler);// ���ô�������
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
//getTitle(handler);// ��ȡ�������ƣ�����ֵΪһ�ַ���
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
//close(handler);// �رմ���
class JJH_Close : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jb::close(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_Close);
};
// bringToTop(handler);//�����Ƶ����
class JJH_BringToTop : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jw::bringToTop(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_BringToTop);
};
// focus(handler);//ʹ���ڻ�ý���
class JJH_Focus : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jw::focus(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_Focus);
};

//hide(handler);//���ش���
class JJH_Hide : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jw::hide(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_Hide);
};
//max(handler);//��󻯴���
class JJH_Max : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jw::maxWin(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_Max);
};
//mini(hander);//��С������
class JJH_Mini : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jw::mini(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_Mini);
};
//restore(handler);//��ԭ���ڣ���Ӧ��hide����
class JJH_Restore : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jw::restore(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_Restore);
};
//setTopMost(handler);//�����ö����˺�����bringToTop���������ڴ˺�����ʹ������Զ�ö�������������һ�����ڵ������ö�����
class JJH_SetTopMost : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jw::setTopMost(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_SetTopMost);
};
//setWindowStyle(style, handler);//�߼����������ô��ڶ������ԣ������ö�֮�ࡣ
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
//setWindowStyle(exStyle, handler);//�߼����������ô��ڶ������ԣ������ö�֮�ࡣ
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

//loadUrl(url, handler);//������ҳ��urlΪ��ҳ·��
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
//reload(handler);//���¼��ص�ǰҳ��
class JJH_Reload : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jb::reload(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_Reload);
};

//reloadIgnoreCache(handler);//���¼��ص�ǰҳ�沢���Ի���
class JJH_ReloadIgnoreCache : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jb::reloadIgnoreCache(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_ReloadIgnoreCache);
};
//showDev(handler);//�򿪿����߹���
class JJH_ShowDev : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jb::showDev(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_ShowDev);
};

//invokeRemote_CallBack(jsonstring,callback,[handler]);// ��JS����Զ�̽��̴���
class JJH_InvokeRemote_CallBack : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		if (arguments.size() < 2){
			exception = (L"invokeRemote_CallBack(jsonstring,callback,[handler])��Ҫ������������");
			return true;
		}
		if (!arguments[1]->IsString()){
			exception = (L"�ڶ��������������ַ���");
			return true;
		}
		string callback = arguments[1]->GetStringValue().ToString();
		int size = callback.size();
		if (size == 0){
			exception = (L"�ڶ������������ǿ��ַ���");
			return true;
		}
		if (size > 100){
			exception = (L"�ڶ����������Ȳ��ܳ���100");
			return true;
		}
		jw::dllex::invokeRemote_CallBack(getHWND(object, arguments, 2), arguments[0]->GetStringValue(), callback);
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_InvokeRemote_CallBack);
};

//JJH_InvokeRemote_NoWait(jsonstring,[handler]);// ��JS����Զ�̽��̴���
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

//JJH_enableDrag(enable);// ��������϶�
class JJH_enableDrag : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jb::enableDrag(getHWND(object, arguments, 10000), arguments[0]->GetBoolValue());
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_enableDrag);
};

//JJH_startDrag();// ��ʼ�����϶�
class JJH_startDrag : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jb::startDrag(getHWND(object, arguments, 10000));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_startDrag);
};
//JJH_stopDrag([handler]);// ֹͣ�϶�
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
//JJH_TestFunInJSON({params}, [handler]);// ���Իص�ͨ��JSON��ʽ�����JS������
//�÷���JWebTop.selectFile({params}, [handler])
//params={
// str:	һ���ַ���
// fun:	��Ҫ���лص���JavaScript����
//}
//����ֵ����
//*/
//class JJH_TestFunInJSON : public CefV8Handler {
//public:
//	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
//		if (arguments.size() < 1) return false;
//		CefRefPtr<CefV8Value> params = arguments[0];
//		CefString str = params->GetValue("str")->GetStringValue();
//		std::wstring ss = str.ToWString();
//		std::reverse(ss.begin(), ss.end());
//		// �����ص�
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
////invokeReflect(function(v){});// ���Իص�js��function
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
			// ����ҳ��׼�����¼���new CustomEvent('JWebTopReady')
			void sendReadey(const CefRefPtr<CefFrame> frame);

			// ҳ���ڵ���ҳ�棨iframe��׼�����¼���new CustomEvent('JWebTopIFrameReady')
			void sendIFrameReady(const CefRefPtr<CefFrame> frame);

			// ���ʹ��ڴ�С�ı��¼�:new CustomEvent('JWebTopResize',{detail:{w:�����ֵ,h:�߶���ֵ}})
			void sendSize(const CefRefPtr<CefFrame> frame, const int w, const int h);

			// ���ʹ���λ�øı��¼�:new CustomEvent('JWebTopMove',{detail:{x:X����ֵ,y:Y����ֵ}})
			void sendMove(const CefRefPtr<CefFrame> frame, const int x, const int y);

			// ���ʹ��ڱ������¼�:new CustomEvent('JWebTopWindowActive',{detail:{handler:������Ĵ��ڵľ��}})
			void sendWinowActive(const CefRefPtr<CefFrame> frame, const long handler, const DWORD state);

			// ����Ӧ�ã�һ��Ӧ�ÿ����ж�����ڣ��������¼�:new CustomEvent('JWebTopAppActive',{detail:{handler:���Ǵ���Ϣ�Ĵ��ڵľ��}})
			void sendAppActive(const CefRefPtr<CefFrame> frame, const long handler);

			// �������ִ�����ļ�������ճ��(JWebTopFilePasted)�������ļ�(JWebTopFileDroped)
			void sendFileEvent(const CefRefPtr<CefFrame> frame, vector<CefString> files, std::string eventName);
			
			// ����ҳ�����ʧ���¼�
			void sendLoadError(const CefRefPtr<CefFrame> frame, int errorCode, const CefString& errorText, const CefString& failedUrl);
		}
	}
}
#endif