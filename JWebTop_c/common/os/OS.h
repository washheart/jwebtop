#ifndef jwebtop_common_OS_H_
#define jwebtop_common_OS_H_
#include <string>
#include <Windows.h>

using namespace std;
namespace jw{
	namespace os{
		namespace file{			
			// 根据文件名获取文件所在路径
			_declspec(dllexport) wstring GetFilePath(wstring appDefFile);

			// 得到程序启动目录
			_declspec(dllexport) wstring GetExePath();

			// 根据相对路径获取绝对路径
			// 如果relativePath已经是绝对路径，则直接返回
			// 否则将返回appPath+relativePath
			_declspec(dllexport) wstring getAbsolutePath(wstring &relativePath, wstring &appPath);
		};// End file namespace

		namespace process{
			// 创建一个新进程，返回的数据为进程中主线程的id
			_declspec(dllexport) DWORD createSubProcess(LPTSTR subProcess, LPTSTR szCmdLine);
		};// End process namespace
	}// End os namespace
}// End jw namespace
#endif