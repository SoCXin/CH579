/********************************** (C) COPYRIGHT *******************************
* File Name          : CH57x_adc.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/12/15
* Description 
*******************************************************************************/

#include "CH57x_common.h"


/*******************************************************************************
* Function Name  : ADC_DataCalib_Rough
* Description    : 采样数据粗调,获取偏差值
*                  注意，使用粗调校准，必须保证 PA5(AIN1)设置为浮空输入模式，管脚外部不要有电压
* Input          : None
* Return         : 偏差值
*******************************************************************************/
signed short ADC_DataCalib_Rough( void )        // 采样数据粗调,获取偏差值
{
    UINT16  i;
    UINT32  sum=0;
    UINT8  ch=0;        // 备份通道
    UINT8   ctrl=0;     // 备份控制寄存器
    
    ch = R8_ADC_CHANNEL;
    ctrl = R8_ADC_CFG;
    
    ADC_ChannelCfg( 1 );
    R8_ADC_CFG |= RB_ADC_OFS_TEST;      // 进入测试模式
    R8_ADC_CONVERT = RB_ADC_START;
    while( R8_ADC_CONVERT & RB_ADC_START );
    for(i=0; i<16; i++)
    {
        R8_ADC_CONVERT = RB_ADC_START;
        while( R8_ADC_CONVERT & RB_ADC_START );
        sum += (~R16_ADC_DATA)&RB_ADC_DATA;
    }    
    sum = (sum+8)>>4;
    R8_ADC_CFG &= ~RB_ADC_OFS_TEST;      // 关闭测试模式
    
    
    R8_ADC_CHANNEL = ch;
    R8_ADC_CFG = ctrl;
    return (2048 - sum); 
}

void ADC_DataCalib_Fine( PUINT16 dat, ADC_SignalPGATypeDef ga )        // 采样数据细调
{
    UINT32  d = (UINT32)*dat;
    
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
                |( 0<<4 )	;       // 使用-12dB模式，
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
    R8_ADC_CONVERT = RB_ADC_START;
    while( R8_ADC_CONVERT & RB_ADC_START );

    return ( R16_ADC_DATA&RB_ADC_DATA );
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

    return ( R16_ADC_DATA&RB_ADC_DATA );
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



