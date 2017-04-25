//////////////////////////////////////////////////////////////////////////  
/// 定义fastipc的服务器
/// 服务端是只侦听来的客户端的写消息。
/// 如果需要双向通信，那么服务端(S1)对应的客户端(C1)需要创建一个服务端（S2），然后服务端(S1)创建一个客户端(C2)进行写。
/// 即S1侦听C1的写事件，S2侦听C2的写事件。
/// 服务端用法和流程如下：create、serListerner、startRead、close。
///  
/// 源码：http://git.oschina.net/washheart/fastipc
/// 源码：https://github.com/washheart/fastipc
/// 说明：https://github.com/washheart/fastipc/wiki
/// @version	1.0 
/// @author		washheart@163.com
/// @date       2015-10-22  
////////////////////////////////////////////////////////////////////////// 
#pragma once
#ifndef FastIPC_Server_H
#define FastIPC_Server_H

#include <Windows.h>
#include <map>
#include "FastIPC.h"

namespace fastipc{
	// 侦听并处理【原始】消息
	class _declspec(dllexport) ReadListener {
	public:
		// 当有服务端读取到数据后，会调用此方法通知等待者进行处理
		// memBlock在分发后会由服务器销毁，外部调用者无需清理操作
		virtual void onRead(MemBlock* memBlock) {}
	};

	class RebuildedBlockListener_impl;// 这个类作为中转，目的是避免导出STL类，同时还隐藏数据封装的实现
	// 实现onRead对【原始】消息进行组装，继承此类后只需要实现onRebuildedRead就可以了
	class _declspec(dllexport) RebuildedBlockListener :public ReadListener {
	private:
		RebuildedBlockListener_impl * impl;
		// 当有服务端读取到数据后，会调用此方法通知等待者进行处理
		// memBlock在分发后会由服务器销毁，外部调用者无需清理操作
		void onRead(MemBlock* readed);
	public:
		RebuildedBlockListener();
		virtual void onRebuildedRead(MemBlock* memBlock) {};
	};

	class  _declspec(dllexport) Server{
	public:
		Server();
		~Server();

	private:
		HANDLE			evtWrited;	// 定义一个事件：写操作已完成，可以读了
		HANDLE			evtReaded;	// 定义一个事件：读操作已完成，可以写了
		HANDLE			mapFile;	// 内存映射文件句柄
		MemBuff*		memBuf;		// 共享在内存的数据结构
		//DWORD			blockSize;	// 设置memBuf.data的空间长度
		ReadListener*	listener;	// 侦听读事件，并进行处理
	public:
		/// 创建服务器
		/// @param serverName 服务器名称
		/// @param blockSize 设置memBuf.data的空间长度，默认是2048，即1024个汉字
		/// @return 0=成功；其他值表示失败，具体常量参见FastIPC.h中的ERR_*常量
		int	create(const std::wstring serverName, DWORD blockSize);

		/// 关闭服务器
		void close(void);

		/// 为服务器设置一个侦听器，以便读取数据并进行处理
		/// @param listener 一个侦听器
		void setListener(ReadListener* listener);

		/// 开始读，数据成功读取后，会回调listener.onRead。
		/// 注意此方法会阻塞当前线程。
		void startRead();

		/// 服务器状态是否已准备好
		/// @return true=准备好
		bool isStable();
	};
}
#endif