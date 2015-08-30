#ifndef components_dlgs_inputdialog_H_
#define components_dlgs_inputdialog_H_

#include <windows.h>
#include <string>
using namespace std;
namespace jw{
	namespace ui{
		namespace Dlgs{
			class InputDialog{
			public:
				static int ShowInputDialog(HWND parent, wstring &result);

				static int ShowInputDialog(HWND parent, wstring &result, const wstring &title, const wstring tip, const wstring defaultResult);


				void onPaint(HWND hWnd);
				void onSize(HWND hWnd);
				void onClose(HWND hWnd);
				void onCommand(HWND hWnd, DWORD cmd);
			private:
				HWND btnOk = NULL, btnCancel = NULL;
				HWND txtInput;
				HWND parent;
				HFONT hFont;
				LPTSTR tip;
				int dlgResult = 0;
				wstring result;

				InputDialog(){}
				~InputDialog(){}
				void initComponents(HWND parent, HWND hWnd, HMODULE lParam);
				int  show(HWND parent, const wstring &title, const wstring tip, const wstring defaultResult);
			};
		}
	}
}
#endif