using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace JWebTop {
    public static class JWebTopNative {
        #region 仅用于测试，且已测试通过的函数
        //// 按Unicode方式来回传递数据，最终没有发现问题（传回c#端后，需要把Unicode转为utf8）
        //[DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode, EntryPoint = "CovWString4")]
        //public static extern IntPtr CovWString直接返回字符串方式([MarshalAs(UnmanagedType.LPTStr)]string fa);
        
        //// 按Unicode方式来回传递数据，最终没有发现问题（传回c#端后，需要把Unicode转为utf8）
        //[DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode, EntryPoint = "CovWString5")]
        //public static extern void CovWString用StringBuilder方式(
        //    [MarshalAs(UnmanagedType.LPTStr)]StringBuilder outsb,
        //    [MarshalAs(UnmanagedType.LPTStr)]string param);

        //public static string CovWString4(string fa) {
        //    IntPtr ptr = CovWString直接返回字符串方式(fa);
        //    return ptr2string(ptr);
        //}

        //// 这种方式可以，但是有个问题，所有的字符都是gbk字符集，和JWebTop的Unicode完全不对应
        //[DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Ansi)]
        //public static extern StringBuilder CovString2([MarshalAs(UnmanagedType.LPStr)]StringBuilder fa);

        //[DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
        //public static extern int CovString(StringBuilder fa);

        //[DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
        //public static extern int CovWString2([MarshalAs(UnmanagedType.LPTStr)]StringBuilder fa);

        //[DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
        //public static extern int CovWString3([MarshalAs(UnmanagedType.LPTStr)]string v1, [MarshalAs(UnmanagedType.LPTStr)]string v2);
        //[DllImport("JWebTop_CSharp_DLL.dll")]
        //public static extern int Calc(int fa);
        #endregion
        #region  仅用于测试，且测试[未]通过的函数
        // 无论如何都无法返回LPTSTR(w_char)类型数据
        //[DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
        //[return: MarshalAs(UnmanagedType.LPTStr)]// 设置返回类型也不管用
        //public static extern string/*设置为string不行*/ CovWString([MarshalAs(UnmanagedType.LPTStr)]string fa);
        //public static extern StringBuilder/*设置为StringBuilder不行*/ CovWString([MarshalAs(UnmanagedType.LPTStr)]StringBuilder fa);
        #endregion
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
        private static string ptr2string(IntPtr ptr) {
            string s_unicode = Marshal.PtrToStringUni(ptr);
            DelPtrInDLL(ptr);// 调用dll，删除指定指针
            byte[] buffer = Encoding.Unicode.GetBytes(s_unicode);
            return Encoding.UTF8.GetString(buffer);
        }

        [DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
        private static extern int nCreateJWebTop([MarshalAs(UnmanagedType.LPTStr)]string processPath, [MarshalAs(UnmanagedType.LPTStr)]string configFile);
        [DllImport("JWebTop_CSharp_DLL.dll")]
        private static extern void nExitJWebTop();
        [DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
        private static extern long nCreateJWebTopBrowser([MarshalAs(UnmanagedType.LPTStr)]string jWebTopConfigJSON);
        [DllImport("JWebTop_CSharp_DLL.dll")]
        private static extern void nCloseJWebTopBrowser(long browserHWnd);
        [DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
        private static extern IntPtr nJWebTopExecuteJSWait(long browserHWnd, string script);
        [DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
        private static extern void nJWebTopExecuteJSNoWait(long browserHWnd, string script);
        [DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
        private static extern IntPtr nJWebTopExecuteJSONWait(long browserHWnd, string json);
        [DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
        private static extern void nJWebTopExecuteJSONNoWait(long browserHWnd, string json);

        private delegate string CSharpCallBack(long browserHWnd, string json);  // 声明回调函数原型        
        [DllImport("JWebTop_CSharp_DLL.dll", EntryPoint = "SetCSharpCallback")]
        private static extern void SetCSharpCallback(CSharpCallBack fa);        // 设置回调函数     
        
        #endregion

        private static JWebtopJSONDispater jsonHandler = null;
        private static bool inited = false;// 是否已经初始化

        /// <summary>
        /// 实现用于dll回调c#的回调函数
        /// </summary>
        /// <param name="browserHWnd">回调来自哪个浏览器</param>
        /// <param name="json">传递的json数据</param>
        /// <returns>执行结果</returns>
        private static string __CSharpCallBack(long browserHWnd, string json) {  // 实现回调函数
            jsonHandler.resetThreadClassLoader();
            return jsonHandler.dispatcher(browserHWnd, json);
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
        public static void createJWebTop(String processPath, String configFile) {
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

        public static long createBrowserByJSON(string jWebTopConfigJSON) {
            return nCreateJWebTopBrowser(jWebTopConfigJSON);
        }

        public static long createBrowserByAppFile(string appfile) {
            JWebTopConfigs config = new JWebTopConfigs();
            config.appDefFile = appfile;
            return createBrowser(config);
        }

        public static long createBrowserByAppFile(string appfile, long parenHwnd) {
            JWebTopConfigs config = new JWebTopConfigs();
            config.appDefFile = appfile;
            config.parentWin = parenHwnd;
            return createBrowser(config);
        }

        public static long createBrowser(JWebTopConfigs configs) {
            JObject jo = JObject.FromObject(configs);
            JWebTopConfigs.removeDefaults(jo);// 移除一些默认值属性
            return nCreateJWebTopBrowser(jo.ToString());
        }
        /**
         * 关闭指定浏览器
         * 
         * @param browserHWnd
         */
        public static void closeBrowser(long browserHWnd) {
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
        public static String executeJSON_Wait(long browserHWnd, String jsonstring) {
            return ptr2string(nJWebTopExecuteJSONWait(browserHWnd, jsonstring));
        }

        /**
         * 执行js，但不等待数据的返回。<br/>
         * 这里发送是包装的JSON数据，其调用JS脚本中的invokeByDLL(jsonvalue)方法
         * 
         * @param jsonstring
         */
        public static void executeJSON_NoWait(long browserHWnd, String jsonstring) {
            nJWebTopExecuteJSONNoWait(browserHWnd, jsonstring);
        }

        /**
         * 执行js，并等待数据的返回。<br/>
         * 推荐使用executeJSON_Wait方法，以减少字符串转换的问题
         * 
         * @param script
         * @return 返回是字符串数据
         */
        public static String executeJS_Wait(long browserHWnd, String script) {
            return ptr2string(nJWebTopExecuteJSWait(browserHWnd, script));
        }

        /**
         * 执行js，但不等待数据的返回。 <br/>
         * 推荐使用executeJSON_NoWait方法，以减少字符串转换的问题
         * 
         * @param script
         * @return
         */
        public static void executeJS_NoWait(long browserHWnd, String script) {
            nJWebTopExecuteJSNoWait(browserHWnd, script);
        }
        public const int SWP_NOMOVE = 0x2;
        public const int SWP_NOZORDER = 0x04;
        public const int HWND_TOPMOST = -1;
        public static void setSize(long browserHwnd, int w, int h) {
            if (browserHwnd != 0) {
                //nSetSize(browserHwnd, w, h);
                System.IntPtr hWnd = new IntPtr(browserHwnd);
                SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER);
            }
        }
    }
}
