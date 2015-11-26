package org.jwebtop;

import java.awt.Container;
import java.awt.Dimension;
import java.awt.Point;
import java.awt.Window;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;

import javax.swing.JComponent;

import org.jwebtop.listener.JWebTopBrowserCreated;

/**
 * 与实际浏览器控件对应的Java控件<br>
 * 通过监听Java控件的移动、最小化等相关事件，然后将这些事件分发给实际的浏览器控件<br>
 * 源码：https://github.com/washheart/jwebtop<br>
 * 说明：https://github.com/washheart/jwebtop/wiki<br>
 * 
 * @author washheart@163.com
 */
public class JWebTopBrowser extends JComponent {

	public void setBrowserVisible(boolean aFlag) {
		if (!aFlag) JWebTopNative.setSize(getBrowserHwnd(), 0, 0);
	}

	protected Window topWindow;
	protected long hWnd;

	private ComponentListener swtPanelComponentListener = new ComponentAdapter() {
		@Override
		public void componentMoved(ComponentEvent e) {
			JWebTopBrowser.this.moveWebTopBrowser();
		}

		@Override
		public void componentResized(ComponentEvent e) {
			JWebTopBrowser.this.moveWebTopBrowser();
		}

		@Override
		public void componentHidden(ComponentEvent e) {}

		@Override
		public void componentShown(ComponentEvent e) {
			moveWebTopBrowser();
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

	public void moveWebTopBrowser() {
		if (!this.isShowing()) return;
		long browserHwnd = getBrowserHwnd();
		if (browserHwnd == 0) return;
		// JWebTopNative.setLocation(browserHwnd, x, y);// 使用WM_CHIILD方式构建的窗口不需要设置Location
		Point p = this.calcBrowserLocation();
		JWebTopNative.setBound(browserHwnd, p.x, p.y, JWebTopBrowser.this.getWidth(), JWebTopBrowser.this.getHeight());
		// JWebTopNative.setSize(browserHwnd, JWebTopBrowser.this.getWidth(), JWebTopBrowser.this.getHeight());
	}

	public Point calcBrowserLocation() {
		if (!isShowing()) return new Point(0, 0);
		Point locOnDesktop = this.getLocationOnScreen();
		if (this.topWindow != null) {
			int[] rc = JWebTopNative.getWindowClient(JWebTopNative.getWindowHWND(this.topWindow));
			locOnDesktop.x -= rc[0];
			locOnDesktop.y -= rc[1];
		}
		return locOnDesktop;
	}

	public void createInernalBrowser(JWebTopContext ctx, String appFile, String url, String title, String icon, JWebTopBrowserCreated listener) {
		JWebTopConfigs config = new JWebTopConfigs();
		config.setAppDefFile(appFile);
		config.setUrl(url);
		config.setName(title);
		config.setIcon(icon);
		createInernalBrowser(ctx, config, listener);
	}

	public void createInernalBrowser(JWebTopContext ctx, JWebTopConfigs config, final JWebTopBrowserCreated listener) {
		long parentHWnd = JWebTopNative.getWindowHWND(this.topWindow);
		if (parentHWnd == 0) throw new JWebTopException("使用JWebTopBrowser时，其所在父窗口必须已显示");
		config.setParentWin(parentHWnd);
		config.setDwStyle(WindowStyle.WS_CHILD | WindowStyle.WS_VISIBLE);
		if (isShowing()) {
			Point p = this.calcBrowserLocation();
			config.setX(p.x);
			config.setY(p.y);//
			Dimension size = this.getSize();
			config.setW(size.width);
			config.setH(size.height);
		} else {
			config.setX(0);
			config.setY(0);//
			config.setW(0);
			config.setH(0);
		}
		config.setMax(0);
		ctx.createBrowser(config, new JWebTopBrowserCreated() {
			@Override
			public void onJWebTopBrowserCreated(long browserHWnd) {
				hWnd = browserHWnd;
				listener.onJWebTopBrowserCreated(browserHWnd);
			}
		});
	}

	private ComponentListener topWindowComponentListener = null;

	public void setTopWindow(Window w) {
		if (w == null) throw new JWebTopException("必须设置容器所在窗口，否则不能创建组件！");
		if (topWindowComponentListener == null) {
			topWindowComponentListener = new ComponentAdapter() {
				@Override
				public void componentMoved(ComponentEvent e) {
					JWebTopBrowser.this.moveWebTopBrowser();
				}
			};
		}
		if (this.topWindow != null) {
			this.topWindow.removeComponentListener(topWindowComponentListener);
		}
		this.topWindow = w;
		this.topWindow.addComponentListener(topWindowComponentListener);
		this.addComponentListener(swtPanelComponentListener);
		if (this.isShowing()) moveWebTopBrowser();
	}

	@Override
	public void setVisible(boolean aFlag) {
		super.setVisible(aFlag);
		this.setBrowserVisible(aFlag);
		if (aFlag) {
			moveWebTopBrowser();
		}
	}

	public void dispose(JWebTopContext ctx) {
		JWebTopNative.setWindowStyle(hWnd, WindowStyle.WS_POPUP);
		ctx.closeBrowser(hWnd);
		hWnd = 0;
	}

	private long getBrowserHwnd() {
		return this.hWnd;
	}

	public void setBrowserHwnd(long hWnd) {
		this.hWnd = hWnd;
	}
}