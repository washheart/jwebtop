#ifndef jwebtop_multi_process_jwebtop_msg_H_
#define jwebtop_multi_process_jwebtop_msg_H_

#define __JWM        100    // 定义一个消息起始值

#define JWM_IPC_CLIENT_OK		(__JWM+101)  // IPC对应的客户端已成功创建
#define JWM_SET_ERR_URL			(__JWM+102)  // 设置错误页面

#define JWM_STARTJWEBTOP		(__JWM+301)  // 启动JWebTop进程
#define JWM_CFGJWEBTOP_FILE		(__JWM+302)  // 
//#define JWM_CFGJWEBTOP_JSON		(__JWM+303)  // 
#define JWM_CEF_APP_INITED		(__JWM+304)  // CEF浏览器已初始完成
#define JWM_CEF_ExitAPP			(__JWM+305)  // 关闭CEF浏览器进程

#define JWM_CREATEBROWSER_JSON	(__JWM+401)  // 创建浏览器的消息		（同步发送）
#define JWM_CREATEBROWSER_FILE	(__JWM+402)  // 创建浏览器的消息		（同步发送）
#define JWM_BROWSER_CREATED		(__JWM+403)  // CEF浏览器已初始完成
#define JWM_CLOSEBROWSER		(__JWM+404)  // 关闭浏览器的消息		（异步发送）

#define JWM_JS_EXECUTE_WAIT		(__JWM+201)  // DLL调用CEF端：需要执行并等待的JS任务 
#define JWM_JS_EXECUTE_RETURN	(__JWM+202)  // 
#define JWM_JSON_EXECUTE_WAIT	(__JWM+211)  // DLL调用CEF端：需要执行并等待的JSON任务 
#define JWM_JSON_EXECUTE_RETURN	(__JWM+212)  // 
#define JWM_DLL_EXECUTE_WAIT	(__JWM+221)  // CEF调用DLL端：需要执行并等待的任务 
#define JWM_DLL_EXECUTE_RETURN	(__JWM+222)  // CEF调用DLL端：需要执行但CEF无需等待的任务 
#define JWM_RESULT_RETURN	    (__JWM+231)  

#define JWM_M_SETURL			(__JWM+501)  // 设置浏览器的URL（通过JS方式设置时，可能会有JS正在阻塞的问题）
#define JWM_M_APPEND_JS			(__JWM+502)  // 为浏览器附加一些JS代码，附加的代码在页面的生命周期内有效

// B2R=Browser to Renderer
#define JWM_B2R_SERVERINFO		(__JWM+601)  // 给render进程发送FastIPC服务端参数
#define JWM_B2R_TASKRESULT		(__JWM+602)  // 给render进程发送远程任务执行结果

#endif