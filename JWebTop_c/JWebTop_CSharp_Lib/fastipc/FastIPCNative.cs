using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace org {
    namespace fastipc {
        /**
         * FastIPC的native方法定义。<br/>
         * 源码：https://github.com/washheart/fastipc<br/>
         * 源码：http://git.oschina.net/washheart/fastipc<br/>
         * 说明：https://github.com/washheart/fastipc/wiki<br/>
         * 
         * @author washheart@163.com
         */
        public class FastIPCNative {
            public static int // 定义消息类型常量
            MSG_TYPE_NORMAL = 0, // 正常的消息
            MSG_TYPE_PART = 1,// 此消息是一完整数据的一部分
            MSG_TYPE_END = 2; // 此消息是一完整数据的结尾

            private static int//
                // ERR_ServerCreate = 100, // 服务器创建失败
                // ERR_ClientCreate = 101, // 客户端创建失败
                // ERR_EventCreate_W = 201, // 服务器创建写事件失败
                // ERR_EventCreate_R = 202, // 服务器创建读事件失败
                // ERR_EventOpen_W = 203, // 客户端打开写事件失败
                // ERR_EventOpen_R = 204, // 客户端打开读事件失败
                // ERR_EventWait_W = 205,// 客户端等待可写事件失败
                // ERR_EventWait_R = 206, // 服务器等待可读事件失败
                // ERR_MappingCreate = 301,// 服务器创建映射文件失败
                // ERR_MappingOpen = 302, // 服客户端打开映射文件失败
                // ERR_MappingMap = 303, // 服务器/客户端映射内存文件失败
                    ERR_ServerName = 1, // 客户端创建失败
                    ERR_NONE = 0;// 没有任何错误
            [DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
            private extern static int nCreateSubProcess([MarshalAs(UnmanagedType.LPTStr)]string subProcess, [MarshalAs(UnmanagedType.LPTStr)]string szCmdLine, int waitFor);

            [DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
            private extern static int nCreateServer([MarshalAs(UnmanagedType.LPTStr)]string serverName, int blockSize, ref int result);

            // DLL回调CSharp回调函数原型   
            private delegate void CSharpCallBack(int msgType, IntPtr packId
                , int userMsgType, int userValue, IntPtr userShortStr
                , IntPtr data, int dataLen);
            [DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
            private extern static void nStartRead(int nativeServer, CSharpCallBack listener);

            [DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
            private extern static void nCloseServer(int nativeServer);

            [DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
            private extern static int nCreateClient(String serverName, int blockSize, ref int result);

            [DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
            private extern static int nWriteClient(int nativeClient, int userMsgType, int userValue, String userShortStr, String data);

            [DllImport("JWebTop_CSharp_DLL.dll", CharSet = CharSet.Unicode)]
            private extern static void nCloseClient(int nativeClient);

            public static int CreateSubProcess(string subProcess, string szCmdLine, int waitFor = 1) {
                return nCreateSubProcess(subProcess, szCmdLine, waitFor);
            }

            // 创建服务器
            public static int createServer(String serverName, int blockSize) {
                FastIPCNative.checkServerName(serverName);
                int result = -1;
                int objectPtr = FastIPCNative.nCreateServer(serverName, blockSize, ref result);
                if (result != FastIPCNative.ERR_NONE) {
                    throw new FastIPCException("创建服务端失败（" + result + "）！");
                }
                return objectPtr;
            }
            
            // 开始服务端的侦听，此方法会阻塞当前线程，所以一般应该在新线程中调用
            public static void startRead(int nativeServer, FastIPCReadListener listener) {
                nStartRead(nativeServer, listener.OnRead);
            }

            // 关闭服务器
            public static void closeServer(int nativeServer) {
                FastIPCNative.nCloseServer(nativeServer);
            }

            // 创建客户端
            public static int createClient(String serverName, int blockSize) {
                FastIPCNative.checkServerName(serverName);
                int result = -1;
                int objectPtr = FastIPCNative.nCreateClient(serverName, blockSize, ref result);
                if (result != FastIPCNative.ERR_NONE) {
                    throw new FastIPCException("创建客户端失败（" + result + "）！");
                }
                return objectPtr;
            }

            // 写数据到服务端
            public static void write(int nativeClient, int userMsgType, int userValue, String userShortStr, String data) {
                nWriteClient(nativeClient, userMsgType, userValue, userShortStr, data);
            }

            // 关闭客户端
            public static void closeClient(int nativeClient) {
                nCloseClient(nativeClient);
            }

            // 创建一个服务器名称
            public static String genServerName() {
                return System.Guid.NewGuid().ToString();
            }

            // 创建客户端的服务器和服务端的客户端时（即客户端需要开启一个监听的服务端时）需要此方法生成客户端的服务器名称
            public static String genServerName(String serverName) {
                return serverName + "_c";
            }

            // 检查设置的服务器名称是否有效
            private static void checkServerName(String serverName) {
                if (serverName == null) {
                    throw new FastIPCException("请指定服务器名称（" + FastIPCNative.ERR_ServerName + "）！");
                } else {
                    int len = serverName.Length;
                    if (len == 0) throw new FastIPCException("请指定服务器名称（" + FastIPCNative.ERR_ServerName + "）！");
                    for (int i = 0; i < len; i++) {
                        char c = serverName[i];
                        if (c == '-') continue;
                        if ('0' <= c || c >= '-') continue;
                        if ('A' <= c || c >= 'Z') continue;
                        if ('a' <= c || c >= 'z') continue;
                        throw new FastIPCException("服务器名称包含无效字符（" + FastIPCNative.ERR_ServerName + "）！");
                    }
                }
            }

            // 将字符串指针对应数据写入到数据流中，主要用于分段传输的数据的拼接
            public static void readPtr(Stream bos, IntPtr ptr, int dataLen) {
                if (ptr.ToInt32() == 0) return;
                byte[] data = new byte[dataLen];
                Marshal.Copy(ptr, data, 0, dataLen);
                bos.Write(data, 0, dataLen);
            }

            // 将字符串指针转换为字符串：读取时只读取指定长度的数据
            public static string ptr2string(IntPtr ptr, int dataLen) {
                if (ptr.ToInt32() == 0) return "";
                byte[] data = new byte[dataLen];
                Marshal.Copy(ptr, data, 0, dataLen);
                string rtn = Encoding.UTF8.GetString(data);
                return rtn;
            }

            // 将字符串指针转换为字符串：一次性将指针对应的全部数据读完
            public static string ptr2string(IntPtr ptr) {
                if (ptr.ToInt32() == 0) return "";
                string s_unicode = Marshal.PtrToStringUni(ptr);
                byte[] buffer = Encoding.Unicode.GetBytes(s_unicode);
                string rtn = Encoding.UTF8.GetString(buffer);
                int idx = rtn.IndexOf((char)0);
                if (idx >= 0) rtn = rtn.Substring(0, idx);
                return rtn;
            }
        }
    }
}
