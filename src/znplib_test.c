/*
 * znplib_test.c
 *
 *  Created on: 2014-2-11
 *      Author: chengm
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include "znplib.h"

//#define ZNPCOMDEVPATH "/dev/ttySAC1"	//for arm linux
#define ZNPCOMDEVPATH "/dev/ttyS0"	//for ubuntu
//#define ZNPCOMDEVPATH "/dev/ttyAMA0" //for Rpi

void pCB_ZnpMsgDefaultCB(stZnpMsg *pZnpMsg)
{
	printf("pCB_ZnpMsgDefaultCB get a msg with type 0x%04X.\n",pZnpMsg->MsgType);
}

void pCB_ZnpMsgCBForAfEndpoint33(unsigned char EP, stZnpMsg *pZnpMsg)
{
	printf("pCB_ZnpMsgCBForAfEndpoint33 get a msg to EP %d.\n",pZnpMsg->un.mAF_INCOMING_MSG.DestEndpoint);
}

void pCB_ZnpMsgCBForAfEndpoint34(unsigned char EP, stZnpMsg *pZnpMsg)
{
	printf("pCB_ZnpMsgCBForAfEndpoint34 get a msg to EP %d.\n",pZnpMsg->un.mAF_INCOMING_MSG.DestEndpoint);
}

int main(int argc, char *argv[])
{
	int rt;
	stZnpCfg ZnpCfg;
	unsigned short TestDevAddr=0x0000;
	unsigned char CoodIEEEAddr[8];
	unsigned char TargetDevIEEEAddr[8]={0xf0,0xd8,0xcb,0x02,0x00,0x4b,0x12,0x00};

	ZnpCfg.pDefaultZnpMsgCB=pCB_ZnpMsgDefaultCB;


	ZnpCfg.pZNPComPath=ZNPCOMDEVPATH;
	rt=ZnpInit(&ZnpCfg);
	if(rt!=0){
		printf("Error, ZnpInit failed.\n");
	}

//	{
//		stSYS_SET_TX_POWER_R mSYS_SET_TX_POWER_R;
//
//		rt=Znp_SYS_SET_TX_POWER(128, &mSYS_SET_TX_POWER_R);
//		if(rt!=0){
//			printf("Error, Znp_SYS_SET_TX_POWER failed %d.\n",rt);
//		}
//	}
//
//	{
//		rt=Znp_ZB_PERMIT_JOINING(0x0000,0xFF);
//		if(rt!=0){
//			printf("Error, Znp_ZB_PERMIT_JOINING failed %d.\n",rt);
//		}
//	}

	{//找到本地的IEEE地址
		int len=8;
		rt=Znp_ZB_GET_DEVICE_INFO(1, &len, CoodIEEEAddr);
		if(rt!=0){
			printf("Error, Znp_ZB_GET_DEVICE_INFO failed %d.\n",rt);
		}
	}

	{//找到目标设备短地址
		stZB_FIND_DEVICE_CONFIRM mZB_FIND_DEVICE_CONFIRM;

		rt=Znp_ZB_FIND_DEVICE_REQUEST(TargetDevIEEEAddr, &mZB_FIND_DEVICE_CONFIRM);
		if(rt!=0){
			printf("Error, Znp_ZB_FIND_DEVICE_REQUEST failed %d.\n",rt);
		}else{
			memcpy(&TestDevAddr,mZB_FIND_DEVICE_CONFIRM.SearchKey,2);
		}
	}

//	{
//		stZDO_IEEE_ADDR_RSP mZDO_IEEE_ADDR_RSP;
//		rt=Znp_ZDO_IEEE_ADDR_REQ(TestDevAddr,1,0,&mZDO_IEEE_ADDR_RSP);
//		if(rt!=0){
//			printf("Error, Znp_ZDO_IEEE_ADDR_REQ failed %d.\n",rt);
//		}
//	}

	{
		rt=Znp_AF_REGISTER(33,0x0104,0,0,0,0,NULL,0,NULL,pCB_ZnpMsgCBForAfEndpoint33);
		if(rt!=0){
			printf("Error, Znp_AF_REGISTER failed %d.\n",rt);
		}
	}

	{//让目标设备绑定我们
		rt=Znp_ZDO_BIND_REQ(TestDevAddr,TargetDevIEEEAddr,30,0x0006,ADDRESS_64_BIT,CoodIEEEAddr,33);
		if(rt!=0){
			printf("Error, Znp_ZDO_BIND_REQ failed %d.\n",rt);
		}
	}

//	{
//		stZDO_NODE_DESC_RSP mZDO_NODE_DESC_RSP;
//		rt=Znp_ZDO_NODE_DESC_REQ(TestDevAddr, TestDevAddr, &mZDO_NODE_DESC_RSP);
//		if(rt!=0){
//			printf("Error, Znp_ZDO_NODE_DESC_REQ failed %d.\n",rt);
//		}
//	}
//
//	{
//		stZDO_ACTIVE_EP_RSP mZDO_ACTIVE_EP_RSP;
//
//		rt= Znp_ZDO_ACTIVE_EP_REQ(TestDevAddr, TestDevAddr, &mZDO_ACTIVE_EP_RSP);
//		if(rt!=0){
//			printf("Error, Znp_ZDO_ACTIVE_EP_REQ failed %d.\n",rt);
//		}
//	}
//
//	{
//		stZDO_SIMPLE_DESC_RSP mZDO_SIMPLE_DESC_RSP;
//
//		rt=Znp_ZDO_SIMPLE_DESC_REQ(TestDevAddr, TestDevAddr, 30, &mZDO_SIMPLE_DESC_RSP);
//		if(rt!=0){
//			printf("Error, Znp_ZDO_SIMPLE_DESC_REQ failed %d.\n",rt);
//		}
//	}
//
//	{
//		stZDO_USER_DESC_RSP mtZDO_USER_DESC_RSP;
//
//		rt=Znp_ZDO_USER_DESC_SET(TestDevAddr, TestDevAddr, 15, "This is a test.");
//		if(rt!=0){
//			printf("Error, Znp_ZDO_USER_DESC_SET failed %d.\n",rt);
//		}
//
//		rt=Znp_ZDO_USER_DESC_REQ(TestDevAddr, TestDevAddr, &mtZDO_USER_DESC_RSP);
//		if(rt!=0){
//			printf("Error, Znp_ZDO_USER_DESC_REQ failed %d.\n",rt);
//		}
//	}
//
//	{
//		stZDO_MGMT_RTG_RSP mZDO_MGMT_RTG_RSP;
//
//		rt=Znp_ZDO_MGMT_RTG_REQ(TestDevAddr,0,&mZDO_MGMT_RTG_RSP);
//		if(rt!=0){
//			printf("Error, Znp_ZDO_MGMT_RTG_REQ failed %d.\n",rt);
//		}
//	}

	{
		unsigned char Data[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
		unsigned char Len=16;

		rt=Znp_AF_DATA_REQUEST(TestDevAddr,30,33,0x0000,AF_ACK_REQUEST|AF_DISCV_ROUTE|AF_EN_SECURITY,7,Len,Data);
		if(rt!=0){
			printf("Error, Znp_AF_DATA_REQUEST failed %d.\n",rt);
		}
	}

//	{
//		unsigned char Data[10]={1,3,5,7,9,11,13,15,17,19};
//
//		rt=Znp_AF_DATA_REQUEST_SRC_RTG(TestDevAddr,30,33,0x0000,0x0001,AF_ACK_REQUEST|AF_DISCV_ROUTE|AF_EN_SECURITY,7,0,NULL,10,Data);
//		if(rt!=0){
//			printf("Error, Znp_AF_DATA_REQUEST_SRC_RTG failed %d.\n",rt);
//		}
//	}
//
//	{
//		unsigned char pIEEE[8]={0x6b,0xd9,0xcb,0x02,0x00,0x4b,0x12,0x00};
//
//		rt=Znp_ZDO_MGMT_LEAVE_REQ(TestDevAddr,pIEEE,0);
//		if(rt!=0){
//			printf("Error, Znp_ZDO_MGMT_LEAVE_REQ failed %d.\n",rt);
//		}
//	}

	while(1){
		sleep(50);
	}

	return 0;
}

