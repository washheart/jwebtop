package org.jwebtop;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.util.HashMap;
import java.util.Map;

import org.fastipc.FastIPCClient;
import org.fastipc.FastIPCException;
import org.fastipc.FastIPCNative;
import org.fastipc.FastIPCReadListener;
import org.fastipc.FastIPCServer;
import org.jwebtop.TaskUtil.ProcessMsgLock;

public abstract class JWebTopContext implements FastIPCReadListener {
	public final static int//
			__JWM = 100, // 定义MPMSG_MINI和MPMSG_LARGE中msgId的起始值
			JWM_IPC_CLIENT_OK = __JWM + 101, // IPC对应的客户端已成功创建
			JWM_CEF_APP_INITED = __JWM + 102, // CEF浏览器已初始完成
			JWM_CEF_ExitAPP = __JWM + 102, // CEF浏览器已初始完成

			JWM_CREATEBROWSER_JSON = __JWM + 401, // 创建浏览器的消息 （同步发送）
			JWM_CREATEBROWSER_FILE = __JWM + 402, // 创建浏览器的消息 （同步发送）
			JWM_CLOSEBROWSER = __JWM + 403, // 关闭浏览器的消息 （异步发送）

			JWM_JS_EXECUTE_WAIT = __JWM + 201, // DLL调用CEF端：需要执行并等待的JS任务
			JWM_JS_EXECUTE_RETURN = __JWM + 202, //
			JWM_JSON_EXECUTE_WAIT = __JWM + 211, // DLL调用CEF端：需要执行并等待的JSON任务
			JWM_JSON_EXECUTE_RETURN = __JWM + 212, //
			JWM_DLL_EXECUTE_WAIT = __JWM + 221, // CEF调用DLL端：需要执行并等待的任务
			JWM_DLL_EXECUTE_RETURN = __JWM + 222, // CEF调用DLL端：需要执行但CEF无需等待的任务
			JWM_RESULT_RETURN = __JWM + 231,//
			JWM_STARTJWEBTOP = __JWM + 301, // 启动JWebTop进程
			JWM_CFGJWEBTOP_FILE = __JWM + 302, //
			JWM_CFGJWEBTOP_JSON = __JWM + 303; //

	private Map<String, ByteArrayOutputStream> FastIPCReceivedCaches = new HashMap<String, ByteArrayOutputStream>();
	private FastIPCServer server = null;
	private FastIPCClient client = null;
	private String serverName;
	private int blockSize;

	private String cfgFile = null/* 通过配置文件初始化JWebTop */, cfgJSON = null/* 通过JSON初始化JWebTop */;

	public static void initDLL(String dllDir) throws Exception {
		File dllFile = new File(dllDir, "FastIPC.dll");
		if (!dllFile.exists()) {
			throw new FastIPCException("DLL文件[" + dllFile + "]不存在");
		}
		System.load(dllFile.getAbsolutePath());
		dllFile = new File(dllDir, "FastIPC_JNI.dll");
		if (!dllFile.exists()) {
			throw new FastIPCException("DLL文件[" + dllFile + "]不存在");
		}
		System.load(dllFile.getAbsolutePath());
	}

	public void createJWebTopByCfgFile(String cfgFile, FastIPCReadListener listener) {
		if (server != null) return;
		this.cfgFile = cfgFile;
		createIPCServer(listener);
	}

	public void createJWebTopByCfgJSON(String cfgJSON, FastIPCReadListener listener) {
		if (server != null) return;
		this.cfgJSON = cfgJSON;
		createIPCServer(listener);
	}

	/**
	 * 根据配置文件创建浏览器
	 * 
	 * @param browerCfgFile
	 * @param listener
	 */
	public void createBrowserByCfgFile(String broserUuid, String browerCfgFile) {
		if (client == null) throw new JWebTopException("尚未完成初始化");
		_createBrowser(JWM_CREATEBROWSER_FILE, broserUuid, browerCfgFile);
	}

	/**
	 * 根据JSON配置文件创建浏览器
	 * 
	 * @param browerCfgFile
	 */
	public void createBrowserByCfgJSON(String broserUuid, String browerCfgJSON) {
		if (client == null) throw new JWebTopException("尚未完成初始化");
		_createBrowser(JWM_CREATEBROWSER_JSON, broserUuid, browerCfgJSON);
	}

	private void _createBrowser(int createType, String broserUuid, String cfgData) {
		client.write(createType, 0, broserUuid, cfgData);
	}

	// 内部线程：用于分发浏览器发来的数据
	class InnerThread extends Thread {
		private static final int JWM_BROWSER_CREATED = 0;
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
			switch (userMsgType) {
			case JWM_DLL_EXECUTE_RETURN:
				invokeByBrowser(userValue, data);
				break;
			case JWM_JS_EXECUTE_RETURN:
				String result = invokeByBrowser(userValue, data); // 取回执行结果
				client.write(JWM_RESULT_RETURN, userValue, userShortStr, result); // 发送结果到远程进程
			case JWM_RESULT_RETURN:
				if (userShortStr != null && userShortStr.length() > 0) TaskUtil.putTaskResult(userShortStr, data);
				break;
			case JWM_BROWSER_CREATED:
				jWebTopBrowserCreated(userShortStr, userValue);
				break;
			case JWM_IPC_CLIENT_OK:
				startJWebTopContext();// 启动JWebTop程序
				break;
			case JWM_CEF_APP_INITED:
				jWebTopInited();
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
					new InnerThread(userMsgType, userValue, userShortStr, new String(bos.toByteArray(), "utf-8"));
				}
			}
		} catch (Throwable e) {
			e.printStackTrace();
		}
	}

	public abstract void jWebTopBrowserCreated(String userShortStr, long userValue);

	public abstract String invokeByBrowser(long userValue, String data);

	public abstract void jWebTopInited();

	private void startJWebTopContext() {
		if (client != null) return;
		client = new FastIPCClient();
		client.create(FastIPCNative.genServerName(serverName), blockSize);
		int userMsgType = JWM_CFGJWEBTOP_FILE;
		String data = cfgFile;
		if (cfgJSON != null) {
			userMsgType = JWM_CFGJWEBTOP_JSON;
			data = cfgJSON;
		}
		client.write(userMsgType, 0, null, data);
	}

	private void createIPCServer(FastIPCReadListener listener) {
		server = new FastIPCServer();
		this.serverName = FastIPCNative.genServerName();
		this.blockSize = 2048;
		this.server.create(serverName, blockSize);
		server.setListener(listener);
		new Thread("createIPCServer") {
			@Override
			public void run() {
				server.startRead();
			}
		}.start();
	}

	private String exeRemoteAndWait(long browserHWnd, String msg, int msgId) {
		String taskId = TaskUtil.createTaskId(); // 生成任务id
		ProcessMsgLock lock = TaskUtil.addTask(taskId); // 放置任务到任务池
		try {
			client.write(msgId, browserHWnd, taskId, msg); // 发送任务到远程进程
			return lock.waitResult(); // 等待任务完成并取回执行结果
		} catch (Throwable th) {
			TaskUtil.removeTask(taskId); // 消息发送失败移除现有消息
			throw new JWebTopException("exeRemoteAndWait出错", th); // 返回数据：注意这里是空字符串
		}
	}

	String JWebTopExecuteJSWait(long browserHWnd, String script) {
		return exeRemoteAndWait(browserHWnd, script, JWM_JS_EXECUTE_WAIT);
	}

	String JWebTopExecuteJSONWait(long browserHWnd, String json) {
		return exeRemoteAndWait(browserHWnd, json, JWM_JSON_EXECUTE_WAIT);
	}

	void JWebTopExecuteJSNoWait(long browserHWnd, String script) {
		client.write(JWM_JS_EXECUTE_RETURN, browserHWnd, null, script); // 发送任务到远程进程
	}

	// jni方法：执行脚本且不等待返回结果
	void JWebTopExecuteJSONNoWait(long browserHWnd, String json) {
		client.write(JWM_JSON_EXECUTE_RETURN, browserHWnd, null, json); // 发送任务到远程进程
	}

	// jni方法：退出JWebTop进程
	void ExitJWebTop() {
		client.write(JWM_CEF_ExitAPP, 0, null, null); // 发送任务到远程进程
	}
}
