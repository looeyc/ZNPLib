/*
 * znpcomhal.h
 *
 *  Created on: 2012-8-12
 *      Author: chengm
 */

#ifndef ZNPCOMHAL_H_
#define ZNPCOMHAL_H_

//初始化ZNP通信端口
extern int ZNPComInit(char *pComPath);

//关闭ZNP通信端口
extern void ZNPComClose();

//接收数据
//pLen - 输入输出，输入为缓存区长度，输出为实际数据长度
//pBuff - 调用者提供接收数据存储区
//return 0-成功，else-失败
extern int ZNPComRcv(unsigned int *pLen, unsigned char *pBuff);

//发送数据
//Len - 发送数据长度
//pBuff - 待发数据存储区
//return 0-成功，else-失败
extern int ZNPComSend(unsigned int Len, unsigned char *pBuff);

#endif /* ZNPCOMHAL_H_ */
