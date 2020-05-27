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
    if( CurrImageFlag != IMAGE_A_FLAG && CurrImageFlag != IMAGE_B_FLAG )
    {
        CurrImageFlag = IMAGE_A_FLAG;
    }
    
    PRINT("Image Flag %02x\n",CurrImageFlag);
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
	
	/* ImageA->ImageB，当前的是ImageB，向量入口是ImageA，需要搬移 */
	if( (CurrImageFlag == IMAGE_B_FLAG) && (vectors_entry < IMAGE_B_START_ADD) )
	{
		unsigned int i;
		unsigned char *p_flash;
		
		/* 读取ImageB的代码第一块 */
		p_flash = (UINT8 *)IMAGE_B_START_ADD;
		for(i=0; i<FLASH_BLOCK_SIZE; i++) flash_buf[i] = p_flash[i];

		/* 擦除ImageA代码的第一块 */
		FlashBlockErase(IMAGE_A_START_ADD);
		
		
		/* 将ImageB中断向量覆盖ImageA中断向量 */
		FlashWriteBuf(IMAGE_A_START_ADD,(PUINT32)&flash_buf[0],FLASH_BLOCK_SIZE);
		
		PRINT("ImageB vectors entry copy complete %08x \n",vectors_entry);
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
#ifdef DEBUG
    GPIOA_SetBits(bTXD1);
    GPIOA_ModeCfg(bTXD1, GPIO_ModeOut_PP_5mA);
    UART1_DefInit( );
#endif   
    PRINT("%s\n",VER_LIB);
    ReadImageFlag();
	ImageVectorsCopy();
	CH57X_BLEInit( );
	HAL_Init( );
	GAPRole_PeripheralInit( );
	Peripheral_Init( ); 
    while(1){
        TMOS_SystemProcess( );
    }
}

/******************************** endfile @ main ******************************/
