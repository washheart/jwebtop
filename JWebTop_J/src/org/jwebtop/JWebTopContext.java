package org.jwebtop;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.fastipc.FastIPCClient;
import org.fastipc.FastIPCException;
import org.fastipc.FastIPCNative;
import org.fastipc.FastIPCReadListener;
import org.fastipc.FastIPCServer;
import org.jwebtop.listener.JWebTopAppInited;
import org.jwebtop.listener.JWebTopBrowserCreated;
import org.jwebtop.listener.JWebTopJSReturn;
import org.jwebtop.listener.JWebtopJSONDispater;

import com.alibaba.fastjson.JSONObject;

/**
 * 创建JWebTop浏览器，一般一个程序只需要一个JWebTopContext。一个JWebTopContext可以创建多个浏览器。<br>
 * 源码：https://github.com/washheart/jwebtop<br>
 * 说明：https://github.com/washheart/jwebtop/wiki<br>
 * 
 * @author washheart@163.com
 */
public class JWebTopContext implements FastIPCReadListener {
	private final static int//
			__JWM = 100, // 定义MPMSG_MINI和MPMSG_LARGE中msgId的起始值
			JWM_IPC_CLIENT_OK = __JWM + 101, // IPC对应的客户端已成功创建
			JWM_SET_ERR_URL = __JWM + 102, // 设置错误页面
			// JWM_SET_TASK_WAIT_TIME = (__JWM + 103), // 执行等待任务时的最大等待时间（毫秒）
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
	private Map<String, ByteArrayOutputStream> FastIPCReceivedCaches = new HashMap<String, ByteArrayOutputStream>();
	private FastIPCServer server = null;
	private FastIPCClient client = null;
	private String serverName;
	private int blockSize;

	// private String cfgFile = null/* 通过配置文件初始化JWebTop */, cfgJSON = null/* 通过JSON初始化JWebTop */;
	// private String processPath;

	private JWebtopJSONDispater jsonHandler = null;
	private ClassLoader commonlassLoader;
	private JWebTopAppInited appInitListner;

	private Map<String, JWebTopBrowserCreated> browserCreateListeners = new HashMap<String, JWebTopBrowserCreated>();
	private Map<String, JWebTopJSReturn> jsReturnListners = new HashMap<String, JWebTopJSReturn>();

	public static void initDLL(String dllDir) {
		File dllFile = new File(dllDir, "JWebTopCommon.dll");
		if (!dllFile.exists()) {
			throw new FastIPCException("DLL文件[" + dllFile + "]不存在");
		}
		System.load(dllFile.getAbsolutePath());
		dllFile = new File(dllDir, "JWebTop.dll");
		if (!dllFile.exists()) {
			throw new FastIPCException("DLL文件[" + dllFile + "]不存在");
		}
		System.load(dllFile.getAbsolutePath());
	}

	/**
	 * 创建JWebTop进程，成功后回调JWebTopAppInited侦听器
	 * 
	 * @param processPath
	 * @param cfgFile
	 * @param appInitListner
	 */
	public void createJWebTopByCfgFile(String processPath, String cfgFile, JWebTopAppInited appInitListner) {
		if (server != null) return;
		if (jsonHandler == null) throw new JWebTopException("必须设置jsonHandler。");
		try {
			this.commonlassLoader = JWebTopNative.class.getClassLoader();
			File file = new File(processPath);
			if (!file.exists()) throw new JWebTopException("找不到可执行文件：" + processPath);
			processPath = file.getCanonicalPath();// 如果不是绝对路径，浏览器无法显示出来
			if (cfgFile != null && cfgFile.trim().length() > 0) {
				file = new File(cfgFile);
				if (!file.exists()) throw new JWebTopException("找不到配置文件：" + cfgFile);
				cfgFile = file.getCanonicalPath();
			}
		} catch (IOException e) {
			throw new JWebTopException("无法创建JWebTop", e);
		}
		server = new FastIPCServer();
		this.serverName = FastIPCNative.genServerName();
		this.blockSize = 2048;
		this.appInitListner = appInitListner;
		this.server.create(serverName, blockSize);
		server.setListener(this);
		new Thread("createIPCServer") {
			@Override
			public void run() {
				server.startRead();
			}
		}.start();
		final String cmds = " :"// “:”作为特殊符号告诉JWebTop主程序，还有其他参数要解析。因为“:”不可能出现在文件路径的开通
				+ " " + String.valueOf(JWebTopNative.getProcessID())// 将当前进程id传递过去，以便当前进程结束后，JWebTop主程序自己退出
				+ " " + Integer.toString(blockSize)// 通过FastIPC进行通信时，缓存区的大小
				+ " " + serverName // 通过FastIPC进行通信时，双方的交互标记
				+ " \"" + cfgFile + "\"" // 配置文件的路径
				// + " " + JWebTopContext.WIN_HWND//
		;
		final String _processPath = processPath;
		new Thread() {
			@Override
			public void run() {
				JWebTopNative.createSubProcess(_processPath, cmds, true);
				if (client != null) client.close();
			}
		}.start();
		// long handle = JWebTopNative.createSubProcess(processPath, cmds, true);
		// if (0 > handle) {
		// server.close();
		// throw new JWebTopException("无法启动程序（" + processPath + "）！");
		// }
		// String[] cmds = {//
		// processPath// 要启动的程序的路径
		// , ":"// “:”作为特殊符号告诉JWebTop主程序，还有其他参数要解析。因为“:”不可能出现在文件路径的开通
		// , String.valueOf(JWebTopNative.getProcessID())// 将当前进程id传递过去，以便当前进程结束后，JWebTop主程序自己退出
		// , Integer.toString(blockSize)// 通过FastIPC进行通信时，缓存区的大小
		// , serverName // 通过FastIPC进行通信时，双方的交互标记
		// , cfgFile // 配置文件的路径
		// };
		// try {
		// Runtime.getRuntime().exec(cmds, null, null);// 这种方式创建的线程，最终会导致java的主线程无法退出？？？
		// } catch (Throwable e) {
		// e.printStackTrace();
		// }
	}

	/**
	 * 根据配置文件创建浏览器，成功后回调JWebTopBrowserCreated
	 * 
	 * @param browserCfgFile
	 */
	public void createBrowserByFile(String browserCfgFile, JWebTopBrowserCreated createListener) {
		if (client == null) throw new JWebTopException("尚未完成初始化");
		String taskId = createTaskId();
		browserCreateListeners.put(taskId, createListener);
		_createBrowser(JWM_CREATEBROWSER_FILE, taskId, browserCfgFile);
	}

	/**
	 * 根据配置对象来创建浏览器，成功后回调JWebTopBrowserCreated
	 * 
	 * @param configs
	 *            配置浏览器的对象
	 */
	public void createBrowser(JWebTopConfigs configs, JWebTopBrowserCreated createListener) {
		if (client == null) throw new JWebTopException("尚未完成初始化");
		JSONObject jo = (JSONObject) JSONObject.toJSON(configs);
		JWebTopConfigs.removeDefaults(jo);// 移除一些默认值属性
		createBrowserByJSON(jo.toString(), createListener);
	}

	/**
	 * 根据JSON配置字符串创建浏览器，成功后回调JWebTopBrowserCreated
	 * 
	 * @param browerCfgJSON
	 *            JSON配置字符串
	 */
	public void createBrowserByJSON(String browerCfgJSON, JWebTopBrowserCreated createListener) {
		if (client == null) throw new JWebTopException("尚未完成初始化");
		String taskId = createTaskId();
		browserCreateListeners.put(taskId, createListener);
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
	public void closeBrowser(long browserHWnd) {
		if (client == null) return;
		client.write(JWM_CLOSEBROWSER, browserHWnd, null, null);
	}

	/**
	 * 关闭当前Context持有的FastIPC服务端、客户端，以及浏览器进程
	 */
	public void closeContext() {
		if (client == null) return;
		client.write(JWM_CEF_ExitAPP, 0, null, null);
		if (server != null) server.close();
		if (client != null) client.close();
	}

	@Override
	protected void finalize() throws Throwable {
		closeContext();
		super.finalize();
	}

	// 内部线程：用于分发浏览器发来的数据
	class InnerThread extends Thread {
		private int userMsgType;
		private long userValue;
		private String userShortStr;
		private String data;

		InnerThread(int userMsgType, long userValue, String userShortStr, String data) {
			super("用于分发浏览器发来的数据");
			this.userMsgType = userMsgType;
			this.userValue = userValue;
			this.userShortStr = userShortStr;
			this.data = data;
		}

		@Override
		public void run() {
			// System.out.println("Readed msgId=" + userMsgType + " userValue=" + userValue + " taskId=" + userShortStr + " msg=" + data);
			switch (userMsgType) {
			case JWM_DLL_EXECUTE_RETURN:
				jsonHandler.dispatcher(userValue, data);
				break;
			case JWM_DLL_EXECUTE_WAIT:
				String result = "";
				try {
					result = jsonHandler.dispatcher(userValue, data); // 取回执行结果
				} catch (Throwable th) {
					th.printStackTrace();// 不管发生什么情况，都需要把taskId返回回去，否则会造成系统在那里死死的等待
				}
				client.write(JWM_RESULT_RETURN, userValue, userShortStr, result); // 发送结果到远程进程
				break;
			case JWM_RESULT_RETURN:
				JWebTopJSReturn tmp1 = jsReturnListners.remove(userShortStr);
				if (tmp1 != null) tmp1.onJWebTopJSReturn(userShortStr, data);
				break;
			case JWM_BROWSER_CREATED:
				JWebTopBrowserCreated tmp2 = browserCreateListeners.remove(userShortStr);
				if (tmp2 != null) tmp2.onJWebTopBrowserCreated(userValue);
				break;
			case JWM_IPC_CLIENT_OK:
				client = new FastIPCClient();// 启动JWebTop程序
				client.create(FastIPCNative.genServerName(serverName), blockSize);
				break;
			case JWM_CEF_APP_INITED:
				if (appInitListner != null) {
					appInitListner.onJWebTopAppInited();
					appInitListner = null;
				}
				break;
			default:
				throw new JWebTopException("无法处理的消息（" + userMsgType + "）");
			}
		}
	}

	@Override
	public void OnRead(int msgType, String packId, int userMsgType, long userValue, String userShortStr, byte[] data) {
		try {
			if (msgType == FastIPCNative.MSG_TYPE_NORMAL) {
				new InnerThread(userMsgType, userValue, userShortStr, new String(data, "utf-8")).start();// 在新线程中进行数据的分发处理，避免阻塞数据的收发
			} else {// 完成数据的组装
				ByteArrayOutputStream bos = FastIPCReceivedCaches.get(packId);
				if (bos == null) {
					bos = new ByteArrayOutputStream();
					FastIPCReceivedCaches.put(packId, bos);
				}
				bos.write(data);
				if (msgType == FastIPCNative.MSG_TYPE_END) {
					FastIPCReceivedCaches.remove(packId);
					new InnerThread(userMsgType, userValue, userShortStr, new String(bos.toByteArray(), "utf-8")).start();
				}
			}
		} catch (Throwable e) {
			e.printStackTrace();
		}
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

	public ClassLoader getCommonlassLoader() {
		return commonlassLoader;
	}

	public void setCommonlassLoader(ClassLoader commonlassLoader) {
		this.commonlassLoader = commonlassLoader;
	}

	/**
	 * 执行js，发送后等待数据的返回。<br/>
	 * 这里发送是包装的JSON数据，其调用JS脚本中的invokeByDLL(jsonvalue)方法
	 * 
	 * @param browserHWnd
	 *            在哪个浏览器执行
	 * @param jsonstring
	 *            待执行的JSON数据
	 * @param jsReturn
	 *            JS执行结果的侦听器
	 * @param uuid
	 *            执行js时用于获取返回结果，如果jsReturn是共用的，那么可以根据此uuid来区分不同的调用
	 */
	public void executeJSON_Wait(long browserHWnd, String jsonstring, JWebTopJSReturn jsReturn, String uuid) {
		String taskId = (uuid == null || uuid.trim().length() == 0) ? createTaskId() : uuid;
		jsReturnListners.put(taskId, jsReturn);
		client.write(JWM_JSON_EXECUTE_WAIT, browserHWnd, taskId, jsonstring);
	}

	/**
	 * 执行js，但不等待数据的返回。<br/>
	 * 这里发送是包装的JSON数据，其调用JS脚本中的invokeByDLL(jsonvalue)方法
	 * 
	 * @param jsonstring
	 */
	public void executeJSON_NoWait(long browserHWnd, String jsonstring) {
		client.write(JWM_JSON_EXECUTE_RETURN, browserHWnd, null, jsonstring);
	}

	/**
	 * 执行js，并等待数据的返回。<br/>
	 * 推荐使用executeJSON_Wait方法，以减少字符串转换的问题
	 * 
	 * @param browserHWnd
	 *            在哪个浏览器执行
	 * @param script
	 *            待执行的JS脚本
	 * @param jsReturn
	 *            JS执行结果的侦听器
	 * @param uuid
	 *            执行js时用于获取返回结果，如果jsReturn是共用的，那么可以根据此uuid来区分不同的调用
	 */
	public void executeJS_Wait(long browserHWnd, String script, JWebTopJSReturn jsReturn, String uuid) {
		String taskId = (uuid == null || uuid.trim().length() == 0) ? createTaskId() : uuid;
		jsReturnListners.put(taskId, jsReturn);
		client.write(JWM_JS_EXECUTE_WAIT, browserHWnd, taskId, script);
	}

	/**
	 * 执行js，但不等待数据的返回。 <br/>
	 * 推荐使用executeJSON_NoWait方法，以减少字符串转换的问题
	 * 
	 * @param script
	 * @return
	 */
	public void executeJS_NoWait(long browserHWnd, String script) {
		client.write(JWM_JS_EXECUTE_RETURN, browserHWnd, null, script);
	}

	public void setUrl(long browserHWnd, String url) {
		client.write(JWM_M_SETURL, browserHWnd, null, url);
	}

	public void setAppendJS(long browserHWnd, String js) {
		client.write(JWM_M_APPEND_JS, browserHWnd, null, js);
	}

	public boolean isStable() {
		return server != null && server.isStable();
	}// 生成任务的id

	/**
	 * 设置一个错误页面
	 * 
	 * @param url
	 */
	public void setErrorUrl(String url) {
		client.write(JWM_SET_ERR_URL, 0, null, url);
	}

	private long tid = 0;

	public String createTaskId() {
		// return UUID.randomUUID().toString();
		return "J_J_" + (++tid);
	}
}