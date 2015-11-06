package org.jwebtop;

import java.io.File;
import java.io.IOException;

import com.alibaba.fastjson.JSONObject;

/**
 * 与JWebTop中的JWebTopConfigs类对应<br>
 * 源码：https://github.com/washheart/jwebtop<br>
 * 说明：https://github.com/washheart/jwebtop/wiki<br>
 * 
 * @author washheart@163.com
 */
public class JWebTopConfigs {
	private final static int DEFAULT_INT = -1;

	private final static String[] checks = { "parentWin", "dwStyle", "dwExStyle"//
			, "x", "y", "w", "h", "max"//
			, "enableDebug", "enableResize", "disableRefresh", "enableDrag"//
			, "single_process", "persist_session_cookies", "log_severity"//
			, "ignore_certificate_errors", "remote_debugging_port" };

	// 移除不需要的配置：否则有appDefFile时会用类默认的配置替换appDefFile中的配置。另外移除配置可以减小json字符串长度
	public static void removeDefaults(JSONObject jo) {
		for (String key : checks) {
			if (jo.getIntValue(key) == DEFAULT_INT) jo.remove(key);
		}
	}

	// private long parentWin = DEFAULT_INT; // 记录父窗口的HWND
	private String parentWinS; // 记录父窗口的HWND

	// [BASE]小节：JWebTop相关配置，配置在[BASE]小节下
	/**
	 * 指定一个配置文件，所有配置信息从配置文件加载<br/>
	 * 注意：配置类中的配置项的优先级高于配置文件中的
	 */
	private String appDefFile; // 当前配置对应的配置文件（有可能为null）

	private String url; // 要打开的链接地址
	private String appendJs; // 通过此参数，可以附加一个js到当前页面（在OnLoad中通过<script type='text/javascript' src='appendJs.js'></script>)附加）
	private String name;// 窗口名称
	private String icon; // 窗口图标

	private String errorUrl; // 错误页面的地址(browser on error时使用）

	private String dwStyleS; // 附加窗口样式，默认(0)不应用任何附加窗口样式
	private String dwExStyleS; // 附加扩展窗口样式，默认(0)不应用任何附加扩展窗口样式

	private int x = DEFAULT_INT, y = DEFAULT_INT; // 窗口左上角坐标,当值为-1时不启用此变量
	private int w = DEFAULT_INT, h = DEFAULT_INT; // 窗口的宽、高，当值为-1时不启用此变量
	private int max = DEFAULT_INT; // 默认是否最大化

	private int enableDebug = DEFAULT_INT; // 是否允许调试，默认(false)不显示调试工具
	private int enableResize = DEFAULT_INT; // 是否可以调整窗口大小
	private int disableRefresh = DEFAULT_INT; // （尚未实现）是否禁止刷新
	private int enableDrag = DEFAULT_INT; // 是否通过页面进行拖动（默认可以）

	// [CEF]小节：cef配置参数（cef相关参数只在应用启动时起作用），配置在[CEF]小节下
	private int single_process = DEFAULT_INT; // 是否使用单进程模式：JWebTop默认使用。CEF默认不使用单进程模式
	private String user_data_path = null; // 用户数据保存目录
	private String cache_path = null; // 浏览器缓存数据的保存目录
	private int persist_session_cookies = DEFAULT_INT; // 是否需要持久化用户cookie数据（若要设置为true，需要同时指定cache_path）
	private String user_agent = null; // HTTP请求中的user_agent,CEF默认是Chorminum的user agent
	private String locale = null; // CEF默认是en-US
	private int log_severity = DEFAULT_INT; // 指定日志输出级别，取值[0,1,2,3,4,99]默认不输出(99)
	private String log_file; // 指定调试时的日志文件，默认为"debug.log"。如果关闭日志，则不输出日志
	private String resources_dir_path; // 指定cef资源文件（ cef.pak、devtools_resources.pak）的目录，默认从程序运行目录取
	private String locales_dir_path; // 指定cef本地化资源(locales)目录，默认去程序运行目录下的locales目录
	private int ignore_certificate_errors = DEFAULT_INT; // 是否忽略SSL证书错误
	private int remote_debugging_port = DEFAULT_INT; // 远程调试端口，取值范围[1024-65535]

	private String proxyServer; // 设置代理服务器地址。设置一个http代理服务器： 设置多个代理服务器：--proxy-server="https=proxy1:80;http=socks4://baz:1080"
	private String proxyAuthUser; // 登录代理服务器时需要的用户名（注意：不管proxyServer设置了多少代理服务器，这里暂时值支持一组用户名和密码）
	private String proxyAuthPwd; // 登录代理服务器时需要的密码

	public void setParentWin(long parentWin) {
		this.parentWinS = String.valueOf(parentWin);
	}

	public String getParentWinS() {
		return parentWinS;
	}

	public void setParentWinS(String parentWinS) {
		this.parentWinS = parentWinS;
	}

	public String getAppDefFile() {
		return appDefFile;
	}

	public void setAppDefFile(String appDefFile) {
		String appfile2;
		try {
			appfile2 = new File(appDefFile).getCanonicalPath();// 如果不是绝对路径，浏览器无法显示出来
		} catch (IOException e) {
			throw new JWebTopException("无法创建Browser", e);
		}
		this.appDefFile = appfile2;
	}

	public String getUrl() {
		return url;
	}

	public void setUrl(String url) {
		this.url = url;
	}

	public String getAppendJs() {
		return appendJs;
	}

	public void setAppendJs(String appendJs) {
		this.appendJs = appendJs;
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getIcon() {
		return icon;
	}

	public void setIcon(String icon) {
		this.icon = icon;
	}

	public String getErrorUrl() {
		return errorUrl;
	}

	public void setErrorUrl(String errorUrl) {
		this.errorUrl = errorUrl;
	}

	public void setDwStyle(long dwStyle) {
		this.dwStyleS = String.valueOf(dwStyle);
	}

	public void setDwExStyle(long dwExStyle) {
		this.dwExStyleS = String.valueOf(dwExStyle);
	}

	public String getDwStyleS() {
		return dwStyleS;
	}

	public void setDwStyleS(String dwStyleS) {
		this.dwStyleS = dwStyleS;
	}

	public String getDwExStyleS() {
		return dwExStyleS;
	}

	public void setDwExStyleS(String dwExStyleS) {
		this.dwExStyleS = dwExStyleS;
	}

	public int getX() {
		return x;
	}

	public void setX(int x) {
		this.x = x;
	}

	public int getY() {
		return y;
	}

	public void setY(int y) {
		this.y = y;
	}

	public int getW() {
		return w;
	}

	public void setW(int w) {
		this.w = w;
	}

	public int getH() {
		return h;
	}

	public void setH(int h) {
		this.h = h;
	}

	public int getSingle_process() {
		return single_process;
	}

	public void setSingle_process(int single_process) {
		this.single_process = single_process;
	}

	public String getUser_data_path() {
		return user_data_path;
	}

	public void setUser_data_path(String user_data_path) {
		this.user_data_path = user_data_path;
	}

	public String getCache_path() {
		return cache_path;
	}

	public int getMax() {
		return max;
	}

	public void setMax(int max) {
		this.max = max;
	}

	public int getEnableDebug() {
		return enableDebug;
	}

	public void setEnableDebug(int enableDebug) {
		this.enableDebug = enableDebug;
	}

	public int getEnableResize() {
		return enableResize;
	}

	public void setEnableResize(int enableResize) {
		this.enableResize = enableResize;
	}

	public int getDisableRefresh() {
		return disableRefresh;
	}

	public void setDisableRefresh(int disableRefresh) {
		this.disableRefresh = disableRefresh;
	}

	public int getEnableDrag() {
		return enableDrag;
	}

	public void setEnableDrag(int enableDrag) {
		this.enableDrag = enableDrag;
	}

	public void setCache_path(String cache_path) {
		this.cache_path = cache_path;
	}

	public int getPersist_session_cookies() {
		return persist_session_cookies;
	}

	public void setPersist_session_cookies(int persist_session_cookies) {
		this.persist_session_cookies = persist_session_cookies;
	}

	public String getUser_agent() {
		return user_agent;
	}

	public void setUser_agent(String user_agent) {
		this.user_agent = user_agent;
	}

	public String getLocale() {
		return locale;
	}

	public void setLocale(String locale) {
		this.locale = locale;
	}

	public int getLog_severity() {
		return log_severity;
	}

	public void setLog_severity(int log_severity) {
		this.log_severity = log_severity;
	}

	public String getLog_file() {
		return log_file;
	}

	public void setLog_file(String log_file) {
		this.log_file = log_file;
	}

	public String getResources_dir_path() {
		return resources_dir_path;
	}

	public void setResources_dir_path(String resources_dir_path) {
		this.resources_dir_path = resources_dir_path;
	}

	public String getLocales_dir_path() {
		return locales_dir_path;
	}

	public void setLocales_dir_path(String locales_dir_path) {
		this.locales_dir_path = locales_dir_path;
	}

	public int getIgnore_certificate_errors() {
		return ignore_certificate_errors;
	}

	public void setIgnore_certificate_errors(int ignore_certificate_errors) {
		this.ignore_certificate_errors = ignore_certificate_errors;
	}

	public int getRemote_debugging_port() {
		return remote_debugging_port;
	}

	public void setRemote_debugging_port(int remote_debugging_port) {
		this.remote_debugging_port = remote_debugging_port;
	}

	public String getProxyServer() {
		return proxyServer;
	}

	public void setProxyServer(String proxyServer) {
		this.proxyServer = proxyServer;
	}

	public String getProxyAuthUser() {
		return proxyAuthUser;
	}

	public void setProxyAuthUser(String proxyAuthUser) {
		this.proxyAuthUser = proxyAuthUser;
	}

	public String getProxyAuthPwd() {
		return proxyAuthPwd;
	}

	public void setProxyAuthPwd(String proxyAuthPwd) {
		this.proxyAuthPwd = proxyAuthPwd;
	}
	// /// 以下cef参数暂不支持配置
	// String product_version; // 如果指定了user_agent，此参数会被忽略,CEF默认是Chorminum的版本号
	// int pack_loading_disabled;// 禁止从resources_dir_path和locales_dir_path读取数据，转为从CefApp::GetResourceBundleHandler()获取
	// String browser_subprocess_path;
	// int multi_threaded_message_loop;
	// int windowless_rendering_enabled;
	// int command_line_args_disabled;
	// cef_string_t javascript_flags;
	// int uncaught_exception_stack_size;// 捕获未知异常时的堆栈深度。指定为0时(默认值)，不会触发OnUncaughtException()
	// int context_safety_implementation;
	// cef_color_t background_color;
	// cef_string_t accept_language_list;// HTTP头"Accept-Language"，以逗号分隔多个与语言列表，默认为 "en-US,en"

}
