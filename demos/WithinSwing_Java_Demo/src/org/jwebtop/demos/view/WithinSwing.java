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
import org.jwebtop.JWebTopNative;
import org.jwebtop.WindowStyle;
import org.jwebtop.demos.ctrl.WithinSwingCtrl;
import org.jwebtop.demos.ctrl.WithinSwingCtrl.WithinSwingCtrlHelper;

/**
 * 测试嵌入浏览器到Swing窗口的例子<br/>
 * 左侧是一个列表浏览器窗口，右侧是一个列表详细说明浏览器窗口
 * 
 * @author washheart@163.com
 */
public class WithinSwing extends JFrame implements WithinSwingCtrlHelper, WindowStyle {
	protected static long RootBrowserHwnd = 0;

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

	private JWebTopBrowser listWebtopView, detailWebtopView;
	private WithinSwingCtrl ctrl;
	private long modalBrowserDlgHWnd;
	public static long frameHwnd;

	private void createDlg(JFrame f) {
		// JDialog d = new JDialog(f, "测试嵌入浏览器在对话框中");
		// d.setSize(100, 200);
		// d.setVisible(true);
		// return JWebTopNative.getWindowHWND(d);
		JWebTopConfigs configs = new JWebTopConfigs();
		configs.setParentWin(JWebTopNative.getWindowHWND(f));
		configs.setUrl("http://www.baidu.com");
		long style = WS_EX_TOOLWINDOW | WS_VISIBLE;
		configs.setDwStyle(style);
		configs.setW(400);
		configs.setH(400);
		this.modalBrowserDlgHWnd = JWebTopNative.createBrowser(configs);
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
		this.ctrl = new WithinSwingCtrl();
		this.ctrl.setWithinSwingCtrlHelper(this);
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
		JWebTopNative.setJsonHandler(this.ctrl);
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
			JOptionPane.showMessageDialog(this, "wait");
			WithinSwing.frameHwnd = JWebTopNative.getWindowHWND(this);
			System.out.println("Java窗口handler = " + WithinSwing.frameHwnd + " hex=0x" + Long.toHexString(WithinSwing.frameHwnd));
			String path = "JWebTop.exe";
			System.out.println("\tpath = " + path);
			String appFile = ctrl.getListAppFile();
			System.out.println("\tappFile = " + appFile);
			JWebTopNative.createJWebTop(path, appFile);
			RootBrowserHwnd = listWebtopView.createInernalBrowser(appFile, null, "列表页", null);
			System.out.println("浏览器窗口handler = " + RootBrowserHwnd + " hex=0x" + Long.toHexString(RootBrowserHwnd));
			ctrl.setListHandler(RootBrowserHwnd);
		} catch (Throwable e) {
			e.printStackTrace();
		}
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
				JWebTopNative.exit();
				System.exit(0);//
			} else {
				return;
			}
		}
		super.processWindowEvent(e);
	}

	public static void main(String[] args) {
		try {
			initDll(args);
			new WithinSwing();
		} catch (Throwable e) {
			e.printStackTrace();
		}
	}
}