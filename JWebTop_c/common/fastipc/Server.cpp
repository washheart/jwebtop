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
		listener = NULL;
		if (evtWrited) {// 关闭事件句柄
			HANDLE handle = evtWrited;
			evtWrited = NULL;
			SetEvent(handle);
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
		while (evtWrited && (WaitForSingleObject(evtWrited, INFINITE) == WAIT_OBJECT_0)){// 等待写完事件
			if (!evtWrited)break;
			// 读取为单线程读取，所以不需要在设置state的前后进行状态位的检查
			//if (memBuf->state != MEM_CAN_READ) continue;  // 当前共享区不是可读状态，可能正在使用，继续等待
			//if (memBuf->state != MEM_IS_BUSY)continue;  // 如果设置后还是空闲状态，那么可能是被其他进程把数据读走了，此时继续等待 
			InterlockedCompareExchange(&memBuf->state, MEM_IS_BUSY, MEM_CAN_READ);// 通过原子操作来设置共享区的状态为读状态
			MemBlock * rtn = NULL;
			try{
				rtn = new MemBlock();
				rtn->dataLen = memBuf->dataLen;
				rtn->msgType = memBuf->msgType;
				rtn->userMsgType = memBuf->userMsgType;
				rtn->userValue = memBuf->userValue;
				rtn->data = (char *)malloc(memBuf->dataLen);
				memcpy(rtn->data, memBuf->data, rtn->dataLen);
				int len = lstrlenA(memBuf->userShortStr);
				ZeroMemory(rtn->userShortStr, PACK_ID_LEN);
				if (len > 0){
					memcpy(rtn->userShortStr, memBuf->userShortStr, len);
				}
				if (memBuf->msgType > MSG_TYPE_NORMAL){
					ZeroMemory(rtn->packId, PACK_ID_LEN);
					memcpy(rtn->packId, memBuf->packId, PACK_ID_LEN);
				}
				listener->onRead(rtn);
				delete rtn;// 清理环境
			} catch (...){
				delete rtn;// 清理环境
			}
			// 多线程情况下，可能会出现server.close被调用后，还有线程在onRead，所以这里加个空指针检查
			if (memBuf){
				InterlockedExchange(&memBuf->state, MEM_CAN_WRITE);// 数据读取之后，设置为可写
			}else{
				close();// 检查下是否有未关闭的资源
			}
			if (evtReaded){
				SetEvent(evtReaded);
			}else{
				close();// 检查下是否有未关闭的资源
			}
		}
	};

	// 数据组装实现
	class  RebuildedBlockListener_impl :public ReadListener {
	private:
		typedef std::map<std::string, MemBlock*> RebuildedBlockMap;// 定义一个存储BrowserWindowInfo的Map
		RebuildedBlockMap rebuildBlocks;
		MemBlock * getRebuildedBlock(std::string packId) {
			RebuildedBlockMap::iterator it = rebuildBlocks.find(packId);
			if (rebuildBlocks.end() != it) {
				return it->second;
			}
			return NULL;
		}
		RebuildedBlockListener* callback;
	public:
		RebuildedBlockListener_impl(RebuildedBlockListener* callback) {
			this->callback = callback;
		}

		// 当有服务端读取到数据后，会调用此方法通知等待者进行处理
		// memBlock在分发后会由服务器销毁，外部调用者无需清理操作
		void onRead(MemBlock* readed) {
			if (readed->msgType == MSG_TYPE_NORMAL) {
				callback->onRebuildedRead(readed);// 普通消息，直接转发
			} else {// 获取重组数据用的uuid（由于发送端可能是多线程交错发送，所以这里用map来存id和数据块的关系）
				char * uid;
				uid = (char *)malloc(PACK_ID_LEN + 1);
				memcpy(uid, readed->packId, PACK_ID_LEN);
				uid[PACK_ID_LEN] = '\0';
				std::string packId(uid);
				MemBlock * tmpBlock = getRebuildedBlock(packId);
				try {
					if (!tmpBlock) {
						tmpBlock = new MemBlock();
						tmpBlock->msgType = MSG_TYPE_NORMAL;
						tmpBlock->dataLen = 0;
						tmpBlock->userMsgType = readed->userMsgType;
						tmpBlock->userValue = readed->userValue;
						int len = lstrlenA(readed->userShortStr);
						ZeroMemory(tmpBlock->userShortStr, PACK_ID_LEN);
						if (len > 0) {
							memcpy(tmpBlock->userShortStr, readed->userShortStr, len);
						}
						rebuildBlocks.insert(std::pair<std::string, MemBlock*>(packId, tmpBlock));
					}
					int len = readed->dataLen;
					tmpBlock->data = (char*)realloc(tmpBlock->data, tmpBlock->dataLen + len);
					memcpy((tmpBlock->data + tmpBlock->dataLen), readed->data, len);// 追加拷贝数据
					tmpBlock->dataLen = tmpBlock->dataLen + len;
					if (readed->msgType == MSG_TYPE_END) {
						rebuildBlocks.erase(packId);// 从map中移除
						callback->onRebuildedRead(tmpBlock);// 重组完成，转发
						delete tmpBlock;// 清理环境
					}
				} catch (...) {
					delete tmpBlock;// 清理环境
				}
			}
		}

	};
	RebuildedBlockListener::RebuildedBlockListener() {
		this->impl = new RebuildedBlockListener_impl(this);
	};
	void  RebuildedBlockListener::onRead(MemBlock* readed) { impl->onRead(readed); };
}