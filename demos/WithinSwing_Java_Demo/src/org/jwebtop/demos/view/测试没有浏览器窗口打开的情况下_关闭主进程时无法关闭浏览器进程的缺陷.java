package org.jwebtop.demos.view;

import org.jwebtop.JWebTopConfigs;
import org.jwebtop.JWebTopContext;
import org.jwebtop.WindowStyle;
import org.jwebtop.listener.JWebTopAppInited;
import org.jwebtop.listener.JWebTopBrowserCreated;
import org.jwebtop.listener.JWebtopJSONDispater;

public class 测试没有浏览器窗口打开的情况下_关闭主进程时无法关闭浏览器进程的缺陷 {
	public static void test() {
		final JWebTopContext ctx = new JWebTopContext();
		ctx.setJsonHandler(new JWebtopJSONDispater() {
			@Override
			public void resetThreadClassLoader() {}

			@Override
			public String dispatcher(long browserHWnd, String json) {
				return null;
			}
		});
		String path = "JWebTop.exe";
		ctx.createJWebTopByCfgFile(path, null, new JWebTopAppInited() {
			@Override
			public void onJWebTopAppInited() {
				JWebTopConfigs configs = new JWebTopConfigs();
				configs.setUrl("http://www.baidu.com");
				long style = WindowStyle.WS_EX_TOOLWINDOW | WindowStyle.WS_VISIBLE;
				configs.setDwStyle(style);
				configs.setMax(0);
				configs.setW(400);
				configs.setH(400);
				ctx.createBrowser(configs, new JWebTopBrowserCreated() {
					@Override
					public void onJWebTopBrowserCreated(long browserHWnd) {
						System.out.println("browserHWnd = " + browserHWnd);
					}
				});
			}
		});
	}
}
