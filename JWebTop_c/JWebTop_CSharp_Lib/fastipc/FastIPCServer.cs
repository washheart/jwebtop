﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace org {
    namespace fastipc {
        /**
         * FastIPC的服务端。<br/>
         * 源码：https://github.com/washheart/fastipc<br/>
         * 源码：http://git.oschina.net/washheart/fastipc<br/>
         * 说明：https://github.com/washheart/fastipc/wiki<br/>
         * 
         * @author washheart@163.com
         */
        class FastIPCServer {
            private int nativeServer = 0;// 指向fastipc::Server实例的指针

            private FastIPCReadListener listener;

            public void create(String serverName, int blockSize) {
                nativeServer = FastIPCNative.createServer(serverName, blockSize);
            }

            public void startRead() {
                if (nativeServer == 0) throw new FastIPCException("服务器尚未创建！");
                if (listener == null) throw new FastIPCException("必须指定listener才能侦听！");
                FastIPCNative.startRead(nativeServer, listener);
            }

            public FastIPCReadListener getListener() {
                return listener;
            }

            public void setListener(FastIPCReadListener listener) {
                this.listener = listener;
            }

            public void close() {
                if (nativeServer != 0) {
                    FastIPCNative.closeServer(nativeServer);
                    nativeServer = 0;
                }
            }

            public bool isStable() {
                return nativeServer != 0;
            }

            ~FastIPCServer() {
                close();
            }
        }
    }
}