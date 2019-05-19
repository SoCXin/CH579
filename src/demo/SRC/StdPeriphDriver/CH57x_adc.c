/********************************** (C) COPYRIGHT *******************************
* File Name          : CH57x_adc.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/12/15
* Description 
*******************************************************************************/

#include "CH57x_common.h"


/*******************************************************************************
* Function Name  : ADC_ExtSingleChSampInit
* Description    : 外部信号单通道采样初始化
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
* Description    : 外部信号差分通道采样初始化
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
* Description    : 内置温度传感器采样初始化
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
* Description    : 内置电池电压采样初始化
* Input          : None
* Return         : None
*******************************************************************************/
void ADC_InterBATSampInit( void )
{
    R8_ADC_CHANNEL = CH_INTE_VBAT;
    R8_ADC_CFG = RB_ADC_POWER_ON			\
                |RB_ADC_BUF_EN				\
                |( 2<<4 )	;
}


/*******************************************************************************
* Function Name  : TouchKey_ChSampInit
* Description    : 触摸按键通道采样初始化
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
* Description    : ADC执行单次转换
* Input          : None
* Return         : ADC转换后的数据
*******************************************************************************/
UINT16 ADC_ExcutSingleConver( void )
{
    R8_ADC_CONVERT |= RB_ADC_START;
    while( R8_ADC_CONVERT & RB_ADC_START );

    return ( R16_ADC_DATA );
}

/*******************************************************************************
* Function Name  : TouchKey_ExcutSingleConver
* Description    : TouchKey转换后数据
* Input          : d:  Touchkey充放电时间，高4bit-放电时间，整个8bit-充电时间
* Return         : 当前TouchKey等效数据
*******************************************************************************/
UINT16 TouchKey_ExcutSingleConver( UINT8 d )
{
    R8_TKEY_CTRL = RB_TKEY_PWR_ON;
    R8_TKEY_CNT = d;
    while( R8_TKEY_CTRL &  RB_TKEY_ACTION );

    return ( R16_ADC_DATA );
}


/*******************************************************************************
* Function Name  : ADC_GetCurrentTS
* Description    : 获取当前采样的温度值（℃）
* Input          : ts_v：当前温度传感器采样输出
* Return         : 转换后的温度值（℃）
*******************************************************************************/
UINT8 ADC_GetCurrentTS( UINT16 ts_v )
{
    UINT16  vol_ts;
    UINT16  D85_tem, D85_vol, D25;
    
    vol_ts = (ts_v*1060)>>11;
    D85_tem = (UINT16)((*((PUINT32)ROM_TMP_85C_ADDR)>>16)&0x00ff); 
    D85_vol = *((PUINT16)ROM_TMP_85C_ADDR);
    D25 = *((PUINT32)ROM_TMP_25C_ADDR);
    
    
    return ( D85_tem - ( D85_vol - vol_ts + 8 ) * 16 / D25 );

}



