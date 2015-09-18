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
    public partial class MainForm : Form {
        private DemoBrowserCtrl ctrl = null;
        private JWebTopBrowser listBrowser = null;
        private JWebTopBrowser detailBrowser = null;
        public MainForm() {
            InitializeComponent();
        }

        private void btnNewNote_Click(object sender, EventArgs e) {
            //JWebTopConfigs configs = new JWebTopConfigs();
            //configs.appDefFile = "d:/index.app";
            //JObject jo = JObject.FromObject(configs);
            //string jsonstring = jo.ToString();
            //textBox1.Text = jsonstring;
            //string v1 = (string)jo["appDefFile"];
            //textBox1.Text = textBox1.Text + "\r\n v1=" + v1;
            //JWebTopConfigs.removeDefaults(jo);
            //String newstr = jo.ToString();
            //textBox1.Text = textBox1.Text + "\r\n newstr==" + newstr;
        }
        private void MainForm_Load(object sender, EventArgs e) {
            this.ctrl = new DemoBrowserCtrl();
            this.listBrowser = new JWebTopBrowser();
            this.detailBrowser = new JWebTopBrowser();
            JWebTopNative.setJsonHandler(this.ctrl);
            listBrowser.Dock = System.Windows.Forms.DockStyle.Fill;
            detailBrowser.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitMain.Panel1.Controls.Add(listBrowser);
            this.splitMain.Panel2.Controls.Add(detailBrowser);
            // 注意：一定要在新线程中进行创建操作，否则会和C#的死锁（难道Form.Load事件是从WndProc中发出的，然后和JWebTopDLL中sendMessage冲突了？？？）
            Thread th = new Thread(new ThreadStart(createBrowser)); //也可简写为new Thread(ThreadMethod);                
            th.Start(); //启动线程  
        }

        private void createBrowser() {
            string appfile = "res/list/index.app";
            JWebTopNative.createJWebTop("JWebTop.exe", appfile);
            listBrowser.createInernalBrowser(appfile, null, null, null);
            appfile = "res/detail/index.app";
            detailBrowser.createInernalBrowser(appfile, null, null, null);
        }
    }
}
