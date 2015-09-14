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
            //int i1 = 10; int i2 = JWebTopNative.Calc(i1);// 测试通过
            //a = a + i2
            //StringBuilder sb = new StringBuilder(a);
            //string b = JWebTopNative.CovWString(sb);
            //StringBuilder b = JWebTopNative.CovWString(sb);
            //string b = JWebTopNative.CovWString(a);
            //int b = JWebTopNative.CovString(sb);// 测试通过
            //StringBuilder b = JWebTopNative.CovString2(sb);// 可以了（但是用的是ascii[gbk]参数，都是string类型），但是结果不对，有乱码
            //int b = JWebTopNative.CovWString2(sb);// 测试通过
            // b = JWebTopNative.CovWString3(a);// 测试通过
            //string b = JWebTopNative.CovWString4(a);// 有乱码
            //string tmp_unicode_error = JWebTopNative.CovWString4(a);// 这里是乱码
            //byte[] buffer = Encoding.Unicode.GetBytes(tmp_unicode_error);
            //string t = Encoding.UTF8.GetString(buffer);// 这里已经转换对了
            //int c = JWebTopNative.CovWString3("中文", "xyz");
            //a = a + t;
            //a = a + b + c;
            //textBox1.Text = a;
            JWebTopNative.createJWebTop("JWebTop.exe", "index.app");
        }
    }
}
