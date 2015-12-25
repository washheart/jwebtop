using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using JWebTop;
using System.Threading;
namespace JWebTop_CSharp_Demo {
    public partial class MainForm : Form, WithinSwingCtrlHelper, JWebTopAppInited {

        // 一般情况下一个独立的应用中有一个JWebTopContext即可，一个JWebTopContext可以创建多个Browser
        public JWebTopContext ctx = new JWebTopContext();
        private DemoBrowserCtrl ctrl = null;
        private JWebTopBrowser listBrowser = null;
        private JWebTopBrowser detailBrowser = null;
        public MainForm() {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e) {
            this.ctrl = new DemoBrowserCtrl();
            this.ctrl.setWithinSwingCtrlHelper(this);
            this.listBrowser = new JWebTopBrowser();
            this.detailBrowser = new JWebTopBrowser();

            ctx.setJsonHandler(this.ctrl);
            listBrowser.Dock = System.Windows.Forms.DockStyle.Fill;
            detailBrowser.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitMain.Panel1.Controls.Add(listBrowser);
            this.splitMain.Panel2.Controls.Add(detailBrowser);
            // 注意：一定要在新线程中进行创建操作，否则会和C#的死锁（难道Form.Load事件是从WndProc中发出的，然后和JWebTopDLL中sendMessage冲突了？？？）
            Thread th = new Thread(new ThreadStart(createBrowser)); //也可简写为new Thread(ThreadMethod);                
            th.Start(); //启动线程  
        }
        private void createBrowser() {
            string appFile = ctrl.getListAppFile();
            ctx.createJWebTopByCfgFile(ctrl.getJWebTopExe(), appFile, this);
        }
        public void onJWebTopAppInited() {
            jwebtopContextInited();
        }
        private class ListBrowserCreated : JWebTopBrowserCreated {
            private DemoBrowserCtrl ctrl = null;
            public ListBrowserCreated(DemoBrowserCtrl ctrl) { this.ctrl = ctrl; }
            public void onJWebTopBrowserCreated(int browserHWnd) {
                ctrl.setListHandler(browserHWnd);
            }
        }
        public void jwebtopContextInited() {
            string appFile = ctrl.getListAppFile();
            listBrowser.createInernalBrowser(ctx, appFile, null, "列表页", null, new ListBrowserCreated(ctrl));
        }
        private void btnNewNote_Click(object sender, EventArgs e) {
            string name = InputBox.ShowInputBox("输入", "请输入名称：");
            name = name.Trim();
            if (name.Length == 0) return;
            ctrl.addNote(name);
        }
        public JWebTopContext getCtx() {
            return this.ctx;
        }       
        private void btnDelNote_Click(object sender, EventArgs e) {
            string note = ctrl.getCurrentNote();
            if (note == null) return;
            if (MessageBox.Show(this, "是否删除【" + note + "】日记？") != DialogResult.OK) return;
            ctrl.delNote();
        }
        // 这里和java不同，java中只有最顶层窗口才有句柄，C#中每个空间都有句柄，所以返回控件的即可 
        public int getMainHWnd() {
            return detailBrowser.getControlHandle();
        }
        public int[] getDetailRect() {
            Size s = detailBrowser.Size;
            return new int[] { 0, 0, s.Width, s.Height };
        }
        public void detailCreated(int browserHWnd) {
            detailBrowser.setBrowserHwnd(browserHWnd);
        }

        public void browserClosed(int browserHWnd) { }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e) {
          
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e) {
            ctx.closeContext();
        }
    }
}
