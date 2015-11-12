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
	class _declspec(dllexport) ReadListener{
	public:
		// 当有服务端读取到数据后，会调用此方法通知等待者进行处理
		// memBlock在分发后会由服务器销毁，外部调用者无需清理操作
		virtual void onRead(MemBlock* memBlock){}
	};

	template  class __declspec(dllexport) std::map < std::string, MemBlock* >;
	// 已经将数据进行了组装，不用考虑其他的了
	class _declspec(dllexport) RebuildedBlockListener :public ReadListener{
	private:
		typedef std::map<std::string, MemBlock*> RebuildedBlockMap;// 定义一个存储BrowserWindowInfo的Map
		RebuildedBlockMap rebuildBlocks;
		MemBlock * getRebuildedBlock(std::string packId){
			RebuildedBlockMap::iterator it = rebuildBlocks.find(packId);
			if (rebuildBlocks.end() != it) {
				return it->second;
			}
			return NULL;
		}
	public:
		virtual void onRebuildedRead(MemBlock* memBlock){}
		//private:
		// 当有服务端读取到数据后，会调用此方法通知等待者进行处理
		// memBlock在分发后会由服务器销毁，外部调用者无需清理操作
		void onRead(MemBlock* readed){
			if (readed->msgType == MSG_TYPE_NORMAL){
				onRebuildedRead(readed);// 普通消息，直接转发
			}
			else{// 获取重组数据用的uuid（由于发送端可能是多线程交错发送，所以这里用map来存id和数据块的关系）
				char * uid;
				uid = (char *)malloc(PACK_ID_LEN + 1);
				memcpy(uid, readed->packId, PACK_ID_LEN);
				uid[PACK_ID_LEN] = '\0';
				std::string packId(uid);
				MemBlock * tmpBlock = getRebuildedBlock(packId);
				try{
					if (!tmpBlock){
						tmpBlock = new MemBlock();
						tmpBlock->msgType = MSG_TYPE_NORMAL;
						tmpBlock->dataLen = 0;
						tmpBlock->userMsgType = readed->userMsgType;
						tmpBlock->userValue = readed->userValue;
						int len = lstrlenA(readed->userShortStr);
						ZeroMemory(tmpBlock->userShortStr, PACK_ID_LEN);
						if (len > 0){
							memcpy(tmpBlock->userShortStr, readed->userShortStr, len);
						}
						rebuildBlocks.insert(std::pair<std::string, MemBlock*>(packId, tmpBlock));
					}
					int len = readed->dataLen;
					tmpBlock->data = (char*)realloc(tmpBlock->data, tmpBlock->dataLen + len);
					memcpy((tmpBlock->data + tmpBlock->dataLen), readed->data, len);// 追加拷贝数据
					tmpBlock->dataLen = tmpBlock->dataLen + len;
					if (readed->msgType == MSG_TYPE_END){
						rebuildBlocks.erase(packId);// 从map中移除
						onRebuildedRead(tmpBlock);// 重组完成，转发
						delete tmpBlock;// 清理环境
					}
				}
				catch (...){
					delete tmpBlock;// 清理环境
				}
			}
		}

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