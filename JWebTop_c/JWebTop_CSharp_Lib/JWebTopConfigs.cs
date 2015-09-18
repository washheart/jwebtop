using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace JWebTop {
    public class JWebTopConfigs {
        private const int DEFAULT_INT = -1;

        // 移除不需要的配置：否则有appDefFile时会用类默认的配置替换appDefFile中的配置。另外移除配置可以减小json字符串长度
        public static void removeDefaults(JObject jo) {
            List<KeyValuePair<string, JToken>> tokens = jo.ToList<KeyValuePair<string, JToken>>();
            foreach (KeyValuePair<string, JToken> entry in tokens) {
                JTokenType t = entry.Value.Type;
                if (t == JTokenType.Null) {
                    jo.Remove(entry.Key); continue;// 移除值为null的数据（字符串）
                }
                if (entry.Value.Type == JTokenType.Integer
                    && (((int)entry.Value) == DEFAULT_INT)) {
                    jo.Remove(entry.Key); continue;// 移除值=-1的数据（数值）
                }
            }
        }

        private string _parentWinS; // 记录父窗口的HWND

        // [BASE]小节：JWebTop相关配置，配置在[BASE]小节下
        /**
         * 指定一个配置文件，所有配置信息从配置文件加载<br/>
         * 注意：配置类中的配置项的优先级高于配置文件中的
         */
        private string _appDefFile; // 当前配置对应的配置文件（有可能为null）

        private string _url; // 要打开的链接地址
        private string _appendJs; // 通过此参数，可以附加一个js到当前页面（在OnLoad中通过<script type='text/javascript' src='appendJs.js'></script>)附加）
        private string _name;// 窗口名称
        private string _icon; // 窗口图标

        private string _dwStyleS; // 附加窗口样式，默认(0)不应用任何附加窗口样式
        private string _dwExStyleS; // 附加扩展窗口样式，默认(0)不应用任何附加扩展窗口样式

        private int _x = DEFAULT_INT, _y = DEFAULT_INT; // 窗口左上角坐标,当值为-1时不启用此变量
        private int _w = DEFAULT_INT, _h = DEFAULT_INT; // 窗口的宽、高，当值为-1时不启用此变量
        private int _max = DEFAULT_INT; // 默认是否最大化

        private int _enableDebug = DEFAULT_INT; // 是否允许调试，默认(false)不显示调试工具
        private int _enableResize = DEFAULT_INT; // 是否可以调整窗口大小
        private int _disableRefresh = DEFAULT_INT; // （尚未实现）是否禁止刷新
        private int _enableDrag = DEFAULT_INT; // 是否通过页面进行拖动（默认可以）

        // [CEF]小节：cef配置参数（cef相关参数只在应用启动时起作用），配置在[CEF]小节下
        private int _single_process = DEFAULT_INT; // 是否使用单进程模式：JWebTop默认使用。CEF默认不使用单进程模式
        private string _user_data_path = null; // 用户数据保存目录
        private string _cache_path = null; // 浏览器缓存数据的保存目录
        private int _persist_session_cookies = DEFAULT_INT; // 是否需要持久化用户cookie数据（若要设置为true，需要同时指定cache_path）
        private string _user_agent = null; // HTTP请求中的user_agent,CEF默认是Chorminum的user agent
        private string _locale = null; // CEF默认是en-US
        private int _log_severity = DEFAULT_INT; // 指定日志输出级别，取值[0,1,2,3,4,99]默认不输出(99)
        private string _log_file; // 指定调试时的日志文件，默认为"debug.log"。如果关闭日志，则不输出日志
        private string _resources_dir_path; // 指定cef资源文件（ cef.pak、devtools_resources.pak）的目录，默认从程序运行目录取
        private string _locales_dir_path; // 指定cef本地化资源(locales)目录，默认去程序运行目录下的locales目录
        private int _ignore_certificate_errors = DEFAULT_INT; // 是否忽略SSL证书错误
        private int _remote_debugging_port = DEFAULT_INT; // 远程调试端口，取值范围[1024-65535]

        private string _proxyServer; // 设置代理服务器地址。设置一个http代理服务器： 设置多个代理服务器：--proxy-server="https=proxy1:80;http=socks4://baz:1080"
        private string _proxyAuthUser; // 登录代理服务器时需要的用户名（注意：不管proxyServer设置了多少代理服务器，这里暂时值支持一组用户名和密码）
        private string _proxyAuthPwd; // 登录代理服务器时需要的密码

        public string parentWinS {
            get { return _parentWinS; }
            set { this._parentWinS = value; }
        }
        public long parentWin {
            set { this._parentWinS = value.ToString(); }
        }
        public string appDefFile {
            get { return _appDefFile; }
            set { this._appDefFile = System.IO.Path.GetFullPath(value); }
        }

        public string url {
            get { return _url; }
            set { this._url = value; }
        }

        public string appendJs {
            get { return _appendJs; }
            set { this._appendJs = value; }
        }

        public string name {
            get { return _name; }
            set { this._name = value; }
        }

        public string icon {
            get { return _icon; }
            set { this._icon = value; }
        }

        public string dwStyleS {
            get { return _dwStyleS; }
            set { this._dwStyleS = value; }
        }
        public long dwStyle {
            set { this._dwStyleS = value.ToString(); }
        }
        public string dwExStyleS {
            get { return _dwExStyleS; }
            set { this._dwExStyleS = value; }
        }

        public long dwExStyle {
            set { this._dwExStyleS = value.ToString(); }
        }
        public int x {
            get { return _x; }
            set { this._x = value; }
        }
        public int y {
            get { return _y; }
            set { this._y = value; }
        }
        public int w {
            get { return _w; }
            set { this._w = value; }
        }
        public int h {
            get { return _h; }
            set { this._h = value; }
        }
        public int single_process {
            get { return _single_process; }
            set { this._single_process = value; }
        }

        public string user_data_path {
            get { return _user_data_path; }
            set { this._user_data_path = value; }
        }
        public string cache_path {
            get { return _cache_path; }
            set { this._cache_path = value; }
        }
        public int max {
            get { return _max; }
            set { this._max = value; }
        }

        public int enableDebug {
            get { return _enableDebug; }
            set { this._enableDebug = value; }
        }

        public int enableResize {
            get { return _enableResize; }
            set { this._enableResize = value; }
        }

        public int disableRefresh {
            get { return _disableRefresh; }
            set { this._disableRefresh = value; }
        }
        public int enableDrag {
            get { return _enableDrag; }
            set { this._enableDrag = value; }
        }

        public int persist_session_cookies {
            get { return _persist_session_cookies; }
            set { this._persist_session_cookies = value; }
        }
        public string user_agent {
            get { return _user_agent; }
            set { this._user_agent = value; }
        }

        public string locale {
            get { return _locale; }
            set { this._locale = value; }
        }

        public int log_severity {
            get { return _log_severity; }
            set { this._log_severity = value; }
        }

        public string log_file {
            get { return _log_file; }
            set { this._log_file = value; }
        }

        public string resources_dir_path {
            get { return _resources_dir_path; }
            set { this._resources_dir_path = value; }
        }
        public string locales_dir_path {
            get { return _locales_dir_path; }
            set { this._locales_dir_path = value; }
        }
        public int ignore_certificate_errors {
            get { return _ignore_certificate_errors; }
            set { this._ignore_certificate_errors = value; }
        }

        public int remote_debugging_port {
            get { return _remote_debugging_port; }
            set { this._remote_debugging_port = value; }
        }
        public string proxyServer {
            get { return _proxyServer; }
            set { this._proxyServer = value; }
        }

        public string proxyAuthUser {
            get { return _proxyAuthUser; }
            set { this._proxyAuthUser = value; }
        }

        public string proxyAuthPwd {
            get { return _proxyAuthPwd; }
            set { this._proxyAuthPwd = value; }
        }
    }
}
