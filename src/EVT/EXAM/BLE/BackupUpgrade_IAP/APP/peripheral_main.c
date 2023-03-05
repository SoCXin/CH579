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

/*******************************************************************************
* Function Name  : SwitchImageFlag
* Description    : �л�dataflash���ImageFlag
* Input          : new_flag���л���ImageFlag
* Output         : none
* Return         : none
*******************************************************************************/
void SwitchImageFlag(UINT8 new_flag)
{
	UINT8  *p_flash;
	UINT16 i;
	UINT8  ver_flag;
	
	/* ��ȡ��һ�� */
	p_flash = (UINT8 *)OTA_DATAFLASH_ADD;
	for(i=0; i<FLASH_BLOCK_SIZE; i++)
	{
		flash_buf[i] = p_flash[i];
	}
	
	/* ������һ�� */
	FlashBlockErase(OTA_DATAFLASH_ADD);

	/* ����Image��Ϣ */
	flash_buf[0] = new_flag;
	
	/* ���DataFlash */
	FlashWriteBuf(OTA_DATAFLASH_ADD, (UINT32 *)flash_buf, FLASH_BLOCK_SIZE);
	
	/* ��ӡ�����Ϣ */
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
		
		/* ������ڰ��� */
		p_flash = (UINT8 *)IMAGE_B_START_ADD;
		for(p=0; p<FLASH_BLOCK_SIZE; p++) flash_buf[p] = p_flash[p];
		FlashBlockErase(IMAGE_A_START_ADD);
		FlashWriteBuf(IMAGE_A_START_ADD,(PUINT32)&flash_buf[0],FLASH_BLOCK_SIZE);
		
    SwitchImageFlag( IMAGE_A_FLAG );
    // ���ٱ��ݴ���
		FlashBlockErase(IMAGE_B_START_ADD);

  }
	/* ��λ�����û��������� */
	SYS_ResetExecute( );

}

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
    if( CurrImageFlag != IMAGE_A_FLAG && CurrImageFlag != IMAGE_B_FLAG && CurrImageFlag != IMAGE_IAP_FLAG)
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
	
	/* ���������Ҫ���� */
	if( (CurrImageFlag == IMAGE_IAP_FLAG) && (vectors_entry < IMAGE_IAP_START_ADD) )
	{
		unsigned int i;
		unsigned char *p_flash;
		
		/* ��ȡImageIAP�Ĵ����һ�� */
		p_flash = (UINT8 *)IMAGE_IAP_START_ADD;
		for(i=0; i<FLASH_BLOCK_SIZE; i++) flash_buf[i] = p_flash[i];

		/* ����ImageA����ĵ�һ�� */
		FlashBlockErase(IMAGE_A_START_ADD);
		
		
		/* ��ImageIAP�ж���������ImageA�ж����� */
		FlashWriteBuf(IMAGE_A_START_ADD,(PUINT32)&flash_buf[0],FLASH_BLOCK_SIZE);
		
		PRINT("ImageIAP vectors entry copy complete %08x \n",vectors_entry);
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
	ReadImageFlag();
	ImageVectorsCopy();
	jump_APP();
}

/******************************** endfile @ main ******************************/
