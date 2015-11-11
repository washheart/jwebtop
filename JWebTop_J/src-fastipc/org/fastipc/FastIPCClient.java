package org.fastipc;

/**
 * FastIPC的客户端。<br/>
 * 源码：https://github.com/washheart/fastipc<br/>
 * 源码：http://git.oschina.net/washheart/fastipc<br/>
 * 说明：https://github.com/washheart/fastipc/wiki<br/>
 * 
 * @author washheart@163.com
 */
public final class FastIPCClient {
	private int nativeClient = 0;// 指向fastipc::Client实例的指针

	public void create(String serverName, int blockSize) {
		nativeClient = FastIPCNative.createClient(serverName, blockSize);
	}

	public void write(int userMsgType, long userValue, String userShortStr, String data) {
		// System.out.println("Writed msgId=" + userMsgType + " userValue=" + userValue + " taskId=" + userShortStr + " msg=" + data);
		FastIPCNative.write(nativeClient, userMsgType, userValue, userShortStr, data);
	}

	public void close() {
		FastIPCNative.closeClient(nativeClient);
	}
}