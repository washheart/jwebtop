#ifndef CEF_JWEBTOP_JJH_WINDOWS_H_
#define CEF_JWEBTOP_JJH_WINDOWS_H_

#include <sstream>
#include <string>
#include "include/cef_app.h" 
#include "JWebTopJSHanlder.h"
#include "JWebTop/winctrl/JWebTopWinCtrl.h"

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
		jw::setBound(getHWND(object, arguments, 4), arguments[1]->GetIntValue(), arguments[2]->GetIntValue(), arguments[3]->GetIntValue(), arguments[4]->GetIntValue());
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
		retval->SetValue("width", CefV8Value::CreateInt(rt.right), V8_PROPERTY_ATTRIBUTE_NONE);
		retval->SetValue("height", CefV8Value::CreateInt(rt.bottom), V8_PROPERTY_ATTRIBUTE_NONE);
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
		jw::close(getHWND(object, arguments, 0));
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
		jw::max(getHWND(object, arguments, 0));
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
//setWindowStyle(exStyle, handler);//�߼����������ô��ڶ������ԣ������ö�֮�ࡣ
class JJH_SetWindowStyle : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		if (arguments.size() < 1)return false;
		jw::setWindowStyle(getHWND(object, arguments, 1), arguments[0]->GetIntValue());
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_SetWindowStyle);
};


//loadUrl(url, handler);//������ҳ��urlΪ��ҳ·��
class JJH_LoadUrl : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		if (arguments.size() < 1)return false;
		jw::loadUrl(getHWND(object, arguments, 1), arguments[0]->GetStringValue().ToWString());
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_LoadUrl);
};
//reload(handler);//���¼��ص�ǰҳ��
class JJH_Reload : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jw::reload(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_Reload);
};

//reloadIgnoreCache(handler);//���¼��ص�ǰҳ�沢���Ի���
class JJH_ReloadIgnoreCache : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jw::reloadIgnoreCache(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_ReloadIgnoreCache);
};
//showDev(handler);//�򿪿����߹���
class JJH_ShowDev : public CefV8Handler {
public:
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) {
		jw::showDev(getHWND(object, arguments, 0));
		return true;
	}
private:
	IMPLEMENT_REFCOUNTING(JJH_ShowDev);
};


#endif