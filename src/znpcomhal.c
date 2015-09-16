/*
 * znpcomhal.c
 *
 *  Created on: 2014-2-11
 *      Author: chengm
 */

//ZNP的底层通信接口，当前使用的是COM口

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
#include <termios.h>    /*PPSIX 终端控制定义*/
#include "znpcomhal.h"

int DevFd = 0;//ZNP通信端口设备文件描述符
static pthread_mutex_t mutex_ComDev = PTHREAD_MUTEX_INITIALIZER;//串口访问锁

int HalConfigDevice(int fd)
{
	struct termios attr;

	//读取串口当前属性
	tcgetattr(fd, &attr);
	//设置最少接收字符个数为0
	attr.c_cc[VMIN] = 0;
	attr.c_cc[VTIME] = 10;
	attr.c_iflag = IGNPAR;
	attr.c_oflag = 0;
	attr.c_lflag = 0;
	//设置波特率为38400，字符长度为8位，偶校验，允许接收
	attr.c_cflag = B38400 | CS8 | CLOCAL | CREAD;
	attr.c_cflag &= ~HUPCL;//attr.c_cflag &= ~PARENB;//关闭奇偶校验
	//设置串口属性
	tcflush(fd,TCIFLUSH);
	tcsetattr(fd, TCSANOW, &attr);
	fcntl(fd,F_SETFL,FNDELAY);

	return 0;
}

extern int ZNPComInit(char *pComPath)
{
	int rt;

	if(pComPath==NULL){
		return -1;
	}

	DevFd = open(pComPath, O_RDWR | O_NDELAY | O_NOCTTY);
	if (DevFd <= 0){
		return -2;
	}

	rt = HalConfigDevice(DevFd);
	if(rt != 0){
		close(DevFd);
		DevFd = 0;
	}

	return rt;
}

void ZNPComColse()
{
	if(DevFd != 0)
		close(DevFd);
}

int ZNPComRcv(unsigned int *pLen, unsigned char *pBuff)
{
	unsigned int nread = 0;
	unsigned int BuffLen=*pLen;
	int rt;
	fd_set fds;

	if((*pLen<256)||(pBuff==NULL)){
		return 1;
	}

	//清除监测集合
	FD_ZERO(&fds);
	//将设备句柄加入到监测集合中
	FD_SET(DevFd, &fds);

	//监测串口是否有数据接收到
	rt = select(DevFd+1, &fds, NULL, NULL, NULL);//永远等待
	if(rt < 0){
		*pLen = 0;
		return rt;
	}else if(rt == 0){
	}else{
		nread = 0;
		pthread_mutex_lock(&mutex_ComDev);
		rt = read(DevFd,pBuff,256);
		nread = (rt>=0)? (nread+(unsigned int)rt) : nread;
		while(1){
			if(rt<=0){//读取不到数据为止
				break;
			}else{//否则继续读取
				if(BuffLen-nread<256){//剩下的空间有溢出风险
					break;
				}
				rt = read(DevFd,pBuff+nread,256);
				if(rt>0){
					nread+=(unsigned int)rt;
				}
			}
		}
		pthread_mutex_unlock(&mutex_ComDev);
	}

	*pLen = nread;
	return 0;
}

int ZNPComSend(unsigned int Len, unsigned char *pBuff)
{
	int rt;
	fd_set fds;
	struct timeval tv;

	if((Len == 0)||(pBuff == NULL)){
		return -1;
	}

	//清除监测集合
	FD_ZERO(&fds);
	//将设备句柄加入到监测集合中
	FD_SET(DevFd, &fds);

	tv.tv_sec = 5;
	tv.tv_usec = 0;
	//监测串口是否可写
	rt = select(DevFd+1, NULL, &fds, NULL, &tv);
	if(rt <= 0){
		return -2;
	}

	pthread_mutex_lock(&mutex_ComDev);
	rt = write(DevFd,pBuff,Len);
	pthread_mutex_unlock(&mutex_ComDev);

	if(rt < 0)
		return -3;

	if((unsigned int)rt < Len){
		return -4;
	}

	return 0;
}

//int ZNPComSend(unsigned int Len, unsigned char *pBuff)
//{
//	int rt;
//
//	if((Len == 0)||(pBuff == NULL)){
//		return -1;
//	}
//	pthread_mutex_lock(&mutex_ComDev);
//	rt = write(DevFd,pBuff,Len);
//	pthread_mutex_unlock(&mutex_ComDev);
//
//	if(rt < 0)
//		return -2;
//
//	if((unsigned int)rt < Len){
//		return -2;
//	}
//
//	return 0;
//}








