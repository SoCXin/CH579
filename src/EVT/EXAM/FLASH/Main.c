/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0 
 * Date               : 2018/12/15
 * Description 		 : Ƭ��flash��ʾ������Code����DataFlash���Ĳ�/��/д
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"

#define  TEST_CODESIZE          100
#define  TEST_CODEADDR          0x00006000
#define  TEST_DATAFLASHSIZE     4
#define  TEST_DATAFLASHADDR     0x0400

UINT32   TestBuf[ TEST_CODESIZE ];

void DebugInit(void)		
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

int main()
{ 
    UINT16   i;
    PUINT32  p32;
    UINT8    buf[8];
    PUINT16  p16;

/* ���ô��ڵ��� */   
    DebugInit();
    PRINT( "Start @ChipID=%02X\n", R8_CHIP_ID );
    
/* ��ȡUID */
    PRINT( "GetUniqueID: ");     
    GetUniqueID( buf );
    p16 = (PUINT16)buf;
    if( (UINT16)*(p16+3) == (UINT16)(*(p16+0) +  *(p16+1) + *(p16+2)) )     // �ж�У���
    {
        for(i=0; i<6; i++)
        {
            PRINT("%02x ", buf[i]);
        }PRINT("\n");
    }
    else
    {
        PRINT( "UniqueID is error or none !!! \n");  
    }
    

/* ��ȡ����MAC��ַ */  
    PRINT( "GetMACAddress: ");     
    GetMACAddress( buf );
    for(i=0; i<6; i++)
    {
        PRINT("%02x ", buf[i]);
    }PRINT("\n");
    
#if 1   
/* ���� code ��д�� */
    p32 = (PUINT32)TEST_CODEADDR;
    PRINT( "Read addr=0x%08lx :\n", (UINT32)p32 );    
    for(i=0; i<TEST_CODESIZE; i++)
    {
        PRINT("%08lx ",*p32++);
    }PRINT("\n");
    
    p32 = (PUINT32)TEST_CODEADDR;
    PRINT( "Earse addr=0x%08lx : ", (UINT32)p32 );
    i = FlashBlockErase( (UINT32)p32 );
    if( i == SUCCESS )  PRINT("success..\n");
    else                PRINT("failed..\n");
 
    p32 = (PUINT32)TEST_CODEADDR;
    PRINT( "Read addr=0x%08lx :\n", (UINT32)p32 );    
    for(i=0; i<TEST_CODESIZE; i++)
    {
        PRINT("%08lx ",*p32++);
    }PRINT("\n");
    
    for(i=0; i<TEST_CODESIZE; i++)
    {
        TestBuf[i] = i;
        TestBuf[i] = (TestBuf[i]<<8) | i;
        TestBuf[i] = (TestBuf[i]<<8) | i;
        TestBuf[i] = (TestBuf[i]<<8) | i;
    }
    
    p32 = (PUINT32)TEST_CODEADDR;
    PRINT( "Write addr=0x%08lx : ", (UINT32)p32 );
    i = FlashWriteBuf( (UINT32)p32, TestBuf, TEST_CODESIZE*4 );
    if( i == SUCCESS )  PRINT("success..\n");
    else                PRINT("failed..\n");  

    p32 = (PUINT32)TEST_CODEADDR;
    PRINT( "Read addr=0x%08lx :\n", (UINT32)p32 );    
    for(i=0; i<TEST_CODESIZE; i++)
    {
        PRINT("%08lx ",*p32++);
    }PRINT("\n");
 
#endif    

#if 1    
/* ���� dataflash ��д�� ע��dataflash ��ʼ��ַ0x3E800 */
    p32 = (PUINT32)(TEST_DATAFLASHADDR + DATA_FLASH_ADDR) ;
    PRINT( "Read dataflash addr=0x%08lx :\n", (UINT32)p32 );    
    for(i=0; i<TEST_DATAFLASHSIZE; i++)
    {
        PRINT("%08lx ",*p32++);
    }PRINT("\n");
    
    p32 = (PUINT32)(TEST_DATAFLASHADDR + DATA_FLASH_ADDR);
    PRINT( "Earse dataflash addr=0x%08lx : ", (UINT32)p32 );
    i = FlashBlockErase( (UINT32)p32 );
    if( i == SUCCESS )  PRINT("success..\n");
    else                PRINT("failed..\n");
    
    for(i=0; i<TEST_DATAFLASHSIZE; i++)
    {
        TestBuf[i] = (i+0x31);
        TestBuf[i] = (TestBuf[i]<<8) | (i+0x31);
        TestBuf[i] = (TestBuf[i]<<8) | (i+0x31);
        TestBuf[i] = (TestBuf[i]<<8) | (i+0x31);
    }
    
    p32 = (PUINT32)(TEST_DATAFLASHADDR + DATA_FLASH_ADDR);
    PRINT( "Write dataflash addr=0x%08lx : ", (UINT32)p32 );
    for(i=0; i<TEST_DATAFLASHSIZE; i++)
    {
        if( FlashWriteDW( (UINT32)p32++, TestBuf[i])!=SUCCESS )  
        {
            break;
        }            
    }
    if( i==TEST_DATAFLASHSIZE ) PRINT("success..\n");
    else                        PRINT("failed..\n");
    
    p32 = (PUINT32)(TEST_DATAFLASHADDR + DATA_FLASH_ADDR);
    PRINT( "Read dataflash addr=0x%08lx :\n", (UINT32)p32 );    
    for(i=0; i<TEST_DATAFLASHSIZE; i++)
    {
        PRINT("%08lx ",*p32++);
    }PRINT("\n");    
 
#endif

    
    while(1);    
}






