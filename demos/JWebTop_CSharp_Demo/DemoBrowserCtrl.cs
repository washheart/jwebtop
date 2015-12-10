using JWebTop;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;

namespace JWebTop_CSharp_Demo {

    interface WithinSwingCtrlHelper {
        int getMainHWnd();
        JWebTopContext getCtx();
        int[] getDetailRect();
        void browserClosed(int browserHWnd);
        void detailCreated(int browserHWnd);
    }
    class DemoBrowserCtrl : JWebTop.JWebtopJSONDispater {
        private static readonly Encoding encoding = Encoding.UTF8;
        private List<string> names = null;
        private WithinSwingCtrlHelper helper;
        private string currentNote;
        private int listHandler, detailHandler;

        object lockThis = new object(); 
        public void resetThreadClassLoader() { }
        
        private void initNames() {
            lock (lockThis) {
                if (names != null) return;
                names = new List<string>();
                string fn = getNotesFile();
                IEnumerable<string> nameLines = File.ReadLines(fn, encoding);
                foreach (string line in nameLines) {
                   names.Add(line);
                }
            }
        }
        public string dispatcher(int browserHWnd, string json) {
            Debug.WriteLine("分发浏览器JS，浏览器句柄=" + browserHWnd + "，" + json);
            if (json.Trim().Length == 0) return "";
            JObject jo = JObject.Parse(json);
            string method = (string)jo["method"];
            if ("initList".Equals(method)) {
                initNames();
                JObject rtn = new JObject();
                Debug.WriteLine("JObject");
                names.Remove(null);
                Debug.WriteLine("Remove(null)");
                rtn["method"] = method;
                rtn["value"] = new JArray(names);
                // JWebTopNative.executeJs(listHandler, rtn.toJSONString());
                return rtn.ToString();
            } else if ("getDetailAppFile".Equals(method)) {
                JObject rtn = new JObject();
                rtn["value"] = getDetailAppFile();
                return rtn.ToString();
            } else if ("setDetailHandler".Equals(method)) {
                this.detailHandler = (int)jo["value"];
                this.helper.detailCreated(this.detailHandler);
                showDetail(names.Count == 0 ? null : names[0]);
            } else if ("showDetail".Equals(method)) {
                string name = (string)jo["value"];
                this.currentNote = name;
                showDetail(name);
            } else if ("saveNote".Equals(method)) {
                saveNote((string)jo["note"], (string)jo["value"]);
            } else if ("showdetail".Equals(method)) {
                String name = (String)jo["value"];
                this.currentNote = name;
                showDetail(name);
            } else if ("saveNote".Equals(method)) {
                saveNote((String)jo["note"], (String)jo["value"]);
            } else if ("javaWindowHwnd".Equals(method)) {
                return "{\"value\":" + helper.getMainHWnd() + "}";
            } else if ("getDetailRect".Equals(method)) {
                int[] rect = helper.getDetailRect();
                StringBuilder sb = new StringBuilder();
                sb.Append("{x:").Append(rect[0]);
                sb.Append(",y:").Append(rect[1]);
                sb.Append(",w:").Append(rect[2]);
                sb.Append(",h:").Append(rect[3]);
                sb.Append("}");
                return sb.ToString();
            } else if ("browserClosed".Equals(method)) {
                helper.browserClosed(browserHWnd);
                return "";
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
            bool hasData = true;
            if (note == null || note.Trim().Length == 0) {
                if (names == null) initNames();
                if (names == null || names.Count == 0) {
                    hasData = false;
                    detail = "请使用“添加日记”按钮新建日记";
                } else {
                    note = names[0];
                }
            }
            if(hasData) {
                detail = "在这里输入【" + note + "】日记的内容";
                string fn = getNoteFile(note);
                if (File.Exists(fn)) {
                    detail = File.ReadAllText(fn, encoding);
                }
            }
            rtn["value"] = detail;
            rtn["note"] = note;

            helper.getCtx().executeJSON_NoWait(detailHandler, rtn.ToString());
        }
        public string getJWebTopExe() {
            string fn = ("JWebTop.exe");
            if (File.Exists(fn)) return Path.GetFullPath(fn);
            // 开发环境下		
            return Path.GetFullPath("../../" + fn);
        }
        public string getListAppFile() {
            string fn = ("res/list/index.app");
            if (File.Exists(fn)) return Path.GetFullPath(fn);
            // 开发环境下		
            return Path.GetFullPath("../../../" + fn);
        }
        public string getDetailAppFile() {
            string fn = ("res/detail/index.app"); if (File.Exists(fn)) return Path.GetFullPath(fn);
            // 开发环境下		
            return Path.GetFullPath("../../../" + fn);
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
            helper.getCtx().executeJSON_NoWait(listHandler, jo.ToString());
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
            helper.getCtx().executeJSON_NoWait(listHandler, jo.ToString());
        }

        public void saveNote(string note, string content) {
            if (note != null) {
                string fn = getNoteFile(note);
                File.WriteAllText(fn, content, encoding);
            }
        }
        public int getDetailHandler() {
            return detailHandler;
        }

        public void setListHandler(int listHandler) {
            this.listHandler = listHandler;
        }

    }
}