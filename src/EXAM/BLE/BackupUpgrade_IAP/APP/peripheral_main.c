/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.1
* Date               : 2019/11/05
* Description        : 外设从机应用主函数及任务系统初始化
*******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include "CONFIG.h"
#include "CH57x_common.h"
#include "HAL.h"
#include "GATTprofile.h"
#include "Peripheral.h"
#include "OTA.h"
#include "OTAprofile.h"


/* 记录当前的Image */
unsigned char CurrImageFlag = 0xff;

__align(8) unsigned char flash_buf[FLASH_BLOCK_SIZE];

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
__align(4) u32 MEM_BUF[BLE_MEMHEAP_SIZE/4];

#if (defined (BLE_MAC)) && (BLE_MAC == TRUE)
u8C MacAddr[6] = {0x84,0xC2,0xE4,0x03,0x02,0x02};
#endif

/*******************************************************************************
* Function Name  : SwitchImageFlag
* Description    : 切换dataflash里的ImageFlag
* Input          : new_flag：切换的ImageFlag
* Output         : none
* Return         : none
*******************************************************************************/
void SwitchImageFlag(UINT8 new_flag)
{
	UINT8  *p_flash;
	UINT16 i;
	UINT8  ver_flag;
	
	/* 读取第一块 */
	p_flash = (UINT8 *)OTA_DATAFLASH_ADD;
	for(i=0; i<FLASH_BLOCK_SIZE; i++)
	{
		flash_buf[i] = p_flash[i];
	}
	
	/* 擦除第一块 */
	FlashBlockErase(OTA_DATAFLASH_ADD);

	/* 更新Image信息 */
	flash_buf[0] = new_flag;
	
	/* 编程DataFlash */
	FlashWriteBuf(OTA_DATAFLASH_ADD, (UINT32 *)flash_buf, FLASH_BLOCK_SIZE);
	
	/* 打印输出消息 */
	p_flash = (UINT8 *)OTA_DATAFLASH_ADD;
	ver_flag = p_flash[0];
	PRINT("SwitchImageFlag: %02x \r\n",ver_flag);
}

/*******************************************************************************
 * Function Name  : jump_APP
 * Description    :
 * Input          : None
 * Return         : None
 *******************************************************************************/
void jump_APP( void )
{
  if( CurrImageFlag==IMAGE_IAP_FLAG )
  {
    UINT8 i;
		uint32 p;
		unsigned char *p_flash;
		
    for(i=1; i<IMAGE_A_SIZE/FLASH_BLOCK_SIZE; i++)
    {
			p_flash = (UINT8 *)IMAGE_B_START_ADD+(i*FLASH_BLOCK_SIZE);
			for(p=0; p<FLASH_BLOCK_SIZE; p++) flash_buf[p] = p_flash[p];
			FlashBlockErase(IMAGE_A_START_ADD+(i*FLASH_BLOCK_SIZE));
			FlashWriteBuf(IMAGE_A_START_ADD+(i*FLASH_BLOCK_SIZE),(PUINT32)&flash_buf[0],FLASH_BLOCK_SIZE);
    }
		
		/* 向量入口搬移 */
		p_flash = (UINT8 *)IMAGE_B_START_ADD;
		for(p=0; p<FLASH_BLOCK_SIZE; p++) flash_buf[p] = p_flash[p];
		FlashBlockErase(IMAGE_A_START_ADD);
		FlashWriteBuf(IMAGE_A_START_ADD,(PUINT32)&flash_buf[0],FLASH_BLOCK_SIZE);
		
    SwitchImageFlag( IMAGE_A_FLAG );
    // 销毁备份代码
		FlashBlockErase(IMAGE_B_START_ADD);

  }
	/* 复位，进用户程序运行 */
	SYS_ResetExecute( );

}

/* 注意：关于程序升级后flash的操作必须先执行，不开启任何中断，防止操作中断和失败 */
/*******************************************************************************
* Function Name  : ReadImageFlag
* Description    : 读取当前的程序的Image标志，DataFlash如果为空，就默认是ImageA
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ReadImageFlag(void)
{
    UINT8 *p_image_flash;
    
    p_image_flash = (UINT8 *)OTA_DATAFLASH_ADD;
    CurrImageFlag = *p_image_flash;
    
    /* 程序第一次执行，或者没有更新过，以后更新后在擦除DataFlash */
    if( CurrImageFlag != IMAGE_A_FLAG && CurrImageFlag != IMAGE_B_FLAG && CurrImageFlag != IMAGE_IAP_FLAG)
    {
        CurrImageFlag = IMAGE_A_FLAG;
    }
    
    PRINT("ImageQ Flag %02x\n",CurrImageFlag);
}

/*******************************************************************************
* Function Name  : ImageVectorsCopy
* Description    : 执行中断向量的搬移，仅当ImageA升级ImageB时执行
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ImageVectorsCopy(void)
{
	unsigned int vectors_entry;
	
	/* 读取当前的程序的入口地址 */
	vectors_entry = *(unsigned int *)IMAGE_A_ENTRY_ADD;
	
	/* 向量入口需要搬移 */
	if( (CurrImageFlag == IMAGE_IAP_FLAG) && (vectors_entry < IMAGE_IAP_START_ADD) )
	{
		unsigned int i;
		unsigned char *p_flash;
		
		/* 读取ImageIAP的代码第一块 */
		p_flash = (UINT8 *)IMAGE_IAP_START_ADD;
		for(i=0; i<FLASH_BLOCK_SIZE; i++) flash_buf[i] = p_flash[i];

		/* 擦除ImageA代码的第一块 */
		FlashBlockErase(IMAGE_A_START_ADD);
		
		
		/* 将ImageIAP中断向量覆盖ImageA中断向量 */
		FlashWriteBuf(IMAGE_A_START_ADD,(PUINT32)&flash_buf[0],FLASH_BLOCK_SIZE);
		
		PRINT("ImageIAP vectors entry copy complete %08x \n",vectors_entry);
	}
}

/*******************************************************************************
* Function Name  : main
* Description    : 主函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main( void ) 
{
#if (defined (HAL_SLEEP)) && (HAL_SLEEP == TRUE)
  GPIOA_ModeCfg( GPIO_Pin_All, GPIO_ModeIN_PU );
  GPIOB_ModeCfg( GPIO_Pin_All, GPIO_ModeIN_PU );
#endif
#ifdef DEBUG
	GPIOA_SetBits(bTXD1);
	GPIOA_ModeCfg(bTXD1, GPIO_ModeOut_PP_5mA);
	UART1_DefInit( );
#endif   
	ReadImageFlag();
	ImageVectorsCopy();
	jump_APP();
}

/******************************** endfile @ main ******************************/
