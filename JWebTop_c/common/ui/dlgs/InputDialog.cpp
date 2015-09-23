#include "InputDialog.h"

#include <windows.h>
#include <WinUser.h>
#include <commdlg.h>
#include <shellapi.h>
#include <direct.h>
#include <sstream>
#include <string>

namespace jw{
	namespace ui{
		namespace Dlgs{
#define BTN_OK			101
#define BTN_CANCEL		102
#define TXT_INPUT		103

#define BUTTON_WIDTH	72
#define BUTTON_HEIGHT	24
#define MARGIN			5

			void InputDialog::initComponents(HWND parent,HWND hWnd, HMODULE hInstance){
				btnOk = CreateWindow(TEXT("button"),//必须为：button    
					TEXT("确定"),//按钮上显示的字符    
					WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd,
					(HMENU)BTN_OK/*事件id*/, hInstance, NULL);

				btnCancel = CreateWindow(TEXT("button"),//必须为：button    
					TEXT("取消"),//按钮上显示的字符    
					WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd,
					(HMENU)BTN_CANCEL, hInstance, NULL);

				txtInput = CreateWindow(TEXT("edit"), NULL,
					WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER
					/*多行文本的特性| WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL*/,
					0, 0, 0, 0,
					hWnd, (HMENU)TXT_INPUT, hInstance, NULL);
				if (parent != NULL){
					hFont = (HFONT)SendMessage(parent, WM_GETFONT, 0, 0);
					if (!hFont){
						LOGFONT font;
						ZeroMemory(&font, sizeof(font));
						lstrcpy(font.lfFaceName, L"宋体");
						font.lfHeight = 12;
						hFont = CreateFontIndirect(&font);

					}
					if (hFont){
						SendMessage(hWnd, WM_SETFONT, WPARAM(hFont), TRUE);
						SendMessage(btnOk, WM_SETFONT, WPARAM(hFont), TRUE);
						SendMessage(btnCancel, WM_SETFONT, WPARAM(hFont), TRUE);
						SendMessage(txtInput, WM_SETFONT, WPARAM(hFont), TRUE);
					}
				}
			}

			void InputDialog::onPaint(HWND hWnd){
				PAINTSTRUCT ps;
				RECT	rect;
				GetClientRect(hWnd, &rect);
				HDC hdc = BeginPaint(hWnd, &ps);
				rect.left = MARGIN;
				SelectObject(hdc, hFont);
				rect.bottom = BUTTON_HEIGHT;
				DrawText(hdc, tip, -1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
				EndPaint(hWnd, &ps);
			}
			void InputDialog::onSize(HWND hWnd){
				RECT	rect;
				GetClientRect(hWnd, &rect);
				MoveWindow(txtInput, MARGIN, BUTTON_HEIGHT + MARGIN, rect.right - MARGIN * 2, BUTTON_HEIGHT, TRUE);
				MoveWindow(btnOk, rect.right - (BUTTON_WIDTH + MARGIN) * 2, rect.bottom - BUTTON_HEIGHT - MARGIN, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
				MoveWindow(btnCancel, rect.right - BUTTON_WIDTH - MARGIN, rect.bottom - BUTTON_HEIGHT - MARGIN, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
			}
			void InputDialog::onClose(HWND hWnd){
				this->result.clear();// 清除输入结果
				this->dlgResult = IDCLOSE;
			}

			void InputDialog::onCommand(HWND hWnd, DWORD cmd){
				switch (cmd)
				{
				case BTN_OK:
				{
							   int iLength = GetWindowTextLength(txtInput);
							   TCHAR *szBuffer;		//缓冲区
							   if (iLength > 0){
								   szBuffer = (TCHAR *)malloc(iLength * 2);
								   GetWindowText(txtInput, szBuffer, iLength + 1);
								   this->result.clear();
								   this->result.append(szBuffer);
								   this->dlgResult = IDOK;
							   }
							   else{
								   this->dlgResult = IDCLOSE;
							   }
				}
					DestroyWindow(hWnd);
					return;
				case BTN_CANCEL:
					this->result.clear();// 清除输入结果
					this->dlgResult = IDCANCEL;
					DestroyWindow(hWnd);
					return;

				}
			}
			LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
				InputDialog* owner = reinterpret_cast<InputDialog*>(GetWindowLong(hWnd, GWL_USERDATA));
				switch (message)
				{
				case WM_PAINT:
					owner->onPaint(hWnd);
					return 0;
				case WM_SIZE:
					owner->onSize(hWnd);
					return 0;
				case WM_CLOSE:
					owner->onClose(hWnd); break;
				case WM_COMMAND:
					owner->onCommand(hWnd, LOWORD(wParam)); return 0;
				case WM_DESTROY:
					PostQuitMessage(0);
					return 0;
				}
				return DefWindowProc(hWnd, message, wParam, lParam);
			}

			int  InputDialog::show(HWND parent, const wstring &title, const wstring tip, const wstring defaultResult){
				this->tip = LPTSTR(tip.c_str());
				//base::FilePath file_path;
				HMODULE hModule = NULL;
				//// Try to load the dialog from the DLL.
				////if (PathService::Get(base::FILE_MODULE, &file_path))
				////	hModule = ::GetModuleHandle(file_path.value().c_str());
				////if (!hModule)
				hModule = ::GetModuleHandle(NULL);
				static TCHAR szAppName[] = TEXT("JWebTop_InputDialog");
				static	WNDCLASS    wndclass;
				if (!wndclass.lpfnWndProc){
					wndclass.lpfnWndProc = WndProc;
					wndclass.style = CS_HREDRAW | CS_VREDRAW |DS_SHELLFONT;
					
					wndclass.hInstance = hModule;
					wndclass.cbClsExtra = 0;
					wndclass.cbWndExtra = 0;
					wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
					wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
					wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
					wndclass.lpszClassName = szAppName;
					wndclass.lpszMenuName = NULL;

					if (!RegisterClass(&wndclass))
					{
						MessageBox(NULL, TEXT("无法注册窗口类!"), TEXT("错误"), MB_OK | MB_ICONERROR);
						return 0;
					}
				}
				RECT rt;
				GetWindowRect(parent, &rt);

				int  w = 400, h = 130;
				int x = rt.left + (rt.right - rt.left - w) / 2;
				int y = rt.top + (rt.bottom - rt.top - h) / 2;
				HWND        hWnd;
				hWnd = CreateWindow(szAppName, LPTSTR(title.c_str()), WS_OVERLAPPEDWINDOW,
					x, y,
					w, h,
					parent, NULL, hModule, NULL);
				this->initComponents(parent, hWnd, hModule);
				if (!defaultResult.empty()){
					SetWindowText(txtInput, LPTSTR(defaultResult.c_str()));
				}
				SetWindowLong(hWnd, GWL_USERDATA, reinterpret_cast<LPARAM>(this));
				ShowWindow(hWnd, SW_SHOW);
				UpdateWindow(hWnd);
				EnableWindow(parent, false);
				MSG            msg;
				while (GetMessage(&msg, NULL, 0, 0))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				EnableWindow(parent, true);
				SetForegroundWindow(parent);// 如果没有对SetForegroundWindow的调用，在关闭模态窗口之后父窗口会最小化。
				return msg.wParam;
			}

			int InputDialog::ShowInputDialog(HWND parent, wstring &result){
				return InputDialog::ShowInputDialog(parent, result, wstring(), wstring(), wstring());
			}

			int InputDialog::ShowInputDialog(HWND parent, wstring &result, const wstring &title, const wstring tip, const wstring defaultResult){
				InputDialog dlg;
				dlg.show(parent, title, tip, defaultResult);
				result.append(dlg.result);
				return dlg.dlgResult;
			}
		}
	}
}