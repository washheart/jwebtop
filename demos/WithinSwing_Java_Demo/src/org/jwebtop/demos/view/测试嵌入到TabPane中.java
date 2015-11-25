package org.jwebtop.demos.view;

import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTextArea;

import org.jwebtop.JWebTopBrowser;
import org.jwebtop.JWebTopConfigs;
import org.jwebtop.JWebTopContext;
import org.jwebtop.listener.JWebTopAppInited;
import org.jwebtop.listener.JWebTopBrowserCreated;
import org.jwebtop.listener.JWebtopJSONDispater;

public class 测试嵌入到TabPane中 extends JFrame {
	private JTabbedPane tabs;
	private JWebTopBrowser baidu;
	private JWebTopBrowser qq;
	private JWebTopContext ctx;

	public 测试嵌入到TabPane中() {
		super.setUndecorated(true);
		// AWTUtilities.setWindowOpaque(this, false);

		String path = "JWebTop.exe";
		this.tabs = new JTabbedPane();
		this.baidu = new JWebTopBrowser();
		this.qq = new JWebTopBrowser();
		this.tabs.add(new JScrollPane(new JTextArea("一个Swing界面")), "Swing");
		this.tabs.add(this.baidu, "百度");
		this.tabs.add(this.qq, "腾讯");
		this.baidu.setTopWindow(this);
		this.qq.setTopWindow(this);
		this.add(tabs);
		this.setSize(800, 600);
		this.setTitle("测试嵌入到TabPane中");
		this.setVisible(true);
		this.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		this.ctx = new JWebTopContext();
		ctx.setJsonHandler(new JWebtopJSONDispater() {
			@Override
			public void resetThreadClassLoader() {}

			@Override
			public String dispatcher(long browserHWnd, String json) {
				return null;
			}
		});
		ctx.createJWebTopByCfgFile(path, null, new JWebTopAppInited() {
			@Override
			public void onJWebTopAppInited() {
				createBrowsers();
			}
		});
	}

	protected void createBrowsers() {
		JWebTopConfigs baiduConfig = new JWebTopConfigs();
		baiduConfig.setUrl("http://www.baidu.com");
		this.baidu.createInernalBrowser(ctx, baiduConfig, new JWebTopBrowserCreated() {
			@Override
			public void onJWebTopBrowserCreated(long browserHWnd) {
				// System.out.println("browserHWnd = " + browserHWnd);
				// String string = "addEventListener('JWebTopReady',function(){JWebTop.showDev();});";
				// ctx.executeJS_NoWait(browserHWnd, string);
			}
		});
		JWebTopConfigs qqConfig = new JWebTopConfigs();
		qqConfig.setUrl("http://www.qq.com");
		this.qq.createInernalBrowser(ctx, qqConfig, new JWebTopBrowserCreated() {
			@Override
			public void onJWebTopBrowserCreated(long browserHWnd) {
				// String string = "addEventListener('JWebTopReady',function(){JWebTop.showDev();});";
				// ctx.executeJS_NoWait(browserHWnd, string);
			}
		});
	}

	public static void test() {
		new 测试嵌入到TabPane中();
	}
}
