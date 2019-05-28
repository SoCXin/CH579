/********************************** (C) COPYRIGHT *******************************
* File Name          : observer.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/12/10
* Description        : 观察应用程序，初始化扫描参数，然后定时扫描，如果扫描结果不为空，则打印扫描到的广播地址
            
*******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "CONFIG.h"
#include "CH57x_common.h"
#include "CH57xBLE_LIB.h"
#include "observer.h"


/*********************************************************************
 * MACROS
 */

// Length of bd addr as a string
#define B_ADDR_STR_LEN                        15

/*********************************************************************
 * CONSTANTS
 */

// Maximum number of scan responses
#define DEFAULT_MAX_SCAN_RES                  8

// Scan duration in (625us)
#define DEFAULT_SCAN_DURATION                 4000

// Discovey mode (limited, general, all)
#define DEFAULT_DISCOVERY_MODE                DEVDISC_MODE_ALL

// TRUE to use active scan
#define DEFAULT_DISCOVERY_ACTIVE_SCAN         TRUE

// TRUE to use white list during discovery
#define DEFAULT_DISCOVERY_WHITE_LIST          FALSE

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
uint8 gStatus;

// Task ID for internal task/event processing
static uint8 ObserverTaskId;

// GAP GATT Attributes
//static const uint8 ObserverDeviceName[GAP_DEVICE_NAME_LEN] = "Simple BLE Observer";

// Number of scan results and scan result index
static uint8 ObserverScanRes;

// Scan result list
static gapDevRec_t ObserverDevList[DEFAULT_MAX_SCAN_RES];

// Scanning state
static uint8 ObserverScanning = FALSE;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void ObserverEventCB( gapObserverRoleEvent_t *pEvent );
static void Observer_ProcessOSALMsg( tmos_event_hdr_t *pMsg );
static void ObserverAddDeviceInfo( uint8 *pAddr, uint8 addrType );
char *bdAddr2Str ( uint8 *pAddr );

/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static const gapObserverRoleCB_t ObserverRoleCB =
{
  NULL,                     // RSSI callback
  ObserverEventCB  // Event callback
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Observer_Init
 *
 * @brief   Initialization function for the Simple BLE Observer App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notification).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void Observer_Init( uint8 task_id )
{
  ObserverTaskId = task_id;

  // Setup Observer Profile
  {
    uint8 scanRes = DEFAULT_MAX_SCAN_RES;
    GAPRole_SetParameter ( GAPROLE_MAX_SCAN_RES, sizeof( uint8 ), &scanRes );
  }
  
  // Setup GAP
  GAP_SetParamValue( TGAP_GEN_DISC_SCAN, DEFAULT_SCAN_DURATION );
  GAP_SetParamValue( TGAP_LIM_DISC_SCAN, DEFAULT_SCAN_DURATION );

  // Setup a delayed profile startup
  tmos_set_event( ObserverTaskId, START_DEVICE_EVT );
}

/*********************************************************************
 * @fn      Observer_ProcessEvent
 *
 * @brief   Simple BLE Observer Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
uint16 Observer_ProcessEvent( uint8 task_id, uint16 events )
{
  
//  VOID task_id; // OSAL required parameter that isn't used in this function
  
  if ( events & SYS_EVENT_MSG )
  {
    uint8 *pMsg;

    if ( (pMsg = tmos_msg_receive( ObserverTaskId )) != NULL )
    {
      Observer_ProcessOSALMsg( (tmos_event_hdr_t *)pMsg );

      // Release the OSAL message
      tmos_msg_deallocate( pMsg );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & START_DEVICE_EVT )
  {
		uint8 stat;
    // Start the Device
    stat = GAPRole_ObserverStartDevice( (gapObserverRoleCB_t *) &ObserverRoleCB );
		PRINT ("start device %d \n",stat);
    return ( events ^ START_DEVICE_EVT );
  }
	if( events & START_SCAN_EVT ){
    PRINT ( "scan event:%d %d...\n",ObserverScanning,ObserverScanRes);      
    // Display discovery results
    if ( ObserverScanning && ObserverScanRes > 0 )
    {
			int i,j;
			// Increment index of current result (with wraparound)
			for( j=0;j<ObserverScanRes;j++ ){
				PRINT ( "Device %d : ", j  );
				for( i=0; i<6; i++)
				{
					PRINT ( "%x ",ObserverDevList[j].addr[i] );
				}
				PRINT ("\n");
			}
		}
    if ( !ObserverScanning )
    {
      ObserverScanning = TRUE;
      ObserverScanRes = 0;
      
      PRINT ( "Discovering...\n");      
      GAPRole_ObserverStartDiscovery( DEFAULT_DISCOVERY_MODE,
                                      DEFAULT_DISCOVERY_ACTIVE_SCAN,
                                      DEFAULT_DISCOVERY_WHITE_LIST );      
    }
    else
    {
      PRINT ( "cancel discovering...\n");      
      GAPRole_ObserverCancelDiscovery();
    }
		tmos_start_task( ObserverTaskId, START_SCAN_EVT, 5000 ); // 5000*625us
		return ( events ^ START_SCAN_EVT );
	}
  // Discard unknown events
  return 0;
}

/*********************************************************************
 * @fn      Observer_ProcessOSALMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void Observer_ProcessOSALMsg( tmos_event_hdr_t *pMsg )
{
  switch ( pMsg->event )
  {
    case GATT_MSG_EVENT:
      break;
		
		default:
			break;
  }
}

/*********************************************************************
 * @fn      ObserverEventCB
 *
 * @brief   Observer event callback function.
 *
 * @param   pEvent - pointer to event structure
 *
 * @return  none
 */
static void ObserverEventCB( gapObserverRoleEvent_t *pEvent )
{
  switch ( pEvent->gap.opcode )
  {
    case GAP_DEVICE_INIT_DONE_EVENT:  
      {
				PRINT ("GAP_DEVICE_INIT_DONE_EVENT\n");
				tmos_set_event( ObserverTaskId, START_SCAN_EVT);
      }
      break;

    case GAP_DEVICE_INFO_EVENT:
      {
				PRINT ("GAP_DEVICE_INFO_EVENT\n");
        ObserverAddDeviceInfo( pEvent->deviceInfo.addr, pEvent->deviceInfo.addrType );
      }
      break;
      
    case GAP_DEVICE_DISCOVERY_EVENT:
      {
        // discovery complete
        ObserverScanning = FALSE;

				PRINT ("GAP_DEVICE_DISCOVERY_EVENT\n");
        // Copy results
        ObserverScanRes = pEvent->discCmpl.numDevs;
        tmos_memcpy( ObserverDevList, pEvent->discCmpl.pDevList,
                     (sizeof( gapDevRec_t ) * pEvent->discCmpl.numDevs) );
      }
      break;
      
    default:
      break;
  }
}

/*********************************************************************
 * @fn      ObserverAddDeviceInfo
 *
 * @brief   Add a device to the device discovery result list
 *
 * @return  none
 */
static void ObserverAddDeviceInfo( uint8 *pAddr, uint8 addrType )
{
  uint8 i;
  
  // If result count not at max
  if ( ObserverScanRes < DEFAULT_MAX_SCAN_RES )
  {
    // Check if device is already in scan results
    for ( i = 0; i < ObserverScanRes; i++ )
    {
      if ( tmos_memcmp( pAddr, ObserverDevList[i].addr , B_ADDR_LEN ) )
      {
        return;
      }
    }
    PRINT ("Add addr to scan result list\n");
    
    // Add addr to scan result list
    tmos_memcpy( ObserverDevList[ObserverScanRes].addr, pAddr, B_ADDR_LEN );
    ObserverDevList[ObserverScanRes].addrType = addrType;
    
    // Increment scan result count
    ObserverScanRes++;
  }
}


/*********************************************************************
*********************************************************************/
