/********************************** (C) COPYRIGHT *******************************
 * File Name          : hidconsumer.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/13
 * Description        : ����������Ӧ�ó��򣬳�ʼ���㲥���Ӳ�����Ȼ��㲥��ֱ�����������󣬶�ʱ�ϴ��������¼�
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
#include "battservice.h"
#include "hiddev.h"
#include "hidconsumer.h"
#include "hidconsumerservice.h"


/*********************************************************************
 * MACROS
 */

// HID consumer input report length
#define HID_CONSUMER_IN_RPT_LEN        2

/*********************************************************************
 * CONSTANTS
 */
// Param update delay
#define START_PARAM_UPDATE_EVT_DELAY          12800

// HID idle timeout in msec; set to zero to disable timeout
#define DEFAULT_HID_IDLE_TIMEOUT              60000

// Minimum connection interval (units of 1.25ms)
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     8

// Maximum connection interval (units of 1.25ms)
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     8

// Slave latency to use if parameter update request
#define DEFAULT_DESIRED_SLAVE_LATENCY         0

// Supervision timeout value (units of 10ms)
#define DEFAULT_DESIRED_CONN_TIMEOUT          500

// Default passcode
#define DEFAULT_PASSCODE                      0

// Default GAP pairing mode
#define DEFAULT_PAIRING_MODE                  GAPBOND_PAIRING_MODE_INITIATE

// Default MITM mode (TRUE to require passcode or OOB when pairing)
#define DEFAULT_MITM_MODE                     FALSE

// Default bonding mode, TRUE to bond
#define DEFAULT_BONDING_MODE                  TRUE

// Default GAP bonding I/O capabilities
#define DEFAULT_IO_CAPABILITIES               GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT

// Battery level is critical when it is less than this %
#define DEFAULT_BATT_CRITICAL_LEVEL           6

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Task ID
static uint8 hidEmuTaskId=INVALID_TASK_ID;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// GAP Profile - Name attribute for SCAN RSP data
static uint8 scanRspData[] =
{
  0x0D,                             // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,   // AD Type = Complete local name
  'H',
  'I',
  'D',
  ' ',
  'C',
  'o',
  'n',
  's',
  'u',
  'm',
  'e',
  'r',
	
  // connection interval range
  0x05,   // length of this data
  GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
  LO_UINT16( DEFAULT_DESIRED_MIN_CONN_INTERVAL ),   // 100ms
  HI_UINT16( DEFAULT_DESIRED_MIN_CONN_INTERVAL ),
  LO_UINT16( DEFAULT_DESIRED_MAX_CONN_INTERVAL ),   // 1s
  HI_UINT16( DEFAULT_DESIRED_MAX_CONN_INTERVAL ),

  // service UUIDs
  0x05,   // length of this data
  GAP_ADTYPE_16BIT_MORE,
  LO_UINT16(HID_SERV_UUID),
  HI_UINT16(HID_SERV_UUID),
  LO_UINT16(BATT_SERV_UUID),
  HI_UINT16(BATT_SERV_UUID),
	
  // Tx power level
  0x02,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,
  0       // 0dBm
};

// Advertising data
static uint8 advertData[] =
{
  // flags
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

  // appearance
  0x03,   // length of this data
  GAP_ADTYPE_APPEARANCE,
  LO_UINT16(GAP_APPEARE_GENERIC_HID),
  HI_UINT16(GAP_APPEARE_GENERIC_HID)
};

// Device name attribute value
static CONST uint8 attDeviceName[GAP_DEVICE_NAME_LEN] = "HID Consumer";

// HID Dev configuration
static hidDevCfg_t hidEmuCfg =
{
  DEFAULT_HID_IDLE_TIMEOUT,   // Idle timeout
  HID_FEATURE_FLAGS           // HID feature flags
};

static uint16 hidEmuConnHandle = GAP_CONNHANDLE_INIT;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void hidEmu_ProcessTMOSMsg( tmos_event_hdr_t *pMsg );
static void hidEmuSendConsumerReport( uint8 volume_up ,uint8 volume_dowm );
static uint8 hidEmuRptCB( uint8 id, uint8 type, uint16 uuid,
                             uint8 oper, uint16 *pLen, uint8 *pData );
static void hidEmuEvtCB( uint8 evt );
static void hidEmuStateCB( gapRole_States_t newState , gapRoleEvent_t * pEvent );

/*********************************************************************
 * PROFILE CALLBACKS
 */

static hidDevCB_t hidEmuHidCBs =
{
  hidEmuRptCB,
  hidEmuEvtCB,
  NULL,
  hidEmuStateCB
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      HidEmu_Init
 *
 * @brief   Initialization function for the HidEmuKbd App Task.
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
void HidEmu_Init( )
{
  hidEmuTaskId = TMOS_ProcessEventRegister(HidEmu_ProcessEvent);
  
  // Setup the GAP Peripheral Role Profile
  {
    uint8 initial_advertising_enable = TRUE;

       
    // Set the GAP Role Parameters
    GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &initial_advertising_enable );

    GAPRole_SetParameter( GAPROLE_ADVERT_DATA, sizeof( advertData ), advertData );
    GAPRole_SetParameter( GAPROLE_SCAN_RSP_DATA, sizeof ( scanRspData ), scanRspData );
  }

  // Set the GAP Characteristics
  GGS_SetParameter( GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, (void *) attDeviceName );

  // Setup the GAP Bond Manager
  {
    uint32 passkey = DEFAULT_PASSCODE;
    uint8 pairMode = DEFAULT_PAIRING_MODE;
    uint8 mitm = DEFAULT_MITM_MODE;
    uint8 ioCap = DEFAULT_IO_CAPABILITIES;
    uint8 bonding = DEFAULT_BONDING_MODE;
    GAPBondMgr_SetParameter( GAPBOND_PERI_DEFAULT_PASSCODE, sizeof( uint32 ), &passkey );
    GAPBondMgr_SetParameter( GAPBOND_PERI_PAIRING_MODE, sizeof( uint8 ), &pairMode );
    GAPBondMgr_SetParameter( GAPBOND_PERI_MITM_PROTECTION, sizeof( uint8 ), &mitm );
    GAPBondMgr_SetParameter( GAPBOND_PERI_IO_CAPABILITIES, sizeof( uint8 ), &ioCap );
    GAPBondMgr_SetParameter( GAPBOND_PERI_BONDING_ENABLED, sizeof( uint8 ), &bonding );
  }

  // Setup Battery Characteristic Values
  {
    uint8 critical = DEFAULT_BATT_CRITICAL_LEVEL;
    Batt_SetParameter( BATT_PARAM_CRITICAL_LEVEL, sizeof (uint8), &critical );
  }

  // Set up HID keyboard service
  Hid_AddService( );

  // Register for HID Dev callback
  HidDev_Register( &hidEmuCfg, &hidEmuHidCBs );

  // Setup a delayed profile startup
  tmos_set_event( hidEmuTaskId, START_DEVICE_EVT );
}

/*********************************************************************
 * @fn      HidEmu_ProcessEvent
 *
 * @brief   HidEmuKbd Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The TMOS assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
uint16 HidEmu_ProcessEvent( uint8 task_id, uint16 events )
{

  if ( events & SYS_EVENT_MSG )
  {
    uint8 *pMsg;

    if ( (pMsg = tmos_msg_receive( hidEmuTaskId )) != NULL )
    {
      hidEmu_ProcessTMOSMsg( (tmos_event_hdr_t *)pMsg );

      // Release the TMOS message
      tmos_msg_deallocate( pMsg );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & START_DEVICE_EVT )
  {
    return ( events ^ START_DEVICE_EVT );
  }
  
  if ( events & START_PARAM_UPDATE_EVT )
  {
    // Send connect param update request
    GAPRole_PeripheralConnParamUpdateReq( hidEmuConnHandle,
                                          DEFAULT_DESIRED_MIN_CONN_INTERVAL,
                                          DEFAULT_DESIRED_MAX_CONN_INTERVAL,
                                          DEFAULT_DESIRED_SLAVE_LATENCY,
                                          DEFAULT_DESIRED_CONN_TIMEOUT,
                                          hidEmuTaskId);
    
    return (events ^ START_PARAM_UPDATE_EVT);
  }

  if ( events & START_REPORT_EVT )
  {
		//Send volume down and release
	  hidEmuSendConsumerReport( 0 ,1 );
    hidEmuSendConsumerReport( 0 ,0 );

		tmos_start_task( hidEmuTaskId, START_REPORT_EVT, 1600 );
    return ( events ^ START_REPORT_EVT );
  }
  return 0;
}

/*********************************************************************
 * @fn      hidEmu_ProcessTMOSMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void hidEmu_ProcessTMOSMsg( tmos_event_hdr_t *pMsg )
{
  switch ( pMsg->event )
  {
		default:
      break;
  }
}

/*********************************************************************
 * @fn      hidEmuSendConsumerReport
 *
 * @brief   Build and send a HID consumer report.
 *
 * @param   volume_up - consumer volume up
 *					volume_dowm - consumer volume dowm
 *
 * @return  none
 */
static void hidEmuSendConsumerReport( uint8 volume_up ,uint8 volume_dowm )
{
  uint8 buf[HID_CONSUMER_IN_RPT_LEN];

  buf[0] = volume_up;   													// volume up
  buf[1] = volume_dowm;    												// volume dowm
	
  HidDev_Report( HID_RPT_ID_CONSUMER_IN, HID_REPORT_TYPE_INPUT,
                 HID_CONSUMER_IN_RPT_LEN, buf );
}

/*********************************************************************
 * @fn      hidEmuStateCB
 *
 * @brief   GAP state change callback.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void hidEmuStateCB( gapRole_States_t newState , gapRoleEvent_t * pEvent )
{
  switch ( newState )
  {
    case GAPROLE_STARTED:
			{
				uint8 ownAddr[6];
				GAPRole_GetParameter( GAPROLE_BD_ADDR, ownAddr );
				GAP_ConfigDeviceAddr( ADDRTYPE_STATIC, ownAddr);
				PRINT( "Initialized..\n" );
			}
      break;

    case GAPROLE_ADVERTISING:
        PRINT( "Advertising..\n" );
      break;

    case GAPROLE_CONNECTED:
      {
        gapEstLinkReqEvent_t *event = (gapEstLinkReqEvent_t *) pEvent;
        
        // get connection handle
        hidEmuConnHandle = event->connectionHandle;    
        tmos_start_task( hidEmuTaskId, START_PARAM_UPDATE_EVT, START_PARAM_UPDATE_EVT_DELAY );    
        PRINT( "Connected..\n" );
      }
      break;

    case GAPROLE_CONNECTED_ADV:
      PRINT( "Connected Advertising..\n" );
      break;      
    
    case GAPROLE_WAITING:
      if( pEvent->gap.opcode == GAP_END_DISCOVERABLE_DONE_EVENT )
      {
        PRINT( "Waiting for advertising..\n" );
      }
      else if( pEvent->gap.opcode == GAP_LINK_TERMINATED_EVENT )
      {
        PRINT( "Disconnected.. Reason:%x\n",pEvent->linkTerminate.reason );
      }
      else if( pEvent->gap.opcode == GAP_LINK_ESTABLISHED_EVENT )
      {
        PRINT( "Advertising timeout..\n" );
      }
      // Enable advertising
      {
        uint8 initial_advertising_enable = TRUE;
        // Set the GAP Role Parameters
        GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &initial_advertising_enable );
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
 * @fn      hidEmuRptCB
 *
 * @brief   HID Dev report callback.
 *
 * @param   id - HID report ID.
 * @param   type - HID report type.
 * @param   uuid - attribute uuid.
 * @param   oper - operation:  read, write, etc.
 * @param   len - Length of report.
 * @param   pData - Report data.
 *
 * @return  GATT status code.
 */
static uint8 hidEmuRptCB( uint8 id, uint8 type, uint16 uuid,
                             uint8 oper, uint16 *pLen, uint8 *pData )
{
  uint8 status = SUCCESS;

  // write
  if ( oper == HID_DEV_OPER_WRITE )
  {
    status = Hid_SetParameter( id, type, uuid, *pLen, pData );
  }
  // read
  else if ( oper == HID_DEV_OPER_READ )
  {
    status = Hid_GetParameter( id, type, uuid, pLen, pData );
  }
  // notifications enabled
  else if ( oper == HID_DEV_OPER_ENABLE )
  {
		tmos_start_task( hidEmuTaskId, START_REPORT_EVT, 500 );
  }
  return status;
}

/*********************************************************************
 * @fn      hidEmuEvtCB
 *
 * @brief   HID Dev event callback.
 *
 * @param   evt - event ID.
 *
 * @return  HID response code.
 */
static void hidEmuEvtCB( uint8 evt )
{
  // process enter/exit suspend or enter/exit boot mode
  return;
}

/*********************************************************************
*********************************************************************/
