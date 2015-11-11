#include "Task.h"
#include <sstream>
#include "common/util/StrUtil.h"
#include "common/tests/TestUtil.h"


using namespace std;
namespace jw{
	namespace task{
		DWORD g_defaultWaitTime = 0;
		void setDefaultTaskWiatTime(DWORD defaultWaitTime){
			g_defaultWaitTime = defaultWaitTime;
		}
		wstring ProcessMsgLock::wait(){
			return wait(g_defaultWaitTime);
		}
		wstring ProcessMsgLock::wait(DWORD microseconds){
			if (!notified){						// 如果已经通知过，直接返回通知的结果
				unique_lock<mutex> locker(lock);// 建立锁并锁上（析构时自动解锁）
				if (!notified){					// 上锁后再检查下状态位，避免early notify
					if (microseconds > 0){		// 如果有指定超时时间，那么使用超时等待方式
						g_queuecheck.wait_until(locker, chrono::system_clock::now() + chrono::microseconds(microseconds));
					}
					else{						// 否则一直等待，直到解锁notify的到来
						g_queuecheck.wait(locker);
					}
				}
			}
			removeTask(this->taskId);			// 移除保存的任务锁
			return this->result;
		}
		void ProcessMsgLock::notify(wstring result){
			this->result = result;				// 设置任务锁的执行结果
			notified = true;					// 上锁之前先把标志位改了，因为此时结果已设置
			unique_lock<mutex> locker(lock);	// 建立锁并锁上（析构时自动解锁）
			g_queuecheck.notify_all();			// 通知所有等待锁：任务完成了
		}

		WaitTaskMap WaitTasks; // 保存所有已创建的任务信息	
		mutex mapLock;         // 所有针对map的操作都要锁定 
		ProcessMsgLock * getTask(wstring taskId){
			ProcessMsgLock * rtn = NULL;
			mapLock.lock();
			try{
				WaitTaskMap::iterator it = WaitTasks.find(taskId);
				if (WaitTasks.end() != it) {
					//it->second->wait();// find时无需等待
					rtn = it->second;
				}
				mapLock.unlock();
			}
			catch (...){
				mapLock.unlock();
			}
			return rtn;
		}

		// 创建一个任务id（最好改为UUID）
		wstring createTaskId(){
			return jw::GenerateGuidW();
		}

		// 添加任务执行结果
		bool putTaskResult(wstring taskId, wstring taskResult){
			ProcessMsgLock * lock = getTask(taskId);
			if (lock == NULL)return false;
			lock->notify(taskResult);
			return true;
		}

		// 添加任务
		ProcessMsgLock * addTask(wstring taskId){
			ProcessMsgLock * lock = new ProcessMsgLock(taskId);
			mapLock.lock();
			try{
				pair<WaitTaskMap::iterator, bool> v = WaitTasks.insert(pair<wstring, ProcessMsgLock * >(taskId, lock));// 记录下任务id与锁的关联
				mapLock.unlock();
			}
			catch (...){
				mapLock.unlock();
				lock = NULL;
			}
			return  lock;
		}

		// 移除任务
		void removeTask(wstring taskId){
			mapLock.lock();
			try{
				WaitTasks.erase(taskId);
				mapLock.unlock();
			}
			catch (...){
				mapLock.unlock();
			}
		}

		// 把所有锁都给解锁：此方法当且仅当在进程退出时执行
		void unlockAndClearAll(){
#ifdef JWebTopLog
			wstringstream wss;
			wss << L"所有未解的锁数量=";
			wss << WaitTasks.size();
			wss << L"\r\n";
			writeLog(wss.str());
#endif
			WaitTaskMap::iterator it = WaitTasks.begin();
			while (WaitTasks.end() != it) {
				try{// 通过try-catch保证每次解锁失败不影响其他的解锁
#ifdef JWebTopLog
					wss.clear();
					wss << L"\t"
						<< " taskId=" << it->first << "\r\n";
					writeLog(wss.str());
#endif
					it->second->notify(L"");
				}
				catch (...){}
				++it;
			}
			WaitTasks.clear();
		}
	}
}