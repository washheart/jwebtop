using System;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Newtonsoft.Json.Linq;
using System.Diagnostics;
 
namespace JWebTop {
    /**
     * 操作Windows窗口的JNI接口java端 <br>
     * 源码：https://github.com/washheart/jwebtop<br>
     * 说明：https://github.com/washheart/jwebtop/wiki<br>
     * 
     * @author washheart@163.com
     */
    public static class JWebTopNative {
        #region 其他windows dll方法和常量

        public const int SWP_NOMOVE = 0x2;
        public const int SWP_NOZORDER = 0x04;
        public const int HWND_TOPMOST = -1;

        [DllImport("User32.dll")]
        private static extern int SetWindowPos(IntPtr hwnd, int hWndInsertAfter, int x, int y, int cx, int cy, int wFlags);
        #endregion

        // FastIPC.createSubProcess
        //// 创建一个新进程，返回的数据为进程中主线程的id
        //public static long createSubProcess(String subProcess, String szCmdLine) {
        //    //return nCreateSubProcess(subProcess, szCmdLine);
        //    ProcessStartInfo process = new ProcessStartInfo();
        //    process.FileName = subProcess;
        //    process.Arguments = " " + szCmdLine;
        //    process.UseShellExecute = false;
        //    process.CreateNoWindow = true;
        //    process.RedirectStandardOutput = true;
        //    Process p = Process.Start(process);
        //    return p.Id;
        //}

        public static long getProcessID() {
            return Process.GetCurrentProcess().Id;
        }

        public static void setSize(long browserHwnd, int w, int h) {
            if (browserHwnd != 0) {
                //nSetSize(browserHwnd, w, h);
                System.IntPtr hWnd = new IntPtr(browserHwnd);
                SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER);
            }
        }
    }// End JWebTopNative class
}// End JWebTop namespace