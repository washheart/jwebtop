#include "Client.h"
#include "common/util/StrUtil.h"
namespace fastipc{
	Client::Client(void){
		evtWrited = NULL;
		evtReaded = NULL;
		mapFile = NULL;
		memBuf = NULL;
	};
	Client::~Client(void){
		close();
	};
	void Client::close(void){
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
	bool Client::isStable(){
		return memBuf != NULL;
	};
	int	Client::create(const std::wstring serverName, DWORD blockSize){
		// 创建两个事件分别用于通知可读、可写
		evtWrited = OpenEvent(EVENT_ALL_ACCESS, FALSE, LPTSTR(genWritedEventName(serverName).c_str()));
		if (evtWrited == NULL || evtWrited == INVALID_HANDLE_VALUE)  return ERR_EventCreate_W;
		evtReaded = OpenEvent(EVENT_ALL_ACCESS, FALSE, LPTSTR(genReadedEventName(serverName).c_str()));
		if (evtReaded == NULL || evtReaded == INVALID_HANDLE_VALUE) return ERR_EventCreate_R;
		// 打开内存映射文件
		mapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, (genMappingFileName(serverName).c_str()));
		if (mapFile == NULL || mapFile == INVALID_HANDLE_VALUE)return ERR_MappingOpen;
		DWORD size = sizeof(MemBuff)+sizeof(char)*blockSize;// 动态加长blockSize个字节
		memBuf = (MemBuff*)MapViewOfFile(mapFile, FILE_MAP_ALL_ACCESS, 0, 0, size);
		if (memBuf == NULL) { return ERR_MappingMap; }
		this->blockSize = blockSize;
		return 0;
	};

	DWORD Client::write(char *pBuff, DWORD len){
		if (!memBuf)return ERR_ClientCreate;
		if (len <= blockSize)return writeBlock(pBuff, len, NULL, MSG_TYPE_NORMAL); // 可以一次性写完
		DWORD idx = 0, tmp = len%blockSize;
		DWORD result = -1;
		len = len - tmp;
		char * id = jw::GenerateGuid();
		len = len - blockSize;// 多减一次，避免在while循环内判断是否是最后的数据包
		while (idx < len){// 将数据分为多个包来写
			result = writeBlock(pBuff + idx, blockSize, id, MSG_TYPE_PART);
			if (result != 0)return result;
			idx += blockSize;
		}
		if (tmp == 0){// 正好被分为多个完整的数据包
			result = writeBlock(pBuff + len, blockSize, id, MSG_TYPE_END); // 发送最后一个包，以及结束标记
		}
		else{
			result = writeBlock(pBuff + len, blockSize, id, MSG_TYPE_PART); // 发送倒数第二个完整包，以及继续标记
			result = writeBlock(pBuff + len + blockSize, tmp, id, MSG_TYPE_END); //发送剩余的包，以及结束标记
		}
		delete id;
		return 0;
	}

	DWORD Client::writeBlock(char *pBuff, DWORD len, char* packId, int msgType){
	writeAble:
		if (memBuf->state == MEM_CAN_WRITE){
			InterlockedCompareExchange(&memBuf->state, MEM_IS_BUSY, MEM_CAN_WRITE);// 通过原子操作来设置共享区的状态为忙碌状态
			if (memBuf->state != MEM_IS_BUSY)goto waitForWrite;	// 如果设置后不是忙碌状态，那么可能有其他线程在操作数据，此时继续等待 
			memBuf->dataLen = len;
			memBuf->msgType = msgType;
			memcpy(memBuf->data, pBuff, len);
			if (msgType > MSG_TYPE_NORMAL){
				ZeroMemory(memBuf->packId, PACK_ID_LEN);
				memcpy(memBuf->packId, packId, min(PACK_ID_LEN, lstrlenA(packId)));
			}
			InterlockedExchange(&memBuf->state, MEM_CAN_READ);	// 设置内存状态为可读
			SetEvent(evtWrited);								// 发送事件，通知等待线程：可以读了
		}
		else{
		waitForWrite:
			if (WaitForSingleObject(evtReaded, INFINITE) == WAIT_OBJECT_0){
				goto writeAble;// 等待可写事件的到来，然后去写
			}
			else{
				return ERR_EventWait_W;
			}
		}
		return 0;
	};
}