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
		String dir = "";
		if (args.length > 0) {
			dir = new File(args[0].trim()).getAbsolutePath();
		} else {
			dir = new File("").getAbsolutePath();
		}
		dir += File.separator;
		// dir = "D:\\c\\jwebtop\\jwebtop_c\\Release\\";
		System.out.println("dir = " + dir);
		String[] dlls = { //
		"d3dcompiler_43.dll",//
				"d3dx9_43.dll", //
				"icudt.dll", //
				"avutil-51.dll", //
				"avcodec-53.dll", //
				"avformat-53.dll",//
				"libcef.dll", //
				"libGLESv2.dll",//
				"libEGL.dll",//
				"zlibwapi.dll",
				//
				"msvcr120.dll",//
				"msvcp120.dll",//
				// "msvcrt.dll",//
				//
				"JWebTop.dll" };
		for (String dll : dlls) {
			String f = dir + dll;
			if (new File(f).exists()) System.load(f);
		}
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