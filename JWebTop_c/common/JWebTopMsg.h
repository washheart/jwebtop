#ifndef jwebtop_multi_process_jwebtop_msg_H_
#define jwebtop_multi_process_jwebtop_msg_H_

#include "common/process/MultiProcess.h"


#define JWEBTOP_MSG_EXECJS      MPMSG_USER+1    // JWebTop进程与DLL通信时的消息值：执行JS脚本
#define JWEBTOP_MSG_EXECJSON    MPMSG_USER+2    // JWebTop进程与DLL通信时的消息值：执行JSON
#define JWEBTOP_MSG_LOADURL     MPMSG_USER+11   // 定义MPMSG_MINI和MPMSG_LARGE中msgId的起始值

#endif