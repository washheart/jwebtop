#include<io.h>
#include <direct.h>  
#include <string>
#include <fstream>

#include <sstream>

#include "FileUtil.h"
#include "common/tests/TestUtil.h"

namespace jw {
	namespace file {
		bool isExsit(const char* file) {
			return file != nullptr && _access(file, 0) == 0;
		}
		bool isExsit(const wchar_t* file) {
			return file != nullptr && _waccess(file, 0) == 0;
		}
		__int64 getLength(const char* file) {
			HANDLE hFile = CreateFileA(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (INVALID_HANDLE_VALUE != hFile) {
				LARGE_INTEGER size;
				::GetFileSizeEx(hFile, &size);
				__int64 nSize1 = size.QuadPart;
				CloseHandle(hFile);
				return nSize1;
			}
			return -1;
		}

		__int64 getLength(const wchar_t* file) {
			HANDLE hFile = CreateFileW(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (INVALID_HANDLE_VALUE != hFile) {
				LARGE_INTEGER size;
				::GetFileSizeEx(hFile, &size);
				__int64 nSize1 = size.QuadPart;
				CloseHandle(hFile);
				return nSize1;
			}
			return -1;
		}

		bool createFile(const char* file) {
			if (file == nullptr)return false;// 字符串不合法
			if (strlen(file) == 0)return false;// 字符串不合法
			if (_access(file, 0) == 0)return true;// 文件已经存在，不再执行创建
			HANDLE hOpenFile = (HANDLE)CreateFileA(file, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, NULL, NULL);
			if (hOpenFile == INVALID_HANDLE_VALUE) {
				hOpenFile = NULL;
				return false;
			}
			return true;
		}

		bool delFile(const char* file) {
			if (file == nullptr)return false;// 字符串不合法
			if (strlen(file) == 0)return false;// 字符串不合法
			if (_access(file, 0) != 0)return true;// 文件不存在，不再执行删除
			DWORD fileAttributes = GetFileAttributesA(file);
			fileAttributes &= ~FILE_ATTRIBUTE_READONLY;
			SetFileAttributesA(file, fileAttributes);
			return 0 != ::DeleteFileA(file);
		}

		bool mkdirs(const char *path) {
			if (nullptr == path) return false;
			char* pszDir = _strdup(path); // 复制字符串：注意后面一定要释放此空间
			int len = strlen(pszDir);
			if (len == 0)return false;
			int i, iRet;
			// 按字符串从前向后，创建多级目录  
			for (i = 0; i < len; i++) {
				if (pszDir[i] == '\\' || pszDir[i] == '/') {
					pszDir[i] = '\0';
					iRet = _access(pszDir, 0);// 检测文件夹是否存在
					if (iRet != 0) {// 当文件夹不存在时，才进行创建
						iRet = _mkdir(pszDir);
						if (iRet != 0) {// 创建时候失败
							free(pszDir);// 需要释放空间
							return false;
						}
					}
					pszDir[i] = '/';// 把所有“\”换成“/”，兼容windows和linux  
				}
			}
			if (pszDir[i--] != '/') {
				iRet = _access(pszDir, 0);// 检测文件夹是否存在
				if (iRet != 0)iRet = _mkdir(pszDir);// 如果不是已文件分隔符结尾，那么需要创建
			}
			free(pszDir);
			return iRet == 0;
		}
		bool createDir(const char* folder) {
			if (folder == nullptr)return false;// 字符串不合法
			if (_access(folder, 0) == 0)return true;// 目录已存在，也认为创建成功
			if (0 != _mkdir(folder)) return false;// 创建目录失败
			return true;
		}

		// 不检查是否null，是否是文件
		void ___listFiles(const wchar_t * dir, FeedFile feedFile, int level, void* feedfile_ctx) {
			int size = wcslen(dir) + 4 /* 4=strlen("\\*.*") */ + 1;
			wchar_t * dirNew = new wchar_t[size];// C++方式，与delete配对			
			wcscpy_s(dirNew, size, dir);
			wcscat_s(dirNew, size, L"\\*.*");    // 在目录后面加上"\\*.*"进行第一次搜索

			intptr_t handle;
			_wfinddata_t findData;

			handle = _wfindfirst(dirNew, &findData);
			if (handle == -1) return;// 检查是否成功
			level++;
			do {
				if (findData.attrib & _A_SUBDIR) {
					if (wcscmp(findData.name, L".") == 0 || wcscmp(findData.name, L"..") == 0) continue;
					feedFile(feedfile_ctx,dir, findData.name,true, level);

					// 在目录后面加上"\\"和搜索到的目录名进行下一次搜索
					int size = wcslen(dir) + 2 /* 2=strlen("\\") */ + wcslen(findData.name) + 1;
					wchar_t * nestdir = new wchar_t[size];// C++方式，与delete配对		
					wcscpy_s(nestdir, size, dir);
					wcscat_s(nestdir, size,L"\\");    // 在目录后面加上"\\*.*"进行第一次搜索
					wcscat_s(nestdir, size, findData.name);
					___listFiles(nestdir, feedFile, level, feedfile_ctx);
					//writeLog("FileUtil_____listFiles_file");writeLog("dir"); writeLog("\r\n");
				} else {
					//writeLog("FileUtil_____listFiles_file");writeLog("dir"); writeLog("\r\n");					
					feedFile(feedfile_ctx, dir, findData.name, false, level);
				}
			} while (_wfindnext(handle, &findData) == 0);

			_findclose(handle);    // 关闭搜索句柄
		}

		void listFiles(const wchar_t * dir, FeedFile feedFile, void* feedfile_ctx) {
			if (dir == nullptr)return;// 空数据检查
			if (_waccess(dir, 0) != 0)return;// 检查文件或目录是否存在，不存在则直接返回
			if (GetFileAttributesW(dir)&FILE_ATTRIBUTE_DIRECTORY) {
				___listFiles(dir, feedFile, 0, feedfile_ctx);// 是目录，进入递归函数进行检查
			} else {
				feedFile(feedfile_ctx, L"", const_cast<wchar_t*>(dir),false, 0);
			}
		}
	}
}