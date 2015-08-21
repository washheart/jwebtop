package org.jwebtop;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.io.File;

import javax.swing.AbstractAction;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JTextArea;

import org.jwebtop.JWebTopBrowser.IBrowserHwndFeeder;

public class TestJWebTop {
	private static void initDll(String[] args) {
		String dll = "JWebTop.dll";
		File tmp = null;
		if (args.length > 0) {
			args[0] = args[0].trim();
			tmp = new File(args[0]);
			dll = tmp.getAbsolutePath();
		} else {
			tmp = new File(dll);
			if (tmp.isFile()) {
				dll = tmp.getAbsolutePath();
			}
		}
		System.out.println("成功加载dll文件 = " + dll);
		System.load(dll);
	}

	// static void test_createBrower(String appfile) throws IOException {
	// System.out.println("准备执行appfile=" + appfile);
	// JWebTopNative.getInstance().createJWebTop(appfile);
	// }
	//
	// static void test_createBrower_multi(String appfile) throws IOException {
	// class CreateBrowserThread extends Thread {
	// private String appfile;
	//
	// CreateBrowserThread(String appfile) {
	// this.appfile = appfile;
	// }
	//
	// @Override
	// public void run() {
	// System.out.println("准备执行appfile=" + appfile);
	// try {
	// JWebTopNative.getInstance().createJWebTop(appfile);
	// } catch (IOException e) {
	// e.printStackTrace();
	// }
	// }
	// }
	// // Crash? 估计是浏览器内部有些静态变量没有维护好
	// new CreateBrowserThread(appfile).start();
	// new CreateBrowserThread(appfile).start();
	// }

	protected static long RootBrowserHwnd;

	// 测试将浏览器作为一个控件
	static void test_createBrowserAsComponnet(final String appfile) {
		final JTextArea txtJson = new JTextArea();
		final JFrame jf = new JFrame("测试窗口");
		jf.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		JPanel toolPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
		toolPanel.add(new JButton(new AbstractAction("显示窗口hwnd") {
			@Override
			public void actionPerformed(ActionEvent e) {
				long hwnd = JWebTopNative.getWindowHWND(jf);
				System.out.println("hwnd = " + hwnd);
			}
		}));
		final JWebTopNative webtop = JWebTopNative.getInstance();
		webtop.setJsonHandler(new JWebtopJSONDispater() {
			@Override
			public String dispatcher(String json) {
				return "{\"a\":1,javaconvert:\"" + json + "\",b:2}";
			}
		});
		final JWebTopBrowser jwebtop = new JWebTopBrowser();
		toolPanel.add(new JButton(new AbstractAction("创建浏览器") {
			@Override
			public void actionPerformed(ActionEvent e) {
				long hwnd = JWebTopNative.getWindowHWND(jf);
				System.out.println("hwnd = " + hwnd);
				try {
					webtop.createJWebTop("JWebTop.exe", appfile);
					RootBrowserHwnd = jwebtop.createInernalBrowser(appfile, null, null, null);
					System.out.println("jwebtop.getLocationOnScreen() = " + jwebtop.getLocationOnScreen());
					// if (jwebtop.isShowing()) jwebtop.setBrowserLocation(jwebtop.getLocationOnScreen());
				} catch (Throwable e1) {
					e1.printStackTrace();
				}
			}
		}));
		toolPanel.add(new JButton(new AbstractAction("执行JS") {
			@Override
			public void actionPerformed(ActionEvent e) {
				new Thread() {
					@Override
					public void run() {
						String json = txtJson.getText().trim();
						System.out.println("准备执行JS脚本 = " + json);
						// System.out.println("      执行结果=" + JWebTopNative.executeJs(0L, script));
						JWebTopNative.executeJSON_NoWait(RootBrowserHwnd, json);
					}
				}.start();
			}
		}));
		JSplitPane mainPanel = new JSplitPane(JSplitPane.VERTICAL_SPLIT);
		mainPanel.setDividerLocation(200);
		JScrollPane jspScript = new JScrollPane(txtJson);
		jspScript.setBorder(BorderFactory.createTitledBorder("JavaScript脚本"));
		mainPanel.add(jspScript, JSplitPane.TOP);
		jwebtop.setBrowserHwndFeeder(new IBrowserHwndFeeder() {
			@Override
			public long getBrowserHwnd() {
				return RootBrowserHwnd;
			}
		});
		jwebtop.setTopWindow(jf);
		mainPanel.add(jwebtop, JSplitPane.BOTTOM);

		jf.getContentPane().add(toolPanel, BorderLayout.NORTH);
		jf.getContentPane().add(mainPanel, BorderLayout.CENTER);
		jf.setSize(800, 600);
		jf.setVisible(true);
	}

	public static void main(String[] args) {
		try {
			// System.in.read();// 用于调试dll时附加进程使用
			initDll(args);
			String appfile = args[1].trim();
			// test_createBrower(appfile);
			// test_createBrower_multi(appfile);
			test_createBrowserAsComponnet(appfile);
		} catch (Throwable e) {
			e.printStackTrace();
		}
	}
}