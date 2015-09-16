/*
 * znpframe.c
 *
 *  Created on: 2014-2-12
 *      Author: chengm
 */
#include <stdio.h>
#include "znpframe.h"

//刷新stZNPFrame中的FCS,FCS位于DataFcs中有效数据的最后一个字节之后
//return 0-成功、else-失败
int fRefreshZNPFrameFCS(stZNPFrame *pZNPFrame)
{
	int i,Len;
	unsigned char temp;
	char *pScan=NULL;

	if(pZNPFrame==NULL){
		return -1;
	}
	if(pZNPFrame->DataLen>250){
		return -2;
	}

	Len=pZNPFrame->DataLen+3;
	pScan=((char *)pZNPFrame)+1;//跳过帧头
	temp=pScan[0];
	for(i=1;i<Len;i++){
		temp = temp ^ pScan[i];
	}
	pZNPFrame->DataFcs[pZNPFrame->DataLen]=temp;

	return 0;
}
