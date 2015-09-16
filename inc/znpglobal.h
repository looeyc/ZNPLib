/*
 * znpglobal.h
 *
 *  Created on: 2014-2-12
 *      Author: chengm
 */

#ifndef ZNPGLOBAL_H_
#define ZNPGLOBAL_H_

#include "pthread.h"
#include "znpmsg.h"
#include "znpmsgfifo.h"

//ZNP的全局状态参数
typedef struct{
	int isInited;//1-已经初始化、else-没有初始化

	unsigned char ZDOState;	//ZNP当前的工作状态
							//0 - DEV_HOLD				Initialized - not started automatically
							//1 - DEV_INIT				Initialized - not connected to anything
							//2 - DEV_NWK_DISC			Discovering PAN's to join
							//3 - DEV_NWK_JOINING		Joining a PAN
							//4 - DEV_NWK_REJOIN		ReJoining a PAN, only for end devices
							//5 - DEV_END_DEVICE_UNAUTH	Joined but not yet authenticated by trust center
							//6 - DEV_END_DEVICE		Started as device after authentication
							//7 - DEV_ROUTER			Device joined, authenticated and is a router
							//8 - DEV_COORD_STARTING	Starting as Zigbee Coordinator
							//9 - DEV_ZB_COORD			Started as Zigbee Coordinator
							//10- DEV_NWK_ORPHAN		Device has lost information about its parent

	//回调，参数pZnpMsg为收到的消息，使用完毕后一定要调用DelocateZnpMsg销毁
	stZnpMsgFIFOHdl *ppZnpMsgFIFO_EP[256];//各EP的回调配套的消息接收队列句柄
	void (*pZnpMsgCB_EP[256])(unsigned char EndPoint, stZnpMsg *pZnpMsg);//各EP的回调函数指针数组

	stZnpMsgFIFOHdl *pZnpMsgFIFO_Default;//实现ZnpMsg消息向pZnpMsgCB_Default回调的异步推送
	void (*pZnpMsgCB_Default)(stZnpMsg *pZnpMsg);//Znp消息默认处理回调，所有没有指定处理方法的消息都会调用该接口进行处理，包括系统消息

	stZnpMsgSubsHdl *pGlobalZnpMsgSubsHdlList;//消息订阅列表指针
	pthread_mutex_t mutexGlobalZnpMsgSubsHdlList;//访问锁

	stZnpMsgFIFOHdl *pZnpMsgFIFO_COM;//ZNPLib内部COM部分向消息分发部分递交消息所使用的ZnpMsg消息队列

	//用于Znp_AF_DATA_REQUEST以及Znp_AF_DATA_REQUEST_SRC_RTG的数据发送与对应的应答消息配对
	pthread_mutex_t mutexTransID;
	unsigned char TransID;
}stZnpStatus;
extern stZnpStatus GlobalZnpStatus;

#endif /* ZNPGLOBAL_H_ */
