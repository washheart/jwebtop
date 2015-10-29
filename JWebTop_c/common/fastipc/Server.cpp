#include "Server.h"

namespace fastipc{
	Server::Server(){
		evtWrited = NULL;
		evtReaded = NULL;
		mapFile = NULL;
		memBuf = NULL;
	};

	Server::~Server(void)	{
		close();
	};

	void Server::close(void){
		if (evtWrited) {// 关闭事件句柄
			HANDLE handle = evtWrited;
			evtWrited = NULL;
			CloseHandle(handle);
		}
		if (evtReaded) {// 关闭事件句柄
			HANDLE handle = evtReaded;
			evtReaded = NULL;
			CloseHandle(handle);
		}
		if (memBuf) {// 取消内存映射
			MemBuff *pBuff = memBuf;
			memBuf = NULL;
			UnmapViewOfFile(pBuff);
		}
		if (mapFile) {// 关闭映射文件
			HANDLE handle = mapFile;
			mapFile = NULL;
			CloseHandle(handle);
		}
	};
	void Server::setListener(ReadListener* listener){
		this->listener = listener;
	};
	bool Server::isStable(){
		return memBuf != NULL;
	};
	int Server::create(const std::wstring serverName, DWORD blockSize){
		// 创建两个事件分别用于通知可读、可写
		evtWrited = CreateEvent(NULL, FALSE, FALSE, LPTSTR(genWritedEventName(serverName).c_str()));
		if (evtWrited == NULL || evtWrited == INVALID_HANDLE_VALUE)  return ERR_EventOpen_W;
		evtReaded = CreateEvent(NULL, FALSE, FALSE, LPTSTR(genReadedEventName(serverName).c_str()));
		if (evtReaded == NULL || evtReaded == INVALID_HANDLE_VALUE) return ERR_EventOpen_R;

		// 创建内存映射文件
		DWORD size = sizeof(MemBuff)+sizeof(char)*blockSize;// 动态加长blockSize个字节
		mapFile = CreateFileMapping(INVALID_HANDLE_VALUE,	// 创建一个与物理文件无关的映射文件
			NULL,											// 安全设置
			PAGE_READWRITE,									// 打开方式
			0,												// 文件映射的最大长度的高32位。
			size,											// 文件映射的最大长度的低32位。
			LPTSTR(genMappingFileName(serverName).c_str()));// 映射文件的名称
		if (mapFile == NULL || mapFile == INVALID_HANDLE_VALUE)  return ERR_MappingCreate;
		// 映射文件到内存
		memBuf = (MemBuff*)MapViewOfFile(mapFile,			// 映射文件句柄
			FILE_MAP_ALL_ACCESS,							// 读写权限
			0, 0,											// 映射起始偏移的高、低32位
			size);											// 指定映射文件的字节数
		if (memBuf == NULL)return ERR_MappingMap;			// 映射文件失败
		ZeroMemory(memBuf, size);							// 清空缓冲区
		return 0;
	};


	void Server::startRead(){
		// 读取为单线程读取，所以不需要在设置state的前后进行状态位的检查
		//if (memBuf->state != MEM_CAN_READ) continue;  // 当前共享区不是可读状态，可能正在使用，继续等待
		//if (memBuf->state != MEM_IS_BUSY)continue;  // 如果设置后还是空闲状态，那么可能是被其他进程把数据读走了，此时继续等待 
		while (memBuf){
			DWORD dwWaitResult = WaitForSingleObject(evtWrited, INFINITE);// 等待写完事件
			if (dwWaitResult == WAIT_OBJECT_0){
				InterlockedCompareExchange(&memBuf->state, MEM_IS_BUSY, MEM_CAN_READ);// 通过原子操作来设置共享区的状态为读状态
					MemBlock * rtn=NULL;
					try{
						rtn = new MemBlock();
						rtn->dataLen = memBuf->dataLen;
						rtn->msgType = memBuf->msgType;
						rtn->data = (char *)malloc(memBuf->dataLen);
						memcpy(rtn->data, memBuf->data, rtn->dataLen);
						if (memBuf->msgType > MSG_TYPE_NORMAL){
							ZeroMemory(rtn->packId, PACK_ID_LEN);
							memcpy(rtn->packId, memBuf->packId, PACK_ID_LEN);
						}
						listener->onRead(rtn);
						delete rtn;// 清理环境
					}
					catch (...){
						delete rtn;// 清理环境
					}
				InterlockedExchange(&memBuf->state, MEM_CAN_WRITE);// 数据读取之后，设置为可写
				SetEvent(evtReaded);
			}
			else{
				break;// 出错后结束循环操作
			}
		}
	};
}