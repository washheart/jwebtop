// 兼容之前的AlloyDesktop函数和事件
addEventListener("JWebTopReady",function(){	
	JWebTop.invokeJava=JWebTop.invokeRemote_Wait;
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
function invokeByDLL(jsonValue){
	// alert(jsonValue);
	return invokeByJava(jsonValue);
	// return "已经调用了js代码invokeByDLL————"+jsonValue.value;
}

function testInvoke(str){
	return "已经调用了js代码————"+str;
}