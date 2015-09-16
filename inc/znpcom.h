/*
 * znpcom.h
 *
 *  Created on: 2014-2-11
 *      Author: chengm
 */

#ifndef ZNPCOM_H_
#define ZNPCOM_H_

#include "znpframe.h"

//ZNP底层通信口的数据接收与处理线程
void *Thread_ZNPComRcvServer(void *pArg);

//发送一个ZNPFrame到通信口
//return 0-成功、else-失败
int ZNPComSendZNPFrame(stZNPFrame *pZNPFrame);

#endif /* ZNPCOM_H_ */
