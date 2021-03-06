﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using org.fastipc;
using System.IO;
using System.Threading;
using Newtonsoft.Json.Linq;
/**
 * 创建JWebTop浏览器，一般一个程序只需要一个JWebTopContext。一个JWebTopContext可以创建多个浏览器。<br>
 * 源码：https://github.com/washheart/jwebtop<br>
 * 说明：https://github.com/washheart/jwebtop/wiki<br>
 * 
 * @author washheart@163.com
 */
namespace JWebTop {
   public class JWebTopContext : RebuildedBlockListener {

        private const int//
                __JWM = 100, // 定义MPMSG_MINI和MPMSG_LARGE中msgId的起始值
                JWM_IPC_CLIENT_OK = __JWM + 101, // IPC对应的客户端已成功创建
                JWM_SET_ERR_URL = __JWM + 102, // 设置错误页面
            //JWM_SET_TASK_WAIT_TIME = (__JWM + 103), // 执行等待任务时的最大等待时间（毫秒）
            // JWM_STARTJWEBTOP = __JWM + 301, // 启动JWebTop进程
            // JWM_CFGJWEBTOP_FILE = __JWM + 302, //
            // JWM_CFGJWEBTOP_JSON = __JWM + 303, //
                JWM_CEF_APP_INITED = __JWM + 304, // CEF浏览器已初始完成
                JWM_CEF_ExitAPP = __JWM + 305, // 关闭CEF浏览器进程

                JWM_CREATEBROWSER_JSON = __JWM + 401, // 创建浏览器的消息 （同步发送）
                JWM_CREATEBROWSER_FILE = __JWM + 402, // 创建浏览器的消息 （同步发送）
                JWM_BROWSER_CREATED = __JWM + 403, // CEF浏览器已初始完成
                JWM_CLOSEBROWSER = __JWM + 404, // 关闭浏览器的消息 （异步发送）

                JWM_M_SETURL = __JWM + 501, // 设置浏览器的URL（通过JS方式设置时，可能会有JS正在阻塞的问题）
                JWM_M_APPEND_JS = __JWM + 502, // 为浏览器附加一些JS代码，附加的代码在页面的生命周期内有效

                JWM_JS_EXECUTE_WAIT = __JWM + 201, // DLL调用CEF端：需要执行并等待的JS任务
                JWM_JS_EXECUTE_RETURN = __JWM + 202, //
                JWM_JSON_EXECUTE_WAIT = __JWM + 211, // DLL调用CEF端：需要执行并等待的JSON任务
                JWM_JSON_EXECUTE_RETURN = __JWM + 212, //
                JWM_DLL_EXECUTE_WAIT = __JWM + 221, // CEF调用DLL端：需要执行并等待的任务
                JWM_DLL_EXECUTE_RETURN = __JWM + 222, // CEF调用DLL端：需要执行但CEF无需等待的任务
                JWM_RESULT_RETURN = __JWM + 231;
        // public static long WIN_HWND = 0;
        private Dictionary<String, JWebTopBrowserCreated> browserCreateListeners = new Dictionary<String, JWebTopBrowserCreated>();
        private Dictionary<String, JWebTopJSReturn> jsReturnListeners = new Dictionary<String, JWebTopJSReturn>();
        private Dictionary<String, Stream> FastIPCReceivedCaches = new Dictionary<String, Stream>();
        private FastIPCServer server = null;
        private FastIPCClient client = null;
        private String serverName;
        private int blockSize;

        private JWebtopJSONDispater jsonHandler = null;
        private JWebTopAppInited appInitListener;

        public JWebTopContext() { }
        ~JWebTopContext() {
            closeContext();
        }

        private void __startIPCServer() {
            server.startRead();
        }

        public void createJWebTopByCfgFile(String processPath, String cfgFile, JWebTopAppInited appInitListener) {
            this.appInitListener = appInitListener;
            if (server != null) return;
            if (jsonHandler == null) throw new JWebTopException("必须设置jsonHandler。");
            try {
                if (!File.Exists(processPath)) throw new JWebTopException("找不到可执行文件：" + processPath);
                processPath = Path.GetFullPath(processPath);// 如果不是绝对路径，浏览器无法显示出来
                if (cfgFile != null && cfgFile.Trim().Length > 0) {
                    if (!File.Exists(cfgFile)) throw new JWebTopException("找不到配置文件：" + processPath);
                    cfgFile = Path.GetFullPath(cfgFile);
                }
            } catch (IOException e) {
                throw new JWebTopException("无法创建JWebTop", e);
            }
            server = new FastIPCServer();
            this.serverName = FastIPCNative.genServerName();
            this.blockSize = 2048;
            this.server.create(serverName, blockSize);
            server.setListener(this);
            Thread th = new Thread(new ThreadStart(__startIPCServer)); //也可简写为new Thread(ThreadMethod);                
            th.Start(); //启动线程  

            String cmds = " :"// “:”作为特殊符号告诉JWebTop主程序，还有其他参数要解析。因为“:”不可能出现在文件路径的开通
                    + " " + JWebTopNative.getProcessID()// 将当前进程id传递过去，以便当前进程结束后，JWebTop主程序自己退出
                    + " " + blockSize// 通过FastIPC进行通信时，缓存区的大小
                    + " " + serverName // 通过FastIPC进行通信时，双方的交互标记
                    + " \"" + cfgFile + "\"" // 配置文件的路径
                // + " " + JWebTopContext.WIN_HWND//
            ;
            FastIPCNative.CreateSubProcess(processPath, cmds);
        }

        /// <summary>
        /// 设置一个错误页面
        /// </summary>
        /// <param name="url">错误页面</param>
        public void setErrorUrl(String url) {
            client.write(JWM_SET_ERR_URL, 0, null, url);
        }

        /**
         * 根据配置文件创建浏览器，成功后返回创建的浏览器的句柄，此方法在浏览器构建过程中会一直阻塞当前线程
         * 
         * @param browerCfgFile
         * @return 创建的浏览器的句柄
         */
        public void createBrowserByFile(String browerCfgFile, JWebTopBrowserCreated browserCreateListener) {
            if (client == null) throw new JWebTopException("尚未完成初始化");
            String taskId = new Guid().ToString();
            browserCreateListeners.Add(taskId, browserCreateListener);
            _createBrowser(JWM_CREATEBROWSER_FILE, taskId, browerCfgFile);
        }

        /**
         * 根据配置对象来创建浏览器，成功后返回创建的浏览器的句柄，此方法在浏览器构建过程中会一直阻塞当前线程
         * 
         * @param configs
         *            配置浏览器的对象
         * @return 创建的浏览器的句柄
         */
        public void createBrowser(JWebTopConfigs configs, JWebTopBrowserCreated browserCreateListener) {
            if (client == null) throw new JWebTopException("尚未完成初始化");
            JObject jo = JObject.FromObject(configs);
            JWebTopConfigs.removeDefaults(jo);// 移除一些默认值属性
            createBrowserByJSON(jo.ToString(), browserCreateListener);
        }

        /**
         * 根据JSON配置字符串创建浏览器，成功后返回创建的浏览器的句柄，此方法在浏览器构建过程中会一直阻塞当前线程
         * 
         * @param browerCfgJSON
         *            JSON配置字符串
         * @return 创建的浏览器的句柄
         */
        public void createBrowserByJSON(String browerCfgJSON, JWebTopBrowserCreated browserCreateListener) {
            if (client == null) throw new JWebTopException("尚未完成初始化");
            String taskId = new Guid().ToString();
            browserCreateListeners.Add(taskId, browserCreateListener);
            _createBrowser(JWM_CREATEBROWSER_JSON, taskId, browerCfgJSON);
        }

        private void _createBrowser(int createType, String browserHWnd, String cfgData) {
            client.write(createType, 0, browserHWnd, cfgData);
        }

        /**
         * 关闭指定浏览器
         * 
         * @param browserHWnd
         */
        public void closeBrowser(int browserHWnd) {
            if (client == null) return;
            client.write(JWM_CLOSEBROWSER, browserHWnd, null, null);
        }

        public void closeContext() {
            if (client == null) return;
            client.write(JWM_CEF_ExitAPP, 0, null, null);
            if (server != null) server.close();
            if (client != null) client.close();
        }

        // 内部线程：用于分发浏览器发来的数据
        class InnerThread {
            private int userMsgType;
            private int userValue;
            private String userShortStr;
            private String data;
            private JWebTopContext ctx;

            public InnerThread(int userMsgType, int userValue, String userShortStr, String data, JWebTopContext ctx) {
                //super("用于分发浏览器发来的数据");
                this.userMsgType = userMsgType;
                this.userValue = userValue;
                this.userShortStr = userShortStr;
                this.data = data;
                this.ctx = ctx;
            }

            ~InnerThread() {
                this.ctx = null;
                this.userShortStr = null;
                this.data = null;
            }

            public void run() {
                // System.out.println("Readed msgId=" + userMsgType + " userValue=" + userValue + " taskId=" + userShortStr + " msg=" + data);
                switch (userMsgType) {
                    case JWM_DLL_EXECUTE_RETURN:
                        ctx.jsonHandler.dispatcher(userValue, data);
                        break;
                    case JWM_DLL_EXECUTE_WAIT:
                        String result = ctx.jsonHandler.dispatcher(userValue, data); // 取回执行结果
                        ctx.client.write(JWM_RESULT_RETURN, userValue, userShortStr, result); // 发送结果到远程进程
                        break;
                    case JWM_RESULT_RETURN: {
                        if (ctx.jsReturnListeners.ContainsKey(userShortStr)) {
                                JWebTopJSReturn tmp = ctx.jsReturnListeners[userShortStr];
                                if (tmp != null) {
                                    ctx.jsReturnListeners.Remove(userShortStr);
                                    tmp.onJWebTopJSReturn(data);
                                }
                            }
                        } break;
                    case JWM_BROWSER_CREATED: {
                            if (ctx.browserCreateListeners.ContainsKey(userShortStr)) {
                                JWebTopBrowserCreated tmp = ctx.browserCreateListeners[userShortStr];
                                ctx.browserCreateListeners.Remove(userShortStr);
                                tmp.onJWebTopBrowserCreated(userValue);
                            }
                        } break;
                    case JWM_IPC_CLIENT_OK:
                        ctx.client = new FastIPCClient();// 启动JWebTop程序
                        ctx.client.create(FastIPCNative.genServerName(ctx.serverName), ctx.blockSize);
                        break;
                    case JWM_CEF_APP_INITED:
                        ctx.appInitListener.onJWebTopAppInited();
                        ctx.appInitListener = null;
                        break;
                    default:
                        throw new JWebTopException("无法处理的消息（" + userMsgType + "）");
                }
            }
        }


        public override void OnRead(int userMsgType, int userValue, String userShortStr, String data) {
            InnerThread t = new InnerThread(userMsgType, userValue, userShortStr, data, this);
            new Thread(new ThreadStart(t.run)).Start();
        }

        // jni方法：退出JWebTop进程
        void ExitJWebTop() {
            client.write(JWM_CEF_ExitAPP, 0, null, null); // 发送任务到远程进程
        }

        public String getServerName() {
            return serverName;
        }

        public void setServerName(String serverName) {
            this.serverName = serverName;
        }

        public int getBlockSize() {
            return blockSize;
        }

        public void setBlockSize(int blockSize) {
            this.blockSize = blockSize;
        }

        public JWebtopJSONDispater getJsonHandler() {
            return jsonHandler;
        }

        public void setJsonHandler(JWebtopJSONDispater jsonHandler) {
            this.jsonHandler = jsonHandler;
        }

        /**
         * 执行js，发送后等待数据的返回。<br/>
         * 这里发送是包装的JSON数据，其调用JS脚本中的invokeByDLL(jsonvalue)方法
         * 
         * @param jsonstring
         * @return 返回是JSON字符串数据
         */
        public void executeJSON_Wait(int browserHWnd, String jsonstring, JWebTopJSReturn jsReturn) {
            String taskId = new Guid().ToString();
            jsReturnListeners.Add(taskId, jsReturn);
            client.write(JWM_JSON_EXECUTE_WAIT, browserHWnd, taskId, jsonstring);
        }

        /**
         * 执行js，但不等待数据的返回。<br/>
         * 这里发送是包装的JSON数据，其调用JS脚本中的invokeByDLL(jsonvalue)方法
         * 
         * @param jsonstring
         */
        public void executeJSON_NoWait(int browserHWnd, String jsonstring) {
            client.write(JWM_JSON_EXECUTE_RETURN, browserHWnd, null, jsonstring);
        }

        /**
         * 执行js，并等待数据的返回。<br/>
         * 推荐使用executeJSON_Wait方法，以减少字符串转换的问题
         * 
         * @param script
         * @return 返回是字符串数据
         */
        public void executeJS_Wait(int browserHWnd, String script, JWebTopJSReturn jsReturn) {
            String taskId = new Guid().ToString();
            jsReturnListeners.Add(taskId, jsReturn);
            client.write(JWM_JS_EXECUTE_WAIT, browserHWnd, taskId, script);
        }

        /**
         * 执行js，但不等待数据的返回。 <br/>
         * 推荐使用executeJSON_NoWait方法，以减少字符串转换的问题
         * 
         * @param script
         * @return
         */
        public void executeJS_NoWait(int browserHWnd, String script) {
            client.write(JWM_JS_EXECUTE_RETURN, browserHWnd, null, script);
        }

        public void setUrl(int browserHWnd, String url) {
            client.write(JWM_M_SETURL, browserHWnd, null, url);
        }

        public void setAppendJS(int browserHWnd, String js) {
            client.write(JWM_M_APPEND_JS, browserHWnd, null, js);
        }

        public bool isStable() {
            return server != null && server.isStable();
        }
    }
}
