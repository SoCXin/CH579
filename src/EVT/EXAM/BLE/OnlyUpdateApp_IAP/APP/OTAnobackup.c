/********************************** (C) COPYRIGHT *******************************
 * File Name          : OTAnobackup.C
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/10
 * Description        : ����ӻ�Ӧ�ó��򣬳�ʼ���㲥���Ӳ�����Ȼ��㲥��ֱ������������ͨ���Զ������������        
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "CONFIG.h"
#include "CH57x_common.h"
#include "devinfoservice.h"
#include "GATTprofile.h"
#include "OTAnobackup.h"
#include "OTA.h"
#include "OTAprofile.h"


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// How often to perform periodic event
#define SBP_PERIODIC_EVT_PERIOD               1000

// What is the advertising interval when device is discoverable (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL          160

// Limited discoverable mode advertises for 30.72s, and then stops
// General discoverable mode advertises indefinitely

#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_GENERAL

// Minimum connection interval (units of 1.25ms, 80=100ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     6

// Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     12

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         0

// Supervision timeout value (units of 10ms, 1000=10s) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT          1000

// Whether to enable automatic parameter update request when a connection is formed
#define DEFAULT_ENABLE_UPDATE_REQUEST         TRUE

// Connection Pause Peripheral time value (in seconds)
#define DEFAULT_CONN_PAUSE_PERIPHERAL         6

// Company Identifier: WCH  
#define WCH_COMPANY_ID                        0x07D7

#define INVALID_CONNHANDLE                    0xFFFF

// Length of bd addr as a string
#define B_ADDR_STR_LEN                        15

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8 Peripheral_TaskID=0xff;   // Task ID for internal task/event processing

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8 scanRspData[31] =
{
  // complete name
  0x12,   // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  'O','T','A',' ','O','T','A',' ','O','T','A',' ','O','T','A',' ','!',   
  // connection interval range
  0x05,   // length of this data
  GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
  LO_UINT16( DEFAULT_DESIRED_MIN_CONN_INTERVAL ),   // 100ms
  HI_UINT16( DEFAULT_DESIRED_MIN_CONN_INTERVAL ),
  LO_UINT16( DEFAULT_DESIRED_MAX_CONN_INTERVAL ),   // 1s
  HI_UINT16( DEFAULT_DESIRED_MAX_CONN_INTERVAL ),

  // Tx power level
  0x02,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,
  0       // 0dBm
};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
static uint8 advertData[] =
{
  // Flags; this sets the device to use limited discoverable
  // mode (advertises for 30 seconds at a time) instead of general
  // discoverable mode (advertises indefinitely)
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  DEFAULT_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

  // service UUID, to notify central devices what services are included
  // in this peripheral
  0x03,   // length of this data
  GAP_ADTYPE_16BIT_MORE,      // some of the UUID's, but not all
  LO_UINT16( SIMPLEPROFILE_SERV_UUID ),
  HI_UINT16( SIMPLEPROFILE_SERV_UUID )

};

// GAP GATT Attributes
static uint8 attDeviceName[GAP_DEVICE_NAME_LEN] = "OTA OTA OTA OTA !";

// OTA IAP VARIABLES
/* OTAͨѶ��֡ */
OTA_IAP_CMD_t iap_rec_data;

/* OTA������� */
__align(8) UINT32 OpParaData[256/4];  
UINT32 OpParaDataLen = 0;
UINT32 OpAdd = 0;

/* flash��һ���������ʱ�洢 */
__align(8) uint8 vectors_block_buf[FLASH_BLOCK_SIZE];

/* codeflash��ʱ���� */
__align(8) uint8 codeflash_block_buf[FLASH_BLOCK_SIZE];

/* Image��ת������ַ���� */
typedef int( *pImageTaskFn)( void );
pImageTaskFn user_image_tasks;

/* Flash �������� */
UINT32 EraseAdd = 0;                   //������ַ
UINT32 EraseBlockNum = 0;              //��Ҫ�����Ŀ���
UINT32 EraseBlockCnt = 0;              //�����Ŀ����

/* FLASH У����� */
UINT8 VerifyStatus = 0;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void Peripheral_ProcessTMOSMsg( tmos_event_hdr_t *pMsg );
static void peripheralStateNotificationCB( gapRole_States_t newState ,gapRoleEvent_t * pEvent );
void OTA_IAPReadDataComplete( unsigned char index );
void OTA_IAPWriteData( unsigned char index, unsigned char *p_data, unsigned char w_len );
void Rec_OTA_IAP_DataDeal(void);
void OTA_IAP_SendCMDDealSta(UINT8 deal_status);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapRolesCBs_t Peripheral_PeripheralCBs =
{
    peripheralStateNotificationCB,  // Profile State Change Callbacks
    NULL,                            // When a valid RSSI is read from controller (not used by application)
    NULL
};

// Simple GATT Profile Callbacks
static OTAProfileCBs_t Peripheral_OTA_IAPProfileCBs =
{
    OTA_IAPReadDataComplete,     // Charactersitic value change callback
	OTA_IAPWriteData
};



// Callback when the connection parameteres are updated.
void PeripheralParamUpdate(uint16 connInterval,uint16 connSlaveLatency, uint16 connTimeout);

gapRolesParamUpdateCB_t PeripheralParamUpdate_t = NULL;


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Peripheral_Init
 *
 * @brief   Initialization function for the Peripheral App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by TMOS.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void Peripheral_Init( )
{
  Peripheral_TaskID = TMOS_ProcessEventRegister(Peripheral_ProcessEvent);

  // Setup the GAP Peripheral Role Profile
  {
		// For other hardware platforms, device starts advertising upon initialization
     uint8 initial_advertising_enable = TRUE;

    // Set the GAP Role Parameters
    GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &initial_advertising_enable );
    GAPRole_SetParameter( GAPROLE_SCAN_RSP_DATA, sizeof ( scanRspData ), scanRspData );
    GAPRole_SetParameter( GAPROLE_ADVERT_DATA, sizeof( advertData ), advertData );
  }

  // Set the GAP Characteristics
  GGS_SetParameter( GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName );

  // Set advertising interval
  {
    uint16 advInt = DEFAULT_ADVERTISING_INTERVAL;

    GAP_SetParamValue( TGAP_DISC_ADV_INT_MIN, advInt );
    GAP_SetParamValue( TGAP_DISC_ADV_INT_MAX, advInt );
  }
  // Initialize GATT attributes
  GGS_AddService( GATT_ALL_SERVICES );            // GAP
  GATTServApp_AddService( GATT_ALL_SERVICES );    // GATT attributes
  OTAProfile_AddService( GATT_ALL_SERVICES );
  
  //  Register callback with OTAGATTprofile
  OTAProfile_RegisterAppCBs( &Peripheral_OTA_IAPProfileCBs );
  
	// Setup a delayed profile startup
  tmos_set_event( Peripheral_TaskID, SBP_START_DEVICE_EVT );

}

/*********************************************************************
 * @fn      Peripheral_ProcessEvent
 *
 * @brief   Peripheral Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The TMOS assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
uint16 Peripheral_ProcessEvent( uint8 task_id, uint16 events )
{

//  VOID task_id; // TMOS required parameter that isn't used in this function

  if ( events & SYS_EVENT_MSG ){
    uint8 *pMsg;

    if ( (pMsg = tmos_msg_receive( Peripheral_TaskID )) != NULL ){
      Peripheral_ProcessTMOSMsg( (tmos_event_hdr_t *)pMsg );
      // Release the TMOS message
      tmos_msg_deallocate( pMsg );
    }
    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & SBP_START_DEVICE_EVT ){
	// Start the Device
    GAPRole_PeripheralStartDevice( Peripheral_TaskID, NULL, &Peripheral_PeripheralCBs );
    // Set timer for first periodic event
    tmos_start_task( Peripheral_TaskID, SBP_PERIODIC_EVT, SBP_PERIODIC_EVT_PERIOD );
    return ( events ^ SBP_START_DEVICE_EVT );
  }

	//OTA_FLASH_ERASE_EVT
	if ( events & OTA_FLASH_ERASE_EVT )
	{
		UINT8 status;
		
		PRINT("ERASE:%08x num:%d\r\n",(int)(EraseAdd+EraseBlockCnt*FLASH_BLOCK_SIZE),(int)EraseBlockCnt);
		status = FlashBlockErase(EraseAdd+EraseBlockCnt*FLASH_BLOCK_SIZE);
		
		/* ����ʧ�� */
		if(status != SUCCESS)
		{
			OTA_IAP_SendCMDDealSta(status);
			return (events ^ OTA_FLASH_ERASE_EVT);
		}
		
		EraseBlockCnt++;
		
		/* �������� */
		if( EraseBlockCnt >= EraseBlockNum )
		{
			PRINT("ERASE Complete\r\n");
			OTA_IAP_SendCMDDealSta(status);
			return (events ^ OTA_FLASH_ERASE_EVT);
		}
		return (events);
	}

	// Discard unknown events
	return 0;
}

/*********************************************************************
 * @fn      Peripheral_ProcessTMOSMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void Peripheral_ProcessTMOSMsg( tmos_event_hdr_t *pMsg )
{
  switch ( pMsg->event ){
		default:
			break;
  }
}

/*********************************************************************
 * @fn      peripheralStateNotificationCB
 *
 * @brief   Notification from the profile of a state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void peripheralStateNotificationCB( gapRole_States_t newState ,gapRoleEvent_t * pEvent )
{
  switch ( newState )
  {
    case GAPROLE_STARTED:
      PRINT( "Initialized \n" );
		break;

    case GAPROLE_ADVERTISING:
      PRINT( "Advertising \n" );
      break;

    case GAPROLE_CONNECTED:
			PRINT( "Connected \n" );
			break;

    case GAPROLE_WAITING:
      PRINT( "Disconnected %x\n", pEvent->linkTerminate.reason);
			{
				uint8 initial_advertising_enable = TRUE;

				// Set the GAP Role Parameters
				GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &initial_advertising_enable );
			}
      break;

    default:
      break;

  }
}

/*******************************************************************************
* Function Name  : OTA_IAP_SendData
* Description    : OTA IAP�������ݣ�ʹ��ʱ����20�ֽ�����
* Input          : p_send_data���������ݵ�ָ��
                   send_len���������ݵĳ���
* Output         : none
* Return         : none
*******************************************************************************/
void OTA_IAP_SendData(UINT8 *p_send_data,UINT8 send_len)
{
	OTAProfile_SendData(OTAPROFILE_CHAR ,p_send_data, send_len );
}

/*******************************************************************************
* Function Name  : OTA_IAP_SendCMDDealSta
* Description    : OTA IAPִ�е�״̬����
* Input          : deal_status�����ص�״̬
* Output         : none
* Return         : none
*******************************************************************************/
void OTA_IAP_SendCMDDealSta(UINT8 deal_status)
{
	UINT8 send_buf[2];
	
	send_buf[0] = deal_status;
	send_buf[1] = 0;
	OTA_IAP_SendData(send_buf,2);
}

/*******************************************************************************
* Function Name  : OTA_IAP_CMDErrDeal
* Description    : OTA IAP�쳣�����봦��
* Input          : none
* Output         : none
* Return         : none
*******************************************************************************/
void OTA_IAP_CMDErrDeal(void)
{
	OTA_IAP_SendCMDDealSta(0xfe);
}

/*******************************************************************************
* Function Name  : CopyOTAFrist512ByteToAPP
* Description    : CopyOTAFrist512ByteToAPP
* Input          : none
* Output         : none
* Return         : none
*******************************************************************************/
void CopyOTAFrist512ByteToAPP( )
{
	UINT8  *p_flash;
	UINT16 i;
	
	/* ��ȡ��һ�� */
	p_flash = (UINT8 *)IMAGE_OTA_START_ADD;
	for(i=0; i<FLASH_BLOCK_SIZE; i++)
	{
		codeflash_block_buf[i] = p_flash[i];
	}
	
	/* ������һ�� */
	FlashBlockErase(IMAGE_APP_START_ADD);

	/* ���CodeFlash */
	FlashWriteBuf(IMAGE_APP_START_ADD, (UINT32 *)codeflash_block_buf, FLASH_BLOCK_SIZE);
}

/*******************************************************************************
* Function Name  : DisableAllIRQ
* Description    : �ر����е��ж�
* Input          : none
* Output         : none
* Return         : none
*******************************************************************************/
void DisableAllIRQ(void)
{
	NVIC->ICER[0] = 0xffffffff;
	SysTick->CTRL = 0;      //�ر�SysTick�ж�
}

/*******************************************************************************
* Function Name  : GotoResetVector
* Description    : ��ȡ��ת��ַ������ת
* Input          : entry_add��Reset������ַ
* Output         : none
* Return         : none
*******************************************************************************/
void GotoResetVector(UINT32 entry_add)
{
	user_image_tasks = (pImageTaskFn)(*(UINT32 *)(entry_add));
	(user_image_tasks)(); 
}

/*******************************************************************************
* Function Name  : Rec_OTA_IAP_DataDeal
* Description    : ���յ�OTA���ݰ�����
* Input          : none
* Output         : none
* Return         : none
*******************************************************************************/
void Rec_OTA_IAP_DataDeal(void)
{
    switch(iap_rec_data.other.buf[0])
    {
		/* ��� */
		case CMD_IAP_PROM:
		{
			UINT32 i;
			UINT8 status;
			uint8 *p_flash;
			OpParaDataLen = iap_rec_data.program.len;   
			
			OpAdd = (UINT32)(iap_rec_data.program.addr[0]);
			OpAdd |= ((UINT32)(iap_rec_data.program.addr[1]) << 8);
			OpAdd = OpAdd * 4;

			PRINT("IAP_PROM: %08x len:%d \r\n",(int)OpAdd,(int)OpParaDataLen);

			for(i=0; i<(OpParaDataLen/4); i++)
			{
				OpParaData[i] = (UINT32)(iap_rec_data.program.buf[0+4*i]);
				OpParaData[i] |= ((UINT32)(iap_rec_data.program.buf[1+4*i]) << 8);
				OpParaData[i] |= ((UINT32)(iap_rec_data.program.buf[2+4*i]) << 16);
				OpParaData[i] |= ((UINT32)(iap_rec_data.program.buf[3+4*i]) << 24);
			}
			
			/* ��һ������ */
			if(OpAdd < FLASH_BLOCK_SIZE)
			{
				for(i = 0; (OpAdd+i < FLASH_BLOCK_SIZE) && (i<OpParaDataLen); i++)
				{
					vectors_block_buf[OpAdd + i] = iap_rec_data.program.buf[i];
				}
				status = SUCCESS;
				if( i== OpParaDataLen )
				{
					OTA_IAP_SendCMDDealSta(status);
				}
				else
				{
					OpParaDataLen -= i;
					p_flash = (UINT8 *)(OpAdd+i);
					status = FlashWriteBuf(OpAdd+i, (PUINT32)&OpParaData[i/4], (UINT16) OpParaDataLen);
					OTA_IAP_SendCMDDealSta(status);
				}
			}
			/* ������ */
			else
			{
				p_flash = (UINT8 *)(OpAdd);
				status = FlashWriteBuf(OpAdd, (PUINT32)OpParaData, (UINT16) OpParaDataLen);
				OTA_IAP_SendCMDDealSta(status);
			}
			
			break;
		}
		/* ���� -- ������������������ */
		case CMD_IAP_ERASE:
		{
			/* һ����ʼ����flash����IAP����ĵ�һ��flash����APP�����һ��flash */
			CopyOTAFrist512ByteToAPP();

			OpAdd = (UINT32)(iap_rec_data.erase.addr[0]);
			OpAdd |= ((UINT32)(iap_rec_data.erase.addr[1]) << 8);
			OpAdd = OpAdd * 4;

			EraseBlockNum = (UINT32)(iap_rec_data.erase.block_num[0]);
			EraseBlockNum |= ((UINT32)(iap_rec_data.erase.block_num[1]) << 8);
			EraseAdd = OpAdd;
			EraseBlockCnt = 0;
			
			/* ����ͷ��ڲ�������0 */
			VerifyStatus = 0;
			
			PRINT("IAP_ERASE start:%08x num:%d\r\n",(int)OpAdd,(int)EraseBlockNum);
			
			EraseBlockNum -= 1;
			EraseAdd += FLASH_BLOCK_SIZE;

			/* �������� */
			tmos_set_event( Peripheral_TaskID, OTA_FLASH_ERASE_EVT );

			break;
		}
		/* У�� */
		case CMD_IAP_VERIFY:
		{
			UINT32 i;
			UINT8 *p_flash;
			UINT8 status = 0;
			
			OpParaDataLen = iap_rec_data.verify.len;
			
			OpAdd = (UINT32)(iap_rec_data.verify.addr[0]);
			OpAdd |= ((UINT32)(iap_rec_data.verify.addr[1]) << 8);
			OpAdd = OpAdd * 4;
			
			
			p_flash = (UINT8 *)OpAdd;
			
			/* ��һ���RAM����Ĳ����Ա� */
			if(OpAdd < FLASH_BLOCK_SIZE)
			{
				for(i = 0; (OpAdd+i < FLASH_BLOCK_SIZE) && (i<OpParaDataLen); i++)
				{
					if(vectors_block_buf[OpAdd+i] != iap_rec_data.verify.buf[i])break;
				}
				if(i == OpParaDataLen) status = SUCCESS;
				else if((OpAdd+i == FLASH_BLOCK_SIZE))
				{
					UINT8 j;
					OpParaDataLen -= i;
					p_flash = (UINT8 *)(OpAdd+i);
					for(j=0; j<OpParaDataLen; j++)
					{
						if(p_flash[j] != iap_rec_data.verify.buf[i+j]) break;
					}
					if(j == OpParaDataLen) status = SUCCESS;
					else                   		status = 0xff;
				}
				else				status = 0xff;
			}
			/* ������Flash��Ա� */
			else
			{
				p_flash = (UINT8 *)(OpAdd);
				for(i=0; i<OpParaDataLen; i++)
				{
					if(p_flash[i] != iap_rec_data.verify.buf[i]) break;
				}
				if(i == OpParaDataLen) status = SUCCESS;
				else                   status = 0xff;
			}
			VerifyStatus |= status;
			OTA_IAP_SendCMDDealSta(VerifyStatus);
			break;
		}
		/* ��̽��� */
		case CMD_IAP_END:
		{
			PRINT("IAP_END \r\n");

			/* �رյ�ǰ����ʹ���жϣ����߷���һ��ֱ��ȫ���ر� */
			DisableAllIRQ();

			/* ���ImageA��һ�� */
			FlashBlockErase(IMAGE_APP_START_ADD);
			
			FlashWriteBuf(IMAGE_APP_START_ADD, (PUINT32) vectors_block_buf, FLASH_BLOCK_SIZE);
			
			/* ��֤��ӡ���� */
			DelayMs( 10 );
			
			/* ��λ�����û��������� */
			SYS_ResetExecute( );
			
			break;
		}
		case CMD_IAP_INFO:
		{
			UINT8 send_buf[20];

			PRINT("IAP_INFO \r\n");
			
			/* IMAGE FLAG */
			send_buf[0] = IMAGE_OTA_FLAG;
			
			/* IMAGE_SIZE */
			send_buf[1] = (UINT8)(IMAGE_APP_SIZE & 0xff);
			send_buf[2] = (UINT8)((IMAGE_APP_SIZE>>8) & 0xff);
			send_buf[3] = (UINT8)((IMAGE_APP_SIZE>>16) & 0xff);
			send_buf[4] = (UINT8)((IMAGE_APP_SIZE>>24) & 0xff);
			
			/* BLOCK SIZE */
			send_buf[5] = (UINT8)(FLASH_BLOCK_SIZE & 0xff);
			send_buf[6] = (UINT8)((FLASH_BLOCK_SIZE>>8) & 0xff);
			
			/* CHIP ID */
            send_buf[7] = CHIP_ID&0xFF;
            send_buf[8] = (CHIP_ID>>8)&0xFF;
			/* ����Ҫ������ */
			
			/* ������Ϣ */
			OTA_IAP_SendData(send_buf,20);
			
			break;
		}
		
		default:
		{
			OTA_IAP_CMDErrDeal();
			break;
		}  
    }
}

/*******************************************************************************
* Function Name  : OTA_IAPReadDataComplete
* Description    : OTA ���ݶ�ȡ��ɴ���
* Input          : none
* Output         : none
* Return         : none
*******************************************************************************/
void OTA_IAPReadDataComplete( unsigned char index )
{
	PRINT("OTA Send Comp \r\n");
}

/*******************************************************************************
* Function Name  : OTA_IAPWriteData
* Description    : OTA ͨ�����ݽ�����ɴ���
* Input          : index��OTA ͨ�����
                   p_data��д�������
                   w_len��д��ĳ���
* Output         : none
* Return         : none
*******************************************************************************/
void OTA_IAPWriteData( unsigned char index, unsigned char *p_data, unsigned char w_len )
{
	unsigned char rec_len;
	unsigned char *rec_data;
	
	rec_len = w_len;
	rec_data = p_data;
	tmos_memcpy( (unsigned char *)&iap_rec_data, rec_data, rec_len );
	Rec_OTA_IAP_DataDeal();
}

/*********************************************************************
*********************************************************************/
