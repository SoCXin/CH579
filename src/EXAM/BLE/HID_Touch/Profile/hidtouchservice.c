/********************************** (C) COPYRIGHT *******************************
 * File Name          : hidtouchservice.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/12
 * Description        : ��������       
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "CONFIG.h"
#include "hidtouchservice.h"
#include "hiddev.h"
#include "battservice.h"


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
#define 	DEF_DESKTOP_X_MAX														800
#define 	DEF_DESKTOP_Y_MAX														800
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// HID service
CONST uint8 hidServUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(HID_SERV_UUID), HI_UINT16(HID_SERV_UUID)
};


// HID Information characteristic
CONST uint8 hidInfoUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(HID_INFORMATION_UUID), HI_UINT16(HID_INFORMATION_UUID)
};

// HID Report Map characteristic
CONST uint8 hidReportMapUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(REPORT_MAP_UUID), HI_UINT16(REPORT_MAP_UUID)
};

// HID Control Point characteristic
CONST uint8 hidControlPointUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(HID_CTRL_PT_UUID), HI_UINT16(HID_CTRL_PT_UUID)
};

// HID Report characteristic
CONST uint8 hidReportUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(REPORT_UUID), HI_UINT16(REPORT_UUID)
};

// HID Protocol Mode characteristic
CONST uint8 hidProtocolModeUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(PROTOCOL_MODE_UUID), HI_UINT16(PROTOCOL_MODE_UUID)
};

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// HID Information characteristic value
static CONST uint8 hidInfo[HID_INFORMATION_LEN] =
{
  LO_UINT16(0x0111), HI_UINT16(0x0111),             // bcdHID (USB HID version)
  0x00,                                             // bCountryCode
  HID_FEATURE_FLAGS                                 // Flags
};

// HID Report Map characteristic value
static CONST uint8 hidReportMap[] =
{
    
    0x05, 0x0D,                               // Usage Page (Digitizer)
    0x09, 0x04,                               // Usage (Touch Screen)
    0xA1, 0x01,                               // Collection (Application)
    0x85, 0x01,                               /* REPORT_ID (01) */
       
    0x05, 0x0D,                               //   Usage Page (Digitizer)
    0x09, 0x22,                               //   Usage (Finger)
    0xA1, 0x02,                               //   Collection (Logical)
    0x05, 0x0D,                               //     Usage Page (Digitizer)
    0x09, 0x42,                               /* USAGE (TIP SWITCH) */
    0x15, 0x00,                               //     Logical Minimum......... (0)
    0x25, 0x01,                               //     Logical Maximum......... (1)
    0x75, 0x01,                               //     Report Size............. (1)
    0x95, 0x01,                               //     Report Count............ (1)
    0x81, 0x02,                               //     Input...................(Data, Variable, Absolute)
    0x75, 0x07,                               //     Report Size............. (7)
    0x95, 0x01,                               //     Report Count............ (1)
    0x81, 0x01,                               //     Input...................(Constant)
    0x09, 0x51, /* USAGE (Contact Identifier) */
    0x75, 0x08,                               //     Report Size............. (8)
    0x95, 0x01,                               //     Report Count............ (1)
    0x81, 0x02,                               //     Input...................(Data, Variable, Absolute)	
    0x05, 0x01,                               //     Usage Page (Generic Desktop)	
    0x09, 0x30,                               //     Usage (X)
    0x15, 0x00,                               //     Logical Minimum......... (0) 
    /*48 49 50*/
    0x26, (uint8)DEF_DESKTOP_X_MAX, (uint8)( DEF_DESKTOP_X_MAX >> 8 ), /* LOGIC_MAX (0X0800) */ 
    0x75, 0x10,                               //     Report Size............. (16)
    0x95, 0x01,                               //     Report Count............ (1)
    0x81, 0x02,                               //     Input...................(Data, Variable, Absolute)
    0x09, 0x31,                               //     Usage (Y)
    0x15, 0x00,                               //     Logical Minimum......... (0)
    /*61 62 63*/
    0x26, (uint8)DEF_DESKTOP_Y_MAX, (uint8)( DEF_DESKTOP_Y_MAX >> 8 ), /* LOGIC_MAX (0X0800) */ 
    0x75, 0x10,                               //     Report Size............. (16) 64 65
    0x95, 0x01,                               //     Report Count............ (1)  66 67
    0x81, 0x02,                               //     Input...................(Data, Variable, Absolute) 68 69
    0xC0,                                     //   End Collection 70
    0xc0,                                     // End Collection  

};

// HID report map length
uint16 hidReportMapLen = sizeof(hidReportMap);

// HID report mapping table
static hidRptMap_t  hidRptMap[HID_NUM_REPORTS];

/*********************************************************************
 * Profile Attributes - variables
 */

// HID Service attribute
static CONST gattAttrType_t hidService = { ATT_BT_UUID_SIZE, hidServUUID };

// Include attribute (Battery service)
static uint16 include = GATT_INVALID_HANDLE;

// HID Information characteristic
static uint8 hidInfoProps = GATT_PROP_READ;

// HID Report Map characteristic
static uint8 hidReportMapProps = GATT_PROP_READ;

// HID External Report Reference Descriptor
static uint8 hidExtReportRefDesc[ATT_BT_UUID_SIZE] =
             { LO_UINT16(BATT_LEVEL_UUID), HI_UINT16(BATT_LEVEL_UUID) };

// HID Control Point characteristic
static uint8 hidControlPointProps = GATT_PROP_WRITE_NO_RSP;
static uint8 hidControlPoint;

// HID Protocol Mode characteristic
static uint8 hidProtocolModeProps = GATT_PROP_READ | GATT_PROP_WRITE_NO_RSP;
uint8 hidProtocolMode = HID_PROTOCOL_MODE_REPORT;

// HID Report characteristic, touch input
static uint8 hidReportTouchInProps = GATT_PROP_READ | GATT_PROP_NOTIFY;
static uint8 hidReportTouchIn;
static gattCharCfg_t hidReportTouchInClientCharCfg[GATT_MAX_NUM_CONN];

// HID Report Reference characteristic descriptor, touch input
static uint8 hidReportRefTouchIn[HID_REPORT_REF_LEN] =
             { HID_RPT_ID_TOUCH_IN, HID_REPORT_TYPE_INPUT };

// Feature Report
static uint8 hidReportFeatureProps = GATT_PROP_READ | GATT_PROP_WRITE;
static uint8 hidReportFeature;

// HID Report Reference characteristic descriptor, Feature
static uint8 hidReportRefFeature[HID_REPORT_REF_LEN] =
             { HID_RPT_ID_FEATURE, HID_REPORT_TYPE_FEATURE };

/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t hidAttrTbl[] =
{
  // HID Service
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *) &hidService                     /* pValue */
  },

    // Included service (battery)
    {
      { ATT_BT_UUID_SIZE, includeUUID },
      GATT_PERMIT_READ,
      0,
      (uint8 *)&include
    },

    // HID Information characteristic declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &hidInfoProps
    },

      // HID Information characteristic
      {
        { ATT_BT_UUID_SIZE, hidInfoUUID },
        GATT_PERMIT_ENCRYPT_READ,
        0,
        (uint8 *) hidInfo
      },

    // HID Control Point characteristic declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &hidControlPointProps
    },

      // HID Control Point characteristic
      {
        { ATT_BT_UUID_SIZE, hidControlPointUUID },
        GATT_PERMIT_ENCRYPT_WRITE,
        0,
        &hidControlPoint
      },

    // HID Protocol Mode characteristic declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &hidProtocolModeProps
    },

      // HID Protocol Mode characteristic
      {
        { ATT_BT_UUID_SIZE, hidProtocolModeUUID },
        GATT_PERMIT_ENCRYPT_READ | GATT_PERMIT_ENCRYPT_WRITE,
        0,
        &hidProtocolMode
      },


    // HID Report Map characteristic declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &hidReportMapProps
    },

      // HID Report Map characteristic
      {
        { ATT_BT_UUID_SIZE, hidReportMapUUID },
        GATT_PERMIT_ENCRYPT_READ,
        0,
        (uint8 *) hidReportMap
      },

      // HID External Report Reference Descriptor
      {
        { ATT_BT_UUID_SIZE, extReportRefUUID },
        GATT_PERMIT_READ,
        0,
        hidExtReportRefDesc


      },

    // HID Report characteristic, touch input declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &hidReportTouchInProps
    },

      // HID Report characteristic, touch input
      {
        { ATT_BT_UUID_SIZE, hidReportUUID },
        GATT_PERMIT_READ,
        0,
        &hidReportTouchIn
      },

      // HID Report characteristic client characteristic configuration
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_ENCRYPT_WRITE,
        0,
        (uint8 *) &hidReportTouchInClientCharCfg
      },

      // HID Report Reference characteristic descriptor, touch input
      {
        { ATT_BT_UUID_SIZE, reportRefUUID },
        GATT_PERMIT_READ,
        0,
        hidReportRefTouchIn
      },

    // Feature Report declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &hidReportFeatureProps
    },

      // Feature Report
      {
        { ATT_BT_UUID_SIZE,  hidReportUUID},
        GATT_PERMIT_ENCRYPT_READ | GATT_PERMIT_ENCRYPT_WRITE,
        0,
        &hidReportFeature
      },

      // HID Report Reference characteristic descriptor, feature
      {
        { ATT_BT_UUID_SIZE, reportRefUUID },
        GATT_PERMIT_READ,
        0,
        hidReportRefFeature
      },
};

// Attribute index enumeration-- these indexes match array elements above
enum
{
  HID_SERVICE_IDX,                // HID Service
  HID_INCLUDED_SERVICE_IDX,       // Included Service
  HID_INFO_DECL_IDX,              // HID Information characteristic declaration
  HID_INFO_IDX,                   // HID Information characteristic
  HID_CONTROL_POINT_DECL_IDX,     // HID Control Point characteristic declaration
  HID_CONTROL_POINT_IDX,          // HID Control Point characteristic
  HID_PROTOCOL_MODE_DECL_IDX,     // HID Protocol Mode characteristic declaration
  HID_PROTOCOL_MODE_IDX,          // HID Protocol Mode characteristic
  HID_REPORT_MAP_DECL_IDX,        // HID Report Map characteristic declaration
  HID_REPORT_MAP_IDX,             // HID Report Map characteristic
  HID_EXT_REPORT_REF_DESC_IDX,    // HID External Report Reference Descriptor
  HID_REPORT_TOUCH_IN_DECL_IDX,     // HID Report characteristic, mouse input declaration
  HID_REPORT_TOUCH_IN_IDX,          // HID Report characteristic, mouse input
  HID_REPORT_TOUCH_IN_CCCD_IDX,     // HID Report characteristic client characteristic configuration
  HID_REPORT_REF_TOUCH_IN_IDX,      // HID Report Reference characteristic descriptor, mouse input
  HID_FEATURE_DECL_IDX,           // Feature Report declaration
  HID_FEATURE_IDX,                // Feature Report
  HID_REPORT_REF_FEATURE_IDX      // HID Report Reference characteristic descriptor, feature
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * PROFILE CALLBACKS
 */

// Service Callbacks
gattServiceCBs_t hidCBs =
{
  HidDev_ReadAttrCB,  // Read callback function pointer
  HidDev_WriteAttrCB, // Write callback function pointer
  NULL                // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Hid_AddService
 *
 * @brief   Initializes the HID Service by registering
 *          GATT attributes with the GATT server.
 *
 * @return  Success or Failure
 */
bStatus_t Hid_AddService( void )
{
  uint8 status = SUCCESS;

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, hidReportTouchInClientCharCfg );

  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService( hidAttrTbl, GATT_NUM_ATTRS( hidAttrTbl ),GATT_MAX_ENCRYPT_KEY_SIZE, &hidCBs );

  // Set up included service
  Batt_GetParameter( BATT_PARAM_SERVICE_HANDLE,
                     &GATT_INCLUDED_HANDLE( hidAttrTbl, HID_INCLUDED_SERVICE_IDX ) );

  // Construct map of reports to characteristic handles
  // Each report is uniquely identified via its ID and type

  // Mouse input report
  hidRptMap[0].id = hidReportRefTouchIn[0];
  hidRptMap[0].type = hidReportRefTouchIn[1];
  hidRptMap[0].handle = hidAttrTbl[HID_REPORT_TOUCH_IN_IDX].handle;
  hidRptMap[0].cccdHandle = hidAttrTbl[HID_REPORT_TOUCH_IN_CCCD_IDX].handle;
  hidRptMap[0].mode = HID_PROTOCOL_MODE_REPORT;

  // Feature report
  hidRptMap[1].id = hidReportRefFeature[0];
  hidRptMap[1].type = hidReportRefFeature[1];
  hidRptMap[1].handle = hidAttrTbl[HID_FEATURE_IDX].handle;
  hidRptMap[1].cccdHandle = 0;
  hidRptMap[1].mode = HID_PROTOCOL_MODE_REPORT;

  // Battery level input report
  Batt_GetParameter( BATT_PARAM_BATT_LEVEL_IN_REPORT, &(hidRptMap[2]) );

  // Setup report ID map
  HidDev_RegisterReports( HID_NUM_REPORTS, hidRptMap );

  return ( status );
}

/*********************************************************************
 * @fn      Hid_SetParameter
 *
 * @brief   Set a HID Kbd parameter.
 *
 * @param   id - HID report ID.
 * @param   type - HID report type.
 * @param   uuid - attribute uuid.
 * @param   len - length of data to right.
 * @param   pValue - pointer to data to write.  This is dependent on
 *          the input parameters and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  GATT status code.
 */
uint8 Hid_SetParameter( uint8 id, uint8 type, uint16 uuid, uint8 len, void *pValue )
{
  bStatus_t ret = SUCCESS;

  switch ( uuid )
  {
    case REPORT_UUID:
      if ( type == HID_REPORT_TYPE_FEATURE )
      {
        if ( len == 1 )
        {
          hidReportFeature = *((uint8 *)pValue);
        }
        else
        {
          ret = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        ret = ATT_ERR_ATTR_NOT_FOUND;
      }
      break;

    default:
      // ignore the request
      break;
  }

  return ( ret );
}

/*********************************************************************
 * @fn      Hid_GetParameter
 *
 * @brief   Get a HID Kbd parameter.
 *
 * @param   id - HID report ID.
 * @param   type - HID report type.
 * @param   uuid - attribute uuid.
 * @param   pLen - length of data to be read
 * @param   pValue - pointer to data to get.  This is dependent on
 *          the input parameters and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  GATT status code.
 */
uint8 Hid_GetParameter( uint8 id, uint8 type, uint16 uuid, uint16 *pLen, void *pValue )
{
  switch ( uuid )
  {
    case REPORT_UUID:
      if ( type == HID_REPORT_TYPE_FEATURE )
      {
        *((uint8 *)pValue) = hidReportFeature;
        *pLen = 1;
      }
      else
      {
        *pLen = 0;
      }
      break;

    default:
      *pLen = 0;
      break;
  }

  return ( SUCCESS );
}


/*********************************************************************
*********************************************************************/
