/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2019/4/29
 * Description 		 : SPI1 Flash读写操作演示
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"
#include "SPIFlash.H"


void DebugInit(void)		
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}


UINT8  buf[512];

int main( )
{
    UINT16    i;
    
    DelayMs(20);
    DebugInit( );                                                             // 调试接口 UART1
    printf("SPI1 Flash demo start ...\n");
    
    
    for(i=0; i!=512; i++)
    {
        buf[i] = i;
    }
    
    SPIFlash_Init();
    EraseExternal4KFlash_SPI(0);
    BlukWriteExternalFlash_SPI(3,512,buf);
    BlukReadExternalFlash_SPI( 0,512,buf );
    
    for(i=0; i!=512; i++)
    {
        printf("%02x ",(UINT16)buf[i]);
    }
    printf("done\n");
    
    while(1);
}




