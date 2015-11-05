package org.fastipc;

import java.util.UUID;

/**
 * FastIPC的native方法定义。<br/>
 * 源码：https://github.com/washheart/fastipc<br/>
 * 源码：http://git.oschina.net/washheart/fastipc<br/>
 * 说明：https://github.com/washheart/fastipc/wiki<br/>
 * 
 * @author washheart@163.com
 */
public class FastIPCNative {
	public static int // 定义消息类型常量
			MSG_TYPE_NORMAL = 0, // 正常的消息
			MSG_TYPE_PART = 1,// 此消息是一完整数据的一部分
			MSG_TYPE_END = 2; // 此消息是一完整数据的结尾

	private static int//
			// ERR_ServerCreate = 100, // 服务器创建失败
			// ERR_ClientCreate = 101, // 客户端创建失败
			// ERR_EventCreate_W = 201, // 服务器创建写事件失败
			// ERR_EventCreate_R = 202, // 服务器创建读事件失败
			// ERR_EventOpen_W = 203, // 客户端打开写事件失败
			// ERR_EventOpen_R = 204, // 客户端打开读事件失败
			// ERR_EventWait_W = 205,// 客户端等待可写事件失败
			// ERR_EventWait_R = 206, // 服务器等待可读事件失败
			// ERR_MappingCreate = 301,// 服务器创建映射文件失败
			// ERR_MappingOpen = 302, // 服客户端打开映射文件失败
			// ERR_MappingMap = 303, // 服务器/客户端映射内存文件失败
			ERR_ServerName = 1, // 客户端创建失败
			ERR_NONE = 0;// 没有任何错误

	private native static int[] nCreateServer(String serverName, int blockSize);

	private native static void nStartRead(int nativeServer, FastIPCReadListener listener);

	private native static void nCloseServer(int nativeServer);

	private native static int[] nCreateClient(String serverName, int blockSize);

	private native static int nWriteClient(int nativeClient, int userMsgType, long userValue, String userShortStr, String data);

	private native static void nCloseClient(int nativeClient);

	static int createServer(String serverName, int blockSize) {
		FastIPCNative.checkServerName(serverName);
		int[] rtn = FastIPCNative.nCreateServer(serverName, blockSize);
		if (rtn[0] != FastIPCNative.ERR_NONE) {
			throw new FastIPCException("创建服务端失败（" + rtn + "）！");
		}
		return rtn[1];
	}

	static void closeServer(int nativeServer) {
		FastIPCNative.nCloseServer(nativeServer);
	}

	static void startRead(int nativeServer, FastIPCReadListener listener) {
		nStartRead(nativeServer, listener);
	}

	static int createClient(String serverName, int blockSize) {
		FastIPCNative.checkServerName(serverName);
		int[] rtn = FastIPCNative.nCreateClient(serverName, blockSize);
		if (rtn[0] != FastIPCNative.ERR_NONE) {
			throw new FastIPCException("创建服务端失败（" + rtn + "）！");
		}
		return rtn[1];
	}

	private static void checkServerName(String serverName) {
		if (serverName == null) {
			throw new FastIPCException("请指定服务器名称（" + FastIPCNative.ERR_ServerName + "）！");
		} else {
			int len = serverName.length();
			if (len == 0) throw new FastIPCException("请指定服务器名称（" + FastIPCNative.ERR_ServerName + "）！");
			for (int i = 0; i < len; i++) {
				char c = serverName.charAt(i);
				if (c == '-') continue;
				if ('0' <= c || c >= '-') continue;
				if ('A' <= c || c >= 'Z') continue;
				if ('a' <= c || c >= 'z') continue;
				throw new FastIPCException("服务器名称包含无效字符（" + FastIPCNative.ERR_ServerName + "）！");
			}
		}
	}

	static void write(int nativeClient, int userMsgType, long userValue, String userShortStr, String data) {
		nWriteClient(nativeClient, userMsgType, userValue, userShortStr, data);
	}

	public static String genServerName() {
		return UUID.randomUUID().toString();
	}

	public static String genServerName(String serverName) {
		return serverName + "_c";
	}

	static void closeClient(int nativeClient) {
		nCloseClient(nativeClient);
	}
}