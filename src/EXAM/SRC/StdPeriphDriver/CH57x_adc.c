/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_adc.c
 * Author             : WCH
 * Version            : V1.1
 * Date               : 2020/04/01
 * Description 
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"


/*******************************************************************************
* Function Name  : ADC_DataCalib_Rough
* Description    : �������ݴֵ�,��ȡƫ��ֵ
*                  ע��: ʹ�ôֵ�У׼�����뱣֤ PA5(AIN1)����Ϊ��������ģʽ���ܽ��ⲿ��Ҫ�е�ѹ
*                       ����������ADC����ô˺�����ȡУ׼ֵ
* Input          : None
* Return         : ƫ��ֵ
*******************************************************************************/
signed short ADC_DataCalib_Rough( void )        // �������ݴֵ�,��ȡƫ��ֵ
{
    UINT16  i;
    UINT32  sum=0;
    UINT8  ch=0;        // ����ͨ��
    UINT8   ctrl=0;     // ���ݿ��ƼĴ���
    
    ch = R8_ADC_CHANNEL;
    ctrl = R8_ADC_CFG;
    
    ADC_ChannelCfg( 1 );
    R8_ADC_CFG |= RB_ADC_OFS_TEST;      // �������ģʽ
    R8_ADC_CONVERT = RB_ADC_START;
    while( R8_ADC_CONVERT & RB_ADC_START );
    for(i=0; i<16; i++)
    {
        R8_ADC_CONVERT = RB_ADC_START;
        while( R8_ADC_CONVERT & RB_ADC_START );
        sum += (~R16_ADC_DATA)&RB_ADC_DATA;
    }    
    sum = (sum+8)>>4;
    R8_ADC_CFG &= ~RB_ADC_OFS_TEST;      // �رղ���ģʽ
    
    
    R8_ADC_CHANNEL = ch;
    R8_ADC_CFG = ctrl;
    return (2048 - sum); 
}

void ADC_DataCalib_Fine( PUINT16 dat, ADC_SignalPGATypeDef ga )        // ��������ϸ��
{
    UINT16  d = (UINT16)*dat;
    
	switch( ga )
	{
        case ADC_PGA_1_4:         // y=0.973x+55.188  
            *dat = (996*d + 56513 + 512)>>10;
			break;
			
		case ADC_PGA_1_2:         // y=0.974x+55.26 
            *dat = (997*d + 56586 + 512)>>10;
			break;
        
		case ADC_PGA_0:         // y=0.975x+53.63  
            *dat = (998*d + 54917 + 512)>>10;
			break;
			
		case ADC_PGA_2:         // y=0.975x+51.58  
            *dat = (998*d + 52818 + 512)>>10;
			break;
	}    
}

/*******************************************************************************
* Function Name  : ADC_ExtSingleChSampInit
* Description    : �ⲿ�źŵ�ͨ��������ʼ��
* Input          : sp:
					refer to ADC_SampClkTypeDef
				   ga:
					refer to ADC_SignalPGATypeDef
* Return         : None
*******************************************************************************/
void ADC_ExtSingleChSampInit( ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga )
{
    R8_ADC_CFG = RB_ADC_POWER_ON			\
                |RB_ADC_BUF_EN				\
                |( sp<<6 )					\
                |( ga<<4 )	;               
}

/*******************************************************************************
* Function Name  : ADC_ExtDiffChSampInit
* Description    : �ⲿ�źŲ��ͨ��������ʼ��
* Input          : sp:
					refer to ADC_SampClkTypeDef
				   ga:
					refer to ADC_SignalPGATypeDef
* Return         : None
*******************************************************************************/
void ADC_ExtDiffChSampInit( ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga )
{
    R8_ADC_CFG = RB_ADC_POWER_ON			\
                |RB_ADC_DIFF_EN             \
                |( sp<<6 )					\
                |( ga<<4 )	;
}

/*******************************************************************************
* Function Name  : ADC_InterTSSampInit
* Description    : �����¶ȴ�����������ʼ��
* Input          : None
* Return         : None
*******************************************************************************/
void ADC_InterTSSampInit( void )
{
    R8_TEM_SENSOR |= RB_TEM_SEN_PWR_ON;
    R8_ADC_CHANNEL = CH_INTE_VTEMP;
    R8_ADC_CFG = RB_ADC_POWER_ON			\
                |( 2<<4 )	;
}

/*******************************************************************************
* Function Name  : ADC_InterBATSampInit
* Description    : ���õ�ص�ѹ������ʼ��
* Input          : None
* Return         : None
*******************************************************************************/
void ADC_InterBATSampInit( void )
{
    R8_ADC_CHANNEL = CH_INTE_VBAT;
    R8_ADC_CFG = RB_ADC_POWER_ON			\
                |RB_ADC_BUF_EN				\
                |( 0<<4 )	;       // ʹ��-12dBģʽ��
}


/*******************************************************************************
* Function Name  : TouchKey_ChSampInit
* Description    : ��������ͨ��������ʼ��
* Input          : None
* Return         : None
*******************************************************************************/
void TouchKey_ChSampInit( void )
{
    R8_ADC_CFG = RB_ADC_POWER_ON | RB_ADC_BUF_EN | ( 2<<4 );
    R8_TKEY_CTRL = RB_TKEY_PWR_ON;
}

/*******************************************************************************
* Function Name  : ADC_ExcutSingleConver
* Description    : ADCִ�е���ת��
* Input          : None
* Return         : ADCת���������
*******************************************************************************/
UINT16 ADC_ExcutSingleConver( void )
{
    R8_ADC_CONVERT = RB_ADC_START;
    while( R8_ADC_CONVERT & RB_ADC_START );

    return ( R16_ADC_DATA&RB_ADC_DATA );
}

/*******************************************************************************
* Function Name  : TouchKey_ExcutSingleConver
* Description    : TouchKeyת��������
* Input          : d:  Touchkey��ŵ�ʱ�䣬��4bit-�ŵ�ʱ�䣬����8bit-���ʱ��
* Return         : ��ǰTouchKey��Ч����
*******************************************************************************/
UINT16 TouchKey_ExcutSingleConver( UINT8 d )
{
    R8_TKEY_CTRL = RB_TKEY_PWR_ON;
    R8_TKEY_CNT = d;
    while( R8_TKEY_CTRL &  RB_TKEY_ACTION );

    return ( R16_ADC_DATA&RB_ADC_DATA );
}


/*******************************************************************************
* Function Name  : ADC_GetCurrentTS
* Description    : ��ȡ��ǰ�������¶�ֵ���棩
* Input          : ts_v����ǰ�¶ȴ������������
* Return         : ת������¶�ֵ���棩
*******************************************************************************/
int ADC_GetCurrentTS( UINT16 ts_v )
{
    UINT16  vol_ts;
    UINT16  D85_tem, D85_vol;
    UINT16  D25_tem, D25_vol;
    UINT16  temperK;
    UINT32  temp;
    UINT8   sum, sumck;
    int     cal;    
    
    temperK = 64;    // mV/16^C
    vol_ts = (ts_v*1060)>>11;
    temp = (*((PUINT32)ROM_TMP_25C_ADDR));
    D25_tem = temp;
    D25_vol = (temp>>16);
    
    if( D25_vol != 0 ){ // Ĭ��ϵ������
        // T = T85 + (V-V85)*16/D25
        cal =  (D25_tem*temperK + vol_ts*16 + (temperK>>1) - D25_vol*16) / temperK ;    
        return ( cal );
    }
    else{  // ����ϵ������  D25_tem  
    	temp = (*((PUINT32)ROM_TMP_85C_ADDR)); 	
    	sum = (UINT8)(temp>>24);		// ����ֽ�    	
    	sumck = (UINT8)(temp>>16);
    	sumck += (UINT8)(temp>>8);
    	sumck += (UINT8)temp;
    	if( sum != sumck )		return 0xff;		// У��ͳ���
    	
        temperK = D25_tem;      // D25_tem = temperK 
        D85_tem = (UINT16)((temp>>16)&0x00ff); 
        D85_vol = (UINT16)temp; 
        
        // T = T85 + (V-V85)*16/D25
        cal =  (D85_tem*temperK + vol_ts*16 + (temperK>>1) - D85_vol*16) / temperK ;    
        return ( cal );  
    }
}



