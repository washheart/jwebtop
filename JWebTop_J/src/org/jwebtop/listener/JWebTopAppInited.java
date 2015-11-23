package org.jwebtop.listener;

/**
 * 用来响应调用浏览器JS后，响应调用后的执行结果<br>
 * 源码：https://github.com/washheart/jwebtop<br>
 * 说明：https://github.com/washheart/jwebtop/wiki<br>
 * 
 * @author washheart@163.com
 */
public interface JWebTopAppInited {
	/**
	 * 当JWebTop进程成功启动后调用，只有在此时间点之后，才可以构建浏览器
	 */
	void onJWebTopAppInited();
}