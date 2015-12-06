using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace JWebTop {
    public interface JWebTopAppInited {
        void onJWebTopAppInited();
    }
    public interface JWebTopBrowserCreated {
        void onJWebTopBrowserCreated(int browserHWnd);
    }
    public interface JWebTopJSReturn {
        void onJWebTopJSReturn(String jsonString);
    }

    public interface JWebtopJSONDispater {
        /**
	     * 处理浏览器发来的json内容，并返回结果
	     * 
	     * @param browserHWnd
	     *            需要执行json的浏览器句柄
	     * @param json
	     *            浏览器端发来的要执行内容
	     * @return 本地处理后的结果
	     */
        string dispatcher(int browserHWnd, string json);

        /**
         * Java中需要泽洋处理（c#是否有必要呢？）
         * 在Dispatcher之前，重新设置当前线程的类加载器。<br/>
         * 否则会使用BootstrapClassLoader来加载类，这样新加载类getClassLoader时会返回null，导致无法继续记载资源。<br/>
         * 一般可以这样实现：首先记录JWebtopJSONDispater实现类的加载器，然后在resetThreadClassLoader方法设置。<br/>
         * 如果使用了插件系统、或者实现了其他加载方式，也需要在resetThreadClassLoader方法中处理好。<br/>
         */
        void resetThreadClassLoader();
    }
}
