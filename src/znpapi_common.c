/*
 * znpapi_common.c
 *
 *  Created on: 2014-2-13
 *      Author: chengm
 */
#include <stdlib.h>
#include <memory.h>
#include "debuglog.h"
#include "znpglobal.h"
#include "znpframe.h"
#include "znpmsg.h"
#include "znpcom.h"
#include "znpapi_common.h"

//分配一个ZnpCommonRsp句柄
stZnpCommonRsp *AllocateZnpCommonRsp()
{
	stZnpCommonRsp *pRet;
	pRet=(stZnpCommonRsp *)malloc(sizeof(stZnpCommonRsp));
	if(pRet!=NULL){
		memset(pRet,0,sizeof(stZnpCommonRsp));
	}
	return pRet;
}

//销毁一个ZnpCommonRsp句柄
//Option 选择是否销毁其中的消息数据，0-不销毁、1-销毁、其他取值保留
void DelocateZnpCommonRsp(stZnpCommonRsp *pZnpCommonRsp, int Option)
{
	if(pZnpCommonRsp==NULL){
		return;
	}

	if(Option!=0){
		if(pZnpCommonRsp->pZnpSRSPMsg!=NULL){
			DelocateZnpMsg(pZnpCommonRsp->pZnpSRSPMsg);
		}
		if(pZnpCommonRsp->pZnpARSPMsg!=NULL){
			DelocateZnpMsg(pZnpCommonRsp->pZnpARSPMsg);
		}
	}
	free(pZnpCommonRsp);
}

//执行请求，所有API都需要用到的流程
//pZnpReqMsg 需要执行（发送）的请求消息
//Srsp 要求的SRSP应答类型，0表示不需要SRSP应答
//Arsp 要求的ARSP应答类型，0表示不需要ARSP应答
//isArspTransIDNeeded - 0-ARSP不需要进行TransID匹配（其实只有极少的ARSP才有TransID）
//ArspTransID - 指定ARSP的TransID，当isArspTransIDNeeded为0时该参数忽略
//pZnpCommonRsp 调用者提供空间，用于返回SRSP、ARSP应答消息，如果请求不需要SRSP/ARSP，则返回的ZnpCommonRsp中，pZnpSRSPMsg/pZnpARSPMsg为NULL
//return 0-请求成功、1-SRSP等待超时、2-ARSP等待超时、else-出错
int ZnpAPICommon_ReqRoutine(stZnpMsg *pZnpReqMsg, unsigned short Srsp, unsigned short Arsp, unsigned char isArspTransIDNeeded, unsigned char ArspTransID, stZnpCommonRsp *pZnpCommonRsp)
{
	 int rt,err=0;
	 stZnpMsgSubsHdl *pZnpMsgSubsHdl_SRSP=NULL;
	 stZnpMsgSubsHdl *pZnpMsgSubsHdl_ARSP=NULL;
	 stZNPFrame ZNPFrame;

	//参数检查
	if(pZnpReqMsg==NULL){
		return -1;
	}
	if((Srsp!=0)||(Arsp!=0)){
		if(pZnpCommonRsp==NULL){
			return -1;
		}
	}
	memset(pZnpCommonRsp,0,sizeof(stZnpCommonRsp));

	//订阅SRSP应答消息
	if(Srsp!=0){
		pZnpMsgSubsHdl_SRSP=ZnpMsgSubscribe(Srsp,0,0);
		if(pZnpMsgSubsHdl_SRSP==NULL){
			DEBUGLOG0(3,"Error, ZnpMsgSubscribe failed.\n");
			return -2;
		}
	}
	//订阅ARSP应答消息
	if(Arsp!=0){
		pZnpMsgSubsHdl_ARSP=ZnpMsgSubscribe(Arsp,isArspTransIDNeeded,ArspTransID);
		if(pZnpMsgSubsHdl_ARSP==NULL){
			DEBUGLOG0(3,"Error, ZnpMsgSubscribe failed.\n");
			return -2;
		}
	}

	//发送请求消息
	TransZnpMsgToZnpFrame(pZnpReqMsg,&ZNPFrame);
	rt=ZNPComSendZNPFrame(&ZNPFrame);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZNPComSendZNPFrame failed %d, can't send znpframe.\n",rt);
		return -3;
	}

	//等待SRSP应答
	if(Srsp!=0){
		pZnpCommonRsp->pZnpSRSPMsg=ZnpMsgPend(pZnpMsgSubsHdl_SRSP,3000);
		ZnpMsgUnsubscribe(pZnpMsgSubsHdl_SRSP);//在等待ARSP之前立即退订SRSP应答消息，防止死锁
		if(pZnpCommonRsp->pZnpSRSPMsg==NULL){
			DEBUGLOG0(2,"Warning, ZnpMsgPend failed, SRSP time out.\n");
			err+=1;
		}
	}
	//等待ARSP应答
	if(Arsp!=0){
		if(err==0){
			pZnpCommonRsp->pZnpARSPMsg=ZnpMsgPend(pZnpMsgSubsHdl_ARSP,5000);
			if(pZnpCommonRsp->pZnpARSPMsg==NULL){
				DEBUGLOG0(2,"Warning, ZnpMsgPend failed, ARSP time out.\n");
				err+=2;
			}
		}
		ZnpMsgUnsubscribe(pZnpMsgSubsHdl_ARSP);//退订ARSP应答消息
	}

	return err;
}

 unsigned char Znp_GetZDOState()
 {
	 return GlobalZnpStatus.ZDOState;
 }
