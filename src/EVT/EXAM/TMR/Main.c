/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2019/4/28
 * Description 		 : ��ʱ������
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"

__align(4) UINT32 CapBuf[100];
__align(4) UINT32 PwmBuf[100];

volatile UINT8 capFlag = 0;

void DebugInit(void)		
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

int main()
{     
    UINT8  i;
		UINT32 x = 0;
  
/* ���ô��ڵ��� */   
    DebugInit();
    PRINT( "Start @ChipID=%02X\n", R8_CHIP_ID );
    
#if 0       /* ��ʱ��0���趨100ms��ʱ������IO�����ƣ� PB3-LED */
    
    GPIOB_SetBits( GPIO_Pin_3 );
    GPIOB_ModeCfg( GPIO_Pin_3, GPIO_ModeOut_PP_5mA );
    
    TMR0_TimerInit( FREQ_SYS/10 );                  // ���ö�ʱʱ�� 100ms
    TMR0_ITCfg(ENABLE, TMR0_3_IT_CYC_END);          // �����ж�
    NVIC_EnableIRQ( TMR0_IRQn );
    
#endif 

#if 1       /* ��ʱ��3��PWM��� */
    
    GPIOA_ResetBits( GPIO_Pin_2 );            // ����PWM�� PA2
    GPIOA_ModeCfg( GPIO_Pin_2, GPIO_ModeOut_PP_5mA );
    
    TMR3_PWMInit( High_Level, PWM_Times_1 );
    TMR3_PWMCycleCfg( 3200 );        // ���� 100us
    TMR3_Disable();
    TMR3_PWMActDataWidth( 1000 );              // ռ�ձ����� , �޸�ռ�ձȱ�����ʱ�رն�ʱ��
    TMR3_Enable();
    
#endif   

#if 1       /* ��ʱ��1��CAP��׽�� */
    PWR_UnitModCfg( DISABLE, UNIT_SYS_LSE );     // ע���������LSE�������ţ�Ҫ��֤�رղ���ʹ����������
    GPIOA_ResetBits( GPIO_Pin_10 );             // ����PWM�� PA10
    GPIOA_ModeCfg( GPIO_Pin_10, GPIO_ModeIN_PU );

    TMR1_CapInit( Edge_To_Edge );
    TMR1_CAPTimeoutCfg( 0xFFFFFFFF );   // ���ò�׽��ʱʱ��
    TMR1_DMACfg( ENABLE, (UINT16)(UINT32)&CapBuf[0], (UINT16)(UINT32)&CapBuf[100], Mode_Single );
    TMR1_ClearITFlag( TMR1_2_IT_DMA_END );      // ����жϱ�־  
    TMR1_ITCfg(ENABLE, TMR1_2_IT_DMA_END);          // ����DMA����ж�
    NVIC_EnableIRQ( TMR1_IRQn );
    
    while( capFlag == 0 );
    capFlag = 0;
    for( i=0; i<100; i++ )
    {
        printf("%08ld ", CapBuf[i]&0x1ffffff);      // bit26 ���λ��ʾ �ߵ�ƽ���ǵ͵�ƽ
    }printf("\n");

#endif
#if 1       /* ��ʱ��2�������� */
		GPIOB_ModeCfg( GPIO_Pin_11, GPIO_ModeIN_PD );
		GPIOPinRemap( ENABLE, RB_PIN_TMR2 );
		
		TMR2_CountInit( FallEdge_To_FallEdge );
		TMR2_CountOverflowCfg( 1000 );                  // ���ü�������1000
		
		/* ������������жϣ�����1000�����ڽ����ж� */   
		TMR2_ClearITFlag( TMR0_3_IT_CYC_END );
		NVIC_EnableIRQ(TMR2_IRQn);
		TMR2_ITCfg( ENABLE, TMR0_3_IT_CYC_END);

		do
		{
						/* Լ0.5s��ӡһ�ε�ǰ����ֵ�������������Ƶ�ʽϸߣ����ܺܿ�����������Ҫ��ʵ������޸� */
				x++;
				if( !( x & 0xfffff ) )	printf("=%ld ", TMR2_GetCurrentCount());			
		}while(1); 

#endif

#if 1 /* ��ʱ��2,DMA PWM.*/
    GPIOB_ModeCfg(GPIO_Pin_11, GPIO_ModeOut_PP_5mA);
    GPIOPinRemap(ENABLE, RB_PIN_TMR2);

    PRINT("TMR2 DMA PWM\n");
    TMR2_PWMCycleCfg(120000); // ���� 2000us
    for(i=0; i<50; i++)
    {
      PwmBuf[i]=2400*i;
    }
    for(i=50; i<100; i++)
    {
      PwmBuf[i]=2400*(100-i);
    }
    TMR2_PWMInit(Low_Level, PWM_Times_16);
    /* Note: DMA����PWM��ʼ����������� */
    TMR2_DMACfg(ENABLE, (uint16_t)(uint32_t)&PwmBuf[0], (uint16_t)(uint32_t)&PwmBuf[100], Mode_LOOP);
    /* ������������жϣ�����1000�����ڽ����ж� */
    TMR2_ClearITFlag(TMR1_2_IT_DMA_END);
    NVIC_EnableIRQ(TMR2_IRQn);
    TMR2_ITCfg(ENABLE, TMR1_2_IT_DMA_END);

#endif
    while(1);    
}



void TMR0_IRQHandler( void )        // TMR0 ��ʱ�ж�
{
    if( TMR0_GetITFlag( TMR0_3_IT_CYC_END ) )
    {
        TMR0_ClearITFlag( TMR0_3_IT_CYC_END );      // ����жϱ�־
        GPIOB_InverseBits( GPIO_Pin_3 );        
    }
}

void TMR1_IRQHandler( void )        // TMR1 ��ʱ�ж�
{
    if( TMR1_GetITFlag( TMR1_2_IT_DMA_END ) )
    {
        TMR1_ITCfg(DISABLE, TMR1_2_IT_DMA_END);       // ʹ�õ���DMA����+�жϣ�ע����ɺ�رմ��ж�ʹ�ܣ������һֱ�ϱ��жϡ�
        TMR1_ClearITFlag( TMR1_2_IT_DMA_END );      // ����жϱ�־  
        capFlag = 1;
        printf("*");
    }
}

void TMR2_IRQHandler(void)
{
	if( TMR2_GetITFlag(TMR0_3_IT_CYC_END) ) 
	{
		 TMR2_ClearITFlag( TMR0_3_IT_CYC_END );
		 /* ������������Ӳ���Զ����㣬���¿�ʼ���� */
		 /* �û������������Ҫ�Ĵ��� */
	}
	if(TMR2_GetITFlag(TMR1_2_IT_DMA_END))
	{
			TMR2_ClearITFlag(TMR1_2_IT_DMA_END);
			PRINT("DMA end\n");
			/* DMA ���� */
			/* �û������������Ҫ�Ĵ��� */
	}
}




