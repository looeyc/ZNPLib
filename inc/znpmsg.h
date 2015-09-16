/*
 * znpmsg.h
 *
 *  Created on: 2012-8-9
 *      Author: chengm
 */

#ifndef ZNPLIB_TYP_H_
#define ZNPLIB_TYP_H_

#include <pthread.h>
#include "znpframe.h"

//添加一个ZNP message类型的步骤
//1、在头本文件中定义CMD_xxx；
//2、在头本文件中定义对应的stxxx结构体，即对应的ZnpMsg结构；
//3、在头本文件中的stZnpMsg定义中加入stxxx作为其中的消息联合体union成员；
//4、如果消息结构stxxx需要级联释放资源，则需要在DelocateZnpMsg中实现对该消息所占资源的级联释放，否则不用修改DelocateZnpMsg；
//5、在TransZnpMsgToZnpFrame中实现消息结构stxxx从结构体到ZnpFrame字节流的转换方法；（如果是应答类消息的话，目前可以省略）
//6、在TransZnpFrameToZnpMsg中实现消息结构stxxx从ZnpFrame字节流到结构体的转换方法；（如果是请求类消息的话，目前可以省略）

//ZNP message类型(0x cmd0 cmd1)
#define	CMD_SYS_RESET_REQ				(unsigned short)0x4100
#define	CMD_SYS_RESET_IND				(unsigned short)0x4180
#define	CMD_SYS_VERSION					(unsigned short)0x2102
#define	CMD_SYS_VERSION_R				(unsigned short)0x6102
#define CMD_SYS_OSAL_NV_READ			(unsigned short)0x2108
#define CMD_SYS_OSAL_NV_READ_R			(unsigned short)0x6108
#define CMD_SYS_OSAL_NV_WRITE			(unsigned short)0x2109
#define CMD_SYS_OSAL_NV_WRITE_R			(unsigned short)0x6109
#define CMD_SYS_OSAL_NV_ITEM_INIT		(unsigned short)0x2107
#define CMD_SYS_OSAL_NV_ITEM_INIT_R		(unsigned short)0x6107
#define CMD_SYS_OSAL_NV_DELETE			(unsigned short)0x2112
#define CMD_SYS_OSAL_NV_DELETE_R		(unsigned short)0x6112
#define CMD_SYS_OSAL_NV_LENGTH			(unsigned short)0x2113
#define CMD_SYS_OSAL_NV_LENGTH_R		(unsigned short)0x6113
#define CMD_SYS_ADC_READ				(unsigned short)0x210D
#define CMD_SYS_ADC_READ_R				(unsigned short)0x610D
#define CMD_SYS_GPIO					(unsigned short)0x210E
#define CMD_SYS_GPIO_R					(unsigned short)0x610E
#define CMD_SYS_RANDOM					(unsigned short)0x210C
#define CMD_SYS_RANDOM_R				(unsigned short)0x610C
#define CMD_SYS_SET_TIME				(unsigned short)0x2110
#define CMD_SYS_SET_TIME_R				(unsigned short)0x6110
#define CMD_SYS_GET_TIME				(unsigned short)0x2111
#define CMD_SYS_GET_TIME_R				(unsigned short)0x6111
#define CMD_SYS_SET_TX_POWER			(unsigned short)0x2114
#define CMD_SYS_SET_TX_POWER_R			(unsigned short)0x6114
#define CMD_ZB_READ_CONFIGURATION		(unsigned short)0x2604
#define CMD_ZB_READ_CONFIGURATION_R		(unsigned short)0x6604
#define CMD_ZB_WRITE_CONFIGURATION		(unsigned short)0x2605
#define CMD_ZB_WRITE_CONFIGURATION_R	(unsigned short)0x6605
#define CMD_ZB_APP_REGISTER_REQUEST		(unsigned short)0x260A
#define CMD_ZB_APP_REGISTER_REQUEST_R	(unsigned short)0x660A
#define CMD_ZB_START_REQUEST			(unsigned short)0x2600
#define CMD_ZB_START_REQUEST_R			(unsigned short)0x6600
#define CMD_ZB_PERMIT_JOINING_REQUEST	(unsigned short)0x2608
#define CMD_ZB_PERMIT_JOINING_REQUEST_R	(unsigned short)0x6608
#define CMD_ZB_BIND_DEVICE				(unsigned short)0x2601
#define CMD_ZB_BIND_DEVICE_R			(unsigned short)0x6601
#define CMD_ZB_ALLOW_BIND				(unsigned short)0x2602
#define CMD_ZB_ALLOW_BIND_R				(unsigned short)0x6602
#define CMD_ZB_SEND_DATA_REQUEST		(unsigned short)0x2603
#define CMD_ZB_SEND_DATA_REQUEST_R		(unsigned short)0x6603
#define CMD_ZB_GET_DEVICE_INFO			(unsigned short)0x2606
#define CMD_ZB_GET_DEVICE_INFO_R		(unsigned short)0x6606
#define CMD_ZB_FIND_DEVICE_REQUEST		(unsigned short)0x2607
#define CMD_ZB_FIND_DEVICE_REQUEST_R	(unsigned short)0x6607
#define CMD_AF_REGISTER					(unsigned short)0x2400
#define CMD_AF_REGISTER_R				(unsigned short)0x6400
#define CMD_AF_DATA_REQUEST				(unsigned short)0x2401
#define CMD_AF_DATA_REQUEST_R			(unsigned short)0x6401
#define CMD_AF_DATA_REQUEST_EXT			(unsigned short)0x2402
#define CMD_AF_DATA_REQUEST_EXT_R		(unsigned short)0x6402
#define CMD_AF_DATA_REQUEST_SRC_RTG		(unsigned short)0x2403
#define CMD_AF_DATA_REQUEST_SRC_RTG_R	(unsigned short)0x6403
#define CMD_AF_INTER_PAN_CTL			(unsigned short)0x2410
#define CMD_AF_INTER_PAN_CTL_R			(unsigned short)0x6410
#define CMD_AF_DATA_STORE				(unsigned short)0x2411
#define CMD_AF_DATA_STORE_R				(unsigned short)0x6411
#define CMD_AF_DATA_CONFIRM				(unsigned short)0x4480
#define CMD_AF_INCOMING_MSG				(unsigned short)0x4481
#define CMD_AF_INCOMING_MSG_EXT			(unsigned short)0x4482
#define CMD_AF_DATA_RETRIEVE			(unsigned short)0x2412
#define CMD_AF_DATA_RETRIEVE_R			(unsigned short)0x6412
#define CMD_AF_APSF_CONFIG_SET			(unsigned short)0x2413
#define CMD_AF_APSF_CONFIG_SET_R		(unsigned short)0x6413
#define CMD_ZDO_NWK_ADDR_REQ			(unsigned short)0x2500
#define CMD_ZDO_NWK_ADDR_REQ_R			(unsigned short)0x6500
#define CMD_ZDO_IEEE_ADDR_REQ			(unsigned short)0x2501
#define CMD_ZDO_IEEE_ADDR_REQ_R			(unsigned short)0x6501
#define CMD_ZDO_NODE_DESC_REQ			(unsigned short)0x2502
#define CMD_ZDO_NODE_DESC_REQ_R			(unsigned short)0x6502
#define CMD_ZDO_POWER_DESC_REQ			(unsigned short)0x2503
#define CMD_ZDO_POWER_DESC_REQ_R		(unsigned short)0x6503
#define CMD_ZDO_SIMPLE_DESC_REQ			(unsigned short)0x2504
#define CMD_ZDO_SIMPLE_DESC_REQ_R		(unsigned short)0x6504
#define CMD_ZDO_ACTIVE_EP_REQ			(unsigned short)0x2505
#define CMD_ZDO_ACTIVE_EP_REQ_R			(unsigned short)0x6505
#define CMD_ZDO_MATCH_DESC_REQ			(unsigned short)0x2506
#define CMD_ZDO_MATCH_DESC_REQ_R		(unsigned short)0x6506
#define CMD_ZDO_COMPLEX_DESC_REQ		(unsigned short)0x2507
#define CMD_ZDO_COMPLEX_DESC_REQ_R		(unsigned short)0x6507
#define CMD_ZDO_USER_DESC_REQ			(unsigned short)0x2508
#define CMD_ZDO_USER_DESC_REQ_R			(unsigned short)0x6508
#define CMD_ZDO_DEVICE_ANNCE			(unsigned short)0x250A
#define CMD_ZDO_DEVICE_ANNCE_R			(unsigned short)0x650A
#define CMD_ZDO_USER_DESC_SET			(unsigned short)0x250B
#define CMD_ZDO_USER_DESC_SET_R			(unsigned short)0x650B
#define CMD_ZDO_SERVER_DISC_REQ			(unsigned short)0x250C
#define CMD_ZDO_SERVER_DISC_REQ_R		(unsigned short)0x650C
#define CMD_ZDO_END_DEVICE_BIND_REQ		(unsigned short)0x2520
#define CMD_ZDO_END_DEVICE_BIND_REQ_R	(unsigned short)0x6520
#define CMD_ZDO_BIND_REQ				(unsigned short)0x2521
#define CMD_ZDO_BIND_REQ_R				(unsigned short)0x6521
#define CMD_ZDO_UNBIND_REQ				(unsigned short)0x2522
#define CMD_ZDO_UNBIND_REQ_R			(unsigned short)0x6522
#define CMD_ZDO_MGMT_NWK_DISC_REQ		(unsigned short)0x2530
#define CMD_ZDO_MGMT_NWK_DISC_REQ_R		(unsigned short)0x6530
#define CMD_ZDO_MGMT_LQI_REQ			(unsigned short)0x2531
#define CMD_ZDO_MGMT_LQI_REQ_R			(unsigned short)0x6531
#define CMD_ZDO_MGMT_RTG_REQ			(unsigned short)0x2532
#define CMD_ZDO_MGMT_RTG_REQ_R			(unsigned short)0x6532
#define CMD_ZDO_MGMT_BIND_REQ			(unsigned short)0x2533
#define CMD_ZDO_MGMT_BIND_REQ_R			(unsigned short)0x6533
#define CMD_ZDO_MGMT_LEAVE_REQ			(unsigned short)0x2534
#define CMD_ZDO_MGMT_LEAVE_REQ_R		(unsigned short)0x6534
#define CMD_ZDO_MGMT_DIRECT_JOIN_REQ	(unsigned short)0x2535
#define CMD_ZDO_MGMT_DIRECT_JOIN_REQ_R	(unsigned short)0x6535
#define CMD_ZDO_MGMT_PERMIT_JOIN_REQ	(unsigned short)0x2536
#define CMD_ZDO_MGMT_PERMIT_JOIN_REQ_R	(unsigned short)0x6536
#define CMD_ZDO_MGMT_NWK_UPDATE_REQ		(unsigned short)0x2537
#define CMD_ZDO_MGMT_NWK_UPDATE_REQ_R	(unsigned short)0x6537
#define CMD_ZDO_STARTUP_FROM_APP		(unsigned short)0x2540
#define CMD_ZDO_STARTUP_FROM_APP_R		(unsigned short)0x6540
#define CMD_ZDO_AUTO_FIND_DESTINATION	(unsigned short)0x4541
#define CMD_ZDO_SET_LINK_KEY			(unsigned short)0x2523
#define CMD_ZDO_SET_LINK_KEY_R			(unsigned short)0x6523
#define CMD_ZDO_REMOVE_LINK_KEY			(unsigned short)0x2524
#define CMD_ZDO_REMOVE_LINK_KEY_R		(unsigned short)0x6524
#define CMD_ZDO_GET_LINK_KEY			(unsigned short)0x2525
#define CMD_ZDO_GET_LINK_KEY_R			(unsigned short)0x6525
#define CMD_ZDO_NWK_DISCOVERY_REQ		(unsigned short)0x2526
#define CMD_ZDO_NWK_DISCOVERY_REQ_R		(unsigned short)0x6526
#define CMD_ZDO_JOIN_REQ				(unsigned short)0x2527
#define CMD_ZDO_JOIN_REQ_R				(unsigned short)0x6527
#define CMD_ZDO_MSG_CB_REGISTER			(unsigned short)0x253E
#define CMD_ZDO_MSG_CB_REGISTER_R		(unsigned short)0x653E
#define CMD_ZDO_MSG_CB_REMOVE			(unsigned short)0x253F
#define CMD_ZDO_MSG_CB_REMOVE_R			(unsigned short)0x653F
#define CMD_ZDO_MSG_CB_INCOMING			(unsigned short)0x45FF
#define	CMD_ZB_START_CONFIRM			(unsigned short)0x4680
#define	CMD_ZB_BIND_CONFIRM				(unsigned short)0x4681
#define	CMD_ZB_ALLOW_BIND_CONFIRM		(unsigned short)0x4682
#define	CMD_ZB_SEND_DATA_CONFIRM		(unsigned short)0x4683
#define	CMD_ZB_RECEIVE_DATA_INDICATION	(unsigned short)0x4687
#define	CMD_ZB_FIND_DEVICE_CONFIRM		(unsigned short)0x4685
#define	CMD_ZDO_NWK_ADDR_RSP			(unsigned short)0x4580
#define	CMD_ZDO_IEEE_ADDR_RSP			(unsigned short)0x4581
#define	CMD_ZDO_NODE_DESC_RSP			(unsigned short)0x4582
#define	CMD_ZDO_POWER_DESC_RSP			(unsigned short)0x4583
#define	CMD_ZDO_SIMPLE_DESC_RSP			(unsigned short)0x4584
#define	CMD_ZDO_ACTIVE_EP_RSP			(unsigned short)0x4585
#define	CMD_ZDO_MATCH_DESC_RSP			(unsigned short)0x4586
#define	CMD_ZDO_COMPLEX_DESC_RSP		(unsigned short)0x4587
#define	CMD_ZDO_USER_DESC_RSP			(unsigned short)0x4591//0x4588
#define	CMD_ZDO_USER_DESC_CONF			(unsigned short)0x4594//0x4589
#define	CMD_ZDO_SERVER_DISC_RSP			(unsigned short)0x458A
#define	CMD_ZDO_END_DEVICE_BIND_RSP		(unsigned short)0x45A0
#define	CMD_ZDO_BIND_RSP				(unsigned short)0x45A1
#define	CMD_ZDO_UNBIND_RSP				(unsigned short)0x45A2
#define	CMD_ZDO_MGMT_NWK_DISC_RSP		(unsigned short)0x45B0
#define	CMD_ZDO_MGMT_LQI_RSP			(unsigned short)0x45B1
#define	CMD_ZDO_MGMT_RTG_RSP			(unsigned short)0x45B2
#define	CMD_ZDO_MGMT_BIND_RSP			(unsigned short)0x45B3
#define	CMD_ZDO_MGMT_LEAVE_RSP			(unsigned short)0x45B4
#define	CMD_ZDO_MGMT_DIRECT_JOIN_RSP	(unsigned short)0x45B5
#define	CMD_ZDO_MGMT_PERMIT_JOIN_RSP	(unsigned short)0x45B6
#define	CMD_ZDO_STATE_CHANGE_IND		(unsigned short)0x45C0
#define	CMD_ZDO_END_DEVICE_ANNCE_IND	(unsigned short)0x45C1
#define	CMD_ZDO_MATCH_DESC_RSP_SENT		(unsigned short)0x45C2
#define	CMD_ZDO_STATUS_ERROR_RSP		(unsigned short)0x45C3
#define	CMD_ZDO_SRC_RTG_IND				(unsigned short)0x45C4
#define	CMD_ZDO_LEAVE_IND				(unsigned short)0x45C9
#define	CMD_ZDO_MSG_CB_INCOMING			(unsigned short)0x45FF

//Device specific configuration parameters-------------------------------------------
//用于Znp_ZB_WRITE_CONFIGURATION 和 Znp_ZB_READ_CONFIGURATION
#define ZCD_NV_STARTUP_OPTION_ID			0x0003
#define ZCD_NV_STARTUP_OPTION_SIZE			1
//value:
#define STARTOPT_CLEAR_STATE	0x02
#define STARTOPT_CLEAR_CONFIG	0x01
//Bit position	7-2			1						0
//Description	Reserved	STARTOPT_CLEAR_STATE	STARTOPT_CLEAR_CONFIG

#define ZCD_NV_LOGICAL_TYPE_ID				0x0087
#define ZCD_NV_LOGICAL_TYPE_SIZE			1
//value:
//COORDINATOR (0x00), ROUTER (0x01) or ENDDEVICE (0x02).
#define LOGICAL_TYPE_COORDINATOR			0x00
#define LOGICAL_TYPE_ROUTER					0x01
#define LOGICAL_TYPE_ENDDEVICE				0x02

#define ZCD_NV_ZDO_DIRECT_CB_ID				0x008F
#define ZCD_NV_ZDO_DIRECT_CB_SIZE			1
//value:
//0 - the host processor must use the ZDO_MSG_CB_REGISTER command to subscribe to a specific ZDO callback in order to receive it.
//1 - the host processor will receive the “verbose” response.

#define ZCD_NV_POLL_RATE_ID					0x0024
#define ZCD_NV_POLL_RATE_SIZE				2
//value:
//if end-device, wake up periodically with this duration to check for data with its parent device.
//in milliseconds and can range from 1 to 65000.

#define ZCD_NV_QUEUED_POLL_RATE_ID			0x0025
#define ZCD_NV_QUEUED_POLL_RATE_SIZE		2

#define ZCD_NV_RESPONSE_POLL_RATE_ID		0x0026
#define ZCD_NV_RESPONSE_POLL_RATE_SIZE		2

#define ZCD_NV_POLL_FAILURE_RETRIES_ID		0x0029
#define ZCD_NV_POLL_FAILURE_RETRIES_SIZE	1

#define ZCD_NV_INDIRECT_MSG_TIMEOUT_ID		0x002B
#define ZCD_NV_INDIRECT_MSG_TIMEOUT_SIZE	1

#define ZCD_NV_APS_FRAME_RETRIES_ID			0x0043
#define ZCD_NV_APS_FRAME_RETRIES_SIZE		1

#define ZCD_NV_APS_ACK_WAIT_DURATION_ID		0x0044
#define ZCD_NV_APS_ACK_WAIT_DURATION_SIZE	2

#define ZCD_NV_BINDING_TIME_ID				0x0046
#define ZCD_NV_BINDING_TIME_SIZE			2

#define ZCD_NV_USERDESC_ID					0x0081
#define ZCD_NV_USERDESC_SIZE				17

#define ZCD_NV_PANID_ID						0x0083
#define ZCD_NV_PANID_SIZE					2
//value:
//This parameter identifies the ZigBee network. This should be set to a value between 0 and 0x3FFF.
//Networks that exist in the same vicinity must have different values for this parameter.
//It can be set to a special value of 0xFFFF to indicate “don’t care”.

#define ZCD_NV_CHANLIST_ID					0x0084
#define ZCD_NV_CHANLIST_SIZE				4
//value:
//CHANNEL 11 0x00000800 ~ CHANNEL 26 0x04000000
///ALL_CHANNELS 0x07FFF800

#define ZCD_NV_PRECFGKEY_ID					0x0062
#define ZCD_NV_PRECFGKEY_SIZE				16
//Default value: [0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F]
//This is used for securing and un-securing packets in the network, if security is enabled for the network.
//NOTE: Use of this configuration item requires the ZNP code to be built with the SECURE=1 compile option.

#define ZCD_NV_PRECFGKEYS_ENABLE_ID			0x0063
#define ZCD_NV_PRECFGKEYS_ENABLE_SIZE		1
//Default value: TRUE
//If security functionality is enabled, there are two options to distribute the security key to all devices in the network.
//If this parameter is true, the same security key is assumed to be pre-configured in all devices in the network.
//If it is set to false, then the key only needs to be configured on the coordinator device.
//In this case, the key is distributed to each device upon joining by the coordinator.
//This key distribution will happen in the “clear” on the last hop of the packet transmission and this constitutes
//a brief “period of vulnerability” during which a malicious device can capture the key.
//Hence it is not recommended unless it can be ensured that there are no malicious devices in the vicinity at the time of network formation.

#define ZCD_NV_SECURITY_MODE_ID				0x0064
#define ZCD_NV_SECURITY_MODE_SIZE			1
//Default value: 0
//This parameter determines if security is used or not in this network.
//It can be set to 0 (to turn off NWK security) or 1 (to turn on NWK security).

#define ZCD_NV_USE_DEFAULT_TCLK_ID			0x006D
#define ZCD_NV_USE_DEFAULT_TCLK_SIZE		1

#define ZCD_NV_BCAST_RETRIES_ID				0x002E
#define ZCD_NV_BCAST_RETRIES_SIZE			1

#define ZCD_NV_PASSIVE_ACK_TIMEOUT_ID		0x002F
#define ZCD_NV_PASSIVE_ACK_TIMEOUT_SIZE		1

#define ZCD_NV_BCAST_DELIVERY_TIME_ID		0x0030
#define ZCD_NV_BCAST_DELIVERY_TIME_SIZE		1

#define ZCD_NV_ROUTE_EXPIRY_TIME_ID			0x002C
#define ZCD_NV_ROUTE_EXPIRY_TIME_SIZE		1

#define ZNP_NV_RF_TEST_PARAMS_ID			0x0F07
#define ZNP_NV_RF_TEST_PARAMS_SIZE			4

//以下是根据ZStack中的 ZComDef.h 代码补充的
#define ZCD_NV_EXTENDED_PAN_ID            	0x002D
#define ZCD_NV_EXTENDED_PAN_SIZE           	8

#define ZCD_NV_EXTADDR_ID                  	0x0001
#define ZCD_NV_EXTADDR_SIZE					8

//用于Znp_AF_DATA_REQUEST，设置发送选项
#define AF_ACK_REQUEST			0x10
#define AF_DISCV_ROUTE			0x20
#define AF_EN_SECURITY			0x40

//用于stZDO_BIND_REQ中的地址模式
#define ADDRESS_NOT_PRESENT 	(unsigned char)0x00 //Address Not Present
#define GROUP_ADDRESS 			(unsigned char)0x01 //Group address
#define ADDRESS_16_BIT 			(unsigned char)0x02 //Address 16 bit
#define ADDRESS_64_BIT 			(unsigned char)0x03 //Address 64 bit
#define BROADCAST 				(unsigned char)0xFF //Broadcast

//设备状态
#define DEV_HOLD				0x00	// Initialized - not started automatically
#define DEV_INIT				0x01	// Initialized - not connected to anything
#define DEV_NWK_DISC			0x02	// Discovering PAN's to join
#define DEV_NWK_JOINING			0x03	// Joining a PAN
#define DEV_NWK_REJOIN			0x04	// ReJoining a PAN, only for end devices
#define DEV_END_DEVICE_UNAUTH	0x05	// Joined but not yet authenticated by trust center
#define DEV_END_DEVICE			0x06	// Started as device after authentication
#define DEV_ROUTER				0x07	// Device joined, authenticated and is a router
#define DEV_COORD_STARTING		0x08	// Started as Zigbee Coordinator
#define DEV_ZB_COORD			0x09	// Started as Zigbee Coordinator
#define DEV_NWK_ORPHAN			0x0A	// Device has lost information about its parent..

//SYS_RESET_REQ
typedef struct{
	unsigned char Type;//This requests a target device reset (0) or serial bootloader reset (1). If the target device does not support serial bootloading, bootloader reset commands are ignored and no response is sent from the target.
}stSYS_RESET_REQ;

//SYS_RESET_IND
typedef struct{
	unsigned char Reason;//Power-up：0x00、External：0x01	、Watch-dog：0x02
	unsigned char TransportRev;
	unsigned char ProductId;
	unsigned char MajorRel;
	unsigned char MinorRel;
	unsigned char HwRev;
}stSYS_RESET_IND;

//SYS_VERSION
typedef struct{
	unsigned char TransportRev;
	unsigned char Product;
	unsigned char MajorRel;
	unsigned char MinorRel;
	unsigned char MaintRel;
}stSYS_VERSION;

typedef struct{
}stSYS_VERSION_R;
typedef struct{
}stSYS_OSAL_NV_READ;
typedef struct{
}stSYS_OSAL_NV_READ_R;
typedef struct{
}stSYS_OSAL_NV_WRITE;
typedef struct{
}stSYS_OSAL_NV_WRITE_R;
typedef struct{
}stSYS_OSAL_NV_ITEM_INIT;
typedef struct{
}stSYS_OSAL_NV_ITEM_INIT_R;
typedef struct{
}stSYS_OSAL_NV_DELETE;
typedef struct{
}stSYS_OSAL_NV_DELETE_R;
typedef struct{
}stSYS_OSAL_NV_LENGTH;
typedef struct{
}stSYS_OSAL_NV_LENGTH_R;
typedef struct{
}stSYS_ADC_READ;
typedef struct{
}stSYS_ADC_READ_R;
typedef struct{
}stSYS_GPIO;
typedef struct{
}stSYS_GPIO_R;
typedef struct{
}stSYS_RANDOM;
typedef struct{
}stSYS_RANDOM_R;
typedef struct{
}stSYS_SET_TIME;
typedef struct{
}stSYS_SET_TIME_R;
typedef struct{
}stSYS_GET_TIME;
typedef struct{
}stSYS_GET_TIME_R;

typedef struct{
	char TX_Power;
}stSYS_SET_TX_POWER;

typedef struct{
	char TX_Power;//Actual TX power setting, in dBm.
}stSYS_SET_TX_POWER_R;

typedef struct{
	unsigned char ConfigId;
}stZB_READ_CONFIGURATION;

typedef struct{
	unsigned char Status;
	unsigned char ConfigId;
	unsigned char Len;
	unsigned char Value[128];
}stZB_READ_CONFIGURATION_R;

//ZB_WRITE_CONFIGURATION
typedef struct{
	unsigned char ConfigId;
	unsigned char Len;
	char Value[128];//1-128字节
}stZB_WRITE_CONFIGURATION;

//ZB_WRITE_CONFIGURATION_R
typedef struct{
	unsigned char Status;
}stZB_WRITE_CONFIGURATION_R;

typedef struct{
}stZB_APP_REGISTER_REQUEST;
typedef struct{
}stZB_APP_REGISTER_REQUEST_R;

//ZB_START_REQUEST
typedef struct{
	//无
}stZB_START_REQUEST;

//ZB_START_REQUEST_R
typedef struct{
	//无
}stZB_START_REQUEST_R;

//ZB_PERMIT_JOINING_REQUEST
typedef struct{
	unsigned short Destination;
	unsigned char Timeout;//seconds
}stZB_PERMIT_JOINING_REQUEST;

//ZB_PERMIT_JOINING_REQUEST_R
typedef struct{
	unsigned char Status;
}stZB_PERMIT_JOINING_REQUEST_R;

typedef struct{
}stZB_BIND_DEVICE;
typedef struct{
}stZB_BIND_DEVICE_R;
typedef struct{
}stZB_ALLOW_BIND;
typedef struct{
}stZB_ALLOW_BIND_R;
typedef struct{
}stZB_SEND_DATA_REQUEST;
typedef struct{
}stZB_SEND_DATA_REQUEST_R;

typedef struct{
	unsigned char Param;
}stZB_GET_DEVICE_INFO;

typedef struct{
	unsigned char Param;
	unsigned char Value[8];//LSB first
}stZB_GET_DEVICE_INFO_R;

typedef struct{
	unsigned char SearchKey[8];
}stZB_FIND_DEVICE_REQUEST;

typedef struct{
	//空
}stZB_FIND_DEVICE_REQUEST_R;

typedef struct{
	unsigned char EndPoint;
	unsigned short AppProfId;
	unsigned short AppDeviceId;
	unsigned char AppDevVer;
	unsigned char LatencyReq;
	unsigned char AppNumInClusters;
	unsigned short AppInClusterList[16];
	unsigned char AppNumOutClusters;
	unsigned short AppOutClusterList[16];
}stAF_REGISTER;

typedef struct{
	unsigned char Status;
}stAF_REGISTER_R;

typedef struct{
	unsigned short DstAddr;
	unsigned char DestEndpoint;
	unsigned char SrcEndpoint;
	unsigned short ClusterID;
	unsigned char TransID;//对应的AF_DATA_CONFIRM应答中将包含相同的值，实现AF_DATA_CONFIRM与AF_DATA_REQUEST的配对
	unsigned char Options;//AF_ACK_REQUEST | AF_DISCV_ROUTE | AF_EN_SECURITY（三个选项可或运算）
	unsigned char Radius;
	unsigned char Len;
	unsigned char Data[128];//0-99 bytes data. Without any security (99 bytes), with NWK security (81 bytes), with NWK and APS security (64 bytes).
}stAF_DATA_REQUEST;

typedef struct{
	unsigned char Status;
}stAF_DATA_REQUEST_R;

typedef struct{
}stAF_DATA_REQUEST_EXT;
typedef struct{
}stAF_DATA_REQUEST_EXT_R;

typedef struct{
	unsigned short DstAddr;//Short address of the destination device
	unsigned char DestEndpoint;//Endpoint of the destination device
	unsigned char SrcEndpoint;//Endpoint of the source device
	unsigned short ClusterID;//Specifies the cluster ID
	unsigned char TransID;//Specifies the transaction sequence number of the message.
	unsigned char Options;//AF_ACK_REQUEST、AF_DISCV_ROUTE、AF_EN_SECURITY，The transmit options field is organized as a bitmask.
	unsigned char Radius;//Specifies the number of hops allowed delivering the message
	unsigned char RelayCount;//<=55,Specifies the number of devices in the relay list for source routing
	unsigned short RelayList[55];//List of relay devices on the source routing path. For each device, it contains 2 bytes short address for each device.
	unsigned char Len;//<=128,Length of the data.
	unsigned char Data[128];//0-128 bytes data
}stAF_DATA_REQUEST_SRC_RTG;

typedef struct{
	unsigned char Status;
}stAF_DATA_REQUEST_SRC_RTG_R;

typedef struct{
}stAF_INTER_PAN_CTL;
typedef struct{
}stAF_INTER_PAN_CTL_R;
typedef struct{
}stAF_DATA_STORE;
typedef struct{
}stAF_DATA_STORE_R;

typedef struct{
	unsigned char Status;
	unsigned char Endpoint;
	unsigned char TransID;
}stAF_DATA_CONFIRM;

//用于数据订阅
typedef struct{
	unsigned short GroupID;
	unsigned short ClusterID;//目的Cluster
	unsigned short SrcAddr;
	unsigned char SrcEndpoint;
	unsigned char DestEndpoint;
	unsigned char WasBroadcast;
	unsigned char LinkQuality;
	unsigned char SecurityUse;
	unsigned int TimeStamp;
	unsigned char TransSeqNumber;
	unsigned char Len;
	unsigned char Data[99];
}stAF_INCOMING_MSG;

typedef struct{
}stAF_DATA_RETRIEVE;
typedef struct{
}stAF_DATA_RETRIEVE_R;
typedef struct{
}stAF_APSF_CONFIG_SET;
typedef struct{
}stAF_APSF_CONFIG_SET_R;
typedef struct{
}stZDO_NWK_ADDR_REQ;
typedef struct{
}stZDO_NWK_ADDR_REQ_R;

typedef struct{
	unsigned short ShortAddr;
	unsigned char ReqType;
	unsigned char StartIndex;
}stZDO_IEEE_ADDR_REQ;

typedef struct{
	unsigned char Status;
}stZDO_IEEE_ADDR_REQ_R;

typedef struct{
	unsigned short DstAddr;
	unsigned short NWKAddrOfInterest;
}stZDO_NODE_DESC_REQ;

typedef struct{
	unsigned char Status;
}stZDO_NODE_DESC_REQ_R;

typedef struct{
}stZDO_POWER_DESC_REQ;
typedef struct{
}stZDO_POWER_DESC_REQ_R;

typedef struct{
	unsigned short DstAddr;
	unsigned short NWKAddrOfInterest;
	unsigned char Endpoint;
}stZDO_SIMPLE_DESC_REQ;

typedef struct{
	unsigned char Status;
}stZDO_SIMPLE_DESC_REQ_R;

typedef struct{
	unsigned short DstAddr;
	unsigned short NWKAddrOfInterest;
}stZDO_ACTIVE_EP_REQ;

typedef struct{
	unsigned char Status;
}stZDO_ACTIVE_EP_REQ_R;

//用于Znp_ZDO_MATCH_DESC_REQ获取指定节点描述符，作为输入到目标描述符
typedef struct{
	unsigned char DstAddr[2];//Specifies NWK address of the device generating the inquiry.
	unsigned char NWKAddrOfInterest[2];//Specifies NWK address of the destination device being queried.
	unsigned char ProfileID[2];//Specifies the profile ID of the device
	unsigned char NumInClusters;//Specifies the number of IDs in the InClusterList(NumInClusters <= 16).
	unsigned char InClusterList[32];//Contains the input cluster IDs ( 2bytes each )
	unsigned char NumOutClusters;//Specifies the number of IDs in the OutClusterList(NumInClusters <= 16).
	unsigned char OutClusterList[32];//Contains the output cluster IDs ( 2bytes each )
}stZDO_MATCH_DESC_REQ;

typedef struct{
}stZDO_MATCH_DESC_REQ_R;
typedef struct{
}stZDO_COMPLEX_DESC_REQ;
typedef struct{
}stZDO_COMPLEX_DESC_REQ_R;

typedef struct{
	unsigned short DstAddr;
	unsigned short NWKAddrOfInterest;
}stZDO_USER_DESC_REQ;

typedef struct{
	unsigned char Status;
}stZDO_USER_DESC_REQ_R;

typedef struct{
}stZDO_DEVICE_ANNCE;
typedef struct{
}stZDO_DEVICE_ANNCE_R;

typedef struct{
	unsigned short DstAddr;
	unsigned short NWKAddrOfInterest;
	unsigned char Len;
	char UserDescriptor[16];
}stZDO_USER_DESC_SET;

typedef struct{
	unsigned char Status;
}stZDO_USER_DESC_SET_R;

typedef struct{
}stZDO_SERVER_DISC_REQ;
typedef struct{
}stZDO_SERVER_DISC_REQ_R;
typedef struct{
}stZDO_END_DEVICE_BIND_REQ;
typedef struct{
}stZDO_END_DEVICE_BIND_REQ_R;

typedef struct{
	unsigned short DstAddr;//指定向哪一个设备发起这个控制，例如0x0000表示协调器来执行这个绑定操作，Specifies the network address of the device to send message.
	unsigned char SrcAddress[8];//绑定的源地址，Specifies the 64 bit binding source IEEE address
	unsigned char SrcEndpoint;//绑定的源EP，Specifies the binding source endpoint.
	unsigned short ClusterID;//绑定的Cluster，Specifies the cluster ID to match in messages.
	unsigned char DstAddrMode;//实验结果显示当前只支持ADDRESS_64_BIT模式，Specifies binding destination address mode: ADDRESS_NOT_PRESENT, GROUP_ADDRESS, ADDRESS_16_BIT, ADDRESS_64_BIT, BROADCAST,
	unsigned char DstAddress[8];//绑定的目的地址Binding destination IEEE address. The field is 8-bytes long. However, depending on the value of the DstAddrMode field, only lowest order 2 bytes could be significant.Not to be confused with DstAddr.
	unsigned char DstEndpoint;//绑定的目的EP，Specifies the binding destination endpoint.
}stZDO_BIND_REQ;

typedef struct{
	unsigned char Status;//Status is either Success (0) or Failure (1).
}stZDO_BIND_REQ_R;

typedef struct{
}stZDO_UNBIND_REQ;
typedef struct{
}stZDO_UNBIND_REQ_R;
typedef struct{
}stZDO_MGMT_NWK_DISC_REQ;
typedef struct{
}stZDO_MGMT_NWK_DISC_REQ_R;
typedef struct{
}stZDO_MGMT_LQI_REQ;
typedef struct{
}stZDO_MGMT_LQI_REQ_R;

typedef struct{
	unsigned short DstAddr;
	unsigned char StartIndex;
}stZDO_MGMT_RTG_REQ;

typedef struct{
	unsigned char Status;
}stZDO_MGMT_RTG_REQ_R;

typedef struct{
}stZDO_MGMT_BIND_REQ;
typedef struct{
}stZDO_MGMT_BIND_REQ_R;

typedef struct{
	unsigned short DstAddr;
	unsigned char DeviceAddr[8];
	unsigned char RemoveChildren_Rejoin;//This field has a value of 1 if the device being asked to leave the network is also being asked to remove its child devices, if any. Otherwise it has a value of 0. Currently, the stack profile of Home Control specifies that this field should always be set to 0.
}stZDO_MGMT_LEAVE_REQ;

typedef struct{
	unsigned char Status;
}stZDO_MGMT_LEAVE_REQ_R;

typedef struct{
}stZDO_MGMT_DIRECT_JOIN_REQ;
typedef struct{
}stZDO_MGMT_DIRECT_JOIN_REQ_R;
typedef struct{
}stZDO_MGMT_PERMIT_JOIN_REQ;
typedef struct{
}stZDO_MGMT_PERMIT_JOIN_REQ_R;
typedef struct{
}stZDO_MGMT_NWK_UPDATE_REQ;
typedef struct{
}stZDO_MGMT_NWK_UPDATE_REQ_R;
typedef struct{
}stZDO_STARTUP_FROM_APP;
typedef struct{
}stZDO_STARTUP_FROM_APP_R;
typedef struct{
}stZDO_AUTO_FIND_DESTINATION;
typedef struct{
}stZDO_SET_LINK_KEY;
typedef struct{
}stZDO_SET_LINK_KEY_R;
typedef struct{
}stZDO_REMOVE_LINK_KEY;
typedef struct{
}stZDO_REMOVE_LINK_KEY_R;
typedef struct{
}stZDO_GET_LINK_KEY;
typedef struct{
}stZDO_GET_LINK_KEY_R;
typedef struct{
}stZDO_NWK_DISCOVERY_REQ;
typedef struct{
}stZDO_NWK_DISCOVERY_REQ_R;
typedef struct{
}stZDO_JOIN_REQ;
typedef struct{
}stZDO_JOIN_REQ_R;
typedef struct{
}stZDO_MSG_CB_REGISTER;
typedef struct{
}stZDO_MSG_CB_REGISTER_R;
typedef struct{
}stZDO_MSG_CB_REMOVE;
typedef struct{
}stZDO_MSG_CB_REMOVE_R;
typedef struct{
}stZB_START_CONFIRM;
typedef struct{
}stZB_BIND_CONFIRM;
typedef struct{
}stZB_ALLOW_BIND_CONFIRM;
typedef struct{
}stZB_SEND_DATA_CONFIRM;
typedef struct{
}stZB_RECEIVE_DATA_INDICATION;

//用于Znp_ZB_FIND_DEVICE_REQUEST
typedef struct{
	unsigned char SearchType;
	unsigned char SearchKey[2];//找到的16位设备短地址
	unsigned char Result[8];
}stZB_FIND_DEVICE_CONFIRM;

typedef struct{
	unsigned char Status;
	unsigned char IEEEAddr[8];
	unsigned short NwkAddr;
	unsigned char StartIndex;
	unsigned char NumAssocDev;
	unsigned short AssocDevList[70/2];
}stZDO_NWK_ADDR_RSP;

//用于Znp_ZDO_IEEE_ADDR_REQ获取指定节点到子节点列表
typedef struct{
	unsigned char Status;
	unsigned char IEEEAddr[8];//Specifies the 64 bit IEEE address of device.
	unsigned short NwkAddr;
	unsigned char StartIndex;//Specifies the starting index into the list of associated devices for this report.
	unsigned char NumAssocDev;//Specifies the number of associated devices.
	unsigned short AssocDevList[70/2];//Contains the list of short addresses ( 2 bytes each ) for associated devices. This list can be a partial list if the entire list doesn’t fit into a packet. If it is a partial list, the starting index is StartIndex.
}stZDO_IEEE_ADDR_RSP;

//用于Znp_ZDO_NODE_DESC_REQ获取的指定节点描述符
typedef struct{
	unsigned short SrcAddr;//The message’s source network address
	unsigned char Status;//This field indicates either SUCCESS or FAILURE
	unsigned short NWKAddrOfInterest;//Device’s short address of this Node descriptor
	unsigned char LogicalType_CmplxDscAvl_UserDscAvl;//Bit 0-2: 0-ZigBee Coordinator,1-ZigBee Router,2-ZigBee End Device
	unsigned char APSFlags_FrequencyBand;//APSFlags - Bit 0-4 Node Flags assigned for APS, NodeFrequencyBand - Bit 5-7 Identifies node frequency band capabilities
	unsigned char MacCapabilitiesFlags;
	unsigned short ManufacturerCode;//Specifies a manufacturer code that is allocated by the ZigBee Alliance, relating to the manufacturer to the device
	unsigned char MaxBufferSize;//Indicates size of maximum NPDU. This field is used as a high level indication for management
	unsigned short MaxInTransferSize;//Indicates maximum size of Transfer up to 0x7fff (This field is reserved in version 1.0 and shall be set to zero)
	unsigned short ServerMask;//Bit0 - Primary Trust Center, Bit1 - Backup Trust Center, Bit2 - Primary Binding Table Cache, Bit3 - Backup Binding Table Cache, Bit4 - Primary Discovery Cache, Bit5 - Backup Discovery Cache
	unsigned short MaxOutTransferSize;//Indicates maximum size of Transfer up to 0x7fff
	unsigned char DescriptorCapabilities;//Specifies the Descriptor capabilities
}stZDO_NODE_DESC_RSP;

typedef struct{
}stZDO_POWER_DESC_RSP;

//用于Znp_ZDO_SIMPLE_DESC_REQ获取的指定节点描述符
typedef struct{
	unsigned short SrcAddr;//Specifies the message’s source network address.
	unsigned char Status;//This field indicates either SUCCESS or FAILURE.
	unsigned short NWKAddr;//Specifies Device’s short address that this response describes.
	unsigned char Len;//Specifies the length of the simple descriptor
	unsigned char Endpoint;//Specifies Endpoint of the device
	unsigned short ProfileId;//The profile ID for this endpoint.
	unsigned short DeviceID;//The Device Description ID for this endpoint.
	unsigned char DeviceVersion;//Defined as the following format: 	0 - Version 1.00, 0x01-0x0F - Reserved.
	unsigned char NumInClusters;
	unsigned short InClusterList[32/2];
	unsigned char NumOutClusters;
	unsigned short OutClusterList[32/2];
}stZDO_SIMPLE_DESC_RSP;

//用于Znp_ZDO_ACTIVE_EP_REQ获取指定节点描述符
typedef struct{
	unsigned short SrcAddr;//The message’s source network address.
	unsigned char Status;//This field indicates either SUCCESS or FAILURE.
	unsigned short NWKAddr;//Device’s short address that this response describes.
	unsigned char ActiveEPCount;//Number of active endpoint in the list
	unsigned char ActiveEPList[77];//Array of active endpoints ( 1 byte each ) on this device.
}stZDO_ACTIVE_EP_RSP;

//用于Znp_ZDO_MATCH_DESC_REQ获取指定节点描述符
typedef struct{
	unsigned char SrcAddr[2];//The message’s source network address.
	unsigned char Status;//This field indicates either SUCCESS or FAILURE.
	unsigned char NWKAddr[2];//Device’s short address that this response describes.
	unsigned char MatchLength;//The count of endpoints on the remote device that match the request criteria
	unsigned char MatchList;//List of bytes, each represents an 8 bit endpoint
}stZDO_MATCH_DESC_RSP;

typedef struct{
}stZDO_COMPLEX_DESC_RSP;

//用于Znp_ZDO_USER_DESC_REQ获取的指定节点描述符
typedef struct{
	unsigned short SrcAddr;//The message’s source network address.
	unsigned char Status;//This field indicates either SUCCESS or FAILURE.
	unsigned short NWKAddr;//Device’s short address that this response describes.
	unsigned char UserLength;//Length of the complex descriptor.
	char UserDescriptor[77];//Array of bytes contains user descriptor.
}stZDO_USER_DESC_RSP;

//用于Znp_ZDO_USER_DESC_SET设置用户描述符
typedef struct{
	unsigned short SrcAddr;//The message’s source network address.
	unsigned char Status;//This field indicates either SUCCESS or FAILURE.
	unsigned short NWKAddr;//Device’s short address that this response describes.
}stZDO_USER_DESC_CONF;

typedef struct{
}stZDO_SERVER_DISC_RSP;
typedef struct{
}stZDO_END_DEVICE_BIND_RSP;

typedef struct{
	unsigned short SrcAddr;//The message’s source network address.
	unsigned char Status;//This field indicates either SUCCESS (0) or FAILURE (1).
}stZDO_BIND_RSP;

typedef struct{
}stZDO_UNBIND_RSP;
typedef struct{
}stZDO_MGMT_NWK_DISC_RSP;

//用于Znp_ZDO_MGMT_LQI_REQ获取邻节点列表
typedef struct{
	unsigned char ExtendedPanID[8];//Extended PAN ID of the neighbor device
	unsigned char ExtendedAddress[8];//Network extended address
	unsigned short NetworkAddress;//Device short address
	unsigned char DevTyp_RxOn_Rel;//DeviceType: bits 1-0, RxOnWhenIdle: bits 3-2, Relationship: bits 6-4
	unsigned char PermitJoining;//PermitJoining: bits 1-0
	unsigned char Depth;
	unsigned char LQI;
}stNeighborLqiList;
typedef struct{
	unsigned short SrcAddr;//Source address of the message.
	unsigned char Status;//This field indicates either SUCCESS or FAILURE.
	unsigned char NeighborTableEntries;//Total number of entries available in the device.
	unsigned char StartIndex;//Where in the total number of entries this response starts.
	unsigned char NeighborLqiListCount;//Number of entries in this response.
	stNeighborLqiList NeighborLqiList[3];//一条回复最多只能包含3条记录
}stZDO_MGMT_LQI_RSP;

typedef struct{
	unsigned short DestinationAddress;//Network destination address
	unsigned char Status;//Route status: bits 2-0	0x00-Active, 0x01-Discovery Underway, 0x02-Discovery Failed, 0x03-Inactive, 0x04 – 0x07 Reserved
	unsigned short NextHop;//Next hop network address
}stRtgListItem;
typedef struct{
	unsigned short SrcAddr;//Source address of the message.
	unsigned char Status;//This field indicates either SUCCESS or FAILURE.
	unsigned char RoutingTableEntries;//Total number of entries available in the device.
	unsigned char StartIndex;//Where in the total number of entries this response starts.
	unsigned char RoutingTableListCount;//Number of entries in this response.
	stRtgListItem RoutingTableList[75/5];//最多容纳15个
}stZDO_MGMT_RTG_RSP;

typedef struct{
}stZDO_MGMT_BIND_RSP;

//用于Znp_ZDO_MGMT_LEAVE_REQ驱离节点
typedef struct{
	unsigned short SrcAddr;//Source address of the message
	unsigned char Status;//This field indicates either SUCCESS (0) or FAILURE (1).
}stZDO_MGMT_LEAVE_RSP;

typedef struct{
}stZDO_MGMT_DIRECT_JOIN_RSP;

typedef struct{
	unsigned short SrcAddr;
	unsigned char Status;
}stZDO_MGMT_PERMIT_JOIN_RSP;

typedef struct{
	unsigned char State;//0-10分别表示：DEV_HOLD、DEV_INIT、DEV_NWK_DISC、DEV_NWK_JOINING、DEV_NWK_REJOIN、DEV_END_DEVICE_UNAUTH、DEV_END_DEVICE、DEV_ROUTER、DEV_COORD_STARTING、DEV_ZB_COORD、DEV_NWK_ORPHAN
}stZDO_STATE_CHANGE_IND;

typedef struct{
	unsigned short SrcAddr;//Source address of the message.
	unsigned short NwkAddr;//Specifies the device’s short address.
	unsigned char IEEEAddr[8];//Specifies the 64 bit IEEE address of source device.
	unsigned char Capabilites;	//Specifies the MAC capabilities of the device.
								//Bit: 0 – Alternate PAN Coordinator
								//1 – Device type: 1- ZigBee Router; 0 – End Device
								//2 – Power Source: 1 Main powered
								//3 – Receiver on when idle
								//4 – Reserved
								//5 – Reserved
								//6 – Security capability
								//7 – Reserved
}stZDO_END_DEVICE_ANNCE_IND;

typedef struct{
}stZDO_MATCH_DESC_RSP_SENT;
typedef struct{
}stZDO_STATUS_ERROR_RSP;
typedef struct{
}stZDO_SRC_RTG_IND;

typedef struct{
	unsigned short SrcAddr;
	unsigned char ExtAddr[8];
	unsigned char Request;
	unsigned char Remove;
	unsigned char Rejoin;
}stZDO_LEAVE_IND;

typedef struct{
}stZDO_MSG_CB_INCOMING;

typedef struct{
	unsigned char GroupID[2];
	unsigned char ClusterID[2];
	unsigned char SrcAddrMode;
	unsigned char SrcAddr[8];
	unsigned char SrcEndpoint;
	unsigned char SrcPanId[2];
	unsigned char DstEndpoint;
	unsigned char WasBroadcast;
	unsigned char LinkQuality;
	unsigned char SecurityUse;
	unsigned char TimeStamp[4];
	unsigned char TransSeqNumber;
	unsigned char Len[2];
	unsigned char Data[223];
}stAF_INCOMING_MSG_EXT;

//统一的ZNP数据结构
typedef struct{
	unsigned short MsgType;//组成结构为{CMD1、CMD0}，CMD0是字节流中的第一个字节，但是作为Type的高字节
	union{
		stSYS_RESET_REQ mSYS_RESET_REQ;
		stSYS_RESET_IND mSYS_RESET_IND;
		stSYS_SET_TX_POWER mSYS_SET_TX_POWER;
		stSYS_SET_TX_POWER_R mSYS_SET_TX_POWER_R;
		stZDO_IEEE_ADDR_RSP mZDO_IEEE_ADDR_RSP;//用于Znp_ZDO_IEEE_ADDR_REQ获取指定节点到子节点列表
		stZDO_NODE_DESC_RSP mZDO_NODE_DESC_RSP;//用于Znp_ZDO_NODE_DESC_REQ获取指定节点描述符
		stZDO_SIMPLE_DESC_RSP mZDO_SIMPLE_DESC_RSP;//用于Znp_ZDO_SIMPLE_DESC_REQ获取指定节点描述符
		stZDO_ACTIVE_EP_RSP mZDO_ACTIVE_EP_RSP;//用于Znp_ZDO_ACTIVE_EP_REQ获取指定节点描述符
		stZDO_MATCH_DESC_REQ mZDO_MATCH_DESC_REQ;//用于Znp_ZDO_MATCH_DESC_REQ获取指定节点描述符，作为输入到目标描述符
		stZDO_MATCH_DESC_RSP mZDO_MATCH_DESC_RSP;//用于Znp_ZDO_MATCH_DESC_REQ获取指定节点描述符
		stZDO_USER_DESC_RSP mZDO_USER_DESC_RSP;//用于Znp_ZDO_USER_DESC_REQ获取指定节点描述符
		stZDO_USER_DESC_CONF mZDO_USER_DESC_CONF;//用于Znp_ZDO_USER_DESC_SET设置用户描述符
		stZDO_MGMT_LQI_RSP mZDO_MGMT_LQI_RSP;//用于Znp_ZDO_MGMT_LQI_REQ获取邻节点列表
		stZDO_MGMT_LEAVE_RSP mZDO_MGMT_LEAVE_RSP;//用于Znp_ZDO_MGMT_LEAVE_REQ驱离节点
		stZDO_STATE_CHANGE_IND mZDO_STATE_CHANGE_IND;
		stZDO_MGMT_PERMIT_JOIN_RSP mZDO_MGMT_PERMIT_JOIN_RSP;
		stZDO_NWK_ADDR_RSP mZDO_NWK_ADDR_RSP;
		stZDO_IEEE_ADDR_REQ mZDO_IEEE_ADDR_REQ;
		stZDO_IEEE_ADDR_REQ_R mZDO_IEEE_ADDR_REQ_R;
		stZDO_END_DEVICE_ANNCE_IND mZDO_END_DEVICE_ANNCE_IND;
		stZDO_ACTIVE_EP_REQ mZDO_ACTIVE_EP_REQ;
		stZDO_ACTIVE_EP_REQ_R mZDO_ACTIVE_EP_REQ_R;
		stZDO_NODE_DESC_REQ mZDO_NODE_DESC_REQ;
		stZDO_NODE_DESC_REQ_R mZDO_NODE_DESC_REQ_R;
		stZDO_SIMPLE_DESC_REQ mZDO_SIMPLE_DESC_REQ;
		stZDO_SIMPLE_DESC_REQ_R mZDO_SIMPLE_DESC_REQ_R;
		stZDO_USER_DESC_REQ mZDO_USER_DESC_REQ;
		stZDO_USER_DESC_REQ_R mZDO_USER_DESC_REQ_R;
		stZDO_USER_DESC_SET mZDO_USER_DESC_SET;
		stZDO_USER_DESC_SET_R mZDO_USER_DESC_SET_R;
		stZDO_MGMT_LEAVE_REQ mZDO_MGMT_LEAVE_REQ;
		stZDO_MGMT_LEAVE_REQ_R mZDO_MGMT_LEAVE_REQ_R;
		stZDO_LEAVE_IND mZDO_LEAVE_IND;
		stZDO_MGMT_RTG_REQ mZDO_MGMT_RTG_REQ;
		stZDO_MGMT_RTG_REQ_R mZDO_MGMT_RTG_REQ_R;
		stZDO_MGMT_RTG_RSP mZDO_MGMT_RTG_RSP;
		stZDO_BIND_REQ mZDO_BIND_REQ;
		stZDO_BIND_REQ_R mZDO_BIND_REQ_R;
		stZDO_BIND_RSP mZDO_BIND_RSP;
		stZB_START_REQUEST mZB_START_REQUEST;
		stZB_START_REQUEST_R mZB_START_REQUEST_R;
		stZB_READ_CONFIGURATION mZB_READ_CONFIGURATION;
		stZB_READ_CONFIGURATION_R mZB_READ_CONFIGURATION_R;
		stZB_WRITE_CONFIGURATION mZB_WRITE_CONFIGURATION;
		stZB_WRITE_CONFIGURATION_R mZB_WRITE_CONFIGURATION_R;
		stZB_PERMIT_JOINING_REQUEST mZB_PERMIT_JOINING_REQUEST;
		stZB_PERMIT_JOINING_REQUEST_R mZB_PERMIT_JOINING_REQUEST_R;
		stZB_GET_DEVICE_INFO mZB_GET_DEVICE_INFO;
		stZB_GET_DEVICE_INFO_R mZB_GET_DEVICE_INFO_R;
		stZB_FIND_DEVICE_REQUEST mZB_FIND_DEVICE_REQUEST;
		stZB_FIND_DEVICE_REQUEST_R mZB_FIND_DEVICE_REQUEST_R;
		stZB_FIND_DEVICE_CONFIRM mZB_FIND_DEVICE_CONFIRM;
		stAF_DATA_CONFIRM mAF_DATA_CONFIRM;
		stAF_DATA_REQUEST_SRC_RTG mAF_DATA_REQUEST_SRC_RTG;
		stAF_DATA_REQUEST_SRC_RTG_R mAF_DATA_REQUEST_SRC_RTG_R;
		stAF_INCOMING_MSG mAF_INCOMING_MSG;
		stAF_INCOMING_MSG_EXT mAF_INCOMING_MSG_EXT;
		stAF_REGISTER mAF_REGISTER;
		stAF_REGISTER_R mAF_REGISTER_R;
		stAF_DATA_REQUEST mAF_DATA_REQUEST;
		stAF_DATA_REQUEST_R mAF_DATA_REQUEST_R;
	}un;
}stZnpMsg;

//用于Znp_ZB_APP_REGISTER_REQUEST注册EP
typedef struct{
	unsigned char 	AppEndPoint;//Specifies the endpoint of the device. This should be in the range of 1 through 240 and should be set to same value for all devices in the network.
	unsigned short 	AppProfileID;//Specifies the profile id of the application. This should be set to same value to all devices in the network. This number is assigned by the ZigBee Alliance.
	unsigned short 	DeviceId;//This is an application-specific identifier. It identifies the device type within the particular profile id. This is not used by the ZigBee stack in any way other than to identify itself when requested.
	unsigned char	DeviceVersion;//This is an application-specific identifier. It identifies the version of the device. This is not used by the ZigBee stack in any way other than to identify itself when requested.
	unsigned char 	InputCommandsNum;//Specifies the number of Input commands that this application will process.
	unsigned short 	*InputCommandsList;//List of input command identifiers that are processed by this application. Each command identifier is 2 bytes long, LSB first.
	unsigned char 	OutputCommandsNum;//Specifies the number of Output commands that this application will generate.
	unsigned short 	*OutputCommandsList;//List of output command identifiers that are generated by this application. Each command identifier is 2 bytes long, LSB first.
}stAppRegParam;

//ZNP消息队列，将消息排队，逐一调用回调进行上报
typedef struct stZnpMsgNode_ stZnpMsgNode;
struct stZnpMsgNode_{
	stZnpMsgNode *pNext;
	stZnpMsg *pZnpMsg;
};

//消息订阅句柄
typedef struct stZnpMsgSubsHdl_ stZnpMsgSubsHdl;
struct stZnpMsgSubsHdl_{
	stZnpMsgSubsHdl *pPrev;
	stZnpMsgSubsHdl *pNext;

	//等待的消息特征
	unsigned short MsgType;
	unsigned char isTransIDNeeded;//0-不需要匹配TransID、1-需要匹配TransID
	unsigned char TransID;//对于AF_DATA_REQUEST_EXT、	AF_DATA_REQUEST_SRC_RTG、AF_DATA_CONFIRM 订阅时可以指定TransID

	int isMsgValid;
	pthread_mutex_t m;
	pthread_cond_t c;

	//消息
	stZnpMsg *pZnpMsg;
};

//创建一个ZnpMsg
stZnpMsg *AllocateZnpMsg();

//销毁一个ZnpMsg
void DelocateZnpMsg(stZnpMsg *pZnpMsg);

//将ZnpMsg从ZNPFrame的字节流转换为本头文件所定义的结构体
//pZnpMsg - 需要转换的ZnpMsg
//pZNPFrame - 用于返回转换得到的ZNPFrame，由调用者提供
//return 0-成功，else-失败
int TransZnpMsgToZnpFrame(stZnpMsg *pZnpMsg, stZNPFrame *pZNPFrame);

//将ZnpMsg从本头文件所定义的结构体转换为ZNPFrame的字节流
//pZNPFrame - 需要转换的ZNPFrame
//pZnpMsg - 转换得到的ZnpMsg，事先要调用AllocateZnpMsg分配得到，使用要调用DellocateZnpMsg销毁
int TransZnpFrameToZnpMsg(stZNPFrame *pZNPFrame, stZnpMsg *pZnpMsg);

//该线程将ZnpMsg队列中的消息逐一取出并调用用户回调进行处理
void *Thread_ZNPMsgHandler(void *pArg);

//订阅消息
//MsgType 需要订阅的消息类型
//isTransIDNeeded - 是否需要指定TransID，0-不需要、1-需要
//TransID - 指定的TransID，isTransIDNeeded为0时该参数忽略
stZnpMsgSubsHdl *ZnpMsgSubscribe(unsigned short MsgType, unsigned char isTransIDNeeded, unsigned char TransID);

//等待订阅的消息
//pZnpMsgSubsHdl 订阅句柄
//Timeout 最大等待时间（毫秒 ms）
//return 成功时，返回得到的消息，失败时为NULL
stZnpMsg *ZnpMsgPend(stZnpMsgSubsHdl *pZnpMsgSubsHdl, int Timeout);

//寄出消息
//在消息订阅列表中查找订阅者，并向其寄出消息
//return 0-表示该消息当前无订阅者, >0-表示该消息当前的订阅者数量, <0-出错
//当前只允许一条消息有一个订阅者
int ZnpMsgPost(stZnpMsg *pZnpMsg);

//退订消息
void ZnpMsgUnsubscribe(stZnpMsgSubsHdl *pZnpMsgSubsHdl);

#endif /* ZNPLIB_TYP_H_ */











