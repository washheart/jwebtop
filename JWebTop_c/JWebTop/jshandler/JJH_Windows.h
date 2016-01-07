#ifndef CEF_JWEBTOP_JJH_WINDOWS_H_
#define CEF_JWEBTOP_JJH_WINDOWS_H_

#include <sstream>
#include <string>
#include "include/cef_app.h" 
#include "JWebTopJSHanlder.h"
#include "common/winctrl/JWebTopWinCtrl.h"
#include "JWebTop/dllex/JWebTop_DLLEx.h"
#include "JWebTop/wndproc/JWebTopWndProc.h"

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
			}
	}
}
#endif