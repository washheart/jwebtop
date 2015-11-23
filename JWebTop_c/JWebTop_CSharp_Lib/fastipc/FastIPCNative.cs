using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace org {
    namespace fastipc {
        /**
         * FastIPC的native方法定义。<br/>
         * 源码：https://github.com/washheart/fastipc<br/>
         * 源码：http://git.oschina.net/washheart/fastipc<br/>
         * 说明：https://github.com/washheart/fastipc/wiki<br/>
         * 
         * @author washheart@163.com
         */
        class FastIPCNative {
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

    [DllImport("JWebTopCommon.dll", CharSet = CharSet.Unicode)]
     private extern static int nCreateServer([MarshalAs(UnmanagedType.LPTStr)]string serverName, int blockSize,ref int result);


    [DllImport("JWebTopCommon.dll", CharSet = CharSet.Unicode)]
	private extern static void nStartRead(int nativeServer, FastIPCReadListener listener);

    [DllImport("JWebTopCommon.dll", CharSet = CharSet.Unicode)]
	private extern static void nCloseServer(int nativeServer);

    private extern static int nCreateClient(String serverName, int blockSize, ref int result);

	private extern static int nWriteClient(int nativeClient, int userMsgType, long userValue, String userShortStr, String data);

    private extern static void nCloseClient(int nativeClient);

	static int createServer(String serverName, int blockSize) {
		FastIPCNative.checkServerName(serverName);
        int result = -1;
        int objectPtr = FastIPCNative.nCreateServer(serverName, blockSize, ref result);
        if (result != FastIPCNative.ERR_NONE) {
            throw new FastIPCException("创建服务端失败（" + result + "）！");
		}
        return objectPtr;
	}

	static void closeServer(int nativeServer) {
		FastIPCNative.nCloseServer(nativeServer);
	}

	static void startRead(int nativeServer, FastIPCReadListener listener) {
		nStartRead(nativeServer, listener);
	}

	static int createClient(String serverName, int blockSize) {
		FastIPCNative.checkServerName(serverName);
        int result = -1;
        int objectPtr = FastIPCNative.nCreateClient(serverName, blockSize, ref result);
        if (result != FastIPCNative.ERR_NONE) {
            throw new FastIPCException("创建客户端失败（" + result + "）！");
        }
        return objectPtr;
	}

	private static void checkServerName(String serverName) {
		if (serverName == null) {
			throw new FastIPCException("请指定服务器名称（" + FastIPCNative.ERR_ServerName + "）！");
		} else {
			int len = serverName.Length;
			if (len == 0) throw new FastIPCException("请指定服务器名称（" + FastIPCNative.ERR_ServerName + "）！");
			for (int i = 0; i < len; i++) {
				char c = serverName[i];
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
		return System.Guid.NewGuid().ToString();
	}

	public static String genServerName(String serverName) {
		return serverName + "_c";
	}

	static void closeClient(int nativeClient) {
		nCloseClient(nativeClient);
	}

        }
    }
}
