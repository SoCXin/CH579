/********************************** (C) COPYRIGHT *******************************
* File Name          : CH57x_flash.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/12/15
* Description 
*******************************************************************************/

#include "CH57x_common.h"


/*******************************************************************************
* Function Name  : GetUniqueID
* Description    : 获取芯片唯一ID，小端模式，6B-ID， 2B-CKS
* Input          : buf: 存储8字节，前6字节（小端）ID，后2字节（小端）校验和
* Return         : None
*******************************************************************************/
void GetUniqueID(PUINT8 buf)
{
    PUINT8  pID;
    UINT8   i;
    
    pID = (PUINT8)ROM_UUID_ADDR;
    for(i=0; i<8; i++)
    {
        *buf++ = *pID++;
    }    
}

/*******************************************************************************
* Function Name  : GetMACAddress
* Description    : 获取网络MAC，小端模式，6B-MAC
* Input          : buf: 存储6字节，6字节（小端）物理 MAC
* Return         : None
*******************************************************************************/
void GetMACAddress(PUINT8 buf)
{
    PUINT8  pMAC;
    UINT8   i;
    
    pMAC = (PUINT8)ROM_MAC_ADDR;
    for(i=0; i<6; i++)
    {
        *buf++ = *pMAC++;
    }    
}

/*******************************************************************************
* Function Name  : EnableCodeFlash
* Description    : 解锁 CodeFlash 区域擦写权限
* Input          : None
* Return         : None
*******************************************************************************/
void EnableCodeFlash(void)
{
    R8_FLASH_PROTECT = RB_ROM_WE_MUST_10|RB_ROM_CODE_WE;
}

/*******************************************************************************
* Function Name  : EnableDataFlash
* Description    : 解锁 DataFlash 区域擦写权限
* Input          : None
* Return         : None
*******************************************************************************/
void EnableDataFlash(void)
{
    R8_FLASH_PROTECT = RB_ROM_WE_MUST_10|RB_ROM_DATA_WE;
}

/*******************************************************************************
* Function Name  : LockFlashALL
* Description    : 锁定 CodeFlash/DataFlash 全部区域
* Input          : None
* Return         : None
*******************************************************************************/
void LockFlashALL(void)
{
    R8_FLASH_PROTECT = RB_ROM_WE_MUST_10;
}


/*******************************************************************************
* Function Name  : Flash_BlockErase
* Description    : CodeFlash 块擦除，一次擦除512B。使用时，需要和 EnableFlashCode()/EnableDataFlash() + LockFlashALL() 配合使用
* Input          : addr: 32位地址，需要512对齐,此地址为绝对地址
				   	CodeFlash区域：0 -
				   	DataFlash区域：0x3e800 -		   				
* Return         : FAILED  - 错误
                   status  - 擦除错误码
				   SUCCESS - 成功
*******************************************************************************/
UINT8 Flash_BlockErase(UINT32 addr)
{
    UINT8  status = 0;

    if( addr & (0x200-1) )          return FAILED;

    R32_FLASH_ADDR = addr;
    R8_FLASH_COMMAND = ROM_CMD_ERASE;
    status = R8_FLASH_STATUS;
    if( status == RB_ROM_ADDR_OK )  return SUCCESS;
    else                            return status;
}

/*******************************************************************************
* Function Name  : Flash_WriteDW
* Description    : DataFlash 双字写，地址需4字节对齐。使用时，需要和 EnableFlashCode()/EnableDataFlash() + LockFlashALL() 配合使用
* Input          : addr: 32位地址，需要512对齐,此地址为绝对地址
				   	CodeFlash区域：0 -
				   	DataFlash区域：0x3e800 -
				   dat: 32位写入数据
* Return         : FAILED  - 错误
                   status  - 写错误码
				   SUCCESS - 成功
*******************************************************************************/
UINT8 Flash_WriteDW(UINT32 addr, UINT32 dat)
{
    UINT8  status = 0;

    if( addr & (4-1) )                  return FAILED;

    R32_FLASH_ADDR = addr;
    R32_FLASH_DATA = dat;	
    R8_FLASH_COMMAND = ROM_CMD_PROG;		
    status = R8_FLASH_STATUS;
    if( status == RB_ROM_ADDR_OK )  return SUCCESS;
    else                            return status;
}

