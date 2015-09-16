/*
 * znplib.h
 *
 *  Created on: 2012-8-14
 *      Author: chengm
 */
#ifndef ZNPLIB_H_
#define ZNPLIB_H_

#include "pthread.h"
#include "znpmsg.h"

typedef struct{
	char *pZNPComPath;//ZNP COM口的路径
	void (*pDefaultZnpMsgCB)(stZnpMsg *pZnpMsg);//函数指针，注册Znp消息默认处理回调，参数pZnpMsg为收到的消息

	//指定ZNP的IEEE地址
	char isValid_IEEEAddr;		//0-未指定、1-指定
	unsigned char IEEEAddr[8];	//对应的isValid_xxx为1时有效，如果为全0xFF，则表示使用不使用NV参数中的IEEEAddr
								//ZStack选择IEEE地址的过程：
								//1、NV参数是否设置了有效的IEEEAddr，有的话就用；
								//2、Secondry IEEE地址否可用，可用的话就用；
								//3、使用TI再Flash的Info page中固定好的IEEE地址；
								//4、如果以上都不成功，则随机生成一个临时的IEEE地址。

	//以下网络参数中任何一个被指定时，ZNP启动的时候都会将所有之前的网络状态NV参数清空，“一切从0开始”，所有的字节点记录都会被清掉
	//指定网络参数PANID
	char isValid_PANID;			//0-未指定、1-指定
	unsigned short PANID;		//对应的isValid_xxx为1时有效，如果为0xFFFF，则表示由协调器自己选择
	//指定网络参数Extended PANID
	char isValid_ExtPANID;		//0-未指定、1-指定
	unsigned char ExtPANID[8];	//对应的isValid_xxx为1时有效，如果为全0xFF，则表示
}stZnpCfg;

//初始化，必须调用
//return 	0--成功、else--失败
int ZnpInit(stZnpCfg *pZnpCfg);

//ZNP命令===============================================================================================================================================================
//除非特别说明，所有命令函数返回值return 	0--成功、else--失败

//SYS interface-----------------------------------------------------------------------------------------------------------------------------------
//复位，复位需要一秒左右的时候完成
//Type – 1 byte – This requests a target device reset (0) or serial bootloader reset (1).
int Znp_SYS_RESET(unsigned char Type);

//设置发送功率
//TX_Power - Requested TX power setting, in dBm.
//pRet --调用者提供返回实际设置成的功率值,dBm
//return 0-成功，else-失败
int Znp_SYS_SET_TX_POWER(char TX_Power, stSYS_SET_TX_POWER_R *pRet);

//Configuration interface-------------------------------------------------------------------------------------------------------------------------

//ConfigId – 1 byte – The identifier for the configuration property
//Len – 1 byte – Specifies the size of the Value buffer in bytes.
//pValue – 1-128 bytes – The buffer containing the new value of the configuration property
//return >=0时为SRSP中的Status值，<0表示发送失败
int Znp_ZB_WRITE_CONFIGURATION(unsigned char ConfigId, unsigned char Len, unsigned char *pValue);

//read the value of a configuration parameter
//ConfigId 需要读取的配置项ID
//pLen 输入输出，输入--pValue所指向到缓存区长度，输出--实际得到的数据长度
//pValue 调用者实先准备号的用于存放返回值到存储区，返回数据长度最长为128
//return >=0时为SRSP中的Status值，<0表示发送失败
int Znp_ZB_READ_CONFIGURATION(unsigned char ConfigId, unsigned char *pLen, unsigned char *pValue);

//Simple API interface----------------------------------------------------------------------------------------------------------------------------
//pAppRegParam 需注册的EP参数
//return >=0时为SRSP中的Status值，<0表示发送失败
//extern int Znp_ZB_APP_REGISTER(stAppRegParam *pAppRegParam);

//启动
//return 0-成功，else-失败
int Znp_ZB_START();

//允许新设备加入
//Destination -- 指定地址的节点或者all routers and coordinator (0xFFFC).
//Timeout -- If timeout is set to 0x00, the device will turn off the joining permissions indefinitely. If it is set to 0xFF, the joining permissions will be turned on indefinitely.
//			0x00-关闭许可，0xFF-永久开放许可，0x01-0xFE-开放的秒数
int Znp_ZB_PERMIT_JOINING(unsigned short Destination, unsigned char Timeout);

//发送数据
//This command initiates transmission of data to another device in the network.
//This command can only be issued after the application processor has registered its application using the ZB_APP_REGISTER_REQUEST and the device has successfully created or joined a network.
//Destination - The destination address of the data packet. It can be one of the following values:
//		0-0xFFF7 16-bit short address of the destination device
//		0xFFFC Group of all routers and coordinator
//		0xFFFD Group of all devices with receiver turned on
//		0xFFFE This is the binding address and should be used when a binding entry has been previously created for this particular CommandId. The destination address will be determined from the binding table by the CC2530-ZNP
//		0xFFFF Broadcast group of all devices in the network
//CommandId - The command ID to send with the message. If the binding address is used for destination, this parameter also indicates the binding to use.
//Handle - A handle used to identify the send data request. The corresponding ZB_SEND_DATA_CONFIRM will have the same handle value. This can be useful if the application wishes to match up ZB_SEND_DATA_REQUESTs with ZB_SEND_DATA_CONFIRMs.
//Ack - TRUE if requesting APS acknowledgement from the destination.
//Radius - The max number of hops the packet can travel through before it is dropped.
//Len - Specifies the size of the Data buffer in bytes.
//pData - 0-99 bytes, Without any security (99 bytes), with NWK security (81 bytes), with NWK and APS security (64 bytes).
//return >=0时为ZB_SEND_DATA_CONFIRM中的Status值，-1--发送失败， -1000--等待ZB_SEND_DATA_CONFIRM超时
//extern int Znp_ZB_SEND_DATA(unsigned short Destination,unsigned short CommandId, unsigned char Handle, unsigned char Ack, unsigned char Radius, unsigned char Len, unsigned char *pData);

//获取设备信息
//Param--需要获取的参数ID
//Param		Size		Description
//	0 		1 byte 		Device state – See 4.5.52
//	1 		8 bytes 	Device IEEE address
//	2 		2 bytes 	Device short address
//	3 		2 bytes 	Short address of the parent device
//	4 		8 bytes 	IEEE address of the parent device
//	5 		1 byte 		Channel on which the ZigBee network is operating
//	6 		2 bytes	 	PAN ID of the ZigBee network
//	7 		8 bytes 	Extended PAN Id of the ZigBee network
//pLen -- 输入输出， 输入-调用者提供的存储空间长度，输出-实际获取的参数长度，简便起见，输入必须>=8，而输出恒定为8
//pValue --调用者提供返回参数的存储空间，长度必须为8byte以上
int Znp_ZB_GET_DEVICE_INFO(unsigned char Param, int *pLen, unsigned char *pValue);

//获取设备16bit网络地址
//pSearchKey - 输入，设备的64bit IEEE地址，指向unsigned char IEEEAddr[8]数组
//pValue - 输出，调用者提供存储空间，存放搜索结果
//return 0--成功，-1--失败, >0-超时，说明指定设备不存在
//20130726 	测试时发现连续反复调用此函数100多次之后出现问题，所有请求都无法发出，SRSP返回成功，ARSP总是等待超时，抓包发现原因是请求根本没有被发无线出去。
//			会不会是因为该函数为广播请求，一段时间内超过一定次数之后ZStack会保护性关闭，以避免给网络造成拥堵？
//			而且，即便用ZTool反复发这个请求，也会出现这样的问题！
int Znp_ZB_FIND_DEVICE_REQUEST(unsigned char *pSearchKey, stZB_FIND_DEVICE_CONFIRM *pValue);

//AF Interface----------------------------------------------------------------------------------------------------------------------------
//注册EndPoint
//EndPoint - 需要注册的EndPoint
//AppProfId - Profile ID
//AppDeviceId - Device ID
//AppDevVer - 设备版本号
//LatencyReq - 只能取0
//AppNumInClusters - 输入Cluster数量（最多16个）
///AppInClusterList[] - 输入Cluster列表，最多16个Cluster ID
//AppNumOutClusters - 输出Cluster数量（最多16个）
//AppOutClusterList[] - 输出Cluster列表，最多16个Cluster ID
//pCBFxn - 回调函数指针，当注册到EP有消息到来时，调用该回调，pCBFxn的参数是指向消息数据的指针，参数pZnpMsg为收到的消息
//			原型为 void (*pCBFxn)(unsigned char EndPoint, stZnpMsg *pZnpMsg)
//return 0-成功，else-出错
int Znp_AF_REGISTER(unsigned char EndPoint,unsigned short AppProfId,unsigned short AppDeviceId,unsigned char AppDevVer,unsigned char LatencyReq,unsigned char AppNumInClusters,unsigned short *pAppInClusterList,unsigned char AppNumOutClusters,unsigned short *pAppOutClusterList,void (*pCBFxn));

//发送数据
//This command is used by the App processor to build and send a message through AF layer.
//DstAddr 		- Short address of the destination device
//DestEndpoint 	- Endpoint of the destination device
//SrcEndpoint 	- Endpoint of the source device
//ClusterID 	- Specifies the cluster ID
//Options		- AF_ACK_REQUEST - set this bit to request APS acknowledgement for this packet
//				  AF_DISCV_ROUTE - set this bit to force route discovery if a routing table entry doesn’t exist
//				  AF_EN_SECURITY - set this bit to enable APS security for this packet.
//Radius		- Specifies the number of hops allowed delivering the message (reference AF_DEFAULT_RADIUS)
//Len			- Length of the data.
//pData 		- 0-99 bytes data. Without any security (99 bytes), with NWK security (81 bytes), with NWK and APS security (64 bytes).
//return  0-成功，else-失败
int Znp_AF_DATA_REQUEST(unsigned short DstAddr,unsigned char DestEndpoint,unsigned char SrcEndpoint,unsigned short ClusterID,unsigned char Options,unsigned char Radius,unsigned char Len,unsigned char *pData);

//This command is used by the App processor to build and send a message through AF layer using source routing.
//DstAddr		- Short address of the destination device
//DestEndpoint	- Endpoint of the destination device
//SrcEndpoint	- Endpoint of the source device
//ClusterID		- Specifies the cluster ID
//TransID		- Specifies the transaction sequence number of the message.
//Options		- AF_ACK_REQUEST、AF_DISCV_ROUTE、AF_EN_SECURITY，The transmit options field is organized as a bitmask.
//Radius		- Specifies the number of hops allowed delivering the message
//RelayCount	- <=55, Specifies the number of devices in the relay list for source routing
//RelayList		- List of relay devices on the source routing path. For each device, it contains 2 bytes short address for each device.
//Len			- <=128, Length of the data.
//pData			- 0-128 bytes data
//return  0-成功，else-失败
int Znp_AF_DATA_REQUEST_SRC_RTG(unsigned short DstAddr,unsigned char DestEndpoint,unsigned char SrcEndpoint,unsigned short ClusterID,unsigned char Options,unsigned char Radius,unsigned char RelayCount,unsigned short *pRelayList,unsigned char Len,unsigned char *pData);

//ZDO Interface----------------------------------------------------------------------------------------------------------------------------
//ShortAddr - Specifies the short address of the device.
//ReqType - 0x00 - Single Device response, 0x01 - Extended, include associated devices
//StartIndex - 从0开始，Starting index into the list of children. This is used to get more of the list if the list is too large for one message
//pRetVal - 输出，调用者提供存储空间，存放ZDO_IEEE_ADDR_RSP中到返回参数
//return 0--成功，else--失败
//注意，ZCD_NV_ZDO_DIRECT_CB 必须设置为TRUE才能收到ZDO_IEEE_ADDR_RSP
int Znp_ZDO_IEEE_ADDR_REQ(unsigned short ShortAddr,unsigned char ReqType,unsigned char StartIndex,stZDO_IEEE_ADDR_RSP *pRetVal);

//This command is generated to inquire about the Node Descriptor information of the destination device.
//DstAddr - Specifies NWK address of the device generating the inquiry.
//NWKAddrOfInterest - Specifies NWK address of the destination device being queried.
//pRetVal - 输出，调用者提供存储空间，存放ZDO_NODE_DESC_RSP中到返回参数
//return 0--成功，>0--发送失败，为SRSP中的Status值，-1000--等待ZDO_NODE_DESC_RSP超时
//注意，ZCD_NV_ZDO_DIRECT_CB 必须设置为TRUE才能收到ZDO_NODE_DESC_RSP
int Znp_ZDO_NODE_DESC_REQ(unsigned short DstAddr, unsigned short NWKAddrOfInterest, stZDO_NODE_DESC_RSP *pRetVal);

//This command is generated to inquire as to the Simple Descriptor of the destination device’s Endpoint.
//DstAddr - Specifies NWK address of the device generating the inquiry.
//NWKAddrOfInterest - Specifies NWK address of the destination device being queried.
//Endpoint - Specifies the application endpoint the data is from.
//pRetVal - 输出，调用者提供存储空间，存放返回参数
//return 0--成功，>0--发送失败，为SRSP中的Status值，-1000--等待ZDO_SIMPLE_DESC_RSP超时
//注意，ZCD_NV_ZDO_DIRECT_CB 必须设置为TRUE才能收到ZDO_SIMPLE_DESC_RSP
int Znp_ZDO_SIMPLE_DESC_REQ(unsigned short DstAddr, unsigned short NWKAddrOfInterest, unsigned char Endpoint, stZDO_SIMPLE_DESC_RSP *pRetVal);

//This command is generated to request a list of active endpoints from the destination device.
//DstAddr - Specifies NWK address of the device generating the inquiry.
//NWKAddrOfInterest - Specifies NWK address of the destination device being queried.
//pRetVal - 输出，调用者提供存储空间，存放返回参数
//return 0--成功，>0--发送失败，为SRSP中的Status值，-1000--等待ZDO_ACTIVE_EP_RSP超时
//注意，ZCD_NV_ZDO_DIRECT_CB 必须设置为TRUE才能收到ZDO_ACTIVE_EP_RSP
int Znp_ZDO_ACTIVE_EP_REQ(unsigned short DstAddr, unsigned short NWKAddrOfInterest, stZDO_ACTIVE_EP_RSP *pRetVal);

//pDescToMatch - 输入，目标描述符，根据该描述符进行匹配
//pRetVal - 输出，调用者提供存储空间，存放返回参数
//return 0--成功，>0--发送失败，为SRSP中的Status值，-1000--等待ZDO_MATCH_DESC_RSP超时
//注意，ZCD_NV_ZDO_DIRECT_CB 必须设置为TRUE才能收到ZDO_MATCH_DESC_RSP
//extern int Znp_ZDO_MATCH_DESC_REQ(stZDO_MATCH_DESC_REQ *pDescToMatch, stZDO_MATCH_DESC_RSP *pRetVal);

//DstAddr - Specifies destination NWK address.
//NWKAddrOfInterest - Specifies NWK address of the device the query is intended for.
//pRetVal - 输出，调用者提供存储空间，存放返回参数
//return 0--成功，else--失败
//注意，ZCD_NV_ZDO_DIRECT_CB 必须设置为TRUE才能收到ZDO_USER_DESC_RSP
//<！！！重要说明：ZNP串口返回的SRSP和AREQ与文档上的有出入，这里是根据实际到返回格式编写的代码>
int Znp_ZDO_USER_DESC_REQ(unsigned short DstAddr, unsigned short NWKAddrOfInterest, stZDO_USER_DESC_RSP *pRetVal);

//DstAddr - Specifies network address of the device generating the set request.
//NWKAddrOfInterest - Specifies NWK address of the destination device being queried.
//Len - Specifies the length of the user descriptor.
//UserDescriptor - User descriptor array (can be up to 16 bytes).
//return 0--成功，else--失败
//<！！！重要说明：ZNP串口返回的SRSP和AREQ与文档上的有出入，这里是根据实际到返回格式编写的代码>
int Znp_ZDO_USER_DESC_SET(unsigned short DstAddr, unsigned short NWKAddrOfInterest, unsigned char Len, char *pUserDescriptor);

//This command is generated to request the destination device to return its neighbor table.
//DstAddr - Specifies the network address of the device generating the query.
//StartIndex - Specifies where to start in the response array list. The result may contain more entries than can be reported, so this field allows the user to retrieve the responses anywhere in the array list.
//pRetVal - 输出，调用者提供存储空间，存放返回参数
//return 0--成功，>0--发送失败，为SRSP中的Status值，-1000--等待ZDO_MGMT_LQI_RSP超时
//注意，ZCD_NV_ZDO_DIRECT_CB 必须设置为TRUE才能收到ZDO_MGMT_LQI_RSP
//extern int Znp_ZDO_MGMT_LQI_REQ(unsigned short DstAddr, unsigned char StartIndex, stZDO_MGMT_LQI_RSP *pRetVal);

//This command is generated to request the Routing Table of the destination device.
//DstAddr - Specifies the network address of the device generating the query.
//StartIndex - Specifies where to start in the response array list. The result may contain more entries than can be reported, so this field allows the user to retrieve the responses anywhere in the array list.
//pRetVal - 输出，调用者提供存储空间，存放返回参数
//return 0--成功，else--失败
int Znp_ZDO_MGMT_RTG_REQ(unsigned short DstAddr,unsigned char StartIndex,stZDO_MGMT_RTG_RSP *pRetVal);

//This command is generated to request a Management Leave Request for the target device and is used to remove devices from the network.
//DstAddr - 需要驱离的设备短地址
//pDeviceAddress - 可以是8个0
//RemoveChildren/Rejoin - Currently, the stack profile of Home Control specifies that this field should always be set to 0. This field has a value of 1 if the device being asked to leave the network is also being asked to remove its child devices, if any. Otherwise it has a value of 0.
//doWaitForRsp - 0-不需要等待RSP，1-需要等待RSP
//return 0--成功，else--失败
int Znp_ZDO_MGMT_LEAVE_REQ(unsigned short DstAddr,unsigned char *pDeviceAddress,unsigned char RemoveChildren_Rejoin,int doWaitForRsp);

//请求一个设备向另一个设备发起绑定
//DstAddr - 指定发起绑定的设备地址
//pSrcAddress - 绑定的源地址，64bit地址
//SrcEndpoint - 绑定的源EP
//ClusterID - 绑定的Cluster
//DstAddrMode - 目的地址模式（实验表明只支持ADDRESS_64_BIT） ADDRESS_NOT_PRESENT, GROUP_ADDRESS, ADDRESS_16_BIT, ADDRESS_64_BIT, BROADCAST
//pDstAddress - 绑定的目的地址
//DstEndpoint - 绑定的目的EP
//return 0--成功，else--失败
int Znp_ZDO_BIND_REQ(unsigned short DstAddr,unsigned char *pSrcAddress,unsigned char SrcEndpoint,unsigned short ClusterID,unsigned char DstAddrMode,unsigned char *pDstAddress,unsigned char DstEndpoint);

//UTIL Interface----------------------------------------------------------------------------------------------------------------------------

//更高一层的接口-------------------------------------------------------------------------------------------------------------------------------
//获取设备列表
//pNum - 输入输出， 输入 - pDevNwkAddrList能容纳的设备地址数量， 输出 - 实际发现的设备数量, 当输出与输入数值相等时，有可能网络没有发现完全
//pDevNwkAddrList - 调用者提供空间，设备短地址列表
//返回结果中，无响应的设备也会被包括进来，第一项地址为0x0000表示协调器
//return 0--成功，else--失败
//int Znp_TOOL_GetDevList(int *pNum, unsigned short *pDevNwkAddrList);

//获取当前Znp的ZDO状态
//0 - DEV_HOLD				Initialized - not started automatically
//1 - DEV_INIT				Initialized - not connected to anything
//2 - DEV_NWK_DISC			Discovering PAN's to join
//3 - DEV_NWK_JOINING		Joining a PAN
//4 - DEV_NWK_REJOIN		ReJoining a PAN, only for end devices
//5 - DEV_END_DEVICE_UNAUTH	Joined but not yet authenticated by trust center
//6 - DEV_END_DEVICE		Started as device after authentication
//7 - DEV_ROUTER			Device joined, authenticated and is a router
//8 - DEV_COORD_STARTING	Starting as Zigbee Coordinator
//9 - DEV_ZB_COORD			Started as Zigbee Coordinator
//10- DEV_NWK_ORPHAN		Device has lost information about its parent
unsigned char Znp_GetZDOState();

#endif /* ZNPLIB_H_ */







