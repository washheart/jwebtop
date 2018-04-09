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
			// 发送页面准备好事件：new CustomEvent('JWebTopReady')
			void sendReadey(const CefRefPtr<CefFrame> frame){
				CefString js_event(L"var e = new CustomEvent('JWebTopReady');dispatchEvent(e);");
				frame->ExecuteJavaScript(js_event, "", 0);
			}
			// 页面内的子页面（iframe）准备好事件：new CustomEvent('JWebTopIFrameReady')
			void sendIFrameReady(const CefRefPtr<CefFrame> frame){
				CefString js_event(L"var e = new CustomEvent('JWebTopIFrameReady');dispatchEvent(e);");
				frame->ExecuteJavaScript(js_event, "", 0);
			}
			// 发送窗口大小改变事件:new CustomEvent('JWebTopResize',{detail:{w:宽度数值,h:高度数值}})
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

			// 发送窗口位置改变事件:new CustomEvent('JWebTopMove',{detail:{x:X坐标值,y:Y坐标值}})
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

			// 发送窗口被激活事件:new CustomEvent('JWebTopWindowActive',{detail:{handler:被激活的窗口的句柄}})
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

			// 发送应用（一个应用可能有多个窗口）被激活事件:new CustomEvent('JWebTopAppActive',{detail:{handler:触发此消息的窗口的句柄}})
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
			// 发送页面加载失败事件
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

			// 发送下载事件
			void sendDownload(const CefRefPtr<CefFrame> frame,
				const int state, const  uint32 id, const int percentComplete, const  int64 totalBytes, const int64 receivedBytes, const int64 currentSpeed,
				const CefString& url, const CefString& originalUrl, const CefString& fullPath, const CefString& suggestedFileName, const CefString& mimeType,
				const CefString& contentDisposition, const CefTime startTime, const CefTime endTime){
				stringstream js_event;
				js_event << "var e = new CustomEvent('JWebTopDownLoad',{"
					<< "	detail:{"
					<< "		contentDisposition:\"" << contentDisposition.ToString() << "\","  // MIME协议扩展，由服务器端指定下载时的文件名称
					<< "		mimeType:\"" << mimeType.ToString() << "\","                      // MIME
					<< "		suggestedFileName:\"" << suggestedFileName.ToString() << "\","    // 下载时的建议名称
					<< "		fullPath:\"" << fullPath.ToString() << "\","                      // 返回正在下载的文件的全路径
					<< "		originalUrl:\"" << originalUrl.ToString() << "\","                // 在任何重定向之前的初始链接
					<< "		url:\"" << url.ToString() << "\","                                // 正在被下载的链接
					<< "		startTime:" << startTime.GetTimeT() << ","                        // 下载开始时间
					<< "		endTime:" << endTime.GetTimeT() << ","                            // 下载结束时间
					<< "		currentSpeed:" << currentSpeed << ","                             // 返回一个下载速度（评估）
					<< "		receivedBytes:" << receivedBytes << ","                           // 已下载的文件大小
					<< "		totalBytes:" << totalBytes << ","                                 // 下载文件总大小
					<< "		percentComplete:" << percentComplete << ","                       // 下载进度
					<< "		id:" << id << ","                                                 // 针对当前下载的一个唯一性标识符
					<< "		state:" << state                                                  // 当前下载状态：1=下载中；2=下载完；3=取消下载；返回其他值为错误
					<< "	}"
					<< "});"
					<< "dispatchEvent(e);";
				frame->ExecuteJavaScript(js_event.str(), "", 0);
			}


		}// End ns-events
	}// End ns-js
}// End ns-jw

// 获取函数对应的窗口句柄：默认从最后一个参数获取，如果没有知道最后一个参数，从object的handler中取
HWND getHWND(CefRefPtr<CefV8Value> object/*JS对象*/, const CefV8ValueList& arguments/*函数参数列表*/
	, CefV8ValueList::size_type lastIdx/*最后一个参数的地址*/){
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

extern CefSettings settings;              // CEF全局设置
void regist(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context){

	CefRefPtr<CefV8Value> object = context->GetGlobal();
	// 尝试添加自定义对象
	CefRefPtr<CefV8Accessor> accessor;// 有必要的话，可以扩展该类
	CefRefPtr<CefV8Value> jWebTop = object->CreateObject(accessor);
	object->SetValue("JWebTop", jWebTop, V8_PROPERTY_ATTRIBUTE_NONE);// 把创建的对象附加到V8根对象上

	// 
	regist(jWebTop, "getPos", new JJH_GetPos());//getPos(handler);//获得窗口位置，返回值为一object，格式如下{x:13,y:54}
	regist(jWebTop, "setSize", new JJH_SetSize());//setSize(x, y, handler);//设置窗口大小
	regist(jWebTop, "getSize", new JJH_GetSize());//getSize(handler);//获得窗口大小，返回值为一object，格式如下{width:130,height:54}
	regist(jWebTop, "getScreenSize", new JJH_GetScreenSize());//getScreenSize();//获取屏幕大小，返回值为一object，格式如下{width:130,height:54}
	regist(jWebTop, "move", new JJH_Move());//move(x, y, handler);//移动窗口
	regist(jWebTop, "setBound", new JJH_SetBound());//setBound(x, y,w ,h, handler);// 同时设置窗口的坐标和大小
	regist(jWebTop, "getBound", new JJH_GetBound());//getBound(handler);//获取窗口的位置和大小，返回值为一object，格式如下{x:100,y:100,width:130,height:54}
	regist(jWebTop, "setTitle", new JJH_SetTitle());//setTitle(title, handler);// 设置窗口名称
	regist(jWebTop, "getTitle", new JJH_GetTitle());//getTitle(handler);// 获取窗口名称，返回值为一字符串

	regist(jWebTop, "bringToTop", new JJH_BringToTop());  // bringToTop(handler);//窗口移到最顶层
	regist(jWebTop, "focus", new JJH_Focus());      	  // focus(handler);//使窗口获得焦点
	regist(jWebTop, "hide", new JJH_Hide());	          // hide(handler);//隐藏窗口
	regist(jWebTop, "max", new JJH_Max());	              // max(handler);//最大化窗口
	regist(jWebTop, "mini", new JJH_Mini());	          // mini(hander);//最小化窗口
	regist(jWebTop, "restore", new JJH_Restore());	      // restore(handler);//还原窗口，对应于hide函数
	regist(jWebTop, "setTopMost", new JJH_SetTopMost());  // setTopMost(handler);//窗口置顶，此函数跟bringToTop的区别在于此函数会使窗口永远置顶，除非有另外一个窗口调用了置顶函数
	regist(jWebTop, "setWindowStyle", new JJH_SetWindowStyle());	//setWindowStyle(exStyle, handler);//高级函数，设置窗口额外属性，诸如置顶之类。
	regist(jWebTop, "setWindowExStyle", new JJH_SetWindowExStyle());

	regist(jWebTop, "invokeRemote_CallBack", new JJH_InvokeRemote_CallBack());// 容易阻塞render进程，屏蔽
	regist(jWebTop, "invokeRemote_NoWait", new JJH_InvokeRemote_NoWait());   
	//regist(jWebTop, "testFunInJSON", new JJH_TestFunInJSON());


	CefRefPtr<CefV8Value> db = object->CreateObject(accessor);
	jWebTop->SetValue("db", db, V8_PROPERTY_ATTRIBUTE_NONE);// 把创建的对象附加到JWebTop根对象上
	CefRefPtr<CefV8Value> dbType = object->CreateObject(accessor);
	// db的数据类型
	db->SetValue("type", dbType, V8_PROPERTY_ATTRIBUTE_NONE);// 把创建的对象附加到JWebTop.db根对象上
	dbType->SetValue("SQLITE_INTEGER", CefV8Value::CreateInt(SQLITE_INTEGER), V8_PROPERTY_ATTRIBUTE_NONE);
	dbType->SetValue("SQLITE_FLOAT", CefV8Value::CreateInt(SQLITE_FLOAT), V8_PROPERTY_ATTRIBUTE_NONE);
	dbType->SetValue("SQLITE_BLOB", CefV8Value::CreateInt(SQLITE_BLOB), V8_PROPERTY_ATTRIBUTE_NONE);
	dbType->SetValue("SQLITE_NULL", CefV8Value::CreateInt(SQLITE_NULL), V8_PROPERTY_ATTRIBUTE_NONE);
	dbType->SetValue("SQLITE_TEXT", CefV8Value::CreateInt(SQLITE_TEXT), V8_PROPERTY_ATTRIBUTE_NONE);
	// db的常用函数
	regist(db, "open", new jw::db::JJH_DB_open());
	regist(db, "close", new jw::db::JJH_DB_close());
	regist(db, "exec", new jw::db::JJH_DB_exec());
	regist(db, "queryCallback", new jw::db::JJH_DB_queryCallbcak());
	regist(db, "queryDataSet", new jw::db::JJH_DB_queryDataSet());
	regist(db, "batch", new jw::db::JJH_DB_batch());
	
	CefRefPtr<CefV8Value> file = object->CreateObject(accessor);
	jWebTop->SetValue("file", file, V8_PROPERTY_ATTRIBUTE_NONE);// 把创建的对象附加到JWebTop根对象上
	regist(file, "selectFile", new JJH_SelectFile());
	regist(file, "rename", new JJH_RenameFiles());
	//regist(jWebTop, "invokeReflect", new JJH_invokeReflect());//测试JS回调
	// 单进程模式下，才可以根据HWND直接获取BrowerWindowInfo
	// 多进程模式要通过消息传递数据，参见JWebTopClient#OnLoadEnd（具体实现是JWebTopCommons#renderBrowserWindow）
	if (settings.single_process){
		regist(jWebTop, "close", new JJH_Close());            // close(handler);// 关闭窗口
		regist(jWebTop, "loadUrl", new JJH_LoadUrl());	      //loadUrl(url, handler);//加载网页，url为网页路径
		regist(jWebTop, "reload", new JJH_Reload());	      //reload(handler);//重新加载当前页面
		regist(jWebTop, "reloadIgnoreCache", new JJH_ReloadIgnoreCache());	//reloadIgnoreCache(handler);//重新加载当前页面并忽略缓存
		regist(jWebTop, "showDev", new JJH_ShowDev());	      //showDev(handler);//打开开发者工具
		regist(jWebTop, "enableDrag", new JJH_enableDrag());  // enableDrag(true|false);
		regist(jWebTop, "startDrag", new JJH_startDrag());	  // startDrag();
		regist(jWebTop, "stopDrag", new JJH_stopDrag());	  // stopDrag();
	}
	// 以下方法只在实现JWebTopClient#OnLoadEnd实现（具体实现是JWebTopCommons#renderBrowserWindow）
	//regist(jWebTop, "runApp", new JJH_RunApp());  //runApp(appName,handler);//运行一个app，appName为.app文件路径。
}