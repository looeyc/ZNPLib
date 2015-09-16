/*
 * znpapi_sys.c
 *
 *  Created on: 2012-8-15
 *      Author: chengm
 */
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include "debuglog.h"
#include "znpframe.h"
#include "znpmsg.h"
#include "znpcom.h"
#include "znpapi_common.h"

int Znp_SYS_RESET(unsigned char Type)
{
	int rt,ret=0;
	stZnpMsg ZnpMsg_Req;
	stZnpMsg *pZnpMsg_Rsp=NULL;
	stZNPFrame ZNPFrame;
	stZnpMsgSubsHdl *pZnpMsgSubsHdl=NULL;

	//参数检查
	if((Type!=0)&&(Type!=1)){
		DEBUGLOG1(3,"Undefined param type %d for Znp_SYS_RESET.\n",Type);
		return -1;
	}

	//创建请求消息
	ZnpMsg_Req.MsgType=CMD_SYS_RESET_REQ;
	ZnpMsg_Req.un.mSYS_RESET_REQ.Type=Type;
	TransZnpMsgToZnpFrame(&ZnpMsg_Req,&ZNPFrame);

	//订阅应答消息
	pZnpMsgSubsHdl=ZnpMsgSubscribe(CMD_SYS_RESET_IND,0,0);
	if(pZnpMsgSubsHdl==NULL){
		DEBUGLOG0(3,"Error, ZnpMsgSubscribe failed.\n");
		return -2;
	}

	//发送请求消息
	rt=ZNPComSendZNPFrame(&ZNPFrame);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZNPComSendZNPFrame failed %d, can't send znpframe.\n",rt);
		return -3;
	}

	//等待应答消息
	pZnpMsg_Rsp=ZnpMsgPend(pZnpMsgSubsHdl,3000);
	if(pZnpMsg_Rsp==NULL){
		DEBUGLOG0(2,"Warning, ZnpMsgPend failed.\n");
		ret=-4;
	}

	//退订应答消息
	ZnpMsgUnsubscribe(pZnpMsgSubsHdl);

	//销毁消息
	DelocateZnpMsg(pZnpMsg_Rsp);

	return ret;
}

int Znp_SYS_SET_TX_POWER(char TX_Power, stSYS_SET_TX_POWER_R *pRet)
{
	int rt,ret=0;
	stZnpMsg ZnpMsg_Req;
	stZnpCommonRsp *pZnpCommonRsp=NULL;

	//参数检查
	if(pRet==NULL){
		return -1;
	}
	//创建请求消息
	ZnpMsg_Req.MsgType=CMD_SYS_SET_TX_POWER;
	ZnpMsg_Req.un.mSYS_SET_TX_POWER.TX_Power=TX_Power;
	//申请例程句柄
	pZnpCommonRsp=AllocateZnpCommonRsp();
	if(pZnpCommonRsp==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpCommonRsp failed.\n");
		return -2;
	}
	//执行请求例程
	rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_SYS_SET_TX_POWER_R, 0, 0, 0, pZnpCommonRsp);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
		ret=-3;
	}else{
		//处理应答消息
		*pRet=pZnpCommonRsp->pZnpSRSPMsg->un.mSYS_SET_TX_POWER_R;
	}
	//销毁例程句柄
	DelocateZnpCommonRsp(pZnpCommonRsp, 1);

	return ret;
}





