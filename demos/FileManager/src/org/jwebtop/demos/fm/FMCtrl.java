package org.jwebtop.demos.fm;

import java.io.File;
import java.net.URL;
import java.text.DecimalFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

import org.jwebtop.JWebtopJSONDispater;

import com.alibaba.fastjson.JSONArray;
import com.alibaba.fastjson.JSONObject;

public class FMCtrl implements JWebtopJSONDispater {
	SimpleDateFormat fdate = new SimpleDateFormat("yyyy-MM-dd hh:mm:ss", java.util.Locale.getDefault());
	DecimalFormat fdoulbe = new DecimalFormat("####.###");

	@Override
	public String dispatcher(String json) {
		// 注意：下面的JSON解析方式未考虑性能，如果JSON数据过大，建议采用jackson按流模式解析
		try {
			JSONObject jo = JSONObject.parseObject(json);
			String method = jo.getString("method");
			if ("init".equals(method)) {
				return expand(jo);
			} else if ("expand".equals(method)) {
				return expand(jo);
			} else if ("showFiles".equals(method)) {
				return showFiles(jo);
			}
		} catch (Throwable e) {
			e.printStackTrace();
		}
		return "";
	}

	private String expand(JSONObject jo) {
		String id = jo.getString("id");
		File[] fs = null;
		if (id == null) {
			fs = File.listRoots();
		} else {
			File dir = new File(id);
			if (!dir.isDirectory()) return "";
			fs = dir.listFiles();
		}
		if (fs == null) return "";
		JSONArray jv = new JSONArray();
		for (File file : fs) {
			if (!file.isDirectory()) continue;
			JSONObject jvv = new JSONObject();
			jvv.put("id", file.getAbsolutePath());
			String fn = file.getName();
			jvv.put("name", (fn == null || fn.trim().length() == 0) ? file.getAbsolutePath() : fn);
			jvv.put("isParent", true);
			jv.add(jvv);
		}
		return jv.toJSONString();
	}

	private String showFiles(JSONObject jo) {
		String id = jo.getString("id");
		System.out.println("id = " + id);
		File dir = new File(id);
		if (!dir.isDirectory()) return "";
		File[] fs = dir.listFiles();
		if (fs == null) return "";
		JSONArray jv = new JSONArray();
		for (File file : fs) {
			if (!file.isFile()) continue;
			JSONObject jvv = new JSONObject();
			jvv.put("id", file.getAbsolutePath());
			jvv.put("name", file.getName());
			jvv.put("lastdate", fdate.format(new Date(file.lastModified())));
			long size = file.length();
			String sizeS;
			if (size <= 1024) {
				sizeS = size + "B";
			} else if (size < 1024 * 1024) {
				sizeS = "K";
				sizeS = fdoulbe.format(1.0 * size / 1024) + "K";
			} else if (size < 1024 * 1024 * 1024) {
				sizeS = fdoulbe.format(1.0 * size / (1024 * 1024)) + "M";
			} else {
				sizeS = fdoulbe.format(1.0 * size / (1024 * 1024 * 1024)) + "G";
			}
			jvv.put("size", sizeS);
			jv.add(jvv);
		}
		return jv.toJSONString();
	}

	public String getAppFile() {
		File f = new File("res/fm/index.app");
		if (f.exists()) return f.getAbsolutePath();
		URL url = FMCtrl.class.getClassLoader().getResource("");
		return url.getFile() + "../../demos/FileManager/res/fm/index.app";
	}
}