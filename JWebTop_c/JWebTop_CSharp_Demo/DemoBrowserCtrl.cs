using JWebTop;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;

namespace JWebTop_CSharp_Demo {

    interface WithinSwingCtrlHelper {
        int getListHandler();
        int getDetailHWnd();
    }
    class DemoBrowserCtrl : JWebTop.JWebtopJSONDispater {
        private static readonly Encoding encoding = Encoding.UTF8;
        private List<string> names = null;
        private WithinSwingCtrlHelper helper;
        private string currentNote;
        public void resetThreadClassLoader() { }
   
        public string dispatcher(long browserHWnd, string json) {
            Debug.WriteLine("分发浏览器JS，浏览器句柄=" + browserHWnd + "，" + json);
            JObject jo = JObject.Parse(json);
            string method = (string)jo["method"];
            if ("initList".Equals(method)) {
                if (names != null) return "{}";
                names = new List<string>();
                string fn = getNotesFile();
                IEnumerable<string> nameLines = File.ReadLines(fn, encoding);
                foreach (string line in nameLines) {
                    names.Add(line);
                }
                //names.Add("a"); names.Add("b");
                JObject rtn = new JObject();
                Debug.WriteLine("JObject");
                names.Remove(null);
                Debug.WriteLine("Remove(null)");
                rtn["method"] = method;
                rtn["value"] = new JArray(names);
                // JWebTopNative.executeJs(listHandler, rtn.toJSONString());
                return rtn.ToString();
            } else if ("showDetail".Equals(method)) {
                string name = (string)jo["value"];
                this.currentNote = name;
                showDetail(name);
            } else if ("saveNote".Equals(method)) {
                saveNote((string)jo["note"], (string)jo["value"]);
            //} else if ("getDetailAppFile".Equals(method)) {
            //    //return getDetailAppFile();
            //} else if ("setDetailHandler".Equals(method)) {
            //    this.detailHandler = jo.getLong("value");
            //    showDetail(null);
            //} else if ("javaWindowHwnd".Equals(method)) {
            //    return Long.toString(WithinSwing.frameHwnd);
            //} else if ("getDetailRect".Equals(method)) {
            //    int[] rect = withinSwingCtrlHelper.getDetailRect();
            //    StringBuilder sb = new StringBuilder();
            //    sb.append("{x:").append(rect[0]);
            //    sb.append(",y:").append(rect[1]);
            //    sb.append(",w:").append(rect[2]);
            //    sb.append(",h:").append(rect[3]);
            //    sb.append("}");
            //    return sb.toString();
            }
            return "";
        }
        private string getNotesFile() {
            string fn = "data/note/list.txt";
            if (!File.Exists(fn)) {
                Directory.CreateDirectory("data/note");
                File.Create(fn).Close();
            }
            return fn;
        }
        private string getNoteFile(string name) {
            string fn = ("data/note/" + name + ".txt");
            if (!File.Exists(fn)) {
                File.Create(fn).Close();
            }
            return fn;
        }

        private void showDetail(string note) {
            JObject rtn = new JObject();
            rtn["method"] = "showDetail";
            string detail = null;
            if (note == null || note.Trim().Length == 0) {
                detail = "请使用“添加日记”按钮新建日记";
            } else {
                detail = "在这里输入【" + note + "】日记的内容";
                string fn = getNoteFile(note);
                if (File.Exists(fn)) {
                    detail = File.ReadAllText(fn, encoding);
                }
            }
            rtn["value"] = detail;
            rtn["note"] = note;
            JWebTopNative.executeJSON_NoWait(helper.getDetailHWnd(), rtn.ToString());
        }
        public string getListAppFile() {
            string fn = ("res/list/index.app");
            if (File.Exists(fn)) return Path.GetFullPath(fn);
            // 开发环境下		
            return Path.GetFullPath("../" + fn);
        }
        public string getDetailAppFile() {
            string fn = ("res/detail/index.app"); if (File.Exists(fn)) return Path.GetFullPath(fn);
            // 开发环境下		
            return Path.GetFullPath("../" + fn);
        }

        public string getCurrentNote() {
            return currentNote;
        }
        public WithinSwingCtrlHelper getWithinSwingCtrlHelper() {
            return helper;
        }
        public void setWithinSwingCtrlHelper(WithinSwingCtrlHelper withinSwingCtrlHelper) {
            this.helper = withinSwingCtrlHelper;
        }

        private void saveNotes() {
            StringBuilder sb = new StringBuilder();
            string fn = getNotesFile();
            foreach (string n in names) {
                sb.Append(n);
                sb.Append("\r\n");
            }
            File.WriteAllText(fn, sb.ToString(), encoding);
        }

        public void addNote(string name) {
            if (this.names.Contains(name)) return;
            this.names.Add(name);
            saveNotes();
            JObject jo = new JObject();
            jo["method"] = "noteAdded";
            jo["value"] = name;
            JWebTopNative.executeJSON_NoWait(helper.getListHandler(), jo.ToString());
        }

        public void delNote() {
            int idx = this.names.IndexOf(currentNote);
            if (idx == -1) return;
            this.names.Remove(currentNote);
            saveNotes();
            JObject jo = new JObject();
            jo["method"] = "noteRemoved";
            jo["value"] = this.currentNote;
            this.currentNote = null;
            if (idx > 0) idx = idx - 1;
            if (idx < this.names.Count) {
                jo["newSel"] = this.names[idx];
            } else {
                showDetail(null);
            }
            JWebTopNative.executeJSON_NoWait(helper.getListHandler(), jo.ToString());
        }

        public void saveNote(string note, string content) {
            if (note != null) {
                string fn = getNoteFile(note);
                File.WriteAllText(fn, content, encoding);
            }
        }
    }
}