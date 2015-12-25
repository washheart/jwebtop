/// 窗口以及空间布局等相关的方法和变量，不涉及fastipc相关的调用

#include "Demo.h"
#include "common/ui/dlgs/InputDialog.h"

static TCHAR szAppName[] = TEXT("测试嵌入两个浏览器到窗口");
// 窗口事件处理函数
LRESULT CALLBACK SimpleWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND g_hWnd;
DemoCtrl * ctrl;

// 用于窗口创建后执行其他操作。比如启动服务器
void _windowCreated(HWND hWnd){
	ctrl = new DemoCtrl();
	ctrl->init();
}
BOOL __createWin(HINSTANCE hInstance)
{
	HWND hWnd = CreateWindow(szAppName, szAppName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	if (!hWnd)return FALSE;
	g_hWnd = hWnd;
	int x = MARGIN;
	HWND btnAddNote = CreateWindow(TEXT("button"),//必须为：button    
		TEXT("新增日记"),//按钮上显示的字符    
		WS_CHILD | WS_VISIBLE, x, 10, BTN_WIDTH, BAR_HEIGHT, hWnd,
		(HMENU)BTN_ADD_NOTE, hInstance, NULL);
	x = x + BTN_WIDTH + MARGIN;
	HWND btnDelNote = CreateWindow(TEXT("button"),//必须为：button    
		TEXT("删除日记"),//按钮上显示的字符    
		WS_CHILD | WS_VISIBLE, x, 10, BTN_WIDTH, BAR_HEIGHT, hWnd,
		(HMENU)BTN_DEL_NOTE, hInstance, NULL);

	HFONT hFont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
	if (!hFont){
		LOGFONT font;
		ZeroMemory(&font, sizeof(font));
		lstrcpy(font.lfFaceName, L"宋体");
		font.lfHeight = 12;
		hFont = CreateFontIndirect(&font);
	}
	if (hFont){
		SendMessage(hWnd, WM_SETFONT, WPARAM(hFont), TRUE);
		SendMessage(btnAddNote, WM_SETFONT, WPARAM(hFont), TRUE);
		SendMessage(btnDelNote, WM_SETFONT, WPARAM(hFont), TRUE);
	}
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

void relayout(HWND hWnd){
	RECT	rect;
	GetClientRect(hWnd, &rect);
	int w1 = 300, w2 = rect.right - MARGIN * 2 - 300;
	int y = MARGIN + BAR_HEIGHT + MARGIN;
	int h = (rect.bottom - y - MARGIN/*两个文本框之间的分隔*/ - MARGIN/*下面文本框与窗口边框间的分隔*/);
	if (h < BAR_HEIGHT)h = BAR_HEIGHT;
	HWND tmp = ctrl->getListBrowser();
	if (tmp != NULL)MoveWindow(tmp, MARGIN, y, w1, h, TRUE);
	tmp = ctrl->getDetailBrowser();
	if (tmp != NULL)MoveWindow(tmp, MARGIN + MARGIN + w1, y, w2, h, TRUE);
}

// 程序进入点
void createWin(HINSTANCE hInstance){
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = SimpleWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	if (!RegisterClass(&wndclass)){
		MessageBox(NULL, TEXT("无法创建窗口，操作系统版本过低！"), szAppName, MB_ICONERROR);
	} else{
		DWORD dwStyle = WS_OVERLAPPEDWINDOW;
		__createWin(hInstance);
	}
}
void _addNode(){
	wstring note;
	jw::ui::Dlgs::InputDialog::ShowInputDialog(g_hWnd, std::ref(note));
	if (note.size() == 0)return;
	ctrl->addNote(note);
}
void _delNote(){
	ctrl->delNote();
}
LRESULT CALLBACK SimpleWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		relayout(hWnd);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case BTN_ADD_NOTE:_addNode(); break;
		case BTN_DEL_NOTE:_delNote(); break;
		}
		break;
	case WM_CREATE:
		_windowCreated(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}