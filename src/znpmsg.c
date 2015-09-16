/*
 * znpmsg.c
 *
 *  Created on: 2014-2-11
 *      Author: chengm
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <pthread.h>
#include "debuglog.h"
#include "znpglobal.h"
#include "znplib.h"
#include "znpmsg.h"
#include "znpmsgfifo.h"

//以指针p所指地址为起始，读取两个字节拼成一个Uint16型的数值，不需要p字节对齐
#define UNALIGEREAD_16BIT(p)		(((unsigned short)(*((unsigned char *)(p))))|(((unsigned short)(*((unsigned char *)(p)+1)))<<8))
//以指针p所指地址为起始，写入一个Uint16型的数据，不需要p字节对齐
#define UNALIGEWRITE_16BIT(p,a)		{*(unsigned char *)(p)=(unsigned char)(a);*((unsigned char *)(p)+1)=(unsigned char)((a)>>8);}

//创建一个znp msg
stZnpMsg *AllocateZnpMsg()
{
	stZnpMsg *pRet=NULL;

	pRet=(stZnpMsg *)malloc(sizeof(stZnpMsg));
	if(pRet==NULL){
		DEBUGLOG0(3,"Error, malloc failed, can't create ZnpMsg.\n");
		return NULL;
	}
	memset(pRet,0,sizeof(stZnpMsg));

	return pRet;
}

//销毁一个znp msg
void DelocateZnpMsg(stZnpMsg *pZnpMsg)
{
	if(pZnpMsg==NULL){
		return;
	}

	free(pZnpMsg);
}

//将ZnpMsg从ZNPFrame的字节流转换为本头文件所定义的结构体
//pZnpMsg - 需要转换的ZnpMsg
//pZNPFrame - 用于返回转换得到的ZNPFrame，由调用者提供
//return 0-成功，else-失败
int TransZnpMsgToZnpFrame(stZnpMsg *pZnpMsg, stZNPFrame *pZNPFrame)
{
	int rt,temp;

	if((pZNPFrame==NULL)||(pZnpMsg==NULL)){
		return -1;
	}

	memset(pZNPFrame,0,sizeof(stZNPFrame));

	pZNPFrame->Sof=ZNP_SOF;
	pZNPFrame->Cmd0=(pZnpMsg->MsgType>>8)&0x00FF;
	pZNPFrame->Cmd1=(pZnpMsg->MsgType)&0x00FF;
	switch(pZnpMsg->MsgType){
	case CMD_SYS_RESET_REQ:
	case CMD_SYS_SET_TX_POWER:
	case CMD_ZB_GET_DEVICE_INFO:
		pZNPFrame->DataLen=1;
		memcpy(pZNPFrame->DataFcs,&pZnpMsg->un,pZNPFrame->DataLen);
		break;
	case CMD_ZB_FIND_DEVICE_REQUEST:
		pZNPFrame->DataLen=8;
		memcpy(pZNPFrame->DataFcs,&pZnpMsg->un,pZNPFrame->DataLen);
		break;
	case CMD_ZB_START_REQUEST:
		pZNPFrame->DataLen=0;
		break;
	case CMD_ZB_READ_CONFIGURATION:
		pZNPFrame->DataLen=1;
		pZNPFrame->DataFcs[0]=pZnpMsg->un.mZB_READ_CONFIGURATION.ConfigId;
		break;
	case CMD_ZB_WRITE_CONFIGURATION:
		pZNPFrame->DataLen=2+pZnpMsg->un.mZB_WRITE_CONFIGURATION.Len;
		pZNPFrame->DataFcs[0]=pZnpMsg->un.mZB_WRITE_CONFIGURATION.ConfigId;
		pZNPFrame->DataFcs[1]=pZnpMsg->un.mZB_WRITE_CONFIGURATION.Len;
		memcpy(&pZNPFrame->DataFcs[2],pZnpMsg->un.mZB_WRITE_CONFIGURATION.Value,pZnpMsg->un.mZB_WRITE_CONFIGURATION.Len);
		break;
	case CMD_ZB_PERMIT_JOINING_REQUEST:
		pZNPFrame->DataLen=3;
		pZNPFrame->DataFcs[0]=pZnpMsg->un.mZB_PERMIT_JOINING_REQUEST.Destination&0x00FF;
		pZNPFrame->DataFcs[1]=(pZnpMsg->un.mZB_PERMIT_JOINING_REQUEST.Destination>>8)&0x00FF;
		pZNPFrame->DataFcs[2]=pZnpMsg->un.mZB_PERMIT_JOINING_REQUEST.Timeout;
		break;
	case CMD_ZDO_IEEE_ADDR_REQ:
		pZNPFrame->DataLen=4;
		memcpy(pZNPFrame->DataFcs,&pZnpMsg->un.mZDO_IEEE_ADDR_REQ.ShortAddr,2);
		pZNPFrame->DataFcs[2]=pZnpMsg->un.mZDO_IEEE_ADDR_REQ.ReqType;
		pZNPFrame->DataFcs[3]=pZnpMsg->un.mZDO_IEEE_ADDR_REQ.StartIndex;
		break;
	case CMD_ZDO_ACTIVE_EP_REQ:
		pZNPFrame->DataLen=4;
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs,pZnpMsg->un.mZDO_ACTIVE_EP_REQ.DstAddr);
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs+2,pZnpMsg->un.mZDO_ACTIVE_EP_REQ.NWKAddrOfInterest);
		break;
	case CMD_ZDO_NODE_DESC_REQ:
		pZNPFrame->DataLen=4;
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs,pZnpMsg->un.mZDO_NODE_DESC_REQ.DstAddr);
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs+2,pZnpMsg->un.mZDO_NODE_DESC_REQ.NWKAddrOfInterest);
		break;
	case CMD_ZDO_SIMPLE_DESC_REQ:
		pZNPFrame->DataLen=5;
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs,pZnpMsg->un.mZDO_SIMPLE_DESC_REQ.DstAddr);
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs+2,pZnpMsg->un.mZDO_SIMPLE_DESC_REQ.NWKAddrOfInterest);
		pZNPFrame->DataFcs[4]=pZnpMsg->un.mZDO_SIMPLE_DESC_REQ.Endpoint;
		break;
	case CMD_ZDO_USER_DESC_REQ:
		pZNPFrame->DataLen=4;
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs,pZnpMsg->un.mZDO_USER_DESC_REQ.DstAddr);
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs+2,pZnpMsg->un.mZDO_USER_DESC_REQ.NWKAddrOfInterest);
		break;
	case CMD_ZDO_USER_DESC_SET:
		pZNPFrame->DataLen=5+pZnpMsg->un.mZDO_USER_DESC_SET.Len;
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs,pZnpMsg->un.mZDO_USER_DESC_SET.DstAddr);
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs+2,pZnpMsg->un.mZDO_USER_DESC_SET.NWKAddrOfInterest);
		pZNPFrame->DataFcs[4]=pZnpMsg->un.mZDO_USER_DESC_SET.Len;
		temp=(pZnpMsg->un.mZDO_USER_DESC_SET.Len>16)?16:pZnpMsg->un.mZDO_USER_DESC_SET.Len;
		memcpy(pZNPFrame->DataFcs+5,pZnpMsg->un.mZDO_USER_DESC_SET.UserDescriptor,temp);
		break;
	case CMD_ZDO_MGMT_LEAVE_REQ:
		pZNPFrame->DataLen=11;
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs,pZnpMsg->un.mZDO_MGMT_LEAVE_REQ.DstAddr);
		memcpy(pZNPFrame->DataFcs+2,pZnpMsg->un.mZDO_MGMT_LEAVE_REQ.DeviceAddr,8);
		pZNPFrame->DataFcs[10]=pZnpMsg->un.mZDO_MGMT_LEAVE_REQ.RemoveChildren_Rejoin;
		break;
	case CMD_ZDO_MGMT_RTG_REQ:
		pZNPFrame->DataLen=3;
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs,pZnpMsg->un.mZDO_MGMT_RTG_REQ.DstAddr);
		pZNPFrame->DataFcs[2]=pZnpMsg->un.mZDO_MGMT_RTG_REQ.StartIndex;
		break;
	case CMD_ZDO_BIND_REQ:
		pZNPFrame->DataLen=23;
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs,pZnpMsg->un.mZDO_BIND_REQ.DstAddr);
		memcpy(pZNPFrame->DataFcs+2,pZnpMsg->un.mZDO_BIND_REQ.SrcAddress,8);
		pZNPFrame->DataFcs[10]=pZnpMsg->un.mZDO_BIND_REQ.SrcEndpoint;
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs+11,pZnpMsg->un.mZDO_BIND_REQ.ClusterID);
		pZNPFrame->DataFcs[13]=pZnpMsg->un.mZDO_BIND_REQ.DstAddrMode;
		memcpy(pZNPFrame->DataFcs+14,pZnpMsg->un.mZDO_BIND_REQ.DstAddress,8);
		pZNPFrame->DataFcs[22]=pZnpMsg->un.mZDO_BIND_REQ.DstEndpoint;
		break;
	case CMD_AF_REGISTER:
		pZNPFrame->DataLen=pZnpMsg->un.mAF_REGISTER.AppNumInClusters*2+pZnpMsg->un.mAF_REGISTER.AppNumOutClusters*2+9;
		pZNPFrame->DataFcs[0]=pZnpMsg->un.mAF_REGISTER.EndPoint;
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs+1,pZnpMsg->un.mAF_REGISTER.AppProfId);
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs+3,pZnpMsg->un.mAF_REGISTER.AppDeviceId);
		pZNPFrame->DataFcs[5]=pZnpMsg->un.mAF_REGISTER.AppDevVer;
		pZNPFrame->DataFcs[6]=pZnpMsg->un.mAF_REGISTER.LatencyReq;
		pZNPFrame->DataFcs[7]=pZnpMsg->un.mAF_REGISTER.AppNumInClusters;
		memcpy(pZNPFrame->DataFcs+8,pZnpMsg->un.mAF_REGISTER.AppInClusterList,pZnpMsg->un.mAF_REGISTER.AppNumInClusters*2);
		pZNPFrame->DataFcs[8+pZnpMsg->un.mAF_REGISTER.AppNumInClusters*2]=pZnpMsg->un.mAF_REGISTER.AppNumOutClusters;
		memcpy(pZNPFrame->DataFcs+9+pZnpMsg->un.mAF_REGISTER.AppNumInClusters*2,pZnpMsg->un.mAF_REGISTER.AppOutClusterList,pZnpMsg->un.mAF_REGISTER.AppNumOutClusters*2);
		break;
	case CMD_AF_DATA_REQUEST:
		pZNPFrame->DataLen=pZnpMsg->un.mAF_DATA_REQUEST.Len+10;
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs,pZnpMsg->un.mAF_DATA_REQUEST.DstAddr);
		pZNPFrame->DataFcs[2]=pZnpMsg->un.mAF_DATA_REQUEST.DestEndpoint;
		pZNPFrame->DataFcs[3]=pZnpMsg->un.mAF_DATA_REQUEST.SrcEndpoint;
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs+4,pZnpMsg->un.mAF_DATA_REQUEST.ClusterID);
		pZNPFrame->DataFcs[6]=pZnpMsg->un.mAF_DATA_REQUEST.TransID;
		pZNPFrame->DataFcs[7]=pZnpMsg->un.mAF_DATA_REQUEST.Options;
		pZNPFrame->DataFcs[8]=pZnpMsg->un.mAF_DATA_REQUEST.Radius;
		pZNPFrame->DataFcs[9]=pZnpMsg->un.mAF_DATA_REQUEST.Len;
		memcpy(pZNPFrame->DataFcs+10,pZnpMsg->un.mAF_DATA_REQUEST.Data,pZnpMsg->un.mAF_DATA_REQUEST.Len);
		break;
	case CMD_AF_DATA_REQUEST_SRC_RTG:
		pZNPFrame->DataLen=11+2*pZnpMsg->un.mAF_DATA_REQUEST_SRC_RTG.RelayCount+pZnpMsg->un.mAF_DATA_REQUEST_SRC_RTG.Len;
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs,pZnpMsg->un.mAF_DATA_REQUEST_SRC_RTG.DstAddr);
		pZNPFrame->DataFcs[2]=pZnpMsg->un.mAF_DATA_REQUEST_SRC_RTG.DestEndpoint;
		pZNPFrame->DataFcs[3]=pZnpMsg->un.mAF_DATA_REQUEST_SRC_RTG.SrcEndpoint;
		UNALIGEWRITE_16BIT(pZNPFrame->DataFcs+4,pZnpMsg->un.mAF_DATA_REQUEST_SRC_RTG.ClusterID);
		pZNPFrame->DataFcs[6]=pZnpMsg->un.mAF_DATA_REQUEST_SRC_RTG.TransID;
		pZNPFrame->DataFcs[7]=pZnpMsg->un.mAF_DATA_REQUEST_SRC_RTG.Options;
		pZNPFrame->DataFcs[8]=pZnpMsg->un.mAF_DATA_REQUEST_SRC_RTG.Radius;
		pZNPFrame->DataFcs[9]=pZnpMsg->un.mAF_DATA_REQUEST_SRC_RTG.RelayCount;
		memcpy(pZNPFrame->DataFcs+10,pZnpMsg->un.mAF_DATA_REQUEST_SRC_RTG.RelayList,pZnpMsg->un.mAF_DATA_REQUEST_SRC_RTG.RelayCount*2);
		pZNPFrame->DataFcs[10+pZnpMsg->un.mAF_DATA_REQUEST_SRC_RTG.RelayCount*2]=pZnpMsg->un.mAF_DATA_REQUEST_SRC_RTG.Len;
		memcpy(pZNPFrame->DataFcs+11+pZnpMsg->un.mAF_DATA_REQUEST_SRC_RTG.RelayCount*2,pZnpMsg->un.mAF_DATA_REQUEST_SRC_RTG.Data,pZnpMsg->un.mAF_DATA_REQUEST_SRC_RTG.Len);
		break;
	default:
		DEBUGLOG1(3,"Error, unknown ZnpMsg type 0x%04X.\n",pZnpMsg->MsgType);
		return 1;
		break;
	}
	//校验码
	rt=fRefreshZNPFrameFCS(pZNPFrame);
	if(rt!=0){
		DEBUGLOG1(3,"Error, fRefreshZNPFrameFCS failed %d.\n",rt);
	}

	return 0;
}

//将ZnpMsg从本头文件所定义的结构体转换为ZNPFrame的字节流
//pZNPFrame - 需要转换的ZNPFrame
//pZnpMsg - 转换得到的ZnpMsg，事先要调用AllocateZnpMsg分配得到，使用要调用DellocateZnpMsg销毁
//return 0-成功，else-失败
int TransZnpFrameToZnpMsg(stZNPFrame *pZNPFrame, stZnpMsg *pZnpMsg)
{
	int i;
	unsigned short CmdID;

	if((pZNPFrame==NULL)||(pZnpMsg==NULL)){
		return -1;
	}

	CmdID=GETZNPFRAMECMDID(pZNPFrame);

	pZnpMsg->MsgType=CmdID;
	switch(CmdID){
	//符合镜像拷贝的结构体
	case CMD_SYS_RESET_IND:
	case CMD_SYS_SET_TX_POWER_R:
	case CMD_ZDO_STATE_CHANGE_IND:
	case CMD_ZDO_MGMT_PERMIT_JOIN_RSP:
	case CMD_ZDO_IEEE_ADDR_REQ_R:
	case CMD_ZDO_ACTIVE_EP_REQ_R:
	case CMD_ZDO_NODE_DESC_REQ_R:
	case CMD_ZDO_SIMPLE_DESC_REQ_R:
	case CMD_ZDO_USER_DESC_REQ_R:
	case CMD_ZDO_USER_DESC_SET_R:
	case CMD_ZDO_MGMT_LEAVE_REQ_R:
	case CMD_ZDO_MGMT_RTG_REQ_R:
	case CMD_ZDO_BIND_REQ_R:
	case CMD_ZB_START_REQUEST_R:
	case CMD_ZB_WRITE_CONFIGURATION_R:
	case CMD_ZB_READ_CONFIGURATION_R:
	case CMD_ZB_PERMIT_JOINING_REQUEST_R:
	case CMD_ZB_FIND_DEVICE_REQUEST_R:
	case CMD_AF_REGISTER_R:
	case CMD_AF_DATA_REQUEST_R:
	case CMD_AF_DATA_CONFIRM:
	case CMD_AF_DATA_REQUEST_SRC_RTG_R:
		memcpy(&pZnpMsg->un,pZNPFrame->DataFcs,pZNPFrame->DataLen);
		break;
	case CMD_AF_INCOMING_MSG:
		pZnpMsg->un.mAF_INCOMING_MSG.GroupID=UNALIGEREAD_16BIT(pZNPFrame->DataFcs);
		pZnpMsg->un.mAF_INCOMING_MSG.ClusterID=UNALIGEREAD_16BIT(pZNPFrame->DataFcs+2);
		pZnpMsg->un.mAF_INCOMING_MSG.SrcAddr=UNALIGEREAD_16BIT(pZNPFrame->DataFcs+4);
		pZnpMsg->un.mAF_INCOMING_MSG.SrcEndpoint=pZNPFrame->DataFcs[6];
		pZnpMsg->un.mAF_INCOMING_MSG.DestEndpoint=pZNPFrame->DataFcs[7];
		pZnpMsg->un.mAF_INCOMING_MSG.WasBroadcast=pZNPFrame->DataFcs[8];
		pZnpMsg->un.mAF_INCOMING_MSG.LinkQuality=pZNPFrame->DataFcs[9];
		pZnpMsg->un.mAF_INCOMING_MSG.SecurityUse=pZNPFrame->DataFcs[10];
		memcpy(&pZnpMsg->un.mAF_INCOMING_MSG.TimeStamp,pZNPFrame->DataFcs+11,4);
		pZnpMsg->un.mAF_INCOMING_MSG.TransSeqNumber=pZNPFrame->DataFcs[15];
		pZnpMsg->un.mAF_INCOMING_MSG.Len=pZNPFrame->DataFcs[16];
		memcpy(pZnpMsg->un.mAF_INCOMING_MSG.Data,pZNPFrame->DataFcs+17,pZnpMsg->un.mAF_INCOMING_MSG.Len);
		break;
	case CMD_ZB_GET_DEVICE_INFO_R:
		pZnpMsg->un.mZB_GET_DEVICE_INFO_R.Param=pZNPFrame->DataFcs[0];
		memcpy(pZnpMsg->un.mZB_GET_DEVICE_INFO_R.Value,pZNPFrame->DataFcs+1,8);
		break;
	case CMD_ZB_FIND_DEVICE_CONFIRM:
		pZnpMsg->un.mZB_FIND_DEVICE_CONFIRM.SearchType=pZNPFrame->DataFcs[0];
		memcpy(pZnpMsg->un.mZB_FIND_DEVICE_CONFIRM.SearchKey,pZNPFrame->DataFcs+1,2);
		memcpy(pZnpMsg->un.mZB_FIND_DEVICE_CONFIRM.Result,pZNPFrame->DataFcs+3,8);
		break;
	case CMD_ZDO_NWK_ADDR_RSP:
	case CMD_ZDO_IEEE_ADDR_RSP:
		pZnpMsg->un.mZDO_NWK_ADDR_RSP.Status=pZNPFrame->DataFcs[0];
		memcpy(pZnpMsg->un.mZDO_NWK_ADDR_RSP.IEEEAddr,pZNPFrame->DataFcs+1,8);
		memcpy(&pZnpMsg->un.mZDO_NWK_ADDR_RSP.NwkAddr,pZNPFrame->DataFcs+9,2);
		pZnpMsg->un.mZDO_NWK_ADDR_RSP.StartIndex=pZNPFrame->DataFcs[11];
		pZnpMsg->un.mZDO_NWK_ADDR_RSP.NumAssocDev=pZNPFrame->DataFcs[12];
		memcpy(pZnpMsg->un.mZDO_NWK_ADDR_RSP.AssocDevList,pZNPFrame->DataFcs+13,70);
		break;
	case CMD_ZDO_END_DEVICE_ANNCE_IND:
		memcpy(&pZnpMsg->un.mZDO_END_DEVICE_ANNCE_IND.SrcAddr,pZNPFrame->DataFcs,2);
		memcpy(&pZnpMsg->un.mZDO_END_DEVICE_ANNCE_IND.NwkAddr,pZNPFrame->DataFcs+2,2);
		memcpy(&pZnpMsg->un.mZDO_END_DEVICE_ANNCE_IND.IEEEAddr,pZNPFrame->DataFcs+4,8);
		memcpy(&pZnpMsg->un.mZDO_END_DEVICE_ANNCE_IND.Capabilites,pZNPFrame->DataFcs+12,1);
		break;
	case CMD_ZDO_ACTIVE_EP_RSP:
		pZnpMsg->un.mZDO_ACTIVE_EP_RSP.SrcAddr=UNALIGEREAD_16BIT(pZNPFrame->DataFcs);
		pZnpMsg->un.mZDO_ACTIVE_EP_RSP.Status=pZNPFrame->DataFcs[2];
		pZnpMsg->un.mZDO_ACTIVE_EP_RSP.NWKAddr=UNALIGEREAD_16BIT(pZNPFrame->DataFcs+3);
		pZnpMsg->un.mZDO_ACTIVE_EP_RSP.ActiveEPCount=pZNPFrame->DataFcs[5];
		memcpy(pZnpMsg->un.mZDO_ACTIVE_EP_RSP.ActiveEPList,pZNPFrame->DataFcs+6,pZnpMsg->un.mZDO_ACTIVE_EP_RSP.ActiveEPCount);
		break;
	case CMD_ZDO_NODE_DESC_RSP:
		pZnpMsg->un.mZDO_NODE_DESC_RSP.SrcAddr=UNALIGEREAD_16BIT(pZNPFrame->DataFcs);
		pZnpMsg->un.mZDO_NODE_DESC_RSP.Status=pZNPFrame->DataFcs[2];
		pZnpMsg->un.mZDO_NODE_DESC_RSP.NWKAddrOfInterest=UNALIGEREAD_16BIT(pZNPFrame->DataFcs+3);
		pZnpMsg->un.mZDO_NODE_DESC_RSP.LogicalType_CmplxDscAvl_UserDscAvl=pZNPFrame->DataFcs[5];
		pZnpMsg->un.mZDO_NODE_DESC_RSP.APSFlags_FrequencyBand=pZNPFrame->DataFcs[6];
		pZnpMsg->un.mZDO_NODE_DESC_RSP.MacCapabilitiesFlags=pZNPFrame->DataFcs[7];
		pZnpMsg->un.mZDO_NODE_DESC_RSP.ManufacturerCode=UNALIGEREAD_16BIT(pZNPFrame->DataFcs+8);
		pZnpMsg->un.mZDO_NODE_DESC_RSP.MaxBufferSize=pZNPFrame->DataFcs[10];
		pZnpMsg->un.mZDO_NODE_DESC_RSP.MaxInTransferSize=UNALIGEREAD_16BIT(pZNPFrame->DataFcs+11);
		pZnpMsg->un.mZDO_NODE_DESC_RSP.ServerMask=UNALIGEREAD_16BIT(pZNPFrame->DataFcs+13);
		pZnpMsg->un.mZDO_NODE_DESC_RSP.MaxOutTransferSize=UNALIGEREAD_16BIT(pZNPFrame->DataFcs+15);
		pZnpMsg->un.mZDO_NODE_DESC_RSP.DescriptorCapabilities=pZNPFrame->DataFcs[17];
		break;
	case CMD_ZDO_SIMPLE_DESC_RSP:
		pZnpMsg->un.mZDO_SIMPLE_DESC_RSP.SrcAddr=UNALIGEREAD_16BIT(pZNPFrame->DataFcs);
		pZnpMsg->un.mZDO_SIMPLE_DESC_RSP.Status=pZNPFrame->DataFcs[2];
		pZnpMsg->un.mZDO_SIMPLE_DESC_RSP.NWKAddr=UNALIGEREAD_16BIT(pZNPFrame->DataFcs+3);
		pZnpMsg->un.mZDO_SIMPLE_DESC_RSP.Len=pZNPFrame->DataFcs[5];
		pZnpMsg->un.mZDO_SIMPLE_DESC_RSP.Endpoint=pZNPFrame->DataFcs[6];
		pZnpMsg->un.mZDO_SIMPLE_DESC_RSP.ProfileId=UNALIGEREAD_16BIT(pZNPFrame->DataFcs+7);
		pZnpMsg->un.mZDO_SIMPLE_DESC_RSP.DeviceID=UNALIGEREAD_16BIT(pZNPFrame->DataFcs+9);
		pZnpMsg->un.mZDO_SIMPLE_DESC_RSP.DeviceVersion=pZNPFrame->DataFcs[11];
		pZnpMsg->un.mZDO_SIMPLE_DESC_RSP.NumInClusters=pZNPFrame->DataFcs[12];
		memcpy(pZnpMsg->un.mZDO_SIMPLE_DESC_RSP.InClusterList,pZNPFrame->DataFcs+13,pZnpMsg->un.mZDO_SIMPLE_DESC_RSP.NumInClusters*2);
		pZnpMsg->un.mZDO_SIMPLE_DESC_RSP.NumOutClusters=pZNPFrame->DataFcs[13+pZnpMsg->un.mZDO_SIMPLE_DESC_RSP.NumInClusters*2];
		memcpy(pZnpMsg->un.mZDO_SIMPLE_DESC_RSP.OutClusterList,pZNPFrame->DataFcs+13+pZnpMsg->un.mZDO_SIMPLE_DESC_RSP.NumInClusters*2+1,pZnpMsg->un.mZDO_SIMPLE_DESC_RSP.NumOutClusters*2);
		break;
	case CMD_ZDO_USER_DESC_RSP:
		pZnpMsg->un.mZDO_USER_DESC_RSP.SrcAddr=UNALIGEREAD_16BIT(pZNPFrame->DataFcs);
		pZnpMsg->un.mZDO_USER_DESC_RSP.Status=pZNPFrame->DataFcs[2];
		pZnpMsg->un.mZDO_USER_DESC_RSP.NWKAddr=UNALIGEREAD_16BIT(pZNPFrame->DataFcs+3);
		pZnpMsg->un.mZDO_USER_DESC_RSP.UserLength=pZNPFrame->DataFcs[5];
		memset(pZnpMsg->un.mZDO_USER_DESC_RSP.UserDescriptor,0,77);
		memcpy(pZnpMsg->un.mZDO_USER_DESC_RSP.UserDescriptor,pZNPFrame->DataFcs+6,pZnpMsg->un.mZDO_USER_DESC_RSP.UserLength);
		break;
	case CMD_ZDO_USER_DESC_CONF:
		pZnpMsg->un.mZDO_USER_DESC_CONF.SrcAddr=UNALIGEREAD_16BIT(pZNPFrame->DataFcs);
		pZnpMsg->un.mZDO_USER_DESC_CONF.Status=pZNPFrame->DataFcs[2];
		pZnpMsg->un.mZDO_USER_DESC_CONF.NWKAddr=UNALIGEREAD_16BIT(pZNPFrame->DataFcs+3);
		break;
	case CMD_ZDO_MGMT_LEAVE_RSP:
		pZnpMsg->un.mZDO_MGMT_LEAVE_RSP.SrcAddr=UNALIGEREAD_16BIT(pZNPFrame->DataFcs);
		pZnpMsg->un.mZDO_MGMT_LEAVE_RSP.Status=pZNPFrame->DataFcs[2];
		break;
	case CMD_ZDO_LEAVE_IND:
		pZnpMsg->un.mZDO_LEAVE_IND.SrcAddr=UNALIGEREAD_16BIT(pZNPFrame->DataFcs);
		memcpy(pZnpMsg->un.mZDO_LEAVE_IND.ExtAddr,pZNPFrame->DataFcs+2,8);
		pZnpMsg->un.mZDO_LEAVE_IND.Request=pZNPFrame->DataFcs[10];
		pZnpMsg->un.mZDO_LEAVE_IND.Remove=pZNPFrame->DataFcs[11];
		pZnpMsg->un.mZDO_LEAVE_IND.Rejoin=pZNPFrame->DataFcs[12];
		break;
	case CMD_ZDO_MGMT_RTG_RSP:
		pZnpMsg->un.mZDO_MGMT_RTG_RSP.SrcAddr=UNALIGEREAD_16BIT(pZNPFrame->DataFcs);
		pZnpMsg->un.mZDO_MGMT_RTG_RSP.Status=pZNPFrame->DataFcs[2];
		pZnpMsg->un.mZDO_MGMT_RTG_RSP.RoutingTableEntries=pZNPFrame->DataFcs[3];
		pZnpMsg->un.mZDO_MGMT_RTG_RSP.StartIndex=pZNPFrame->DataFcs[4];
		pZnpMsg->un.mZDO_MGMT_RTG_RSP.RoutingTableListCount=pZNPFrame->DataFcs[5];
		for(i=0;i<pZnpMsg->un.mZDO_MGMT_RTG_RSP.RoutingTableListCount;i++){
			pZnpMsg->un.mZDO_MGMT_RTG_RSP.RoutingTableList[i].DestinationAddress=UNALIGEREAD_16BIT(pZNPFrame->DataFcs+6+i*5);
			pZnpMsg->un.mZDO_MGMT_RTG_RSP.RoutingTableList[i].Status=pZNPFrame->DataFcs[6+i*5+2];
			pZnpMsg->un.mZDO_MGMT_RTG_RSP.RoutingTableList[i].NextHop=UNALIGEREAD_16BIT(pZNPFrame->DataFcs+6+i*5+3);
		}
		break;
	case CMD_ZDO_BIND_RSP:
		pZnpMsg->un.mZDO_BIND_RSP.SrcAddr=UNALIGEREAD_16BIT(pZNPFrame->DataFcs);
		pZnpMsg->un.mZDO_BIND_RSP.Status=pZNPFrame->DataFcs[2];
		break;
	default:
		DEBUGLOG1(3,"Error, unknown ZnpMsg type 0x%04X.\n",pZnpMsg->MsgType);
		return 1;
		break;
	}

	return 0;
}

//订阅消息
//MsgType 需要订阅的消息类型
//isTransIDNeeded - 是否需要指定TransID，0-不需要、1-需要
//TransID - 指定的TransID，isTransIDNeeded为0时该参数忽略
stZnpMsgSubsHdl *ZnpMsgSubscribe(unsigned short MsgType, unsigned char isTransIDNeeded, unsigned char TransID)
{
	stZnpMsgSubsHdl *pRet=NULL;
	stZnpMsgSubsHdl *pScan=NULL;

	pRet=(stZnpMsgSubsHdl *)malloc(sizeof(stZnpMsgSubsHdl));
	if(pRet==NULL){
		DEBUGLOG0(3,"Error, malloc failed.\n");
		return NULL;
	}
	memset(pRet,0,sizeof(stZnpMsgSubsHdl));
	pRet->MsgType=MsgType;
	pRet->isTransIDNeeded=isTransIDNeeded;
	pRet->TransID=TransID;
	pthread_mutex_init(&pRet->m,NULL);
	pthread_cond_init(&pRet->c,NULL);

	//加入全局订阅列表
	pthread_mutex_lock(&GlobalZnpStatus.mutexGlobalZnpMsgSubsHdlList);
	if(GlobalZnpStatus.pGlobalZnpMsgSubsHdlList==NULL){
		GlobalZnpStatus.pGlobalZnpMsgSubsHdlList=pRet;
	}else{
		pScan=GlobalZnpStatus.pGlobalZnpMsgSubsHdlList;
		while(pScan->pNext!=NULL){
			pScan=pScan->pNext;
		}
		pScan->pNext=pRet;
		pRet->pPrev=pScan;
	}
	pthread_mutex_unlock(&GlobalZnpStatus.mutexGlobalZnpMsgSubsHdlList);

	return pRet;
}

//等待订阅的消息
//pZnpMsgSubsHdl 订阅句柄
//Timeout 最大等待时间（毫秒 ms）
//return 成功时，返回得到的消息，失败时为NULL，消息使用完毕后要调用DelocateZnpMsg销毁
stZnpMsg *ZnpMsgPend(stZnpMsgSubsHdl *pZnpMsgSubsHdl, int Timeout)
{
	int rt;
	struct timespec tv;
	struct timeval now;
	stZnpMsg *pRet=NULL;

	if((pZnpMsgSubsHdl==NULL)||(Timeout<0)){
		return NULL;
	}

	pthread_mutex_lock(&pZnpMsgSubsHdl->m);//加锁的目的是防止在访问isMsgValid标志位的时候被同时到来的ZnpMsgPost干扰，同时防止在pthread_cond_timedwait前就发生ZnpMsgPost
	if(pZnpMsgSubsHdl->isMsgValid==0){//如果消息没有到来，也就是还没有发生ZnpMsgPost动作，则等待
		gettimeofday(&now,NULL);
		tv.tv_sec = now.tv_sec + Timeout/1000;
		tv.tv_nsec = now.tv_usec*1000 + (Timeout%1000)*1000*1000;

		while(1){
			rt=pthread_cond_timedwait(&pZnpMsgSubsHdl->c,&pZnpMsgSubsHdl->m,&tv);
			if(rt == 0){//触发
				if(pZnpMsgSubsHdl->isMsgValid != 0){//确实触发了
					break;
				}
			}else if(rt == ETIMEDOUT){//超时
				DEBUGLOG1(2,"Warning, ZnpMsgPend for 0x%04X timed out.\n",pZnpMsgSubsHdl->MsgType);
				break;
			}else{//出错
				DEBUGLOG1(3,"Error, pthread_cond_timedwait failed %d.\n",rt);
				break;
			}
		}
	}
	//当消息确实到来时，提取消息
	if(pZnpMsgSubsHdl->isMsgValid){
		pRet=pZnpMsgSubsHdl->pZnpMsg;
		pZnpMsgSubsHdl->isMsgValid=0;//消息已经被提取，将相应标志位清空
		pZnpMsgSubsHdl->pZnpMsg=NULL;
	}
	pthread_mutex_unlock(&pZnpMsgSubsHdl->m);

	return pRet;
}

//寄出消息
//在消息订阅列表中查找订阅者，并向其寄出消息
//return 0-表示该消息当前无订阅者, >0-表示该消息当前的订阅者数量, <0-出错
//当前只允许一条消息有一个订阅者
int ZnpMsgPost(stZnpMsg *pZnpMsg)
{
	int Ret=0;
	char isMatch=0;
	stZnpMsgSubsHdl *pScan=NULL;
	unsigned char TransID;

	if(pZnpMsg==NULL){
		return -1;
	}

	//寻找消息的订阅者
	pthread_mutex_lock(&GlobalZnpStatus.mutexGlobalZnpMsgSubsHdlList);
	pScan=GlobalZnpStatus.pGlobalZnpMsgSubsHdlList;
	while(1){
		if(pScan==NULL){
			break;
		}
		if(pScan->MsgType==pZnpMsg->MsgType){
			if((pZnpMsg->MsgType==CMD_AF_DATA_REQUEST)||(pZnpMsg->MsgType==CMD_AF_DATA_REQUEST_EXT)||(pZnpMsg->MsgType==CMD_AF_DATA_REQUEST_SRC_RTG)||(pZnpMsg->MsgType==CMD_AF_DATA_CONFIRM)){
				switch(pZnpMsg->MsgType){
				case CMD_AF_DATA_REQUEST:
					TransID=pZnpMsg->un.mAF_DATA_REQUEST.TransID;
					break;
				case CMD_AF_DATA_REQUEST_SRC_RTG:
					TransID=pZnpMsg->un.mAF_DATA_REQUEST_SRC_RTG.TransID;
					break;
				case CMD_AF_DATA_CONFIRM:
					TransID=pZnpMsg->un.mAF_DATA_CONFIRM.TransID;
					break;
				default:
					DEBUGLOG1(3,"Error, unsupported ZnpMsg type 0x%04X.\n",pZnpMsg->MsgType);
					break;
				}
				if((pScan->isTransIDNeeded)&&(pScan->TransID==TransID)){
					isMatch=1;
					break;
				}else{
					if(pScan->isTransIDNeeded){
						DEBUGLOG2(0,"PendingTransID != IncomingTransID, 0x%04X != 0x%04X.\n",pScan->TransID,TransID);
					}
				}
			}else{
				isMatch=1;
				break;
			}
		}
		pScan=pScan->pNext;
	}
	if(isMatch&&(pScan!=NULL)){
		if(pScan->isMsgValid!=0){
			DEBUGLOG0(2,"Warning, discarding duplicated ZnpMsg.\n");
			DelocateZnpMsg(pZnpMsg);
			Ret=1;
		}else{
			pthread_mutex_lock(&pScan->m);
			pScan->pZnpMsg=pZnpMsg;
			pScan->isMsgValid=1;
			pthread_cond_signal(&pScan->c);
			pthread_mutex_unlock(&pScan->m);
			Ret=1;
		}
	}
	pthread_mutex_unlock(&GlobalZnpStatus.mutexGlobalZnpMsgSubsHdlList);
	return Ret;
}

//退订消息
void ZnpMsgUnsubscribe(stZnpMsgSubsHdl *pZnpMsgSubsHdl)
{
	int rt;

	if(pZnpMsgSubsHdl==NULL){
		return;
	}
	if(GlobalZnpStatus.pGlobalZnpMsgSubsHdlList==NULL){//This should not happen when pZnpMsgSubsHdl!=NULL.
		return;
	}

	pthread_mutex_lock(&GlobalZnpStatus.mutexGlobalZnpMsgSubsHdlList);
	if(pZnpMsgSubsHdl==GlobalZnpStatus.pGlobalZnpMsgSubsHdlList){//如果是队首
		GlobalZnpStatus.pGlobalZnpMsgSubsHdlList=GlobalZnpStatus.pGlobalZnpMsgSubsHdlList->pNext;
		if(GlobalZnpStatus.pGlobalZnpMsgSubsHdlList!=NULL){
			GlobalZnpStatus.pGlobalZnpMsgSubsHdlList->pPrev=NULL;
		}
	}else if(pZnpMsgSubsHdl->pNext==NULL){//如果是队尾
		pZnpMsgSubsHdl->pPrev->pNext=NULL;
	}else{//如果是中间元素
		pZnpMsgSubsHdl->pPrev->pNext=pZnpMsgSubsHdl->pNext;
		pZnpMsgSubsHdl->pNext->pPrev=pZnpMsgSubsHdl->pPrev;
	}

	//释放申请的系统资源
	rt=pthread_mutex_destroy(&pZnpMsgSubsHdl->m);
	if(rt!=0){
		DEBUGLOG1(3,"Warning, pthread_mutex_destroy failed %d.\n",rt);
	}
	rt=pthread_cond_destroy(&pZnpMsgSubsHdl->c);
	if(rt!=0){
		DEBUGLOG1(3,"Warning, pthread_cond_destroy failed %d.\n",rt);
	}
	pthread_mutex_unlock(&GlobalZnpStatus.mutexGlobalZnpMsgSubsHdlList);

	//销毁其中可能存在的消息,pend超时返回到消息退订之间的时间空隙可能碰巧会有消息到来
	if(pZnpMsgSubsHdl->isMsgValid){
		DelocateZnpMsg(pZnpMsgSubsHdl->pZnpMsg);
	}
	free(pZnpMsgSubsHdl);
}

//该线程将ZnpMsg队列中的消息逐一取出后进行分发处理
void *Thread_ZNPMsgHandler(void *pArg)
{
	unsigned char EP;
	int temp,rt;
	stZnpMsg *pZnpMsg=NULL;
	int isDelocateMsg=0;

	while(1){
		//取出消息
		pZnpMsg=ZnpMsgFIFOPop(GlobalZnpStatus.pZnpMsgFIFO_COM);//没有消息就阻塞在此
		if(pZnpMsg==NULL){
			DEBUGLOG0(3,"Error, ZnpMsgFIFOPop failed.\n");
			sleep(5);
			continue;
		}
		//DEBUGLOG1(1,"Get a ZnpMsg 0x%04X.\n",pZnpMsg->MsgType);
		//处理消息
		temp=ZnpMsgPost(pZnpMsg);//尝试查找消息订阅者，如果有，则向订阅者寄出消息
		if(temp<0){
			DEBUGLOG1(2,"Warning, ZnpMsgPost failed %d.\n",temp);
			isDelocateMsg=1;
		}
		//DEBUGLOG1(1,"ZnpMsgPost returned %d.\n",temp);
		if(temp==0){//消息无订阅者，对消息分情况处理
			switch(pZnpMsg->MsgType){
			//与EndPoint有关
			case CMD_AF_INCOMING_MSG:
				EP=pZnpMsg->un.mAF_INCOMING_MSG.DestEndpoint;
				if(GlobalZnpStatus.ppZnpMsgFIFO_EP[EP]!=NULL){
					//DEBUGLOG1(0,"Push to EP %d.\n",EP);
					rt=ZnpMsgFIFOPush(GlobalZnpStatus.ppZnpMsgFIFO_EP[EP],pZnpMsg);
					if(rt!=0){
						DEBUGLOG1(3,"Error, ZnpMsgFIFOPush failed %d.\n",rt);
						isDelocateMsg=1;
					}
				}else{//如果EP还没有注册则进行默认处理
					//DEBUGLOG1(0,"Push unregisted EP msg(to EP %d) to ZnpMsgFIFO_Default.\n",EP);
					if(GlobalZnpStatus.pZnpMsgFIFO_Default!=NULL){
						rt=ZnpMsgFIFOPush(GlobalZnpStatus.pZnpMsgFIFO_Default,pZnpMsg);
						if(rt!=0){
							DEBUGLOG1(3,"Error, ZnpMsgFIFOPush failed %d.\n",rt);
							isDelocateMsg=1;
						}
					}
				}
				break;
			case CMD_ZDO_STATE_CHANGE_IND:
				GlobalZnpStatus.ZDOState=pZnpMsg->un.mZDO_STATE_CHANGE_IND.State;
				isDelocateMsg=1;
				break;
			default:
				if(GlobalZnpStatus.pZnpMsgFIFO_Default!=NULL){
					rt=ZnpMsgFIFOPush(GlobalZnpStatus.pZnpMsgFIFO_Default,pZnpMsg);
					if(rt!=0){
						DEBUGLOG1(3,"Error, ZnpMsgFIFOPush failed %d.\n",rt);
						isDelocateMsg=1;
					}
				}
				break;
			}
		}

		//将已处理消息从队列中剔除
		if(isDelocateMsg){//有些消息已经不需要了，直接销毁
			DelocateZnpMsg(pZnpMsg);
			isDelocateMsg=0;
		}
		pZnpMsg=NULL;

		//马上继续
		continue;
	}

	return NULL;
}



