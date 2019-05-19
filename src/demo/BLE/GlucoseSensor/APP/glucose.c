/********************************** (C) COPYRIGHT *******************************
* File Name          : glucose.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/12/12
* Description        : 葡萄糖传感器应用程序，初始化广播连接参数，然后广播，直至连接主机后，定时上传葡萄糖浓度
            
*******************************************************************************/


/*********************************************************************
 * INCLUDES
 */

#include "CONFIG.h"
#include "CH57x_common.h"
#include "CH57xBLE_LIB.h"
#include "battservice.h"
#include "devinfoservice.h"
#include "glucservice.h"
#include "glucose.h"
#include "glucose_clock.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// Fast advertising interval in 625us units
#define DEFAULT_FAST_ADV_INTERVAL             32

// Duration of fast advertising duration in sec
#define DEFAULT_FAST_ADV_DURATION             30

// Slow advertising interval in 625us units
#define DEFAULT_SLOW_ADV_INTERVAL             1600

// Duration of slow advertising duration in sec
#define DEFAULT_SLOW_ADV_DURATION             30

// Whether to enable automatic parameter update request when a connection is formed
#define DEFAULT_ENABLE_UPDATE_REQUEST         FALSE

// Minimum connection interval (units of 1.25ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     200

// Maximum connection interval (units of 1.25ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     1600

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         1

// Supervision timeout value (units of 10ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT          1000

// Whether to enable automatic parameter update request when a connection is formed
#define DEFAULT_ENABLE_UPDATE_REQUEST         FALSE

// Default passcode
#define DEFAULT_PASSCODE                      125800

// Default GAP pairing mode
#define DEFAULT_PAIRING_MODE                  GAPBOND_PAIRING_MODE_WAIT_FOR_REQ

// Default MITM mode (TRUE to require passcode or OOB when pairing)
#define DEFAULT_MITM_MODE                     TRUE //FALSE

// Default bonding mode, TRUE to bond
#define DEFAULT_BONDING_MODE                  TRUE

// Default GAP bonding I/O capabilities
#define DEFAULT_IO_CAPABILITIES               GAPBOND_IO_CAP_DISPLAY_ONLY //GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT

// Notification period in (625us)
#define DEFAULT_NOTI_PERIOD                   100

// Meas state bit field
#define GLUCOSE_MEAS_STATE_VALID              0x01
#define GLUCOSE_MEAS_STATE_FILTER_PASS        0x02
#define GLUCOSE_MEAS_STATE_ALL                (GLUCOSE_MEAS_STATE_VALID | GLUCOSE_MEAS_STATE_FILTER_PASS)

// Some values used to simulate measurements
#define MEAS_IDX_MAX                          sizeof(glucoseMeasArray)/sizeof(glucoseMeas_t)

// Maximum number of dynamically allocated measurements (must be less than MEAS_IDX_MAX)
#define DYNAMIC_REC_MAX                       3

#define GLUCOSE_MEAS_LEN                      17
#define GLUCOSE_CONTEXT_LEN                   17
#define GLUCOSE_CTL_PNT_LEN                   4
/*********************************************************************
 * TYPEDEFS
 */

// contains the data of control point command
typedef struct {
  tmos_event_hdr_t hdr; //!< MSG_EVENT and status
  uint8 len;
  uint8 data[GLUCOSE_CTL_PNT_MAX_SIZE];
} glucoseCtlPntMsg_t;


// Data in a glucose measurement as defined in the profile
typedef struct {
  uint8 state;
  uint8 flags;
  uint16 seqNum;
  UTCTimeStruct baseTime;
  int16 timeOffset;
  uint16 concentration;
  uint8 typeSampleLocation;
  uint16 sensorStatus;
} glucoseMeas_t;

// Context data as defined in profile
typedef struct {
  uint8 flags;
  uint16 seqNum;
  uint8 extendedFlags;
  uint8 carboId;
  uint16 carboVal;
  uint8 mealVal;
  uint8 TesterHealthVal;
  uint16 exerciseDuration;
  uint8 exerciseIntensity;
  uint8 medId;
  uint16 medVal;
  uint16 HbA1cVal;
} glucoseContext_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Task ID
uint8 glucoseTaskId;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// GAP State
static gaprole_States_t gapProfileState = GAPROLE_INIT;

// GAP Profile - Name attribute for SCAN RSP data
static uint8 scanRspData[] =
{
  0x08,   // length of this data
  0x09,   // AD Type = Complete local name
  'G',
  'l',
  'u',
  'c',
  'o',
  's',
  'e'
};

static uint8 advertData[] =
{
  // flags
  0x02,
  GAP_ADTYPE_FLAGS,
  GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
  // service UUIDs
  0x05,
  GAP_ADTYPE_16BIT_MORE,
  LO_UINT16(GLUCOSE_SERV_UUID),
  HI_UINT16(GLUCOSE_SERV_UUID),
  LO_UINT16(DEVINFO_SERV_UUID),
  HI_UINT16(DEVINFO_SERV_UUID)
};

// Device name attribute value
static uint8 attDeviceName[GAP_DEVICE_NAME_LEN] = "Glucose Sensor";

// Bonded state
static BOOL glucoseBonded = FALSE;

// Bonded peer address
static uint8 glucoseBondedAddr[B_ADDR_LEN];

// GAP connection handle
static uint16 gapConnHandle;

// Indication structures for glucose
static attHandleValueNoti_t  glucoseMeas;
static attHandleValueNoti_t glucoseContext;
static attHandleValueInd_t glucoseCtlPntRsp;

// Set to TRUE if context should be sent with measurement data
static BOOL glucoseSendContext = FALSE;

// Advertising user-cancelled state
static BOOL glucoseAdvCancelled = FALSE;

//If TRUE, then send all valid and selected glucose measurements
BOOL glucoseSendAllRecords = FALSE;

// For the example application we have hard coded glucose measurements
// Note dates are in UTC time; day and month start at 0
static glucoseMeas_t glucoseMeasArray[] =
{
  //Meas 1
  {
  GLUCOSE_MEAS_STATE_ALL,
  GLUCOSE_MEAS_FLAG_ALL,   //mol/L
  1,
  {14,17,23,22,1,2013},
  0,
  0xC050, // 0.008 (8.0 mmol)
  (GLUCOSE_LOCATION_FINGER | GLUCOSE_TYPE_CAPILLARY_WHOLE),
  0
  },
  //Meas 2
  {
  GLUCOSE_MEAS_STATE_ALL,
  GLUCOSE_MEAS_FLAG_ALL,   //mol/L
  2,
  {6,0,0,2,1,2012},
  0,                      // Time offset
  0xC03C,                 // 0.006
  (GLUCOSE_LOCATION_AST | GLUCOSE_TYPE_CAPILLARY_PLASMA),
  0xFFFF,                 // Status
  },
  //Meas 3
  {
  GLUCOSE_MEAS_STATE_ALL,
  GLUCOSE_MEAS_FLAG_ALL,                              //mol/L
  3,
  {11,23,0,5,2,2012},
  0,
  0xC037,                // 5.5 mmol/L
  (GLUCOSE_LOCATION_EARLOBE | GLUCOSE_TYPE_VENOUS_WHOLE),
  0x5555
  },
  // Time offset +1 hr
  //Meas 4
  {
  GLUCOSE_MEAS_STATE_ALL,
  GLUCOSE_MEAS_FLAG_ALL,                              //mol/L
  4,
  {12,2,0,14,1,2011},
  60,
  0xB165,  // 3.57 mmol /L
  (GLUCOSE_LOCATION_CONTROL | GLUCOSE_TYPE_VENOUS_PLASMA),
  0xAAAA
  },
  //Meas 5
  {
  GLUCOSE_MEAS_STATE_ALL,
  GLUCOSE_MEAS_FLAG_ALL,                              //mol/L
  5,
  {13,5,12,12,1,2011},
  60,
  0xB1F6, // 5.02 mmol /L
  (GLUCOSE_LOCATION_NOT_AVAIL | GLUCOSE_TYPE_ARTERIAL_WHOLE),
  0xA5A5
  },
  //Meas 6
  {
  GLUCOSE_MEAS_STATE_ALL,
  GLUCOSE_MEAS_FLAG_ALL & ~GLUCOSE_MEAS_FLAG_UNITS,   //kg/L
  6,
  {7,15,0,5,2,2011},
  60,
  0xB07E, // 126 mg/dl
  (GLUCOSE_LOCATION_FINGER | GLUCOSE_TYPE_ARTERIAL_PLASMA),
  0x5A5A
  },
  // Time offset -2 hrs
  //Meas 7
  {
  GLUCOSE_MEAS_STATE_ALL,
  GLUCOSE_MEAS_FLAG_ALL & ~GLUCOSE_MEAS_FLAG_UNITS,   //kg/L
  7,
  {9,4,0,14,4,2011},
  -120,
  0xB064,    // 100 mg/dl
  (GLUCOSE_LOCATION_FINGER | GLUCOSE_TYPE_UNDETER_WHOLE),
  0x55AA
  },
  //Meas 8
  {
  GLUCOSE_MEAS_STATE_ALL,
  GLUCOSE_MEAS_FLAG_ALL & ~GLUCOSE_MEAS_FLAG_UNITS,    //kg/L
  8,
  {0,0,0,1,1,2013},
  -120,
  0xB05A,  // 90 mg/dl
  (GLUCOSE_LOCATION_AST | GLUCOSE_TYPE_UNDETER_PLASMA),
  0xAA55
  },
  //Meas 9
  {
  GLUCOSE_MEAS_STATE_ALL,
  GLUCOSE_MEAS_FLAG_ALL & ~GLUCOSE_MEAS_FLAG_UNITS,    //kg/L
  9,
  {12,0,0,5,2,2013},
  -120,
  0xB048, // 72 mg/dl
  (GLUCOSE_LOCATION_EARLOBE | GLUCOSE_TYPE_ISF),
  0x1111
  },
};

// Each measurement entry must have a corresponding context, it is only sent
// based on the flag in the measurement, but it must exist for this app.
static glucoseContext_t glucoseContextArray[] =
{
  //Context 1
  {
   GLUCOSE_CONTEXT_FLAG_ALL & ~GLUCOSE_CONTEXT_FLAG_MEDICATION_UNITS, //kg
   1,
   0,
   GLUCOSE_CARBO_BREAKFAST,
   9,
   GLUCOSE_MEAL_PREPRANDIAL,
   GLUCOSE_HEALTH_NONE | GLUCOSE_TESTER_SELF,
   1800,
   70,
   GLUCOSE_MEDICATION_RAPID,
   100,
   50
  },
  //Context 2
  {
   GLUCOSE_CONTEXT_FLAG_ALL & ~GLUCOSE_CONTEXT_FLAG_MEDICATION_UNITS, //kg
   2,
   0,
   GLUCOSE_CARBO_BREAKFAST,
   10,
   GLUCOSE_MEAL_PREPRANDIAL,
   GLUCOSE_HEALTH_NONE | GLUCOSE_TESTER_SELF,
   1800,
   70,
   GLUCOSE_MEDICATION_RAPID,
   100,
   49
  },
  //Context 3
  {
   GLUCOSE_CONTEXT_FLAG_ALL, //L
   3,
   0,
   GLUCOSE_CARBO_BREAKFAST,
   10,
   GLUCOSE_MEAL_PREPRANDIAL,
   GLUCOSE_HEALTH_NONE | GLUCOSE_TESTER_SELF,
   1800,
   70,
   GLUCOSE_MEDICATION_RAPID,
   100,
   49
  },
  //Context 4
  {
   GLUCOSE_CONTEXT_FLAG_ALL & ~GLUCOSE_CONTEXT_FLAG_MEDICATION_UNITS, //kg
   4,
   0,
   GLUCOSE_CARBO_BREAKFAST,
   10,
   GLUCOSE_MEAL_PREPRANDIAL,
   GLUCOSE_HEALTH_NONE | GLUCOSE_TESTER_SELF,
   1800,
   70,
   GLUCOSE_MEDICATION_RAPID,
   100,
   49
  },
  //Context 5
  {
   GLUCOSE_CONTEXT_FLAG_ALL & ~GLUCOSE_CONTEXT_FLAG_MEDICATION_UNITS, //kg
   5,
   0,
   GLUCOSE_CARBO_BREAKFAST,
   10,
   GLUCOSE_MEAL_PREPRANDIAL,
   GLUCOSE_HEALTH_NONE | GLUCOSE_TESTER_SELF,
   1800,
   70,
   GLUCOSE_MEDICATION_RAPID,
   100,
   49
  },
  //Context 6
  {
   GLUCOSE_CONTEXT_FLAG_ALL, //L
   6,
   0,
   GLUCOSE_CARBO_BREAKFAST,
   10,
   GLUCOSE_MEAL_PREPRANDIAL,
   GLUCOSE_HEALTH_NONE | GLUCOSE_TESTER_SELF,
   1800,
   70,
   GLUCOSE_MEDICATION_RAPID,
   100,
   49
  },
  //Context 7
  {
   GLUCOSE_CONTEXT_FLAG_ALL & ~GLUCOSE_CONTEXT_FLAG_MEDICATION_UNITS, //kg
   7,
   0,
   GLUCOSE_CARBO_BREAKFAST,
   10,
   GLUCOSE_MEAL_PREPRANDIAL,
   GLUCOSE_HEALTH_NONE | GLUCOSE_TESTER_SELF,
   1800,
   70,
   GLUCOSE_MEDICATION_RAPID,
   100,
   49
  },
  //Context 8
  {
   GLUCOSE_CONTEXT_FLAG_ALL & ~GLUCOSE_CONTEXT_FLAG_MEDICATION_UNITS, //kg
   8,
   0,
   GLUCOSE_CARBO_BREAKFAST,
   11,
   GLUCOSE_MEAL_PREPRANDIAL,
   GLUCOSE_HEALTH_NONE | GLUCOSE_TESTER_SELF,
   1800,
   70,
   GLUCOSE_MEDICATION_RAPID,
   100,
   48
  },
  //Context 9
  {
   GLUCOSE_CONTEXT_FLAG_ALL, //L
   9,
   0,
   GLUCOSE_CARBO_BREAKFAST,
   12,
   GLUCOSE_MEAL_PREPRANDIAL,
   GLUCOSE_HEALTH_NONE | GLUCOSE_TESTER_SELF,
   1800,
   70,
   GLUCOSE_MEDICATION_RAPID,
   100,
   47
  }
};

// initial index of measurement/context
static uint8 glucoseMeasIdx = 0;


/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void glucoseSendNext( void );
static void glucose_ProcessTMOSMsg( tmos_event_hdr_t *pMsg );
static void glucoseProcessCtlPntMsg( glucoseCtlPntMsg_t* pMsg);
static void glucoseGapStateCB( gaprole_States_t newState );
static void glucoseMeasSend(void);
static void glucoseContextSend(void);
static void glucoseCtlPntResponse(uint8 rspCode, uint8 opcode);
static void glucoseCtlPntNumRecordsResponse(uint16 numRecords);
static void glucoseCB(uint8 event, uint8* valueP, uint8 len);
static uint8 glucoseMarkAllRecords(BOOL setBits, uint8 mask);
static uint8 glucoseMarkFirstValidRecord(BOOL setBits, uint8 mask);
static uint8 glucoseMarkLastValidRecord(BOOL setBits, uint8 mask);
static uint8 glucoseVerifyTime(UTCTimeStruct* pTime);
static uint8 glucoseMarkAllRecordsEarlierThanSeqNum(uint16* pSeqNum, BOOL set, uint8 mask);
static uint8 glucoseMarkAllRecordsLaterThanSeqNum(uint16* pSeqNum, BOOL set, uint8 mask);
static uint8 glucoseMarkAllRecordsInRangeSeqNum(uint16* pSeqNum1, uint16* pSeqNum2, BOOL set, uint8 mask);
static uint8 glucoseMarkAllRecordsEarlierThanTime(UTCTimeStruct* time, BOOL set, uint8 mask);
static uint8 glucoseMarkAllRecordsLaterThanTime(UTCTimeStruct* time, BOOL set, uint8 mask);
static uint8 glucoseMarkAllRecordsInRangeTime(UTCTimeStruct* time1, UTCTimeStruct* time2, BOOL set, uint8 mask);
static uint8 glucoseFilterRecords(uint8 oper, uint8 filterType, void* param1, void* param2, BOOL set, uint8 mask, uint8 opcode);
static uint8 glucoseFindFirstValidFilteredIndex(uint8 startIndex);
static void glucoseCtlPntHandleOpcode(uint8 opcode, uint8 oper, uint8 filterType, void* param1, void* param2);
static void glucosePasscodeCB( uint8 *deviceAddr, uint16 connectionHandle,
                                        uint8 uiInputs, uint8 uiOutputs );
static void glucosePairStateCB( uint16 connHandle, uint8 state, uint8 status );

/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapRolesCBs_t glucose_PeripheralCBs =
{
  glucoseGapStateCB,  // Profile State Change Callbacks
  NULL                // When a valid RSSI is read from controller
};

// Bond Manager Callbacks
static const gapBondCBs_t glucoseBondCB =
{
  glucosePasscodeCB,
  glucosePairStateCB
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Glucose_Init
 *
 * @brief   Initialization function for the Glucose App Task.
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
void Glucose_Init( uint8 task_id )
{
  glucoseTaskId = task_id;

  // Setup the GAP Peripheral Role Profile
  {
      // For other hardware platforms, device starts advertising upon initialization
      uint8 initial_advertising_enable = TRUE;

    // Setting this to zero, the device will go into the waiting state after
    // being discoverable for 30.72 second, and will not being advertising again
    // until the enabler is set back to TRUE
    uint16 gapRole_AdvertOffTime = 100;

    uint8 enable_update_request = DEFAULT_ENABLE_UPDATE_REQUEST;
    uint16 desired_min_interval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16 desired_max_interval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16 desired_slave_latency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16 desired_conn_timeout = DEFAULT_DESIRED_CONN_TIMEOUT;

    // Set the GAP Role Parameters
    GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &initial_advertising_enable );
    GAPRole_SetParameter( GAPROLE_ADVERT_OFF_TIME, sizeof( uint16 ), &gapRole_AdvertOffTime );

    GAPRole_SetParameter( GAPROLE_SCAN_RSP_DATA, sizeof ( scanRspData ), scanRspData );
    GAPRole_SetParameter( GAPROLE_ADVERT_DATA, sizeof( advertData ), advertData );

    GAPRole_SetParameter( GAPROLE_PARAM_UPDATE_ENABLE, sizeof( uint8 ), &enable_update_request );
    GAPRole_SetParameter( GAPROLE_MIN_CONN_INTERVAL, sizeof( uint16 ), &desired_min_interval );
    GAPRole_SetParameter( GAPROLE_MAX_CONN_INTERVAL, sizeof( uint16 ), &desired_max_interval );
    GAPRole_SetParameter( GAPROLE_SLAVE_LATENCY, sizeof( uint16 ), &desired_slave_latency );
    GAPRole_SetParameter( GAPROLE_TIMEOUT_MULTIPLIER, sizeof( uint16 ), &desired_conn_timeout );
  }

  // Set the GAP Characteristics
  GGS_SetParameter( GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName );

  // Setup the GAP Bond Manager
  {
    uint32 passkey = DEFAULT_PASSCODE;
    uint8 pairMode = DEFAULT_PAIRING_MODE;
    uint8 mitm = DEFAULT_MITM_MODE;
    uint8 ioCap = DEFAULT_IO_CAPABILITIES;
    uint8 bonding = DEFAULT_BONDING_MODE;

    GAPBondMgr_SetParameter( GAPBOND_DEFAULT_PASSCODE, sizeof ( uint32 ), &passkey );
    GAPBondMgr_SetParameter( GAPBOND_PAIRING_MODE, sizeof ( uint8 ), &pairMode );
    GAPBondMgr_SetParameter( GAPBOND_MITM_PROTECTION, sizeof ( uint8 ), &mitm );
    GAPBondMgr_SetParameter( GAPBOND_IO_CAPABILITIES, sizeof ( uint8 ), &ioCap );
    GAPBondMgr_SetParameter( GAPBOND_BONDING_ENABLED, sizeof ( uint8 ), &bonding );
  }

  // Initialize GATT Client
  VOID GATT_InitClient();

  // Register to receive incoming ATT Indications/Notifications
  GATT_RegisterForInd( glucoseTaskId );

  // Initialize GATT attributes
  GGS_AddService( GATT_ALL_SERVICES );         // GAP
  GATTServApp_AddService( GATT_ALL_SERVICES ); // GATT attributes
  Glucose_AddService( GATT_ALL_SERVICES );
  DevInfo_AddService( );

  // Register for Glucose service callback
  Glucose_Register ( glucoseCB);

  // Setup a delayed profile startup
  tmos_set_event( glucoseTaskId, START_DEVICE_EVT );
}

/*********************************************************************
 * @fn      Glucose_ProcessEvent
 *
 * @brief   Glucose Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The TMOS assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
uint16 Glucose_ProcessEvent( uint8 task_id, uint16 events )
{

  VOID task_id; // TMOS required parameter that isn't used in this function

  if ( events & SYS_EVENT_MSG )
  {
    uint8 *pMsg;

    if ( (pMsg = tmos_msg_receive( glucoseTaskId )) != NULL )
    {
      glucose_ProcessTMOSMsg( (tmos_event_hdr_t *)pMsg );

      // Release the TMOS message
      VOID tmos_msg_deallocate( pMsg );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & START_DEVICE_EVT )
  {
    // Start the Device
    VOID GAPRole_StartDevice( &glucose_PeripheralCBs );

    // Register with bond manager after starting device
    GAPBondMgr_Register( (gapBondCBs_t *) &glucoseBondCB );

    return ( events ^ START_DEVICE_EVT );
  }

  if ( events & NOTI_TIMEOUT_EVT )
  {
    // Send next notification
    glucoseSendNext();

    return ( events ^ NOTI_TIMEOUT_EVT );
  }

  return 0;
}

/*********************************************************************
 * @fn      glucose_ProcessTMOSMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void glucose_ProcessTMOSMsg( tmos_event_hdr_t *pMsg )
{
  switch ( pMsg->event )
  {
  case CTL_PNT_MSG:
      glucoseProcessCtlPntMsg( (glucoseCtlPntMsg_t *) pMsg);
      break;

  default:
      break;
  }
}

/*********************************************************************
 * @fn      glucoseSendNext
 *
 * @brief   Send next notification
 *
 * @return  none
 */
static void glucoseSendNext( void )
{
  if(glucoseSendContext)
  {
    glucoseSendContext = FALSE;
    glucoseContextSend();
  }
  else if(glucoseSendAllRecords)
  {
    glucoseMeasIdx = glucoseFindFirstValidFilteredIndex(glucoseMeasIdx + 1);

    if (glucoseMeasIdx == MEAS_IDX_MAX)
    {
      glucoseMeasIdx = 0;
      glucoseSendAllRecords = FALSE;
      glucoseCtlPntResponse(CTL_PNT_RSP_SUCCESS, CTL_PNT_OP_REQ);
    }
    else
    {
      glucoseMeasSend();
    }
  }
}

/*********************************************************************
 * @fn      glucoseProcessCtlPntMsg
 *
 * @brief   Process Control Point messages
 *
 * @return  none
 */
static void glucoseProcessCtlPntMsg( glucoseCtlPntMsg_t* pMsg)
{
  uint8 opcode = pMsg->data[0];
  uint8 oper = pMsg->data[1];
  UTCTimeStruct time1, time2;
  BOOL opcodeValid = TRUE;
  uint16 seqNum1, seqNum2;

  switch(opcode)
  {
  case CTL_PNT_OP_REQ:
  case CTL_PNT_OP_CLR:
  case CTL_PNT_OP_GET_NUM:
    if(oper == CTL_PNT_OPER_NULL)
    {
      glucoseCtlPntResponse(CTL_PNT_RSP_OPER_INVALID, opcode);
      opcodeValid = FALSE;
    }
    break;

  case CTL_PNT_OP_ABORT:
    if(oper != CTL_PNT_OPER_NULL)
    {
      glucoseCtlPntResponse(CTL_PNT_RSP_OPER_INVALID, opcode);
      opcodeValid = FALSE;
    }
    break;

  default:
    glucoseCtlPntResponse(CTL_PNT_RSP_OPCODE_NOT_SUPPORTED, opcode);
    opcodeValid = FALSE;
    break;
  }

  if(opcodeValid)
  {
    switch(oper)
    {
    case CTL_PNT_OPER_NULL:
    case CTL_PNT_OPER_ALL:
    case CTL_PNT_OPER_FIRST:
    case CTL_PNT_OPER_LAST:
      if(pMsg->len == 2)
      {
        glucoseCtlPntHandleOpcode(opcode, oper, 0, NULL, NULL);
      }
      else
      {
        // No operand should exist, but msg is longer than 2 bytes
        glucoseCtlPntResponse(CTL_PNT_RSP_OPERAND_INVALID, opcode);
      }
      break;


    case CTL_PNT_OPER_RANGE:
      // check filter type
      if (pMsg->data[2] == CTL_PNT_FILTER_SEQNUM && pMsg->len == 7)
      {
        seqNum1 = BUILD_UINT16(pMsg->data[3], pMsg->data[4]);
        seqNum2 = BUILD_UINT16(pMsg->data[5], pMsg->data[6]);

        if ( seqNum1 <= seqNum2 )
        {
          glucoseCtlPntHandleOpcode(opcode, oper, pMsg->data[2], &seqNum1, &seqNum2);
        }
        else
        {
          glucoseCtlPntResponse(CTL_PNT_RSP_OPERAND_INVALID, opcode);
        }
      }
      else if (pMsg->data[2] == CTL_PNT_FILTER_TIME && pMsg->len == 17)
      {
        time1.year = BUILD_UINT16(pMsg->data[3], pMsg->data[4]);
        time1.month = pMsg->data[5];
        time1.day = pMsg->data[6];
        time1.hour = pMsg->data[7];
        time1.minutes = pMsg->data[8];
        time1.seconds = pMsg->data[9];

        time2.year = BUILD_UINT16(pMsg->data[10], pMsg->data[11]);
        time2.month = pMsg->data[12];
        time2.day = pMsg->data[13];
        time2.hour = pMsg->data[14];
        time2.minutes = pMsg->data[15];
        time2.seconds = pMsg->data[16];

        glucoseCtlPntHandleOpcode(opcode, oper, pMsg->data[2], &time1, &time2);
      }
      else
      {
         glucoseCtlPntResponse(CTL_PNT_RSP_OPERAND_INVALID, opcode);
      }
      break;

    case CTL_PNT_OPER_LESS_EQUAL:
    case CTL_PNT_OPER_GREATER_EQUAL:
      // check filter type
      if (pMsg->data[2] == CTL_PNT_FILTER_SEQNUM && pMsg->len == 5)
      {
        seqNum1 = BUILD_UINT16(pMsg->data[3], pMsg->data[4]);

        glucoseCtlPntHandleOpcode(opcode, oper, pMsg->data[2], &seqNum1, NULL);
      }
      else if (pMsg->data[2] == CTL_PNT_FILTER_TIME && pMsg->len == 10)
      {
        time1.year = BUILD_UINT16(pMsg->data[3], pMsg->data[4]);
        time1.month = pMsg->data[5];
        time1.day = pMsg->data[6];
        time1.hour = pMsg->data[7];
        time1.minutes = pMsg->data[8];
        time1.seconds = pMsg->data[9];

        glucoseCtlPntHandleOpcode(opcode, oper, pMsg->data[2], &time1, NULL);
      }
      else
      {
        glucoseCtlPntResponse(CTL_PNT_RSP_FILTER_NOT_SUPPORTED, opcode);
      }
      break;

    default:
      glucoseCtlPntResponse(CTL_PNT_RSP_OPER_NOT_SUPPORTED, opcode);
      break;
    }
  }
}

/*********************************************************************
 * @fn      glucoseGapStateCB
 *
 * @brief   Notification from the profile of a state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void glucoseGapStateCB( gaprole_States_t newState )
{
  // if connected
  if ( newState == GAPROLE_CONNECTED )
  {

  }
  // if disconnected
  else if (gapProfileState == GAPROLE_CONNECTED &&
           newState != GAPROLE_CONNECTED)
  {
    uint8 advState = TRUE;

    // clear state variables
    glucoseSendAllRecords = FALSE;
    glucoseSendContext = FALSE;

    // stop notification timer
    tmos_stop_task(glucoseTaskId, NOTI_TIMEOUT_EVT);

    if ( newState == GAPROLE_WAITING_AFTER_TIMEOUT )
    {
      // link loss timeout-- use fast advertising
      GAP_SetParamValue( TGAP_LIM_DISC_ADV_INT_MIN, DEFAULT_FAST_ADV_INTERVAL );
      GAP_SetParamValue( TGAP_LIM_DISC_ADV_INT_MAX, DEFAULT_FAST_ADV_INTERVAL );
      GAP_SetParamValue( TGAP_LIM_ADV_TIMEOUT, DEFAULT_FAST_ADV_DURATION );
    }
    else
    {
      // Else use slow advertising
      GAP_SetParamValue( TGAP_LIM_DISC_ADV_INT_MIN, DEFAULT_SLOW_ADV_INTERVAL );
      GAP_SetParamValue( TGAP_LIM_DISC_ADV_INT_MAX, DEFAULT_SLOW_ADV_INTERVAL );
      GAP_SetParamValue( TGAP_LIM_ADV_TIMEOUT, DEFAULT_SLOW_ADV_DURATION );
    }

    // Enable advertising
    GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &advState );
  }
  // if advertising stopped
  else if ( gapProfileState == GAPROLE_ADVERTISING &&
            newState == GAPROLE_WAITING )
  {
    // if advertising stopped by user
    if ( glucoseAdvCancelled )
    {
      glucoseAdvCancelled = FALSE;
    }
    // if fast advertising switch to slow
    else if ( GAP_GetParamValue( TGAP_LIM_DISC_ADV_INT_MIN ) == DEFAULT_FAST_ADV_INTERVAL )
    {
      uint8 advState = TRUE;

      GAP_SetParamValue( TGAP_LIM_DISC_ADV_INT_MIN, DEFAULT_SLOW_ADV_INTERVAL );
      GAP_SetParamValue( TGAP_LIM_DISC_ADV_INT_MAX, DEFAULT_SLOW_ADV_INTERVAL );
      GAP_SetParamValue( TGAP_LIM_ADV_TIMEOUT, DEFAULT_SLOW_ADV_DURATION );
      GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &advState );
    }
  }
  // if started
  else if ( newState == GAPROLE_STARTED )
  {
    // Set the system ID from the bd addr
    uint8 systemId[DEVINFO_SYSTEM_ID_LEN];
    GAPRole_GetParameter(GAPROLE_BD_ADDR, systemId);

    // shift three bytes up
    systemId[7] = systemId[5];
    systemId[6] = systemId[4];
    systemId[5] = systemId[3];

    // set middle bytes to zero
    systemId[4] = 0;
    systemId[3] = 0;

    DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemId);
  }
  gapProfileState = newState;
}

/*********************************************************************
 * @fn      glucosePairStateCB
 *
 * @brief   Pairing state callback.
 *
 * @return  none
 */
static void glucosePairStateCB( uint16 connHandle, uint8 state, uint8 status )
{
  if ( state == GAPBOND_PAIRING_STATE_COMPLETE )
  {
    if ( status == SUCCESS )
    {
      linkDBItem_t  *pItem;

      if ( (pItem = linkDB_Find( gapConnHandle )) != NULL )
      {
        // Store bonding state of pairing
        glucoseBonded = ( (pItem->stateFlags & LINK_BOUND) == LINK_BOUND );

        if ( glucoseBonded )
        {
          tmos_memcpy( glucoseBondedAddr, pItem->addr, B_ADDR_LEN );
        }
      }
    }
  }
}

/*********************************************************************
 * @fn      glucosePasscodeCB
 *
 * @brief   Passcode callback.
 *
 * @return  none
 */
static void glucosePasscodeCB( uint8 *deviceAddr, uint16 connectionHandle,
                                        uint8 uiInputs, uint8 uiOutputs )
{
  // Send passcode response
  GAPBondMgr_PasscodeRsp( connectionHandle, SUCCESS, DEFAULT_PASSCODE );
}

/*********************************************************************
 * @fn      glucoseMeasSend
 *
 * @brief   Prepare and send a glucose measurement
 *
 * @return  none
 */
static void glucoseMeasSend(void)
{
  glucoseMeas.pValue = GATT_bm_alloc(gapConnHandle, ATT_HANDLE_VALUE_NOTI,
                                     GLUCOSE_MEAS_LEN, NULL, 0);	
	if (glucoseMeas.pValue != NULL)
	{
		// att value notification structure
		uint8 *p = glucoseMeas.pValue;

		uint8 flags = glucoseMeasArray[glucoseMeasIdx].flags;

		// flags 1 byte long
		*p++ = flags;

		// sequence number
		*p++ = LO_UINT16(glucoseMeasArray[glucoseMeasIdx].seqNum);
		*p++ = HI_UINT16(glucoseMeasArray[glucoseMeasIdx].seqNum);

		// base time; convert day and month from utc time to characteristic format
		*p++ = LO_UINT16(glucoseMeasArray[glucoseMeasIdx].baseTime.year);
		*p++ = HI_UINT16(glucoseMeasArray[glucoseMeasIdx].baseTime.year);
		*p++ = (glucoseMeasArray[glucoseMeasIdx].baseTime.month + 1);
		*p++ = (glucoseMeasArray[glucoseMeasIdx].baseTime.day + 1);
		*p++ = glucoseMeasArray[glucoseMeasIdx].baseTime.hour;
		*p++ = glucoseMeasArray[glucoseMeasIdx].baseTime.minutes;
		*p++ = glucoseMeasArray[glucoseMeasIdx].baseTime.seconds;

		// time offset
		if(flags & GLUCOSE_MEAS_FLAG_TIME_OFFSET)
		{
			*p++ =  LO_UINT16(glucoseMeasArray[glucoseMeasIdx].timeOffset);
			*p++ =  HI_UINT16(glucoseMeasArray[glucoseMeasIdx].timeOffset);
		}

		// concentration
		if(flags & GLUCOSE_MEAS_FLAG_CONCENTRATION)
		{
			*p++ = LO_UINT16(glucoseMeasArray[glucoseMeasIdx].concentration);
			*p++ = HI_UINT16(glucoseMeasArray[glucoseMeasIdx].concentration);
			*p++ =  glucoseMeasArray[glucoseMeasIdx].typeSampleLocation;
		}

		if(flags & GLUCOSE_MEAS_FLAG_STATUS_ANNUNCIATION)
		{
			*p++ =  LO_UINT16(glucoseMeasArray[glucoseMeasIdx].sensorStatus);
			*p++ =  HI_UINT16(glucoseMeasArray[glucoseMeasIdx].sensorStatus);
		}

		glucoseMeas.len = (uint8) (p - glucoseMeas.pValue);

		//Send Measurement
		if(glucoseMeasArray[glucoseMeasIdx].state == GLUCOSE_MEAS_STATE_ALL)
		{
			if(flags & GLUCOSE_MEAS_FLAG_CONTEXT_INFO)
			{
				glucoseSendContext = TRUE;
			}
			
      // Send Measurement.
      if (Glucose_MeasSend(gapConnHandle, &glucoseMeas,  glucoseTaskId) != SUCCESS)
      {
        GATT_bm_free((gattMsg_t *)&glucoseMeas, ATT_HANDLE_VALUE_NOTI);
      }
			
			// Start notification timer.
			tmos_start_task(glucoseTaskId, NOTI_TIMEOUT_EVT, DEFAULT_NOTI_PERIOD);
		}
	}
}

/*********************************************************************
 * @fn      glucoseContextSend
 *
 * @brief   Prepare and send a glucose measurement context
 *
 * @return  none
 */
static void glucoseContextSend(void)
{
	glucoseContext.pValue = GATT_bm_alloc(gapConnHandle, ATT_HANDLE_VALUE_NOTI,
																				GLUCOSE_CONTEXT_LEN, NULL, 0);
	if (glucoseContext.pValue != NULL)
	{	
		// att value notification structure
		uint8 *p = glucoseContext.pValue;

		uint8 flags = glucoseContextArray[glucoseMeasIdx].flags;

		// flags 1 byte long
		*p++ = flags;

		// sequence number
		*p++ = LO_UINT16(glucoseContextArray[glucoseMeasIdx].seqNum);
		*p++ = HI_UINT16(glucoseContextArray[glucoseMeasIdx].seqNum);

		if(flags & GLUCOSE_CONTEXT_FLAG_EXTENDED)
			*p++ = glucoseContextArray[glucoseMeasIdx].extendedFlags;

		if(flags & GLUCOSE_CONTEXT_FLAG_CARBO)
		{
			*p++ = glucoseContextArray[glucoseMeasIdx].carboId;
			*p++ = LO_UINT16(glucoseContextArray[glucoseMeasIdx].carboVal);
			*p++ = HI_UINT16(glucoseContextArray[glucoseMeasIdx].carboVal);
		}

		if (flags & GLUCOSE_CONTEXT_FLAG_MEAL)
		{
			*p++ = glucoseContextArray[glucoseMeasIdx].mealVal;
		}

		if(flags & GLUCOSE_CONTEXT_FLAG_TESTER_HEALTH)
		{
			*p++ = glucoseContextArray[glucoseMeasIdx].TesterHealthVal;
		}

		if(flags & GLUCOSE_CONTEXT_FLAG_EXERCISE)
		{
			*p++ = LO_UINT16(glucoseContextArray[glucoseMeasIdx].exerciseDuration);
			*p++ = HI_UINT16(glucoseContextArray[glucoseMeasIdx].exerciseDuration);
			*p++ = glucoseContextArray[glucoseMeasIdx].exerciseIntensity;
		}

		if(flags & GLUCOSE_CONTEXT_FLAG_MEDICATION)
		{
			*p++ = glucoseContextArray[glucoseMeasIdx].medId;
			*p++ = LO_UINT16(glucoseContextArray[glucoseMeasIdx].medVal);
			*p++ = HI_UINT16(glucoseContextArray[glucoseMeasIdx].medVal);
		}

		if(flags & GLUCOSE_CONTEXT_FLAG_HbA1c)
		{
			*p++ = LO_UINT16(glucoseContextArray[glucoseMeasIdx].HbA1cVal);
			*p++ = HI_UINT16(glucoseContextArray[glucoseMeasIdx].HbA1cVal);
		}


		glucoseContext.len = (uint8) (p - glucoseContext.pValue);

		// Send Measurement
		if(glucoseMeasArray[glucoseMeasIdx].state == GLUCOSE_MEAS_STATE_ALL)
		{
			
      // Send Measurement.
      if (Glucose_ContextSend(gapConnHandle, &glucoseContext, 
                              glucoseTaskId) != SUCCESS)
      {
        GATT_bm_free((gattMsg_t *)&glucoseContext, ATT_HANDLE_VALUE_NOTI);
      }
      
      // Start notification timer.			
			tmos_start_task(glucoseTaskId, NOTI_TIMEOUT_EVT, DEFAULT_NOTI_PERIOD);
		}
	}
}

/*********************************************************************
 * @fn      glucoseCtlPntResponse
 *
 * @brief   Send a record control point response
 *
 * @param   rspCode - the status code of the operation
 * @param   opcode - control point opcode
 *
 * @return  none
 */
static void glucoseCtlPntResponse(uint8 rspCode, uint8 opcode)
{
  glucoseCtlPntRsp.pValue = GATT_bm_alloc(gapConnHandle, ATT_HANDLE_VALUE_IND,
                                          GLUCOSE_CTL_PNT_LEN, NULL, 0);
  if (glucoseCtlPntRsp.pValue != NULL)
  {	
		glucoseCtlPntRsp.len = 4;
		glucoseCtlPntRsp.pValue[0] = CTL_PNT_OP_REQ_RSP;
		glucoseCtlPntRsp.pValue[1] = CTL_PNT_OPER_NULL;
		glucoseCtlPntRsp.pValue[2] = opcode;
		glucoseCtlPntRsp.pValue[3] = rspCode;
		
    // Send indication.
    if (Glucose_CtlPntIndicate(gapConnHandle, &glucoseCtlPntRsp,
                               glucoseTaskId) != SUCCESS)
    {
      GATT_bm_free((gattMsg_t *)&glucoseCtlPntRsp, ATT_HANDLE_VALUE_IND);
    }
	}
}

/*********************************************************************
 * @fn      glucoseCtlPntNumRecordsResponse
 *
 * @brief   Send a record control point num records response
 *
 * @param   numRecords - number of records found
 * @param   oper - operator used to filter the record list
 *
 * @return  none
 */
static void glucoseCtlPntNumRecordsResponse(uint16 numRecords)
{
  glucoseCtlPntRsp.len = 4;
  glucoseCtlPntRsp.pValue[0] = CTL_PNT_OP_NUM_RSP;
  glucoseCtlPntRsp.pValue[1] = CTL_PNT_OPER_NULL;
  glucoseCtlPntRsp.pValue[2] = LO_UINT16(numRecords);
  glucoseCtlPntRsp.pValue[3] = HI_UINT16(numRecords);

  Glucose_CtlPntIndicate(gapConnHandle, &glucoseCtlPntRsp,  glucoseTaskId);
}

/*********************************************************************
 * @fn      glucoseCB
 *
 * @brief   Callback function for glucose service.
 *
 * @param   event - service event
 *
 * @return  none
 */
static void glucoseCB(uint8 event, uint8* valueP, uint8 len)
{

  switch (event)
  {
  case GLUCOSE_CTL_PNT_CMD:
    {
      glucoseCtlPntMsg_t* msgPtr;

      // Send the address to the task
      msgPtr = (glucoseCtlPntMsg_t *)tmos_msg_allocate( sizeof(glucoseCtlPntMsg_t) );
      if ( msgPtr )
      {
        msgPtr->hdr.event = CTL_PNT_MSG;
        msgPtr->len = len;
        tmos_memcpy(msgPtr->data, valueP, len);

        tmos_msg_send( glucoseTaskId, (uint8 *)msgPtr );
      }
    }
    break;

  default:
    break;
  }
}

/*********************************************************************
 * @fn      glucoseMarkAllRecords
 *
 * @brief   Set the valid/filtered flags in the measurement array
 *
 * @param   setBits - if TRUE bits are set, otherwise cleared
 * @param   mask - bits to set or clear
 *
 * @return  number of valid records set or cleared.
 */
static uint8 glucoseMarkAllRecords(BOOL setBits, uint8 mask)
{
  uint8 i;
  uint8 count = 0;

  for(i = 0; i < MEAS_IDX_MAX; i++)
  {
    if(glucoseMeasArray[i].state & GLUCOSE_MEAS_STATE_VALID)
        count++;

    if(setBits)
      glucoseMeasArray[i].state |= mask;
    else
      glucoseMeasArray[i].state &= ~mask;

  }

  return count;
}

/*********************************************************************
 * @fn      glucoseMarkFirstValidRecord
 *
 * @brief   Set the state in the first valid record
 *
 * @param   setBits - if TRUE bits are set, otherwise cleared
 * @param   mask - bits to set or clear
 *
 * @return  number of valid records set or cleared.
 */
static uint8 glucoseMarkFirstValidRecord(BOOL setBits, uint8 mask)
{
  uint8 i;

  for(i = 0; i < MEAS_IDX_MAX; i++)
  {
    if(glucoseMeasArray[i].state & GLUCOSE_MEAS_STATE_VALID)
    {
      if(setBits)
        glucoseMeasArray[i].state |= mask;
      else
        glucoseMeasArray[i].state &= ~mask;

      return 1;
    }
  }

  return 0;
}

/*********************************************************************
 * @fn      glucoseMarkLastValidRecord
 *
 * @brief   Set the state in the last valid record
 *
 * @param   setBits - if TRUE bits are set, otherwise cleared
 * @param   mask - bits to set or clear
 *
 * @return  number of valid records set or cleared.
 */
static uint8 glucoseMarkLastValidRecord(BOOL setBits, uint8 mask)
{
  uint8 i;

  for(i = MEAS_IDX_MAX; i > 0; i--)
  {
    if(glucoseMeasArray[i-1].state & GLUCOSE_MEAS_STATE_VALID)
    {
      if(setBits)
        glucoseMeasArray[i-1].state |= mask;
      else
        glucoseMeasArray[i-1].state &= ~mask;

      return 1;
    }
  }

  return 0;
}

/*********************************************************************
 * @fn      glucoseVerifyTime
 *
 * @brief   Verify time values are suitable for filtering
 *
 * @param   pTime - UTC time struct
 *
 * @return  TRUE if time is ok, FALSE otherwise
 */
static uint8 glucoseVerifyTime(UTCTimeStruct* pTime)
{
  // sanity check year
  if (pTime->year < 2000 || pTime->year > 2111)
  {
    return FALSE;
  }
  // check day range
  if (pTime->day == 0 || pTime->day > 31)
  {
    return FALSE;
  }
  // check month range
  if (pTime->month == 0 || pTime->month > 12)
  {
    return FALSE;
  }

  // adjust month and day; utc time uses 0-11 and 0-30, characteristic uses 1-12 and 1-31
  pTime->day--;
  pTime->month--;

  return TRUE;
}

/*********************************************************************
 * @fn      glucoseMarkAllRecordsEarlierThanTime
 *
 * @brief   Mark all records earlier than a specific time
 *
 * @param   time - time filter
 * @param   set - if TRUE bits are set, otherwise cleared
 * @param   mask - bits to set or clear
 *
 * @return  number of valid records set or cleared.
 */
static uint8 glucoseMarkAllRecordsEarlierThanTime(UTCTimeStruct* time, BOOL set, uint8 mask)
{
  uint8 i;
  uint8 count = 0;
  UTCTime testTime;
  UTCTime recordTime;

  if (glucoseVerifyTime( time ) == FALSE)
  {
    return 0;
  }

  testTime = tmos_ConvertUTCSecs( time );

  for(i = 0; i < MEAS_IDX_MAX; i++)
  {
    recordTime = tmos_ConvertUTCSecs(&glucoseMeasArray[i].baseTime ) + glucoseMeasArray[i].timeOffset;

    if(recordTime <= testTime)
    {
      if(glucoseMeasArray[i].state & GLUCOSE_MEAS_STATE_VALID)
        count++;

      if(set)
        glucoseMeasArray[i].state |= mask;
      else
        glucoseMeasArray[i].state &= ~mask;
    }
  }
  return count;
}

/*********************************************************************
 * @fn      glucoseMarkAllRecordsLaterThanTime
 *
 * @brief   Mark all records later than a specific time
 *
 * @param   time - time filter
 * @param   set - if TRUE bits are set, otherwise cleared
 * @param   mask - bits to set or clear
 *
 * @return  number of valid records set or cleared.
 */
static uint8 glucoseMarkAllRecordsLaterThanTime(UTCTimeStruct* time, BOOL set, uint8 mask)
{
  uint8 i;
  uint8 count = 0;
  UTCTime testTime;
  UTCTime recordTime;

  if (glucoseVerifyTime( time ) == FALSE)
  {
    return 0;
  }

  testTime = tmos_ConvertUTCSecs( time );

  for(i = 0; i < MEAS_IDX_MAX; i++)
  {
    recordTime = tmos_ConvertUTCSecs(&glucoseMeasArray[i].baseTime ) + glucoseMeasArray[i].timeOffset;
    if(recordTime >= testTime)
    {
      if(glucoseMeasArray[i].state & GLUCOSE_MEAS_STATE_VALID)
        count++;

      if(set)
        glucoseMeasArray[i].state |= mask;
      else
        glucoseMeasArray[i].state &= ~mask;
    }
  }

  return count;
}

/*********************************************************************
 * @fn      glucoseMarkAllRecordsInRangeTime
 *
 * @brief   Mark all records between two times
 *
 * @param   time1 - time filter low end of range
 * @param   time2 - time filter high end of range
 * @param   set - if TRUE bits are set, otherwise cleared
 * @param   mask - bits to set or clear
 *
 * @return  number of valid records set or cleared.
 */
static uint8 glucoseMarkAllRecordsInRangeTime(UTCTimeStruct* time1, UTCTimeStruct* time2, BOOL set, uint8 mask)
{
  uint8 i;
  uint8 count = 0;
  UTCTime lowEnd, highEnd;
  UTCTime recordTime;

  if (glucoseVerifyTime( time1 ) == FALSE)
  {
    return 0;
  }
  if (glucoseVerifyTime( time2 ) == FALSE)
  {
    return 0;
  }

  lowEnd = tmos_ConvertUTCSecs( time1 );
  highEnd = tmos_ConvertUTCSecs( time2 );

  for(i = 0; i < MEAS_IDX_MAX; i++)
  {
    recordTime = tmos_ConvertUTCSecs(&glucoseMeasArray[i].baseTime ) + glucoseMeasArray[i].timeOffset;
    if((recordTime >= lowEnd) && (recordTime <= highEnd))
    {
      if(glucoseMeasArray[i].state & GLUCOSE_MEAS_STATE_VALID)
        count++;

      if(set)
        glucoseMeasArray[i].state |= mask;
      else
        glucoseMeasArray[i].state &= ~mask;
    }
  }

  return count;
}


/*********************************************************************
 * @fn      glucoseMarkAllRecordsEarlierThanSeqNum
 *
 * @brief   Mark all records earlier than a specific sequence number
 *
 * @param   pSeqNum - filter
 * @param   set - if TRUE bits are set, otherwise cleared
 * @param   mask - bits to set or clear
 *
 * @return  number of valid records set or cleared.
 */
static uint8 glucoseMarkAllRecordsEarlierThanSeqNum(uint16 *pSeqNum, BOOL set, uint8 mask)
{
  uint8 i;
  uint8 count = 0;

  for(i = 0; i < MEAS_IDX_MAX; i++)
  {
    if(glucoseMeasArray[i].seqNum <= *pSeqNum)
    {
      if(glucoseMeasArray[i].state & GLUCOSE_MEAS_STATE_VALID)
        count++;

      if(set)
        glucoseMeasArray[i].state |= mask;
      else
        glucoseMeasArray[i].state &= ~mask;
    }
  }
  return count;
}

/*********************************************************************
 * @fn      glucoseMarkAllRecordsLaterThanSeqNum
 *
 * @brief   Mark all records later than a specific sequence number
 *
 * @param   pSeqNum - filter
 * @param   set - if TRUE bits are set, otherwise cleared
 * @param   mask - bits to set or clear
 *
 * @return  number of valid records set or cleared.
 */
static uint8 glucoseMarkAllRecordsLaterThanSeqNum(uint16 *pSeqNum, BOOL set, uint8 mask)
{
  uint8 i;
  uint8 count = 0;

  for(i = 0; i < MEAS_IDX_MAX; i++)
  {
    if(glucoseMeasArray[i].seqNum >= *pSeqNum)
    {
      if(glucoseMeasArray[i].state & GLUCOSE_MEAS_STATE_VALID)
        count++;

      if(set)
        glucoseMeasArray[i].state |= mask;
      else
        glucoseMeasArray[i].state &= ~mask;
    }
  }

  return count;
}

/*********************************************************************
 * @fn      glucoseMarkAllRecordsInRangeSeqNum
 *
 * @brief   Mark all records between two sequence numbers
 *
 * @param   pSeqNum1 - filter low end of range
 * @param   pSeqNum2 - filter high end of range
 * @param   set - if TRUE bits are set, otherwise cleared
 * @param   mask - bits to set or clear
 *
 * @return  number of valid records set or cleared.
 */
static uint8 glucoseMarkAllRecordsInRangeSeqNum(uint16 *pSeqNum1, uint16 *pSeqNum2, BOOL set, uint8 mask)
{
  uint8 i;
  uint8 count = 0;

  for(i = 0; i < MEAS_IDX_MAX; i++)
  {
    if((glucoseMeasArray[i].seqNum >= *pSeqNum1) && (glucoseMeasArray[i].seqNum <= *pSeqNum2))
    {
      if(glucoseMeasArray[i].state & GLUCOSE_MEAS_STATE_VALID)
        count++;

      if(set)
        glucoseMeasArray[i].state |= mask;
      else
        glucoseMeasArray[i].state &= ~mask;
    }
  }

  return count;
}

/*********************************************************************
 * @fn      glucoseFilterRecords
 *
 * @brief   Call the correct filter function for a particular operator
 *
 * @param   oper - control point operator
 * @param   filterType - control point filter type
 * @param   param1 - filter (if applicable), otherwise NULL
 * @param   param2 - filter (if applicable), otherwise NULL
 * @param   set - if TRUE bits are set, otherwise cleared
 * @param   mask - bits to set or clear
 * @param   opcode - control point opcode
 *
 * @return  number of valid records within filter times
 */
static uint8 glucoseFilterRecords(uint8 oper, uint8 filterType, void* param1, void* param2, BOOL set, uint8 mask, uint8 opcode)
{
  uint8 numFiltered = 0;

  switch(oper)
  {
    case CTL_PNT_OPER_NULL:
      glucoseCtlPntResponse(CTL_PNT_RSP_OPER_NOT_SUPPORTED, opcode);
      break;

    case CTL_PNT_OPER_ALL:
      numFiltered = glucoseMarkAllRecords(set, mask);
      break;

    case CTL_PNT_OPER_FIRST:
      numFiltered = glucoseMarkFirstValidRecord(set, mask);
      break;

    case CTL_PNT_OPER_LAST:
      numFiltered = glucoseMarkLastValidRecord(set, mask);
      break;

    case CTL_PNT_OPER_RANGE:
      if (filterType == CTL_PNT_FILTER_SEQNUM)
      {
        numFiltered = glucoseMarkAllRecordsInRangeSeqNum(param1, param2, set, mask);
      }
      else
      {
        numFiltered = glucoseMarkAllRecordsInRangeTime(param1, param2, set, mask);
      }
      break;

    case CTL_PNT_OPER_LESS_EQUAL:
      if (filterType == CTL_PNT_FILTER_SEQNUM)
      {
        numFiltered = glucoseMarkAllRecordsEarlierThanSeqNum(param1, set, mask);
      }
      else
      {
        numFiltered = glucoseMarkAllRecordsEarlierThanTime(param1, set, mask);
      }
      break;

    case CTL_PNT_OPER_GREATER_EQUAL:
      if (filterType == CTL_PNT_FILTER_SEQNUM)
      {
        numFiltered = glucoseMarkAllRecordsLaterThanSeqNum(param1, set, mask);
      }
      else
      {
        numFiltered = glucoseMarkAllRecordsLaterThanTime(param1, set, mask);
      }
      break;

    default:
      glucoseCtlPntResponse(CTL_PNT_RSP_OPER_INVALID, opcode);
      break;

  }

  return numFiltered;
}

/*********************************************************************
 * @fn      glucoseFindFirstValidFilteredIndex
 *
 * @brief   Find the first valid entry, that also is in the last filter
 *
 * @param   startIndex - starting index of search
 *
 * @return  index of next valid and filtered record or MEAS_IDX_MAX if none
 */
static uint8 glucoseFindFirstValidFilteredIndex(uint8 startIndex)
{
  uint8 i;

  if(startIndex >= MEAS_IDX_MAX)
    return MEAS_IDX_MAX;

  for(i = startIndex; i < MEAS_IDX_MAX; i++)
  {
    if(glucoseMeasArray[i].state == GLUCOSE_MEAS_STATE_ALL)
      break;
  }

  return i;
}


/*********************************************************************
 * @fn      glucoseCtlPntHandleOpcode
 *
 * @brief   Handle control point opcodes
 *
 * @param   opcode - control point opcode
 * @param   oper - control point operator
 * @param   filterType - control point filter type
 * @param   param1 - filter (if applicable), otherwise NULL
 * @param   param2 - filter (if applicable), otherwise NULL
 *
 * @return  none
 */
static void glucoseCtlPntHandleOpcode(uint8 opcode, uint8 oper, uint8 filterType, void* param1, void* param2)
{
  switch(opcode)
  {
  case CTL_PNT_OP_REQ:
    //Clear all filter bits, before running the new test
    glucoseMarkAllRecords(FALSE, GLUCOSE_MEAS_STATE_FILTER_PASS);
    if(glucoseFilterRecords(oper, filterType, param1, param2, TRUE, GLUCOSE_MEAS_STATE_FILTER_PASS, opcode) > 0)
    {
      glucoseSendAllRecords = TRUE;
      glucoseMeasIdx = glucoseFindFirstValidFilteredIndex(0);
      glucoseMeasSend();
    }
    else
    {
      glucoseCtlPntResponse(CTL_PNT_RSP_NO_RECORDS, opcode);
    }
    break;

  case CTL_PNT_OP_CLR:
    if(glucoseFilterRecords(oper, filterType, param1, param2, FALSE, GLUCOSE_MEAS_STATE_VALID, opcode) > 0)
    {
      glucoseCtlPntResponse(CTL_PNT_RSP_SUCCESS, opcode);
    }
    else
    {
      glucoseCtlPntResponse(CTL_PNT_RSP_NO_RECORDS, opcode);
    }
    break;

  case CTL_PNT_OP_ABORT:
    glucoseSendAllRecords = FALSE;
    glucoseSendContext = FALSE;
    tmos_stop_task(glucoseTaskId, NOTI_TIMEOUT_EVT);
    glucoseCtlPntResponse(CTL_PNT_RSP_SUCCESS, opcode);
    break;

  case CTL_PNT_OP_GET_NUM:
    // Clear all previous filter bits before running the test.
    glucoseMarkAllRecords(FALSE, GLUCOSE_MEAS_STATE_FILTER_PASS);
    glucoseCtlPntNumRecordsResponse(glucoseFilterRecords(oper, filterType, param1, param2, TRUE, GLUCOSE_MEAS_STATE_FILTER_PASS, opcode));
    break;

  default:
    break;
  }
}


/*********************************************************************
*********************************************************************/
