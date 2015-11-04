package org.fastipc;

/**
 * 服务端侦听器，当客户端有数据写入时，会触发。<br/>
 * 源码：https://github.com/washheart/fastipc<br/>
 * 源码：http://git.oschina.net/washheart/fastipc<br/>
 * 说明：https://github.com/washheart/fastipc/wiki<br/>
 * 
 * @author washheart@163.com
 */
public interface FastIPCReadListener {

	void OnRead(int msgType, String packId, int userMsgType, long userValue, String userShortStr, byte[] data);
}