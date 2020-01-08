/********************************** (C) COPYRIGHT *******************************
* File Name          : CONFIG.h
* Author             : WCH
* Version            : V1.10
* Date               : 2019/11/05
* Description        : 配置说明及默认值，在工程属性中配置当前值
*******************************************************************************/

/******************************************************************************/
#ifndef __CONFIG_H
#define __CONFIG_H

#define	ID_CH577							0x77
#define	ID_CH578							0x78
#define	ID_CH579							0x79

#define CHIP_ID								ID_CH579

#ifdef CH57xBLE_ROM
#include "CH57xBLE_ROM.H"
#else
#include "CH57xBLE_LIB.H"
#endif
/*********************************************************************
【MAC】
	BLE_MAC												- 是否自定义蓝牙Mac地址 ( 默认:FALSE )，需要在main.c修改Mac地址定义

【SLEEP】
	HAL_SLEEP   									- 是否开启睡眠功能 ( 默认:FALSE )
	WAKE_UP_RTC_MAX_TIME					- 睡眠唤醒所需时间，根据不同睡眠类型取值可分为：睡眠模式/下电模式 -	80
																																								暂停模式					-	60
																																								空闲模式					-	10
														
【SNV】
	BLE_SNV    				            - 是否开启SNV功能，用于储存绑定信息( 默认:TRUE )
	BLE_SNV_ADDR    					    - SNV信息保存地址，使用code flash最后一块( 默认:0x3E600 )
	BLE_SNV_NUM    					    	- SNV信息保存块数量，每块可以保存三个连接( 默认:1 )

【RTC】
	CLK_OSC32K_RC									- 是否使用内部32K，使用了蓝牙主机功能则开启，其余关闭( 默认:TRUE )	

【MEMORY】
	BLE_MEMHEAP_SIZE  						- 蓝牙协议栈使用的RAM大小，( 默认:(1024*8) )               

【DATA】
	BLE_BUFF_MAX_LEN							- 单个连接最大包长度( 默认:27 (ATT_MTU=23)，取值范围[27~251] )
	BLE_BUFF_NUM									- 控制器缓存的包数量( 默认:10 )
	BLE_TX_NUM_EVENT							- 单个连接事件最多可以发多少个数据包( 默认:1 )
	BLE_TX_POWER									- 发射功率( 默认:LL_TX_POWEER_0_DBM (0dBm) )

【TEST】
	BLE_DIRECT_TEST								- 是否启用直接测试模式( 默认:FALSE )
	
【MULTICONN】
	PERIPHERAL_MAX_CONNECTION			- 最多可同时做多少从机角色( 默认:1 )
	CENTRAL_MAX_CONNECTION				- 最多可同时做多少主机角色( 默认:3 )
**********************************************************************/

/*********************************************************************
 * 默认配置值
 */
#ifndef BLE_MAC
#define BLE_MAC											FALSE
#endif
#ifndef HAL_SLEEP
#define HAL_SLEEP										FALSE
#define WAKE_UP_RTC_MAX_TIME				0
#endif
#ifndef HAL_KEY
#define HAL_KEY											FALSE
#endif
#ifndef HAL_LED
#define HAL_LED											FALSE	
#endif
#ifndef BLE_SNV
#define BLE_SNV											TRUE
#define BLE_SNV_ADDR								0x3E600
#define BLE_SNV_NUM									1
#endif
#ifndef CLK_OSC32K_RC
#define CLK_OSC32K_RC								TRUE
#endif
#ifndef BLE_MEMHEAP_SIZE
#define BLE_MEMHEAP_SIZE						(1024*8)
#endif
#ifndef BLE_BUFF_MAX_LEN
#define BLE_BUFF_MAX_LEN						27
#endif
#ifndef BLE_BUFF_NUM
#define BLE_BUFF_NUM								10
#endif
#ifndef BLE_TX_NUM_EVENT
#define BLE_TX_NUM_EVENT						1
#endif
#ifndef BLE_TX_POWER
#define BLE_TX_POWER								LL_TX_POWEER_0_DBM
#endif
#ifndef BLE_DIRECT_TEST
#define BLE_DIRECT_TEST							FALSE
#endif
#ifndef PERIPHERAL_MAX_CONNECTION
#define PERIPHERAL_MAX_CONNECTION		1
#endif
#ifndef CENTRAL_MAX_CONNECTION
#define CENTRAL_MAX_CONNECTION			3
#endif

extern u32 MEM_BUF[BLE_MEMHEAP_SIZE/4];
extern u8C MacAddr[6];

#endif

/******************************* endfile @ config ******************************/
