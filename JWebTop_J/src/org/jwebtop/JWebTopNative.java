package org.jwebtop;

import java.awt.Component;
import java.awt.peer.ComponentPeer;

/**
 * 操作Windows窗口的JNI接口java端 <br>
 * 源码：https://github.com/washheart/jwebtop<br>
 * 说明：https://github.com/washheart/jwebtop/wiki<br>
 * 
 * @author washheart@163.com
 */
public final class JWebTopNative {

	private static native long nSetActiveWindow(long hWnd);

	private static native long nCreateSubProcess(String subProcess, String szCmdLine, boolean waitFor);

	private static native long nGetProcessID();

	private static native void nSetSize(long browserHwnd, int w, int h);

	private static native int[] nGetSize(long browserHwnd);

	private static native int[] nGetScreenSize();

	private static native void nSetLocation(long browserHwnd, int xOnScreen, int yOnScreen);

	private static native int[] nGetLocation(long browserHwnd);

	private static native void nSetBound(long browserHwnd, int xOnScreen, int yOnScreen, int w, int h);

	private static native int[] nGetBound(long browserHwnd);

	private static native int[] nGetWindowClient(long browserHwnd);

	private static native void nBringToTop(long browserHWnd);

	private static native void nFocus(long browserHWnd);

	private static native void nHide(long browserHWnd);

	private static native void nMax(long browserHWnd);

	private static native void nMini(long browserHWnd);

	private static native void nRestore(long browserHWnd);

	private static native boolean nIsVisible(long browserHWnd);

	private static native void nSetWindowStyle(long browserHWnd, int style);

	private static native void nSetWindowExStyle(long browserHWnd, int exStyle);

	private static native void nSetTopMost(long browserHWnd);

	private JWebTopNative() {}

	public static void setActiveWindow(long hWnd) {
		nSetActiveWindow(hWnd);
	}

	/**
	 * 创建一个新进程
	 * 
	 * @param subProcess
	 *            新进程执行的可执行文件
	 * @param szCmdLine
	 *            命令行参数
	 * @param waitFor
	 *            是否等待进程结束再返回
	 * @return waitFor=true时返回0，否则返回的数据为进程中主线程的id
	 */
	public static long createSubProcess(String subProcess, String szCmdLine, boolean waitFor) {
		return nCreateSubProcess(subProcess, szCmdLine, waitFor);
	}

	public static long getProcessID() {
		return nGetProcessID();
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
	 * 得到指定窗口的大小
	 * 
	 * @param browserHwnd
	 * @return int[0]=宽，int[1]=高
	 */
	public static int[] getSize(long browserHwnd) {
		return nGetSize(browserHwnd);
	}

	/**
	 * 得到屏幕可用区域的大小
	 * 
	 * @return int[0]=宽，int[1]=高
	 */
	public static int[] getScreenSize() {
		return nGetScreenSize();
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
	 * 得到窗口在屏幕上的位置
	 * 
	 * @param browserHwnd
	 * @return int[0]=x，int[1]=y
	 */
	public static int[] getLocation(long browserHwnd) {
		return nGetLocation(browserHwnd);
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

	/**
	 * 得到窗口在屏幕的坐标信息
	 * 
	 * @param browserHwnd
	 * @return int[0]=x，int[1]=y，int[2]=宽，int[3]=高
	 */
	public static int[] getBound(long browserHwnd) {
		return nGetBound(browserHwnd);
	}

	// 窗口移到最顶层
	public static void bringToTop(long browserHWnd) {
		nBringToTop(browserHWnd);
	}

	// 使窗口获得焦点
	public static void focus(long browserHWnd) {
		nBringToTop(browserHWnd);
	}

	// 隐藏窗口
	public static void hide(long browserHWnd) {
		nHide(browserHWnd);
	}

	// 最大化窗口
	public static void max(long browserHWnd) {
		nMax(browserHWnd);
	}

	// 最小化窗口
	public static void mini(long browserHWnd) {
		nMini(browserHWnd);
	}

	// 还原窗口，对应于hide函数
	public static void restore(long browserHWnd) {
		nRestore(browserHWnd);
	}

	// 检查窗口是否正在显示状态
	public static boolean isVisible(long browserHWnd) {
		return nIsVisible(browserHWnd);
	}

	// 窗口置顶，此函数跟bringToTop的区别在于此函数会使窗口永远置顶，除非有另外一个窗口调用了置顶函数
	public static void setTopMost(long browserHWnd) {
		nSetTopMost(browserHWnd);
	}

	public static void setWindowStyle(long browserHWnd, int style) {
		nSetWindowStyle(browserHWnd, style);
	}

	public static void setWindowExStyle(long browserHWnd, int exStyle) {
		nSetWindowExStyle(browserHWnd, exStyle);
	}

	/**
	 * 得到某Java控件对应的句柄HWND
	 * 
	 * @param target
	 *            给定的控件
	 * @return 控件windowsID
	 */
	public static long getWindowHWND(Component target) {
		ComponentPeer peer = target.getPeer();
		if (peer == null) return 0;
		return ((sun.awt.windows.WComponentPeer/* 此类来自JDK，纯jre不行 */) peer).getHWnd();
	}

	/**
	 * 获取指定窗口的客户区坐标信息，返回大小为4的数组，依次为右左上角x、y、右下角x、y
	 * 
	 * @param hWnd
	 *            窗口句柄
	 * @return int[left,top,right,bottom]
	 */
	public static int[] getWindowClient(long hWnd) {
		return nGetWindowClient(hWnd);
	}
}