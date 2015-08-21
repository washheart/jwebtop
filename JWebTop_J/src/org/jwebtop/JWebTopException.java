package org.jwebtop;

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
