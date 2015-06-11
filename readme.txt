Use JNI to connect Java and WebTop(base on cef).
通过JNI连接Java和webtop（基于CEF）。

WebTop是依据CEF封装的，可以通过一些js接口与浏览器通信（与nw，node-webkit有点类似，但本地访问能力不强），其目标应该用于开发一些轻客户端（猜的，^_^），但是此项目现在不维护了，:(。 
JWebTop对WebTop进行了一些封装，将其变为一个DLL，可以通过JNI来与Java进行交互，这样就可以用Java来使用CEF了。
但是JWebTop和jcef等框架不同，JWebTop不是把CEF当成一个控件嵌入到到现有Java程序中，只是和CEF交换数据。 

两个例子（计划，但尚未实现）： 
 1.FileManager：用WebTop做界面，java做后台实现文件的管理。 
 2.WithinSwing：将WebTop作为一个“控件”，嵌入到Swing程序中。
