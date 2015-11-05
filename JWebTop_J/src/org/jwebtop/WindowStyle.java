package org.jwebtop;

/**
 * Windows系统的窗口风格<br>
 * 源码：https://github.com/washheart/jwebtop<br>
 * 说明：https://github.com/washheart/jwebtop/wiki<br>
 * 
 * @author washheart@163.com
 */
public interface WindowStyle {

	/*
	 * Window Styles
	 */
	int WS_OVERLAPPED = 0x00000000;
	int WS_POPUP = 0x80000000;
	int WS_CHILD = 0x40000000;
	int WS_MINIMIZE = 0x20000000;
	int WS_VISIBLE = 0x10000000;
	int WS_DISABLED = 0x08000000;
	int WS_CLIPSIBLINGS = 0x04000000;
	int WS_CLIPCHILDREN = 0x02000000;
	int WS_MAXIMIZE = 0x01000000;
	int WS_CAPTION = 0x00C00000; /* WS_BORDER | WS_DLGFRAME */
	int WS_BORDER = 0x00800000;
	int WS_DLGFRAME = 0x00400000;
	int WS_VSCROLL = 0x00200000;
	int WS_HSCROLL = 0x00100000;
	int WS_SYSMENU = 0x00080000;
	int WS_THICKFRAME = 0x00040000;
	int WS_GROUP = 0x00020000;
	int WS_TABSTOP = 0x00010000;

	int WS_MINIMIZEBOX = 0x00020000;
	int WS_MAXIMIZEBOX = 0x00010000;

	int WS_TILED = WS_OVERLAPPED;
	int WS_ICONIC = WS_MINIMIZE;
	int WS_SIZEBOX = WS_THICKFRAME;

	/*
	 * Common Window Styles
	 */
	int WS_OVERLAPPEDWINDOW = (WS_OVERLAPPED | //
			WS_CAPTION | //
			WS_SYSMENU | //
			WS_THICKFRAME | //
			WS_MINIMIZEBOX | //
	WS_MAXIMIZEBOX);
	int WS_TILEDWINDOW = WS_OVERLAPPEDWINDOW;

	int WS_POPUPWINDOW = (WS_POPUP | //
			WS_BORDER | //
	WS_SYSMENU);

	int WS_CHILDWINDOW = (WS_CHILD);

	/*
	 * Extended Window Styles
	 */
	int WS_EX_DLGMODALFRAME = 0x00000001;
	int WS_EX_NOPARENTNOTIFY = 0x00000004;
	int WS_EX_TOPMOST = 0x00000008;
	int WS_EX_ACCEPTFILES = 0x00000010;
	int WS_EX_TRANSPARENT = 0x00000020;
	// #if(WINVER >= =0x0400)
	int WS_EX_MDICHILD = 0x00000040;
	int WS_EX_TOOLWINDOW = 0x00000080;
	int WS_EX_WINDOWEDGE = 0x00000100;
	int WS_EX_CLIENTEDGE = 0x00000200;
	int WS_EX_CONTEXTHELP = 0x00000400;

	// #endif /* WINVER >= =0x0400 */
	// #if(WINVER >= =0x0400)

	int WS_EX_RIGHT = 0x00001000;
	int WS_EX_LEFT = 0x00000000;
	int WS_EX_RTLREADING = 0x00002000;
	int WS_EX_LTRREADING = 0x00000000;
	int WS_EX_LEFTSCROLLBAR = 0x00004000;
	int WS_EX_RIGHTSCROLLBAR = 0x00000000;

	int WS_EX_CONTROLPARENT = 0x00010000;
	int WS_EX_STATICEDGE = 0x00020000;
	int WS_EX_APPWINDOW = 0x00040000;

	int WS_EX_OVERLAPPEDWINDOW = (WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE);
	int WS_EX_PALETTEWINDOW = (WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST);

	// #endif /* WINVER >= =0x0400 */

	// #if(_WIN32_WINNT >= =0x0500)
	int WS_EX_LAYERED = 0x00080000;

	// #endif /* _WIN32_WINNT >= =0x0500 */

	// #if(WINVER >= =0x0500)
	int WS_EX_NOINHERITLAYOUT = 0x00100000; // Disable inheritence of mirroring by children
	int WS_EX_LAYOUTRTL = 0x00400000; // Right to left mirroring
	// #endif /* WINVER >= =0x0500 */

	// #if(_WIN32_WINNT >= =0x0501)
	int WS_EX_COMPOSITED = 0x02000000;
	// #endif /* _WIN32_WINNT >= =0x0501 */
	// #if(_WIN32_WINNT >= =0x0500)
	int WS_EX_NOACTIVATE = 0x08000000;
	// #endif /* _WIN32_WINNT >= =0x0500 */

}