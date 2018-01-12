#include "Client.h"
#include <algorithm>
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
		if (evtWrited) {// �ر��¼����
			HANDLE handle = evtWrited;
			evtWrited = NULL;
			CloseHandle(handle);
		}
		if (evtReaded) {// �ر��¼����
			HANDLE handle = evtReaded;
			evtReaded = NULL;
			CloseHandle(handle);
		}
		if (memBuf) {// ȡ���ڴ�ӳ��
			MemBuff *pBuff = memBuf;
			memBuf = NULL;
			UnmapViewOfFile(pBuff);
		}
		if (mapFile) {// �ر�ӳ���ļ�
			HANDLE handle = mapFile;
			mapFile = NULL;
			CloseHandle(handle);
		}
	};
	bool Client::isStable(){
		return memBuf != NULL;
	};
	int	Client::create(const std::wstring serverName, DWORD blockSize){
		// ���������¼��ֱ�����֪ͨ�ɶ�����д
		evtWrited = OpenEvent(EVENT_ALL_ACCESS, FALSE, LPTSTR(genWritedEventName(serverName).c_str()));
		if (evtWrited == NULL || evtWrited == INVALID_HANDLE_VALUE)  return ERR_EventCreate_W;
		evtReaded = OpenEvent(EVENT_ALL_ACCESS, FALSE, LPTSTR(genReadedEventName(serverName).c_str()));
		if (evtReaded == NULL || evtReaded == INVALID_HANDLE_VALUE) return ERR_EventCreate_R;
		// ���ڴ�ӳ���ļ�
		mapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, (genMappingFileName(serverName).c_str()));
		if (mapFile == NULL || mapFile == INVALID_HANDLE_VALUE)return ERR_MappingOpen;
		DWORD size = sizeof(MemBuff)+sizeof(char)*blockSize;// ��̬�ӳ�blockSize���ֽ�
		memBuf = (MemBuff*)MapViewOfFile(mapFile, FILE_MAP_ALL_ACCESS, 0, 0, size);
		if (memBuf == NULL) { return ERR_MappingMap; }
		this->blockSize = blockSize;
		return 0;
	};

	DWORD Client::write(LONG userMsgType, LONG userValue, char* userShortStr, char *pBuff, DWORD len){
		if (!memBuf)return ERR_ClientCreate;
		if (len <= blockSize)return writeBlock(userMsgType, userValue, userShortStr, pBuff, len, NULL, MSG_TYPE_NORMAL); // ����һ����д��
		DWORD idx = 0, tmp = len%blockSize;
		DWORD result = (DWORD)-1;
		len = len - tmp;
		char * id = jw::str::GenerateGuid();
		len = len - blockSize;// ���һ�Σ�������whileѭ�����ж��Ƿ����������ݰ�
		while (idx < len){// �����ݷ�Ϊ�������д
			result = writeBlock(userMsgType, userValue, userShortStr, pBuff + idx, blockSize, id, MSG_TYPE_PART);
			if (result != 0)return result;
			idx += blockSize;
		}
		if (tmp == 0){// ���ñ���Ϊ������������ݰ�
			result = writeBlock(userMsgType, userValue, userShortStr, pBuff + len, blockSize, id, MSG_TYPE_END); // �������һ�������Լ��������
		}
		else{
			result = writeBlock(userMsgType, userValue, userShortStr, pBuff + len, blockSize, id, MSG_TYPE_PART); // ���͵����ڶ������������Լ��������
			result = writeBlock(userMsgType, userValue, userShortStr, pBuff + len + blockSize, tmp, id, MSG_TYPE_END); //����ʣ��İ����Լ��������
		}
		delete id;
		return 0;
	}

	DWORD Client::writeBlock(LONG userMsgType, LONG userValue, char* userShortStr, char *pBuff, DWORD len, char* packId, int msgType){
	writeAble:
		if (memBuf->state == MEM_CAN_WRITE){
			InterlockedCompareExchange(&memBuf->state, MEM_IS_BUSY, MEM_CAN_WRITE);// ͨ��ԭ�Ӳ��������ù�������״̬Ϊæµ״̬
			if (memBuf->state != MEM_IS_BUSY)goto waitForWrite;	// ������ú���æµ״̬����ô�����������߳��ڲ������ݣ���ʱ�����ȴ� 
			memBuf->dataLen = len;
			memBuf->msgType = msgType;
			memBuf->userMsgType = userMsgType;
			memBuf->userValue = userValue;
			memcpy(memBuf->data, pBuff, len);
			int taskIdLen = lstrlenA(userShortStr);
			if (taskIdLen > 0){
				ZeroMemory(memBuf->userShortStr, PACK_ID_LEN);
				memcpy(memBuf->userShortStr, userShortStr, min(PACK_ID_LEN, taskIdLen));
			}
			if (msgType > MSG_TYPE_NORMAL){
				ZeroMemory(memBuf->packId, PACK_ID_LEN);
				memcpy(memBuf->packId, packId, min(PACK_ID_LEN, lstrlenA(packId)));
			}
			InterlockedExchange(&memBuf->state, MEM_CAN_READ);	// �����ڴ�״̬Ϊ�ɶ�
			SetEvent(evtWrited);								// �����¼���֪ͨ�ȴ��̣߳����Զ���
		}
		else{
		waitForWrite:
			if (WaitForSingleObject(evtReaded, INFINITE) == WAIT_OBJECT_0){
				goto writeAble;// �ȴ���д�¼��ĵ�����Ȼ��ȥд
			}
			else{
				return ERR_EventWait_W;
			}
		}
		return 0;
	};
}