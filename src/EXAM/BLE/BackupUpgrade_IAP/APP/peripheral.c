/********************************** (C) COPYRIGHT *******************************
 * File Name          : Peripheral.C
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
#include "Peripheral.h"
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
  's','i','m','p','l','e',' ','P','e','r','i','p','h','e','r','a','l',   
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
static uint8 attDeviceName[GAP_DEVICE_NAME_LEN] = "Simple Peripheral";

// OTA IAP VARIABLES
/* OTAͨѶ��֡ */
OTA_IAP_CMD_t iap_rec_data;

/* OTA������� */
__align(8) UINT32 OpParaData[4];  
UINT32 OpParaDataLen = 0;
UINT32 OpAdd = 0;

/* flash��һ���������ʱ�洢 */
__align(8) uint8 vectors_block_buf[FLASH_BLOCK_SIZE];

/* dataflash��ʱ���� */
__align(8) uint8 dataflash_block_buf[FLASH_BLOCK_SIZE];

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
static void performPeriodicTask( void );
static void simpleProfileChangeCB( uint8 paramID );
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

// GAP Bond Manager Callbacks
static gapBondCBs_t Peripheral_BondMgrCBs =
{
    NULL,                     // Passcode callback (not used by application)
    NULL                      // Pairing / Bonding state Callback (not used by application)
};

// Simple GATT Profile Callbacks
static simpleProfileCBs_t Peripheral_SimpleProfileCBs =
{
    simpleProfileChangeCB    // Charactersitic value change callback
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
	
	//PeripheralParamUpdate_t = (gapRolesParamUpdateCB_t *)PeripheralParamUpdate;
	
	//PRINT( "PeripheralParamUpdate_t %08x \n",(int)PeripheralParamUpdate_t );
	
	//GAPRole_PeripheralRegisterAppCBs( (gapRolesParamUpdateCB_t *)&PeripheralParamUpdate ); 
  }

  // Set the GAP Characteristics
  GGS_SetParameter( GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName );

  // Set advertising interval
  {
    uint16 advInt = DEFAULT_ADVERTISING_INTERVAL;

    GAP_SetParamValue( TGAP_DISC_ADV_INT_MIN, advInt );
    GAP_SetParamValue( TGAP_DISC_ADV_INT_MAX, advInt );
  }

  // Setup the GAP Bond Manager
  {
    uint32 passkey = 0; // passkey "000000"
    uint8 pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
    uint8 mitm = TRUE;
    uint8 ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
    uint8 bonding = TRUE;
    GAPBondMgr_SetParameter( GAPBOND_PERI_DEFAULT_PASSCODE, sizeof ( uint32 ), &passkey );
    GAPBondMgr_SetParameter( GAPBOND_PERI_PAIRING_MODE, sizeof ( uint8 ), &pairMode );
    GAPBondMgr_SetParameter( GAPBOND_PERI_MITM_PROTECTION, sizeof ( uint8 ), &mitm );
    GAPBondMgr_SetParameter( GAPBOND_PERI_IO_CAPABILITIES, sizeof ( uint8 ), &ioCap );
    GAPBondMgr_SetParameter( GAPBOND_PERI_BONDING_ENABLED, sizeof ( uint8 ), &bonding );
  }

  // Initialize GATT attributes
  GGS_AddService( GATT_ALL_SERVICES );            // GAP
  GATTServApp_AddService( GATT_ALL_SERVICES );    // GATT attributes
  DevInfo_AddService();                           // Device Information Service
  SimpleProfile_AddService( GATT_ALL_SERVICES );  // Simple GATT Profile
  OTAProfile_AddService( GATT_ALL_SERVICES );
  
  // Setup the SimpleProfile Characteristic Values
  {
    uint8 charValue1 = 1;
    uint8 charValue2 = 2;
    uint8 charValue3 = 3;
    uint8 charValue4[SIMPLEPROFILE_CHAR4_LEN] = "01234567";
    uint8 charValue5[SIMPLEPROFILE_CHAR5_LEN] = { 1, 2, 3, 4, 5 };
		
    SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR1, sizeof ( uint8 ), &charValue1 );
    SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR2, sizeof ( uint8 ), &charValue2 );
    SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR3, sizeof ( uint8 ), &charValue3 );

    SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR4, SIMPLEPROFILE_CHAR4_LEN, charValue4 );
    SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR5, SIMPLEPROFILE_CHAR5_LEN, charValue5 );
  }

  // 
  {
	  //GAPRole_PeripheralRegisterAppCBs( (gapRolesParamUpdateCB_t *)PeripheralParamUpdate_t ); 
	  
  }
  
  // Register callback with SimpleGATTprofile
  SimpleProfile_RegisterAppCBs( &Peripheral_SimpleProfileCBs );

  //  Register callback with OTAGATTprofile
  OTAProfile_RegisterAppCBs( &Peripheral_OTA_IAPProfileCBs );
  
	// Setup a delayed profile startup
  tmos_set_event( Peripheral_TaskID, SBP_START_DEVICE_EVT );

}

void PeripheralParamUpdate(uint16 connInterval,uint16 connSlaveLatency, uint16 connTimeout)
{
	PRINT( "update %d %d %d \n",connInterval,connSlaveLatency,connTimeout );
	
//		GAPRole_SendUpdateParam( DEFAULT_DESIRED_MIN_CONN_INTERVAL, DEFAULT_DESIRED_MAX_CONN_INTERVAL,
//                                 DEFAULT_DESIRED_SLAVE_LATENCY, DEFAULT_DESIRED_CONN_TIMEOUT, GAPROLE_NO_ACTION );	
	
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
    GAPRole_PeripheralStartDevice( Peripheral_TaskID, &Peripheral_BondMgrCBs, &Peripheral_PeripheralCBs );
    // Set timer for first periodic event
    tmos_start_task( Peripheral_TaskID, SBP_PERIODIC_EVT, SBP_PERIODIC_EVT_PERIOD );
    return ( events ^ SBP_START_DEVICE_EVT );
  }

  if ( events & SBP_PERIODIC_EVT ){
   // Restart timer
    if ( SBP_PERIODIC_EVT_PERIOD ){
      tmos_start_task( Peripheral_TaskID, SBP_PERIODIC_EVT, SBP_PERIODIC_EVT_PERIOD );
    }
    // Perform periodic application task
    performPeriodicTask();
    return (events ^ SBP_PERIODIC_EVT);
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
			PRINT( "Initialized..\n" );
      break;

    case GAPROLE_ADVERTISING:
      PRINT( "Advertising..\n" );
      break;

    case GAPROLE_CONNECTED:
			{
				gapEstLinkReqEvent_t *event = (gapEstLinkReqEvent_t *) pEvent;
				uint16 conn_interval = 0;

				conn_interval = event->connInterval;
				PRINT( "Connected.. \n" );
				
				if( conn_interval > DEFAULT_DESIRED_MAX_CONN_INTERVAL)
				{
					PRINT("Send Update\r\n");
					GAPRole_PeripheralConnParamUpdateReq( event->connectionHandle, 
																								DEFAULT_DESIRED_MIN_CONN_INTERVAL, 
																								DEFAULT_DESIRED_MAX_CONN_INTERVAL,
																								DEFAULT_DESIRED_SLAVE_LATENCY, 
																								DEFAULT_DESIRED_CONN_TIMEOUT, 
																								Peripheral_TaskID );
					
				}
				break;
			}
    case GAPROLE_CONNECTED_ADV:
      PRINT( "Connected Advertising..\n" );
      break;      
    case GAPROLE_WAITING:
			{
				uint8 initial_advertising_enable = TRUE;

				// Set the GAP Role Parameters
				GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &initial_advertising_enable );
				PRINT( "Disconnected..\n" );
			}
      break;

    case GAPROLE_ERROR:
			PRINT( "Error..\n" );
      break;

    default:
      break;

  }
}

/*********************************************************************
 * @fn      performPeriodicTask
 *
 * @brief   Perform a periodic application task. This function gets
 *          called every five seconds as a result of the SBP_PERIODIC_EVT
 *          TMOS event. In this example, the value of the third
 *          characteristic in the SimpleGATTProfile service is retrieved
 *          from the profile, and then copied into the value of the
 *          the fourth characteristic.
 *
 * @param   none
 *
 * @return  none
 */
static void performPeriodicTask( void )
{
  uint8 valueToCopy[SIMPLEPROFILE_CHAR4_LEN];
  uint8 stat;

  // Call to retrieve the value of the third characteristic in the profile
  stat = SimpleProfile_GetParameter( SIMPLEPROFILE_CHAR4, valueToCopy);
  if( stat == SUCCESS ){
    SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR4, SIMPLEPROFILE_CHAR4_LEN, valueToCopy);
  }
}

/*********************************************************************
 * @fn      simpleProfileChangeCB
 *
 * @brief   Callback from Profile indicating a value change
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  none
 */
static void simpleProfileChangeCB( uint8 paramID )
{
  uint8 newValue;

  switch( paramID )
  {
    case SIMPLEPROFILE_CHAR1:
			PRINT("profile ChangeCB CHAR1..\n");
      SimpleProfile_GetParameter( SIMPLEPROFILE_CHAR1, &newValue );
      break;

    case SIMPLEPROFILE_CHAR3:
			PRINT("profile ChangeCB CHAR3..\n");
      SimpleProfile_GetParameter( SIMPLEPROFILE_CHAR3, &newValue );
      break;

    default:
      // should not reach here!
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
* Function Name  : SwitchImageFlag
* Description    : �л�dataflash���ImageFlag
* Input          : new_flag���л���ImageFlag
* Output         : none
* Return         : none
*******************************************************************************/
void SwitchImageFlag(UINT8 new_flag)
{
	UINT8  *p_flash;
	UINT16 i;
	UINT8  ver_flag;
	
	/* ��ȡ��һ�� */
	p_flash = (UINT8 *)OTA_DATAFLASH_ADD;
	for(i=0; i<FLASH_BLOCK_SIZE; i++)
	{
		dataflash_block_buf[i] = p_flash[i];
	}
	
	/* ������һ�� */
	FlashBlockErase(OTA_DATAFLASH_ADD);

	/* ����Image��Ϣ */
	dataflash_block_buf[0] = new_flag;
	
	/* ���DataFlash */
	FlashWriteBuf(OTA_DATAFLASH_ADD, (UINT32 *)dataflash_block_buf, FLASH_BLOCK_SIZE);
	
	/* ��ӡ�����Ϣ */
	p_flash = (UINT8 *)OTA_DATAFLASH_ADD;
	ver_flag = p_flash[0];
	PRINT("SwitchImageFlag: %02x \r\n",ver_flag);
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
			
			status = FlashWriteBuf(OpAdd, OpParaData, (UINT16) OpParaDataLen);				
			
			if( status ) 	OTA_IAP_SendCMDDealSta(status);
			break;
		}
		/* ���� -- ������������������ */
		case CMD_IAP_ERASE:
		{
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
			
			PRINT("IAP_VERIFY: %08x len:%d \r\n",(int)OpAdd,(int)OpParaDataLen);
			
			p_flash = (UINT8 *)OpAdd;
			
			/* ��ǰ��ImageA��ֱ�Ӷ�ȡImageBУ�� */
			for(i=0; i<OpParaDataLen; i++)
			{
				if(p_flash[i] != iap_rec_data.verify.buf[i]) break;
			}
			if(i == OpParaDataLen) status = SUCCESS;
			else                   status = 0xff;
			VerifyStatus |= status;
			OTA_IAP_SendCMDDealSta(VerifyStatus);
			break;
		}
		/* ��̽��� */
		case CMD_IAP_END:
		{
			PRINT("IAP_END \r\n");
			
			/* ��ǰ����ImageA */
			/* �޸�DataFlash���л���ImageIAP */
			SwitchImageFlag(IMAGE_IAP_FLAG);

			/* �رյ�ǰ����ʹ���жϣ����߷���һ��ֱ��ȫ���ر� */
			DisableAllIRQ();

			/* ��֤��ӡ���� */
			DelayMs( 10 );
			
			/* ����ImageIAP���� */
			GotoResetVector(IMAGE_IAP_ENTRY_ADD);
			break;
		}
		case CMD_IAP_INFO:
		{
			UINT8 send_buf[20];

			PRINT("IAP_INFO \r\n");
			
			/* IMAGE FLAG */
			send_buf[0] = IMAGE_B_FLAG;
			
			/* IMAGE_SIZE */
			send_buf[1] = (UINT8)(IMAGE_SIZE & 0xff);
			send_buf[2] = (UINT8)((IMAGE_SIZE>>8) & 0xff);
			send_buf[3] = (UINT8)((IMAGE_SIZE>>16) & 0xff);
			send_buf[4] = (UINT8)((IMAGE_SIZE>>24) & 0xff);
			
			/* BLOCK SIZE */
			send_buf[5] = (UINT8)(FLASH_BLOCK_SIZE & 0xff);
			send_buf[6] = (UINT8)((FLASH_BLOCK_SIZE>>8) & 0xff);
			
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
