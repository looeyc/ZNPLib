/*
 * znpapi_config.c
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

int Znp_ZB_WRITE_CONFIGURATION(unsigned char ConfigId, unsigned char Len, unsigned char *pValue)
{
	int rt,ret=0;
	stZnpMsg ZnpMsg_Req;
	stZnpCommonRsp *pZnpCommonRsp=NULL;

	//参数检查
	if(pValue == NULL){
		return -1;
	}
	if(Len>128){//超过允许的最大数据长度
		return -1;
	}

	//创建请求消息
	ZnpMsg_Req.MsgType=CMD_ZB_WRITE_CONFIGURATION;
	ZnpMsg_Req.un.mZB_WRITE_CONFIGURATION.ConfigId=ConfigId;
	ZnpMsg_Req.un.mZB_WRITE_CONFIGURATION.Len=Len;
	memcpy(ZnpMsg_Req.un.mZB_WRITE_CONFIGURATION.Value,pValue,Len);

	//申请例程句柄
	pZnpCommonRsp=AllocateZnpCommonRsp();
	if(pZnpCommonRsp==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpCommonRsp failed.\n");
		return -2;
	}
	//执行请求例程
	rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_ZB_WRITE_CONFIGURATION_R, 0, 0, 0, pZnpCommonRsp);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
		ret=-3;
	}else{
		//处理应答消息
		if(pZnpCommonRsp->pZnpSRSPMsg->un.mZB_WRITE_CONFIGURATION_R.Status!=0){
			DEBUGLOG1(2,"Warning, SRSP status indicated a failure %d.\n",pZnpCommonRsp->pZnpSRSPMsg->un.mZB_WRITE_CONFIGURATION_R.Status);
			ret=1;
		}
	}

	//销毁例程句柄
	DelocateZnpCommonRsp(pZnpCommonRsp, 1);

	return ret;
}

int Znp_ZB_READ_CONFIGURATION(unsigned char ConfigId, unsigned char *pLen, unsigned char *pValue)
{
	int rt,ret=0;
	stZnpMsg ZnpMsg_Req;
	stZnpCommonRsp *pZnpCommonRsp=NULL;

	//参数检查
	if((pLen==NULL)||(pValue == NULL)){
		return -1;
	}
	if(*pLen<=0){
		return -1;
	}

	//创建请求消息
	ZnpMsg_Req.MsgType=CMD_ZB_READ_CONFIGURATION;
	ZnpMsg_Req.un.mZB_READ_CONFIGURATION.ConfigId=ConfigId;

	//申请例程句柄
	pZnpCommonRsp=AllocateZnpCommonRsp();
	if(pZnpCommonRsp==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpCommonRsp failed.\n");
		return -2;
	}
	//执行请求例程
	rt=ZnpAPICommon_ReqRoutine(&ZnpMsg_Req, CMD_ZB_READ_CONFIGURATION_R, 0, 0, 0, pZnpCommonRsp);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZnpAPICommon_ReqRoutine failed %d.\n",rt);
		ret=-3;
	}else{
		//处理应答消息
		if(pZnpCommonRsp->pZnpSRSPMsg->un.mZB_READ_CONFIGURATION_R.Status!=0){
			DEBUGLOG1(2,"Warning, SRSP status indicated a failure %d.\n",pZnpCommonRsp->pZnpSRSPMsg->un.mZB_READ_CONFIGURATION_R.Status);
			ret=1;
		}else{
			if(*pLen<pZnpCommonRsp->pZnpSRSPMsg->un.mZB_READ_CONFIGURATION_R.Len){
				DEBUGLOG2(3,"Error, the lenth of pValue is not big enough too hold requried data.(%d<%d)",*pLen,pZnpCommonRsp->pZnpSRSPMsg->un.mZB_READ_CONFIGURATION_R.Len);
				ret=2;
			}else{
				*pLen=pZnpCommonRsp->pZnpSRSPMsg->un.mZB_READ_CONFIGURATION_R.Len;
				memcpy(pValue,pZnpCommonRsp->pZnpSRSPMsg->un.mZB_READ_CONFIGURATION_R.Value,pZnpCommonRsp->pZnpSRSPMsg->un.mZB_READ_CONFIGURATION_R.Len);
			}
		}
	}

	//销毁例程句柄
	DelocateZnpCommonRsp(pZnpCommonRsp, 1);

	return ret;
}






