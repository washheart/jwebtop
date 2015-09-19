using System;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Newtonsoft.Json.Linq;
using System.Diagnostics;
 
namespace JWebTop {
    public static class JWebTopNative {
        #region 其他windows dll方法
        [DllImport("User32.dll")]
        private static extern int SetWindowPos(IntPtr hwnd, int hWndInsertAfter, int x, int y, int cx, int cy, int wFlags);

        #endregion
        #region JWebTopDLL方法引入

        // 删除dll中定义的指针，防止内存泄漏
        [DllImport("JWebTop_CSharp_DLL.dll")]
        private static extern void DelPtrInDLL(IntPtr p);
        /// <summary>
        ///  将dll传来的字符数据从指针转为字符串，执行：
        ///  1、从指针读取数据
        ///  2、删除指针
        ///  3、转为utf-8格式
        /// </summary>
        /// <param name="ptr"></param>
        /// <returns></returns>
        private static string ptr2stringWithDel(IntPtr ptr) {
            string s_unicode = Marshal.PtrToStringUni(ptr);
            DelPtrInDLL(ptr);// 调用dll，删除指定指针
            byte[] buffer = Encoding.Unicode.GetBytes(s_unicode);
            string rtn = Encoding.UTF8.GetString(buffer);
            int idx = rtn.IndexOf((char)0);
            if (idx >= 0) rtn = rtn.Substring(0, idx);
            return rtn;
        }
        private static string ptr2stringNoDel(IntPtr ptr) {
            string s_unicode = Marshal.PtrToStringUni(ptr);
            //DelPtrInDLL(ptr);// 调用dll，删除指定指针
            byte[] buffer = Encoding.Unicode.GetBytes(s_unicode);
            string rtn = Encoding.UTF8.GetString(buffer);
            int idx = rtn.IndexOf((char)0);
            if (idx >= 0) rtn = rtn.Substring(0, idx);
            return rtn;
        }
        [DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
        private static extern int nCreateJWebTop([MarshalAs(UnmanagedType.LPTStr)]string processPath, [MarshalAs(UnmanagedType.LPTStr)]string configFile);
        [DllImport("JWebTop_CSharp_DLL.dll")]
        private static extern void nExitJWebTop();
        [DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
        private static extern int nCreateJWebTopBrowser([MarshalAs(UnmanagedType.LPTStr)]string jWebTopConfigJSON);
        [DllImport("JWebTop_CSharp_DLL.dll")]
        private static extern void nCloseJWebTopBrowser(int browserHWnd);
        [DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
        private static extern IntPtr nJWebTopExecuteJSWait(int browserHWnd, string script);
        [DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
        private static extern void nJWebTopExecuteJSNoWait(int browserHWnd, string script);
        [DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
        private static extern IntPtr nJWebTopExecuteJSONWait(int browserHWnd, string json);
        [DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
        private static extern void nJWebTopExecuteJSONNoWait(int browserHWnd, string json);

        private delegate StringBuilder CSharpCallBack(int browserHWnd, IntPtr json);  // 声明回调函数原型        
        [DllImport("JWebTop_CSharp_DLL.dll", EntryPoint = "SetCSharpCallback")]
        private static extern void SetCSharpCallback(CSharpCallBack callback);        // 设置回调函数     

        #endregion

        private static JWebtopJSONDispater jsonHandler = null;
        private static bool inited = false;// 是否已经初始化

        /// <summary>
        /// 实现用于dll回调c#的回调函数
        /// </summary>
        /// <param name="browserHWnd">回调来自哪个浏览器</param>
        /// <param name="json">传递的json数据</param>
        /// <returns>执行结果</returns>
        ///  
        private static StringBuilder __CSharpCallBack(int browserHWnd, IntPtr jsonPtr) {  // 实现回调函数
            jsonHandler.resetThreadClassLoader();
            string json = ptr2stringNoDel(jsonPtr);
            return new StringBuilder(jsonHandler.dispatcher(browserHWnd, json));
        }
        private static void check() {
            if (inited) return;
            SetCSharpCallback(__CSharpCallBack);
        }
        /// <summary>
        /// 设置一个json分发器，用于处理浏览器相关逻辑
        /// </summary>
        /// <param name="_jsonHandler"></param>
        public static void setJsonHandler(JWebtopJSONDispater _jsonHandler) {
            jsonHandler = _jsonHandler;
        }

        /// <summary>
        ///     启动JWebTop进程，不立即创建浏览器。<br/>
        ///    此方法仅在第一次调用时创建进程，再次调用不执行任何操作
        /// </summary>
        /// <param name="processPath">待执行的JWebTop.exe的全路径。比如d:\c\JWebTop.exe，当然JWebTop可以命名为其他名字。</param>
        /// <param name="configFile">此配置文件用于启动JWebTop进程（但，不立刻创建浏览器窗口，即使指定了url参数）</param>
        public static void createJWebTop(string processPath, string configFile) {
            check();
            if (jsonHandler == null) throw new JWebTopException("必须设置jsonHandler。");
            string processPath2, appfile2;
            try {
                if (!File.Exists(processPath)) throw new JWebTopException("找不到可执行文件：" + processPath);
                processPath2 = Path.GetFullPath(processPath);// 如果不是绝对路径，浏览器无法显示出来
                appfile2 = Path.GetFullPath(configFile);// 如果不是绝对路径，浏览器无法显示出来
                nCreateJWebTop(processPath2, appfile2);
            } catch (IOException e) {
                throw new JWebTopException("无法创建JWebTop", e);
            }
        }

        public static int createBrowserByJSON(string jWebTopConfigJSON) {
            return nCreateJWebTopBrowser(jWebTopConfigJSON);
        }

        public static int createBrowserByAppFile(string appfile) {
            JWebTopConfigs config = new JWebTopConfigs();
            config.appDefFile = appfile;
            return createBrowser(config);
        }

        public static int createBrowserByAppFile(string appfile, long parenHwnd) {
            Trace.WriteLine("createBrowserByAppFile appfile=" + appfile);
            JWebTopConfigs config = new JWebTopConfigs();
            config.appDefFile = appfile;
            config.parentWin = parenHwnd;
            return createBrowser(config);
        }

        public static int createBrowser(JWebTopConfigs configs) {
            JObject jo = JObject.FromObject(configs);
            JWebTopConfigs.removeDefaults(jo);// 移除一些默认值属性
            return nCreateJWebTopBrowser(jo.ToString());
        }
        /**
         * 关闭指定浏览器
         * 
         * @param browserHWnd
         */
        public static void closeBrowser(int browserHWnd) {
            nCloseJWebTopBrowser(browserHWnd);
        }

        // 关闭JWebTop的进程
        public static void exitJWebTop() {
            nExitJWebTop();
        }
        /**
         * 执行js，发送后等待数据的返回。<br/>
         * 这里发送是包装的JSON数据，其调用JS脚本中的invokeByDLL(jsonvalue)方法
         * 
         * @param jsonstring
         * @return 返回是JSON字符串数据
         */
        public static string executeJSON_Wait(int browserHWnd, string jsonstring) {
            return ptr2stringWithDel(nJWebTopExecuteJSONWait(browserHWnd, jsonstring));
        }

        /**
         * 执行js，但不等待数据的返回。<br/>
         * 这里发送是包装的JSON数据，其调用JS脚本中的invokeByDLL(jsonvalue)方法
         * 
         * @param jsonstring
         */
        public static void executeJSON_NoWait(int browserHWnd, string jsonstring) {
            nJWebTopExecuteJSONNoWait(browserHWnd, jsonstring);
        }

        /**
         * 执行js，并等待数据的返回。<br/>
         * 推荐使用executeJSON_Wait方法，以减少字符串转换的问题
         * 
         * @param script
         * @return 返回是字符串数据
         */
        public static string executeJS_Wait(int browserHWnd, string script) {
            return ptr2stringWithDel(nJWebTopExecuteJSWait(browserHWnd, script));
        }

        /**
         * 执行js，但不等待数据的返回。 <br/>
         * 推荐使用executeJSON_NoWait方法，以减少字符串转换的问题
         * 
         * @param script
         * @return
         */
        public static void executeJS_NoWait(int browserHWnd, string script) {
            nJWebTopExecuteJSNoWait(browserHWnd, script);
        }
        public const int SWP_NOMOVE = 0x2;
        public const int SWP_NOZORDER = 0x04;
        public const int HWND_TOPMOST = -1;
        public static void setSize(int browserHwnd, int w, int h) {
            if (browserHwnd != 0) {
                //nSetSize(browserHwnd, w, h);
                System.IntPtr hWnd = new IntPtr(browserHwnd);
                SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER);
            }
        }
    }
}
