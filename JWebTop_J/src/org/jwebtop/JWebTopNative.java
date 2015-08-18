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
	private static native void nExit();

	private static native void nCreateJWebTop(String appfile, long parenHwnd, String url, String title, String icon, int x, int y, int w, int h);

	private static native String nExecuteJSONWait(long browserHWnd, String jsonstring);

	private static native void nExecuteJSONNoWait(long browserHWnd, String jsonstring);

	private static native String nExecuteJSWait(long browserHWnd, String script);

	private static native void nExecuteJSNoWait(long browserHWnd, String script);

	private static native void nSetSize(long browserHwnd, int w, int h);

	private static native void nSetLocation(long browserHwnd, int xOnScreen, int yOnScreen);

	private static native void nSetBound(long browserHwnd, int xOnScreen, int yOnScreen, int w, int h);

	// private static native void nSetUrl(long browserHwnd, String url);

	private final static JWebTopNative INSTANCE = new JWebTopNative();

	private final class CreateBrowserLocker {
		long hwnd = 0L;
	}

	private boolean waitLock = false;
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
		return createJWebTop(appfile, parenHwnd, null, null, null, -1, -1, -1, -1);
	}

	public long createJWebTop(String appfile, final long parenHwnd
	// 以下参数会替换appfile中的相应参数
			, final String url // --------要打开的链接地址
			, final String title // ------窗口名称
			, final String icon // -------窗口图标
			, final int x, final int y // 窗口左上角坐标,当值为-1时不启用此变量
			, final int w, final int h // 窗口的宽、高，当值为-1时不启用此变量
	) throws IOException {
		final String appfile2 = new File(appfile).getCanonicalPath();// 如果不是绝对路径，浏览器无法显示出来
		Runnable creator = new Runnable() {
			@Override
			public void run() {
				System.out.println("准备调用JNI");
				nCreateJWebTop(appfile2, parenHwnd, url, title, icon, x, y, w, h);
			}
		};
		// SwingUtilities.invokeLater(creator);// 如果是在UI中调用此方法，那么这里会陷入无限等待，因为当前线程会锁死在lock的等待上
		new Thread(creator).start();
		System.out.println("已调用创建浏览器方法=");
		synchronized (locker) {
			try {
				waitLock = true;
				System.out.println("已调用创建浏览器方法，waitLock=" + waitLock);
				locker.wait();
				System.out.println("已调用创建浏览器方法，waitLock=------------");
				waitLock = false;
			} catch (InterruptedException e) {
				e.printStackTrace();
			} finally {
				waitLock = false;
			}
		}
		return locker.hwnd;
	}

	public long createJWebTop(String appfile) throws IOException {
		return createJWebTop(appfile, 0);
	}

	// 关闭JWebTop的进程
	public void exit() {
		nExit();
	}

	/**
	 * 执行js，发送后等待数据的返回。<br/>
	 * 这里发送是包装的JSON数据，其调用JS脚本中的invokeByDLL(jsonvalue)方法
	 * 
	 * @param jsonstring
	 * @return 返回是JSON字符串数据
	 */
	public static String executeJSON_Wait(long browserHWnd, String jsonstring) {
		return nExecuteJSONWait(browserHWnd, jsonstring);
	}

	/**
	 * 执行js，但不等待数据的返回。<br/>
	 * 这里发送是包装的JSON数据，其调用JS脚本中的invokeByDLL(jsonvalue)方法
	 * 
	 * @param jsonstring
	 */
	public static void executeJSON_NoWait(long browserHWnd, String jsonstring) {
		nExecuteJSONNoWait(browserHWnd, jsonstring);
	}

	/**
	 * 执行js，并等待数据的返回。<br/>
	 * 推荐使用executeJSON_Wait方法，以减少字符串转换的问题
	 * 
	 * @param script
	 * @return 返回是字符串数据
	 */
	public static String executeJS_Wait(long browserHWnd, String script) {
		return nExecuteJSWait(browserHWnd, script);
	}

	/**
	 * 执行js，但不等待数据的返回。 <br/>
	 * 推荐使用executeJSON_NoWait方法，以减少字符串转换的问题
	 * 
	 * @param script
	 * @return
	 */
	public static void executeJS_NoWait(long browserHWnd, String script) {
		nExecuteJSNoWait(browserHWnd, script);
	}

	String dispatch(String json) throws IOException {
		if (json.startsWith("@")) {// 约定通过特殊标记来进行处理
			json = json.substring(1);
			long rootBrowserHwnd = 0;
			JsonFactory f = new JsonFactory();
			JsonParser p = f.createParser(json);
			JsonToken result = null;
			while ((result = p.nextToken()) != null) {
				if (result == JsonToken.FIELD_NAME) {
					String field = p.getText();
					p.nextToken();
					if ("hwnd".equals(field)) {
						rootBrowserHwnd = p.getLongValue();
						break;
					}
				}
			}
			p.close();
			System.out.println("waitLock=" + waitLock + "\t " + rootBrowserHwnd);
			if (waitLock) synchronized (locker) {
				locker.hwnd = rootBrowserHwnd;
				locker.notify();// FIXME:通知解锁的时机需要控制
			}
		}
		if (jsonHandler != null) {
			return jsonHandler.dispatcher(json);
		}
		return "";
	}

	public void setJsonHandler(JWebtopJSONDispater jsonHandler) {
		this.jsonHandler = jsonHandler;
	}

	/**
	 * 此方法不可以被混淆，其会被DLL调用
	 * 
	 * @param json
	 * @return
	 */
	private static String invokeByJS(String json) {
		try {
			System.out.println("从dll端发起的调用 = " + json);
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
		// System.out.println("browserHwnd = " + browserHwnd + " =[w=" + w + ",h=" + h + "]");
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
	 * 移动并重设大小(性能较高， 但此方法有时有点问题：刷新不及时)
	 * 
	 * @param browserHwnd
	 * @param xOnScreen
	 * @param yOnScreen
	 * @param w
	 * @param h
	 */
	public static void setBound(long browserHwnd, int xOnScreen, int yOnScreen, int w, int h) {
		if (browserHwnd != 0) nSetBound(browserHwnd, xOnScreen, yOnScreen, w, h);
	}

	// public static void setUrl(long browserHwnd, String url) {
	// if (browserHwnd != 0) nSetUrl(browserHwnd, url);
	// }

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