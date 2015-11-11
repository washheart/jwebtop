package org.jwebtop;

import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

/**
 * 通过锁实现多线程任务的等待/通知功能，使其看上去像是单进程<br>
 * 源码：https://github.com/washheart/jwebtop<br>
 * 说明：https://github.com/washheart/jwebtop/wiki<br>
 * 
 * @author washheart@163.com
 */
public class TaskContainer {

	// 锁、任务执行结果包装类
	public static class ProcessMsgLock {
		boolean notified = false; // 用来标记是否已通知过（防止解锁通知在等待之前就发来，也为了在对象消耗时进行解锁）
		String result; // 任务的执行结果（可能是js的执行结果，也可能是其他线程任务等）
		String taskId; // 任务的唯一标识，这里采用uuid

		private ProcessMsgLock(String taskId) {
			this.taskId = taskId;
		}

		@Override
		protected void finalize() throws Throwable {
			if (!notified) notify("");
		}

		// 等待执行结果
		public String waitResult(TaskContainer tc) {
			return waitResult(tc, tc.defaultWaitTime);
		}

		// 等待执行结果
		public String waitResult(TaskContainer tc, long waitTime) {
			if (!notified) {
				try {
					synchronized (this) {
						if (!notified) this.wait(waitTime);// 进入同步块后也检查一下，避免eraly notify现象
					}
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
			tc.removeTask(this.taskId);
			return this.result;
		}

		// 结果取回后进行通知
		public void notify(String result) {
			this.result = result;
			notified = true;
			synchronized (this) {
				this.notifyAll();
			}
		}
	};

	// map<key,value>,key=已成功发送到远程的任务的id，value=任务执行完成后放置任务执行结果
	private final Map<String, ProcessMsgLock> WaitTasks = new HashMap<String, TaskContainer.ProcessMsgLock>();
	private final Object mapLock = new Object(); // 所有针对map的操作都要锁定

	// 生成任务的id
	private long tid = 0;
	private long defaultWaitTime;

	public String createTaskId() {
		// return UUID.randomUUID().toString();
		return "J_J_" + (++tid);
	}

	// 添加一个等待任务到任务列表
	public ProcessMsgLock addTask(String taskId) {
		ProcessMsgLock lock = new ProcessMsgLock(taskId);
		synchronized (mapLock) {
			try {
				WaitTasks.put(taskId, lock);// 记录下任务id与锁的关联
			} catch (Throwable e) {
				e.printStackTrace();
			}
		}
		return lock;
	}

	// 移除已完成的任务
	public void removeTask(String taskId) {
		synchronized (mapLock) {
			try {
				WaitTasks.remove(taskId);// 移除某任务
			} catch (Throwable e) {
				e.printStackTrace();
			}
		}
	}

	private ProcessMsgLock getTask(String taskId) {
		ProcessMsgLock rtn = null;
		synchronized (mapLock) {
			try {
				return WaitTasks.get(taskId);
			} catch (Throwable e) {
				e.printStackTrace();
			}
		}
		return rtn;
	}

	// 将任务执行结果保存进来，保存后会调用notify通知等待的程序：一般是在另外一个线程中调用
	public boolean putTaskResult(String taskId, String taskResult) {
		ProcessMsgLock lock = getTask(taskId);
		if (lock == null) return false;
		lock.notify(taskResult);
		return true;
	}

	// 把所有锁都给解锁：此方法当且仅当在进程退出时执行
	public void unlockAndClearAll() {
		Set<Entry<String, ProcessMsgLock>> entries = WaitTasks.entrySet();
		for (Entry<String, ProcessMsgLock> entry : entries) {
			try {// 通过try-catch保证每次解锁失败不影响其他的解锁
				entry.getValue().notify("");
			} catch (Throwable e) {
				e.printStackTrace();
			}
		}
		WaitTasks.clear();
	}

	@Override
	protected void finalize() throws Throwable {
		unlockAndClearAll();
		super.finalize();
	}

	/**
	 * 设置任务的默认等待时间，如果不设置所有的等待任务会一直等下去（容易造成死锁）
	 * 
	 * @param defaultWaitTime
	 */
	public void setTaskWaitTime(long defaultWaitTime) {
		this.defaultWaitTime = defaultWaitTime;
	}
}