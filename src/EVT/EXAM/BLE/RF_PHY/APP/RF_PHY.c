/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/11/12
 * Description        : 
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include "CONFIG.h"
#include "CH57x_common.h"
#include "RF_PHY.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
uint8 taskID;
uint8 TX_DATA[10] ={1,2,3,4,5,6,7,8,9,0};


void RF_2G4StatusCallBack(uint8_t sta, uint8_t crc, uint8_t *rxBuf)
{
    switch(sta)
    {
        case TX_MODE_TX_FINISH:
        {
            break;
        }
        case TX_MODE_TX_FAIL:
        {
            break;
        }
        case TX_MODE_RX_DATA:
        {
            if (crc == 0) {
                uint8_t i;

                PRINT("tx recv,rssi:%d\n", (int8_t)rxBuf[0]);
                PRINT("len:%d-", rxBuf[1]);

                for (i = 0; i < rxBuf[1]; i++) {
                    PRINT("%x ", rxBuf[i + 2]);
                }
                PRINT("\n");
            } else {
                if (crc & (1<<0)) {
                    PRINT("crc error\n");
                }

                if (crc & (1<<1)) {
                    PRINT("match type error\n");
                }
            }
            break;
        }
        case TX_MODE_RX_TIMEOUT: // Timeout is about 200us
        {
            break;
        }
        case RX_MODE_RX_DATA:
        {
            if (crc == 0) {
                uint8_t i;

                PRINT("rx recv, rssi: %d\n", (int8_t)rxBuf[0]);
                PRINT("len:%d-", rxBuf[1]);
                
                for (i = 0; i < rxBuf[1]; i++) {
                    PRINT("%x ", rxBuf[i + 2]);
                }
                PRINT("\n");
            } else {
                if (crc & (1<<0)) {
                    PRINT("crc error\n");
                }

                if (crc & (1<<1)) {
                    PRINT("match type error\n");
                }
            }
            break;
        }
        case RX_MODE_TX_FINISH:
        {
            tmos_set_event(taskID, SBP_RF_RF_RX_EVT);
            break;
        }
        case RX_MODE_TX_FAIL:
        {
            tmos_set_event(taskID, SBP_RF_RF_RX_EVT);
            break;
        }
    }
    PRINT("STA: %x\n", sta);
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
  if( events & SBP_RF_START_DEVICE_EVT )
	{
    tmos_start_task( taskID , SBP_RF_PERIODIC_EVT ,1000 );
    return events^SBP_RF_START_DEVICE_EVT;
  }
  if ( events & SBP_RF_PERIODIC_EVT )
	{
    RF_Shut( );
    RF_Tx( TX_DATA,10, 0xFF, 0xFF );
    tmos_start_task( taskID , SBP_RF_PERIODIC_EVT ,1000 );
    return events^SBP_RF_PERIODIC_EVT;
  }
  if( events & SBP_RF_RF_RX_EVT )
  {
    uint8 state;
    RF_Shut();
    TX_DATA[0]++;
    state = RF_Rx( TX_DATA,10, 0xFF, 0xFF );
    PRINT("RX mode.state = %x\n",state);
    return events^SBP_RF_RF_RX_EVT;
  }
  return 0;
}

void RF_Init( void )
{
  uint8 state;
  rfConfig_t rfConfig;
  
  taskID = TMOS_ProcessEventRegister( RF_ProcessEvent );
  rfConfig.TxAccessAddress = 0x71764129;	// ��ֹʹ��0x55555555�Լ�0xAAAAAAAA ( ���鲻����24��λ��ת���Ҳ�����������6��0��1 )
  rfConfig.RxAccessAddress = 0x71764129;	// ��ֹʹ��0x55555555�Լ�0xAAAAAAAA ( ���鲻����24��λ��ת���Ҳ�����������6��0��1 )
  rfConfig.TxCRCInit = 0x555555;
  rfConfig.RxCRCInit = 0x555555;
  rfConfig.Channel = 8;
  rfConfig.LLEMode = LLE_MODE_AUTO;
  rfConfig.rfStatusCB = RF_2G4StatusCallBack;
  state = RF_Config( &rfConfig );
  PRINT("rf 2.4g init: %x\n",state);
	{ // RX mode
		state = RF_Rx( TX_DATA,10, 0xFF, 0xFF );
		PRINT("RX mode.state = %x\n",state);
	}
//	{ // TX mode
//		tmos_set_event( taskID , SBP_RF_PERIODIC_EVT );
//	}
}

/******************************** endfile @ main ******************************/
