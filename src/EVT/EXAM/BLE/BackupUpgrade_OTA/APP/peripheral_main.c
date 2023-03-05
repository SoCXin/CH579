/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.1
 * Date               : 2019/11/05
 * Description        : ����ӻ�Ӧ��������������ϵͳ��ʼ��
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/******************************************************************************/
/* ͷ�ļ����� */
#include "CONFIG.h"
#include "CH57x_common.h"
#include "HAL.h"
#include "GATTprofile.h"
#include "Peripheral.h"
#include "OTA.h"
#include "OTAprofile.h"


/* ��¼��ǰ��Image */
unsigned char CurrImageFlag = 0xff;

__align(8) unsigned char flash_buf[FLASH_BLOCK_SIZE];

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
__align(4) u32 MEM_BUF[BLE_MEMHEAP_SIZE/4];

#if (defined (BLE_MAC)) && (BLE_MAC == TRUE)
u8C MacAddr[6] = {0x84,0xC2,0xE4,0x03,0x02,0x02};
#endif

/* ע�⣺���ڳ���������flash�Ĳ���������ִ�У��������κ��жϣ���ֹ�����жϺ�ʧ�� */
/*******************************************************************************
* Function Name  : ReadImageFlag
* Description    : ��ȡ��ǰ�ĳ����Image��־��DataFlash���Ϊ�գ���Ĭ����ImageA
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ReadImageFlag(void)
{
    UINT8 *p_image_flash;
    
    p_image_flash = (UINT8 *)OTA_DATAFLASH_ADD;
    CurrImageFlag = *p_image_flash;
    
    /* �����һ��ִ�У�����û�и��¹����Ժ���º��ڲ���DataFlash */
    if( CurrImageFlag != IMAGE_A_FLAG && CurrImageFlag != IMAGE_B_FLAG )
    {
        CurrImageFlag = IMAGE_A_FLAG;
    }
    
    PRINT("Image Flag %02x\n",CurrImageFlag);
}

/*******************************************************************************
* Function Name  : ImageVectorsCopy
* Description    : ִ���ж������İ��ƣ�����ImageA����ImageBʱִ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ImageVectorsCopy(void)
{
	unsigned int vectors_entry;
	
	/* ��ȡ��ǰ�ĳ������ڵ�ַ */
	vectors_entry = *(unsigned int *)IMAGE_A_ENTRY_ADD;
	
	/* ImageA->ImageB����ǰ����ImageB�����������ImageA����Ҫ���� */
	if( (CurrImageFlag == IMAGE_B_FLAG) && (vectors_entry < IMAGE_B_START_ADD) )
	{
		unsigned int i;
		unsigned char *p_flash;
		
		/* ��ȡImageB�Ĵ����һ�� */
		p_flash = (UINT8 *)IMAGE_B_START_ADD;
		for(i=0; i<FLASH_BLOCK_SIZE; i++) flash_buf[i] = p_flash[i];

		/* ����ImageA����ĵ�һ�� */
		FlashBlockErase(IMAGE_A_START_ADD);
		
		
		/* ��ImageB�ж���������ImageA�ж����� */
		FlashWriteBuf(IMAGE_A_START_ADD,(PUINT32)&flash_buf[0],FLASH_BLOCK_SIZE);
		
		PRINT("ImageB vectors entry copy complete %08x \n",vectors_entry);
	}
}

/*******************************************************************************
* Function Name  : main
* Description    : ������
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
