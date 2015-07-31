// 兼容之前的AlloyDesktop函数和事件
addEventListener("JWebTopReady",function(){	
	AlloyDesktop=JWebTop;
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
