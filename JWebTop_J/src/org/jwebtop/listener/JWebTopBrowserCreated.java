package org.jwebtop.listener;

/**
 * 当浏览器被成功创建后，会调用此侦听器<br>
 * 源码：https://github.com/washheart/jwebtop<br>
 * 说明：https://github.com/washheart/jwebtop/wiki<br>
 * 
 * @author washheart@163.com
 */
public interface JWebTopBrowserCreated {
	/**
	 * 当指定浏览器被创建后调用
	 * 
	 * @param browserHWnd
	 *            成功创建的浏览器的窗口句柄
	 */
	void onJWebTopBrowserCreated(long browserHWnd);
}