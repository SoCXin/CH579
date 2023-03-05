/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2020/05/27
 * Description 		 : ϵͳ˯��ģʽ��������ʾ��GPIOA_6��GPIOA_5��Ϊ����Դ����6��˯�ߵȼ�
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/* 
ע�⣺ʹ�õ͹��ģ�Idleģʽ���⣩˯�߻��Ѻ���Ҫ��������ϵͳʱ�ӣ�����ʹ�õ����ڲ�32Mʱ��5��Ƶ��6.4M��Ƶ 
�л����ⲿHSEֱ������Ƶ���߼�ӷ�ʽ������PLL��ͨ��PLL��Ƶ����Ƶ�������Ѻ���ʱ1.2ms��һ�㣩�����л�����HSE�����ȶ���
�л����ڲ�HSIֱ������Ƶ���߼�ӷ�ʽ������PLL��ͨ��PLL��Ƶ����Ƶ�������Ѻ�����ȴ�ʱ�䡣

ע�⣺�л���HSEʱ��Դ������ȴ��ȶ�ʱ���ѡ������þ�������йأ�ѡ��һ���µľ�������Ķ������ṩ�ľ��弰��
���ص��ݲ���ֵ��ͨ������R8_XT32M_TUNE�Ĵ������������ò�ͬ�ĸ��ص��ݺ�ƫ�õ��������������ȶ�ʱ�䡣
*/

#include "CH57x_common.h"

void DebugInit(void)		
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

int main()
{
    DelayMs(2); 
    SetSysClock( CLK_SOURCE_HSE_32MHz );            // �����ⲿ32M����Ƶ
    GPIOA_ModeCfg( GPIO_Pin_All, GPIO_ModeIN_PU );
    GPIOB_ModeCfg( GPIO_Pin_All, GPIO_ModeIN_PU );
    
    
/* ���ô��ڵ��� */   
    DebugInit();
    PRINT( "Start @ChipID=%02x\n", R8_CHIP_ID );
    DelayMs(200); 

#if 1    
    /* ���û���ԴΪ GPIO - PA6&PA5 */
    GPIOA_ModeCfg( GPIO_Pin_6|GPIO_Pin_5, GPIO_ModeIN_PU );
    GPIOA_ITModeCfg( GPIO_Pin_6|GPIO_Pin_5, GPIO_ITMode_FallEdge );        // �½��ػ���
    NVIC_EnableIRQ( GPIO_IRQn );
    PWR_PeriphWakeUpCfg( ENABLE, RB_SLP_GPIO_WAKE );
#endif    
    
#if 1
    PRINT( "IDLE mode sleep \n");   
    DelayMs(1);
    LowPower_Idle();
    PRINT( "wake.. \n"); 
    DelayMs(500);
#endif    
    
#if 1
    PRINT( "Halt_1 mode sleep \n");   
    DelayMs(1);
    LowPower_Halt_1();
/*
ʹ��HSI/5=6.4M˯�ߣ�����ʱ������Ҫ 2048Tsys��330us
HSE����һ�㲻����1.2ms(500us-1200us)�������л����ⲿHSE����Ҫ 1.2ms-330us ���ʱ����Ա�֤HSE�㹻�ȶ���һ����������
DelayUs()����ʱ����32Mʱ�ӵ���д����ʱ��ƵΪ6.4M������ DelayUs((1200-330)/5)    
*/
    if(!(R8_HFCK_PWR_CTRL&RB_CLK_XT32M_PON)) {     // �Ƿ�HSE�ϵ�
        PWR_UnitModCfg( ENABLE, UNIT_SYS_HSE );   // HSE�ϵ�
        DelayUs((1200)/5);
    }
    else if(!(R16_CLK_SYS_CFG&RB_CLK_OSC32M_XT)){   // �Ƿ�ѡ�� HSI/5 ��ʱ��Դ
        DelayUs((1200-330)/5);
    } 
    HSECFG_Current( HSE_RCur_100 );     // ��Ϊ�����(�͹��ĺ�����������HSEƫ�õ���)
    DelayUs(5/5);                       // �ȴ��ȶ� 1-5us
    SetSysClock( CLK_SOURCE_HSE_32MHz );
    PRINT( "wake.. \n"); 
    DelayMs(500);    
#endif    
    
#if 1
    PRINT( "Halt_2 mode sleep \n");   
    DelayMs(1);
    LowPower_Halt_2();
/*
ʹ��HSI/5=6.4M˯�ߣ�����ʱ������Ҫ 2048Tsys��330us
HSE����һ�㲻����1.2ms(500us-1200us)�������л����ⲿHSE����Ҫ 1.2ms-330us ���ʱ����Ա�֤HSE�㹻�ȶ���һ����������
DelayUs()����ʱ����32Mʱ�ӵ���д����ʱ��ƵΪ6.4M������ DelayUs((1200-330)/5)    
*/
    if(!(R8_HFCK_PWR_CTRL&RB_CLK_XT32M_PON)) {     // �Ƿ�HSE�ϵ�
        PWR_UnitModCfg( ENABLE, UNIT_SYS_HSE );   // HSE�ϵ�
        DelayUs((1200)/5);
    }
    else if(!(R16_CLK_SYS_CFG&RB_CLK_OSC32M_XT)){   // �Ƿ�ѡ�� HSI/5 ��ʱ��Դ
        DelayUs((1200-330)/5);
    } 
    HSECFG_Current( HSE_RCur_100 );     // ��Ϊ�����(�͹��ĺ�����������HSEƫ�õ���)
    DelayUs(5/5);                       // �ȴ��ȶ� 1-5us
    SetSysClock( CLK_SOURCE_HSE_32MHz );
    PRINT( "wake.. \n"); 
    DelayMs(500);    
#endif    

#if 1
    PRINT( "sleep mode sleep \n");   
    DelayMs(1);
    LowPower_Sleep( RB_PWR_RAM14K|RB_PWR_RAM2K );       //ֻ����14+2K SRAM ����
/*
ʹ��HSI/5=6.4M˯�ߣ�����ʱ������Ҫ 2048Tsys��330us
HSE����һ�㲻����1.2ms(500us-1200us)�������л����ⲿHSE����Ҫ 1.2ms-330us ���ʱ����Ա�֤HSE�㹻�ȶ���һ����������
DelayUs()����ʱ����32Mʱ�ӵ���д����ʱ��ƵΪ6.4M������ DelayUs((1200-330)/5)    
*/
    if(!(R8_HFCK_PWR_CTRL&RB_CLK_XT32M_PON)) {     // �Ƿ�HSE�ϵ�
        PWR_UnitModCfg( ENABLE, UNIT_SYS_HSE );   // HSE�ϵ�
        DelayUs((1200)/5);
    }
    else if(!(R16_CLK_SYS_CFG&RB_CLK_OSC32M_XT)){   // �Ƿ�ѡ�� HSI/5 ��ʱ��Դ
        DelayUs((1200-330)/5);
    } 
    HSECFG_Current( HSE_RCur_100 );     // ��Ϊ�����(�͹��ĺ�����������HSEƫ�õ���)
    DelayUs(5/5);                       // �ȴ��ȶ� 1-5us
    SetSysClock( CLK_SOURCE_HSE_32MHz );
    PRINT( "wake.. \n");      
    DelayMs(500);
#endif

#if 1
    PRINT( "shut down mode sleep \n");   
    DelayMs(1);
    LowPower_Shutdown( NULL );                          //ȫ���ϵ磬���Ѻ�λ
/* 
   ��ģʽ���Ѻ��ִ�и�λ������������벻�����У�
   ע��Ҫȷ��ϵͳ˯��ȥ�ٻ��Ѳ��ǻ��Ѹ�λ�������п��ܱ��IDLE�ȼ����� 
*/
/*
ʹ��HSI/5=6.4M˯�ߣ�����ʱ������Ҫ 2048Tsys��330us
HSE����һ�㲻����1.2ms(500us-1200us)�������л����ⲿHSE����Ҫ 1.2ms-330us ���ʱ����Ա�֤HSE�㹻�ȶ���һ����������
DelayUs()����ʱ����32Mʱ�ӵ���д����ʱ��ƵΪ6.4M������ DelayUs((1200-330)/5)    
*/
    if(!(R8_HFCK_PWR_CTRL&RB_CLK_XT32M_PON)) {     // �Ƿ�HSE�ϵ�
        PWR_UnitModCfg( ENABLE, UNIT_SYS_HSE );   // HSE�ϵ�
        DelayUs((1200)/5);
    }
    else if(!(R16_CLK_SYS_CFG&RB_CLK_OSC32M_XT)){   // �Ƿ�ѡ�� HSI/5 ��ʱ��Դ
        DelayUs((1200-330)/5);
    } 
    HSECFG_Current( HSE_RCur_100 );     // ��Ϊ�����(�͹��ĺ�����������HSEƫ�õ���)
    DelayUs(5/5);                       // �ȴ��ȶ� 1-5us
    SetSysClock( CLK_SOURCE_HSE_32MHz );
    PRINT( "wake.. \n");
    DelayMs(500);
#endif

    while(1);    
}


void GPIO_IRQHandler(void)
{
    GPIOA_ClearITFlagBit( GPIO_Pin_6|GPIO_Pin_5 );
}




