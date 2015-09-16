/*
 * znpapi_zdo.c
 *
 *  Created on: 2014-2-14
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

int Znp_ZDO_IEEE_ADDR_REQ(unsigned short ShortAddr,unsigned char ReqType,unsigned char StartIndex,stZDO_IEEE_ADDR_RSP *pRetVal)
{
	int rt,ret=0;
	stZnpMsg ZnpMsg_Req;
	stZnpCommonRsp *pZnpCommonRsp=NULL;

	//参数检查
	if(pRetVal==NULL){
		return -1;
	}
	if((ReqType!=0)&&(ReqType!=1)){
		return -1;
	}
	//创建请求消息
	ZnpMsg_Req.MsgType=CMD_ZDO_IEEE_ADDR_REQ;
	ZnpMsg_Req.un.mZDO_IEEE_ADDR_REQ.ShortAddr=ShortAddr;
	ZnpMsg_Req.un.mZDO_IEEE_ADDR_REQ.ReqType=ReqType;
	ZnpMsg_Req.un.mZDO_IEEE_ADDR_REQ.StartIndex=StartIndex;
	//申请例程句柄
	pZnpCommonRsp=AllocateZnpCommonRsp();
	if(pZnpCommonRsp==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpCommonRsp failed.\n");
		return -2;
	}
	//执行请求例程
	rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_ZDO_IEEE_ADDR_REQ_R, CMD_ZDO_IEEE_ADDR_RSP, 0, 0, pZnpCommonRsp);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
		ret=-3;
	}else{
		if(pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_IEEE_ADDR_REQ_R.Status!=0){
			DEBUGLOG1(2,"Warning, SRSP status indicated a failure %d.\n",pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_IEEE_ADDR_REQ_R.Status);
			ret=1;
		}else{//ARSP
			if(pZnpCommonRsp->pZnpARSPMsg->un.mZDO_IEEE_ADDR_RSP.Status!=0){
				DEBUGLOG1(2,"Warning, ZDO_IEEE_ADDR_RSP from target device indicated a failure %d.\n",pZnpCommonRsp->pZnpARSPMsg->un.mZDO_IEEE_ADDR_RSP.Status);
				ret=2;
			}else{
				*pRetVal=pZnpCommonRsp->pZnpARSPMsg->un.mZDO_IEEE_ADDR_RSP;
			}
		}
	}
	//销毁例程句柄
	DelocateZnpCommonRsp(pZnpCommonRsp, 1);

	return ret;
}

int Znp_ZDO_ACTIVE_EP_REQ(unsigned short DstAddr, unsigned short NWKAddrOfInterest, stZDO_ACTIVE_EP_RSP *pRetVal)
{
	int rt,ret=0;
	stZnpMsg ZnpMsg_Req;
	stZnpCommonRsp *pZnpCommonRsp=NULL;

	//参数检查
	if(pRetVal==NULL){
		return -1;
	}
	//创建请求消息
	ZnpMsg_Req.MsgType=CMD_ZDO_ACTIVE_EP_REQ;
	ZnpMsg_Req.un.mZDO_ACTIVE_EP_REQ.DstAddr=DstAddr;
	ZnpMsg_Req.un.mZDO_ACTIVE_EP_REQ.NWKAddrOfInterest=NWKAddrOfInterest;
	//申请例程句柄
	pZnpCommonRsp=AllocateZnpCommonRsp();
	if(pZnpCommonRsp==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpCommonRsp failed.\n");
		return -2;
	}
	//执行请求例程
	rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_ZDO_ACTIVE_EP_REQ_R, CMD_ZDO_ACTIVE_EP_RSP, 0, 0, pZnpCommonRsp);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
		ret=-3;
	}else{
		if(pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_ACTIVE_EP_REQ_R.Status!=0){
			DEBUGLOG1(2,"Warning, SRSP status indicated a failure %d.\n",pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_ACTIVE_EP_REQ_R.Status);
			ret=1;
		}else{//ARSP
			if(pZnpCommonRsp->pZnpARSPMsg->un.mZDO_ACTIVE_EP_RSP.Status!=0){
				DEBUGLOG1(2,"Warning, ZDO_ACTIVE_EP_RSP from target device indicated a failure %d.\n",pZnpCommonRsp->pZnpARSPMsg->un.mZDO_ACTIVE_EP_RSP.Status);
				ret=2;
			}else{
				*pRetVal=pZnpCommonRsp->pZnpARSPMsg->un.mZDO_ACTIVE_EP_RSP;
			}
		}
	}
	//销毁例程句柄
	DelocateZnpCommonRsp(pZnpCommonRsp, 1);

	return ret;
}

int Znp_ZDO_NODE_DESC_REQ(unsigned short DstAddr, unsigned short NWKAddrOfInterest, stZDO_NODE_DESC_RSP *pRetVal)
{
	int rt,ret=0;
	stZnpMsg ZnpMsg_Req;
	stZnpCommonRsp *pZnpCommonRsp=NULL;

	//参数检查
	if(pRetVal==NULL){
		return -1;
	}
	//创建请求消息
	ZnpMsg_Req.MsgType=CMD_ZDO_NODE_DESC_REQ;
	ZnpMsg_Req.un.mZDO_NODE_DESC_REQ.DstAddr=DstAddr;
	ZnpMsg_Req.un.mZDO_NODE_DESC_REQ.NWKAddrOfInterest=NWKAddrOfInterest;
	//申请例程句柄
	pZnpCommonRsp=AllocateZnpCommonRsp();
	if(pZnpCommonRsp==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpCommonRsp failed.\n");
		return -2;
	}
	//执行请求例程
	rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_ZDO_NODE_DESC_REQ_R, CMD_ZDO_NODE_DESC_RSP, 0, 0, pZnpCommonRsp);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
		ret=-3;
	}else{
		if(pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_NODE_DESC_REQ_R.Status!=0){
			DEBUGLOG1(2,"Warning, SRSP status indicated a failure %d.\n",pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_NODE_DESC_REQ_R.Status);
			ret=1;
		}else{//ARSP
			if(pZnpCommonRsp->pZnpARSPMsg->un.mZDO_NODE_DESC_RSP.Status!=0){
				DEBUGLOG1(2,"Warning, ZDO_NODE_DESC_RSP from target device indicated a failure %d.\n",pZnpCommonRsp->pZnpARSPMsg->un.mZDO_NODE_DESC_RSP.Status);
				ret=2;
			}else{
				*pRetVal=pZnpCommonRsp->pZnpARSPMsg->un.mZDO_NODE_DESC_RSP;
			}
		}
	}
	//销毁例程句柄
	DelocateZnpCommonRsp(pZnpCommonRsp, 1);

	return ret;
}

int Znp_ZDO_SIMPLE_DESC_REQ(unsigned short DstAddr, unsigned short NWKAddrOfInterest, unsigned char Endpoint, stZDO_SIMPLE_DESC_RSP *pRetVal)
{
	int rt,ret=0;
	stZnpMsg ZnpMsg_Req;
	stZnpCommonRsp *pZnpCommonRsp=NULL;

	//参数检查
	if(pRetVal==NULL){
		return -1;
	}
	//创建请求消息
	ZnpMsg_Req.MsgType=CMD_ZDO_SIMPLE_DESC_REQ;
	ZnpMsg_Req.un.mZDO_SIMPLE_DESC_REQ.DstAddr=DstAddr;
	ZnpMsg_Req.un.mZDO_SIMPLE_DESC_REQ.NWKAddrOfInterest=NWKAddrOfInterest;
	ZnpMsg_Req.un.mZDO_SIMPLE_DESC_REQ.Endpoint=Endpoint;
	//申请例程句柄
	pZnpCommonRsp=AllocateZnpCommonRsp();
	if(pZnpCommonRsp==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpCommonRsp failed.\n");
		return -2;
	}
	//执行请求例程
	rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_ZDO_SIMPLE_DESC_REQ_R, CMD_ZDO_SIMPLE_DESC_RSP, 0, 0, pZnpCommonRsp);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
		ret=-3;
	}else{
		if(pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_SIMPLE_DESC_REQ_R.Status!=0){
			DEBUGLOG1(2,"Warning, SRSP status indicated a failure %d.\n",pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_SIMPLE_DESC_REQ_R.Status);
			ret=1;
		}else{//ARSP
			if(pZnpCommonRsp->pZnpARSPMsg->un.mZDO_SIMPLE_DESC_RSP.Status!=0){
				DEBUGLOG1(2,"Warning, ZDO_SIMPLE_DESC_RSP from target device indicated a failure %d.\n",pZnpCommonRsp->pZnpARSPMsg->un.mZDO_SIMPLE_DESC_RSP.Status);
				ret=2;
			}else{
				*pRetVal=pZnpCommonRsp->pZnpARSPMsg->un.mZDO_SIMPLE_DESC_RSP;
			}
		}
	}
	//销毁例程句柄
	DelocateZnpCommonRsp(pZnpCommonRsp, 1);

	return ret;
}

int Znp_ZDO_USER_DESC_REQ(unsigned short DstAddr, unsigned short NWKAddrOfInterest, stZDO_USER_DESC_RSP *pRetVal)
{
	int rt,ret=0;
	stZnpMsg ZnpMsg_Req;
	stZnpCommonRsp *pZnpCommonRsp=NULL;

	//参数检查
	if(pRetVal==NULL){
		return -1;
	}
	//创建请求消息
	ZnpMsg_Req.MsgType=CMD_ZDO_USER_DESC_REQ;
	ZnpMsg_Req.un.mZDO_USER_DESC_REQ.DstAddr=DstAddr;
	ZnpMsg_Req.un.mZDO_USER_DESC_REQ.NWKAddrOfInterest=NWKAddrOfInterest;
	//申请例程句柄
	pZnpCommonRsp=AllocateZnpCommonRsp();
	if(pZnpCommonRsp==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpCommonRsp failed.\n");
		return -2;
	}
	//执行请求例程
	rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_ZDO_USER_DESC_REQ_R, CMD_ZDO_USER_DESC_RSP, 0, 0, pZnpCommonRsp);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
		ret=-3;
	}else{
		if(pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_USER_DESC_REQ_R.Status!=0){
			DEBUGLOG1(2,"Warning, SRSP status indicated a failure %d.\n",pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_USER_DESC_REQ_R.Status);
			ret=1;
		}else{//ARSP
			if(pZnpCommonRsp->pZnpARSPMsg->un.mZDO_USER_DESC_RSP.Status!=0){
				DEBUGLOG1(2,"Warning, ZDO_USER_DESC_RSP from target device indicated a failure %d.\n",pZnpCommonRsp->pZnpARSPMsg->un.mZDO_USER_DESC_RSP.Status);
				ret=2;
			}else{
				*pRetVal=pZnpCommonRsp->pZnpARSPMsg->un.mZDO_USER_DESC_RSP;
			}
		}
	}
	//销毁例程句柄
	DelocateZnpCommonRsp(pZnpCommonRsp, 1);

	return ret;
}

int Znp_ZDO_USER_DESC_SET(unsigned short DstAddr, unsigned short NWKAddrOfInterest, unsigned char Len, char *pUserDescriptor)
{
	int rt,ret=0;
	stZnpMsg ZnpMsg_Req;
	stZnpCommonRsp *pZnpCommonRsp=NULL;

	//参数检查
	if(Len>16){
		return -2;
	}
	//创建请求消息
	ZnpMsg_Req.MsgType=CMD_ZDO_USER_DESC_SET;
	ZnpMsg_Req.un.mZDO_USER_DESC_SET.DstAddr=DstAddr;
	ZnpMsg_Req.un.mZDO_USER_DESC_SET.NWKAddrOfInterest=NWKAddrOfInterest;
	ZnpMsg_Req.un.mZDO_USER_DESC_SET.Len=Len;
	memcpy(ZnpMsg_Req.un.mZDO_USER_DESC_SET.UserDescriptor,pUserDescriptor,Len);
	//申请例程句柄
	pZnpCommonRsp=AllocateZnpCommonRsp();
	if(pZnpCommonRsp==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpCommonRsp failed.\n");
		return -2;
	}
	//执行请求例程
	rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_ZDO_USER_DESC_SET_R, CMD_ZDO_USER_DESC_CONF, 0, 0, pZnpCommonRsp);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
		ret=-3;
	}else{
		if(pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_USER_DESC_SET_R.Status!=0){
			DEBUGLOG1(2,"Warning, SRSP status indicated a failure %d.\n",pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_USER_DESC_SET_R.Status);
			ret=1;
		}else{//ARSP
			if(pZnpCommonRsp->pZnpARSPMsg->un.mZDO_USER_DESC_CONF.Status!=0){
				DEBUGLOG1(2,"Warning, ZDO_USER_DESC_CONF from target device indicated a failure %d.\n",pZnpCommonRsp->pZnpARSPMsg->un.mZDO_USER_DESC_CONF.Status);
				ret=2;
			}
		}
	}
	//销毁例程句柄
	DelocateZnpCommonRsp(pZnpCommonRsp, 1);

	return ret;
}

int Znp_ZDO_MGMT_RTG_REQ(unsigned short DstAddr,unsigned char StartIndex,stZDO_MGMT_RTG_RSP *pRetVal)
{
	int rt,ret=0;
	stZnpMsg ZnpMsg_Req;
	stZnpCommonRsp *pZnpCommonRsp=NULL;

	//参数检查
	if(pRetVal==NULL){
		return -1;
	}
	//创建请求消息
	ZnpMsg_Req.MsgType=CMD_ZDO_MGMT_RTG_REQ;
	ZnpMsg_Req.un.mZDO_MGMT_RTG_REQ.DstAddr=DstAddr;
	ZnpMsg_Req.un.mZDO_MGMT_RTG_REQ.StartIndex=StartIndex;
	//申请例程句柄
	pZnpCommonRsp=AllocateZnpCommonRsp();
	if(pZnpCommonRsp==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpCommonRsp failed.\n");
		return -2;
	}
	//执行请求例程
	rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_ZDO_MGMT_RTG_REQ_R, CMD_ZDO_MGMT_RTG_RSP, 0, 0, pZnpCommonRsp);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
		ret=-3;
	}else{
		if(pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_MGMT_RTG_REQ_R.Status!=0){
			DEBUGLOG1(2,"Warning, SRSP status indicated a failure %d.\n",pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_MGMT_RTG_REQ_R.Status);
			ret=1;
		}else{//ARSP
			if(pZnpCommonRsp->pZnpARSPMsg->un.mZDO_MGMT_RTG_RSP.Status!=0){
				DEBUGLOG1(2,"Warning, ZDO_MGMT_RTG_RSP from target device indicated a failure %d.\n",pZnpCommonRsp->pZnpARSPMsg->un.mZDO_MGMT_RTG_RSP.Status);
				ret=2;
			}else{
				*pRetVal=pZnpCommonRsp->pZnpARSPMsg->un.mZDO_MGMT_RTG_RSP;
			}
		}
	}
	//销毁例程句柄
	DelocateZnpCommonRsp(pZnpCommonRsp, 1);

	return ret;
}

int Znp_ZDO_MGMT_LEAVE_REQ(unsigned short DstAddr,unsigned char *pDeviceAddress,unsigned char RemoveChildren_Rejoin,int doWaitForRsp)
{
	int rt,ret=0;
	stZnpMsg ZnpMsg_Req;
	stZnpCommonRsp *pZnpCommonRsp=NULL;

	//参数检查
	if(pDeviceAddress==NULL){
		return -1;
	}
	//创建请求消息
	ZnpMsg_Req.MsgType=CMD_ZDO_MGMT_LEAVE_REQ;
	ZnpMsg_Req.un.mZDO_MGMT_LEAVE_REQ.DstAddr=DstAddr;
	memcpy(ZnpMsg_Req.un.mZDO_MGMT_LEAVE_REQ.DeviceAddr,pDeviceAddress,8);
	ZnpMsg_Req.un.mZDO_MGMT_LEAVE_REQ.RemoveChildren_Rejoin=RemoveChildren_Rejoin;
	//申请例程句柄
	pZnpCommonRsp=AllocateZnpCommonRsp();
	if(pZnpCommonRsp==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpCommonRsp failed.\n");
		return -2;
	}
	//执行请求例程
	if(doWaitForRsp){
		rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_ZDO_MGMT_LEAVE_REQ_R, CMD_ZDO_MGMT_LEAVE_RSP, 0, 0, pZnpCommonRsp);
		if(rt!=0){
			DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
			ret=-3;
		}else{
			if(pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_MGMT_LEAVE_REQ_R.Status!=0){
				DEBUGLOG1(2,"Warning, SRSP status indicated a failure %d.\n",pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_MGMT_LEAVE_REQ_R.Status);
				ret=1;
			}else{//ARSP
				if(pZnpCommonRsp->pZnpARSPMsg->un.mZDO_MGMT_LEAVE_RSP.Status!=0){
					DEBUGLOG1(2,"Warning, ZDO_MGMT_LEAVE_RSP from target device indicated a failure %d.\n",pZnpCommonRsp->pZnpARSPMsg->un.mZDO_MGMT_LEAVE_RSP.Status);
					ret=2;
				}
			}
		}
	}else{
		rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_ZDO_MGMT_LEAVE_REQ_R, 0, 0, 0, pZnpCommonRsp);
		if(rt!=0){
			DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
			ret=-3;
		}
	}
	//销毁例程句柄
	DelocateZnpCommonRsp(pZnpCommonRsp, 1);

	return ret;
}

//DstAddr - 指定发起绑定的设备地址
//pSrcAddress - 绑定的源地址，64bit地址
//SrcEndpoint - 绑定的源EP
//ClusterID - 绑定的Cluster
//DstAddrMode - 目的地址模式（实验表明只支持ADDRESS_64_BIT） ADDRESS_NOT_PRESENT, GROUP_ADDRESS, ADDRESS_16_BIT, ADDRESS_64_BIT, BROADCAST
//pDstAddress - 绑定的目的地址,长度必须为8字节
//DstEndpoint - 绑定的目的EP
//return 0--成功，else--失败
int Znp_ZDO_BIND_REQ(unsigned short DstAddr,unsigned char *pSrcAddress,unsigned char SrcEndpoint,unsigned short ClusterID,unsigned char DstAddrMode,unsigned char *pDstAddress,unsigned char DstEndpoint)
{
	int rt,ret=0;
	stZnpMsg ZnpMsg_Req;
	stZnpCommonRsp *pZnpCommonRsp=NULL;

	//参数检查
	if((pSrcAddress==NULL)||(pDstAddress==NULL)){
		return -1;
	}
	//创建请求消息
	memset(&ZnpMsg_Req,0,sizeof(stZnpMsg));
	ZnpMsg_Req.MsgType=CMD_ZDO_BIND_REQ;
	ZnpMsg_Req.un.mZDO_BIND_REQ.DstAddr=DstAddr;
	memcpy(ZnpMsg_Req.un.mZDO_BIND_REQ.SrcAddress,pSrcAddress,8);
	ZnpMsg_Req.un.mZDO_BIND_REQ.SrcEndpoint=SrcEndpoint;
	ZnpMsg_Req.un.mZDO_BIND_REQ.ClusterID=ClusterID;
	ZnpMsg_Req.un.mZDO_BIND_REQ.DstEndpoint=DstEndpoint;
	ZnpMsg_Req.un.mZDO_BIND_REQ.DstAddrMode=DstAddrMode;
	memcpy(ZnpMsg_Req.un.mZDO_BIND_REQ.DstAddress,pDstAddress,8);

	//申请例程句柄
	pZnpCommonRsp=AllocateZnpCommonRsp();
	if(pZnpCommonRsp==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpCommonRsp failed.\n");
		return -2;
	}
	//执行请求例程
	rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_ZDO_BIND_REQ_R, CMD_ZDO_BIND_RSP, 0, 0, pZnpCommonRsp);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
		ret=-3;
	}else{
		if(pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_BIND_REQ_R.Status!=0){
			DEBUGLOG1(2,"Warning, SRSP status indicated a failure %d.\n",pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_MGMT_LEAVE_REQ_R.Status);
			ret=1;
		}else{//ARSP
			if(pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_BIND_RSP.Status!=0){
				DEBUGLOG1(2,"Warning, ARSP indicate a failure of ZDO_BIND_REQ %d.\n",pZnpCommonRsp->pZnpSRSPMsg->un.mZDO_BIND_RSP.Status);
				ret=2;
			}
		}
	}
	//销毁例程句柄
	DelocateZnpCommonRsp(pZnpCommonRsp, 1);

	return ret;
}











