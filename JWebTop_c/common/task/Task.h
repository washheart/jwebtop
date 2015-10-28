#ifndef jwebtop_task_H_
#define jwebtop_task_H_

#include <string>
#include <thread>
#include <map>
#include <mutex>
#include <condition_variable>

using namespace std;
namespace jw{
	namespace task{

		// 锁、任务执行结果包装类
		class ProcessMsgLock
		{
		private:
			mutex lock;
			condition_variable g_queuecheck;
			bool notified = false;// 用来标记是否已通知过（防止解锁通知在等待之前就发来，也为了在对象消耗时进行解锁）
			wstring result;
		public:
			ProcessMsgLock(wstring taskId){ this->taskId = taskId; }
			~ProcessMsgLock(){ if (!notified)notify(L""); };
			wstring wait();
			void notify(wstring result);
		};

		// map<key,value>,key=已成功发送到远程的任务的id，value=任务执行完成后放置任务执行结果
		typedef map<wstring, ProcessMsgLock * > WaitTaskMap;

		// 生成任务的id
		wstring createTaskId();

		// 添加一个等待任务到任务列表
		ProcessMsgLock * addTask(wstring taskId);

		// 移除已完成的任务
		void removeTask(wstring taskId);

		// 将任务执行结果保存进来，保存后会调用notify通知等待的程序：一般是在另外一个线程中调用
		bool putTaskResult(wstring taskId, wstring taskResult);

		// 把所有锁都给解锁：此方法当且仅当在进程退出时执行
		void unlockAndClearAll();
	}
}
#endif