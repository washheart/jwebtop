package org.jwebtop.listener;

/**
 * 用来响应浏览器的JSON调用<br>
 * 源码：https://github.com/washheart/jwebtop<br>
 * 说明：https://github.com/washheart/jwebtop/wiki<br>
 * 
 * @author washheart@163.com
 */
public interface JWebtopJSONDispater {

	/**
	 * 处理浏览器发来的json内容，并返回结果
	 * 
	 * @param browserHWnd
	 *            需要执行json的浏览器句柄
	 * @param json
	 *            浏览器端发来的要执行内容
	 * @return 本地处理后的结果
	 */
	String dispatcher(long browserHWnd, String json);

	/**
	 * 在Dispatcher之前，重新设置当前线程的类加载器。<br/>
	 * 否则会使用BootstrapClassLoader来加载类，这样新加载类getClassLoader时会返回null，导致无法继续记载资源。<br/>
	 * 一般可以这样实现：首先记录JWebtopJSONDispater实现类的加载器，然后在resetThreadClassLoader方法设置。<br/>
	 * 如果使用了插件系统、或者实现了其他加载方式，也需要在resetThreadClassLoader方法中处理好。<br/>
	 * 下面是一个实现示例：
	 * 
	 * <pre>
	 * private ClassLoader DEFAULT = SomeJSONDispater.class.getClassLoader();
	 * 
	 * &#064;Override
	 * public void resetThreadClassLoader() {
	 * 	Thread.currentThread().setContextClassLoader(DEFAULT);
	 * }
	 * </pre>
	 */
	void resetThreadClassLoader();
}