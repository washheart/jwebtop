#ifndef CEF_JWEBTOP_CONFIGS_H_
#define CEF_JWEBTOP_CONFIGS_H_
#include <string>
#include "include/cef_client.h"
#include "include/internal/cef_types.h"
class
	JWebTopConfigs{
public:
	// [BASE]小节：JWebTop相关配置，配置在[BASE]小节下
	//DWORD dwStyle = WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	//DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
	DWORD dwStyle = 0;             // 附加窗口样式，默认(0)不应用任何附加窗口样式
	DWORD dwExStyle = 0;           // 附加扩展窗口样式，默认(0)不应用任何附加扩展窗口样式

	int x = -1, y = -1;            // 窗口左上角坐标,当值为-1时不启用此变量		 
	int w = -1, h = -1;            // 窗口的宽、高，当值为-1时不启用此变量		
	int max;                       // 默认是否最大化

	bool enableDebug;              // 是否允许调试，默认(false)不显示调试工具
	bool enableResize;             // 是否可以调整窗口大小
	bool disableRefresh;           // （尚未实现）是否禁止刷新
	bool enableDrag;               // 是否通过页面进行拖动（默认可以）

	CefString appDefFile;          // 当前配置对应的配置文件（有可能为null）
	CefString appPath;             // 当前配置文件所在目录（不能为null）
	CefString url=CefString("file:///index.html");   // 要打开的链接地址
	CefString appendJs;			   // 通过此参数，可以附加一个js到当前页面（在OnLoad中通过<script type='text/javascript' src='appendJs.js'></script>)附加）
	CefString name;                // 窗口名称
	CefString icon;                // 窗口图标

	// [CEF]小节：cef配置参数（cef相关参数只在应用启动时起作用），配置在[CEF]小节下
	int single_process = 1;                                 // 是否使用单进程模式：JWebTop默认使用。CEF默认不使用单进程模式
	CefString user_data_path = CefString(L"data/jwebtop");  // 用户数据保存目录
	CefString cache_path = CefString(L"data/jwebtop");      // 浏览器缓存数据的保存目录
	int persist_session_cookies;                            // 是否需要持久化用户cookie数据（若要设置为true，需要同时指定cache_path）
	CefString user_agent = CefString(L"JWebTop2.0");        // HTTP请求中的user_agent,CEF默认是Chorminum的user agent
	CefString locale = CefString(L"zh-CN");                 // CEF默认是en-US
	int log_severity = LOGSEVERITY_DISABLE;                 // 指定日志输出级别，取值[0,1,2,3,4,99]默认不输出(99)
	CefString log_file;                                     // 指定调试时的日志文件，默认为"debug.log"。如果关闭日志，则不输出日志
	CefString resources_dir_path;                           // 指定cef资源文件（ cef.pak、devtools_resources.pak）的目录，默认从程序运行目录取
	CefString locales_dir_path;                             // 指定cef本地化资源(locales)目录，默认去程序运行目录下的locales目录
	int ignore_certificate_errors;                          // 是否忽略SSL证书错误
	int remote_debugging_port;                              // 远程调试端口，取值范围[1024-65535]

	CefString proxyServer;									// 设置代理服务器地址。设置一个http代理服务器：  设置多个代理服务器：--proxy-server="https=proxy1:80;http=socks4://baz:1080"
	CefString proxyAuthUser;								// 登录代理服务器时需要的用户名（注意：不管proxyServer设置了多少代理服务器，这里暂时值支持一组用户名和密码）
	CefString proxyAuthPwd;									// 登录代理服务器时需要的密码

	int no_sandbox = 1;                                     // 是否使用沙盒模式：JWebTop默认不使用。CEF默认使用。此参数在JWebTop中不可以配置为使用，否则会造成部分JWebTop JS不能使用

	
	///// 以下cef参数暂不支持配置
	//CefString product_version;                 // 如果指定了user_agent，此参数会被忽略,CEF默认是Chorminum的版本号
	//int pack_loading_disabled;// 禁止从resources_dir_path和locales_dir_path读取数据，转为从CefApp::GetResourceBundleHandler()获取
	//CefString browser_subprocess_path;
	//int multi_threaded_message_loop;
	//int windowless_rendering_enabled;
	//int command_line_args_disabled;
	//cef_string_t javascript_flags;
	//int uncaught_exception_stack_size;// 捕获未知异常时的堆栈深度。指定为0时(默认值)，不会触发OnUncaughtException() 
	//int context_safety_implementation;
	//cef_color_t background_color;
	//cef_string_t accept_language_list;// HTTP头"Accept-Language"，以逗号分隔多个与语言列表，默认为 "en-US,en"

	long parentWin = 0;   // 记录父窗口的HWND

	JWebTopConfigs(){}
	~JWebTopConfigs(){}

	// 根据传入的参数获取配置文件路径(目前主要用于处理传入参数为NULL或空字符串的情况)
	static std::wstring JWebTopConfigs::getAppDefFile(LPCTSTR lpCmdLine);
	// 从数据定义文件读取
	static JWebTopConfigs * loadConfigs(std::wstring appDefFile);

	// 根据相对路径获取绝对路径
	// 如果relativePath已经是绝对路径，则直接返回
	// 否则将返回appPath+relativePath
	CefString getAbsolutePath(std::wstring relativePath);

	static void JWebTopConfigs::setErrorURL(std::wstring url);
	static std::wstring JWebTopConfigs::getErrorURL();

	// 按JSON字符串格式，生成JWebTopConfigs
	static JWebTopConfigs * JWebTopConfigs::loadAsJSON(std::wstring jsonString);
};


#endif