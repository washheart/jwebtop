package org.jwebtop.demos.view;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.WindowEvent;
import java.io.File;

import javax.swing.AbstractAction;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JSplitPane;

import org.jwebtop.JWebTopBrowser;
import org.jwebtop.JWebTopConfigs;
import org.jwebtop.JWebTopContext;
import org.jwebtop.JWebTopNative;
import org.jwebtop.WindowStyle;
import org.jwebtop.demos.ctrl.WithinSwingCtrl;
import org.jwebtop.demos.ctrl.WithinSwingCtrl.WithinSwingCtrlHelper;
import org.jwebtop.listener.JWebTopAppInited;
import org.jwebtop.listener.JWebTopBrowserCreated;

/**
 * 测试嵌入浏览器到Swing窗口的例子<br/>
 * 左侧是一个列表浏览器窗口，右侧是一个列表详细说明浏览器窗口<br>
 * 源码：https://github.com/washheart/jwebtop<br>
 * 说明：https://github.com/washheart/jwebtop/wiki<br>
 * 
 * @author washheart@163.com
 */
public class WithinSwing extends JFrame implements WithinSwingCtrlHelper, WindowStyle {
	protected static long RootBrowserHwnd = 0;

	private static File initDll(String[] args) {
		String dll = "JWebTop.dll";
		File tmp = null;
		args = new String[] { "" };
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
		if (tmp.isFile()) tmp = tmp.getParentFile();
		JWebTopContext.initDLL(tmp.getAbsolutePath());
		return tmp;
	}

	// 一般情况下一个独立的应用中有一个JWebTopContext即可，一个JWebTopContext可以创建多个Browser
	public JWebTopContext ctx = new JWebTopContext();
	private JWebTopBrowser listWebtopView, detailWebtopView;
	private WithinSwingCtrl ctrl;
	private long modalBrowserDlgHWnd;
	private String appFile;
	public static long frameHwnd;

	private void createDlg(JFrame f) {
		JWebTopConfigs configs = new JWebTopConfigs();
		configs.setParentWin(JWebTopNative.getWindowHWND(f));
		configs.setUrl("http://www.baidu.com");
		long style = WS_EX_TOOLWINDOW | WS_VISIBLE;
		configs.setDwStyle(style);
		configs.setMax(0);
		configs.setW(400);
		configs.setH(400);
		ctx.createBrowser(configs, new JWebTopBrowserCreated() {
			@Override
			public void onJWebTopBrowserCreated(long browserHWnd) {
				modalBrowserDlgHWnd = browserHWnd;
			}
		});
		f.setEnabled(false);// 将主窗口设置为不可用：相当于对话框是模态的
	}

	@Override
	public void browserClosed(long browserHWnd) {
		if (modalBrowserDlgHWnd == browserHWnd) {
			this.setEnabled(true);// 将主窗口设置为可用
			modalBrowserDlgHWnd = 0;
		}
	}

	public WithinSwing() {
		this.setTitle("测试嵌入两个浏览器窗口到Swing中");
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.ctrl = new WithinSwingCtrl(this);
		this.ctrl.setWithinSwingCtrlHelper(this);
		ctx.setJsonHandler(this.ctrl);
		JPanel toolPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
		toolPanel.add(new JButton(new AbstractAction("新增日记") {
			@Override
			public void actionPerformed(ActionEvent e) {
				addNote();
			}
		}));
		toolPanel.add(new JButton(new AbstractAction("删除日记") {
			@Override
			public void actionPerformed(ActionEvent e) {
				delNote();
			}
		}));
		toolPanel.add(new JButton(new AbstractAction("打开一个Modal浏览器") {
			@Override
			public void actionPerformed(ActionEvent e) {
				createDlg(WithinSwing.this);
			}
		}));
		JSplitPane mainPanel = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT);
		mainPanel.setDividerLocation(200);
		this.listWebtopView = new JWebTopBrowser();
		this.listWebtopView.setTopWindow(this);
		this.detailWebtopView = new JWebTopBrowser();
		this.detailWebtopView.setTopWindow(this);
		mainPanel.add(this.listWebtopView, JSplitPane.LEFT);
		mainPanel.add(this.detailWebtopView, JSplitPane.BOTTOM);

		this.getContentPane().add(toolPanel, BorderLayout.NORTH);
		this.getContentPane().add(mainPanel, BorderLayout.CENTER);
		this.setSize(800, 600);
		this.setVisible(true);
		// 创建浏览器
		try {
			// JOptionPane.showMessageDialog(this, "wait");
			WithinSwing.frameHwnd = JWebTopNative.getWindowHWND(this);
			// JWebTopContext.WIN_HWND = WithinSwing.frameHwnd;

			System.out.println("Java窗口handler = " + WithinSwing.frameHwnd + " hex=0x" + Long.toHexString(WithinSwing.frameHwnd));
			String path = "JWebTop.exe";
			System.out.println("\tpath = " + path);
			this.appFile = ctrl.getListAppFile();
			System.out.println("\tappFile = " + appFile);
			ctx.createJWebTopByCfgFile(path, appFile, new JWebTopAppInited() {
				@Override
				public void onJWebTopAppInited() {
					jwebtopContextInited();
				}
			});
			// JWebTopNative.createJWebTop(path, appFile);
		} catch (Throwable e) {
			e.printStackTrace();
		}
	}

	@Override
	public void detailCreated(long browserHWnd) {
		detailWebtopView.setBrowserHwnd(browserHWnd);
	}

	@Override
	public int[] getDetailRect() {
		Point p = detailWebtopView.calcBrowserLocation();
		Dimension s = detailWebtopView.getSize();
		return new int[] { p.x, p.y, s.width, s.height };
	}

	// 新增日记
	protected void addNote() {
		String name = JOptionPane.showInputDialog(this, "请输入名称：");
		if (name == null) return;
		name = name.trim();
		if (name.length() == 0) return;
		ctrl.addNote(name);
	}

	// 删除日记
	protected void delNote() {
		String note = ctrl.getCurrentNote();
		if (note == null) return;
		if (JOptionPane.showConfirmDialog(this, "是否删除【" + note + "】日记？") != JOptionPane.YES_OPTION) return;
		ctrl.delNote();
	}

	@Override
	protected void processWindowEvent(WindowEvent e) {
		if (e.getID() == WindowEvent.WINDOW_CLOSING) {
			if (JOptionPane.showConfirmDialog(this, "确认退出系统吗?", "退出系统", JOptionPane.OK_CANCEL_OPTION) == JOptionPane.OK_OPTION) {
				ctrl.notifyWillClose();
				ctx.closeContext();
				System.exit(0);//
			} else {
				return;
			}
		}
		super.processWindowEvent(e);
	}

	public void jwebtopContextInited() {
		listWebtopView.createInernalBrowser(ctx, appFile, null, "列表页", null, new JWebTopBrowserCreated() {
			@Override
			public void onJWebTopBrowserCreated(long browserHWnd) {
				RootBrowserHwnd = browserHWnd;
				System.out.println("浏览器窗口handler = " + RootBrowserHwnd + " hex=0x" + Long.toHexString(RootBrowserHwnd));
				ctrl.setListHandler(RootBrowserHwnd);
			}
		});
	}

	public static String arrayToString(byte[] array) {
		final String nullText = "null";
		if (array == null) {
			return nullText;
		}
		int length = array.length;
		if (length == 0) return "[]";
		final String arraySeparator = ",";
		StringBuffer buffer = new StringBuffer("[");
		for (int i = 0; i < length; i++) {
			Object item = array[i];
			buffer.append((item == null) ? nullText : item.toString());
			buffer.append(arraySeparator);
		}
		buffer.setCharAt(buffer.length() - 1, ']');
		return buffer.toString();
	}

	public static void main(String[] args) {
		try {
			File dir = initDll(args);
			System.out.println("dir = " + dir.getCanonicalPath());
			// 测试没有浏览器窗口打开的情况下_关闭主进程时无法关闭浏览器进程的缺陷.test();
			// 测试嵌入到TabPane中.test();
			new WithinSwing();
		} catch (Throwable e) {
			e.printStackTrace();
		}
	}
}