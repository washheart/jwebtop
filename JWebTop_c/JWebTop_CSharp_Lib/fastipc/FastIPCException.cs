using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace org {
    namespace fastipc {
        /**
         * FastIPC创建和使用过程中出现的异常。<br/>
         * 源码：https://github.com/washheart/fastipc<br/>
         * 源码：http://git.oschina.net/washheart/fastipc<br/>
         * 说明：https://github.com/washheart/fastipc/wiki<br/>
         * 
         * @author washheart@163.com
         */
        class FastIPCException : ApplicationException {
            public FastIPCException(string msg) : base(msg) { }
            public FastIPCException(string msg, Exception inner) : base(msg, inner) { }
        }
    }
}