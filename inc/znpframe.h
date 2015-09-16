/*
 * znpframe.h
 *
 *  Created on: 2012-8-9
 *      Author: chengm
 */

#ifndef ZNPFRAME_H_
#define ZNPFRAME_H_

#define MAXFZNPRAMELEN		255//ZNP帧最大长度

//SOF
#define ZNP_SOF				(unsigned char)0xFE

//Command type
#define ZNP_POLL			(unsigned char)0x00
#define ZNP_SREQ			(unsigned char)0x20
#define ZNP_AREQ			(unsigned char)0x40
#define ZNP_SRSP			(unsigned char)0x60

//Subsystem
#define ZNP_RPCERR_IF		(unsigned char)0x00
#define ZNP_SYS_IF			(unsigned char)0x01
#define ZNP_AF_IF			(unsigned char)0x04
#define ZNP_ZDO_IF			(unsigned char)0x05
#define ZNP_SAPI_IF			(unsigned char)0x06
#define ZNP_UTIL_IF			(unsigned char)0x07

//stZNPFrame结构体的字节流就是ZNP帧
typedef struct{
	unsigned char Sof;		//0xFE
	unsigned char DataLen;	//数据部分长度
	unsigned char Cmd0;		//cmd0 = Type | Subsystem
	unsigned char Cmd1;		//cmd1 = ID
	unsigned char DataFcs[251];//数据部分，包含FCS
}stZNPFrame;

//p为(stZNPFrame *)类型
#define GETLEN_ZNPFRAME(p) (p->DataLen+5)
#define GETZNPFRAMECMDID(p) ((((unsigned short)p->Cmd0)<<8)|((unsigned short)p->Cmd1))

//刷新stZNPFrame中的FCS,FCS位于DataFcs中有效数据的最后一个字节之后
//return 0-成功、else-失败
int fRefreshZNPFrameFCS(stZNPFrame *pZNPFrame);

#endif /* ZNPFRAME_H_ */
