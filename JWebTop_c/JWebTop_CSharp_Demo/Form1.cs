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
namespace JWebTop_CSharp_Demo
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void btnNewNote_Click(object sender, EventArgs e)
        {
            JWebTopConfigs configs = new JWebTopConfigs();
            configs.appDefFile = "d:/index.app";
            JObject jo = JObject.FromObject(configs);
            string jsonstring = jo.ToString();
            textBox1.Text = jsonstring;
            string v1 = (string)jo["appDefFile"];
            textBox1.Text = textBox1.Text + "\r\n v1=" + v1;
            JWebTopConfigs.removeDefaults(jo);
            String newstr = jo.ToString();
            textBox1.Text = textBox1.Text + "\r\n newstr==" + newstr;
        }
        DemoBrowserCtrl ctrl;
        private void MainForm_Load(object sender, EventArgs e) {
            this.ctrl = new DemoBrowserCtrl();
            JWebTopNative.setJsonHandler(this.ctrl);
            //JWebTopNative.check();// 测试通过
            //string a = "来自csharp端";
            //int i1 = 10; int i2 = JWebTopNative.Calc(i1);a = a + i2// 测试通过
            //StringBuilder sb = new StringBuilder(a);
            //StringBuilder b = JWebTopNative.CovString2(sb);
            //string tmp_unicode_error = JWebTopNative.CovWString4(a);
            //textBox1.Text += "\r\n CovWString4=" + tmp_unicode_error;
            //sb = new StringBuilder(20);
            //JWebTopNative.CovWString用StringBuilder方式(sb, a);
            //textBox1.Text += "\r\n CovWString用StringBuilder方式=" + sb;
            //sb = new StringBuilder(2);
            //JWebTopNative.CovWString用StringBuilder方式(sb,a);
            //textBox1.Text += "\r\n CovWString用StringBuilder方式=" +sb;

            JWebTopNative.createJWebTop("JWebTop.exe", "index.app");
            this.detailBrowser.createInernalBrowser("index.app",null,null,null );
        }
    }
}
