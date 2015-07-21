#ifndef CEF_JWEBTOP_CONFIGS_H_
#define CEF_JWEBTOP_CONFIGS_H_
#include <string>
#include "include\cef_client.h"
class
	JWebTopConfigs{
public:
	//DWORD dwStyle = WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	//DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
	DWORD dwStyle = 0;             // 附加窗口样式，默认(0)不应用任何附加窗口样式
	DWORD dwExStyle = 0;           // 附加扩展窗口样式，默认(0)不应用任何附加扩展窗口样式

	int x = -1, y = -1;            // 窗口左上角坐标,当值为-1时不启用此变量		 
	int w = -1, h = -1;            // 窗口的宽、高，当值为-1时不启用此变量		
	int max;                       // 默认是否最大化

	bool enableDebug;              // 是否允许调试，默认(false)不显示调试工具
	bool enableResize;             // （尚未实现）是否可以调整窗口大小
	bool disableRefresh;           // （尚未实现）是否禁止刷新
	bool enableDrag;               // 是否通过页面进行拖动（默认可以）

	CefString appDefFile;          // 当前配置对应的配置文件（有可能为null）
	CefString appPath;             // 当前配置文件所在目录（不能为null）
	CefString url;                 // 要打开的链接地址
	CefString name;                // 窗口名称
	CefString icon;                // 窗口图标

	JWebTopConfigs(){}
	~JWebTopConfigs(){}

	// 根据相对路径获取绝对路径
	// 如果relativePath已经是绝对路径，则直接返回
	// 否则将返回appPath+relativePath
	CefString getAbsolutePath(std::wstring relativePath);
};

// 从数据定义文件读取
JWebTopConfigs loadConfigs(std::wstring appDefFile);

#endif