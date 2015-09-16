/*
 * znpapi_af.c
 *
 *  Created on: 2014-2-17
 *      Author: chengm
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include "debuglog.h"
#include "znpframe.h"
#include "znpmsg.h"
#include "znpmsgfifo.h"
#include "znpcom.h"
#include "znpglobal.h"
#include "znpapi_common.h"

typedef struct{
	unsigned char EndPoint;
}stParam_Thread_ZNPMsgHandler_EP;

//负责调用EP的消息回调，对到来的与EP相关的消息进行处理
//传递过来的参数是通过malloc分配的不使用的时候要free掉
void *Thread_ZNPMsgHandler_EP(void *pArg)
{
	unsigned char EndPoint;
	stZnpMsg *pZnpMsg=NULL;
	stParam_Thread_ZNPMsgHandler_EP *pParam=(stParam_Thread_ZNPMsgHandler_EP *)pArg;

	//记录下传递过来的参数
	EndPoint=pParam->EndPoint;
	free(pParam);

	while(1){
		//抽取消息，没有消息时阻塞
		pZnpMsg=ZnpMsgFIFOPop(GlobalZnpStatus.ppZnpMsgFIFO_EP[EndPoint]);
		if(pZnpMsg==NULL){
			DEBUGLOG0(3,"Error, ZnpMsgFIFOPop failed.\n");
			sleep(10);
		}
		//调用回调进行处理
		if(GlobalZnpStatus.pZnpMsgCB_EP[EndPoint]!=NULL){
			GlobalZnpStatus.pZnpMsgCB_EP[EndPoint](EndPoint,pZnpMsg);
		}
		//处理后一定要销毁消息
		DelocateZnpMsg(pZnpMsg);
	}
	return NULL;
}

int Znp_AF_REGISTER(unsigned char EndPoint,unsigned short AppProfId,unsigned short AppDeviceId,unsigned char AppDevVer,unsigned char LatencyReq,unsigned char AppNumInClusters,unsigned short *pAppInClusterList,unsigned char AppNumOutClusters,unsigned short *pAppOutClusterList,void (*pCBFxn))
{
	int rt,ret=0;
	pthread_t ntid;
	stZnpMsg ZnpMsg_Req;
	stZnpCommonRsp *pZnpCommonRsp=NULL;
	stParam_Thread_ZNPMsgHandler_EP *pThreadParam=NULL;;

	//参数检查
	if(EndPoint==0){
		return -1;
	}
	if((AppNumInClusters!=0)&&(pAppInClusterList==NULL)){
		return -1;
	}
	if((AppNumOutClusters!=0)&&(pAppOutClusterList==NULL)){
		return -1;
	}
	if(pCBFxn==NULL){
		return -1;
	}

	//创建请求消息
	ZnpMsg_Req.MsgType=CMD_AF_REGISTER;
	ZnpMsg_Req.un.mAF_REGISTER.EndPoint=EndPoint;
	ZnpMsg_Req.un.mAF_REGISTER.AppProfId=AppProfId;
	ZnpMsg_Req.un.mAF_REGISTER.AppDeviceId=AppDeviceId;
	ZnpMsg_Req.un.mAF_REGISTER.AppDevVer=AppDevVer;
	ZnpMsg_Req.un.mAF_REGISTER.LatencyReq=LatencyReq;
	ZnpMsg_Req.un.mAF_REGISTER.AppNumInClusters=AppNumInClusters;
	memcpy(ZnpMsg_Req.un.mAF_REGISTER.AppInClusterList,pAppInClusterList,AppNumInClusters*2);
	ZnpMsg_Req.un.mAF_REGISTER.AppNumOutClusters=AppNumOutClusters;
	memcpy(ZnpMsg_Req.un.mAF_REGISTER.AppOutClusterList,pAppOutClusterList,AppNumOutClusters*2);

	//申请例程句柄
	pZnpCommonRsp=AllocateZnpCommonRsp();
	if(pZnpCommonRsp==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpCommonRsp failed.\n");
		return -2;
	}
	//执行请求例程
	rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_AF_REGISTER_R, 0, 0, 0, pZnpCommonRsp);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
		ret=-3;
	}else{
		//处理应答消息
		if(pZnpCommonRsp->pZnpSRSPMsg->un.mAF_REGISTER_R.Status!=0){
			DEBUGLOG1(2,"Warning, SRSP status indicated a failure %d.\n",pZnpCommonRsp->pZnpSRSPMsg->un.mZB_WRITE_CONFIGURATION_R.Status);
			ret=1;
		}else{
			//成功后在本地注册回调
			//创建配套的消息接收队列
			GlobalZnpStatus.ppZnpMsgFIFO_EP[EndPoint]=ZnpMsgFIFOCreate();
			if(GlobalZnpStatus.ppZnpMsgFIFO_EP[EndPoint]==NULL){
				DEBUGLOG0(3,"Error, ZnpMsgFIFOCreate failed.\n");
				ret=-4;
			}else{
				GlobalZnpStatus.pZnpMsgCB_EP[EndPoint]=pCBFxn;//注册回调指针，回调线程需要用
				//创建EP的消息回调线程，该线程在与EP相关的消息到来时调用注册的回调函数进行处理
				pThreadParam=(stParam_Thread_ZNPMsgHandler_EP *)malloc(sizeof(stParam_Thread_ZNPMsgHandler_EP));
				pThreadParam->EndPoint=EndPoint;//将EP作为参数传递给线程
				rt=pthread_create(&ntid,NULL,Thread_ZNPMsgHandler_EP,(void *)pThreadParam);
				if(rt!=0){
					DEBUGLOG1(3,"Error, pthread_create failed %d, can't create Thread_ZNPMsgHandler_EP.\n",rt);
					ret=-5;
				}
			}
		}
	}

	//销毁例程句柄
	DelocateZnpCommonRsp(pZnpCommonRsp, 1);

	return ret;
}

//获取当前唯一的TransID
static unsigned char GenTransID()
{
	unsigned char TransID;
	pthread_mutex_lock(&GlobalZnpStatus.mutexTransID);
	TransID=GlobalZnpStatus.TransID++;
	pthread_mutex_unlock(&GlobalZnpStatus.mutexTransID);

	return TransID;
}

int Znp_AF_DATA_REQUEST(unsigned short DstAddr,unsigned char DestEndpoint,unsigned char SrcEndpoint,unsigned short ClusterID,unsigned char Options,unsigned char Radius,unsigned char Len,unsigned char *pData)
{
	int rt,ret=0;
	unsigned char TransID;
	stZnpMsg ZnpMsg_Req;
	stZnpCommonRsp *pZnpCommonRsp=NULL;

	//参数检查
	if((Len!=0)&&(pData==NULL)){
		return -1;
	}
	if(Len>64){
		DEBUGLOG1(3,"Error, the data lenth (%d) should not exceed 64 byte.\n",Len);
		return -2;
	}
	//创建请求消息
	ZnpMsg_Req.MsgType=CMD_AF_DATA_REQUEST;
	ZnpMsg_Req.un.mAF_DATA_REQUEST.DstAddr=DstAddr;
	ZnpMsg_Req.un.mAF_DATA_REQUEST.DestEndpoint=DestEndpoint;
	ZnpMsg_Req.un.mAF_DATA_REQUEST.SrcEndpoint=SrcEndpoint;
	ZnpMsg_Req.un.mAF_DATA_REQUEST.ClusterID=ClusterID;
	TransID=GenTransID();
	ZnpMsg_Req.un.mAF_DATA_REQUEST.TransID=TransID;
	ZnpMsg_Req.un.mAF_DATA_REQUEST.Options=Options;
	ZnpMsg_Req.un.mAF_DATA_REQUEST.Radius=Radius;
	ZnpMsg_Req.un.mAF_DATA_REQUEST.Len=Len;
	memcpy(ZnpMsg_Req.un.mAF_DATA_REQUEST.Data,pData,Len);
	//申请例程句柄
	pZnpCommonRsp=AllocateZnpCommonRsp();
	if(pZnpCommonRsp==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpCommonRsp failed.\n");
		return -2;
	}

	//执行请求例程
	if(Options & AF_ACK_REQUEST){//如果需要AF_DATA_CONFIRM
		rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_AF_DATA_REQUEST_R, CMD_AF_DATA_CONFIRM, 1, TransID, pZnpCommonRsp);
		if(rt!=0){
			DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
			ret=-3;
		}else{
			//处理应答消息
			if(pZnpCommonRsp->pZnpSRSPMsg->un.mAF_DATA_REQUEST_R.Status!=0){
				DEBUGLOG1(2,"Warning, SRSP status indicated a failure %d.\n",pZnpCommonRsp->pZnpSRSPMsg->un.mZB_WRITE_CONFIGURATION_R.Status);
				ret=1;
			}else{
				if(pZnpCommonRsp->pZnpARSPMsg->un.mAF_DATA_CONFIRM.Status!=0){
					DEBUGLOG1(2,"Warning, ARSP status indicated a failure %d.\n",pZnpCommonRsp->pZnpARSPMsg->un.mAF_DATA_CONFIRM.Status);
					ret=2;
				}else if(pZnpCommonRsp->pZnpARSPMsg->un.mAF_DATA_CONFIRM.TransID!=TransID){
					DEBUGLOG2(3,"Error, ARSP TransID(%d) dose not match the request TransID (%d).\n",pZnpCommonRsp->pZnpARSPMsg->un.mAF_DATA_CONFIRM.TransID,TransID);
					ret=3;
				}else if(pZnpCommonRsp->pZnpARSPMsg->un.mAF_DATA_CONFIRM.Endpoint!=SrcEndpoint){
					DEBUGLOG2(3,"Error, ARSP Endpoint(%d) dose not match the request Endpoint (%d).\n",pZnpCommonRsp->pZnpARSPMsg->un.mAF_DATA_CONFIRM.Endpoint,DestEndpoint);
					ret=4;
				}
			}
		}
	}else{//不需要AF_DATA_CONFIRM
		rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_AF_DATA_REQUEST_R, 0, 0, 0, pZnpCommonRsp);
		if(rt!=0){
			DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
			ret=-3;
		}else{
			//处理应答消息
			if(pZnpCommonRsp->pZnpSRSPMsg->un.mAF_DATA_REQUEST_R.Status!=0){
				DEBUGLOG1(2,"Warning, SRSP status indicated a failure %d.\n",pZnpCommonRsp->pZnpSRSPMsg->un.mZB_WRITE_CONFIGURATION_R.Status);
				ret=1;
			}
		}
	}

	//销毁例程句柄
	DelocateZnpCommonRsp(pZnpCommonRsp, 1);

	return ret;
}

int Znp_AF_DATA_REQUEST_SRC_RTG(unsigned short DstAddr,unsigned char DestEndpoint,unsigned char SrcEndpoint,unsigned short ClusterID,unsigned char Options,unsigned char Radius,unsigned char RelayCount,unsigned short *pRelayList,unsigned char Len,unsigned char *pData)
{
	int rt,ret=0;
	unsigned char TransID;
	stZnpMsg ZnpMsg_Req;
	stZnpCommonRsp *pZnpCommonRsp=NULL;

	//参数检查
	if((RelayCount!=0)&&(pRelayList==NULL)){
		return -1;
	}
	if((Len!=0)&&(pData==NULL)){
		return -1;
	}
	if(RelayCount>55){
		return -1;
	}
	if(Len>128){
		return -1;
	}
	//创建请求消息
	ZnpMsg_Req.MsgType=CMD_AF_DATA_REQUEST_SRC_RTG;
	ZnpMsg_Req.un.mAF_DATA_REQUEST_SRC_RTG.DstAddr=DstAddr;
	ZnpMsg_Req.un.mAF_DATA_REQUEST_SRC_RTG.DestEndpoint=DestEndpoint;
	ZnpMsg_Req.un.mAF_DATA_REQUEST_SRC_RTG.SrcEndpoint=SrcEndpoint;
	ZnpMsg_Req.un.mAF_DATA_REQUEST_SRC_RTG.ClusterID=ClusterID;
	TransID=GenTransID();
	ZnpMsg_Req.un.mAF_DATA_REQUEST_SRC_RTG.TransID=TransID;
	ZnpMsg_Req.un.mAF_DATA_REQUEST_SRC_RTG.Options=Options;
	ZnpMsg_Req.un.mAF_DATA_REQUEST_SRC_RTG.Radius=Radius;
	ZnpMsg_Req.un.mAF_DATA_REQUEST_SRC_RTG.RelayCount=RelayCount;
	memcpy(ZnpMsg_Req.un.mAF_DATA_REQUEST_SRC_RTG.RelayList,pRelayList,RelayCount*2);
	ZnpMsg_Req.un.mAF_DATA_REQUEST_SRC_RTG.Len=Len;
	memcpy(ZnpMsg_Req.un.mAF_DATA_REQUEST_SRC_RTG.Data,pData,Len);
	//申请例程句柄
	pZnpCommonRsp=AllocateZnpCommonRsp();
	if(pZnpCommonRsp==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpCommonRsp failed.\n");
		return -2;
	}
	//执行请求例程
	rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_AF_DATA_REQUEST_SRC_RTG_R, CMD_AF_DATA_CONFIRM, 1, TransID, pZnpCommonRsp);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
		ret=-3;
	}else{
		//处理应答消息
		if(pZnpCommonRsp->pZnpSRSPMsg->un.mAF_DATA_REQUEST_SRC_RTG_R.Status!=0){
			DEBUGLOG1(2,"Warning, SRSP status indicated a failure %d.\n",pZnpCommonRsp->pZnpSRSPMsg->un.mAF_DATA_REQUEST_SRC_RTG_R.Status);
			ret=1;
		}
	}
	//销毁例程句柄
	DelocateZnpCommonRsp(pZnpCommonRsp, 1);

	return ret;
}








