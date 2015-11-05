package org.jwebtop;

/**
 * 抛出使用JWebTop时可能出现的各种异常情况<br>
 * 源码：https://github.com/washheart/jwebtop<br>
 * 说明：https://github.com/washheart/jwebtop/wiki<br>
 * 
 * @author washheart@163.com
 */
public class JWebTopException extends RuntimeException {

	public JWebTopException() {
		super();
		init();
	}

	public JWebTopException(String message, Throwable cause) {
		super(message, cause);
		init();
	}

	public JWebTopException(String message) {
		super(message);
		init();
	}

	public JWebTopException(Throwable cause) {
		super(cause);
		init();
	}

	private void init() {}
}
