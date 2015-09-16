using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;

namespace JWebTop_CSharp_Demo {
    class DemoBrowserCtrl:JWebTop.JWebtopJSONDispater {
        private List<String> names = null;
        public string dispatcher(long browserHWnd, string json) {
            Debug.WriteLine("分发浏览器JS，浏览器句柄=" + browserHWnd + "，" + json);
            JObject jo=JObject.Parse(json);
            string method = (string)jo["method"];
            if ("initList".Equals(method)) {
                if (names != null) return "{}";
                names = new List<String>();
                names.Add("a"); names.Add("b");
                //BufferedReader br = null;
                //try {
                //    File f = getNotesFile();
                //    Reader fr = new FileReader(f);
                //    br = new BufferedReader(fr);
                //    String line = null;
                //    while ((line = br.readLine()) != null) {
                //        names.add(line);
                //    }
                //    br.close();
                //    fr.close();
                //} catch (IOException e) {
                //    e.printStackTrace();
                //} finally {
                //    if (br != null) try {
                //            br.close();
                //        } catch (IOException e) { }
                //}
                JObject rtn = new JObject();
                names.Remove(null);
                rtn["method"]= method;
                rtn["value"] = new JArray(names);                
                // JWebTopNative.executeJs(listHandler, rtn.toJSONString());
                return rtn.ToString();
            //} else if ("getDetailAppFile".equals(method)) {
            //    //return getDetailAppFile();
            //} else if ("setDetailHandler".equals(method)) {
            //    this.detailHandler = jo.getLong("value");
            //    showDetail(null);
            //} else if ("showDetail".equals(method)) {
            //    String name = jo.getString("value");
            //    this.currentNote = name;
            //    showDetail(name);
            //} else if ("saveNote".equals(method)) {
            //    saveNote(jo.getString("note"), jo.getString("value"));
            //} else if ("javaWindowHwnd".equals(method)) {
            //    return Long.toString(WithinSwing.frameHwnd);
            //} else if ("getDetailRect".equals(method)) {
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

        public void resetThreadClassLoader() {
            //
        }
    }
}
