#include "Task.h"

#include <sstream>
namespace jw{
	namespace task{
		using namespace std;
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

		long count = 0;// 
		// 创建一个任务id（最好改为UUID）
		wstring createTaskId(){
			mapLock.lock();
			wstringstream wss;
			count++;
			wss << "id_" << count;
			mapLock.unlock();
			return wss.str();
		}

		// 添加任务执行结果
		bool putTaskResult(wstring taskId, wstring taskResult){
			ProcessMsgLock * lock = getTask(taskId);
			if (lock == NULL)return false;
			lock->result = taskResult;
			lock->notify();
			return true;
		}

		// 添加任务
		ProcessMsgLock * addTask(wstring taskId){
			ProcessMsgLock * lock = new ProcessMsgLock();
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
	}
}