/*
 * znpinit.cpp
 *
 *  Created on: 2012-8-14
 *      Author: root
 */
#include <unistd.h>
#include <pthread.h>
#include <sys/msg.h>
#include "memory.h"
#include "debuglog.h"
#include "znpglobal.h"
#include "znpcomhal.h"
#include "znpcom.h"
#include "znpmsgfifo.h"
#include "znplib.h"

#define MAXRETRYCNT		5

stZnpStatus GlobalZnpStatus;

//负责调用ZnpInit参数中注册的默认回调，对到来的需要默认处理的ZnpMsg消息进行处理
void *Thread_ZNPMsgHandler_Default(void *pArg)
{
	stZnpMsg *pZnpMsg=NULL;

	while(1){
		//抽取消息，没有消息时阻塞
		pZnpMsg=ZnpMsgFIFOPop(GlobalZnpStatus.pZnpMsgFIFO_Default);
		if(pZnpMsg==NULL){
			DEBUGLOG0(3,"Error, ZnpMsgFIFOPop failed.\n");
			sleep(10);
		}
		//调用回调进行处理
		if(GlobalZnpStatus.pZnpMsgCB_Default!=NULL){
			GlobalZnpStatus.pZnpMsgCB_Default(pZnpMsg);
		}
		//处理后一定要销毁消息
		DelocateZnpMsg(pZnpMsg);
	}
	return NULL;
}

int ZnpInit(stZnpCfg *pZnpCfg)
{
	int rt;
	pthread_t ntid;
	unsigned char Temp8;
	int RetryCnt=0;

	if(GlobalZnpStatus.isInited==1){
		DEBUGLOG0(3,"ZNP already inited.\n");
		return 0;
	}

	if(pZnpCfg==NULL){
		return -1;
	}

	//初始化全局参数
	memset(&GlobalZnpStatus,0,sizeof(stZnpCfg));
	GlobalZnpStatus.pZnpMsgCB_Default=pZnpCfg->pDefaultZnpMsgCB;
	pthread_mutex_init(&GlobalZnpStatus.mutexGlobalZnpMsgSubsHdlList,NULL);
	pthread_mutex_init(&GlobalZnpStatus.mutexTransID,NULL);

	//创建内部使用的ZnpMsg消息队列
	GlobalZnpStatus.pZnpMsgFIFO_COM=ZnpMsgFIFOCreate();
	if(GlobalZnpStatus.pZnpMsgFIFO_COM==NULL){
		DEBUGLOG0(3,"Error, ZnpMsgFIFOCreate failed.\n");
		return -2;
	}

	//初始化串口
	rt=ZNPComInit(pZnpCfg->pZNPComPath);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZNPComInit failed %d.\n",rt);
		return -3;
	}

	//创建ZNP通信口接收线程，该线程用于接收ZNP通信口数据
	rt=pthread_create(&ntid,NULL,Thread_ZNPComRcvServer,NULL);
	if(rt!=0){
		DEBUGLOG1(3,"Error, pthread_create failed %d, can't create Thread_ZNPComRcvServer.\n",rt);
		return -4;
	}

	//创建ZnpMsg分发处理线程，该线程负责对COM部分送上来的消息进行分发处理
	rt=pthread_create(&ntid,NULL,Thread_ZNPMsgHandler,NULL);
	if(rt!=0){
		DEBUGLOG1(3,"Error, pthread_create failed %d, can't create Thread_ZNPMsgHandler.\n",rt);
		return -5;
	}

	//创建默认消息回调线程，该线程在需要默认处理的ZnpMsg到来时调用ZnpInit配置参数中注册的默认回调函数进行处理
	GlobalZnpStatus.pZnpMsgFIFO_Default=ZnpMsgFIFOCreate();//创建配套的消息队列
	if(GlobalZnpStatus.pZnpMsgFIFO_Default==NULL){
		DEBUGLOG0(3,"Error, ZnpMsgFIFOCreate failed.\n");
		return -6;
	}
	rt=pthread_create(&ntid,NULL,Thread_ZNPMsgHandler_Default,NULL);
	if(rt!=0){
		DEBUGLOG1(3,"Error, pthread_create failed %d, can't create Thread_ZNPMsgHandler_Default.\n",rt);
		return -7;
	}

	GlobalZnpStatus.isInited=1;

	//配置参数
	while(RetryCnt<MAXRETRYCNT){
		//先复位一次，让Znp模块退出之前的工作状态
		//rt=Znp_SYS_RESET(1);//Softreset 涉及到SBL，有时候会导致ZNP模块没有反应，如同死机？
		rt=Znp_SYS_RESET(0);//先复位一次，让Znp模块退出之前的工作状态
		if(rt != 0){
			DEBUGLOG1(2,"Error, Znp_SYS_RESET failed %d.\n",rt);
			RetryCnt++;
			continue;
		}

		if(pZnpCfg->isValid_IEEEAddr){
			rt = Znp_ZB_WRITE_CONFIGURATION((unsigned char)ZCD_NV_EXTADDR_ID,ZCD_NV_EXTADDR_SIZE,pZnpCfg->IEEEAddr);
			if(rt != 0){
				DEBUGLOG1(3,"Error, Znp_ZB_WRITE_CONFIGURATION IEEEAddr failed %d.\n",rt);
				RetryCnt++;
				continue;
			}
		}

		//写入其他参数
		Temp8=1;
		rt = Znp_ZB_WRITE_CONFIGURATION((unsigned char)ZCD_NV_ZDO_DIRECT_CB_ID,ZCD_NV_ZDO_DIRECT_CB_SIZE,&Temp8);
		if(rt != 0){
			DEBUGLOG1(3,"Error, Znp_ZB_WRITE_CONFIGURATION ZCD_NV_ZDO_DIRECT_CB failed %d.\n",rt);
			RetryCnt++;
			continue;
		}

		//以下参数需要清空网络状态NV参数
		if((pZnpCfg->isValid_ExtPANID)||(pZnpCfg->isValid_PANID)){
			//清空网络状态NV参数
			Temp8=STARTOPT_CLEAR_STATE;
			rt = Znp_ZB_WRITE_CONFIGURATION((unsigned char)ZCD_NV_STARTUP_OPTION_ID,ZCD_NV_STARTUP_OPTION_SIZE,&Temp8);
			if(rt != 0){
				DEBUGLOG1(3,"Error, Znp_ZB_WRITE_CONFIGURATION ZCD_NV_STARTUP_OPTION_ID failed %d.\n",rt);
			}

			rt=Znp_SYS_RESET(1);//复位一次才能清空
			if(rt != 0){
				DEBUGLOG1(2,"Error, Znp_SYS_RESET failed %d.\n",rt);
				RetryCnt++;
				continue;
			}else{
				DEBUGLOG0(3,"All NV items have been reset.\n");
			}
			//写入参数
			if(pZnpCfg->isValid_ExtPANID){
				rt = Znp_ZB_WRITE_CONFIGURATION((unsigned char)ZCD_NV_EXTENDED_PAN_ID,ZCD_NV_EXTENDED_PAN_SIZE,(unsigned char *)pZnpCfg->ExtPANID);
				if(rt != 0){
					DEBUGLOG1(3,"Error, Znp_ZB_WRITE_CONFIGURATION Extended PANID failed %d.\n",rt);
					RetryCnt++;
					continue;
				}
			}
			if(pZnpCfg->isValid_PANID){
				rt = Znp_ZB_WRITE_CONFIGURATION((unsigned char)ZCD_NV_PANID_ID,ZCD_NV_PANID_SIZE,(unsigned char *)(&pZnpCfg->PANID));
				if(rt != 0){
					DEBUGLOG1(3,"Error, Znp_ZB_WRITE_CONFIGURATION PANID failed %d.\n",rt);
					RetryCnt++;
					continue;
				}
			}
		}else{
			rt=Znp_SYS_RESET(0);//复位一次
			if(rt != 0){
				DEBUGLOG1(2,"Error, Znp_SYS_RESET failed %d.\n",rt);
				RetryCnt++;
				continue;
			}
		}

		//启动
		rt=Znp_ZB_START();
		if(rt!=0){
			DEBUGLOG1(3,"Error, Znp_ZB_START failed %d.\n",rt);
			RetryCnt++;
			continue;
		}
		DEBUGLOG0(0,"ZNP starting...\n");
		while(Znp_GetZDOState()!=DEV_ZB_COORD){//等待Znp启动完成
			sleep(1);
		}

		break;
	}
	if(RetryCnt>=MAXRETRYCNT){
		DEBUGLOG1(3,"Error, Znp_ZB_WRITE_CONFIGURATION failed, retried %d times with no success.\n",MAXRETRYCNT);
		return 1;
	}else{
		return 0;
	}
}

//正确的ZNP启动步骤:
//1. use the ZB_WRITE_CONFIGURATION command to configure at the minimum the
//ZCD_NV_LOGICAL_TYPE, ZCD_NV_PAN_ID, and ZCD_NV_CHANLIST configuration items.

//2. If the Simple API is used, the ZB_APP_REGISTER_REQUEST command should be
//sent by the host processor to register the application endpoint.

//3. The ZB_START_REQUEST command should be sent by the host processor to either form a network
//(if the device is a coordinator) or join a network (if the device is a router or end device).

//4. The host processor should then wait for the ZB_START_CONFIRM command with a status of ZB_SUCCESS
//before performing any other API operations.

//5. If the Simple API is not used after performing step 1, the AF_REGISTER command should be sent
//by the host processor to register the application endpoint.

//6. The ZDO_STARTUP_FROM_APP command should be sent by the host processor to either form a network
//(if the device is a coordinator) or join a network (if the device is a router or end device).

//7. The host processor should then wait for the ZDO_STATE_CHANGE_IND command with a status of
//DEV_ZB_COORD, DEV_ROUTER, or DEV_END_DEVICE before performing any other API operations.



