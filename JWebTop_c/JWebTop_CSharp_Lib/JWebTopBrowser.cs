﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Drawing;
using System.Text;

namespace JWebTop {
    public class JWebTopBrowser : System.Windows.Forms.Panel {
        public JWebTopBrowser() {
            this.Resize += new System.EventHandler(this.sizeChanged);
        }
        private int hWnd = 0;
        private int getBorwserHWnd() {
            return this.hWnd;
        }
        private void sizeChanged(object sender, EventArgs e) {
            Size size = this.Size;
            JWebTopNative.setSize(getBorwserHWnd(), size.Width, size.Height);
        }
        public int createInernalBrowser(String appFile, String url, String title, String icon) {
            JWebTopConfigs config = new JWebTopConfigs();
            config.appDefFile = appFile;
            config.url = url;
            config.name = title;
            config.icon = icon;
            return createInernalBrowser(config);
        }

        public int createInernalBrowser(JWebTopConfigs config) {
            Size size = this.Size;
            config.parentWin = this.Handle.ToInt32();
            config.x = 0;
            config.y = 0;//
            config.w = size.Width;
            config.h = size.Height;
            this.hWnd = JWebTopNative.createBrowser(config);
            return this.hWnd;
        }
    }
}