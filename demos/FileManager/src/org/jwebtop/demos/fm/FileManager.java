package org.jwebtop.demos.fm;

import java.io.File;
import java.io.IOException;

import org.jwebtop.JWebTopNative;

/**
 * 测试嵌入浏览器到Swing窗口的例子<br/>
 * 左侧是一个列表浏览器窗口，右侧是一个列表详细说明浏览器窗口
 * 
 * @author washheart@163.com
 */
public class FileManager {
	private static void initDll(String[] args) {
		String dll = "JWebTop.dll";
		File tmp = null;
		if (args.length > 0) {
			args[0] = args[0].trim();
			tmp = new File(args[0]);
			dll = tmp.getAbsolutePath();
		} else {
			tmp = new File(dll);
			if (tmp.isFile()) {
				dll = tmp.getAbsolutePath();
			}
		}
		System.out.println("成功加载dll文件 = " + dll);
		System.load(dll);
	}

	private FMCtrl ctrl;

	public FileManager() {
		this.ctrl = new FMCtrl();
		JWebTopNative.getInstance().setJsonHandler(this.ctrl);
		// 创建浏览器
		try {
			JWebTopNative.getInstance().createJWebTop(ctrl.getAppFile());
		} catch (IOException e1) {
			e1.printStackTrace();
		}
	}

	public static void main(String[] args) {
		try {
			initDll(args);
			new FileManager();
		} catch (Throwable e) {
			e.printStackTrace();
		}
	}
}