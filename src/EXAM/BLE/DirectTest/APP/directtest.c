/********************************** (C) COPYRIGHT *******************************
* File Name          : directtest.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/11/12
* Description        : 直接测试程序，测试指定通信频道发送数据包
*******************************************************************************/




/******************************************************************************/
/* 头文件包含 */
#include "CONFIG.h"
#include "CH57x_common.h"
#include "HAL.h"
#include "directtest.h"


static tmosTaskID testTaskID;
#if BLE_DIRECT_TEST
static u8 TestEnalbe = FALSE;
#endif


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
tmosEvents TEST_ProcessEvent( tmosTaskID task_id, tmosEvents events )
{
  static u8 payload=0;
	uint8 * msgPtr;
  keyChange_t *msgKeyPtr;

  if( events & SYS_EVENT_MSG ){  // 处理HAL层消息，调用tmos_msg_receive读取消息，处理完成后删除消息。
		msgPtr = tmos_msg_receive(task_id);
    if( msgPtr ){
			switch(  ((tmos_event_hdr_t*)msgPtr)->event){
        case KEY_CHANGE:
          msgKeyPtr = (keyChange_t *)msgPtr;
          if( msgKeyPtr->keys&HAL_KEY_SW_1 ){
            if( TestEnalbe == FALSE ){ 
              payload++;
              TestEnalbe = TRUE;
              HalLedBlink( HAL_LED_1, 0xff, 30 , 4000);
              API_LE_TransmitterTestCmd( 0, 20, payload&7, 0x15|0x80 );
              PRINT("(key)test start ...\n");
            }
            else{
              TestEnalbe = FALSE;
              HalLedSet( HAL_LED_1, HAL_LED_MODE_OFF );
              API_LE_TestEndCmd( );
              PRINT("   (key)end!\n");
            }
            tmos_stop_task( testTaskID, TEST_EVENT );
          }
          break;
        default:
          break;
      }
      /* De-allocate */
      tmos_msg_deallocate( msgPtr );
    }
    return events ^ SYS_EVENT_MSG;
	}
  if( events & TEST_EVENT ){
    if( TestEnalbe == FALSE ){ 
      payload++;
      TestEnalbe = TRUE;
      HalLedBlink( HAL_LED_1, 0xff, 30 , 4000);
      API_LE_TransmitterTestCmd( 0, 20, payload&7, 0x15|0x80 );
      tmos_start_task( testTaskID , TEST_EVENT ,MS1_TO_SYSTEM_TIME(20*1000) ); // 测试时间20s
      PRINT("test start ...\n");
    }
    else{
      TestEnalbe = FALSE;
      HalLedSet( HAL_LED_1, HAL_LED_MODE_OFF );
      API_LE_TestEndCmd();
      PRINT("   end!\n");
    }
		return events ^ TEST_EVENT;
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
 void TEST_Init( )
{
  testTaskID = TMOS_ProcessEventRegister(TEST_ProcessEvent);
#if (defined HAL_KEY) && (HAL_KEY == TRUE)
  HAL_KEY_RegisterForKeys(testTaskID);
#endif
#if BLE_DIRECT_TEST
  tmos_start_task( testTaskID , TEST_EVENT ,1000 ); // 添加一个测试任务
#endif
}
/******************************** endfile @ mcu ******************************/
