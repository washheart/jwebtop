using System;
using System.Collections.Generic;
using System.Linq;
using System.Drawing;
using System.Text;

namespace JWebTop {
    public class JWebTopBrowser : System.Windows.Forms.Panel,JWebTopBrowserCreated {
        public JWebTopBrowser() {
            this.Resize += new System.EventHandler(this.sizeChanged);
        }
        private int hWnd = 0;
        private JWebTopBrowserCreated jWebTopBrowserCreated;

        private void sizeChanged(object sender, EventArgs e) {
            Size size = this.Size;
            JWebTopNative.setSize(getBorwserHWnd(), size.Width, size.Height);
        }

        public void createInernalBrowser(JWebTopContext ctx, string appFile, string url, string title, string icon, JWebTopBrowserCreated listener) {
            JWebTopConfigs config = new JWebTopConfigs();
            config.appDefFile = appFile;
            config.url = url;
            config.name = title;
            config.icon = icon;
            createInernalBrowser(ctx, config, listener);
        }
        public void createInernalBrowser(JWebTopContext ctx, JWebTopConfigs config,
             JWebTopBrowserCreated listener) {
            Size size = this.Size;
            config.parentWin = this.getControlHandle();
            config.x = 0;
            config.y = 0;//
            config.w = size.Width;
            config.h = size.Height;
            config.max=0;
            this.jWebTopBrowserCreated = listener;
		    ctx.createBrowser(config, this);
        }

        public void onJWebTopBrowserCreated(int browserHWnd) {
            hWnd = browserHWnd;
            jWebTopBrowserCreated.onJWebTopBrowserCreated(browserHWnd);
        }
        private int GetHandle_direct() {
            return this.Handle.ToInt32();
        }
        private delegate int GetHandle_delegate(); //定义委托变量
        private int getControlHandle() {
            if (this.InvokeRequired) {
                GetHandle_delegate d = new GetHandle_delegate(GetHandle_direct);
                return (int)this.Invoke(d);
            } else {
                return GetHandle_direct();
            }
        }

        public long getBrowserHWnd() {
            return this.hWnd;
        }

        public Point calcBrowserLocation() {
            Point locOnDesktop = this.Location;
            //if (this.topWindow != null) {
            //    int[] rc = JWebTopNative.getWindowClient(JWebTopNative.getWindowHWND(this.topWindow));
            //    locOnDesktop.x -= rc[0];
            //    locOnDesktop.y -= rc[1];
            //}
            return locOnDesktop;
        }

        public int getBorwserHWnd() {
            return this.hWnd;
        }
        public void setBrowserHwnd(int browserHWnd) {
            this.hWnd = browserHWnd;
        }
    }
}