/********************************** (C) COPYRIGHT *******************************
* File Name          : OTAprofile.h
* Author             : WCH
* Version            : V1.0
* Date               : 2018/12/11
* Description        :        
*******************************************************************************/

#ifndef OTAPROFILE_H
#define OTAPROFILE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "CH57xBLE_LIB.h"	
	

/*********************************************************************
 * CONSTANTS
 */

// OTA Profile通道Index定义
#define OTAPROFILE_CHAR                 0                               
                                    
// OTA 服务的UUID定义
#define OTAPROFILE_SERV_UUID            0xFEE0
    
// OTA 通讯通道UUID定义
#define OTAPROFILE_CHAR_UUID            0xFEE1

// Simple Keys Profile Services bit fields
#define OTAPROFILE_SERVICE               0x00000001


/*********************************************************************
 * TYPEDEFS
 */
/* OTA操作Flash的保护状态标志 */
#define OP_CODEFLASH_SAFE_FLAG1          0x57
#define OP_CODEFLASH_SAFE_FLAG2          0xA8

/* OTA操作DataFlash的保护状态标志 */
#define OP_DATAFLASH_SAFE_FLAG1          0x57
#define OP_DATAFLASH_SAFE_FLAG2          0xA8

  
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

// 读写操作函数回调
typedef void (*OTAProfileRead_t)( unsigned char paramID );
typedef void (*OTAProfileWrite_t)( unsigned char paramID ,unsigned char *p_data, unsigned char w_len);

typedef struct
{
	OTAProfileRead_t         pfnOTAProfileRead;  
    OTAProfileWrite_t        pfnOTAProfileWrite;  
} OTAProfileCBs_t;


/* OTA操作Flash的保护状态变量 */
extern uint8 op_codeflash_access_flag1;
extern uint8 op_codeflash_access_flag2;

/* OTA操作DataFlash的保护状态变量 */
extern uint8 op_dataflash_access_flag1;
extern uint8 op_dataflash_access_flag2;





/*********************************************************************
 * API FUNCTIONS 
 */

/*******************************************************************************
* Function Name  : OTAProfile_AddService
* Description    : OTA Profile初始化
* Input          : services：服务控制字
* Output         : None
* Return         : 初始化的状态
*******************************************************************************/
bStatus_t OTAProfile_AddService( uint32 services );
/*******************************************************************************
* Function Name  : OTAProfile_RegisterAppCBs
* Description    : OTA Profile读写回调函数注册
* Input          : appCallbacks：函数结构体指针
* Output         : None
* Return         : 执行的状态
*******************************************************************************/
bStatus_t OTAProfile_RegisterAppCBs( OTAProfileCBs_t *appCallbacks );
/*******************************************************************************
* Function Name  : OTAProfile_SendData
* Description    : OTA Profile通道发送数据
* Input          : paramID：OTA通道选择
				   p_data：数据指针
				   send_len：发送数据长度
* Output         : None
* Return         : 函数执行状态
*******************************************************************************/
bStatus_t OTAProfile_SendData(unsigned char paramID ,unsigned char *p_data, unsigned char send_len );
/*******************************************************************************
* Function Name  : OTA_CodeFlash_BlockErase
* Description    : CodeFlash 块擦除，一次擦除512B
* Input          : addr: 32位地址，需要512对齐		   				
* Return         : 1  - 错误
				   0 - 成功
*******************************************************************************/
unsigned char OTA_CodeFlash_BlockErase(unsigned long addr);
/*******************************************************************************
* Function Name  : OTA_CodeFlash_WriteBuf
* Description    : CodeFlash 连续多个双字写入
* Input          : addr: 32位地址，需要4对齐
				   pdat: 待写入数据缓存区首地址
				    len: 待写入数据字节长度
* Return         : 1  - 错误
				   0 - 成功
*******************************************************************************/
unsigned char OTA_CodeFlash_WriteBuf(unsigned long addr,unsigned long* pdat,unsigned short len);
/*******************************************************************************
* Function Name  : OTA_DataFlash_BlockErase
* Description    : DataFlash 块擦除，一次擦除512B
* Input          : addr: 32位地址，需要512对齐		   				
* Return         : 1  - 错误
				   0 - 成功
*******************************************************************************/
unsigned char OTA_DataFlash_BlockErase(unsigned long addr);
/*******************************************************************************
* Function Name  : OTA_DataFlash_WriteBuf
* Description    : DataFlash 连续多个双字写入
* Input          : addr: 32位地址，需要4对齐
				   pdat: 待写入数据缓存区首地址
				    len: 待写入数据字节长度
* Return         : FAILED  - 错误
				   SUCCESS - 成功
*******************************************************************************/
unsigned char OTA_DataFlash_WriteBuf(unsigned long addr,unsigned long* pdat,unsigned short len);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
