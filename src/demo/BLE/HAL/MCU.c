/********************************** (C) COPYRIGHT *******************************
* File Name          : MCU.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/11/12
* Description        : 硬件任务处理函数及BLE和硬件初始化
*******************************************************************************/




/******************************************************************************/
/* 头文件包含 */
#include "CH57x_common.h"
#include "HAL.h"

                                                    
__align(4) u32 MEM_BUF[BLE_MEMHEAP_SIZE/4];
//u8C MacAddr[6] = {0x84,0xC2,0xE4,0x85,0x22,0x11};
tmosTaskID halTaskID;


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
  UINT8 i;
  bleConfig_t cfg;

  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
  R16_CLK_SYS_CFG = RB_CLK_OSC32M_XT|(2<<6)|0x08;			// 32M -> Fsys
  R8_SAFE_ACCESS_SIG = 0;
  tmos_memset(&cfg,0,sizeof(bleConfig_t));
	cfg.MEMAddr  = (u32)MEM_BUF;
	cfg.MEMLen   = (u32)BLE_MEMHEAP_SIZE;
	cfg.SNVAddr  = (u32)BLE_SNV_ADDR;
	cfg.TxNumEvent =  BLE_NUM_PKT_PER_EVT;
//	for(i=0;i<6;i++) cfg.MacAddr[i]  = MacAddr[5-i];
	if( !cfg.MEMAddr || cfg.MEMLen < 4*1024 )while(1);
  i = BLE_LibInit( &cfg );
  if(i){
    PRINT("LIB init error...\n");
    while(1);
  }
  SysTick_Config( SysTick_LOAD_RELOAD_Msk );
  SysTick->CTRL  &= ~SysTick_CTRL_TICKINT_Msk ;   /* disable SysTick IRQ */
  TMOS_RandRegister( SYS_GetSysTickCnt );
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
 * @fn          Hal_Init
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
 void Hal_Init( tmosTaskID id )
{
  halTaskID = id;
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
//  tmos_start_task( halTaskID , HAL_TEST_EVENT ,1000 ); // 添加一个测试任务
}
/******************************** endfile @ mcu ******************************/
