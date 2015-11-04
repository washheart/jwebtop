package org.jwebtop.demos.view;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Random;

import org.jwebtop.TaskUtil;
import org.jwebtop.TaskUtil.ProcessMsgLock;

public class TestTasks {
	private final static int MaxExecuteTime = 500;

	static class TaskExcuteThread extends Thread {
		private ProcessMsgLock task;
		private String taskId;

		TaskExcuteThread(String taskId, ProcessMsgLock task) {
			this.taskId = taskId;
			this.task = task;
		}

		@Override
		public void run() {
			System.out.println("   Excute  " + taskId);
			int v = 1200 + new Random().nextInt(MaxExecuteTime);
			try {
				Thread.sleep(v);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			TaskUtil.putTaskResult(taskId, taskId);
			System.out.println("   Excute(" + v + ")  " + taskId);
		}
	}

	static class TaskWaitThread extends Thread {
		private ProcessMsgLock task;
		private String taskId;

		TaskWaitThread(String taskId, ProcessMsgLock task) {
			this.taskId = taskId;
			this.task = task;
		}

		@Override
		public void run() {
			System.out.println("WaitTask  " + taskId);
			String result = task.waitResult();
			System.out.println("WaitTask  " + result + " " + taskId.equals(result));
		}
	}

	static void testLockWait() {
		int tasks = 10;
		List<TaskWaitThread> waits = new LinkedList<TestTasks.TaskWaitThread>();
		for (int i = 0; i < tasks; i++) {
			String taskId = "T" + i;
			ProcessMsgLock task = TaskUtil.addTask(taskId);
			TaskWaitThread t1 = new TaskWaitThread(taskId, task);
			t1.start();
			waits.add(t1);
			TaskExcuteThread t2 = new TaskExcuteThread(taskId, task);
			t2.start();
		}
		for (Iterator<TaskWaitThread> it = waits.iterator(); it.hasNext();) {
			TaskWaitThread t = it.next();
			try {
				t.join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}

	public static void main(String[] args) {
		testLockWait();
	}
}