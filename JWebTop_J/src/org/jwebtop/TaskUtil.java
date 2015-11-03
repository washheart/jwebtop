package org.jwebtop;

import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.UUID;

public class TaskUtil {

	// 锁、任务执行结果包装类
	public static class ProcessMsgLock {
		// private Object lock;
		// condition_variable g_queuecheck;
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
		public String waitResult() {
			if (notified) return result;
			try {
				synchronized (this) {
					this.wait();
				}
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			removeTask(this.taskId);
			return this.result;
		}

		// 结果取回后进行通知
		public void notify(String result) {
			this.result = result;
			synchronized (this) {
				this.notifyAll();
			}
			notified = true;
		}
	};

	// map<key,value>,key=已成功发送到远程的任务的id，value=任务执行完成后放置任务执行结果
	private final static Map<String, ProcessMsgLock> WaitTasks = new HashMap<String, TaskUtil.ProcessMsgLock>();
	private final static Object mapLock = new Object(); // 所有针对map的操作都要锁定

	// 生成任务的id
	public static String createTaskId() {
		return UUID.randomUUID().toString();
	}

	// 添加一个等待任务到任务列表
	public static ProcessMsgLock addTask(String taskId) {
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
	public static void removeTask(String taskId) {
		synchronized (mapLock) {
			try {
				WaitTasks.remove(taskId);// 移除某任务
			} catch (Throwable e) {
				e.printStackTrace();
			}
		}
	}

	private static ProcessMsgLock getTask(String taskId) {
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
	public static boolean putTaskResult(String taskId, String taskResult) {
		ProcessMsgLock lock = getTask(taskId);
		if (lock == null) return false;
		lock.notify(taskResult);
		return true;
	}

	// 把所有锁都给解锁：此方法当且仅当在进程退出时执行
	public static void unlockAndClearAll() {
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
}