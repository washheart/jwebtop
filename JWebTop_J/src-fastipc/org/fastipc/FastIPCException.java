package org.fastipc;

/**
 * FastIPC创建和使用过程中出现的异常。<br/>
 * 源码：https://github.com/washheart/fastipc<br/>
 * 源码：http://git.oschina.net/washheart/fastipc<br/>
 * 说明：https://github.com/washheart/fastipc/wiki<br/>
 * 
 * @author washheart@163.com
 */
public class FastIPCException extends RuntimeException {
	public FastIPCException(String msg) {
		super(msg);
	}
}