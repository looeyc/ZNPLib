/*
 * znpapi_common.h
 *
 *  Created on: 2014-2-13
 *      Author: chengm
 */

#ifndef ZNPAPI_COMMON_H_
#define ZNPAPI_COMMON_H_

//应答消息组合结构
typedef struct{
	stZnpMsg *pZnpSRSPMsg;
	stZnpMsg *pZnpARSPMsg;
}stZnpCommonRsp;

//分配一个ZnpCommonRsp句柄
stZnpCommonRsp *AllocateZnpCommonRsp();
//销毁一个ZnpCommonRsp句柄，
//Option 选择是否销毁其中的消息数据，0-不销毁、1-销毁、其他取值保留
void DelocateZnpCommonRsp(stZnpCommonRsp *pZnpCommonRsp, int Option);

//执行请求，所有API都需要用到的流程
//pZnpReqMsg 需要执行（发送）的请求消息
//Srsp 要求的SRSP应答类型，0表示不需要SRSP应答
//Arsp 要求的ARSP应答类型，0表示不需要ARSP应答
//isArspTransIDNeeded - 0-ARSP不需要进行TransID匹配（其实只有极少的ARSP才有TransID）
//ArspTransID - 指定ARSP的TransID，当isArspTransIDNeeded为0时该参数忽略
//pZnpCommonRsp 调用者提供空间，用于返回SRSP、ARSP应答消息，如果请求不需要SRSP/ARSP，则返回的ZnpCommonRsp中，pZnpSRSPMsg/pZnpARSPMsg为NULL
//return 0-请求成功、1-SRSP等待超时、2-ARSP等待超时、else-出错
int ZnpAPICommon_ReqRoutine(stZnpMsg *pZnpReqMsg, unsigned short Srsp, unsigned short Arsp, unsigned char isArspTransIDNeeded, unsigned char ArspTransID, stZnpCommonRsp *pZnpCommonRsp);

#endif /* ZNPAPI_COMMON_H_ */
