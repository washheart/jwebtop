using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace JWebTop
{
    //public static class NativeMethod
    //{
    //    [DllImport("kernel32.dll", EntryPoint = "LoadLibrary")]
    //    public static extern int LoadLibrary(
    //        [MarshalAs(UnmanagedType.LPStr)] string lpLibFileName);

    //    [DllImport("kernel32.dll", EntryPoint = "GetProcAddress")]
    //    public static extern IntPtr GetProcAddress(int hModule,
    //        [MarshalAs(UnmanagedType.LPStr)] string lpProcName);

    //    [DllImport("kernel32.dll", EntryPoint = "FreeLibrary")]
    //    public static extern bool FreeLibrary(int hModule);
    //}

    public static class JWebTopNative
    {
        [DllImport("User32.dll")]
        public static extern int SetWindowPos(IntPtr hwnd, int hWndInsertAfter, int x, int y, int cx, int cy, int wFlags);
    /*
	 * 该方法用于创建一个浏览器窗口
	 * processPath 待执行的JWebTop.exe的全路径。比如d:\c\JWebTop.exe，当然JWebTop可以命名为其他名字。
	 * configFile  此配置文件用于启动JWebTop进程（但，不立刻创建浏览器窗口，即使指定了url参数）
	 *
	 * return 内被用于通信的隐藏窗口的句柄
	 */
    [DllImport("JWebTop_CSharp_DLL.dll", EntryPoint = "CreateJWebTop")]
    public static extern long  nCreateJWebTop(string processPath, string configFile);

	/*
	 * 退出JWebTop进程
	 */
    [DllImport("JWebTop_CSharp_DLL.dll", EntryPoint = "ExitJWebTop")]
    public static extern void nExitJWebTop();

	/*
	* 该方法用于创建一个浏览器窗口
	* jWebTopConfigJSON 浏览器配置信息JSON(JWebTopConfigs.h)
	*
	* return 返回创建的浏览器窗口的句柄
	*/
    [DllImport("JWebTop_CSharp_DLL.dll", EntryPoint = "CreateJWebTopBrowser")]
	public static extern long  nCreateJWebTopBrowser(string jWebTopConfigJSON);

	/*
	 * 该方法用于关闭一个浏览器窗口
	 * browserHWnd  浏览器窗口句柄
	 */
    [DllImport("JWebTop_CSharp_DLL.dll", EntryPoint = "CloseJWebTopBrowser")]
	 public static extern void  nCloseJWebTopBrowser(long browserHWnd);

	/*
	 * 下面四个方法用于执行浏览器脚本
	 */
    [DllImport("JWebTop_CSharp_DLL.dll", EntryPoint = "JWebTopExecuteJSWait")]
	public static extern string  nJWebTopExecuteJSWait(long browserHWnd, string script);
    [DllImport("JWebTop_CSharp_DLL.dll", EntryPoint = "JWebTopExecuteJSNoWait")]
	 public static extern void  nJWebTopExecuteJSNoWait(long browserHWnd, string script);
    [DllImport("JWebTop_CSharp_DLL.dll", EntryPoint = "JWebTopExecuteJSONWait")]
	public static extern string  nJWebTopExecuteJSONWait(long browserHWnd, string json);
    [DllImport("JWebTop_CSharp_DLL.dll", EntryPoint = "JWebTopExecuteJSONNoWait")]
	public static extern void  nJWebTopExecuteJSONNoWait(long browserHWnd, string json);

    public delegate string CSharpCallBack(long browserHWnd, string json);
    //设置回调函数
    [DllImport("JWebTop_CSharp_DLL.dll", EntryPoint = "SetCSharpCallback")]
    public static extern void SetCSharpCallback(CSharpCallBack fa);
    // 声明回调的函数
    public static string __CSharpCallBack(long browserHWnd, string json) {
        //MessageBox.Show(buf);
        return "abcdefg";
    }
	private static JWebtopJSONDispater jsonHandler = null;
    private static bool inited = false;// 是否已经初始化
        private static void check()
        {
            if (inited) return;
            SetCSharpCallback(__CSharpCallBack);
        }
        public static void setJsonHandler(JWebtopJSONDispater _jsonHandler) {
		jsonHandler = _jsonHandler;
	}
        	static String dispatch(long browserHWnd, String json)  {
		jsonHandler.resetThreadClassLoader();
		return jsonHandler.dispatcher(browserHWnd, json);
	}
/**
	 * 启动JWebTop进程，不立即创建浏览器。<br/>
	 * 此方法仅在第一次调用时创建进程，再次调用不执行任何操作
	 * 
	 * @param processPath
	 *            待执行的JWebTop.exe的全路径。比如d:\c\JWebTop.exe，当然JWebTop可以命名为其他名字。
	 * @param configFile
	 *            此配置文件用于启动JWebTop进程（但，不立刻创建浏览器窗口，即使指定了url参数）
	 * @return
	 */
	public static void createJWebTop(String processPath, String configFile) {
        check();
		if (jsonHandler == null) throw new JWebTopException("必须设置jsonHandler。");
		string processPath2, appfile2;
		try {
            //commonlassLoader = JWebTopNative.class.getClassLoader();
            //File file = new File(processPath);
			if (!File.Exists(processPath)) throw new JWebTopException("找不到可执行文件：" + processPath);
			processPath2 = Path.GetFullPath(processPath);// 如果不是绝对路径，浏览器无法显示出来
			appfile2 = Path.GetFullPath(configFile);// 如果不是绝对路径，浏览器无法显示出来
			nCreateJWebTop(processPath2, appfile2);
		} catch (IOException e) {
			throw new JWebTopException("无法创建JWebTop", e);
		}
	}

	public static long createBrowserByJSON(string jWebTopConfigJSON)  {
        return nCreateJWebTopBrowser(jWebTopConfigJSON);
	}

	public static long createBrowserByAppFile(string appfile){
		JWebTopConfigs config = new JWebTopConfigs();
		config.appDefFile=appfile;
		return createBrowser(config);
	}

	public static long createBrowserByAppFile(string appfile,  long parenHwnd) {
		JWebTopConfigs config = new JWebTopConfigs();
		config.appDefFile=appfile;
		config.parentWin=parenHwnd;
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
        return nJWebTopExecuteJSONWait(browserHWnd, jsonstring);
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
        return nJWebTopExecuteJSWait(browserHWnd, script);
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
