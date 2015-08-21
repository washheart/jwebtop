package org.jwebtop.demos.fm;

import java.io.File;

import org.jwebtop.JWebTopNative;

/**
 * 测试直接启动浏览器窗口，不嵌入
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
			String appfile = ctrl.getAppFile();
			JWebTopNative.getInstance().createJWebTop("JWebTop.exe", appfile);
			JWebTopNative.getInstance().createBrowser(appfile);
		} catch (Throwable e1) {
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