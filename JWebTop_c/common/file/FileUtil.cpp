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
			if (file == nullptr)return false;// �ַ������Ϸ�
			if (strlen(file) == 0)return false;// �ַ������Ϸ�
			if (_access(file, 0) == 0)return true;// �ļ��Ѿ����ڣ�����ִ�д���
			HANDLE hOpenFile = (HANDLE)CreateFileA(file, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, NULL, NULL);
			if (hOpenFile == INVALID_HANDLE_VALUE) {
				hOpenFile = NULL;
				return false;
			}
			return true;
		}

		bool delFile(const char* file) {
			if (file == nullptr)return false;// �ַ������Ϸ�
			if (strlen(file) == 0)return false;// �ַ������Ϸ�
			if (_access(file, 0) != 0)return true;// �ļ������ڣ�����ִ��ɾ��
			DWORD fileAttributes = GetFileAttributesA(file);
			fileAttributes &= ~FILE_ATTRIBUTE_READONLY;
			SetFileAttributesA(file, fileAttributes);
			return 0 != ::DeleteFileA(file);
		}

		bool mkdirs(const char *path) {
			if (nullptr == path) return false;
			char* pszDir = _strdup(path); // �����ַ�����ע�����һ��Ҫ�ͷŴ˿ռ�
			int len = strlen(pszDir);
			if (len == 0)return false;
			int i, iRet;
			// ���ַ�����ǰ��󣬴����༶Ŀ¼  
			for (i = 0; i < len; i++) {
				if (pszDir[i] == '\\' || pszDir[i] == '/') {
					pszDir[i] = '\0';
					iRet = _access(pszDir, 0);// ����ļ����Ƿ����
					if (iRet != 0) {// ���ļ��в�����ʱ���Ž��д���
						iRet = _mkdir(pszDir);
						if (iRet != 0) {// ����ʱ��ʧ��
							free(pszDir);// ��Ҫ�ͷſռ�
							return false;
						}
					}
					pszDir[i] = '/';// �����С�\�����ɡ�/��������windows��linux  
				}
			}
			if (pszDir[i--] != '/') {
				iRet = _access(pszDir, 0);// ����ļ����Ƿ����
				if (iRet != 0)iRet = _mkdir(pszDir);// ����������ļ��ָ�����β����ô��Ҫ����
			}
			free(pszDir);
			return iRet == 0;
		}
		bool createDir(const char* folder) {
			if (folder == nullptr)return false;// �ַ������Ϸ�
			if (_access(folder, 0) == 0)return true;// Ŀ¼�Ѵ��ڣ�Ҳ��Ϊ�����ɹ�
			if (0 != _mkdir(folder)) return false;// ����Ŀ¼ʧ��
			return true;
		}

		// ������Ƿ�null���Ƿ����ļ�
		void ___listFiles(const wchar_t * dir, FeedFile feedFile, int level, void* feedfile_ctx) {
			int size = wcslen(dir) + 4 /* 4=strlen("\\*.*") */ + 1;
			wchar_t * dirNew = new wchar_t[size];// C++��ʽ����delete���			
			wcscpy_s(dirNew, size, dir);
			wcscat_s(dirNew, size, L"\\*.*");    // ��Ŀ¼�������"\\*.*"���е�һ������

			intptr_t handle;
			_wfinddata_t findData;

			handle = _wfindfirst(dirNew, &findData);
			if (handle == -1) return;// ����Ƿ�ɹ�
			level++;
			do {
				if (findData.attrib & _A_SUBDIR) {
					if (wcscmp(findData.name, L".") == 0 || wcscmp(findData.name, L"..") == 0) continue;
					feedFile(feedfile_ctx,dir, findData.name,true, level);

					// ��Ŀ¼�������"\\"����������Ŀ¼��������һ������
					int size = wcslen(dir) + 2 /* 2=strlen("\\") */ + wcslen(findData.name) + 1;
					wchar_t * nestdir = new wchar_t[size];// C++��ʽ����delete���		
					wcscpy_s(nestdir, size, dir);
					wcscat_s(nestdir, size,L"\\");    // ��Ŀ¼�������"\\*.*"���е�һ������
					wcscat_s(nestdir, size, findData.name);
					___listFiles(nestdir, feedFile, level, feedfile_ctx);
					//writeLog("FileUtil_____listFiles_file");writeLog("dir"); writeLog("\r\n");
				} else {
					//writeLog("FileUtil_____listFiles_file");writeLog("dir"); writeLog("\r\n");					
					feedFile(feedfile_ctx, dir, findData.name, false, level);
				}
			} while (_wfindnext(handle, &findData) == 0);

			_findclose(handle);    // �ر��������
		}

		void listFiles(const wchar_t * dir, FeedFile feedFile, void* feedfile_ctx) {
			if (dir == nullptr)return;// �����ݼ��
			if (_waccess(dir, 0) != 0)return;// ����ļ���Ŀ¼�Ƿ���ڣ���������ֱ�ӷ���
			if (GetFileAttributesW(dir)&FILE_ATTRIBUTE_DIRECTORY) {
				___listFiles(dir, feedFile, 0, feedfile_ctx);// ��Ŀ¼������ݹ麯�����м��
			} else {
				feedFile(feedfile_ctx, L"", const_cast<wchar_t*>(dir),false, 0);
			}
		}
	}
}