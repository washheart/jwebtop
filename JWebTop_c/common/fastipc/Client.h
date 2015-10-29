//////////////////////////////////////////////////////////////////////////  
/// 定义fastipc的客户端
/// 客户端是只能写数据给服务端。具体使用参见[Server.h]。
///  
/// @version	1.0 
/// @author		washheart@163.com
/// @date       2015-10-22  
////////////////////////////////////////////////////////////////////////// 
#pragma once

#ifndef FastIPC_Client_H
#define FastIPC_Client_H

#include <Windows.h>
#include "FastIPC.h"

namespace fastipc{
	class _declspec(dllexport)  Client{
	public:
		Client(void);
		~Client();
	private:
		HANDLE		evtWrited;		// 定义一个事件：写操作已完成，可以读了
		HANDLE		evtReaded;		// 定义一个事件：读操作已完成，可以写了
		HANDLE		mapFile;		// 内存映射文件句柄
		MemBuff*	memBuf;			// 共享在内存的数据结构
		DWORD		blockSize = 2;	// 设置memBuf.data的空间长度

	public:
		/// 创建服务器
		/// @param serverName 服务器名称
		/// @param blockSize 设置memBuf.data的空间长度，默认是2048，即1024个汉字
		/// @return 0=成功；其他值表示失败，具体常量参见FastIPC.h中的ERR_*常量
		int	create(const std::wstring, DWORD blockSize);

		/// 关闭客户端
		void close(void);

		/// 写数据到共享内存，当数据的长度大于MEM_SIZE时会拆分为多个MemBuff来发送
		/// @param pBuff 待发送的数据
		/// @param len	 需要发送的长度
		/// @return 0=成功；其他值表示失败，具体常量参见FastIPC.h中的ERR_*常量
		DWORD write(char *pBuff, DWORD len);		// 

		/// 客户端状态是否已准备好
		/// @return true=准备好
		bool isStable();

	private:
		/// 写数据到共享内存
		/// @param pBuff	待发送的数据
		/// @param len		需要发送的长度，len是小于等于MEM_SIZE的
		/// @param packId	当发送大数据时，根据此id将多个MemBuff组合在一起
		/// @param msgType	发送的数据的类型
		/// @return 0=成功；其他值表示失败，具体常量参见FastIPC.h中的ERR_*常量
		DWORD Client::writeBlock(char *pBuff, DWORD len, char* packId, int msgType);
	};
}
#endif