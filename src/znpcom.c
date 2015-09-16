/*
 * znpcom.c
 *
 *  Created on: 2014-2-11
 *      Author: chengm
 */

#include <stdio.h>      /*标准输入输出定义*/
#include <stdlib.h>     /*标准函数库定义*/
#include <memory.h>
#include <unistd.h>     /*Unix 标准函数定义*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>      /*文件控制定义*/
#include <errno.h>      /*错误号定义*/
#include <pthread.h>
#include <sys/time.h>
#include "debuglog.h"
#include "znpglobal.h"
#include "znpframe.h"
#include "znpmsg.h"
#include "znpcomhal.h"
#include "znpcom.h"
#include "znplib.h"
#include "znpmsgfifo.h"

//串口接收缓存，环形buffer可以应对命令跨帧的情况
#define RCVBUFFSIZE		(100*MAXFZNPRAMELEN)
#define CIRCLEBUFFLEN 	(300*MAXFZNPRAMELEN)
static unsigned char CircleBuff[CIRCLEBUFFLEN];
static unsigned int CircleRead=0,CircleWrite=0;//CircleWrite指向下一个将要写入的位置

void fZNPFrameProcess(unsigned char *pBuff)
{
	int rt;

	stZNPFrame *pZNPFrame;
	stZnpMsg *pZnpMsg=NULL;

	pZNPFrame = (stZNPFrame *)pBuff;

	//形成stZnpMsg数据结构l
	pZnpMsg=AllocateZnpMsg();
	if(pZnpMsg==NULL){
		DEBUGLOG0(3,"Error, AllocateZnpMsg failed, no Znp msg could be submitted.\n");
		return;
	}
	rt=TransZnpFrameToZnpMsg(pZNPFrame,pZnpMsg);
	if(rt!=0){
		DEBUGLOG0(3,"Error, TransZnpFrameToZnpMsg failed, can't create ZnpMsg from ZnpFrame.\n");
		DelocateZnpMsg(pZnpMsg);
		return;
	}

//	//打印ZnpFrame
//	{
//		int i;
//		DEBUGLOG0(0," ");
//		for(i=0;i<GETLEN_ZNPFRAME(pZNPFrame);i++){
//			printf("%02X ",((unsigned char *)pZNPFrame)[i]);
//		}
//		printf("\n");
//	}

	//Submit
	//DEBUGLOG1(3,"Push ZnpMsg 0x%04X.\n",pZnpMsg->MsgType);
	rt=ZnpMsgFIFOPush(GlobalZnpStatus.pZnpMsgFIFO_COM,pZnpMsg);
	if(rt!=0){
		DEBUGLOG1(3,"Error, ZnpMsgFIFOPush failed %d, can't submit ZnpMsg.\n",rt);
	}
}

//计算pBuff的FCS校验值
unsigned char fCalcFCS(unsigned char *pBuff, int Len)
{
	int i;
	unsigned char temp;

	temp = pBuff[0];
	for(i=1;i<Len;i++){
		temp = temp ^ pBuff[i];
	}

	return temp;
}

void fBuffProcess(unsigned char *pBuff, int DataLen)
{
	int i;
	unsigned int temp,ValidDataLen,FrameLen,FrameDataLen;
	unsigned char pZNPFrame[MAXFZNPRAMELEN];
	unsigned char fCalcFCS(unsigned char *pBuff, int Len);

	//存入环形buffer，目的是处理命令没有接收完整的情况以及避免越界问题
	for(i=0;i<DataLen;i++){
		CircleBuff[(CircleWrite+i)%CIRCLEBUFFLEN] = pBuff[i];
	}
	CircleWrite = (CircleWrite+i)%CIRCLEBUFFLEN;

	//处理
	while(CircleRead != CircleWrite){
		//找帧头
		if(CircleBuff[CircleRead] != ZNP_SOF){//每次处理完一个命令之后，pCircleRead必然指向下一条命令的帧头
			DEBUGLOG1(0,"Skipping bad data1 0x%02X (Bad SOF).\n",CircleBuff[CircleRead]);
			CircleRead = (CircleRead+1)%CIRCLEBUFFLEN;
			continue;
		}

		//长度检查
		//如果环形FIFO中的可用数据不满足最少的长度要求，则需要等待
		ValidDataLen=(CircleWrite>CircleRead)?(CircleWrite-CircleRead):(CircleWrite+CIRCLEBUFFLEN-CircleRead);
		if(ValidDataLen<5){
			return;
		}
		//帧中标记的数据长度是否合法
		FrameDataLen=CircleBuff[(CircleRead+1)%CIRCLEBUFFLEN];
		if(FrameDataLen>250){
			DEBUGLOG2(0,"Skipping bad data2 0x%02X (Bad frame data len %d>250).\n",CircleBuff[CircleRead],FrameDataLen);
			CircleRead = (CircleRead+1)%CIRCLEBUFFLEN;
			continue;
		}
		//可用数据长度是否至少达到了帧中标记的数据长度，如果不满足，则需要等待
		FrameLen=FrameDataLen+5;
		if(ValidDataLen<FrameLen){
			return;
		}

		//从环形buffer中提取一条命令
		for(i=0;i<FrameLen;i++){
			pZNPFrame[i] = CircleBuff[(CircleRead+i)%CIRCLEBUFFLEN];
		}

		//校验确认
		temp = fCalcFCS(&pZNPFrame[1],((unsigned int)pZNPFrame[1])+3);
		if((unsigned char)temp != pZNPFrame[pZNPFrame[1]+4]){//校验失败
			DEBUGLOG3(0,"Skipping bad data3 0x%02X (Bad crc, 0x%02X!=0x%02X).\n",CircleBuff[CircleRead],temp,pZNPFrame[pZNPFrame[1]+4]);
			CircleRead = (CircleRead+1)%CIRCLEBUFFLEN;
			continue;
		}

		//更新
		CircleRead = (CircleRead + pZNPFrame[1] + 5)%CIRCLEBUFFLEN;

		//开始处理
		fZNPFrameProcess(pZNPFrame);
	}
}

void *Thread_ZNPComRcvServer(void *pArg)
{
	int rt;
	unsigned int nread;
	unsigned char buff[RCVBUFFSIZE];

	while(1){
		//接收数据
		nread = RCVBUFFSIZE;
		rt = ZNPComRcv(&nread,buff);
		if(rt != 0){
			DEBUGLOG1(3,"ZNPComRcv error.(%d)\n",rt);
		}

		if(nread > 0){
//			//打印串口数据
//			{
//				int i;
//
//				DEBUGLOG0(0," ");
//				for(i=0;i<nread;i++){
//					printf("%02X ",buff[i]);
//				}
//				printf("\n");
//			}

			fBuffProcess(buff,(int)nread);//process data
		}
	}

	return NULL;
}

//发送一个ZNPFrame到通信口
//return 0-成功、else-失败
int ZNPComSendZNPFrame(stZNPFrame *pZNPFrame)
{
	int rt;

	if(pZNPFrame==NULL){
		return -1;
	}

	rt=ZNPComSend(GETLEN_ZNPFRAME(pZNPFrame),(unsigned char *)pZNPFrame);
	if(rt!=0){
		return -2;
	}else{
		return 0;
	}
}




