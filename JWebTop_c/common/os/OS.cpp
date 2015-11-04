#include "OS.h"
#include <sstream>
#include "common/util/StrUtil.h"
#include "common/tests/TestUtil.h"

using namespace std;
namespace jw{
	namespace os{
		namespace file{
			// 根据文件名获取文件所在路径
			wstring GetFilePath(wstring appDefFile){
				const int BUFSIZE = 4096;
				TCHAR  buffer[BUFSIZE] = TEXT("");
				GetFullPathName(LPCWSTR(appDefFile.data()), BUFSIZE, buffer, NULL);// 获取文件的绝对路径
				wstring pp(buffer);
				int p = pp.find_last_of('\\');
				if (p != -1)pp = pp.substr(0, p + 1);
				return pp;
			}

			// 得到程序启动目录
			wstring GetExePath(){
				TCHAR   szPath[1000];
				GetModuleFileName(NULL, szPath, MAX_PATH);
				wstring path(szPath);
				path = path.substr(0, path.find_last_of('\\') + 1);
				return path;
			}

			// 根据相对路径获取绝对路径
			// 如果relativePath已经是绝对路径，则直接返回
			// 否则将返回appPath+relativePath
			wstring getAbsolutePath(wstring &relativePath, wstring &appPath){
				if (relativePath.find(L":") == -1){// 如果指定的路径是相对路径
					wstring _path(appPath);
					_path.append(relativePath);
					return _path;
				}
				return relativePath;
			}
		}// End file namespace
		namespace process{
			// 创建一个新进程，返回的数据为进程中主线程的id
			DWORD createSubProcess(LPTSTR subProcess, LPTSTR szCmdLine){
				std::wstring cmd;
				if (subProcess[0] == L'"'){// 如果url中已以双引号开头，那么这里就不再添加双引号
					cmd.append(subProcess);
				}
				else{// 把子进程用双引号包含起来，避免恶意程序问题
					cmd.append(L"\"");
					cmd.append(subProcess);
					cmd.append(L"\"");
				}
				cmd += L" "; cmd += szCmdLine;// 追加一个空格在命令行参数的前面，否则新进程的szCmdLine会取不到参数

				STARTUPINFO sui;
				ZeroMemory(&sui, sizeof(STARTUPINFO)); // 对一个内存区清零，最好用ZeroMemory, 它的速度要快于memset
				sui.cb = sizeof(STARTUPINFO);

				PROCESS_INFORMATION pi; // 保存了所创建子进程的信息
				//DWORD dwCreationFlags = DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP;
				DWORD dwCreationFlags = CREATE_NEW_PROCESS_GROUP;
				if (CreateProcess(NULL, LPTSTR(cmd.c_str()), // 第一个参数置空，可执行文件和命令行放到一起避免恶意程序问题
					NULL, NULL, FALSE,//句柄不继承 
					dwCreationFlags, //使用正常优先级 
					NULL, //使用父进程的环境变量 
					NULL, //指定工作目录 
					&sui, &pi))
				{
					CloseHandle(pi.hProcess); // 子进程的进程句柄
					CloseHandle(pi.hThread);  // 子进程的线程句柄，windows中进程就是一个线程的容器，每个进程至少有一个线程在执行
					return pi.dwThreadId;
				}
				//else{// else的处理主要是为了方便在VS环境下调试程序，因为VS下启动的程序默认的工作目录是$(ProjectDir)，如果改为$(SolutionDir)$(Configuration)\则不需要else的处理
				//	TCHAR   szPath[1000];
				//	GetModuleFileName(NULL, szPath, MAX_PATH);
				//	std::wstring path(szPath);
				//	path = path.substr(0, path.find_last_of('\\') + 1);
				//	path += subProcess;
				//	if (CreateProcess(path.c_str(), LPTSTR(cmd.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &sui, &pi))
				//	{
				//		CloseHandle(pi.hProcess); // 子进程的进程句柄
				//		CloseHandle(pi.hThread);  // 子进程的线程句柄，windows中进程就是一个线程的容器，每个进程至少有一个线程在执行
				//		return pi.dwThreadId;
				//	}
				//}
				return 0;
			}
		}// End process namespace
	}// End os namespace
}// End jw namespace