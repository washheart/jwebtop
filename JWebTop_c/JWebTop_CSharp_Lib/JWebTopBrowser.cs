using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace JWebTop {
    public partial class JWebTopBrowser : UserControl {
        private long hWnd = 0;
        public JWebTopBrowser() {
            InitializeComponent();
        }

        private void sizeChanged(object sender, EventArgs e) {
            Size size = this.Size;

            JWebTopNative.setSize(this.hWnd, size.Width, size.Height);
        }

        public long createInernalBrowser(String appFile, String url, String title, String icon) {
            JWebTopConfigs config = new JWebTopConfigs();
            config.appDefFile = appFile;
            config.url = url;
            config.name = title;
            config.icon = icon;
            return createInernalBrowser(config);
        }

        public long createInernalBrowser(JWebTopConfigs config) {
            Size size = this.Size;
            long parentHWnd = this.Handle.ToInt32();
            config.parentWin = parentHWnd;
            config.x = 0;
            config.y = 0;//
            config.w = size.Width;
            config.h = size.Height;
            this.hWnd = JWebTopNative.createBrowser(config);
            return this.hWnd;
        }
    }
}
