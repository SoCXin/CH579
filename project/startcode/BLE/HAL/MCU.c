/********************************** (C) COPYRIGHT *******************************
* File Name          : MCU.c
* Author             : WCH
* Version            : V1.1
* Date               : 2019/11/05
* Description        : 硬件任务处理函数及BLE和硬件初始化
*******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include "CH57x_common.h"
#include "HAL.h"
                                                
tmosTaskID halTaskID;

/*******************************************************************************
 * @fn          Lib_Calibration_LSI
 *
 * @brief       内部32k校准
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void Lib_Calibration_LSI( void )
{
	if( Calibration_LSI() > 2 )
	{
		Calibration_LSI();
	}
}

/*******************************************************************************
 * @fn          CH57X_BLEInit
 *
 * @brief       BLE 库初始化
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void CH57X_BLEInit( void )
{
  uint8 i;
  bleConfig_t cfg;

  if( tmos_memcmp( VER_LIB,VER_FILE,strlen(VER_FILE)) == FALSE ){
    PRINT("head file error...\n");
    while(1);
  }
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
  R16_CLK_SYS_CFG = RB_CLK_OSC32M_XT|(2<<6)|0x08;			// 32M -> Fsys
  R8_SAFE_ACCESS_SIG = 0;
  SysTick_Config( SysTick_LOAD_RELOAD_Msk );
  SysTick->CTRL  &= ~SysTick_CTRL_TICKINT_Msk ;   /* disable SysTick IRQ */
  tmos_memset(&cfg,0,sizeof(bleConfig_t));
	cfg.MEMAddr  		= (u32)MEM_BUF;
	cfg.MEMLen   		= (u32)BLE_MEMHEAP_SIZE;
	cfg.BufMaxLen		= (u32)BLE_BUFF_MAX_LEN;
	cfg.BufNumber		= (u32)BLE_BUFF_NUM;
	cfg.TxNumEvent  = (u32)BLE_TX_NUM_EVENT;
	cfg.TxPower			= (u32)BLE_TX_POWER;
#if (defined (BLE_SNV)) && (BLE_SNV == TRUE)
	cfg.SNVAddr			= (u32)BLE_SNV_ADDR;
#endif
#if( CLK_OSC32K )	
	cfg.SelRTCClock	= (u32)CLK_OSC32K;
#endif
	cfg.ConnectNumber  = (PERIPHERAL_MAX_CONNECTION&3)|(CENTRAL_MAX_CONNECTION<<2);
  cfg.srandCB = SYS_GetSysTickCnt;
#if (defined TEM_SAMPLE)  && (TEM_SAMPLE == TRUE)
  cfg.tsCB = HAL_GetInterTempValue;  // 根据温度变化校准RF和内部RC( 大于7摄氏度 )
#if( CLK_OSC32K )
  cfg.rcCB = Lib_Calibration_LSI; // 内部32K时钟校准  
#endif
#endif
#if (defined (HAL_SLEEP)) && (HAL_SLEEP == TRUE)
  cfg.WakeUpTime = WAKE_UP_RTC_MAX_TIME;  
  cfg.sleepCB = CH57X_LowPower;  // 启用睡眠
#endif
#if (defined (BLE_MAC)) && (BLE_MAC == TRUE)
	for(i=0;i<6;i++) cfg.MacAddr[i]  = MacAddr[5-i];
#endif
	if( !cfg.MEMAddr || cfg.MEMLen < 4*1024 )while(1);
#if (defined 	HAL_SLEEP) && (HAL_SLEEP == TRUE)
	if( (u32)MEM_BUF < (u32)0x20003800 ){
    PRINT("RAM config error...\n");
    while(1);
  }		
#endif
  i = BLE_LibInit( &cfg );
  if(i){
    PRINT("LIB init error code: %x ...\n",i);
    while(1);
  }
}

/*******************************************************************************
 * @fn          HAL_ProcessEvent
 *
 * @brief       硬件层事务处理
 *
 * input parameters
 *
 * @param       task_id.
 * @param       events.
 *
 * output parameters
 *
 * @param       events.
 *
 * @return      None.
 */
tmosEvents HAL_ProcessEvent( tmosTaskID task_id, tmosEvents events )
{
	uint8 * msgPtr;

  if( events & SYS_EVENT_MSG ){  // 处理HAL层消息，调用tmos_msg_receive读取消息，处理完成后删除消息。
		msgPtr = tmos_msg_receive(task_id);
    if( msgPtr ){
      /* De-allocate */
      tmos_msg_deallocate( msgPtr );
    }
    return events ^ SYS_EVENT_MSG;
	}
  if ( events & LED_BLINK_EVENT ){
#if (defined HAL_LED) && (HAL_LED == TRUE)
  	HalLedUpdate( );
#endif // HAL_LED
		return events ^ LED_BLINK_EVENT;
  }
  if( events & HAL_KEY_EVENT ){
#if (defined HAL_KEY) && (HAL_KEY == TRUE)
    HAL_KeyPoll();              /* Check for keys */
    if (!Hal_KeyIntEnable){        
			tmos_start_task( halTaskID, HAL_KEY_EVENT, MS1_TO_SYSTEM_TIME(100) );
    }
		return events ^ HAL_KEY_EVENT;
#endif
  }
  if( events & HAL_REG_INIT_EVENT ){
#if (defined BLE_CALIBRATION_ENABLE) && (BLE_CALIBRATION_ENABLE == TRUE)	// 校准任务，单次校准耗时小于10ms
    BLE_RegInit();  // 校准RF
#if( CLK_OSC32K )	
    Lib_Calibration_LSI();  // 校准内部RC
#endif
    tmos_start_task( halTaskID , HAL_REG_INIT_EVENT ,MS1_TO_SYSTEM_TIME(BLE_CALIBRATION_PERIOD) ); 
		return events ^ HAL_REG_INIT_EVENT;
#endif
  }
  if( events & HAL_TEST_EVENT ){
    PRINT("* ");
    tmos_start_task( halTaskID , HAL_TEST_EVENT ,MS1_TO_SYSTEM_TIME(10000) ); 
		return events ^ HAL_TEST_EVENT;
  }
  return 0;
}

/*******************************************************************************
 * @fn          HAL_Init
 *
 * @brief       硬件初始化
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
 void HAL_Init( )
{
  halTaskID = TMOS_ProcessEventRegister( HAL_ProcessEvent );
  HAL_TimeInit( );
#if (defined HAL_SLEEP) && (HAL_SLEEP == TRUE)
  HAL_SleepInit( );
#endif
#if (defined HAL_LED) && (HAL_LED == TRUE)
  HAL_LedInit( );
#endif
#if (defined HAL_KEY) && (HAL_KEY == TRUE)
  HAL_KeyInit( );
#endif
  __enable_irq();
#if (defined BLE_CALIBRATION_ENABLE) && (BLE_CALIBRATION_ENABLE == TRUE)
	tmos_start_task( halTaskID , HAL_REG_INIT_EVENT ,MS1_TO_SYSTEM_TIME(BLE_CALIBRATION_PERIOD) );	// 添加校准任务，单次校准耗时小于10ms
#endif
//  tmos_start_task( halTaskID , HAL_TEST_EVENT ,1000 ); // 添加一个测试任务
}

/*******************************************************************************
 * @fn          LLE_IRQHandler
 *
 * @brief       LLE interrupt function 
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void LLE_IRQHandler(void)
{
  BLE_IRQHandler();
}

/*******************************************************************************
 * @fn          HAL_GetInterTempValue
 *
 * @brief       None.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
uint16 HAL_GetInterTempValue( void )
{
  uint8 sensor,channel,config;
  uint16 adc_data;
  
  sensor  = R8_TEM_SENSOR;
  channel = R8_ADC_CHANNEL;
  config  = R8_ADC_CFG;
  R8_TEM_SENSOR |= RB_TEM_SEN_PWR_ON;
  R8_ADC_CHANNEL = CH_INTE_VTEMP;
  R8_ADC_CFG = RB_ADC_POWER_ON|( 2<<4 )	;
  R8_ADC_CONVERT |= RB_ADC_START;
  while( R8_ADC_CONVERT & RB_ADC_START );
  adc_data = R16_ADC_DATA;
  R8_TEM_SENSOR  = sensor;
  R8_ADC_CHANNEL = channel;
  R8_ADC_CFG = config;
  return( adc_data );
}

/******************************** endfile @ mcu ******************************/
