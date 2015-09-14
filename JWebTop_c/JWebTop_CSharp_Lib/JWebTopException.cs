using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace JWebTop {
    public class JWebTopException : ApplicationException {

        public JWebTopException(string msg) : base(msg) { }
        public JWebTopException(string msg, Exception inner) : base(msg, inner) { }
    }
}
