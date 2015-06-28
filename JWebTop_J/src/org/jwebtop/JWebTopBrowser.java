package org.jwebtop;

import java.awt.Container;
import java.awt.Point;
import java.awt.Window;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;

import javax.swing.JComponent;

/**
 * 与实际浏览器控件对应的Java控件<br>
 * 通过监听Java控件的移动、最小化等相关事件，然后将这些事件分发给实际的浏览器控件
 * 
 * @author washheart@163.com
 */
public class JWebTopBrowser extends JComponent {
	interface IBrowserHwndFeeder {

		long getBrowserHwnd();

	}

	// class BrowserWindow {
	// 只保留setLocation方法目前看也没有什么问题
	// public void setSize(int w, int h) {
	// // if (browserHwnd == 0) return;
	// // if (w == JWebTopBrowser.this.getWidth() && h == JWebTopBrowser.this.getHeight()) return;
	// // System.out.println("setSize[" + browserHwnd + "]w=" + w + ",=h" + h);
	// // JWebTopNative.setSize(browserHwnd, w, h);
	// }

	public void setBrowserVisible(boolean aFlag) {
		if (!aFlag) JWebTopNative.setSize(getBrowserHwnd(), 0, 0);
	}

	public void setBrowserLocation(Point locationOnScreen) {
		long browserHwnd = getBrowserHwnd();
		if (browserHwnd == 0) return;
		int x = locationOnScreen.x, y = locationOnScreen.y;
		// 是否需要移动和设置大小在dll端去检查，这里不检查了
		JWebTopNative.setLocation(browserHwnd, x, y);
		JWebTopNative.setSize(browserHwnd, JWebTopBrowser.this.getWidth(), JWebTopBrowser.this.getHeight());
	}

	private long getBrowserHwnd() {
		// TODO Auto-generated method stub
		return browserHwndFeeder == null ? 0 : browserHwndFeeder.getBrowserHwnd();
	}

	// }

	public IBrowserHwndFeeder getBrowserHwndFeeder() {
		return browserHwndFeeder;
	}

	public void setBrowserHwndFeeder(IBrowserHwndFeeder browserHwndFeeder) {
		this.browserHwndFeeder = browserHwndFeeder;
	}

	// protected BrowserWindow browserWindow = new BrowserWindow();
	protected Window topWindow;
	protected IBrowserHwndFeeder browserHwndFeeder = null;
	private ComponentListener swtPanelComponentListener = new ComponentAdapter() {
		@Override
		public void componentMoved(ComponentEvent e) {
			JWebTopBrowser.this.moveSwtWindow();
		}

		@Override
		public void componentResized(ComponentEvent e) {
			JWebTopBrowser.this.moveSwtWindow();
		}

		@Override
		public void componentHidden(ComponentEvent e) {
			// browserWindow.setSize(0, 0);
		}

		@Override
		public void componentShown(ComponentEvent e) {
			moveSwtWindow();
			Container parent = JWebTopBrowser.this.getParent();
			while (parent != null) {
				ComponentListener[] componentListeners = parent.getComponentListeners();
				boolean exist = false;
				for (ComponentListener componentListener : componentListeners) {
					if (componentListener == this) {
						exist = true;
					}
				}
				if (!exist) {
					parent.addComponentListener(this);
				}
				parent = parent.getParent();
			}
		}
	};

	public JWebTopBrowser() {
		this.setOpaque(false);
	}

	protected void moveSwtWindow() {
		if (!this.isShowing()) return;
		this.setBrowserLocation(getLocationOnScreen());
		// this.browserWindow.setSize(this.getWidth(), this.getHeight());
	}

	private ComponentListener topWindowComponentListener = null;

	// private WindowStateListener topWindowStateListener = null;
	//
	// public void setBrowserHwnd(long browserHwnd) {
	// this.browserHwnd = browserHwnd;
	// }

	public void setTopWindow(Window w) {
		if (w == null) throw new RuntimeException("必须设置容器所在窗口，否则不能创建SWT组件！");
		if (topWindowComponentListener == null) {
			topWindowComponentListener = new ComponentAdapter() {
				@Override
				public void componentMoved(ComponentEvent e) {
					JWebTopBrowser.this.moveSwtWindow();
				}
			};
			// topWindowStateListener = new WindowStateListener() {
			// private int w, h;
			//
			// @Override
			// public void windowStateChanged(WindowEvent e) {
			// int state = e.getNewState();
			// if (state == WindowEvent.WINDOW_ICONIFIED) {
			// this.w = JWebTopBrowser.this.getWidth();
			// this.h = JWebTopBrowser.this.getHeight();
			// browserWindow.setSize(0, 0);
			// } else {
			// browserWindow.setSize(w, h);
			// }
			// // TODO Auto-generated method stub
			// }
			// };
		}
		if (this.topWindow != null) {
			this.topWindow.removeComponentListener(topWindowComponentListener);
			// this.topWindow.removeWindowStateListener(topWindowStateListener);
		}
		this.topWindow = w;
		this.topWindow.addComponentListener(topWindowComponentListener);
		// this.topWindow.addWindowStateListener(topWindowStateListener);
		this.addComponentListener(swtPanelComponentListener);
		if (this.isShowing()) moveSwtWindow();
	}

	public void createTopWindowComponentListener(final Window topWindow) {}

	@Override
	public void setVisible(boolean aFlag) {
		super.setVisible(aFlag);
		this.setBrowserVisible(aFlag);
		if (aFlag) {
			moveSwtWindow();
		}
	}

	protected void innerSwtHasGainedFocus() {}
}