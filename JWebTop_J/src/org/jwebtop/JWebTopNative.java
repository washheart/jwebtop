package org.jwebtop;

import java.awt.Component;
import java.io.File;
import java.io.IOException;

import com.fasterxml.jackson.core.JsonFactory;
import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.core.JsonToken;

/**
 * 与JWebTop交互的JNI接口java端
 * 
 * @author washheart@163.com
 */
public final class JWebTopNative {
	private static native void nCreateJWebTop(String appfile, long parenHwnd);

	private static native void nExecuteJs(long broserHWnd, String json);

	private static native void nSetSize(long browserHwnd, int i, int j);

	private static native void nSetLocation(long browserHwnd, int xOnScreen, int yOnScreen);

	private static native void nMove(long browserHwnd, int xOnScreen, int yOnScreen, int w, int h);

	private final static JWebTopNative INSTANCE = new JWebTopNative();

	private long rootBrowserHwnd = 0L;

	private final class CreateBrowserLocker {
		long hwnd = 0L;
	}

	private final CreateBrowserLocker locker = new CreateBrowserLocker();
	private JWebtopJSONDispater jsonHandler = null;

	public static JWebTopNative getInstance() {
		return INSTANCE;
	}

	private JWebTopNative() {}

	/**
	 * 创建JWebTop浏览器
	 * 
	 * @param appfile
	 * @return
	 */
	public long createJWebTop(String appfile, final long parenHwnd) throws IOException {
		final String appfile2 = new File(appfile).getCanonicalPath();// 如果不是绝对路径，浏览器无法显示出来
		new Thread() {
			@Override
			public void run() {
				nCreateJWebTop(appfile2, parenHwnd);
			}
		}.start();
		synchronized (locker) {
			try {
				locker.wait();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		return locker.hwnd;
	}

	public long createJWebTop(String appfile) throws IOException {
		return createJWebTop(appfile, 0);
	}

	/**
	 * 执行js。<br/>
	 * 这里发送是包装的JSON数据，而不是具体的js方法，返回的也是JSON数据
	 * 
	 * @param json
	 * @return
	 */
	public static void executeJs(long browserHWnd, String script) {
		nExecuteJs(browserHWnd, script);
	}

	String dispatch(String json) throws IOException {
		if (json.startsWith("@")) {// 约定通过特殊标记来进行处理
		}
		if (rootBrowserHwnd == 0) {
			JsonFactory f = new JsonFactory();
			JsonParser p = f.createParser(json);
			JsonToken result = null;
			while ((result = p.nextToken()) != null) {
				if (result == JsonToken.FIELD_NAME) {
					String field = p.getText();
					p.nextToken();
					if ("hwnd".equals(field)) {
						rootBrowserHwnd = p.getLongValue();
						synchronized (locker) {
							locker.hwnd = rootBrowserHwnd;
							locker.notify();
						}
					}
				}
			}
			p.close();
		} else {
			if (jsonHandler != null) {
				String rtn = jsonHandler.dispatcher(json);
				System.out.println("rtn = " + rtn);
				return rtn;
			}
		}
		return "";
	}

	public void setJsonHandler(JWebtopJSONDispater jsonHandler) {
		this.jsonHandler = jsonHandler;
	}

	public long getRootBrowserHwnd() {
		return rootBrowserHwnd;
	}

	/**
	 * 此方法不可以被混淆，其会被DLL调用
	 * 
	 * @param json
	 * @return
	 */
	private static String invokeByJS(String json) {
		try {
			// StringBuilder sb = new StringBuilder(json);
			System.out.println("从dll端发起的调用 = " + json);
			// INSTANCE.dispatch(json);
			// sb.reverse();
			// System.out.println("\t\t准备返回的结果 = " + sb);
			// return sb.toString();
			return INSTANCE.dispatch(json);
		} catch (Throwable e) {
			e.printStackTrace();
			return "{success:false,msg:\"调用Java失败" + e.getMessage() + "\"}";
		}
	}

	/**
	 * 设置指定窗口的大小
	 * 
	 * @param browserHwnd
	 * @param w
	 * @param h
	 */
	public static void setSize(long browserHwnd, int w, int h) {
		if (browserHwnd != 0) nSetSize(browserHwnd, w, h);
	}

	/**
	 * 设置指定窗口的位置
	 * 
	 * @param browserHwnd
	 * @param xOnScreen
	 * @param yOnScreen
	 */
	public static void setLocation(long browserHwnd, int xOnScreen, int yOnScreen) {
		if (browserHwnd != 0) nSetLocation(browserHwnd, xOnScreen, yOnScreen);
	}

	/**
	 * 移动并重设大小
	 * 
	 * @param browserHwnd
	 * @param xOnScreen
	 * @param yOnScreen
	 * @param w
	 * @param h
	 */
	public static void move(long browserHwnd, int xOnScreen, int yOnScreen, int w, int h) {
		if (browserHwnd != 0) nMove(browserHwnd, xOnScreen, yOnScreen, w, h);
	}

	/**
	 * 得到某Java控件对应的句柄HWND
	 * 
	 * @param target
	 *            给定的控件
	 * @return 控件windowsID
	 */
	public static long getWindowHWND(Component target) {
		return ((sun.awt.windows.WComponentPeer/* 此类来自JDK，纯jre不行 */) target.getPeer()).getHWnd();
	}

}