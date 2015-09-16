/*
 * znpapi_simple.c
 *
 *  Created on: 2014-2-13
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

int Znp_ZB_START()
{
	int rt,ret=0;
	stZnpMsg ZnpMsg_Req;
	stZnpCommonRsp *pZnpCommonRsp=NULL;

	//创建请求消息
	ZnpMsg_Req.MsgType=CMD_ZB_START_REQUEST;

	//申请例程句柄
	pZnpCommonRsp=AllocateZnpCommonRsp();
	if(pZnpCommonRsp==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpCommonRsp failed.\n");
		return -2;
	}
	//执行请求例程
	rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_ZB_START_REQUEST_R, 0, 0, 0, pZnpCommonRsp);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
		ret=-3;
	}

	//销毁例程句柄
	DelocateZnpCommonRsp(pZnpCommonRsp, 1);

	return ret;
}

int Znp_ZB_PERMIT_JOINING(unsigned short Destination, unsigned char Timeout)
{
	int rt,ret=0;
	stZnpMsg ZnpMsg_Req;
	stZnpCommonRsp *pZnpCommonRsp=NULL;

	//创建请求消息
	ZnpMsg_Req.MsgType=CMD_ZB_PERMIT_JOINING_REQUEST;
	ZnpMsg_Req.un.mZB_PERMIT_JOINING_REQUEST.Destination=Destination;
	ZnpMsg_Req.un.mZB_PERMIT_JOINING_REQUEST.Timeout=Timeout;

	//申请例程句柄
	pZnpCommonRsp=AllocateZnpCommonRsp();
	if(pZnpCommonRsp==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpCommonRsp failed.\n");
		return -2;
	}
	//执行请求例程
	rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_ZB_PERMIT_JOINING_REQUEST_R, CMD_ZDO_MGMT_PERMIT_JOIN_RSP, 0, 0, pZnpCommonRsp);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
		ret=-3;
	}else{
		//处理应答消息
		if(pZnpCommonRsp->pZnpSRSPMsg->un.mZB_PERMIT_JOINING_REQUEST_R.Status!=0){//SRSP
			DEBUGLOG1(2,"Warning, SRSP status indicated a failure %d.\n",pZnpCommonRsp->pZnpSRSPMsg->un.mZB_PERMIT_JOINING_REQUEST_R.Status);
			ret=1;
		}else{//ARSP
			if(pZnpCommonRsp->pZnpARSPMsg->un.mZDO_MGMT_PERMIT_JOIN_RSP.Status!=0){
				DEBUGLOG1(2,"Warning, ZDO_MGMT_PERMIT_JOIN_RSP from target device indicated a failure %d.\n",pZnpCommonRsp->pZnpARSPMsg->un.mZDO_MGMT_PERMIT_JOIN_RSP.Status);
				ret=2;
			}else{
				if(pZnpCommonRsp->pZnpARSPMsg->un.mZDO_MGMT_PERMIT_JOIN_RSP.SrcAddr!=Destination){
					DEBUGLOG2(3,"Error, the SrcAddr of the incoming ZDO_MGMT_PERMIT_JOIN_RSP(0x%04X) does not match the ZB_PERMIT_JOINING_REQUEST (0x%04X).\n",pZnpCommonRsp->pZnpARSPMsg->un.mZDO_MGMT_PERMIT_JOIN_RSP.SrcAddr,Destination);
					ret=3;
				}
			}
		}
	}

	//销毁例程句柄
	DelocateZnpCommonRsp(pZnpCommonRsp, 1);

	return ret;
}

int Znp_ZB_GET_DEVICE_INFO(unsigned char Param, int *pLen, unsigned char *pValue)
{
	int rt,ret=0;
	stZnpMsg ZnpMsg_Req;
	stZnpCommonRsp *pZnpCommonRsp=NULL;

	//参数检查
	if((pLen==NULL)||(pValue==NULL)){
		return -1;
	}
	if(*pLen<8){
		return -1;
	}

	//创建请求消息
	ZnpMsg_Req.MsgType=CMD_ZB_GET_DEVICE_INFO;
	ZnpMsg_Req.un.mZB_GET_DEVICE_INFO.Param=Param;

	//申请例程句柄
	pZnpCommonRsp=AllocateZnpCommonRsp();
	if(pZnpCommonRsp==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpCommonRsp failed.\n");
		return -2;
	}
	//执行请求例程
	rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_ZB_GET_DEVICE_INFO_R, 0, 0, 0, pZnpCommonRsp);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
		ret=-3;
	}else{
		//处理应答消息
		if(pZnpCommonRsp->pZnpSRSPMsg->un.mZB_GET_DEVICE_INFO_R.Param!=Param){
			DEBUGLOG2(3,"Error, the Param of the incoming ZB_GET_DEVICE_INFO_R %d does not match the requested Param %d.\n",pZnpCommonRsp->pZnpSRSPMsg->un.mZB_GET_DEVICE_INFO_R.Param,Param);
			ret=1;
		}else{
			memcpy(pValue,pZnpCommonRsp->pZnpSRSPMsg->un.mZB_GET_DEVICE_INFO_R.Value,8);
		}
	}

	//销毁例程句柄
	DelocateZnpCommonRsp(pZnpCommonRsp, 1);

	return ret;
}

int Znp_ZB_FIND_DEVICE_REQUEST(unsigned char *pSearchKey, stZB_FIND_DEVICE_CONFIRM *pValue)
{
	int rt,ret=0;
	stZnpMsg ZnpMsg_Req;
	stZnpCommonRsp *pZnpCommonRsp=NULL;

	//参数检查
	if((pSearchKey==NULL)||(pValue==NULL)){
		return -1;
	}
	//创建请求消息
	ZnpMsg_Req.MsgType=CMD_ZB_FIND_DEVICE_REQUEST;
	memcpy(ZnpMsg_Req.un.mZB_FIND_DEVICE_REQUEST.SearchKey,pSearchKey,8);
	//申请例程句柄
	pZnpCommonRsp=AllocateZnpCommonRsp();
	if(pZnpCommonRsp==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpCommonRsp failed.\n");
		return -2;
	}
	//执行请求例程
	rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_ZB_FIND_DEVICE_REQUEST_R, CMD_ZB_FIND_DEVICE_CONFIRM, 0, 0, pZnpCommonRsp);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
		ret=-3;
	}else{
		//处理ARSP应答消息
		*pValue=pZnpCommonRsp->pZnpARSPMsg->un.mZB_FIND_DEVICE_CONFIRM;
	}

	//销毁例程句柄
	DelocateZnpCommonRsp(pZnpCommonRsp, 1);

	return ret;
}



















