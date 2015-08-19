#ifndef CEF_JWEBTOP_EXE_DLLEX_H_
#define CEF_JWEBTOP_EXE_DLLEX_H_
namespace jw{
	namespace dllex{// 和DLL进行交互的相关扩展方法
		// 创建一个线程用来监听远程进程是否终止以便结束当前程序
		void waitForRemoteProcessTerminate();
	}
}
#endif