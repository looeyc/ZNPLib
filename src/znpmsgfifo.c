/*
 * znpmsgfifo.c
 *
 *  Created on: 2014-2-20
 *      Author: chengm
 */
#include <memory.h>
#include <pthread.h>
#include "stdlib.h"
#include "debuglog.h"
#include "znpmsgfifo.h"

//创建一个ZnpMsg消息队列
//return 创建的消息队列句柄，NULL-创建失败
stZnpMsgFIFOHdl *ZnpMsgFIFOCreate()
{
	stZnpMsgFIFOHdl *pRet=NULL;

	pRet=(stZnpMsgFIFOHdl *)malloc(sizeof(stZnpMsgFIFOHdl));
	if(pRet==NULL){
		return NULL;
	}
	memset(pRet,0,sizeof(stZnpMsgFIFOHdl));
	pthread_mutex_init(&pRet->m,NULL);
	pthread_cond_init(&pRet->c,NULL);

	return pRet;
}

//往ZnpMsg消息队列中加入一条消息
//pZnpMsgFIFO - 消息队列句柄
//pZnpMsg - 需要加入的ZnpMsg消息
//return 0-成功，else-失败
//成功后，消息的指针被排入队列，消息数据被FIFO接管
int ZnpMsgFIFOPush(stZnpMsgFIFOHdl *pZnpMsgFIFO, stZnpMsg *pZnpMsg)
{
	stZnpMsgListItem *pScan=NULL;
	stZnpMsgListItem *pNewNode=NULL;

	if((pZnpMsgFIFO==NULL)||(pZnpMsg==NULL)){
		return -1;
	}

	//创建消息队列节点
	pNewNode=(stZnpMsgListItem *)malloc(sizeof(stZnpMsgListItem));
	if(pNewNode==NULL){
		DEBUGLOG0(3,"Error, malloc failed.\n");
		return -2;
	}
	memset(pNewNode,0,sizeof(stZnpMsgListItem));
	pNewNode->pZnpMsg=pZnpMsg;

	//加入队列
	pthread_mutex_lock(&pZnpMsgFIFO->m);
	pScan=pZnpMsgFIFO->pZnpMsgList;
	if(pScan==NULL){//队列中还没有消息
		pZnpMsgFIFO->pZnpMsgList=pNewNode;
	}else{//队列中已经有消息了
		while(pScan->pNext!=NULL){//找到队尾
			pScan=pScan->pNext;
		}
		pScan->pNext=pNewNode;
	}
	pZnpMsgFIFO->MsgNum++;
	//发出通知，有新消息到来
	pthread_cond_signal(&pZnpMsgFIFO->c);
	pthread_mutex_unlock(&pZnpMsgFIFO->m);

	return 0;
}

//从ZnpMsg消息队列中取出一条消息
//pZnpMsgFIFO  - 消息队列句柄
//return 取出的消息指针，NULL表示出错
//成功后，消息数据由调用者接管，消息在使用完毕后要调用DelocateZnpMsg进行销毁
//如果队列中没有消息，该函数将一直阻塞
stZnpMsg *ZnpMsgFIFOPop(stZnpMsgFIFOHdl *pZnpMsgFIFO)
{
	int rt;
	stZnpMsgListItem *pFirstNode;;
	stZnpMsg *pRet=NULL;


	if(pZnpMsgFIFO==NULL){
		return NULL;
	}

	pthread_mutex_lock(&pZnpMsgFIFO->m);
	if(pZnpMsgFIFO->MsgNum==0){//队列空时才阻塞在此等待通知
		while(1){
			rt=pthread_cond_wait(&pZnpMsgFIFO->c,&pZnpMsgFIFO->m);
			if(rt!=0){
				DEBUGLOG1(3,"Error, pthread_cond_wait failed %d.\n",rt);
				break;
			}
			if(pZnpMsgFIFO->MsgNum>0){
				break;
			}
		}
	}
	//取出消息
	if(pZnpMsgFIFO->MsgNum>0){
		pFirstNode=pZnpMsgFIFO->pZnpMsgList;
		pZnpMsgFIFO->pZnpMsgList=pZnpMsgFIFO->pZnpMsgList->pNext;
		pRet=pFirstNode->pZnpMsg;
		free(pFirstNode);
		pZnpMsgFIFO->MsgNum--;
	}
	pthread_mutex_unlock(&pZnpMsgFIFO->m);

//	//打印ZnpFrame
//	if(pRet!=NULL){
//		DEBUGLOG3(0,"Pop a znpmsg 0x%04X from fifo %d - left MsgNum %d.\n",pRet->MsgType,(unsigned int)pZnpMsgFIFO,pZnpMsgFIFO->MsgNum);
//	}

	return pRet;
}









