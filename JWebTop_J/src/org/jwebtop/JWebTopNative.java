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

	static long hwnd = 0L;

	private static native void nCreateJWebTop(String appfile, long parenHwnd);

	private static native void nExecuteJs(long broserHWnd, String json);

	private static native void nSetSize(long browserHwnd, int i, int j);

	private static native void nSetLocation(long browserHwnd, int xOnScreen, int yOnScreen);

	/**
	 * 创建JWebTop浏览器
	 * 
	 * @param appfile
	 */
	public static void createJWebTop(String appfile, long parenHwnd) throws IOException {
		appfile = new File(appfile).getCanonicalPath();// 如果不是绝对路径，浏览器无法显示出来
		nCreateJWebTop(appfile, parenHwnd);
	}

	public static void createJWebTop(String appfile) throws IOException {
		createJWebTop(appfile, 0);
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

	/**
	 * 此方法不可以被混淆，其会被DLL调用
	 * 
	 * @param json
	 * @return
	 */
	public static String invokeByJS(String json) {
		try {
			StringBuilder sb = new StringBuilder(json);
			System.out.println("从dll端发起的调用 = " + json);
			print(json);
			sb.reverse();
			System.out.println("\t\t准备返回的结果 = " + sb);
			return sb.toString();
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

	public interface JWebtopJSONHandler {

		void setBrowserHwnd(long hwnd);// FIXME:临时调试用

	}

	public static JWebtopJSONHandler jsonHandler = null;

	static void print(String json) throws IOException {
		JsonFactory f = new JsonFactory();
		JsonParser p = f.createParser(json);
		JsonToken result = null;
		while ((result = p.nextToken()) != null) {
			if (result == JsonToken.FIELD_NAME) {
				String field = p.getText();
				p.nextToken();
				if ("hwnd".equals(field)) {
					hwnd = p.getLongValue();
					if (jsonHandler != null) jsonHandler.setBrowserHwnd(hwnd);
				}
				System.out.println("\t" + field + "=" + p.getText());
			}
		}
		p.close();
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