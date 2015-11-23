package org.jwebtop.listener;

/**
 * 用来响应调用浏览器JS后，响应调用后的执行结果<br>
 * 源码：https://github.com/washheart/jwebtop<br>
 * 说明：https://github.com/washheart/jwebtop/wiki<br>
 * 
 * @author washheart@163.com
 */
public interface JWebTopJSReturn {
	/**
	 * js执行后的执行结果
	 * 
	 * @param uuid
	 *            执行JS时传递的uuid，根据此主要用于一个JWebTopJSReturn中处理多个JS返回结果的情况
	 * @param jsonString
	 *            执行结果，绝大多数情况下是一个JSON字符串
	 */
	void onJWebTopJSReturn(String uuid, String jsonString);
}