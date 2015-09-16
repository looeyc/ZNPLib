/*
 * znpmsgfifo.h
 *
 *  Created on: 2014-2-20
 *      Author: chengm
 */

#ifndef ZNPMSGFIFO_H_
#define ZNPMSGFIFO_H_

#include <pthread.h>
#include "znpmsg.h"

//ZnpMsg消息队列
typedef struct stZnpMsgListItem_ stZnpMsgListItem;
struct stZnpMsgListItem_{
	stZnpMsgListItem *pNext;
	stZnpMsg *pZnpMsg;
};

//ZnpMsg消息队列句柄
typedef struct{
	pthread_mutex_t m;//实现对队列的访问互斥
	pthread_cond_t c;
	stZnpMsgListItem *pZnpMsgList;//消息队列指针
	int MsgNum;//队列中的消息数量
}stZnpMsgFIFOHdl;

//创建一个ZnpMsg消息队列
//return 创建的消息队列句柄，NULL-创建失败
stZnpMsgFIFOHdl *ZnpMsgFIFOCreate();

//往ZnpMsg消息队列中加入一条消息
//pZnpMsgFIFO - 消息队列句柄
//pZnpMsg - 需要加入的ZnpMsg消息
//return 0-成功，else-失败
//成功后，消息的指针被排入队列，消息数据被FIFO接管
int ZnpMsgFIFOPush(stZnpMsgFIFOHdl *pZnpMsgFIFO, stZnpMsg *pZnpMsg);

//从ZnpMsg消息队列中取出一条消息
//pZnpMsgFIFO  - 消息队列句柄
//return 取出的消息指针，NULL表示出错
//成功后，消息数据由调用者接管，消息在使用完毕后要调用DelocateZnpMsg进行销毁
stZnpMsg *ZnpMsgFIFOPop(stZnpMsgFIFOHdl *pZnpMsgFIFO);

#endif /* ZNPMSGFIFO_H_ */








