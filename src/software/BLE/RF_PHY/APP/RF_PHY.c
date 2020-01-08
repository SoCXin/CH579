/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/11/12
* Description        : 
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include "CONFIG.h"
#include "CH57x_common.h"

#ifdef FLASH_ROM
#include "CH57xBLE_rom.H"
#else
#include "CH57xBLE_LIB.H"
#endif
#include "RF_PHY.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
u8 taskID;
u8 TX_DATA[10] ={1,2,3,4,5,6,7,8,9,0};


void RF_2G4StatusCallBack( u8 sta , u8 crc, u8 *rxBuf )
{
  switch( sta ){
    case TX_MODE_TX_FINISH:
    {
      break;
    }
    case TX_MODE_RX_DATA:
    {
      RF_Shut();
      if( crc){
        PRINT("crc error\n");
      }
      {
        u8 i;
      
        PRINT("tx recv,rssi:%d\n",(s8)rxBuf[0]);
        PRINT("len:%d-",rxBuf[1]);
        for(i=0;i<rxBuf[1];i++) PRINT("%x ",rxBuf[i+2]);
        PRINT("\n");
      }
      break;
    }
    case RX_MODE_RX_DATA:
    {
      if( crc){
        PRINT("crc error\n");
      }
      {
        u8 i;
      
        PRINT("tx recv,rssi:%d\n",(s8)rxBuf[0]);
        PRINT("len:%d-",rxBuf[1]);
        for(i=0;i<rxBuf[1];i++) PRINT("%x ",rxBuf[i+2]);
        PRINT("\n");
      }
      break;
    }
    case RX_MODE_TX_FINISH:
    {
      RF_Shut();
      TX_DATA[0]++;
      PRINT( "#%d\n",RF_Rx( TX_DATA,10));
      break;
    }
  }
  PRINT("STA:%d\n",sta);
}


uint16 RF_ProcessEvent( uint8 task_id, uint16 events )
{
  if ( events & SYS_EVENT_MSG )
  {
    uint8 *pMsg;

    if ( (pMsg = tmos_msg_receive( task_id )) != NULL )
    {
      // Release the TMOS message
      tmos_msg_deallocate( pMsg );
    }
    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }
  if( events & SBP_RF_START_DEVICE_EVT ){
    tmos_start_task( taskID , SBP_RF_PERIODIC_EVT ,1000 );
    return events^SBP_RF_START_DEVICE_EVT;
  }
  if ( events & SBP_RF_PERIODIC_EVT ){
    RF_Shut( );
    RF_Tx( TX_DATA,10);
    tmos_start_task( taskID , SBP_RF_PERIODIC_EVT ,1000 );
    return events^SBP_RF_PERIODIC_EVT;
  }
  return 0;
}

void RF_Init( void )
{
  u8 s;
  rfConfig_t rfConfig;
  
  taskID = TMOS_ProcessEventRegister( RF_ProcessEvent );
  rfConfig.TxAccessAddress =0x66666666;
  rfConfig.RxAccessAddress =0x66666666;
  rfConfig.TxCRCInit =0x555555;
  rfConfig.RxCRCInit =0x555555;
  rfConfig.Channel =8;
  rfConfig.LLEMode =1;
  rfConfig.rfStatusCB = RF_2G4StatusCallBack;
  s = RF_Config( &rfConfig );
  PRINT("rf 2.4g init:%d\n",s);
{ // RX mode
  s = RF_Rx( TX_DATA,10);
  PRINT("RX mode.s=%d\n",s);
}
//{ // TX mode
//  tmos_set_event( taskID , SBP_RF_PERIODIC_EVT );
//}
}

/******************************** endfile @ main ******************************/
