//closeBtn.onclick = function() {
//	AlloyDesktop.close();
//}
var fm = {
	isMax : false,
	prex : 0,
	prey : 0,
	prew : 0,
	preh : 0
};
$("#miniBtn").click(function() {
	AlloyDesktop.mini();
})
$("#maxBtn").click(function() {
	if (fm.isMax) {
		// AlloyDesktop.restore();
		AlloyDesktop.setBound(fm.prex, fm.prey, fm.prew, fm.preh);
	} else {
		var jo = AlloyDesktop.getPos();
		fm.prex = jo.x;
		fm.prey = jo.y;
		jo = AlloyDesktop.getSize();
		fm.prew = jo.width;
		fm.preh = jo.height;
		AlloyDesktop.max();
	}
	fm.isMax = !fm.isMax;
})
$("#closeBtn").click(function() {
	AlloyDesktop.close();
})

$("#titleBar").mousedown(function() {
	AlloyDesktop.drag();
})
$("#titleBar").mouseup(function() {
	AlloyDesktop.stopDrag();
})
var sizeHandler = function(e) {
	var jo = AlloyDesktop.getSize();
	$("#treeDemo").height(jo.height - 40 - 10 - 2);// 40=标题栏高度，10=treeDemo上下padding，2=上下border
	$("#files").height(jo.height - 40 - 21);// 40=标题栏高度，10=上下padding，21=上面的空白
	$("#files").width(jo.width - $("#treeDemo").width() - 10 - 2);// 10=treeDemo左右padding，2=左右border
	setTableHeaderWidth();
}
addEventListener("AlloyDesktopWindowResize", sizeHandler);

var setting = {
	view : {
		selectedMulti : false
	},
	callback : {
		onClick : nodeClicked,
		onExpand : nodeExpand
	}
};

var zTree;
function nodeClicked(event, treeId, treeNode) {
	var jsons = AlloyDesktop.invokeJava(JSON.stringify({
		method : "showFiles",
		id : treeNode.id
	}));// 调用java让其进行初始化操作
	var jo = jsons == "" ? {} : eval("(" + jsons + ")")
	var filesHtml = "";
	if (jsons == "" || jo.length == 0) {
		filesHtml = "<tr><td colspan='3'>请在左侧选择一个目录</td></tr>";
	} else for ( var idx in jo) {
		var file = jo[idx];
		filesHtml += "<tr><td>" + file.name + "</td><td>" + file.lastdate + "</td><td>" + file.size + "</td></tr>"
	}
	$("#filesBody").html(filesHtml);
	setTableHeaderWidth()
}
function setTableHeaderWidth() {
	var header = $("thead th"), firstRows = $("tbody tr:first td");
	if (firstRows.length > 1) {
		$(header[0]).width($(firstRows[0]).width())
		$(header[1]).width($(firstRows[1]).width())
		$(header[2]).width($(firstRows[2]).width())
	} else {
		var w = $("#files").width();
		$(header[0]).width(w * 0.5)
		$(header[1]).width(w * 0.3)
		$(header[2]).width(w * 0.2)
	}
}

function nodeExpand(event, treeId, treeNode) {
	var jsons = AlloyDesktop.invokeJava(JSON.stringify({
		method : "expand",
		id : treeNode.id
	}));// 调用java让其进行初始化操作
	var jo = eval("(" + jsons + ")")
	zTree.addNodes(treeNode, jo, true)
};

// 被java回调的js函数
function invokeByJava(jo) {
	// if (!jo || !jo.method) return;
}
function afterBrowserInit() {// 直接的js代码会执行两次，所以这里需要通过
	$.fn.zTree.init($("#treeDemo"), setting, []);
	zTree = $.fn.zTree.getZTreeObj("treeDemo");
	var jsons = AlloyDesktop.invokeJava(JSON.stringify({
		method : "expand"
	}));// 调用java让其进行初始化操作
	var jo = eval("(" + jsons + ")")
	zTree.addNodes(null, jo, true)
	sizeHandler();
}
onbeforeunload=function(){return "您确认要关闭软件？";}
onunload=function(){
	AlloyDesktop.invokeJava(JSON.stringify({
		method : "closing"// 关闭事件
	}));
}
addEventListener("AlloyDesktopReady", afterBrowserInit);