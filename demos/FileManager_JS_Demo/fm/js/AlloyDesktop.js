// 兼容之前的AlloyDesktop函数和事件
addEventListener("JWebTopReady",function(){	
	JWebTop.invokeJava=JWebTop.invokeRemote_Wait
	AlloyDesktop=JWebTop;
	AlloyDesktop.drag=JWebTop.startDrag;
	dispatchEvent(new CustomEvent('AlloyDesktopReady'));
});
addEventListener("JWebTopResize",function(v){	
	var e = new CustomEvent('AlloyDesktopWindowResize',v);
	dispatchEvent(e);
});
addEventListener("JWebTopMove",function(v){
	var e = new CustomEvent('AlloyDesktopWindowMove',v);
	dispatchEvent(e);
});
