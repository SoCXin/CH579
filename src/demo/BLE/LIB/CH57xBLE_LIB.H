/********************************** (C) COPYRIGHT ******************************
* File Name          : CH57xBLE_LIB.H
* Author             : WCH
* Version            : V1.10
* Date               : 2019/03/02
* Description        : 
*******************************************************************************/



/******************************************************************************/
#ifndef __CH57xBLE_LIB_H
#define __CH57xBLE_LIB_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef int8
typedef char                            int8;
#endif
#ifndef int16
typedef short                           int16;
#endif
#ifndef BOOL
typedef unsigned char          	        BOOL;
#endif
#ifndef s8
typedef signed char           	        s8;
#endif
#ifndef s16
typedef signed short                    s16;
#endif
#ifndef s32
typedef signed long                     s32;
#endif
#ifndef u8
typedef unsigned char                   u8;
#endif
#ifndef u16
typedef unsigned short                  u16;
#endif
#ifndef u32
typedef unsigned long                   u32;
#endif
#ifndef u64
typedef unsigned long long              u64;
#endif
#ifndef u8C
typedef const unsigned char             u8C;
#endif
#ifndef u32V
typedef unsigned long volatile          u32V;
#endif
#ifndef uint8
typedef unsigned char                   uint8;
#endif
#ifndef uint16
typedef unsigned short                  uint16;
#endif
#ifndef uint32
typedef unsigned long                   uint32;
#endif
#ifndef TRUE
  #define TRUE                          1
#endif
#ifndef FALSE
  #define FALSE                  	      0
#endif
#ifndef NULL
  #define NULL                    	    0
#endif
#ifndef VOID
  #define VOID                          (void)
#endif
#ifndef ENABLE
  #define ENABLE                        (1)
  #define DISABLE                       (0)
#endif
#ifndef	CONST
  #define CONST     	                  const
#endif
#ifndef	bStatus_t
typedef unsigned char                   bStatus_t;
#endif
#ifndef	tmosTaskID
typedef unsigned char          	        tmosTaskID;
#endif
#ifndef	tmosEvents
typedef unsigned short         	        tmosEvents;
#endif
#ifndef	tmosTimer
typedef unsigned long         	        tmosTimer;
#endif
// 定义随机值种子函数类型
typedef u32  ( *pfnSrandCB)( void );
// 定义睡眠唤醒函数类型
typedef u32  ( *pfnSleepCB)( u32 );
// 定义内部32K时钟校准函数类型
typedef void ( *pfnHSECalibrationCB)( void );
// 定义任务处理函数类型
typedef tmosEvents(*pTaskEventHandlerFn)( tmosTaskID taskID, tmosEvents event );
  
/*********************************************************************
 * GLOBAL MACROS
 */
// defined for all task
#define SYS_EVENT_MSG                   (0x8000)  // A message is waiting event
#define INVALID_TASK_ID                 0xFF      // Task ID isn't setup properly
#define TASK_NO_TASK                    0xFF

/* ------------------------------------------------------------------------------------------------
 *                                             Macros
 * ------------------------------------------------------------------------------------------------
 */
#define ACTIVE_LOW        !
#define ACTIVE_HIGH       !!                      // double negation forces result to be '1'

#ifndef BV
#define BV(n)      (1 << (n))
#endif

#ifndef BF
#define BF(x,b,s)  (((x) & (b)) >> (s))
#endif

#ifndef MIN
#define MIN(n,m)   (((n) < (m)) ? (n) : (m))
#endif

#ifndef MAX
#define MAX(n,m)   (((n) < (m)) ? (m) : (n))
#endif

#ifndef ABS
#define ABS(n)     (((n) < 0) ? -(n) : (n))
#endif

#ifndef st
#define st(x)      do { x } while (__LINE__ == -1)
#endif

/* 发送功率定义 TxPower define */
#define LL_TX_POWEER_MINUS_20_DBM   0x01
#define LL_TX_POWEER_MINUS_14_DBM   0x03
#define LL_TX_POWEER_MINUS_8_DBM    0x07
#define LL_TX_POWEER_MINUS_3_DBM    0x0F
#define LL_TX_POWEER_0_DBM          0x15
#define LL_TX_POWEER_1_DBM          0x1D
#define LL_TX_POWEER_2_DBM          0x23
#define LL_TX_POWEER_3_DBM          0x3F

/* 协议栈配置结构体 */
typedef struct  tag_ble_config
{
    u32 MEMAddr;                       // 蓝牙协议栈缓存首地址（默认值：无，必须配置）
    u32 MEMLen;                        // 蓝牙协议栈缓存的长度（默认值：无，必须配置）
    u32 SNVAddr;                       // 存储配置信息首地址（默认值：无，不配置表示不保存配对信息）
    u32 SNVBlock;                      // 存储配置信息块大小（默认值：512）
    u32 SNVNum;                        // 存储配置信息块数（默认值：1）
    u32 AttMtu;                        // 配置ATT_MTU（默认值：27）
    u32 RxNumMax;                      // 缓存数据包个数(默认值：10)
    u32 TxNumEvent;                    // 一个事件发送最大包个数（默认值：1）
    u32 TxPower;                       // 发送功率（默认值：0x15（0DBm））
    u32 WakeUpTime;                    // 配置唤醒后需要的时间，以RTC一个计数为单位（默认值：70）
    u32 OscRC32KEnable;                // 使能内部RC作为32K时钟源 （默认：0，外部32K晶振；1，内部RC）
    u32 WindowWidening;                // 窗口扩展（默认：8，一般不需要配置）
    u32 BLEIrqOff;                     // 关闭蓝牙中断（默认：0，开启中断；1，关闭中断）
    u8  MacAddr[6];                    // MAC地址（小端）（默认值：出厂的MAC地址，如配置则优先使用配置值）
} bleConfig_t;
extern bleConfig_t  ble;

typedef struct
{
    tmosTaskID resv;                   // 未使用，只是为了对齐
    tmosTaskID taskID;                 // 任务ID号
    tmosEvents events;                 // 事件值
    tmosTimer  timeload;               // 保存重加载事件的时间值
    tmosTimer  timeout;                // 事件触发的相对时间值
    void * next;             
}tmosTaskList_t;

typedef struct
{
  u8  event;
  u8  status;
}tmos_event_hdr_t;

extern const u8 VER_LIB[]; // 库版本信息
extern pTaskEventHandlerFn tasksArr[];                             // 定义任务处理存放位置
extern tmosTaskID  TASK_CNT;                                       // 定义任务处理数量
#define SYSTEM_TIME_MICROSEN     625                               // 任务处理的单位时间625us
#define MS1_TO_SYSTEM_TIME(x)    ((x)*1000/SYSTEM_TIME_MICROSEN)   // 将1ms转换为系统时间计数值（注意误差）
/* takes a byte out of a uint32 : var - uint32,  ByteNum - byte to take out (0 - 3) */
#define BREAK_UINT32( var, ByteNum ) (uint8)((uint32)(((var) >>((ByteNum) * 8)) & 0x00FF))
#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)
#define HI_UINT8(a) (((a) >> 4) & 0x0F)
#define LO_UINT8(a) ((a) & 0x0F)

#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3) \
          ((uint32)(((uint32)(Byte0) & 0x00FF) \
          + (((uint32)(Byte1) & 0x00FF) << 8) \
          + (((uint32)(Byte2) & 0x00FF) << 16) \
          + (((uint32)(Byte3) & 0x00FF) << 24)))
#define BUILD_UINT16(loByte, hiByte) ((uint16)(((loByte) & 0x00FF)|(((hiByte) & 0x00FF)<<8)))
#define BUILD_UINT8(hiByte, loByte) ((uint8)(((loByte) & 0x0F) + (((hiByte) & 0x0F) << 4)))
  
/* ---------------------------------------------------------------------------
 *                                             lib FUNCTION
 * ----------------------------------------------------------------------------
 */
/*****************************************************************************/
extern u32   tmos_rand( void  ); // 获取一个随机数
extern void  tmos_memset( void * pDst, u8 Value, u32 len );
extern void  tmos_memcpy( void *dst, const void *src, u32 len );
extern u8    tmos_memcmp( const void *src1, const void *src2, u32 len );
extern void *tmos_memdup( const void *src, u32 len );
extern void *tmos_revmemcpy( void *dst, const void *src, u32 len );
extern u32   tmos_strlen( char *pString );
extern u8    tmos_isbufset( u8 *buf, u8 val, u32 len );

/*******************************************************************************
 * @fn          tmos_set_event
 *
 * @brief       立即启动一个事件
 *
 * input parameters
 *
 * @param       taskID - 要设置的事件ID
 * @param       event - 事件值
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      status.
 */
extern bStatus_t tmos_set_event( tmosTaskID taskID, tmosEvents event );

/*******************************************************************************
 * @fn          tmos_clear_event
 *
 * @brief       清除一个到期的事件
 *
 * input parameters
 *
 * @param       taskID - 要清除的事件ID
 * @param       event - 事件值
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      status.
 */
extern bStatus_t tmos_clear_event( tmosTaskID taskID, tmosEvents event );

/*******************************************************************************
 * @fn          tmos_start_task
 *
 * @brief       启动一个事件
 *
 * input parameters
 *
 * @param       taskID - 要启动的事件ID
 * @param       event - 事件值
 * @param       time - 事件到期时间
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
extern tmosTaskList_t * tmos_start_task( tmosTaskID taskID, tmosEvents event, tmosTimer time  );

/*******************************************************************************
 * @fn          tmos_stop_task
 *
 * @brief       停止指定事件
 *
 * input parameters
 *
 * @param       taskID - 要停止的事件ID
 * @param       event - 事件值
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
extern bStatus_t tmos_stop_task( tmosTaskID taskID, tmosEvents event );

/*******************************************************************************
 * @fn          tmos_get_task_timer
 *
 * @brief       获取当前事件时间
 *
 * input parameters
 *
 * @param       taskID -要获取时间的事件ID
 * @param       event - 事件值
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      the timer's tick count if found, zero otherwise.
 */
extern tmosTimer tmos_get_task_timer( tmosTaskID taskID, tmosEvents event );

/*******************************************************************************
 * @fn          tmos_msg_send
 *
 * @brief       发送消息
 *
 * input parameters
 *
 * @param       taskID - 需要发送消息的ID
 * @param       *msg_ptr - 指向消息的指针
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS, INVALID_TASK, INVALID_MSG_POINTER.
 */
extern u8    tmos_msg_send( tmosTaskID taskID, u8 *msg_ptr );

/*******************************************************************************
 * @fn          tmos_msg_deallocate
 *
 * @brief       删除消息
 *
 * input parameters
 *
 * @param       *msg_ptr - 指向消息的指针
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS, INVALID_MSG_POINTER
 */
extern u8    tmos_msg_deallocate( u8 *msg_ptr );

/*******************************************************************************
 * @fn          tmos_msg_receive
 *
 * @brief       接收消息
 *
 * input parameters
 *
 * @param       taskID  - 需要接收消息的ID
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      *u8 - message information or NULL if no message
 */
extern u8   *tmos_msg_receive( tmosTaskID taskID );

/*******************************************************************************
 * @fn          tmos_msg_allocate
 *
 * @brief       用于发送消息时向系统申请一段缓存
 *
 * input parameters
 *
 * @param       len  - 需要发送消息的长度
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      pointer to allocated buffer or NULL if allocation failed.
 */
extern u8   *tmos_msg_allocate( u16 len );

/*******************************************************************************
 * @fn          TMOS_Rc32KRegister
 *
 * @brief       注册一个内部32K时钟校准函数,并使用内部32K时钟
 *
 * input parameters
 *
 * @param       rcCB - 内部32k时钟校准子程序
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
extern void TMOS_Rc32KRegister( pfnHSECalibrationCB rcCB );

/*******************************************************************************
 * @fn          TMOS_RandRegister
 *
 * @brief       注册一个产生随机值种子的函数.
 *
 * input parameters
 *
 * @param       srandCB - 随机值种子产生子程序
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
 extern void TMOS_RandRegister( pfnSrandCB srandCB );
 
/*******************************************************************************
 * @fn          TMOS_RandRegister
 *
 * @brief       注册一个睡眠唤醒的函数.
 *
 * input parameters
 *
 * @param       sleepCB - 睡眠唤醒子程序
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
extern void TMOS_SleepRegister( pfnSleepCB sleepCB );

/*******************************************************************************
 * @fn          TMOS_SystemProcess
 *
 * @brief       系统处理函数
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
extern void TMOS_SystemProcess( void );

/*******************************************************************************
 * @fn          TMOS_GetSystemClock
 *
 * @brief      获取当前系统时间
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      当前系统时间，625us为单位
 */
extern u32 TMOS_GetSystemClock( void );

/*******************************************************************************
 * @fn          BLE_LibInit
 *
 * @brief       协议栈初始化。使用蓝牙协议栈库，RTC被占用，应用不可配置RTC时间
 *
 * input parameters
 *
 * @param       pCfg-配置协议栈相关的参数.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      0-success.
 */
extern u32  BLE_LibInit( bleConfig_t* pCfg );  

/*******************************************************************************
 * @fn          TMOS_Init
 *
 * @brief       系统初始化
 *
 * input parameters
 *
 * @param       id 
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
extern void TMOS_Init( tmosTaskID id );  

/*******************************************************************************
 * @fn          TMOS_CbTimerProcessEvent
 *
 * @brief       协议栈使用
 *
 * input parameters
 *
 * @param       taskId - 系统层的事件ID值
 * @param       events - 已产生的所有事件
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      result.
 *
 */
extern tmosEvents TMOS_CbTimerProcessEvent( tmosTaskID taskId, tmosEvents events );

/*******************************************************************************
 * @fn          LL_Init
 *
 * @brief       控制器初始化
 *
 * input parameters
 *
 * @param       id - 分配给LL层的事件ID值
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
extern void LL_Init( tmosTaskID id );

/*******************************************************************************
 * @fn          LL_ProcessEvent
 *
 * @brief       控制器任务处理
 *
 * input parameters
 *
 * @param       taskId - LL层的事件ID值
 * @param       events - 已产生的所有事件
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      result.
 *
 */
extern tmosEvents LL_ProcessEvent( tmosTaskID taskID, tmosEvents events );

  
#ifdef __cplusplus
}
#endif

#endif

/******************************************************************************/
#ifndef __HOST_H
#define __HOST_H

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************
 * MACROS
 */
//! Default Public and Random Address Length
#define B_ADDR_LEN                               6
//! BLE Random Number Size
#define B_RANDOM_NUM_SIZE               8
//! Default key length
#define KEYLEN                                       16
//! BLE Maximum Advertising Packet Length
#define B_MAX_ADV_LEN                         31

#ifndef	 SUCCESS	    
#define  SUCCESS	                                0x00	  
#endif
#define  FAILURE                                     0x01
#define INVALIDPARAMETER                   0x02
#define INVALID_TASK                             0x03
#define MSG_BUFFER_NOT_AVAIL            0x04
#define INVALID_MSG_POINTER              0x05
#define INVALID_EVENT_ID                     0x06
#define INVALID_INTERRUPT_ID              0x07
#define NO_TIMER_AVAIL                       0x08
#define NV_ITEM_UNINIT                       0x09
#define NV_OPER_FAILED                       0x0A
#define INVALID_MEM_SIZE                   0x0B
#define NV_BAD_ITEM_LEN                    0x0C

/** BLE_STATUS_VALUES BLE Default BLE Status Values
 * returned as bStatus_t
 */
#define bleInvalidTaskID                INVALID_TASK  //!< Task ID isn't setup properly
#define bleNotReady                     0x10  //!< Not ready to perform task
#define bleAlreadyInRequestedMode       0x11  //!< Already performing that task
#define bleIncorrectMode                0x12  //!< Not setup properly to perform that task
#define bleMemAllocError                0x13  //!< Memory allocation error occurred
#define bleNotConnected                 0x14  //!< Can't perform function when not in a connection
#define bleNoResources                  0x15  //!< There are no resource available
#define blePending                      0x16  //!< Waiting
#define bleTimeout                      0x17  //!< Timed out performing function
#define bleInvalidRange                 0x18  //!< A parameter is out of range
#define bleLinkEncrypted                0x19  //!< The link is already encrypted
#define bleProcedureComplete            0x1A  //!< The Procedure is completed
#define bleInvalidMtuSize	            	0x1B

/*******************************LinkDB***************************************/
// Special case connection handles
#define INVALID_CONNHANDLE              0xFFFF  // Invalid connection handle, used for no connection handle
#define LOOPBACK_CONNHANDLE             0xFFFE  // Loopback connection handle, used to loopback a message
// Link state flags
#define LINK_NOT_CONNECTED              0x00    // Link isn't connected
#define LINK_CONNECTED                  0x01    // Link is connected
#define LINK_AUTHENTICATED              0x02    // Link is authenticated
#define LINK_BOUND                      0x04    // Link is bonded
#define LINK_ENCRYPTED                  0x10    // Link is encrypted
// Link Database Status callback changeTypes
#define LINKDB_STATUS_UPDATE_NEW        0       // New connection created
#define LINKDB_STATUS_UPDATE_REMOVED    1       // Connection was removed
#define LINKDB_STATUS_UPDATE_STATEFLAGS 2       // Connection state flag changed
// Link Authentication Errors
#define LINKDB_ERR_INSUFFICIENT_AUTHEN      0x05  // Link isn't even encrypted
#define LINBDB_ERR_INSUFFICIENT_KEYSIZE     0x0c  // Link is encrypted but the key size is too small
#define LINKDB_ERR_INSUFFICIENT_ENCRYPTION  0x0f  // Link is encrypted but it's not authenticated

#define LINKDB_CONNECT_NUM            3
/*******************************gattUUID***************************************/
/**
 * GATT Services
 */
#define GAP_SERVICE_UUID                           0x1800 // Generic Access Profile
#define GATT_SERVICE_UUID                          0x1801 // Generic Attribute Profile
/**
 * GATT Declarations
 */
#define GATT_PRIMARY_SERVICE_UUID                  0x2800 // Primary Service
#define GATT_SECONDARY_SERVICE_UUID                0x2801 // Secondary Service
#define GATT_INCLUDE_UUID                          0x2802 // Include
#define GATT_CHARACTER_UUID                        0x2803 // Characteristic
/**
 * GATT Descriptors
 */
#define GATT_CHAR_EXT_PROPS_UUID                   0x2900 // Characteristic Extended Properties
#define GATT_CHAR_USER_DESC_UUID                   0x2901 // Characteristic User Description
#define GATT_CLIENT_CHAR_CFG_UUID                  0x2902 // Client Characteristic Configuration
#define GATT_SERV_CHAR_CFG_UUID                    0x2903 // Server Characteristic Configuration
#define GATT_CHAR_FORMAT_UUID                      0x2904 // Characteristic Presentation Format
#define GATT_CHAR_AGG_FORMAT_UUID                  0x2905 // Characteristic Aggregate Format
#define GATT_VALID_RANGE_UUID                      0x2906 // Valid Range
#define GATT_EXT_REPORT_REF_UUID                   0x2907 // External Report Reference Descriptor
#define GATT_REPORT_REF_UUID                       0x2908 // Report Reference Descriptor
/**
 * GATT Characteristics
 */
#define DEVICE_NAME_UUID                           0x2A00 // Device Name
#define APPEARANCE_UUID                            0x2A01 // Appearance
#define PERI_PRIVACY_FLAG_UUID                     0x2A02 // Peripheral Privacy Flag
#define RECONNECT_ADDR_UUID                        0x2A03 // Reconnection Address
#define PERI_CONN_PARAM_UUID                       0x2A04 // Peripheral Preferred Connection Parameters
#define SERVICE_CHANGED_UUID                       0x2A05 // Service Changed
#define CENTRAL_ADDRESS_RESOLUTION_UUID            0x2AA6 // Central Address Resolution
/**
 * GATT Service UUIDs
 */
#define IMMEDIATE_ALERT_SERV_UUID       0x1802  // Immediate Alert
#define LINK_LOSS_SERV_UUID             0x1803  // Link Loss
#define TX_PWR_LEVEL_SERV_UUID          0x1804  // Tx Power
#define CURRENT_TIME_SERV_UUID          0x1805  // Current Time Service
#define REF_TIME_UPDATE_SERV_UUID       0x1806  // Reference Time Update Service
#define NEXT_DST_CHANGE_SERV_UUID       0x1807  // Next DST Change Service
#define GLUCOSE_SERV_UUID               0x1808  // Glucose
#define THERMOMETER_SERV_UUID           0x1809  // Health Thermometer
#define DEVINFO_SERV_UUID               0x180A  // Device Information
#define NWA_SERV_UUID                   0x180B  // Network Availability
#define HEARTRATE_SERV_UUID             0x180D  // Heart Rate
#define PHONE_ALERT_STS_SERV_UUID       0x180E  // Phone Alert Status Service
#define BATT_SERV_UUID                  0x180F  // Battery Service
#define BLOODPRESSURE_SERV_UUID         0x1810  // Blood Pressure
#define ALERT_NOTIF_SERV_UUID           0x1811  // Alert Notification Service
#define HID_SERV_UUID                   0x1812  // Human Interface Device
#define SCAN_PARAM_SERV_UUID            0x1813  // Scan Parameters
#define RSC_SERV_UUID                   0x1814  // Running Speed and Cadence
#define CSC_SERV_UUID                   0x1816  // Cycling Speed and Cadence
#define CYCPWR_SERV_UUID                0x1818  // Cycling Power
#define LOC_NAV_SERV_UUID               0x1819  // Location and Navigation
/**
 * GATT Characteristic UUIDs
 */
#define ALERT_LEVEL_UUID                0x2A06  // Alert Level
#define TX_PWR_LEVEL_UUID               0x2A07  // Tx Power Level
#define DATE_TIME_UUID                  0x2A08  // Date Time
#define DAY_OF_WEEK_UUID                0x2A09  // Day of Week
#define DAY_DATE_TIME_UUID              0x2A0A  // Day Date Time
#define EXACT_TIME_256_UUID             0x2A0C  // Exact Time 256
#define DST_OFFSET_UUID                 0x2A0D  // DST Offset
#define TIME_ZONE_UUID                  0x2A0E  // Time Zone
#define LOCAL_TIME_INFO_UUID            0x2A0F  // Local Time Information
#define TIME_WITH_DST_UUID              0x2A11  // Time with DST
#define TIME_ACCURACY_UUID              0x2A12  // Time Accuracy
#define TIME_SOURCE_UUID                0x2A13  // Time Source
#define REF_TIME_INFO_UUID              0x2A14  // Reference Time Information
#define TIME_UPDATE_CTRL_PT_UUID        0x2A16  // Time Update Control Point
#define TIME_UPDATE_STATE_UUID          0x2A17  // Time Update State
#define GLUCOSE_MEAS_UUID               0x2A18  // Glucose Measurement
#define BATT_LEVEL_UUID                 0x2A19  // Battery Level
#define TEMP_MEAS_UUID                  0x2A1C  // Temperature Measurement
#define TEMP_TYPE_UUID                  0x2A1D  // Temperature Type
#define IMEDIATE_TEMP_UUID              0x2A1E  // Intermediate Temperature
#define MEAS_INTERVAL_UUID              0x2A21  // Measurement Interval
#define BOOT_KEY_INPUT_UUID             0x2A22  // Boot Keyboard Input Report
#define SYSTEM_ID_UUID                  0x2A23  // System ID
#define MODEL_NUMBER_UUID               0x2A24  // Model Number String
#define SERIAL_NUMBER_UUID              0x2A25  // Serial Number String
#define FIRMWARE_REV_UUID               0x2A26  // Firmware Revision String
#define HARDWARE_REV_UUID               0x2A27  // Hardware Revision String
#define SOFTWARE_REV_UUID               0x2A28  // Software Revision String
#define MANUFACTURER_NAME_UUID          0x2A29  // Manufacturer Name String
#define IEEE_11073_CERT_DATA_UUID       0x2A2A  // IEEE 11073-20601 Regulatory Certification Data List
#define CURRENT_TIME_UUID               0x2A2B  // Current Time
#define SCAN_REFRESH_UUID               0x2A31  // Scan Refresh
#define BOOT_KEY_OUTPUT_UUID            0x2A32  // Boot Keyboard Output Report
#define BOOT_MOUSE_INPUT_UUID           0x2A33  // Boot Mouse Input Report
#define GLUCOSE_CONTEXT_UUID            0x2A34  // Glucose Measurement Context
#define BLOODPRESSURE_MEAS_UUID         0x2A35  // Blood Pressure Measurement
#define IMEDIATE_CUFF_PRESSURE_UUID     0x2A36  // Intermediate Cuff Pressure
#define HEARTRATE_MEAS_UUID             0x2A37  // Heart Rate Measurement
#define BODY_SENSOR_LOC_UUID            0x2A38  // Body Sensor Location
#define HEARTRATE_CTRL_PT_UUID          0x2A39  // Heart Rate Control Point
#define NETWORK_AVAIL_UUID              0x2A3E  // Network Availability
#define ALERT_STATUS_UUID               0x2A3F  // Alert Status
#define RINGER_CTRL_PT_UUID             0x2A40  // Ringer Control Point
#define RINGER_SETTING_UUID             0x2A41  // Ringer Setting
#define ALERT_CAT_ID_BMASK_UUID         0x2A42  // Alert Category ID Bit Mask
#define ALERT_CAT_ID_UUID               0x2A43  // Alert Category ID
#define ALERT_NOTIF_CTRL_PT_UUID        0x2A44  // Alert Notification Control Point
#define UNREAD_ALERT_STATUS_UUID        0x2A45  // Unread Alert Status
#define NEW_ALERT_UUID                  0x2A46  // New Alert
#define SUP_NEW_ALERT_CAT_UUID          0x2A47  // Supported New Alert Category
#define SUP_UNREAD_ALERT_CAT_UUID       0x2A48  // Supported Unread Alert Category
#define BLOODPRESSURE_FEATURE_UUID      0x2A49  // Blood Pressure Feature
#define HID_INFORMATION_UUID            0x2A4A  // HID Information
#define REPORT_MAP_UUID                 0x2A4B  // Report Map
#define HID_CTRL_PT_UUID                0x2A4C  // HID Control Point
#define REPORT_UUID                     0x2A4D  // Report
#define PROTOCOL_MODE_UUID              0x2A4E  // Protocol Mode
#define SCAN_INTERVAL_WINDOW_UUID       0x2A4F  // Scan Interval Window
#define PNP_ID_UUID                     0x2A50  // PnP ID
#define GLUCOSE_FEATURE_UUID            0x2A51  // Glucose Feature
#define RECORD_CTRL_PT_UUID             0x2A52  // Record Access Control Point
#define RSC_MEAS_UUID                   0x2A53  // RSC Measurement
#define RSC_FEATURE_UUID                0x2A54  // RSC Feature
#define SC_CTRL_PT_UUID                 0x2A55  // SC Control Point
#define CSC_MEAS_UUID                   0x2A5B  // CSC Measurement
#define CSC_FEATURE_UUID                0x2A5C  // CSC Feature
#define SENSOR_LOC_UUID                 0x2A5D  // Sensor Location
#define CYCPWR_MEAS_UUID                0x2A63  // Cycling Power Measurement
#define CYCPWR_VECTOR_UUID              0x2A64  // Cycling Power Vector
#define CYCPWR_FEATURE_UUID             0x2A65  // Cycling Power Feature
#define CYCPWR_CTRL_PT_UUID             0x2A66  // Cycling Power Control Point
#define LOC_SPEED_UUID                  0x2A67  // Location and Speed
#define NAV_UUID                        0x2A68  // Navigation
#define POS_QUALITY_UUID                0x2A69  // Position Quality
#define LN_FEATURE_UUID                 0x2A6A  // LN Feature
#define LN_CTRL_PT_UUID                 0x2A6B  // LN Control Point  
/**
 * GATT Unit UUIDs
 */
#define GATT_UNITLESS_UUID                    0x2700  // <Symbol>, <Expressed in terms of SI base units>
#define GATT_UNIT_LENGTH_METER_UUID           0x2701  // m, m
#define GATT_UNIT_MASS_KGRAM_UUID             0x2702  // kg, kg
#define GATT_UNIT_TIME_SECOND_UUID            0x2703  // s, s
#define GATT_UNIT_ELECTRIC_CURRENT_A_UUID     0x2704  // A, A
#define GATT_UNIT_THERMODYN_TEMP_K_UUID       0x2705  // K, K
#define GATT_UNIT_AMOUNT_SUBSTANCE_M_UUID     0x2706  // mol, mol
#define GATT_UNIT_LUMINOUS_INTENSITY_C_UUID   0x2707  // cd, cd

#define GATT_UNIT_AREA_SQ_MTR_UUID            0x2710  // m^2, m^2
#define GATT_UNIT_VOLUME_CUBIC_MTR_UUID       0x2711  // m^3, m^3
#define GATT_UNIT_VELOCITY_MPS_UUID           0x2712  // m/s, m s^-1
#define GATT_UNIT_ACCELERATION_MPS_SQ_UUID    0x2713  // m/s^2, m s^-2
#define GATT_UNIT_WAVENUMBER_RM_UUID          0x2714  // ? m^-1
#define GATT_UNIT_DENSITY_KGPCM_UUID          0x2715  // p, kg m^-3
#define GATT_UNIT_SURFACE_DENSITY_KGPSM_UUID  0x2716  // pA, kg m^-2
#define GATT_UNIT_SPECIFIC_VOLUME_CMPKG_UUID  0x2717  // v, m^3 kg^-1
#define GATT_UNIT_CURRENT_DENSITY_APSM_UUID   0x2718  // j, A m^-2
#define GATT_UNIT_MAG_FIELD_STRENGTH_UUID     0x2719  // H, A m
#define GATT_UNIT_AMOUNT_CONC_MPCM_UUID       0x271A  // c, mol m^-3
#define GATT_UNIT_MASS_CONC_KGPCM_UUID        0x271B  // c, kg m^-3
#define GATT_UNIT_LUMINANCE_CPSM_UUID         0x271C  // Lv, cd m^-2
#define GATT_UNIT_REFRACTIVE_INDEX_UUID       0x271D  // n, 1
#define GATT_UNIT_RELATIVE_PERMEABLILTY_UUID  0x271E  // u, 1
#define GATT_UNIT_PLANE_ANGLE_RAD_UUID        0x2720  // rad, m m-1
#define GATT_UNIT_SOLID_ANGLE_STERAD_UUID     0x2721  // sr, m2 m-2
#define GATT_UNIT_FREQUENCY_HTZ_UUID          0x2722  // Hz, s-1
#define GATT_UNIT_FORCE_NEWTON_UUID           0x2723  // N, m kg s-2
#define GATT_UNIT_PRESSURE_PASCAL_UUID        0x2724  // Pa, N/m2 = m2 kg s-2
#define GATT_UNIT_ENERGY_JOULE_UUID           0x2725  // J, N m = m2 kg s-2
#define GATT_UNIT_POWER_WATT_UUID             0x2726  // W, J/s = m2 kg s-3
#define GATT_UNIT_E_CHARGE_C_UUID             0x2727  // C, sA
#define GATT_UNIT_E_POTENTIAL_DIF_V_UUID      0x2728  // V, W/A = m2 kg s-3 A-1

#define GATT_UNIT_CELSIUS_TEMP_DC_UUID        0x272F  // oC, t/oC = T/K - 273.15

#define GATT_UNIT_TIME_MINUTE_UUID            0x2760  // min, 60 s
#define GATT_UNIT_TIME_HOUR_UUID              0x2761  // h, 3600 s
#define GATT_UNIT_TIME_DAY_UUID               0x2762  // d, 86400 s
#define GATT_UNIT_PLANE_ANGLE_DEGREE_UUID     0x2763  // o, (pi/180) rad
#define GATT_UNIT_PLANE_ANGLE_MINUTE_UUID     0x2764  // ', (pi/10800) rad
#define GATT_UNIT_PLANE_ANGLE_SECOND_UUID     0x2765  // '', (pi/648000) rad
#define GATT_UNIT_AREA_HECTARE_UUID           0x2766  // ha, 10^4 m^2
#define GATT_UNIT_VOLUME_LITRE_UUID           0x2767  // l, 10^-3 m^3
#define GATT_UNIT_MASS_TONNE_UUID             0x2768  // t, 10^3 kg

#define GATT_UINT_LENGTH_YARD_UUID            0x27A0  // yd, 0.9144 m
#define GATT_UNIT_LENGTH_PARSEC_UUID          0x27A1  // pc, 3.085678 ?1016 m
#define GATT_UNIT_LENGTH_INCH_UUID            0x27A2  // in, 0.0254 m
#define GATT_UNIT_LENGTH_FOOT_UUID            0x27A3  // ft, 0.3048 m
#define GATT_UNIT_LENGTH_MILE_UUID            0x27A4  // mi, 1609.347 m
#define GATT_UNIT_PRESSURE_PFPSI_UUID         0x27A5  // psi, 6.894757 ?103 Pa
#define GATT_UNIT_VELOCITY_KMPH_UUID          0x27A6  // km/h, 0.2777778 m^s-1
#define GATT_UNIT_VELOCITY_MPH_UUID           0x27A7  // mi/h, 0.44704 m^ s-1
#define GATT_UNIT_ANGULAR_VELOCITY_RPM_UUID   0x27A8  // r/min, 0.1047198 rad s-1
#define GATT_UNIT_ENERGY_GCAL_UUID            0x27A9  //
#define GATT_UNIT_ENERGY_KCAL_UUID            0x27AA  // kcal, 4190.02 J
#define GATT_UNIT_ENERGY_KWH_UUID             0x27AB  // kWh, 3600000 J
#define GATT_UNIT_THERMODYN_TEMP_DF_UUID      0x27AC  // oF, t/oF = T/K ?1.8 - 459.67
#define GATT_UNIT_PERCENTAGE_UUID             0x27AD  // %
#define GATT_UNIT_PER_MILE_UUID               0x27AE  //
#define GATT_UNIT_PERIOD_BPM_UUID             0x27AF  //
#define GATT_UNIT_E_CHARGE_AH_UUID            0x27B0  //
#define GATT_UNIT_MASS_DENSITY_MGPD_UUID      0x27B1  //
#define GATT_UNIT_MASS_DENSITY_MMPL_UUID      0x27B2  //
#define GATT_UNIT_TIME_YEAR_UUID              0x27B3  //
#define GATT_UNIT_TIME_MONTH_UUID             0x27B4  //

/************************************************ Messages IDs *************************************************************/
// L2CAP - Messages IDs (0xA0 - 0xAF)
#define L2CAP_DATA_EVENT                 0xA0 //!< Incoming data on a channel
#define L2CAP_SIGNAL_EVENT               0xA2 //!< Incoming Signaling message
// GATT - Messages IDs (0xB0 - 0xBF)
#define GATT_MSG_EVENT                   0xB0 //!< Incoming GATT message
#define GATT_SERV_MSG_EVENT              0xB1 //!< Incoming GATT Serv App message
// GAP - Messages IDs (0xD0 - 0xDF)
#define GAP_MSG_EVENT                    0xD0 //!< Incoming GAP message
/************************************************ l2cap *********************************************/  
#define L2CAP_MTU_SIZE                   23 //!< Minimum supported information payload for the Basic information frame (B-frame)
/************************************************ ATT *************************************************************/
#define ATT_MTU_SIZE                     L2CAP_MTU_SIZE //!< Minimum ATT MTU size
#define ATT_MAX_MTU_SIZE                 517            //!< Maximum ATT MTU size
// ATT Methods 
#define ATT_ERROR_RSP                    0x01 //!< ATT Error Response
#define ATT_EXCHANGE_MTU_REQ             0x02 //!< ATT Exchange MTU Request
#define ATT_EXCHANGE_MTU_RSP             0x03 //!< ATT Exchange MTU Response
#define ATT_FIND_INFO_REQ                0x04 //!< ATT Find Information Request
#define ATT_FIND_INFO_RSP                0x05 //!< ATT Find Information Response
#define ATT_FIND_BY_TYPE_VALUE_REQ       0x06 //!< ATT Find By Type Value Request
#define ATT_FIND_BY_TYPE_VALUE_RSP       0x07 //!< ATT Find By Type Value Response
#define ATT_READ_BY_TYPE_REQ             0x08 //!< ATT Read By Type Request
#define ATT_READ_BY_TYPE_RSP             0x09 //!< ATT Read By Type Response
#define ATT_READ_REQ                     0x0a //!< ATT Read Request
#define ATT_READ_RSP                     0x0b //!< ATT Read Response
#define ATT_READ_BLOB_REQ                0x0c //!< ATT Read Blob Request
#define ATT_READ_BLOB_RSP                0x0d //!< ATT Read Blob Response
#define ATT_READ_MULTI_REQ               0x0e //!< ATT Read Multiple Request
#define ATT_READ_MULTI_RSP               0x0f //!< ATT Read Multiple Response
#define ATT_READ_BY_GRP_TYPE_REQ         0x10 //!< ATT Read By Group Type Request
#define ATT_READ_BY_GRP_TYPE_RSP         0x11 //!< ATT Read By Group Type Response
#define ATT_WRITE_REQ                    0x12 //!< ATT Write Request
#define ATT_WRITE_RSP                    0x13 //!< ATT Write Response
#define ATT_PREPARE_WRITE_REQ            0x16 //!< ATT Prepare Write Request
#define ATT_PREPARE_WRITE_RSP            0x17 //!< ATT Prepare Write Response
#define ATT_EXECUTE_WRITE_REQ            0x18 //!< ATT Execute Write Request
#define ATT_EXECUTE_WRITE_RSP            0x19 //!< ATT Execute Write Response
#define ATT_HANDLE_VALUE_NOTI            0x1b //!< ATT Handle Value Notification
#define ATT_HANDLE_VALUE_IND             0x1d //!< ATT Handle Value Indication
#define ATT_HANDLE_VALUE_CFM             0x1e //!< ATT Handle Value Confirmation

#define ATT_WRITE_CMD                    0x52 //!< ATT Write Command
#define ATT_SIGNED_WRITE_CMD             0xD2 //!< ATT Signed Write Command

//  ATT Error Codes
#define ATT_ERR_INVALID_HANDLE           0x01 //!< Attribute handle value given was not valid on this attribute server
#define ATT_ERR_READ_NOT_PERMITTED       0x02 //!< Attribute cannot be read
#define ATT_ERR_WRITE_NOT_PERMITTED      0x03 //!< Attribute cannot be written
#define ATT_ERR_INVALID_PDU              0x04 //!< The attribute PDU was invalid
#define ATT_ERR_INSUFFICIENT_AUTHEN      0x05 //!< The attribute requires authentication before it can be read or written
#define ATT_ERR_UNSUPPORTED_REQ          0x06 //!< Attribute server doesn't support the request received from the attribute client
#define ATT_ERR_INVALID_OFFSET           0x07 //!< Offset specified was past the end of the attribute
#define ATT_ERR_INSUFFICIENT_AUTHOR      0x08 //!< The attribute requires an authorization before it can be read or written
#define ATT_ERR_PREPARE_QUEUE_FULL       0x09 //!< Too many prepare writes have been queued
#define ATT_ERR_ATTR_NOT_FOUND           0x0a //!< No attribute found within the given attribute handle range
#define ATT_ERR_ATTR_NOT_LONG            0x0b //!< Attribute cannot be read or written using the Read Blob Request or Prepare Write Request
#define ATT_ERR_INSUFFICIENT_KEY_SIZE    0x0c //!< The Encryption Key Size used for encrypting this link is insufficient
#define ATT_ERR_INVALID_VALUE_SIZE       0x0d //!< The attribute value length is invalid for the operation
#define ATT_ERR_UNLIKELY                 0x0e //!< The attribute request that was requested has encountered an error that was very unlikely, and therefore could not be completed as requested
#define ATT_ERR_INSUFFICIENT_ENCRYPT     0x0f //!< The attribute requires encryption before it can be read or written
#define ATT_ERR_UNSUPPORTED_GRP_TYPE     0x10 //!< The attribute type is not a supported grouping attribute as defined by a higher layer specification
#define ATT_ERR_INSUFFICIENT_RESOURCES   0x11 //!< Insufficient Resources to complete the request
#define ATT_ERR_INVALID_VALUE            0x80 //!< The attribute value is invalid for the operation

/*********************************************************************
 * ATT Find By Type Value Response macros
 */
// Attribute Handle and Group End Handle pair indexes
#define ATT_ATTR_HANDLE_IDX( i )           ( (i) * (2 + 2) )
#define ATT_GRP_END_HANDLE_IDX( i )        ( ATT_ATTR_HANDLE_IDX( (i) ) + 2 )

#define ATT_ATTR_HANDLE( info, i )         ( BUILD_UINT16( (info)[ATT_ATTR_HANDLE_IDX((i))], \
                                                           (info)[ATT_ATTR_HANDLE_IDX((i))+1] ) )
#define ATT_GRP_END_HANDLE( info, i )      ( BUILD_UINT16( (info)[ATT_GRP_END_HANDLE_IDX((i))], \
                                                           (info)[ATT_GRP_END_HANDLE_IDX((i))+1] ) )

/*** Opcode fields: bitmasks ***/
// Method (bits 5-0)
#define ATT_METHOD_BITS                  0x3f
// Command Flag (bit 6)
#define ATT_CMD_FLAG_BIT                 0x40
// Authentication Signature Flag (bit 7)
#define ATT_AUTHEN_SIG_FLAG_BIT          0x80
// Size of 16-bit Bluetooth UUID
#define ATT_BT_UUID_SIZE                 2
// Size of 128-bit UUID
#define ATT_UUID_SIZE                    16
/************************************************ GATT *************************************************************/

#define GATT_CONNECT_NUM            (LINKDB_CONNECT_NUM)
#define GATT_MAX_NUM_CONN                ( GATT_CONNECT_NUM + 1  )

// GATT Attribute Access Permissions Bit Fields
#define GATT_PERMIT_READ                 0x01 //!< Attribute is Readable
#define GATT_PERMIT_WRITE                0x02 //!< Attribute is Writable
#define GATT_PERMIT_AUTHEN_READ          0x04 //!< Read requires Authentication
#define GATT_PERMIT_AUTHEN_WRITE         0x08 //!< Write requires Authentication
#define GATT_PERMIT_AUTHOR_READ          0x10 //!< Read requires Authorization
#define GATT_PERMIT_AUTHOR_WRITE         0x20 //!< Write requires Authorization
#define GATT_PERMIT_ENCRYPT_READ         0x40 //!< Read requires Encryption
#define GATT_PERMIT_ENCRYPT_WRITE        0x80 //!< Write requires Encryption

// GATT Characteristic Properties Bit Fields
#define GATT_PROP_BCAST                  0x01 //!< Permits broadcasts of the Characteristic Value
#define GATT_PROP_READ                   0x02 //!< Permits reads of the Characteristic Value
#define GATT_PROP_WRITE_NO_RSP           0x04 //!< Permits writes of the Characteristic Value without response
#define GATT_PROP_WRITE                  0x08 //!< Permits writes of the Characteristic Value with response
#define GATT_PROP_NOTIFY                 0x10 //!< Permits notifications of a Characteristic Value without acknowledgement
#define GATT_PROP_INDICATE               0x20 //!< Permits indications of a Characteristic Value with acknowledgement
#define GATT_PROP_AUTHEN                 0x40 //!< Permits signed writes to the Characteristic Value
#define GATT_PROP_EXTENDED               0x80 //!< Additional characteristic properties are defined in the Characteristic Extended Properties Descriptor

// GATT local read or write operation
#define GATT_LOCAL_READ                  0xFF
#define GATT_LOCAL_WRITE                 0xFE

// GATT Encryption Key Size Limits
#define GATT_MIN_ENCRYPT_KEY_SIZE          7  //!< GATT Minimum Encryption Key Size
#define GATT_MAX_ENCRYPT_KEY_SIZE          16 //!< GATT Maximum Encryption Key Size

// GATT Base Method
#define GATT_BASE_METHOD                 0x40

// Attribute handle definitions
#define GATT_INVALID_HANDLE              0x0000 // Invalid attribute handle
#define GATT_MIN_HANDLE                  0x0001 // Minimum attribute handle
#define GATT_MAX_HANDLE                  0xFFFF // Maximum attribute handle

#define GATT_MAX_MTU                     0xFFFF // Maximum MTU size

// Attribute Access Permissions
#define gattPermitRead( a )              ( (a) & GATT_PERMIT_READ )
#define gattPermitWrite( a )             ( (a) & GATT_PERMIT_WRITE )
#define gattPermitAuthenRead( a )        ( (a) & GATT_PERMIT_AUTHEN_READ )
#define gattPermitAuthenWrite( a )       ( (a) & GATT_PERMIT_AUTHEN_WRITE )
#define gattPermitAuthorRead( a )        ( (a) & GATT_PERMIT_AUTHOR_READ )
#define gattPermitAuthorWrite( a )       ( (a) & GATT_PERMIT_AUTHOR_WRITE )
#define gattPermitEncryptRead( a )       ( (a) & GATT_PERMIT_ENCRYPT_READ )
#define gattPermitEncryptWrite( a )      ( (a) & GATT_PERMIT_ENCRYPT_WRITE )

// Check for different UUID types
#define gattPrimaryServiceType( t )      ( ATT_CompareUUID( primaryServiceUUID, ATT_BT_UUID_SIZE, \
                                                            (t).uuid, (t).len ) )
#define gattSecondaryServiceType( t )    ( ATT_CompareUUID( secondaryServiceUUID, ATT_BT_UUID_SIZE, \
                                                            (t).uuid, (t).len ) )
#define gattCharacterType( t )           ( ATT_CompareUUID( characterUUID, ATT_BT_UUID_SIZE, \
                                                            (t).uuid, (t).len ) )
#define gattIncludeType( t )             ( ATT_CompareUUID( includeUUID, ATT_BT_UUID_SIZE, \
                                                            (t).uuid, (t).len ) )
#define gattServiceType( t )             ( gattPrimaryServiceType( (t) ) || \
                                           gattSecondaryServiceType( (t) ) )

// GATT Client Characteristic Configuration Bit Fields
#define GATT_CLIENT_CFG_NOTIFY           0x0001 //!< The Characteristic Value shall be notified
#define GATT_CLIENT_CFG_INDICATE         0x0002 //!< The Characteristic Value shall be indicated

#define GATT_CFG_NO_OPERATION            0x0000 // No operation

// All profile services bit fields
#define GATT_ALL_SERVICES                0xFFFFFFFF

// The number of attribute records in a given attribute table
#define GATT_NUM_ATTRS( attrs )          ( sizeof( attrs ) / sizeof( gattAttribute_t ) )

// The handle of a service is the handle of the first attribute
#define GATT_SERVICE_HANDLE( attrs )     ( (attrs)[0].handle )

// The handle of the first included service (i = 1) is the value of the second attribute
#define GATT_INCLUDED_HANDLE( attrs, i ) ( *((uint16 *)((attrs)[(i)].pValue)) )
  
// Client Characteristic Configuration table (from CCC attribute value pointer)
#define GATT_CCC_TBL( pValue )           ( (gattCharCfg_t *)(*((PTR_TYPE)(&pValue))))

/*************************************************** SM **************************************/
// SM I/O Capabilities
#define DISPLAY_ONLY              0x00  //!< Display Only Device
#define DISPLAY_YES_NO            0x01  //!< Display and Yes and No Capable
#define KEYBOARD_ONLY             0x02  //!< Keyboard Only
#define NO_INPUT_NO_OUTPUT        0x03  //!< No Display or Input Device
#define KEYBOARD_DISPLAY          0x04  //!< Both Keyboard and Display Capable

#define SM_AUTH_MITM_MASK(a)    (((a) & 0x04) >> 2)

// SM Passkey Types (Bit Masks)
#define SM_PASSKEY_TYPE_INPUT   0x01    //!< Input the passkey
#define SM_PASSKEY_TYPE_DISPLAY 0x02    //!< Display the passkey

// SM AuthReq Bonding Flags
#define SM_AUTH_REQ_NO_BONDING    0x00  //!< No bonding
#define SM_AUTH_REQ_BONDING       0x01  //!< Bonding

#define PASSKEY_LEN     6   //! Passkey Character Length (ASCII Characters)

#define SM_AUTH_STATE_AUTHENTICATED       0x04  //! Authenticate requested
#define SM_AUTH_STATE_BONDING             0x01  //! Bonding requested

/************************************************ GAP  *************************************************************/
#define GAP_DEVICE_INIT_DONE_EVENT            0x00 //!< Sent when the Device Initialization is complete.  This event is sent as an tmos message defined as gapDeviceInitDoneEvent_t.
#define GAP_DEVICE_DISCOVERY_EVENT            0x01 //!< Sent when the Device Discovery Process is complete. This event is sent as an tmos message defined as gapDevDiscEvent_t.
#define GAP_ADV_DATA_UPDATE_DONE_EVENT        0x02 //!< Sent when the Advertising Data or SCAN_RSP Data has been updated. This event is sent as an tmos message defined as gapAdvDataUpdateEvent_t.
#define GAP_MAKE_DISCOVERABLE_DONE_EVENT      0x03 //!< Sent when the Make Discoverable Request is complete. This event is sent as an tmos message defined as gapMakeDiscoverableRspEvent_t.
#define GAP_END_DISCOVERABLE_DONE_EVENT       0x04 //!< Sent when the Advertising has ended. This event is sent as an tmos message defined as gapEndDiscoverableRspEvent_t.
#define GAP_LINK_ESTABLISHED_EVENT            0x05 //!< Sent when the Establish Link Request is complete. This event is sent as an tmos message defined as gapEstLinkReqEvent_t.
#define GAP_LINK_TERMINATED_EVENT             0x06 //!< Sent when a connection was terminated. This event is sent as an tmos message defined as gapTerminateLinkEvent_t.
#define GAP_LINK_PARAM_UPDATE_EVENT           0x07 //!< Sent when an Update Parameters Event is received. This event is sent as an tmos message defined as gapLinkUpdateEvent_t.
#define GAP_RANDOM_ADDR_CHANGED_EVENT         0x08 //!< Sent when a random address was changed. This event is sent as an tmos message defined as gapRandomAddrEvent_t.
#define GAP_SIGNATURE_UPDATED_EVENT           0x09 //!< Sent when the device's signature counter is updated. This event is sent as an tmos message defined as gapSignUpdateEvent_t.
#define GAP_AUTHENTICATION_COMPLETE_EVENT     0x0A //!< Sent when the Authentication (pairing) process is complete. This event is sent as an tmos message defined as gapAuthCompleteEvent_t.
#define GAP_PASSKEY_NEEDED_EVENT              0x0B //!< Sent when a Passkey is needed.  This is part of the pairing process. This event is sent as an tmos message defined as gapPasskeyNeededEvent_t.
#define GAP_SLAVE_REQUESTED_SECURITY_EVENT    0x0C //!< Sent when a Slave Security Request is received. This event is sent as an tmos message defined as gapSlaveSecurityReqEvent_t.
#define GAP_DEVICE_INFO_EVENT                 0x0D //!< Sent during the Device Discovery Process when a device is discovered. This event is sent as an tmos message defined as gapDeviceInfoEvent_t.
#define GAP_BOND_COMPLETE_EVENT               0x0E //!< Sent when the bonding(bound) process is complete. This event is sent as an tmos message defined as gapBondCompleteEvent_t.
#define GAP_PAIRING_REQ_EVENT                 0x0F //!< Sent when an unexpected Pairing Request is received. This event is sent as an tmos message defined as gapPairingReqEvent_t.

// GAP_PROFILE_ROLE_DEFINES GAP Profile Roles
#define GAP_PROFILE_BROADCASTER   0x01 //!< A device that sends advertising events only.
#define GAP_PROFILE_OBSERVER      0x02 //!< A device that receives advertising events only.
#define GAP_PROFILE_PERIPHERAL    0x04 //!< A device that accepts the establishment of an LE physical link using the connection establishment procedure
#define GAP_PROFILE_CENTRAL       0x08 //!< A device that supports the Central role initiates the establishment of a physical connection

// GAP Status Return Values - returned as bStatus_t
#define bleGAPUserCanceled              0x30  //!< The user canceled the task
#define bleGAPConnNotAcceptable         0x31  //!< The connection was not accepted
#define bleGAPBondRejected              0x32  //!< The bound information was rejected.

#define GAP_DEVICE_NAME_LEN                     21 // Excluding null-terminate char

/*-------------------------------------------------------------------
 * CONSTANTS
 */
/** @defgroup GAP_CONN_HANDLE_DEFINES GAP Special Connection Handles
 * Used by GAP_TerminateLinkReq()
 * @{
 */
#define GAP_CONNHANDLE_INIT     0xFFFE  //!< terminates a link create
#define GAP_CONNHANDLE_ALL      0xFFFF  //!< terminates all links for the matching task ID.
/** @} End GAP_CONN_HANDLE_DEFINES */

// Privacy Flag States
#define GAP_PRIVACY_DISABLED                    0x00
#define GAP_PRIVACY_ENABLED                     0x01

// GAP GATT Server Parameters used with GGS Get/Set Parameter and Application's Callback functions
#define GGS_DEVICE_NAME_ATT                     0   // RW  uint8[GAP_DEVICE_NAME_LEN]
#define GGS_APPEARANCE_ATT                      1   // RW  uint16
#define GGS_PERI_PRIVACY_FLAG_ATT               2   // RW  uint8
#define GGS_RECONNCT_ADDR_ATT                   3   // RW  uint8[B_ADDR_LEN]
#define GGS_PERI_CONN_PARAM_ATT                 4   // RW  sizeof(gapPeriConnectParams_t)
#define GGS_PERI_PRIVACY_FLAG_PROPS             5   // RW  uint8
#define GGS_W_PERMIT_DEVICE_NAME_ATT            6   // W   uint8
#define GGS_W_PERMIT_APPEARANCE_ATT             7   // W   uint8
#define GGS_W_PERMIT_PRIVACY_FLAG_ATT           8   // W   uint8

// GAP Services bit fields
#define GAP_SERVICE                             0x00000001

// GAP_PARAMETER_ID_DEFINES GAP Parameter IDs
// Timers
#define TGAP_GEN_DISC_ADV_MIN          0  //!< Minimum time to remain advertising, when in Discoverable mode (0.625 mSec).  Setting this parameter to 0 turns off the timeout (default).
#define TGAP_LIM_ADV_TIMEOUT           1  //!< Maximum time to remain advertising, when in Limited Discoverable mode. In seconds (default 180 seconds)
#define TGAP_GEN_DISC_SCAN             2  //!< Minimum time to perform scanning, when performing General Discovery proc (0.625 mSec)
#define TGAP_LIM_DISC_SCAN             3  //!< Minimum time to perform scanning, when performing Limited Discovery proc (0.625 mSec)
#define TGAP_CONN_EST_ADV_TIMEOUT      4  //!< Advertising timeout, when performing Connection Establishment proc (0.625 mSec)
#define TGAP_CONN_PARAM_TIMEOUT        5  //!< Link Layer connection parameter update notification timer, connection parameter update proc (0.625 mSec)
// Constants
#define TGAP_LIM_DISC_ADV_INT_MIN      6  //!< Minimum advertising interval, when in limited discoverable mode (n * 0.625 mSec)
#define TGAP_LIM_DISC_ADV_INT_MAX      7  //!< Maximum advertising interval, when in limited discoverable mode (n * 0.625 mSec)
#define TGAP_GEN_DISC_ADV_INT_MIN      8  //!< Minimum advertising interval, when in General discoverable mode (n * 0.625 mSec)
#define TGAP_GEN_DISC_ADV_INT_MAX      9  //!< Maximum advertising interval, when in General discoverable mode (n * 0.625 mSec)
#define TGAP_CONN_ADV_INT_MIN         10  //!< Minimum advertising interval, when in Connectable mode (n * 0.625 mSec)
#define TGAP_CONN_ADV_INT_MAX         11  //!< Maximum advertising interval, when in Connectable mode (n * 0.625 mSec)
#define TGAP_CONN_SCAN_INT            12  //!< Scan interval used during Link Layer Initiating state, when in Connectable mode (n * 0.625 mSec)
#define TGAP_CONN_SCAN_WIND           13  //!< Scan window used during Link Layer Initiating state, when in Connectable mode (n * 0.625 mSec)
#define TGAP_CONN_HIGH_SCAN_INT       14  //!< Scan interval used during Link Layer Initiating state, when in Connectable mode, high duty scan cycle scan parameters (n * 0.625 mSec)
#define TGAP_CONN_HIGH_SCAN_WIND      15  //!< Scan window used during Link Layer Initiating state, when in Connectable mode, high duty scan cycle scan parameters (n * 0.625 mSec)
#define TGAP_GEN_DISC_SCAN_INT        16  //!< Scan interval used during Link Layer Scanning state, when in General Discovery proc (n * 0.625 mSec)
#define TGAP_GEN_DISC_SCAN_WIND       17  //!< Scan window used during Link Layer Scanning state, when in General Discovery proc (n * 0.625 mSec)
#define TGAP_LIM_DISC_SCAN_INT        18  //!< Scan interval used during Link Layer Scanning state, when in Limited Discovery proc (n * 0.625 mSec)
#define TGAP_LIM_DISC_SCAN_WIND       19  //!< Scan window used during Link Layer Scanning state, when in Limited Discovery proc (n * 0.625 mSec)
#define TGAP_CONN_EST_ADV             20  //!< Advertising interval, when using Connection Establishment proc (n * 0.625 mSec). Obsolete - Do not use.
#define TGAP_CONN_EST_INT_MIN         21  //!< Minimum Link Layer connection interval, when using Connection Establishment proc (n * 1.25 mSec)
#define TGAP_CONN_EST_INT_MAX         22  //!< Maximum Link Layer connection interval, when using Connection Establishment proc (n * 1.25 mSec)
#define TGAP_CONN_EST_SCAN_INT        23  //!< Scan interval used during Link Layer Initiating state, when using Connection Establishment proc (n * 0.625 mSec)
#define TGAP_CONN_EST_SCAN_WIND       24  //!< Scan window used during Link Layer Initiating state, when using Connection Establishment proc (n * 0.625 mSec)
#define TGAP_CONN_EST_SUPERV_TIMEOUT  25  //!< Link Layer connection supervision timeout, when using Connection Establishment proc (n * 10 mSec)
#define TGAP_CONN_EST_LATENCY         26  //!< Link Layer connection slave latency, when using Connection Establishment proc (in number of connection events)
#define TGAP_CONN_EST_MIN_CE_LEN      27  //!< Local informational parameter about min len of connection needed, when using Connection Establishment proc (n * 0.625 mSec)
#define TGAP_CONN_EST_MAX_CE_LEN      28  //!< Local informational parameter about max len of connection needed, when using Connection Establishment proc (n * 0.625 mSec)
#define TGAP_PRIVATE_ADDR_INT         29  //!< Minimum Time Interval between private (resolvable) address changes. In minutes (default 15 minutes)
#define TGAP_CONN_PAUSE_CENTRAL       30  //!< Central idle timer. In seconds (default 1 second)
#define TGAP_CONN_PAUSE_PERIPHERAL    31  //!< Minimum time upon connection establishment before the peripheral starts a connection update procedure. In seconds (default 5 seconds)
// Proprietary
#define TGAP_SM_TIMEOUT               32  //!< SM Message Timeout (milliseconds). Default 30 seconds.
#define TGAP_SM_MIN_KEY_LEN           33  //!< SM Minimum Key Length supported. Default 7.
#define TGAP_SM_MAX_KEY_LEN           34  //!< SM Maximum Key Length supported. Default 16.
#define TGAP_FILTER_ADV_REPORTS       35  //!< Filter duplicate advertising reports. Default TRUE.
#define TGAP_SCAN_RSP_RSSI_MIN        36  //!< Minimum RSSI required for scan responses to be reported to the app. Default -127.
#define TGAP_REJECT_CONN_PARAMS       37  //!< Whether or not to reject Connection Parameter Update Request received on Central device. Default FALSE.
#define TGAP_AUTH_TASK_ID             38  //!< Task ID override for Task Authentication control (for stack internal use only)
#define TGAP_PARAMID_MAX              39  //!< ID MAX-valid Parameter ID

// GAP_DEVDISC_MODE_DEFINES GAP Device Discovery Modes
#define DEVDISC_MODE_NONDISCOVERABLE  0x00    //!< No discoverable setting
#define DEVDISC_MODE_GENERAL          0x01    //!< General Discoverable devices
#define DEVDISC_MODE_LIMITED          0x02    //!< Limited Discoverable devices
#define DEVDISC_MODE_ALL              0x03    //!< Not filtered

// GAP_ADDR_TYPE_DEFINES GAP Address Types
#define ADDRTYPE_PUBLIC               0x00  //!< Use the BD_ADDR
#define ADDRTYPE_STATIC               0x01  //!< Static address
#define ADDRTYPE_PRIVATE_NONRESOLVE   0x02  //!< Generate Non-Resolvable Private Address
#define ADDRTYPE_PRIVATE_RESOLVE      0x03  //!< Generate Resolvable Private Address

// GAP_ADVERTISEMENT_TYPE_DEFINES GAP Advertising Event Types
#define GAP_ADTYPE_ADV_IND                0x00  //!< Connectable undirected advertisement
#define GAP_ADTYPE_ADV_HDC_DIRECT_IND     0x01  //!< Connectable high duty cycle directed advertisement
#define GAP_ADTYPE_ADV_SCAN_IND           0x02  //!< Scannable undirected advertisement
#define GAP_ADTYPE_ADV_NONCONN_IND        0x03  //!< Non-Connectable undirected advertisement
#define GAP_ADTYPE_ADV_LDC_DIRECT_IND     0x04  //!< Connectable low duty cycle directed advertisement

// GAP_ADVERTISEMENT_REPORT_TYPE_DEFINES GAP Advertising Report Event Types
#define GAP_ADRPT_ADV_IND                 0x00  //!< Connectable undirected advertisement
#define GAP_ADRPT_ADV_DIRECT_IND          0x01  //!< Connectable directed advertisement
#define GAP_ADRPT_ADV_SCAN_IND            0x02  //!< Scannable undirected advertisement
#define GAP_ADRPT_ADV_NONCONN_IND         0x03  //!< Non-Connectable undirected advertisement
#define GAP_ADRPT_SCAN_RSP                0x04  //!< Scan Response

// GAP_FILTER_POLICY_DEFINES GAP Advertiser Filter Scan Parameters
#define GAP_FILTER_POLICY_ALL         0x00 //!< Allow Scan Request from Any, Allow Connect Request from Any (default).
#define GAP_FILTER_POLICY_WHITE_SCAN  0x01 //!< Allow Scan Request from White List Only, Allow Connect from Any
#define GAP_FILTER_POLICY_WHITE_CON   0x02 //!< Allow Scan Request from Any, Connect from White List Only
#define GAP_FILTER_POLICY_WHITE       0x03 //!< Allow Scan Request and Connect from White List Only

//! Advertiser Channel Map
#define ADV_CHANMAP_SIZE                 5

//! Maximum Pairing Passcode/Passkey value.  Range of a passkey can be 0 - 999,999.
#define GAP_PASSCODE_MAX                 999999

/** Sign Counter Initialized - Sign counter hasn't been used yet.  Used when setting up
 *  a connection's signing information.
 */
#define GAP_INIT_SIGN_COUNTER            0xFFFFFFFF

// GAP_ADVCHAN_DEFINES GAP Advertisement Channel Map
#define GAP_ADVCHAN_37  0x01  //!< Advertisement Channel 37
#define GAP_ADVCHAN_38  0x02  //!< Advertisement Channel 38
#define GAP_ADVCHAN_39  0x04  //!< Advertisement Channel 39
#define GAP_ADVCHAN_ALL (GAP_ADVCHAN_37 | GAP_ADVCHAN_38 | GAP_ADVCHAN_39) //!< All Advertisement Channels Enabled

// GAP_WHITELIST_DEFINES GAP White List Options
#define WL_NOTUSED    0x00    //!< White list not used but the advertiser's address in this command is used
#define WL_USED       0x01    //!< White list is used and the advertiser's address in this command is not used.

// GAP_ADTYPE_DEFINES GAP Advertisement Data Types
#define GAP_ADTYPE_FLAGS                        0x01 //!< Discovery Mode: @ref GAP_ADTYPE_FLAGS_MODES
#define GAP_ADTYPE_16BIT_MORE                   0x02 //!< Service: More 16-bit UUIDs available
#define GAP_ADTYPE_16BIT_COMPLETE               0x03 //!< Service: Complete list of 16-bit UUIDs
#define GAP_ADTYPE_32BIT_MORE                   0x04 //!< Service: More 32-bit UUIDs available
#define GAP_ADTYPE_32BIT_COMPLETE               0x05 //!< Service: Complete list of 32-bit UUIDs
#define GAP_ADTYPE_128BIT_MORE                  0x06 //!< Service: More 128-bit UUIDs available
#define GAP_ADTYPE_128BIT_COMPLETE              0x07 //!< Service: Complete list of 128-bit UUIDs
#define GAP_ADTYPE_LOCAL_NAME_SHORT             0x08 //!< Shortened local name
#define GAP_ADTYPE_LOCAL_NAME_COMPLETE          0x09 //!< Complete local name
#define GAP_ADTYPE_POWER_LEVEL                  0x0A //!< TX Power Level: 0xXX: -127 to +127 dBm
#define GAP_ADTYPE_OOB_CLASS_OF_DEVICE          0x0D //!< Simple Pairing OOB Tag: Class of device (3 octets)
#define GAP_ADTYPE_OOB_SIMPLE_PAIRING_HASHC     0x0E //!< Simple Pairing OOB Tag: Simple Pairing Hash C (16 octets)
#define GAP_ADTYPE_OOB_SIMPLE_PAIRING_RANDR     0x0F //!< Simple Pairing OOB Tag: Simple Pairing Randomizer R (16 octets)
#define GAP_ADTYPE_SM_TK                        0x10 //!< Security Manager TK Value
#define GAP_ADTYPE_SM_OOB_FLAG                  0x11 //!< Security Manager OOB Flags
#define GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE    0x12 //!< Min and Max values of the connection interval (2 octets Min, 2 octets Max) (0xFFFF indicates no conn interval min or max)
#define GAP_ADTYPE_SIGNED_DATA                  0x13 //!< Signed Data field
#define GAP_ADTYPE_SERVICES_LIST_16BIT          0x14 //!< Service Solicitation: list of 16-bit Service UUIDs
#define GAP_ADTYPE_SERVICES_LIST_128BIT         0x15 //!< Service Solicitation: list of 128-bit Service UUIDs
#define GAP_ADTYPE_SERVICE_DATA                 0x16 //!< Service Data - 16-bit UUID
#define GAP_ADTYPE_PUBLIC_TARGET_ADDR           0x17 //!< Public Target Address
#define GAP_ADTYPE_RANDOM_TARGET_ADDR           0x18 //!< Random Target Address
#define GAP_ADTYPE_APPEARANCE                   0x19 //!< Appearance
#define GAP_ADTYPE_ADV_INTERVAL                 0x1A //!< Advertising Interval
#define GAP_ADTYPE_LE_BD_ADDR                   0x1B //!< LE Bluetooth Device Address
#define GAP_ADTYPE_LE_ROLE                      0x1C //!< LE Role
#define GAP_ADTYPE_SIMPLE_PAIRING_HASHC_256     0x1D //!< Simple Pairing Hash C-256
#define GAP_ADTYPE_SIMPLE_PAIRING_RANDR_256     0x1E //!< Simple Pairing Randomizer R-256
#define GAP_ADTYPE_SERVICE_DATA_32BIT           0x20 //!< Service Data - 32-bit UUID
#define GAP_ADTYPE_SERVICE_DATA_128BIT          0x21 //!< Service Data - 128-bit UUID
#define GAP_ADTYPE_3D_INFO_DATA                 0x3D //!< 3D Information Data
#define GAP_ADTYPE_MANUFACTURER_SPECIFIC        0xFF //!< Manufacturer Specific Data: first 2 octets contain the Company Identifier Code followed by the additional manufacturer specific data

// GAP_ADTYPE_FLAGS_MODES GAP ADTYPE Flags Discovery Modes
#define GAP_ADTYPE_FLAGS_LIMITED                0x01 //!< Discovery Mode: LE Limited Discoverable Mode
#define GAP_ADTYPE_FLAGS_GENERAL                0x02 //!< Discovery Mode: LE General Discoverable Mode
#define GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED    0x04 //!< Discovery Mode: BR/EDR Not Supported

// GAP_APPEARANCE_VALUES GAP Appearance Values
#define GAP_APPEARE_UNKNOWN                     0x0000 //!< Unknown
#define GAP_APPEARE_GENERIC_PHONE               0x0040 //!< Generic Phone
#define GAP_APPEARE_GENERIC_COMPUTER            0x0080 //!< Generic Computer
#define GAP_APPEARE_GENERIC_WATCH               0x00C0 //!< Generic Watch
#define GAP_APPEARE_WATCH_SPORTS                0x00C1 //!< Watch: Sports Watch
#define GAP_APPEARE_GENERIC_CLOCK               0x0100 //!< Generic Clock
#define GAP_APPEARE_GENERIC_DISPLAY             0x0140 //!< Generic Display
#define GAP_APPEARE_GENERIC_RC                  0x0180 //!< Generic Remote Control
#define GAP_APPEARE_GENERIC_EYE_GALSSES         0x01C0 //!< Generic Eye-glasses
#define GAP_APPEARE_GENERIC_TAG                 0x0200 //!< Generic Tag
#define GAP_APPEARE_GENERIC_KEYRING             0x0240 //!< Generic Keyring
#define GAP_APPEARE_GENERIC_MEDIA_PLAYER        0x0280 //!< Generic Media Player
#define GAP_APPEARE_GENERIC_BARCODE_SCANNER     0x02C0 //!< Generic Barcode Scanner
#define GAP_APPEARE_GENERIC_THERMOMETER         0x0300 //!< Generic Thermometer
#define GAP_APPEARE_GENERIC_THERMO_EAR          0x0301 //!< Thermometer: Ear
#define GAP_APPEARE_GENERIC_HR_SENSOR           0x0340 //!< Generic Heart rate Sensor
#define GAP_APPEARE_GENERIC_HRS_BELT            0x0341 //!< Heart Rate Sensor: Heart Rate Belt
#define GAP_APPEARE_GENERIC_BLOOD_PRESSURE      0x0380 //!< Generic Blood Pressure
#define GAP_APPEARE_GENERIC_BP_ARM              0x0381 //!< Blood Pressure: Arm
#define GAP_APPEARE_GENERIC_BP_WRIST            0x0382 //!< Blood Pressure: Wrist
#define GAP_APPEARE_GENERIC_HID                 0x03C0 //!< Generic Human Interface Device (HID)
#define GAP_APPEARE_HID_KEYBOARD                0x03C1 //!< HID Keyboard
#define GAP_APPEARE_HID_MOUSE                   0x03C2 //!< HID Mouse
#define GAP_APPEARE_HID_JOYSTIC                 0x03C3 //!< HID Joystick
#define GAP_APPEARE_HID_GAMEPAD                 0x03C4 //!< HID Gamepad
#define GAP_APPEARE_HID_DIGITIZER_TYABLET       0x03C5 //!< HID Digitizer Tablet
#define GAP_APPEARE_HID_DIGITAL_CARDREADER      0x03C6 //!< HID Card Reader
#define GAP_APPEARE_HID_DIGITAL_PEN             0x03C7 //!< HID Digital Pen
#define GAP_APPEARE_HID_BARCODE_SCANNER         0x03C8 //!< HID Barcode Scanner
/*********************************************gapRole*********************************************************/
#define GAPROLE_PROFILEROLE         0x300  //!< Reading this parameter will return GAP Role type. Read Only. Size is uint8.
#define GAPROLE_IRK                 0x301  //!< Identity Resolving Key. Read/Write. Size is uint8[KEYLEN]. Default is all 0, which means that the IRK will be randomly generated.
#define GAPROLE_SRK                 0x302  //!< Signature Resolving Key. Read/Write. Size is uint8[KEYLEN]. Default is all 0, which means that the SRK will be randomly generated.
#define GAPROLE_SIGNCOUNTER         0x303  //!< Sign Counter. Read/Write. Size is uint32. Default is 0.
#define GAPROLE_BD_ADDR             0x304  //!< Device's Address. Read Only. Size is uint8[B_ADDR_LEN]. This item is read from the controller.
#define GAPROLE_ADVERT_ENABLED      0x305  //!< Enable/Disable Advertising. Read/Write. Size is uint8. Default is TRUE=Enabled.
#define GAPROLE_ADVERT_OFF_TIME     0x306  //!< Advertising Off Time for Limited advertisements (in milliseconds). Read/Write. Size is uint16. Default is 30 seconds.
#define GAPROLE_ADVERT_DATA         0x307  //!< Advertisement Data. Read/Write. Max size is uint8[B_MAX_ADV_LEN].  Default is "02:01:01", which means that it is a Limited Discoverable Advertisement.
#define GAPROLE_SCAN_RSP_DATA       0x308  //!< Scan Response Data. Read/Write. Max size is uint8[B_MAX_ADV_LEN]. Defaults to all 0.
#define GAPROLE_ADV_EVENT_TYPE      0x309  //!< Advertisement Type. Read/Write. Size is uint8.  Default is GAP_ADTYPE_ADV_IND (defined in GAP.h).
#define GAPROLE_ADV_DIRECT_TYPE     0x30A  //!< Direct Advertisement Address Type. Read/Write. Size is uint8. Default is ADDRTYPE_PUBLIC (defined in GAP.h).
#define GAPROLE_ADV_DIRECT_ADDR     0x30B  //!< Direct Advertisement Address. Read/Write. Size is uint8[B_ADDR_LEN]. Default is NULL.
#define GAPROLE_ADV_CHANNEL_MAP     0x30C  //!< Which channels to advertise on. Read/Write Size is uint8. Default is GAP_ADVCHAN_ALL (defined in GAP.h)
#define GAPROLE_ADV_FILTER_POLICY   0x30D  //!< Filter Policy. Ignored when directed advertising is used. Read/Write. Size is uint8. Default is GAP_FILTER_POLICY_ALL (defined in GAP.h).
#define GAPROLE_CONNHANDLE          0x30E  //!< Connection Handle. Read Only. Size is uint16.
#define GAPROLE_RSSI_READ_RATE      0x30F  //!< How often to read the RSSI during a connection. Read/Write. Size is uint16. The value is in milliseconds. Default is 0 = OFF.
#define GAPROLE_PARAM_UPDATE_ENABLE 0x310  //!< Slave Connection Parameter Update Enable. Read/Write. Size is uint8. If TRUE then automatic connection parameter update request is sent. Default is FALSE.
#define GAPROLE_MIN_CONN_INTERVAL   0x311  //!< Minimum Connection Interval to allow (n * 1.25ms).  Range: 7.5 msec to 4 seconds (0x0006 to 0x0C80). Read/Write. Size is uint16. Default is 7.5 milliseconds (0x0006).
#define GAPROLE_MAX_CONN_INTERVAL   0x312  //!< Maximum Connection Interval to allow (n * 1.25ms).  Range: 7.5 msec to 4 seconds (0x0006 to 0x0C80). Read/Write. Size is uint16. Default is 4 seconds (0x0C80).
#define GAPROLE_SLAVE_LATENCY       0x313  //!< Update Parameter Slave Latency. Range: 0 - 499. Read/Write. Size is uint16. Default is 0.
#define GAPROLE_TIMEOUT_MULTIPLIER  0x314  //!< Update Parameter Timeout Multiplier (n * 10ms). Range: 100ms to 32 seconds (0x000a - 0x0c80). Read/Write. Size is uint16. Default is 1000.
#define GAPROLE_CONN_BD_ADDR        0x315  //!< Address of connected device. Read only. Size is uint8[B_MAX_ADV_LEN]. Set to all zeros when not connected.
#define GAPROLE_CONN_INTERVAL       0x316  //!< Current connection interval.  Read only. Size is uint16.  Range is 7.5ms to 4 seconds (0x0006 to 0x0C80).  Default is 0 (no connection).
#define GAPROLE_CONN_LATENCY        0x317  //!< Current slave latency.  Read only.  Size is uint16.  Range is 0 to 499. Default is 0 (no slave latency or no connection).
#define GAPROLE_CONN_TIMEOUT        0x318  //!< Current timeout value.  Read only.  size is uint16.  Range is 100ms to 32 seconds.  Default is 0 (no connection).
#define GAPROLE_PARAM_UPDATE_REQ    0x319  //!< Slave Connection Parameter Update Request. Write. Size is uint8. If TRUE then connection parameter update request is sent.
#define GAPROLE_STATE               0x31A  //!< Reading this parameter will return GAP Peripheral Role State. Read Only. Size is uint8.
#define GAPROLE_MAX_SCAN_RES        0x31B  //!< Maximum number of discover scan results to receive. Default is 0 = unlimited.

/*****************************************************GAPBOND*****************************************************************/

// GAPBOND_PROFILE_PARAMETERS GAP Bond Manager Parameters
#define GAPBOND_PAIRING_MODE       0x400  //!< Pairing Mode: @ref  GAPBOND_PAIRING_MODE_DEFINES. Read/Write. Size is uint8. Default is GAPBOND_PAIRING_MODE_WAIT_FOR_REQ.
#define GAPBOND_INITIATE_WAIT      0x401  //!< Pairing Mode Initiate wait timeout.  This is the time it will wait for a Pairing Request before sending the Slave Initiate Request. Read/Write. Size is uint16. Default is 1000(in milliseconds).
#define GAPBOND_MITM_PROTECTION    0x402  //!< Man-In-The-Middle (MITM) basically turns on Passkey protection in the pairing algorithm. Read/Write. Size is uint8. Default is 0(disabled).
#define GAPBOND_IO_CAPABILITIES    0x403  //!< I/O capabilities.  Read/Write. Size is uint8. Default is GAPBOND_IO_CAP_DISPLAY_ONLY @ref GAPBOND_IO_CAP_DEFINES.
#define GAPBOND_OOB_ENABLED        0x404  //!< OOB data available for pairing algorithm. Read/Write. Size is uint8. Default is 0(disabled).
#define GAPBOND_OOB_DATA           0x405  //!< OOB Data. Read/Write. size uint8[16]. Default is all 0's.
#define GAPBOND_BONDING_ENABLED    0x406  //!< Request Bonding during the pairing process if enabled.  Read/Write. Size is uint8. Default is 0(disabled).
#define GAPBOND_KEY_DIST_LIST      0x407  //!< The key distribution list for bonding.  size is uint8.  @ref GAPBOND_KEY_DIST_DEFINES. Default is sEncKey, sIdKey, mIdKey, mSign enabled.
#define GAPBOND_DEFAULT_PASSCODE   0x408  //!< The default passcode for MITM protection. size is uint32. Range is 0 - 999,999. Default is 0.
#define GAPBOND_ERASE_ALLBONDS     0x409  //!< Erase all of the bonded devices. Write Only. No Size.
#define GAPBOND_AUTO_FAIL_PAIRING  0x40A  //!< TEST MODE (DO NOT USE) to automatically send a Pairing Fail when a Pairing Request is received. Read/Write. size is uint8. Default is 0 (disabled).
#define GAPBOND_AUTO_FAIL_REASON   0x40B  //!< TEST MODE (DO NOT USE) Pairing Fail reason when auto failing. Read/Write. size is uint8. Default is 0x05 (SMP_PAIRING_FAILED_NOT_SUPPORTED).
#define GAPBOND_KEYSIZE            0x40C  //!< Key Size used in pairing. Read/Write. size is uint8. Default is 16.
#define GAPBOND_AUTO_SYNC_WL       0x40D  //!< Clears the White List adds to it each unique address stored by bonds in NV. Read/Write. Size is uint8. Default is FALSE.
#define GAPBOND_BOND_COUNT         0x40E  //!< Gets the total number of bonds stored in NV. Read Only. Size is uint8. Default is 0 (no bonds).
#define GAPBOND_BOND_FAIL_ACTION   0x40F  //!< Possible actions Central may take upon an unsuccessful bonding. Write Only. Size is uint8. Default is 0x02 (Terminate link upon unsuccessful bonding).
#define GAPBOND_ERASE_SINGLEBOND   0x410  //!< Erase a single bonded device. Write only. Must provide address type followed by device address.

// GAPBOND_PAIRING_MODE_DEFINES GAP Bond Manager Pairing Modes
#define GAPBOND_PAIRING_MODE_NO_PAIRING          0x00  //!< Pairing is not allowed
#define GAPBOND_PAIRING_MODE_WAIT_FOR_REQ        0x01  //!< Wait for a pairing request or slave security request
#define GAPBOND_PAIRING_MODE_INITIATE            0x02  //!< Don't wait, initiate a pairing request or slave security request

// GAPBOND_IO_CAP_DEFINES GAP Bond Manager I/O Capabilities
#define GAPBOND_IO_CAP_DISPLAY_ONLY              0x00  //!< Display Only Device
#define GAPBOND_IO_CAP_DISPLAY_YES_NO            0x01  //!< Display and Yes and No Capable
#define GAPBOND_IO_CAP_KEYBOARD_ONLY             0x02  //!< Keyboard Only
#define GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT        0x03  //!< No Display or Input Device
#define GAPBOND_IO_CAP_KEYBOARD_DISPLAY          0x04  //!< Both Keyboard and Display Capable

// GAPBOND_KEY_DIST_DEFINES GAP Bond Manager Key Distribution
#define GAPBOND_KEYDIST_SENCKEY                  0x01  //!< Slave Encryption Key
#define GAPBOND_KEYDIST_SIDKEY                   0x02  //!< Slave IRK and ID information
#define GAPBOND_KEYDIST_SSIGN                    0x04  //!< Slave CSRK
#define GAPBOND_KEYDIST_SLINK                    0x08  //!< Slave Link Key
#define GAPBOND_KEYDIST_MENCKEY                  0x10  //!< Master Encrypton Key
#define GAPBOND_KEYDIST_MIDKEY                   0x20  //!< Master IRK and ID information
#define GAPBOND_KEYDIST_MSIGN                    0x40  //!< Master CSRK
#define GAPBOND_KEYDIST_MLINK                    0x80  //!< Master Link Key

// GAPBOND_PAIRING_STATE_DEFINES GAP Bond Manager Pairing States
#define GAPBOND_PAIRING_STATE_STARTED             0x00  //!< Pairing started
#define GAPBOND_PAIRING_STATE_COMPLETE            0x01  //!< Pairing complete
#define GAPBOND_PAIRING_STATE_BONDED              0x02  //!< Devices bonded
#define GAPBOND_PAIRING_STATE_BOND_SAVED          0x03  //!< Bonding record saved in NV

// SMP_PAIRING_FAILED_DEFINES Pairing failure status values
#define SMP_PAIRING_FAILED_PASSKEY_ENTRY_FAILED   0x01 //!< The user input of the passkey failed, for example, the user cancelled the operation.
#define SMP_PAIRING_FAILED_OOB_NOT_AVAIL          0x02 //!< The OOB data is not available
#define SMP_PAIRING_FAILED_AUTH_REQ               0x03 //!< The pairing procedure can't be performed as authentication requirements can't be met due to IO capabilities of one or both devices
#define SMP_PAIRING_FAILED_CONFIRM_VALUE          0x04 //!< The confirm value doesn't match the calculated compare value
#define SMP_PAIRING_FAILED_NOT_SUPPORTED          0x05 //!< Pairing isn't supported by the device
#define SMP_PAIRING_FAILED_ENC_KEY_SIZE           0x06 //!< The resultant encryption key size is insufficient for the security requirements of this device.
#define SMP_PAIRING_FAILED_CMD_NOT_SUPPORTED      0x07 //!< The SMP command received is not supported on this device.
#define SMP_PAIRING_FAILED_UNSPECIFIED            0x08 //!< Pairing failed due to an unspecified reason
#define SMP_PAIRING_FAILED_REPEATED_ATTEMPTS      0x09 //!< Pairing or authentication procedure is disallowed because too little time has elapsed since the last pairing request or security request.

// GAPBOND_BONDING_FAILURE_DEFINES Bonding Failure Actions
#define GAPBOND_FAIL_NO_ACTION                         0x00 //!< Take no action upon unsuccessful bonding
#define GAPBOND_FAIL_INITIATE_PAIRING                  0x01 //!< Initiate pairing upon unsuccessful bonding
#define GAPBOND_FAIL_TERMINATE_LINK                    0x02 //!< Terminate link upon unsuccessful bonding
#define GAPBOND_FAIL_TERMINATE_ERASE_BONDS             0x03 //!< Terminate link and erase all existing bonds on device upon unsuccessful bonding

/*********************************************************************
 * TYPEDEFS
 */
typedef struct
{
  uint8 srk[KEYLEN];  // Signature Resolving Key
  uint32 signCounter; // Sign Counter 
} linkSec_t;

typedef struct
{
  uint8 ltk[KEYLEN];             // Long Term Key
  uint16 div;                    // Diversifier
  uint8 rand[B_RANDOM_NUM_SIZE]; // random number
  uint8 keySize;                 // LTK Key Size
} encParams_t;

typedef struct
{
  uint8 taskID;            // Application that controls the link
  uint16 connectionHandle; // Controller connection handle
  uint8 stateFlags;        // LINK_CONNECTED, LINK_AUTHENTICATED...
  uint8 addrType;          // Address type of connected device
  uint8 addr[B_ADDR_LEN];  // Other Device's address
  uint8 connRole;          // Connection formed as Master or Slave
  uint16 connInterval;     // The connection's interval (n * 1.23 ms)
  uint16 MTU;              // The connection's MTU size
  linkSec_t sec;           // Connection Security related items
  encParams_t *pEncParams; // pointer to LTK, ediv, rand. if needed.
} linkDBItem_t;

// function pointer used to register for a status callback
typedef void (*pfnLinkDBCB_t)( uint16 connectionHandle, uint8 changeType );

// function pointer used to perform specialized link database searches
typedef void (*pfnPerformFuncCB_t)( linkDBItem_t *pLinkItem );

/**
 * Attribute Type format (2 or 16 octet UUID).
 */
typedef struct
{
  uint8 len;                 //!< Length of UUID (2 or 16)
  uint8 uuid[ATT_UUID_SIZE]; //!< 16 or 128 bit UUID
} attAttrType_t;

/**
 * Attribute Type format (2-octet Bluetooth UUID).
 */
typedef struct
{
  uint8 len;                    //!< Length of UUID (2)
  uint8 uuid[ATT_BT_UUID_SIZE]; //!< 16 bit UUID
} attAttrBtType_t;

/**
 * Error Response format.
 */
typedef struct
{
  uint8 reqOpcode; //!< Request that generated this error response
  uint16 handle;   //!< Attribute handle that generated error response
  uint8 errCode;   //!< Reason why the request has generated error response
} attErrorRsp_t;

/**
 * Exchange MTU Request format.
 */
typedef struct
{
  uint16 clientRxMTU; //!< Client receive MTU size 
} attExchangeMTUReq_t;

/**
 * Exchange MTU Response format.
 */
typedef struct
{
  uint16 serverRxMTU; //!< Server receive MTU size 
} attExchangeMTURsp_t;

/**
 * Find Information Request format.
 */
typedef struct
{
  uint16 startHandle;       //!< First requested handle number (must be first field)
  uint16 endHandle;         //!< Last requested handle number
} attFindInfoReq_t;

/**
 * Find Information Response format.
 */
typedef struct
{
  uint16 numInfo; //!< Number of attribute handle-UUID pairs found
  uint8 format;   //!< Format of information data
  uint8 *pInfo;   //!< Information data whose format is determined by format field (4 to ATT_MTU_SIZE-2)
} attFindInfoRsp_t;

/**
 * Find By Type Value Request format.
 */
typedef struct
{
  uint16 startHandle;   //!< First requested handle number (must be first field)
  uint16 endHandle;     //!< Last requested handle number
  attAttrBtType_t type; //!< 2-octet UUID to find
  uint16 len;           //!< Length of value
  uint8 *pValue;        //!< Attribute value to find (0 to ATT_MTU_SIZE-7)
} attFindByTypeValueReq_t;

/**
 * Find By Type Value Response format.
 */
typedef struct
{
  uint16 numInfo;      //!< Number of handles information found
  uint8 *pHandlesInfo; //!< List of 1 or more handles information (4 to ATT_MTU_SIZE-1)
} attFindByTypeValueRsp_t;

/**
 * Read By Type Request format.
 */
typedef struct
{
  uint16 startHandle; //!< First requested handle number (must be first field)
  uint16 endHandle;   //!< Last requested handle number
  attAttrType_t type; //!< Requested type (2 or 16 octet UUID)
} attReadByTypeReq_t;

/**
 * Read By Type Response format.
 */
typedef struct
{
  uint16 numPairs;  //!< Number of attribute handle-UUID pairs found
  uint16 len;       //!< Size of each attribute handle-value pair
  uint8 *pDataList; //!< List of 1 or more attribute handle-value pairs (2 to ATT_MTU_SIZE-2)
} attReadByTypeRsp_t;

/**
 * Read Request format.
 */
typedef struct
{
  uint16 handle; //!< Handle of the attribute to be read (must be first field)
} attReadReq_t;

/**
 * Read Response format.
 */
typedef struct
{
  uint16 len;    //!< Length of value
  uint8 *pValue; //!< Value of the attribute with the handle given (0 to ATT_MTU_SIZE-1)
} attReadRsp_t;

/**
 * Read Blob Req format.
 */
typedef struct
{
  uint16 handle; //!< Handle of the attribute to be read (must be first field)
  uint16 offset; //!< Offset of the first octet to be read
} attReadBlobReq_t;

/**
 * Read Blob Response format.
 */
typedef struct
{
  uint16 len;    //!< Length of value
  uint8 *pValue; //!< Part of the value of the attribute with the handle given (0 to ATT_MTU_SIZE-1)
} attReadBlobRsp_t;

/**
 * Read Multiple Request format.
 */
typedef struct
{
  uint8 *pHandles;   //!< Set of two or more attribute handles (4 to ATT_MTU_SIZE-1) - must be first field
  uint16 numHandles; //!< Number of attribute handles
} attReadMultiReq_t;

/**
 * Read Multiple Response format.
 */
typedef struct
{
  uint16 len;     //!< Length of values
  uint8 *pValues; //!< Set of two or more values (0 to ATT_MTU_SIZE-1)
} attReadMultiRsp_t;

/**
 * Read By Group Type Request format.
 */
typedef struct
{
  uint16 startHandle; //!< First requested handle number (must be first field)
  uint16 endHandle;   //!< Last requested handle number
  attAttrType_t type; //!< Requested group type (2 or 16 octet UUID)
} attReadByGrpTypeReq_t;

/**
 * Read By Group Type Response format.
 */
typedef struct
{
  uint16 numGrps;   //!< Number of attribute handle, end group handle and value sets found
  uint16 len;       //!< Length of each attribute handle, end group handle and value set
  uint8 *pDataList; //!< List of 1 or more attribute handle, end group handle and value (4 to ATT_MTU_SIZE-2)
} attReadByGrpTypeRsp_t;

/**
 * Write Request format.
 */
typedef struct
{
  uint16 handle; //!< Handle of the attribute to be written (must be first field)
  uint16 len;    //!< Length of value
  uint8 *pValue; //!< Value of the attribute to be written (0 to ATT_MTU_SIZE-3)
  uint8 sig;     //!< Authentication Signature status (not included (0), valid (1), invalid (2))
  uint8 cmd;     //!< Command Flag
} attWriteReq_t;

/**
 * Prepare Write Request format.
 */
typedef struct
{
  uint16 handle; //!< Handle of the attribute to be written (must be first field)
  uint16 offset; //!< Offset of the first octet to be written
  uint16 len;    //!< Length of value
  uint8 *pValue; //!< Part of the value of the attribute to be written (0 to ATT_MTU_SIZE-5) - must be allocated
} attPrepareWriteReq_t;

/**
 * Prepare Write Response format.
 */
typedef struct
{
  uint16 handle; //!< Handle of the attribute that has been read
  uint16 offset; //!< Offset of the first octet to be written
  uint16 len;    //!< Length of value
  uint8 *pValue; //!< Part of the value of the attribute to be written (0 to ATT_MTU_SIZE-5)
} attPrepareWriteRsp_t;

/**
 * Execute Write Request format.
 */
typedef struct
{
  uint8 flags; //!< 0x00 - cancel all prepared writes.
               //!< 0x01 - immediately write all pending prepared values.
} attExecuteWriteReq_t;

/**
 * Handle Value Notification format.
 */
typedef struct
{
  uint16 handle; //!< Handle of the attribute that has been changed (must be first field)
  uint16 len;    //!< Length of value
  uint8 *pValue; //!< Current value of the attribute (0 to ATT_MTU_SIZE-3)
} attHandleValueNoti_t;

/**
 * Handle Value Indication format.
 */
typedef struct
{
  uint16 handle; //!< Handle of the attribute that has been changed (must be first field)
  uint16 len;    //!< Length of value
  uint8 *pValue; //!< Current value of the attribute (0 to ATT_MTU_SIZE-3)
} attHandleValueInd_t;

/**
 * ATT Flow Control Violated Event message format.  This message is sent to the
 * app by the local ATT Server or Client when a sequential ATT Request-Response
 * or Indication-Confirmation protocol flow control is violated for a connection.
 * All subsequent ATT Requests and Indications received by the local ATT Server
 * and Client respectively will be dropped.
 *
 * This message is to inform the app (that has registered with GAP by calling 
 * GAP_RegisterForMsgs()) in case it wants to drop the connection.
 */
typedef struct
{
  uint8 opcode;        //!< opcode of message that caused flow control violation
  uint8 pendingOpcode; //!< opcode of pending message
} attFlowCtrlViolatedEvt_t;

/**
 * ATT MTU Updated Event message format.  This message is sent to the app
 * by the local ATT Server or Client when the ATT MTU size is updated for a
 * connection. The default ATT MTU size is 23 octets.
 *
 * This message is to inform the app (that has registered with GAP by calling 
 * GAP_RegisterForMsgs()) about the new ATT MTU size negotiated for a connection.
 */
typedef struct
{
  uint16 MTU; //!< new MTU size
} attMtuUpdatedEvt_t;

/**
 * ATT Message format. It's a union of all attribute protocol messages and
 * locally-generated events used between the attribute protocol and upper
 * layer profile/application.
 */
typedef union
{
  // Request messages
  attExchangeMTUReq_t exchangeMTUReq;         //!< ATT Exchange MTU Request
  attFindInfoReq_t findInfoReq;               //!< ATT Find Information Request
  attFindByTypeValueReq_t findByTypeValueReq; //!< ATT Find By Type Value Request
  attReadByTypeReq_t readByTypeReq;           //!< ATT Read By Type Request
  attReadReq_t readReq;                       //!< ATT Read Request
  attReadBlobReq_t readBlobReq;               //!< ATT Read Blob Request
  attReadMultiReq_t readMultiReq;             //!< ATT Read Multiple Request
  attReadByGrpTypeReq_t readByGrpTypeReq;     //!< ATT Read By Group Type Request
  attWriteReq_t writeReq;                     //!< ATT Write Request
  attPrepareWriteReq_t prepareWriteReq;       //!< ATT Prepare Write Request
  attExecuteWriteReq_t executeWriteReq;       //!< ATT Execute Write Request

  // Response messages
  attErrorRsp_t errorRsp;                     //!< ATT Error Response
  attExchangeMTURsp_t exchangeMTURsp;         //!< ATT Exchange MTU Response
  attFindInfoRsp_t findInfoRsp;               //!< ATT Find Information Response
  attFindByTypeValueRsp_t findByTypeValueRsp; //!< ATT Find By Type Value Response
  attReadByTypeRsp_t readByTypeRsp;           //!< ATT Read By Type Response
  attReadRsp_t readRsp;                       //!< ATT Read Response
  attReadBlobRsp_t readBlobRsp;               //!< ATT Read Blob Response
  attReadMultiRsp_t readMultiRsp;             //!< ATT Read Multiple Response
  attReadByGrpTypeRsp_t readByGrpTypeRsp;     //!< ATT Read By Group Type Response
  attPrepareWriteRsp_t prepareWriteRsp;       //!< ATT Prepare Write Response

  // Indication and Notification messages
  attHandleValueNoti_t handleValueNoti;       //!< ATT Handle Value Notification
  attHandleValueInd_t handleValueInd;         //!< ATT Handle Value Indication
  
  // Locally-generated event messages
  attFlowCtrlViolatedEvt_t flowCtrlEvt;       //!< ATT Flow Control Violated Event
  attMtuUpdatedEvt_t mtuEvt;                  //!< ATT MTU Updated Event
} attMsg_t;

/**
 * GATT Find By Type Value Request format.
 */
typedef struct
{
  uint16 startHandle;  //!< First requested handle number (must be first field)
  uint16 endHandle;    //!< Last requested handle number
  attAttrType_t value; //!< Primary service UUID value (2 or 16 octets)
} gattFindByTypeValueReq_t;

/**
 * GATT Read By Type Request format.
 */
typedef struct
{
  uint8 discCharsByUUID;  //!< Whether this is a GATT Discover Characteristics by UUID sub-procedure
  attReadByTypeReq_t req; //!< Read By Type Request
} gattReadByTypeReq_t;

/**
 * GATT Write Long Request format. Do not change the order of the members.
 */
typedef struct
{
  uint8 reliable;           //!< Whether reliable writes requested (always FALSE for Write Long)
  attPrepareWriteReq_t req; //!< ATT Prepare Write Request
  uint16 lastOffset;        //!< Offset of last Prepare Write Request sent
} gattWriteLongReq_t;

/**
 * GATT Reliable Writes Request format. Do not change the order of the members.
 */
typedef struct
{
  uint8 reliable;              //!< Whether reliable writes requested (always TRUE for Reliable Writes)
  attPrepareWriteReq_t *pReqs; //!< Array of Prepare Write Requests (must be allocated)
  uint8 numReqs;               //!< Number of Prepare Write Requests
  uint8 index;                 //!< Index of last Prepare Write Request sent
  uint8 flags;                 //!< 0x00 - cancel all prepared writes.
                               //!< 0x01 - immediately write all pending prepared values.
} gattReliableWritesReq_t;

/************************************************************************************************************/
/**
 * GATT Message format. It's a union of all attribute protocol/profile messages
 * and locally-generated events used between the attribute protocol/profile and
 * upper layer application.
 */
typedef union
{
  // Request messages
  attExchangeMTUReq_t exchangeMTUReq;              //!< ATT Exchange MTU Request
  attFindInfoReq_t findInfoReq;                    //!< ATT Find Information Request
  attFindByTypeValueReq_t findByTypeValueReq;      //!< ATT Find By Type Value Request
  attReadByTypeReq_t readByTypeReq;                //!< ATT Read By Type Request
  attReadReq_t readReq;                            //!< ATT Read Request
  attReadBlobReq_t readBlobReq;                    //!< ATT Read Blob Request
  attReadMultiReq_t readMultiReq;                  //!< ATT Read Multiple Request
  attReadByGrpTypeReq_t readByGrpTypeReq;          //!< ATT Read By Group Type Request
  attWriteReq_t writeReq;                          //!< ATT Write Request
  attPrepareWriteReq_t prepareWriteReq;            //!< ATT Prepare Write Request
  attExecuteWriteReq_t executeWriteReq;            //!< ATT Execute Write Request
  gattFindByTypeValueReq_t gattFindByTypeValueReq; //!< GATT Find By Type Value Request
  gattReadByTypeReq_t gattReadByTypeReq;           //!< GATT Read By Type Request
  gattWriteLongReq_t gattWriteLongReq;             //!< GATT Long Write Request
  gattReliableWritesReq_t gattReliableWritesReq;   //!< GATT Reliable Writes Request

  // Response messages
  attErrorRsp_t errorRsp;                          //!< ATT Error Response
  attExchangeMTURsp_t exchangeMTURsp;              //!< ATT Exchange MTU Response
  attFindInfoRsp_t findInfoRsp;                    //!< ATT Find Information Response
  attFindByTypeValueRsp_t findByTypeValueRsp;      //!< ATT Find By Type Value Response
  attReadByTypeRsp_t readByTypeRsp;                //!< ATT Read By Type Response
  attReadRsp_t readRsp;                            //!< ATT Read Response
  attReadBlobRsp_t readBlobRsp;                    //!< ATT Read Blob Response
  attReadMultiRsp_t readMultiRsp;                  //!< ATT Read Multiple Response
  attReadByGrpTypeRsp_t readByGrpTypeRsp;          //!< ATT Read By Group Type Response
  attPrepareWriteRsp_t prepareWriteRsp;            //!< ATT Prepare Write Response

  // Indication and Notification messages
  attHandleValueNoti_t handleValueNoti;            //!< ATT Handle Value Notification
  attHandleValueInd_t handleValueInd;              //!< ATT Handle Value Indication
  
  // Locally-generated event messages
  attFlowCtrlViolatedEvt_t flowCtrlEvt;            //!< ATT Flow Control Violated Event
  attMtuUpdatedEvt_t mtuEvt;                       //!< ATT MTU Updated Event
} gattMsg_t;

/**
 * GATT tmos GATT_MSG_EVENT message format. This message is used to forward an
 * incoming attribute protocol/profile message up to upper layer application.
 */
typedef struct
{
  tmos_event_hdr_t hdr; //!< GATT_MSG_EVENT and status
  uint16 connHandle;    //!< Connection message was received on
  uint8 method;         //!< Type of message
  gattMsg_t msg;        //!< Attribute protocol/profile message
} gattMsgEvent_t;

/**
 * GATT Attribute Type format.
 */
typedef struct
{
  uint8 len;         //!< Length of UUID (2 or 6)
  const uint8 *uuid; //!< Pointer to UUID
} gattAttrType_t;

/**
 * GATT Attribute format.
 */
typedef struct attAttribute_t
{
  gattAttrType_t type; //!< Attribute type (2 or 16 octet UUIDs)
  uint8 permissions;   //!< Attribute permissions
  uint16 handle;       //!< Attribute handle - assigned internally by attribute server
  uint8* pValue; //!< Attribute value - encoding of the octet array is defined in 
                       //!< the applicable profile. The maximum length of an attribute 
                       //!< value shall be 512 octets.
} gattAttribute_t;

/**
 * GATT Service format.
 */
typedef struct
{
  uint16 numAttrs; //!< Number of attributes in attrs
  uint8 encKeySize;//!< Minimum encryption key size required by service (7-16 bytes)
  
  /** Array of attribute records. 
   *  NOTE: The list must start with a Service attribute followed by
   *        all attributes associated with this Service attribute. 
   */
  gattAttribute_t *attrs;
} gattService_t;

/**
 * @brief   Callback function prototype to read an attribute value.
 *
 *          Note: blePending can be returned ONLY for the following
 *                read operations:
 *                - Read Request: ATT_READ_REQ
 *                - Read Blob Request: ATT_READ_BLOB_REQ
 *
 *          Note: If blePending is returned then it's the responsibility
 *                of the application to respond to ATT_READ_REQ and 
 *                ATT_READ_BLOB_REQ message with ATT_READ_RSP and
 *                ATT_READ_BLOB_RSP message respectively.
 *
 *          Note: Payload 'pValue' used with ATT_READ_RSP and ATT_READ_BLOB_RSP
 *                must be allocated using GATT_bm_alloc().
 *
 * @param   connHandle - connection request was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be read (to be returned)
 * @param   pLen - length of data (to be returned)
 * @param   offset - offset of the first octet to be read
 * @param   maxLen - maximum length of data to be read
 * @param   method - type of read message
 *
 * @return  SUCCESS: Read was successfully.<BR>
 *          blePending: A response is pending for this client.<BR>
 *          Error, otherwise: ref ATT_ERR_CODE_DEFINES.<BR>
 */
typedef uint8 (*pfnGATTReadAttrCB_t)( uint16 connHandle, gattAttribute_t *pAttr,
                                          uint8 *pValue, uint16 *pLen, uint16 offset,
                                          uint16 maxLen, uint8 method );
/**
 * @brief   Callback function prototype to write an attribute value.
 *
 *          Note: blePending can be returned ONLY for the following
 *                write operations:
 *                - Write Request: ATT_WRITE_REQ
 *                - Write Command: ATT_WRITE_CMD
 *                - Write Long: ATT_EXECUTE_WRITE_REQ
 *                - Reliable Writes: Multiple ATT_PREPARE_WRITE_REQ followed
 *                  by one final ATT_EXECUTE_WRITE_REQ
 *
 *          Note: If blePending is returned then it's the responsibility
 *                of the application to 1) respond to ATT_WRITE_REQ and 
 *                ATT_EXECUTE_WRITE_REQ message with ATT_WRITE_RSP and
 *                ATT_EXECUTE_WRITE_RSP message respectively, and 2) free
 *                each request payload 'pValue' using BM_free().
 *
 *          Note: Write Command (ATT_WRITE_CMD) does NOT require a response
 *                message.
 *
 * @param   connHandle - connection request was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   pLen - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS: Write was successfully.<BR>
 *          blePending: A response is pending for this client.<BR>
 *          Error, otherwise: ref ATT_ERR_CODE_DEFINES.<BR>
 */
typedef uint8 (*pfnGATTWriteAttrCB_t)( uint16 connHandle, gattAttribute_t *pAttr,
                                           uint8 *pValue, uint16 len, uint16 offset,
                                           uint8 method );
/**
 * @brief   Callback function prototype to authorize a Read or Write operation
 *          on a given attribute.
 *
 * @param   connHandle - connection request was received on
 * @param   pAttr - pointer to attribute
 * @param   opcode - request opcode (ATT_READ_REQ or ATT_WRITE_REQ)
 *
 * @return  SUCCESS: Operation authorized.<BR>
 *          ATT_ERR_INSUFFICIENT_AUTHOR: Authorization required.<BR>
 */
typedef bStatus_t (*pfnGATTAuthorizeAttrCB_t)( uint16 connHandle, gattAttribute_t *pAttr,
                                               uint8 opcode );

/**
 * GATT Structure for Characteristic Presentation Format Value.
 */
typedef struct
{
  uint8 format;    //!< Format of the value of this characteristic
  int8 exponent;   //!< A sign integer which represents the exponent of an integer
  uint16 unit;     //!< Unit of this attribute as defined in the data dictionary
  uint8 nameSpace; //!< Name space of the description
  uint16 desc;     //!< Description of this attribute as defined in a higher layer profile
} gattCharFormat_t;

/**
 * GATT Structure for Client Characteristic Configuration.
 */
typedef struct
{
  uint16 connHandle; //!< Client connection handle
  uint8  value;      //!< Characteristic configuration value for this client
} gattCharCfg_t;

/**
 * GATT Structure for service callback functions - must be setup by the application
 * and used when GATTServApp_RegisterService() is called.
 */
typedef struct
{
  pfnGATTReadAttrCB_t pfnReadAttrCB;           //!< Read callback function pointer
  pfnGATTWriteAttrCB_t pfnWriteAttrCB;         //!< Write callback function pointer
  pfnGATTAuthorizeAttrCB_t pfnAuthorizeAttrCB; //!< Authorization callback function pointer
} gattServiceCBs_t;

/*******************************************SM******************************************************/

/**
 * SM_NEW_RAND_KEY_EVENT message format.  This message is sent to the
 * requesting task.
 */
typedef struct
{
  tmos_event_hdr_t  hdr;      //!< SM_NEW_RAND_KEY_EVENT and status
  uint8 newKey[KEYLEN];       //!< New key value - if status is SUCCESS
} smNewRandKeyEvent_t;

/**
 * Key Distribution field  - True or False fields
 */
typedef struct
{
  unsigned int sEncKey:1;    //!< Set to distribute slave encryption key
  unsigned int sIdKey:1;     //!< Set to distribute slave identity key
  unsigned int sSign:1;      //!< Set to distribute slave signing key
  unsigned int sLinkKey:1;   //!< Set to derive slave link key from slave LTK
  unsigned int sReserved:4;  //!< Reserved for slave - don't use
  unsigned int mEncKey:1;    //!< Set to distribute master encryption key
  unsigned int mIdKey:1;     //!< Set to distribute master identity key
  unsigned int mSign:1;      //!< Set to distribute master signing key
  unsigned int mLinkKey:1;   //!< Set to derive master link key from master LTK
  unsigned int mReserved:4;  //!< Reserved for master - don't use
} keyDist_t;

/**
 * Link Security Requirements
 */
typedef struct
{
  uint8 ioCaps;               //!< I/O Capabilities (ie.
  uint8 oobAvailable;         //!< True if Out-of-band key available
  uint8 oob[KEYLEN];          //!< Out-Of-Bounds key
  uint8 authReq;              //!< Authentication Requirements
  keyDist_t keyDist;          //!< Key Distribution mask
  uint8 maxEncKeySize;        //!< Maximum Encryption Key size (7-16 bytes)
} smLinkSecurityReq_t;

/**
 * Link Security Information
 */
typedef struct
{
  uint8 ltk[KEYLEN];              //!< Long Term Key (LTK)
  uint16 div;                     //!< LTK Diversifier
  uint8 rand[B_RANDOM_NUM_SIZE];  //!< LTK random number
  uint8 keySize;                  //!< LTK Key Size (7-16 bytes)
} smSecurityInfo_t;

/**
 * Link Identity Information
 */
typedef struct
{
  uint8 irk[KEYLEN];          //!< Identity Resolving Key (IRK)
  uint8 bd_addr[B_ADDR_LEN];  //!< The advertiser may set this to zeroes to not disclose its BD_ADDR (public address).
} smIdentityInfo_t;

/**
 * Signing Information
 */
typedef struct
{
  uint8  srk[KEYLEN]; //!< Signature Resolving Key (CSRK)
  uint32 signCounter; //!< Sign Counter
} smSigningInfo_t;

/**
 * Pairing Request & Response - authReq field
 */
typedef struct
{
  unsigned int bonding:2;    //!< Bonding flags
  unsigned int mitm:1;       //!< Man-In-The-Middle (MITM)
  unsigned int sc:1;         //!< LE Secure Connection
  unsigned int keypress:1;
  unsigned int ct2:1;
  unsigned int reserved:2;   //!< Reserved - don't use
} authReq_t;

/*******************************************gap******************************************************/
/**
 * Connection parameters for the peripheral device.  These numbers are used
 * to compare against connection events and request connection parameter
 * updates with the master.
 */
typedef struct
{
  /** Minimum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  uint16 intervalMin;
  /** Maximum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  uint16 intervalMax;
  /** Number of LL latency connection events (0x0000 - 0x03e8) */
  uint16 latency;
  /** Connection Timeout (0x000A - 0x0C80 * 10 ms) */
  uint16 timeout;
} gapPeriConnectParams_t;

/**
 * GAP event header format.
 */
typedef struct
{
  tmos_event_hdr_t  hdr;           //!< GAP_MSG_EVENT and status
  uint8 opcode;                    //!< GAP type of command. Ref: @ref GAP_MSG_EVENT_DEFINES
} gapEventHdr_t;

/**
 * GAP_DEVICE_INIT_DONE_EVENT message format.  This message is sent to the
 * app when the Device Initialization is done [initiated by calling
 * GAP_DeviceInit()].
 */
typedef struct
{
  tmos_event_hdr_t  hdr;              //!< GAP_MSG_EVENT and status
  uint8 opcode;                       //!< GAP_DEVICE_INIT_DONE_EVENT
  uint8 devAddr[B_ADDR_LEN];          //!< Device's BD_ADDR
  uint16 dataPktLen;                  //!< HC_LE_Data_Packet_Length
  uint8 numDataPkts;                  //!< HC_Total_Num_LE_Data_Packets
} gapDeviceInitDoneEvent_t;

/**
 * GAP_SIGNATURE_UPDATED_EVENT message format.  This message is sent to the
 * app when the signature counter has changed.  This message is to inform the
 * application in case it wants to save it to be restored on reboot or reconnect.
 * This message is sent to update a connection's signature counter and to update
 * this device's signature counter.  If devAddr == BD_ADDR, then this message pertains
 * to this device.
 */
typedef struct
{
  tmos_event_hdr_t  hdr;              //!< GAP_MSG_EVENT and status
  uint8 opcode;                       //!< GAP_SIGNATURE_UPDATED_EVENT
  uint8 addrType;                     //!< Device's address type for devAddr
  uint8 devAddr[B_ADDR_LEN];          //!< Device's BD_ADDR, could be own address
  uint32 signCounter;                 //!< new Signed Counter
} gapSignUpdateEvent_t;

/**
 * GAP_DEVICE_INFO_EVENT message format.  This message is sent to the
 * app during a Device Discovery Request, when a new advertisement or scan
 * response is received.
 */
typedef struct
{
  tmos_event_hdr_t  hdr;    //!< GAP_MSG_EVENT and status
  uint8 opcode;             //!< GAP_DEVICE_INFO_EVENT
  uint8 eventType;          //!< Advertisement Type: @ref GAP_ADVERTISEMENT_REPORT_TYPE_DEFINES
  uint8 addrType;           //!< address type: @ref GAP_ADDR_TYPE_DEFINES
  uint8 addr[B_ADDR_LEN];   //!< Address of the advertisement or SCAN_RSP
  int8 rssi;                //!< Advertisement or SCAN_RSP RSSI
  uint8 dataLen;            //!< Length (in bytes) of the data field (evtData)
  uint8 *pEvtData;          //!< Data field of advertisement or SCAN_RSP
} gapDeviceInfoEvent_t;

/**
 * Type of device discovery (Scan) to perform.
 */
typedef struct
{
  uint8 taskID;       //!< Requesting App's Task ID, used to return results
  uint8 mode;         //!< Discovery Mode: @ref GAP_DEVDISC_MODE_DEFINES
  uint8 activeScan;   //!< TRUE for active scanning
  uint8 whiteList;    //!< TRUE to only allow advertisements from devices in the white list.
} gapDevDiscReq_t;

/**
 * Type of device discovery (Scan) to perform.
 */
typedef struct
{
  uint8 eventType;        //!< Indicates advertising event type used by the advertiser: @ref GAP_ADVERTISEMENT_REPORT_TYPE_DEFINES
  uint8 addrType;         //!< Address Type: @ref GAP_ADDR_TYPE_DEFINES
  uint8 addr[B_ADDR_LEN]; //!< Device's Address
} gapDevRec_t;

typedef struct 
{
  uint8 eventType;
  uint8 addrType;
  uint8 addr[6];
  uint8 *advertData;
  uint8 *scanResponData;
}gapScanRecs_t;

/**
 * GAP_DEVICE_DISCOVERY_EVENT message format. This message is sent to the
 * Application after a scan is performed.
 */
typedef struct
{
  tmos_event_hdr_t  hdr; //!< GAP_MSG_EVENT and status
  uint8 opcode;          //!< GAP_DEVICE_DISCOVERY_EVENT
  uint8 numDevs;         //!< Number of devices found during scan
  gapDevRec_t *pDevList; //!< array of device records
} gapDevDiscEvent_t;

/**
 * Advertising Parameters
 */
typedef struct
{
  uint8 eventType;          //!< Advertise Event Type: @ref GAP_ADVERTISEMENT_TYPE_DEFINES
  uint8 initiatorAddrType;  //!< Initiator's address type: @ref GAP_ADDR_TYPE_DEFINES
  uint8 initiatorAddr[B_ADDR_LEN];  //!< Initiator's addr - used only with connectable directed eventType (ADV_EVTTYPE_CONNECTABLE_DIRECTED).
  uint8 channelMap;         //!< Channel Map: Bit mask @ref GAP_ADVCHAN_DEFINES
  uint8 filterPolicy;       //!< Filer Policy: @ref GAP_FILTER_POLICY_DEFINES. Ignored when directed advertising is used.
} gapAdvertisingParams_t;

/**
 * GAP_MAKE_DISCOVERABLE_DONE_EVENT message format.  This message is sent to the
 * app when the Advertise config is complete.
 */
typedef struct
{
  tmos_event_hdr_t  hdr; //!< GAP_MSG_EVENT and status
  uint8 opcode;          //!< GAP_MAKE_DISCOVERABLE_DONE_EVENT
} gapMakeDiscoverableRspEvent_t;

/**
 * GAP_END_DISCOVERABLE_DONE_EVENT message format.  This message is sent to the
 * app when the Advertising has stopped.
 */
typedef struct
{
  tmos_event_hdr_t  hdr; //!< GAP_MSG_EVENT and status
  uint8 opcode;          //!< GAP_END_DISCOVERABLE_DONE_EVENT
} gapEndDiscoverableRspEvent_t;

/**
 * GAP_ADV_DATA_UPDATE_DONE_EVENT message format.  This message is sent to the
 * app when Advertising Data Update is complete.
 */
typedef struct
{
  tmos_event_hdr_t  hdr; //!< GAP_MSG_EVENT and status
  uint8 opcode;          //!< GAP_ADV_DATA_UPDATE_DONE_EVENT
  uint8 adType;          //!< TRUE if advertising data, FALSE if SCAN_RSP
} gapAdvDataUpdateEvent_t;

/**
 * Establish Link Request parameters
 */
typedef struct
{
  uint8 taskID;               //!< Requesting App/Profile's Task ID
  uint8 highDutyCycle;        //!< TRUE to high duty cycle scan, FALSE if not.
  uint8 whiteList;            //!< Determines use of the white list: @ref GAP_WHITELIST_DEFINES
  uint8 addrTypePeer;         //!< Address type of the advertiser: @ref GAP_ADDR_TYPE_DEFINES
  uint8 peerAddr[B_ADDR_LEN]; //!< Advertiser's address
} gapEstLinkReq_t;

/**
 * Update Link Parameters Request parameters
 */
typedef struct
{
  uint16 connectionHandle; //!< Connection handle of the update
  uint16 intervalMin;      //!< Minimum Connection Interval
  uint16 intervalMax;      //!< Maximum Connection Interval
  uint16 connLatency;      //!< Connection Latency
  uint16 connTimeout;      //!< Connection Timeout
} gapUpdateLinkParamReq_t;

/**
 * GAP_LINK_ESTABLISHED_EVENT message format.  This message is sent to the app
 * when the link request is complete.<BR>
 * <BR>
 * For an Observer, this message is sent to complete the Establish Link Request.<BR>
 * For a Peripheral, this message is sent to indicate that a link has been created.
 */
typedef struct
{
  tmos_event_hdr_t  hdr;     //!< GAP_MSG_EVENT and status
  uint8 opcode;              //!< GAP_LINK_ESTABLISHED_EVENT
  uint8 devAddrType;         //!< Device address type: @ref GAP_ADDR_TYPE_DEFINES
  uint8 devAddr[B_ADDR_LEN]; //!< Device address of link
  uint16 connectionHandle;   //!< Connection Handle from controller used to ref the device
  uint8 connRole;            //!< Connection formed as Master or Slave
  uint16 connInterval;       //!< Connection Interval
  uint16 connLatency;        //!< Connection Latency
  uint16 connTimeout;        //!< Connection Timeout
  uint8 clockAccuracy;       //!< Clock Accuracy
} gapEstLinkReqEvent_t;

/**
 * GAP_LINK_PARAM_UPDATE_EVENT message format.  This message is sent to the app
 * when the connection parameters update request is complete.
 */
typedef struct
{
  tmos_event_hdr_t hdr;     //!< GAP_MSG_EVENT and status
  uint8 opcode;             //!< GAP_LINK_PARAM_UPDATE_EVENT
  uint8 status;             //!< bStatus_t
  uint16 connectionHandle;  //!< Connection handle of the update
  uint16 connInterval;      //!< Requested connection interval
  uint16 connLatency;       //!< Requested connection latency
  uint16 connTimeout;       //!< Requested connection timeout
} gapLinkUpdateEvent_t;

/**
 * GAP_LINK_TERMINATED_EVENT message format.  This message is sent to the
 * app when a link to a device is terminated.
 */
typedef struct
{
  tmos_event_hdr_t  hdr;   //!< GAP_MSG_EVENT and status
  uint8 opcode;            //!< GAP_LINK_TERMINATED_EVENT
  uint16 connectionHandle; //!< connection Handle
  uint8 reason;            //!< termination reason from LL
} gapTerminateLinkEvent_t;

/**
 * GAP_PASSKEY_NEEDED_EVENT message format.  This message is sent to the
 * app when a Passkey is needed from the app's user interface.
 */
typedef struct
{
  tmos_event_hdr_t  hdr;        //!< GAP_MSG_EVENT and status
  uint8 opcode;                 //!< GAP_PASSKEY_NEEDED_EVENT
  uint8 deviceAddr[B_ADDR_LEN]; //!< address of device to pair with, and could be either public or random.
  uint16 connectionHandle;      //!< Connection handle
  uint8 uiInputs;               //!< Pairing User Interface Inputs - Ask user to input passcode
  uint8 uiOutputs;              //!< Pairing User Interface Outputs - Display passcode
} gapPasskeyNeededEvent_t;

/**
 * GAP_AUTHENTICATION_COMPLETE_EVENT message format.  This message is sent to the app
 * when the authentication request is complete.
 */
typedef struct
{
  tmos_event_hdr_t  hdr;           //!< GAP_MSG_EVENT and status
  uint8 opcode;                    //!< GAP_AUTHENTICATION_COMPLETE_EVENT
  uint16 connectionHandle;         //!< Connection Handle from controller used to ref the device
  uint8 authState;                 //!< TRUE if the pairing was authenticated (MITM)
  smSecurityInfo_t *pSecurityInfo; //!< BOUND - security information from this device
  smSigningInfo_t *pSigningInfo;   //!< Signing information
  smSecurityInfo_t *pDevSecInfo;   //!< BOUND - security information from connected device
  smIdentityInfo_t *pIdentityInfo; //!< BOUND - identity information
} gapAuthCompleteEvent_t;

/**
 * securityInfo and identityInfo are only used if secReqs.bondable == BOUND, which means that
 * the device is already bound and we should use the security information and keys.
 */
typedef struct
{
  uint16 connectionHandle;      //!< Connection Handle from controller,
  smLinkSecurityReq_t  secReqs; //!< Pairing Control info
} gapAuthParams_t;

/**
 * GAP_SLAVE_REQUESTED_SECURITY_EVENT message format.  This message is sent to the app
 * when a Slave Security Request is received.
 */
typedef struct
{
  tmos_event_hdr_t  hdr;        //!< GAP_MSG_EVENT and status
  uint8 opcode;                 //!< GAP_SLAVE_REQUESTED_SECURITY_EVENT
  uint16 connectionHandle;      //!< Connection Handle
  uint8 deviceAddr[B_ADDR_LEN]; //!< address of device requesting security
  uint8 authReq;                //!< Authentication Requirements: Bit 2: MITM, Bits 0-1: bonding (0 - no bonding, 1 - bonding)
} gapSlaveSecurityReqEvent_t;

/**
 * GAP_BOND_COMPLETE_EVENT message format.  This message is sent to the
 * app when a bonding is complete.  This means that a key is loaded and the link is encrypted.
 */
typedef struct
{
  tmos_event_hdr_t  hdr;   //!< GAP_MSG_EVENT and status
  uint8 opcode;            //!< GAP_BOND_COMPLETE_EVENT
  uint16 connectionHandle; //!< connection Handle
} gapBondCompleteEvent_t;

/**
 * Pairing Request fields - the parsed fields of the SMP Pairing Request command.
 */
typedef struct
{
  uint8 ioCap;         //!< Pairing Request ioCap field
  uint8 oobDataFlag;   //!< Pairing Request OOB Data Flag field
  uint8 authReq;       //!< Pairing Request Auth Req field
  uint8 maxEncKeySize; //!< Pairing Request Maximum Encryption Key Size field
  keyDist_t keyDist;   //!< Pairing Request Key Distribution field
} gapPairingReq_t;

/**
 * GAP_PAIRING_REQ_EVENT message format.<BR>
 * <BR>
 * This message is sent to the
 * app when an unexpected Pairing Request is received.  The application is
 * expected to setup for a Security Manager pairing/bonding.<BR>
 * <BR>
 * To setup an SM Pairing, the application should call GAP_Authenticate() with these "pairReq" fields.<BR>
 * <BR>
* NOTE: This message should only be sent to peripheral devices.
 */
typedef struct
{
  tmos_event_hdr_t hdr;    //!< GAP_MSG_EVENT and status
  uint8 opcode;            //!< GAP_PAIRING_REQ_EVENT
  uint16 connectionHandle; //!< connection Handle
  gapPairingReq_t pairReq; //!< The Pairing Request fields received.
} gapPairingReqEvent_t;

/**
 * Number of simultaneous links with periodic RSSI reads
 */
#ifndef GAPCENTRALROLE_NUM_RSSI_LINKS
#define GAPCENTRALROLE_NUM_RSSI_LINKS     4
#endif

/**
 * Observer Event Structure
 */
typedef union
{
  gapEventHdr_t             gap;                //!< GAP_MSG_EVENT and status.
  gapDeviceInitDoneEvent_t  initDone;           //!< GAP initialization done.
  gapDeviceInfoEvent_t      deviceInfo;         //!< Discovery device information event structure.
  gapDevDiscEvent_t         discCmpl;           //!< Discovery complete event structure.
} gapObserverRoleEvent_t;

/**
 * RSSI Read Callback Function
 */
typedef void (*pfnGapObserverRoleRssiCB_t)
(
  uint16 connHandle,                    //!< Connection handle.
  int8  rssi                            //!< New RSSI value.
);

/**
 * Observer Event Callback Function
 */
typedef void (*pfnGapObserverRoleEventCB_t)
(
  gapObserverRoleEvent_t *pEvent         //!< Pointer to event structure.
);

/**
 * Observer Callback Structure
 */
typedef struct
{
  pfnGapObserverRoleRssiCB_t   rssiCB;   //!< RSSI callback.
  pfnGapObserverRoleEventCB_t  eventCB;  //!< Event callback.
} gapObserverRoleCB_t;

typedef enum
{
  GAPROLE_INIT = 0,                       //!< Waiting to be started
  GAPROLE_STARTED,                        //!< Started but not advertising
  GAPROLE_ADVERTISING,                    //!< Currently Advertising
  GAPROLE_WAITING,                        //!< Device is started but not advertising, is in waiting period before advertising again
  GAPROLE_WAITING_AFTER_TIMEOUT,          //!< Device just timed out from a connection but is not yet advertising, is in waiting period before advertising again
  GAPROLE_CONNECTED,                      //!< In a connection
  GAPROLE_CONNECTED_ADV,                  //!< In a connection + advertising
  GAPROLE_ERROR                           //!< Error occurred - invalid state
} gaprole_States_t;

/**
 * Central Event Structure
 */
typedef union
{
  gapEventHdr_t             gap;                //!< GAP_MSG_EVENT and status.
  gapDeviceInitDoneEvent_t  initDone;           //!< GAP initialization done.
  gapDeviceInfoEvent_t      deviceInfo;         //!< Discovery device information event structure.
  gapDevDiscEvent_t         discCmpl;           //!< Discovery complete event structure.
  gapEstLinkReqEvent_t      linkCmpl;           //!< Link complete event structure.
  gapLinkUpdateEvent_t      linkUpdate;         //!< Link update event structure.
  gapTerminateLinkEvent_t   linkTerminate;      //!< Link terminated event structure.
} gapCentralRoleEvent_t;

/**
 * RSSI Read Callback Function
 */
typedef void (*pfnGapCentralRoleRssiCB_t)
(
  uint16 connHandle,                    //!< Connection handle.
  int8  rssi                            //!< New RSSI value.
);

/**
 * Central Event Callback Function
 */
typedef void (*pfnGapCentralRoleEventCB_t)
(
  gapCentralRoleEvent_t *pEvent         //!< Pointer to event structure.
);

/**
 * Central Callback Structure
 */
typedef struct
{
  pfnGapCentralRoleRssiCB_t   rssiCB;   //!< RSSI callback.
  pfnGapCentralRoleEventCB_t  eventCB;  //!< Event callback.
} gapCentralRoleCB_t;
/**
 *  Possible actions the peripheral device may take if an unsuccessful parameter
 *  update is received.
 *
 *  Parameters for GAPRole_SendUpdateParam() only
 */
#define GAPROLE_NO_ACTION                    0 // Take no action upon unsuccessful parameter updates
#define GAPROLE_RESEND_PARAM_UPDATE          1 // Continue to resend request until successful update
#define GAPROLE_TERMINATE_LINK               2 // Terminate link upon unsuccessful parameter updates

/**
 * Passcode Callback Function
 */
typedef void (*pfnPasscodeCB_t)
(
  uint8  *deviceAddr,                   //!< address of device to pair with, and could be either public or random.
  uint16 connectionHandle,              //!< Connection handle
  uint8  uiInputs,                      //!< Pairing User Interface Inputs - Ask user to input passcode
  uint8  uiOutputs                      //!< Pairing User Interface Outputs - Display passcode
 );

/**
 * Pairing State Callback Function
 */
typedef void (*pfnPairStateCB_t)
(
  uint16 connectionHandle,              //!< Connection handle
  uint8  state,                         //!< Pairing state @ref GAPBOND_PAIRING_STATE_DEFINES
  uint8  status                         //!< Pairing status
);

/**
 * Callback Registration Structure
 */
typedef struct
{
  pfnPasscodeCB_t     passcodeCB;       //!< Passcode callback
  pfnPairStateCB_t    pairStateCB;      //!< Pairing state callback
} gapBondCBs_t;

/**
 * Callback when the connection parameteres are updated.
 */
typedef void (*gapRolesParamUpdateCB_t)( uint16 connInterval,
                                         uint16 connSlaveLatency,
                                         uint16 connTimeout );

/**
 * Callback when the device has been started.  Callback event to
 * the Notify of a state change.
 */
typedef void (*gapRolesStateNotify_t)( gaprole_States_t newState );

/**
 * Callback when the device has read an new RSSI value during a connection.
 */
typedef void (*gapRolesRssiRead_t)( int8 newRSSI );

/**
 * Callback structure - must be setup by the application and used when gapRoles_StartDevice() is called.
 */
typedef struct
{
  gapRolesStateNotify_t    pfnStateChange;  //!< Whenever the device changes state
  gapRolesRssiRead_t       pfnRssiRead;     //!< When a valid RSSI is read from controller
} gapRolesCBs_t;

typedef struct tag_ble_host_para { 
	u8  gapProfileRole;  //GAP_PROFILE_ROLE_DEFINES
	u8  l2capTaskID;
	u8  gapTaskID;
	u8  gapUnwantedTaskID;
	u8  gattTaskID;
	u8  smTaskID;
	u8  resv1;
	u8  resv2;
}ble_host_para_t;
extern ble_host_para_t gBleHostPara;

/*********************************************************************
 * GLOBAL VARIABLES
 */

extern uint8 linkDBNumConns;

/**
 * GATT Services
 */
extern CONST uint8 gapServiceUUID[];
extern CONST uint8 gattServiceUUID[];

/**
 * GATT Attribute Types
 */
extern CONST uint8 primaryServiceUUID[];
extern CONST uint8 secondaryServiceUUID[];
extern CONST uint8 includeUUID[];
extern CONST uint8 characterUUID[];

/**
 * GATT Characteristic Descriptors
 */
extern CONST uint8 charExtPropsUUID[];
extern CONST uint8 charUserDescUUID[];
extern CONST uint8 clientCharCfgUUID[];
extern CONST uint8 servCharCfgUUID[];
extern CONST uint8 charFormatUUID[];
extern CONST uint8 charAggFormatUUID[];
extern CONST uint8 validRangeUUID[];
extern CONST uint8 extReportRefUUID[];
extern CONST uint8 reportRefUUID[];

/**
 * GATT Characteristic Types
 */
extern CONST uint8 deviceNameUUID[];
extern CONST uint8 appearanceUUID[];
extern CONST uint8 periPrivacyFlagUUID[];
extern CONST uint8 reconnectAddrUUID[];
extern CONST uint8 periConnParamUUID[];
extern CONST uint8 centAddrResUUID[];
extern CONST uint8 serviceChangedUUID[];
extern CONST uint8 manuNameUUID[];
extern CONST uint8 serialNumUUID[];
extern CONST uint8 manuAddrUUID[];

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*
 * linkDB_Init - Initialize the Link Database.
 */
extern void linkDB_Init( void );

/*
 * linkDB_Register - Register with this function to receive a callback when
 *              status changes on a connection.
 */
extern uint8 linkDB_Register( pfnLinkDBCB_t pFunc );

/*
 * linkDB_Add - Adds a record to the link database.
 */
extern bStatus_t linkDB_Add( uint8 taskID, uint16 connectionHandle,
                         uint8 stateFlags, uint8 addrType, uint8 *pAddr,
                         uint8 connRole, uint16 connInterval, uint16 MTU );

/*
 * linkDB_Remove - Removes a record from the link database.
 */
extern uint8 linkDB_Remove( uint16 connectionHandle );

/*
 * linkDB_Update - This function is used to update the stateFlags of 
 *              a link record.
 */
extern uint8 linkDB_Update( uint16 connectionHandle, uint8 newState );

/*
 * linkDB_NumActive - returns the number of active connections.
 */
extern uint8 linkDB_NumActive( void );

/*
 * linkDB_NumConns - returns the maximum number of connections supported.
 */
extern uint8 linkDB_NumConns( void );

/*
 * linkDB_UpdateMTU - This function is used to update the MTU size of 
 *              a link record.
 */
extern uint8 linkDB_UpdateMTU( uint16 connectionHandle, uint16 newMtu );

/*
 * linkDB_MTU - This function is used to get the MTU size of 
 *              a link.
 */
extern uint16 linkDB_MTU( uint16 connectionHandle );

/*
 * linkDB_Find - Find link database item (link information)
 * 
 *    returns a pointer to the link item, NULL if not found
 */
extern linkDBItem_t *linkDB_Find( uint16 connectionHandle );

/*
 * linkDB_FindFirst - Find the first link that matches the taskID.
 * 
 *    returns a pointer to the link item, NULL if not found
 */
extern linkDBItem_t *linkDB_FindFirst( uint8 taskID );

/*
 * linkDB_State - Check to see if a physical link is in a specific state.
 * 
 *    returns TRUE is the link is in state. FALSE, otherwise.
 */
extern uint8 linkDB_State( uint16 connectionHandle, uint8 state );

/*
 * linkDB_Authen - Check to see if the physical link is encrypted and authenticated.
 *    returns SUCCESS if the link is authenticated or 
 *            bleNotConnected - connection handle is invalid, 
 *            LINKDB_ERR_INSUFFICIENT_AUTHEN - link is not encrypted,
 *            LINBDB_ERR_INSUFFICIENT_KEYSIZE - key size encrypted is not large enough,
 *            LINKDB_ERR_INSUFFICIENT_ENCRYPTION - link is encrypted, but not authenticated
 */
extern uint8 linkDB_Authen( uint16 connectionHandle, uint8 keySize, uint8 mitmRequired );

/*
 * linkDB_Role - Get the role of a physical link.
 * 
 *    returns GAP_PROFILE_CENTRAL, GAP_PROFILE_PERIPHERAL or 0 (unknown).
 */
extern uint8 linkDB_Role( uint16 connectionHandle );

/*
 * linkDB_PerformFunc - Perform a function of each connection in the link database.
 */
extern void linkDB_PerformFunc( pfnPerformFuncCB_t cb );

/*
 * linkDB_Up - Check to see if a physical link is up (connected).
 *    Use like:  uint8 linkDB_Up( uint16 connectionHandle );
 *            connectionHandle - controller link connection handle.
 *            returns TRUE if the link is up. FALSE, otherwise.
 */
#define linkDB_Up( connectionHandle )  linkDB_State( (connectionHandle), LINK_CONNECTED )

/*
 * linkDB_Encrypted - Check to see if the physical link is encrypted.
 *    Use like:  linkDB_Encrypted( uint16 connectionHandle );
 *            connectionHandle - controller link connection handle.
 *            returns TRUE if the link is encrypted. FALSE, otherwise.
 */
#define linkDB_Encrypted( connectionHandle )  linkDB_State( (connectionHandle), LINK_ENCRYPTED )

/*
 * linkDB_Authenticated - Check to see if the physical link is authenticated.
 *    Use like:  linkDB_Authenticated( uint16 connectionHandle );
 *            connectionHandle - controller link connection handle.
 *            returns TRUE if the link is authenticated. FALSE, otherwise.
 */
#define linkDB_Authenticated( connectionHandle )  linkDB_State( (connectionHandle), LINK_AUTHENTICATED )

/*
 * linkDB_Bonded - Check to see if the physical link is bonded.
 *    Use like:  linkDB_Bonded( uint16 connectionHandle );
 *            connectionHandle - controller link connection handle.
 *            returns TRUE if the link is bonded. FALSE, otherwise.
 */
#define linkDB_Bonded( connectionHandle )  linkDB_State( (connectionHandle), LINK_BOUND )

/**
 * @internal
 *
 * @brief   Initialize the L2CAP layer.
 *
 * @param   taskId - Task identifier for the desired task
 *
 * @return  none
 */
extern void L2CAP_Init( uint8 taskId );

/**
 * @internal
 *
 * @brief   L2CAP Task event processing function. This function should
 *          be called at periodic intervals when event occur.
 *
 * @param   taskId - Task ID
 * @param   events  - Bitmap of events
 *
 * @return  none
 */
extern uint16 L2CAP_ProcessEvent( uint8 taskId, uint16 events );

/**
 * @brief   This function is used to get the MTU size of a connection.
 *
 * @param   connHandle - connection handle.
 *
 * @return  connection MTU size.<BR>
 */
extern uint16 ATT_GetMTU( uint16 connHandle );

/*
 * Compare two UUIDs. The UUIDs are converted if necessary.
 */
extern uint8 ATT_CompareUUID( const uint8 *pUUID1, uint16 len1, 
                              const uint8 *pUUID2, uint16 len2 );
/*
 * Convert a 16-bit UUID to 128-bit UUID.
 */
extern uint8 ATT_ConvertUUIDto128( const uint8 *pUUID16, uint8 *pUUID128 );

/*
 * Convert a 128-bit UUID to 16-bit UUID.
 */
extern uint8 ATT_ConvertUUIDto16( const uint8 *pUUID128, uint8 *pUUID16 );

/**
 * @brief   Initialize the Generic Attribute Profile Client.
 *
 * @return  SUCCESS: Client initialized successfully.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 */
extern bStatus_t GATT_InitClient( void );

/**
 * @brief   Register to receive incoming ATT Indications or Notifications
 *          of attribute values.
 *
 * @param   taskId ?task to forward indications or notifications to
 *
 * @return  void
 */
extern void GATT_RegisterForInd( uint8 taskId );

/**
 * @brief   The Prepare Write Request is used to request the server to
 *          prepare to write the value of an attribute.
 *
 *          Note: This function is needed only for GATT testing.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_PrepareWriteReq( uint16 connHandle, attPrepareWriteReq_t *pReq, uint8 taskId );

/**
 * @brief   The Execute Write Request is used to request the server to
 *          write or cancel the write of all the prepared values currently
 *          held in the prepare queue from this client.
 *
 *          Note: This function is needed only for GATT testing.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_ExecuteWriteReq( uint16 connHandle, attExecuteWriteReq_t *pReq, uint8 taskId );

/**
 * @brief   Find the attribute record for a given handle and UUID.
 *
 * @param   startHandle - first handle to look for
 * @param   endHandle - last handle to look for
 * @param   pUUID - pointer to UUID to look for
 * @param   len - length of UUID
 * @param   pHandle - handle of owner of attribute (to be returned)
 * 
 * @return  Pointer to attribute record. NULL, otherwise.
 */
extern gattAttribute_t *GATT_FindHandleUUID( uint16 startHandle, uint16 endHandle, const uint8 *pUUID,
                                             uint16 len, uint16 *pHandle );
/**
 * @brief   Find the attribute record for a given handle
 *
 * @param   handle - handle to look for
 * @param   pHandle - handle of owner of attribute (to be returned)
 * 
 * @return  Pointer to attribute record. NULL, otherwise.
 */
extern gattAttribute_t *GATT_FindHandle( uint16 handle, uint16 *pHandle );

/**
 * @brief   Find the next attribute of the same type for a given attribute.
 *
 * @param   pAttr - pointer to attribute to find a next for
 * @param   endHandle - last handle to look for
 * @param   service - handle of owner service
 * @param   pLastHandle - handle of last attribute (to be returned)
 * 
 * @return  Pointer to next attribute record. NULL, otherwise.
 */
extern gattAttribute_t *GATT_FindNextAttr( gattAttribute_t *pAttr, uint16 endHandle,
                                           uint16 service, uint16 *pLastHandle );

/**
 * @brief   This sub-procedure is used when a server is configured to 
 *          indicate a characteristic value to a client and expects an
 *          attribute protocol layer acknowledgement that the indication
 *          was successfully received.
 *
 *          The ATT Handle Value Indication is used in this sub-procedure.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive an tmos GATT_MSG_EVENT message.
 *          The type of the message will be ATT_HANDLE_VALUE_CFM.
 *
 *          Note: This sub-procedure is complete when ATT_HANDLE_VALUE_CFM
 *                (with SUCCESS or bleTimeout status) is received by the 
 *                calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pInd - pointer to indication to be sent
 * @param   authenticated - whether an authenticated link is required
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Indication was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A confirmation is pending with this client.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_Indication( uint16 connHandle, attHandleValueInd_t *pInd, 
                                  uint8 authenticated, uint8 taskId );
/**
 * @brief   This sub-procedure is used when a server is configured to 
 *          notify a characteristic value to a client without expecting
 *          any attribute protocol layer acknowledgement that the 
 *          notification was successfully received.
 *
 *          The ATT Handle Value Notification is used in this sub-procedure.
 *
 *          Note: A notification may be sent at any time and does not
 *          invoke a confirmation.
 *
 *          No confirmation will be sent to the calling application task for
 *          this sub-procedure.
 *
 * @param   connHandle - connection to use
 * @param   pNoti - pointer to notification to be sent
 * @param   authenticated - whether an authenticated link is required
 *
 * @return  SUCCESS: Notification was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_Notification( uint16 connHandle, attHandleValueNoti_t *pNoti,
                                    uint8 authenticated );

/**
 * @brief   This sub-procedure is used by the client to set the ATT_MTU
 *          to the maximum possible value that can be supported by both
 *          devices when the client supports a value greater than the
 *          default ATT_MTU for the Attribute Protocol. This sub-procedure
 *          shall only be initiated once during a connection.
 *
 *          The ATT Exchange MTU Request is used by this sub-procedure.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive an tmos GATT_MSG_EVENT message.
 *          The type of the message will be either ATT_EXCHANGE_MTU_RSP or
 *          ATT_ERROR_RSP (if an error occurred on the server).
 *
 *          Note: This sub-procedure is complete when either ATT_EXCHANGE_MTU_RSP
 *                (with SUCCESS or bleTimeout status) or ATT_ERROR_RSP (with
 *                SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_ExchangeMTU( uint16 connHandle, attExchangeMTUReq_t *pReq, uint8 taskId );

/**
 * @brief   This sub-procedure is used by a client to discover all 
 *          the primary services on a server.
 *
 *          The ATT Read By Group Type Request is used with the Attribute
 *          Type parameter set to the UUID for "Primary Service". The
 *          Starting Handle is set to 0x0001 and the Ending Handle is
 *          set to 0xFFFF.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_READ_BY_GRP_TYPE_RSP
 *          or ATT_ERROR_RSP (if an error occurred on the server).
 *
 *          Note: This sub-procedure is complete when either ATT_READ_BY_GRP_TYPE_RSP
 *                (with bleProcedureComplete or bleTimeout status) or ATT_ERROR_RSP
 *                (with SUCCESS status) is received by the calling application
 *                task.
 *
 * @param   connHandle - connection to use
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_DiscAllPrimaryServices( uint16 connHandle, uint8 taskId );

/**
 * @brief   This sub-procedure is used by a client to discover a specific
 *          primary service on a server when only the Service UUID is
 *          known. The primary specific service may exist multiple times 
 *          on a server. The primary service being discovered is identified
 *          by the service UUID.
 *
 *          The ATT Find By Type Value Request is used with the Attribute
 *          Type parameter set to the UUID for "Primary Service" and the
 *          Attribute Value set to the 16-bit Bluetooth UUID or 128-bit
 *          UUID for the specific primary service. The Starting Handle shall
 *          be set to 0x0001 and the Ending Handle shall be set to 0xFFFF.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_FIND_BY_TYPE_VALUE_RSP
 *          or ATT_ERROR_RSP (if an error occurred on the server).
 *
 *          Note: This sub-procedure is complete when either ATT_FIND_BY_TYPE_VALUE_RSP
 *                (with bleProcedureComplete or bleTimeout status) or ATT_ERROR_RSP
 *                (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pUUID - pointer to service UUID to look for
 * @param   len - length of value
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_DiscPrimaryServiceByUUID( uint16 connHandle, uint8 *pUUID,
                                                uint8 len, uint8 taskId );
/**
 * @brief   This sub-procedure is used by a client to find include
 *          service declarations within a service definition on a 
 *          server. The service specified is identified by the service
 *          handle range.
 *
 *          The ATT Read By Type Request is used with the Attribute
 *          Type parameter set to the UUID for "Included Service". The
 *          Starting Handle is set to starting handle of the specified
 *          service and the Ending Handle is set to the ending handle
 *          of the specified service.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_READ_BY_TYPE_RSP
 *          or ATT_ERROR_RSP (if an error occurred on the server).
 *
 *          Note: This sub-procedure is complete when either ATT_READ_BY_TYPE_RSP
 *                (with bleProcedureComplete or bleTimeout status) or ATT_ERROR_RSP
 *                (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   startHandle - starting handle
 * @param   endHandle - end handle
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_FindIncludedServices( uint16 connHandle, uint16 startHandle,
                                            uint16 endHandle, uint8 taskId );
/**
 * @brief   This sub-procedure is used by a client to find all the 
 *          characteristic declarations within a service definition on
 *          a server when only the service handle range is known. The
 *          service specified is identified by the service handle range.
 *
 *          The ATT Read By Type Request is used with the Attribute Type
 *          parameter set to the UUID for "Characteristic". The Starting
 *          Handle is set to starting handle of the specified service and
 *          the Ending Handle is set to the ending handle of the specified
 *          service.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_READ_BY_TYPE_RSP
 *          or ATT_ERROR_RSP (if an error occurred on the server).
 *
 *          Note: This sub-procedure is complete when either ATT_READ_BY_TYPE_RSP
 *                (with bleProcedureComplete or bleTimeout status) or ATT_ERROR_RSP
 *                (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   startHandle - starting handle
 * @param   endHandle - end handle
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_DiscAllChars( uint16 connHandle, uint16 startHandle,
                                    uint16 endHandle, uint8 taskId );
/**
 * @brief   This sub-procedure is used by a client to discover service 
 *          characteristics on a server when only the service handle 
 *          ranges are known and the characteristic UUID is known. 
 *          The specific service may exist multiple times on a server.
 *          The characteristic being discovered is identified by the 
 *          characteristic UUID.
 *
 *          The ATT Read By Type Request is used with the Attribute Type
 *          is set to the UUID for "Characteristic" and the Starting
 *          Handle and Ending Handle parameters is set to the service
 *          handle range.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_READ_BY_TYPE_RSP
 *          or ATT_ERROR_RSP (if an error occurred on the server).
 *
 *          Note: This sub-procedure is complete when either ATT_READ_BY_TYPE_RSP
 *                (with bleProcedureComplete or bleTimeout status) or ATT_ERROR_RSP
 *                (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_DiscCharsByUUID( uint16 connHandle, attReadByTypeReq_t *pReq, uint8 taskId );

/**
 * @brief   This sub-procedure is used by a client to find all the 
 *          characteristic descriptor抯 Attribute Handles and Attribute
 *          Types within a characteristic definition when only the 
 *          characteristic handle range is known. The characteristic 
 *          specified is identified by the characteristic handle range.
 *
 *          The ATT Find Information Request is used with the Starting
 *          Handle set to starting handle of the specified characteristic
 *          and the Ending Handle set to the ending handle of the specified
 *          characteristic. The UUID Filter parameter is NULL (zero length).
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_FIND_INFO_RSP or
 *          ATT_ERROR_RSP (if an error occurred on the server).
 *
 *          Note: This sub-procedure is complete when either ATT_FIND_INFO_RSP
 *                (with bleProcedureComplete or bleTimeout status) or ATT_ERROR_RSP
 *                (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   startHandle - starting handle
 * @param   endHandle - end handle
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_DiscAllCharDescs( uint16 connHandle, uint16 startHandle,
                                        uint16 endHandle, uint8 taskId );
/**
 * @brief   This sub-procedure is used to read a Characteristic Value
 *          from a server when the client knows the Characteristic Value
 *          Handle. The ATT Read Request is used with the Attribute Handle
 *          parameter set to the Characteristic Value Handle. The Read 
 *          Response returns the Characteristic Value in the Attribute
 *          Value parameter.
 *
 *          The Read Response only contains a Characteristic Value that
 *          is less than or equal to (ATT_MTU ?1) octets in length. If 
 *          the Characteristic Value is greater than (ATT_MTU ?1) octets
 *          in length, the Read Long Characteristic Value procedure may
 *          be used if the rest of the Characteristic Value is required.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive an tmos GATT_MSG_EVENT message.
 *          The type of the message will be either ATT_READ_RSP or
 *          ATT_ERROR_RSP (if an error occurred on the server).
 *
 *          Note: This sub-procedure is complete when either ATT_READ_RSP
 *                (with SUCCESS or bleTimeout status) or ATT_ERROR_RSP (with
 *                SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_ReadCharValue( uint16 connHandle, attReadReq_t *pReq, uint8 taskId );

/**
 * @brief   This sub-procedure is used to read a Characteristic Value
 *          from a server when the client only knows the characteristic
 *          UUID and does not know the handle of the characteristic.
 *
 *          The ATT Read By Type Request is used to perform the sub-procedure.
 *          The Attribute Type is set to the known characteristic UUID and
 *          the Starting Handle and Ending Handle parameters shall be set
 *          to the range over which this read is to be performed. This is
 *          typically the handle range for the service in which the 
 *          characteristic belongs.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive an tmos GATT_MSG_EVENT messages.
 *          The type of the message will be either ATT_READ_BY_TYPE_RSP
 *          or ATT_ERROR_RSP (if an error occurred on the server).
 *
 *          Note: This sub-procedure is complete when either ATT_READ_BY_TYPE_RSP
 *                (with SUCCESS or bleTimeout status) or ATT_ERROR_RSP (with
 *                SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_ReadUsingCharUUID( uint16 connHandle, attReadByTypeReq_t *pReq, uint8 taskId );
/**
 * @brief   This sub-procedure is used to read a Characteristic Value from
 *          a server when the client knows the Characteristic Value Handle
 *          and the length of the Characteristic Value is longer than can 
 *          be sent in a single Read Response Attribute Protocol message.
 *
 *          The ATT Read Blob Request is used in this sub-procedure.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_READ_BLOB_RSP or
 *          ATT_ERROR_RSP (if an error occurred on the server).
 *
 *          Note: This sub-procedure is complete when either ATT_READ_BLOB_RSP
 *                (with bleProcedureComplete or bleTimeout status) or ATT_ERROR_RSP
 *                (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_ReadLongCharValue( uint16 connHandle, attReadBlobReq_t *pReq, uint8 taskId );

/**
 * @brief   This sub-procedure is used to read multiple Characteristic Values
 *          from a server when the client knows the Characteristic Value 
 *          Handles. The Attribute Protocol Read Multiple Requests is used 
 *          with the Set Of Handles parameter set to the Characteristic Value
 *          Handles. The Read Multiple Response returns the Characteristic
 *          Values in the Set Of Values parameter.
 *
 *          The ATT Read Multiple Request is used in this sub-procedure.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive an tmos GATT_MSG_EVENT message.
 *          The type of the message will be either ATT_READ_MULTI_RSP
 *          or ATT_ERROR_RSP (if an error occurred on the server).
 *
 *          Note: This sub-procedure is complete when either ATT_READ_MULTI_RSP
 *                (with SUCCESS or bleTimeout status) or ATT_ERROR_RSP (with
 *                SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_ReadMultiCharValues( uint16 connHandle, attReadMultiReq_t *pReq, uint8 taskId );

/**
 * @brief   This sub-procedure is used to write a Characteristic Value
 *          to a server when the client knows the Characteristic Value
 *          Handle and the client does not need an acknowledgement that
 *          the write was successfully performed. This sub-procedure
 *          only writes the first (ATT_MTU ?3) octets of a Characteristic
 *          Value. This sub-procedure can not be used to write a long
 *          characteristic; instead the Write Long Characteristic Values
 *          sub-procedure should be used.
 * 
 *          The ATT Write Command is used for this sub-procedure. The
 *          Attribute Handle parameter shall be set to the Characteristic
 *          Value Handle. The Attribute Value parameter shall be set to
 *          the new Characteristic Value.
 *
 *          No response will be sent to the calling application task for this
 *          sub-procedure. If the Characteristic Value write request is the 
 *          wrong size, or has an invalid value as defined by the profile, 
 *          then the write will not succeed and no error will be generated 
 *          by the server.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to command to be sent
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_WriteNoRsp( uint16 connHandle, attWriteReq_t *pReq );

/**
 * @brief   This sub-procedure is used to write a Characteristic Value
 *          to a server when the client knows the Characteristic Value 
 *          Handle and the ATT Bearer is not encrypted. This sub-procedure
 *          shall only be used if the Characteristic Properties authenticated
 *          bit is enabled and the client and server device share a bond as
 *          defined in the GAP.
 *
 *          This sub-procedure only writes the first (ATT_MTU ?15) octets
 *          of an Attribute Value. This sub-procedure cannot be used to 
 *          write a long Attribute.
 *
 *          The ATT Write Command is used for this sub-procedure. The
 *          Attribute Handle parameter shall be set to the Characteristic
 *          Value Handle. The Attribute Value parameter shall be set to 
 *          the new Characteristic Value authenticated by signing the
 *          value, as defined in the Security Manager.
 *
 *          No response will be sent to the calling application task for this
 *          sub-procedure. If the authenticated Characteristic Value that is 
 *          written is the wrong size, or has an invalid value as defined by 
 *          the profile, or the signed value does not authenticate the client,
 *          then the write will not succeed and no error will be generated by
 *          the server.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to command to be sent
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleLinkEncrypted: Connection is already encrypted.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_SignedWriteNoRsp( uint16 connHandle, attWriteReq_t *pReq );

/**
 * @brief   This sub-procedure is used to write a characteristic value
 *          to a server when the client knows the characteristic value
 *          handle. This sub-procedure only writes the first (ATT_MTU-3)
 *          octets of a characteristic value. This sub-procedure can not
 *          be used to write a long attribute; instead the Write Long 
 *          Characteristic Values sub-procedure should be used.
 *
 *          The ATT Write Request is used in this sub-procedure. The 
 *          Attribute Handle parameter shall be set to the Characteristic
 *          Value Handle. The Attribute Value parameter shall be set to
 *          the new characteristic.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive an tmos GATT_MSG_EVENT message.
 *          The type of the message will be either ATT_WRITE_RSP
 *          or ATT_ERROR_RSP (if an error occurred on the server).
 *
 *          Note: This sub-procedure is complete when either ATT_WRITE_RSP
 *                (with SUCCESS or bleTimeout status) or ATT_ERROR_RSP (with
 *                SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_WriteCharValue( uint16 connHandle, attWriteReq_t *pReq, uint8 taskId );

/**
 * @brief   This sub-procedure is used to write a Characteristic Value to
 *          a server when the client knows the Characteristic Value Handle
 *          but the length of the Characteristic Value is longer than can 
 *          be sent in a single Write Request Attribute Protocol message.
 *
 *          The ATT Prepare Write Request and Execute Write Request are
 *          used to perform this sub-procedure.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_PREPARE_WRITE_RSP,
 *          ATT_EXECUTE_WRITE_RSP or ATT_ERROR_RSP (if an error occurred on
 *          the server).
 *
 *          Note: This sub-procedure is complete when either ATT_PREPARE_WRITE_RSP
 *                (with bleTimeout status), ATT_EXECUTE_WRITE_RSP (with SUCCESS
 *                or bleTimeout status), or ATT_ERROR_RSP (with SUCCESS status)
 *                is received by the calling application task.
 *
 *          Note: The 'pReq->pValue' pointer will be freed when the sub-procedure
 *                is complete. 
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_WriteLongCharValue( uint16 connHandle, attPrepareWriteReq_t *pReq, uint8 taskId );

/**
 * @brief   This sub-procedure is used to write a Characteristic Value to
 *          a server when the client knows the Characteristic Value Handle,
 *          and assurance is required that the correct Characteristic Value
 *          is going to be written by transferring the Characteristic Value
 *          to be written in both directions before the write is performed.
 *          This sub-procedure can also be used when multiple values must 
 *          be written, in order, in a single operation.
 *
 *          The sub-procedure has two phases, the first phase prepares the
 *          characteristic values to be written.  Once this is complete, 
 *          the second phase performs the execution of all of the prepared
 *          characteristic value writes on the server from this client.
 *
 *          In the first phase, the ATT Prepare Write Request is used.
 *          In the second phase, the attribute protocol Execute Write
 *          Request is used.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_PREPARE_WRITE_RSP,
 *          ATT_EXECUTE_WRITE_RSP or ATT_ERROR_RSP (if an error occurred on
 *          the server).
 *
 *          Note: This sub-procedure is complete when either ATT_PREPARE_WRITE_RSP
 *                (with bleTimeout status), ATT_EXECUTE_WRITE_RSP (with SUCCESS
 *                or bleTimeout status), or ATT_ERROR_RSP (with SUCCESS status)
 *                is received by the calling application task. 
 *
 *          Note: The 'pReqs' pointer will be freed when the sub-procedure is
 *                complete. 
 *
 * @param   connHandle - connection to use
 * @param   pReqs - pointer to requests to be sent (must be allocated)
 * @param   numReqs - number of requests in pReq
 * @param   flags - execute write request flags
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_ReliableWrites( uint16 connHandle, attPrepareWriteReq_t *pReqs, 
                                      uint8 numReqs, uint8 flags, uint8 taskId );
/**
 * @brief   This sub-procedure is used to read a characteristic descriptor
 *          from a server when the client knows the characteristic descriptor
 *          declaration抯 Attribute handle.
 *
 *          The ATT Read Request is used for this sub-procedure. The Read
 *          Request is used with the Attribute Handle parameter set to the 
 *          characteristic descriptor handle. The Read Response returns the
 *          characteristic descriptor value in the Attribute Value parameter.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive an tmos GATT_MSG_EVENT message.
 *          The type of the message will be either ATT_READ_RSP or
 *          ATT_ERROR_RSP (if an error occurred on the server).
 *
 *          Note: This sub-procedure is complete when either ATT_READ_RSP
 *                (with SUCCESS or bleTimeout status) or ATT_ERROR_RSP (with
 *                SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_ReadCharDesc( uint16 connHandle, attReadReq_t *pReq, uint8 taskId );

/**
 * @brief   This sub-procedure is used to read a characteristic descriptor
 *          from a server when the client knows the characteristic descriptor
 *          declaration抯 Attribute handle and the length of the characteristic
 *          descriptor declaration is longer than can be sent in a single Read
 *          Response attribute protocol message.
 *
 *          The ATT Read Blob Request is used to perform this sub-procedure.
 *          The Attribute Handle parameter shall be set to the characteristic
 *          descriptor handle. The Value Offset parameter shall be the offset
 *          within the characteristic descriptor to be read.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_READ_BLOB_RSP or
 *          ATT_ERROR_RSP (if an error occurred on the server).
 *
 *          Note: This sub-procedure is complete when either ATT_READ_BLOB_RSP
 *                (with bleProcedureComplete or bleTimeout status) or ATT_ERROR_RSP
 *                (with SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_ReadLongCharDesc( uint16 connHandle, attReadBlobReq_t *pReq, uint8 taskId );

/**
 * @brief   This sub-procedure is used to write a characteristic
 *          descriptor value to a server when the client knows the
 *          characteristic descriptor handle.
 *
 *          The ATT Write Request is used for this sub-procedure. The
 *          Attribute Handle parameter shall be set to the characteristic
 *          descriptor handle. The Attribute Value parameter shall be
 *          set to the new characteristic descriptor value.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive an tmos GATT_MSG_EVENT message.
 *          The type of the message will be either ATT_WRITE_RSP
 *          or ATT_ERROR_RSP (if an error occurred on the server).
 *
 *          Note: This sub-procedure is complete when either ATT_WRITE_RSP
 *                (with SUCCESS or bleTimeout status) or ATT_ERROR_RSP (with
 *                SUCCESS status) is received by the calling application task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_WriteCharDesc( uint16 connHandle, attWriteReq_t *pReq, uint8 taskId );

/**
 * @brief   This sub-procedure is used to write a Characteristic Value to
 *          a server when the client knows the Characteristic Value Handle
 *          but the length of the Characteristic Value is longer than can 
 *          be sent in a single Write Request Attribute Protocol message.
 *
 *          The ATT Prepare Write Request and Execute Write Request are
 *          used to perform this sub-procedure.
 *
 *          If the return status from this function is SUCCESS, the calling
 *          application task will receive multiple tmos GATT_MSG_EVENT messages.
 *          The type of the messages will be either ATT_PREPARE_WRITE_RSP,
 *          ATT_EXECUTE_WRITE_RSP or ATT_ERROR_RSP (if an error occurred on
 *          the server).
 *
 *          Note: This sub-procedure is complete when either ATT_PREPARE_WRITE_RSP
 *                (with bleTimeout status), ATT_EXECUTE_WRITE_RSP (with SUCCESS
 *                or bleTimeout status), or ATT_ERROR_RSP (with SUCCESS status)
 *                is received by the calling application task.
 *
 *          Note: The 'pReq->pValue' pointer will be freed when the sub-procedure
 *                is complete.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  SUCCESS: Request was sent successfully.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.v
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A response is pending with this server.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleTimeout: Previous transaction timed out.<BR>
 */
extern bStatus_t GATT_WriteLongCharDesc( uint16 connHandle, attPrepareWriteReq_t *pReq, uint8 taskId );

/**
 * @brief   Register your task ID to receive GATT local events and ATT Response
 *          messages pending for transmission. When the GATT Server fails to
 *          respond to an incoming ATT Request due to lack of HCI Tx buffers,
 *          the response will be forwarded the app for retransmission.
 *
 *          Note: A pending ATT Response message forwarded to the app should be
 *                transmitted using GATT_SendRsp(), or its payload freed using
 *                GATT_bm_free() if not transmitted.
 *
 * @param   taskID - Default task ID to send GATT events/pending ATT responses.
 *
 * @return  none
 */
extern void GATT_RegisterForMsgs( uint8 taskID );

/**
 * @brief   Send an event to upper layer application/protocol.
 *
 * @param   connHandle - connection event belongs to
 * @param   status - status
 * @param   method - type of message
 * @param   pMsg - pointer to message to be sent
 *
 * @return  SUCCESS, INVALIDPARAMETER or bleMemAllocError
 */
extern bStatus_t GATT_NotifyEvent( uint16 connHandle, uint8 status, uint8 method, gattMsg_t *pMsg );

/**
 * @brief   GATT implementation of the allocator functionality.
 *
 *          Note: This function should only be called by GATT and
 *                the upper layer protocol/application.
 *
 * @param   connHandle - connection that message is to be sent on.
 * @param   opcode - opcode of message that buffer to be allocated for.
 * @param   size - number of bytes to allocate from the heap.
 * @param   pSizeAlloc - number of bytes allocated for the caller from the heap.
 * @param   flag - .
 *
 * @return  pointer to the heap allocation; NULL if error or failure.
 */
extern void *GATT_bm_alloc( uint16 connHandle, uint8 opcode, uint16 size, uint16 *pSizeAlloc, u8 flag );

/**
 * @brief   GATT implementation of the de-allocator functionality.
 *
 * @param   pMsg - pointer to GATT message containing the memory to free.
 * @param   opcode - opcode of the message
 *
 * @return  none
 */
extern void GATT_bm_free( gattMsg_t *pMsg, uint8 opcode );

/*-------------------------------------------------------------------
 * TASK API - These functions must only be called by tmos.
 */

/**
 * @internal
 *
 * @brief   GATT Task initialization function.
 *
 * @param   taskId - GATT task ID.
 *
 * @return  void
 */
extern void GATT_Init( uint8 taskId );

/**
 * @internal
 *
 * @brief   GATT Task event processing function.
 *
 * @param   taskId - GATT task ID
 * @param   events - GATT events.
 *
 * @return  events not processed
 */
extern uint16 GATT_ProcessEvent( uint8 taskId, uint16 events );

/**
 * @brief   Register your task ID to receive event messages
 *          from the GATT Server Application.
 *
 * @param   taskID - Default task ID to send events.
 *
 * @return  none
 */
extern void GATTServApp_RegisterForMsg( uint8 taskID );

/**
 * @brief   Register a service's attribute list and callback functions with
 *          the GATT Server Application.
 *
 * @param   pAttrs - Array of attribute records to be registered
 * @param   numAttrs - Number of attributes in array
 * @param   encKeySize - Minimum encryption key size required by service (7-16 bytes)
 * @param   pServiceCBs - Service callback function pointers
 *
 * @return  SUCCESS: Service registered successfully.<BR>
 *          INVALIDPARAMETER: Invalid service fields.<BR>
 *          FAILURE: Not enough attribute handles available.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 *          bleInvalidRange: Encryption key size's out of range.<BR>
 */
extern bStatus_t GATTServApp_RegisterService( gattAttribute_t *pAttrs,
                                              uint16 numAttrs, uint8 encKeySize,
                                              gattServiceCBs_t *pServiceCBs );
/**
 * @brief   Deregister a service's attribute list and callback functions from
 *          the GATT Server Application.
 *
 *          NOTE: It's the caller's responsibility to free the service attribute
 *          list returned from this API.
 *
 * @param   handle - handle of service to be deregistered
 * @param   p2pAttrs - pointer to array of attribute records (to be returned)
 *
 * @return  SUCCESS: Service deregistered successfully.<BR>
 *          FAILURE: Service not found.<BR>
 */
bStatus_t GATTServApp_DeregisterService( uint16 handle, gattAttribute_t **p2pAttrs );

/**
 * @brief       Find the attribute record within a service attribute
 *              table for a given attribute value pointer.
 *
 * @param       pAttrTbl - pointer to attribute table
 * @param       numAttrs - number of attributes in attribute table
 * @param       pValue - pointer to attribute value
 *
 * @return      Pointer to attribute record. NULL, if not found.
 */
extern gattAttribute_t *GATTServApp_FindAttr( gattAttribute_t *pAttrTbl,
                                              uint16 numAttrs, uint8 *pValue );
/**
 * @brief   Add function for the GATT Service.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  SUCCESS: Service added successfully.<BR>
 *          INVALIDPARAMETER: Invalid service field.<BR>
 *          FAILURE: Not enough attribute handles available.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 */
extern bStatus_t GATTServApp_AddService( uint32 services );

/**
 * @brief   Delete function for the GATT Service.
 *
 * @param   services - services to delete. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  SUCCESS: Service deleted successfully.<BR>
 *          FAILURE: Service not found.<BR>
 */
extern bStatus_t GATTServApp_DelService( uint32 services );

/**
 * @brief   Set a GATT Server parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to right
 * @param   pValue - pointer to data to write. This is dependent on the
 *                   parameter ID and WILL be cast to the appropriate
 *                   data type (example: data type of uint16 will be cast
 *                   to uint16 pointer).
 *
 * @return  SUCCESS: Parameter set successful
 *          FAILURE: Parameter in use
 *          INVALIDPARAMETER: Invalid parameter
 *          bleInvalidRange: Invalid value
 *          bleMemAllocError: Memory allocation failed
 */
extern bStatus_t GATTServApp_SetParameter( uint8 param, uint8 len, void *pValue );

/**
 * @brief   Get a GATT Server parameter.
 *
 * @param   param - Profile parameter ID
 * @param   pValue - pointer to data to put. This is dependent on the
 *                   parameter ID and WILL be cast to the appropriate
 *                   data type (example: data type of uint16 will be
 *                   cast to uint16 pointer).
 *
 * @return  SUCCESS: Parameter get successful
 *          INVALIDPARAMETER: Invalid parameter
 */
extern bStatus_t GATTServApp_GetParameter( uint8 param, void *pValue );

/**
 * @brief   Update the Client Characteristic Configuration for a given
 *          Client.
 *
 *          Note: This API should only be called from the Bond Manager.
 *
 * @param   connHandle - connection handle.
 * @param   attrHandle - attribute handle.
 * @param   value - characteristic configuration value.
 *
 * @return  SUCCESS: Parameter get successful
 *          INVALIDPARAMETER: Invalid parameter
 */
extern bStatus_t GATTServApp_UpdateCharCfg( uint16 connHandle, uint16 attrHandle, uint16 value );

/**
 * @brief   Initialize the client characteristic configuration table.
 *
 *          Note: Each client has its own instantiation of the Client
 *                Characteristic Configuration. Reads/Writes of the Client
 *                Characteristic Configuration only only affect the
 *                configuration of that client.
 *
 * @param   connHandle - connection handle (0xFFFF for all connections).
 * @param   charCfgTbl - client characteristic configuration table.
 *
 * @return  none
 */
extern void GATTServApp_InitCharCfg( uint16 connHandle, gattCharCfg_t *charCfgTbl );

/**
 * @brief   Read the client characteristic configuration for a given
 *          client.
 *
 *          Note: Each client has its own instantiation of the Client
 *                Characteristic Configuration. Reads of the Client
 *                Characteristic Configuration only shows the configuration
 *                for that client.
 *
 * @param   connHandle - connection handle.
 * @param   charCfgTbl - client characteristic configuration table.
 *
 * @return  attribute value
 */
extern uint16 GATTServApp_ReadCharCfg( uint16 connHandle, gattCharCfg_t *charCfgTbl );

/**
 * @brief   Write the client characteristic configuration for a given
 *          client.
 *
 *          Note: Each client has its own instantiation of the Client
 *                Characteristic Configuration. Writes of the Client
 *                Characteristic Configuration only only affect the
 *                configuration of that client.
 *
 * @param   connHandle - connection handle.
 * @param   charCfgTbl - client characteristic configuration table.
 * @param   value - attribute new value.
 *
 * @return  Success or Failure
 */
extern uint8 GATTServApp_WriteCharCfg( uint16 connHandle, gattCharCfg_t *charCfgTbl, uint16 value );

/**
 * @brief   Process the client characteristic configuration
 *          write request for a given client.
 *
 * @param   connHandle - connection message was received on.
 * @param   pAttr - pointer to attribute.
 * @param   pValue - pointer to data to be written.
 * @param   len - length of data.
 * @param   offset - offset of the first octet to be written.
 * @param   validCfg - valid configuration.
 *
 * @return  Success or Failure
 */
extern bStatus_t GATTServApp_ProcessCCCWriteReq( uint16 connHandle, gattAttribute_t *pAttr,
                                          uint8 *pValue, uint16 len, uint16 offset,
                                          uint16 validCfg );

/**
 * @brief   Process Client Characteristic Configuration change.
 *
 * @param   charCfgTbl - characteristic configuration table.
 * @param   pValue - pointer to attribute value.
 * @param   authenticated - whether an authenticated link is required.
 * @param   attrTbl - attribute table.
 * @param   numAttrs - number of attributes in attribute table.
 * @param   taskId - task to be notified of confirmation.
 * @param   pfnReadAttrCB - read callback function pointer.
 *
 * @return  Success or Failure
 */
extern bStatus_t GATTServApp_ProcessCharCfg( gattCharCfg_t *charCfgTbl, uint8 *pValue,
                                        uint8 authenticated, gattAttribute_t *attrTbl,
                                        uint16 numAttrs, uint8 taskId,
                                        pfnGATTReadAttrCB_t pfnReadAttrCB );

/**
 * @brief   Build and send the GATT_CLIENT_CHAR_CFG_UPDATED_EVENT to
 *          the application.
 *
 * @param   connHandle - connection handle
 * @param   attrHandle - attribute handle
 * @param   value - attribute new value
 *
 * @return  none
 */
extern void GATTServApp_SendCCCUpdatedEvent( uint16 connHandle, uint16 attrHandle, uint16 value );

/**
 * @brief   Send out a Service Changed Indication.
 *
 * @param   connHandle - connection to use
 * @param   taskId - task to be notified of confirmation
 *
 * @return  SUCCESS: Indication was sent successfully.<BR>
 *          FAILURE: Service Changed attribute not found.<BR>
 *          INVALIDPARAMETER: Invalid connection handle or request field.<BR>
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.<BR>
 *          bleNotConnected: Connection is down.<BR>
 *          blePending: A confirmation is pending with this client.<BR>
 */
extern bStatus_t GATTServApp_SendServiceChangedInd( uint16 connHandle, uint8 taskId );

/**
 * @brief       Read an attribute. If the format of the attribute value
 *              is unknown to GATT Server, use the callback function
 *              provided by the Service.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       service - handle of owner service
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read
 *
 * @return      Success or Failure
 */
extern bStatus_t GATTServApp_ReadAttr( uint16 connHandle, gattAttribute_t *pAttr,
                                   uint16 service, uint8 *pValue, uint16 *pLen,
                                   uint16 offset, uint16 maxLen, uint8 method );

/**
 * @brief   Write attribute data
 *
 * @param   connHandle - connection message was received on
 * @param   handle - attribute handle
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write
 *
 * @return  Success or Failure
 */
extern bStatus_t GATTServApp_WriteAttr( uint16 connHandle, uint16 handle,
                                    uint8 *pValue, uint16 len, uint16 offset,
                                    uint8 method );

extern void GATTServApp_Init( tmosTaskID taskId );

extern tmosEvents GATTServApp_ProcessEvent( tmosTaskID taskId, tmosEvents events);

  /**
   * @internal
   *
   * @brief       SM Task Initialization Function.
   *
   * @param       taskID - SM task ID.
   *
   * @return      void
   */
  extern void SM_Init( uint8 task_id );

  /**
   * @internal
   *
   * @brief       SM Task event processing function.
   *
   * @param       taskID - SM task ID
   * @param       events - SM events.
   *
   * @return      events not processed
   */
  extern uint16 SM_ProcessEvent( uint8 task_id, uint16 events );


/**
 * @brief   Set a GAP GATT Server parameter.
 *
 * @param   param - Profile parameter ID<BR>
 * @param   len - length of data to right
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).<BR>
 *
 * @return  bStatus_t
 */
extern bStatus_t GGS_SetParameter( uint8 param, uint8 len, void *value );

/**
 * @brief   Get a GAP GATT Server parameter.
 *
 * @param   param - Profile parameter ID<BR>
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).<BR>
 *
 * @return  bStatus_t
 */
extern bStatus_t GGS_GetParameter( uint8 param, void *value );

/**
 * @brief   Add function for the GAP GATT Service.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  SUCCESS: Service added successfully.<BR>
 *          INVALIDPARAMETER: Invalid service field.<BR>
 *          FAILURE: Not enough attribute handles available.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 */
extern bStatus_t GGS_AddService( uint32 services );

/*-------------------------------------------------------------------
 * FUNCTIONS - Device Discovery
 */
/*******************************************************************************
 * @fn          GAP_ResolvePrivateAddr
 *
 * @brief       Resolves a private address against an IRK.
 *
 * input parameters
 *
 * @param       pIRK - pointer to the IRK
 * @param       pAddr - pointer to the Resolvable Private address
 *
 * output parameters
 *
 * @param       pIRK
 * @param       pAddr
 *
 * @return      SUCCESS: match,<BR>
 *              FAILURE: don't match,<BR>
 *              INVALIDPARAMETER: parameters invalid<BR>
 */
extern bStatus_t GAP_ResolvePrivateAddr( u8 *pIRK, u8 *pAddr );

/*-------------------------------------------------------------------
 * FUNCTIONS - Link Establishment
 */

/*******************************************************************************
 * @fn          GAP_UpdateLinkParamReq
 *
 * @brief       Update the link parameters to a Master or Slave device.
 *
 * input parameters
 *
 * @param       pParams - link update parameters
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS: started update link process,<BR
 *              INVALIDPARAMETER: one of the parameters were invalid,<BR>
 *              bleIncorrectMode: invalid profile role,<BR>
 *              bleAlreadyInRequestedMode: already updating link parameters,<BR>
 *              bleNotConnected: not in a connection<BR>
 */
 extern bStatus_t GAP_UpdateLinkParamReq( gapUpdateLinkParamReq_t *pParams );

/*******************************************************************************
 * @fn          GAP_NumActiveConnections
 *
 * @brief       Returns the number of active connections.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Number of active connections.
 */
 extern uint8 GAP_NumActiveConnections( void );

/*-------------------------------------------------------------------
 * FUNCTIONS - Pairing
 */
/*******************************************************************************
 * @fn          GAP_PasscodeUpdate
 *
 * @brief       Update the passkey in a numeric value (not string).
 *              This function is called by the application/profile in response
 *              to receiving the GAP_PASSKEY_NEEDED_EVENT message.
 *
 * NOTE:        This function is the same as GAP_PasskeyUpdate(), except that
 *              the passkey is passed in as a non-string format.
 *
 * input parameters
 *
 * @param       passcode - not string - range: 0 - 999,999.
 * @param       connectionHandle - connection handle.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS: will start pairing with this entry,<BR>
 *              bleIncorrectMode: Link not found,<BR>
 *              INVALIDPARAMETER: passkey == NULL or passkey isn't formatted properly.<BR>
 */
extern bStatus_t GAP_PasscodeUpdate( uint32 passcode, uint16 connectionHandle );

/*******************************************************************************
 * @fn          GAP_PasskeyUpdate
 *
 * @brief       Update the passkey in string format.  This function is called by the
 *              application/profile in response to receiving the
 *              GAP_PASSKEY_NEEDED_EVENT message.
 *
 * NOTE:        This function is the same as GAP_PasscodeUpdate(), except that
 *              the passkey is passed in as a string format.
 *
 * input parameters
 *
 * @param       pPasskey - new passkey - pointer to numeric string (ie. "019655" ).
 *              This string's range is "000000" to "999999".
 * @param       connectionHandle - connection handle.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS: will start pairing with this entry,<BR>
 *              bleIncorrectMode: Link not found,<BR>
 *              INVALIDPARAMETER: passkey == NULL or passkey isn't formatted properly.<BR>
 */
extern bStatus_t GAP_PasskeyUpdate( u8 *pPasskey, u16 connectionHandle );

/*******************************************************************************
 * @fn          GAP_TerminateAuth
 *
 * @brief       Send a Pairing Failed message and end any existing pairing.
 *
 * input parameters
 *
 * @param       connectionHandle - connection handle.
 * @param       reason - Pairing Failed reason code.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS - function was successful,<BR>
 *              bleMemAllocError - memory allocation error,<BR>
 *              INVALIDPARAMETER - one of the parameters were invalid,<BR>
 *              bleNotConnected - link not found,<BR>
 *              bleInvalidRange - one of the parameters were not within range.
 */
 extern bStatus_t GAP_TerminateAuth( uint16 connectionHandle, uint8 reason );

/*******************************************************************************
 * @fn          GAP_Authenticate
 *
 * @brief       Start the Authentication process with the requested device.
 *              This function is used to Initiate/Allow pairing.
 *              Called by both master and slave device (Central and Peripheral).
 *
 * NOTE:        This function is called after the link is established.
 *
 * input parameters
 *
 * @param       pParams - Authentication parameters
 * @param       pPairReq - Enter these parameters if the Pairing Request was already received.
 *              NULL, if waiting for Pairing Request or if initiating.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS,<BR>
 *              bleIncorrectMode: Not correct profile role,<BR>
 *              INVALIDPARAMETER, <BR>
 *              bleNotConnected,<BR>
 *              bleAlreadyInRequestedMode,<BR>
 *              FAILURE - not workable.<BR>
 */
extern bStatus_t GAP_Authenticate( gapAuthParams_t *pParams, gapPairingReq_t *pPairReq );

/*******************************************************************************
 * @fn          GAP_Signable
 *
 * @brief       Set up the connection to accept signed data.
 *
 * NOTE:        This function is called after the link is established.
 *
 * input parameters
 *
 * @param       connectionHandle - connection handle of the signing information
 * @param       authenticated - TRUE if the signing information is authenticated, FALSE otherwise
 * @param       pParams - signing parameters
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS, <BR>
 *              bleIncorrectMode: Not correct profile role,<BR>
 *              INVALIDPARAMETER, <BR>
 *              bleNotConnected,<BR>
 *              FAILURE: not workable.<BR>
 */
 extern bStatus_t GAP_Signable( u16 connectionHandle, u8 authenticated, smSigningInfo_t *pParams );

/*******************************************************************************
 * @fn          GAP_SendSlaveSecurityRequest
 *
 * @brief       Generate a Slave Requested Security message to the master.
 *
 * input parameters
 *
 * @param       connectionHandle - connection handle.
 * @param       authReq - Authentication Requirements: Bit 2: MITM, Bits 0-1: bonding (0 - no bonding, 1 - bonding)
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS: will send,<BR>
 *              bleNotConnected: Link not found,<BR>
 *              bleIncorrectMode: wrong GAP role, must be a Peripheral Role<BR>
 */
extern bStatus_t GAP_SendSlaveSecurityRequest( uint16 connectionHandle, uint8 authReq );

/*******************************************************************************
 * @fn          GAP_TerminateLinkReq
 *
 * @brief       Terminate a link connection.
 *
 * input parameters
 *
 * @param       taskID - requesting app's task id.
 * @param       connectionHandle - connection handle of link to terminate
 *                  or @ref GAP_CONN_HANDLE_DEFINES
 * @param       reason - terminate reason.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS: Terminate started,<BR>
 *              bleIncorrectMode: No Link to terminate,<BR>
 *              bleInvalidTaskID: not app that established link<BR>
 */
extern bStatus_t GAP_TerminateLinkReq( u8 taskID, u16 connectionHandle, u8 reason);

/*******************************************************************************
 * @fn          GAP_Bond
 *
 * @brief       Set up the connection's bound paramaters.
 *
 * NOTE:        This function is called after the link is established.
 *
 * input parameters
 *
 * @param       connectionHandle - connection handle of the signing information
 * @param       authenticated - this connection was previously authenticated
 * @param       pParams - the connected device's security parameters
 * @param       startEncryption - whether or not to start encryption
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS, <BR>
 *              bleIncorrectMode: Not correct profile role,<BR>
 *              INVALIDPARAMETER, <BR>
 *              bleNotConnected,<BR>
 *              FAILURE: not workable.<BR>
 */
extern bStatus_t GAP_Bond( u16 connectionHandle,
                        	u8 authenticated,
                        	smSecurityInfo_t *pParams,
                        	u8 startEncryption );

/*-------------------------------------------------------------------
 * FUNCTIONS - Initialization and Configuration
 */
/*******************************************************************************
 * @fn          GAP_PasscodeUpdate
 *
 * @brief       Get a GAP Parameter value.
 *
 * NOTE:        This function is the same as GAP_PasskeyUpdate(), except that
 *              the passkey is passed in as a non-string format.
 *
 * input parameters
 *
 * @param       paramID - parameter ID: @ref GAP_PARAMETER_ID_DEFINES
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      GAP Parameter value or 0xFFFF if invalid
 */
extern uint16 GAP_GetParamValue( uint16 paramID );

/*******************************************************************************
 * @fn          GAP_ConfigDeviceAddr
 *
 * @brief       Setup the device's address type.  If ADDRTYPE_PRIVATE_RESOLVE
 *              is selected, the address will change periodically.
 *
 * input parameters
 *
 * @param       addrType - @ref GAP_ADDR_TYPE_DEFINES
 * @param       pStaticAddr - Only used with ADDRTYPE_STATIC
 *                       or ADDRTYPE_PRIVATE_NONRESOLVE type.<BR>
 *                   NULL to auto generate otherwise the application
 *                   can specify the address value
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS: address type updated,<BR>
 *              bleNotReady: Can't be called until GAP_DeviceInit() is called
 *                   and the init process is completed,<BR>
 *              bleIncorrectMode: can't change with an active connection,<BR>
 *               or INVALIDPARAMETER.<BR>
 *
 *              If return value isn't SUCCESS, the address type remains
 *              the same as before this call.
 */
extern bStatus_t GAP_ConfigDeviceAddr( u8 addrType, u8 *pStaticAddr );

/*******************************************************************************
 * @fn          GAP_RegisterForMsgs
 *
 * @brief       Register your task ID to receive extra (unprocessed)
 *              HCI status and complete, and Host events.
 *
 * input parameters
 *
 * @param       taskID - Default task ID to send events.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
extern void GAP_RegisterForMsgs( uint8 taskID );

/*******************************************************************************
 * @fn          GAP_DeviceInit
 *
 * @brief       Called to setup the device.  Call just once on initialization.
 *
 *        NOTE: When initialization is complete, the calling app will be
 *              sent the GAP_DEVICE_INIT_DONE_EVENT
 *
 * @param       taskID - Default task ID to send events.
 * @param       profileRole - GAP Profile Roles: @ref GAP_PROFILE_ROLE_DEFINES
 * @param       maxScanResponses - maximum number to scan responses
 *                we can receive during a device discovery.
 * @param       pIRK - pointer to Identity Root Key, NULLKEY (all zeroes) if the app
 *                wants the GAP to generate the key.
 * @param       pSRK - pointer to Sign Resolving Key, NULLKEY if the app
 *                wants the GAP to generate the key.
 * @param       pSignCounter - 32 bit value used in the SM Signing
 *                algorithm that shall be initialized to zero and incremented
 *                with every new signing. This variable must also be maintained
 *                by the application.
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS - Processing, expect GAP_DEVICE_INIT_DONE_EVENT, <BR>
 *              INVALIDPARAMETER - for invalid profile role or role combination, <BR>
 *              bleIncorrectMode - trouble communicating with HCI
 */
extern bStatus_t GAP_DeviceInit(  u8  taskID,
                                  u8  profileRole,
                                  u8  maxScanResponses,
                                  u8  *pIRK,
                                  u8  *pSRK,
                                  u32 *pSignCounter );

/*******************************************************************************
 * @fn          GAP_SetParamValue
 *
 * @brief       Set a GAP Parameter value.  Use this function to change
 *              the default GAP parameter values.
 *
 * input parameters
 *
 * @param       paramID - parameter ID: @ref GAP_PARAMETER_ID_DEFINES
 * @param       paramValue - new param value
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS or INVALIDPARAMETER (invalid paramID)
 */
extern bStatus_t GAP_SetParamValue( uint16 paramID, u16 paramValue );

/*-------------------------------------------------------------------
 * TASK API - These functions must only be called tmos.
 */
/*******************************************************************************
 * @fn          GAP_Init
 *
 * @brief       GAP Task initialization function.
 *
 * input parameters
 *
 * @param       taskID - GAP task ID.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
extern void GAP_Init( u8 task_id );

/*******************************************************************************
 * @fn          GAP_ProcessEvent
 *
 * @brief       GAP Task event processing function.
 *
 * input parameters
 *
 * @param       taskID - GAP task ID
 * @param       events - GAP events.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      events not processed
 */
extern u16 GAP_ProcessEvent( u8 task_id, u16 events );

/*******************************************************************************
 * @fn          GAP_PeriDevMgrInit
 *
 * @brief       Initialize the GAP Peripheral Dev Manager.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS or bleMemAllocError
 */
extern bStatus_t GAP_PeriDevMgrInit( void );
  
/*******************************************************************************
 * @fn          GAP_PeriConnRegister
 *
 * @brief       Register the GAP Peripheral Connection processing functions.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS or bleMemAllocError
 */
extern void GAP_PeriConnRegister( void );
  
/*******************************************************************************
 * @fn          GAP_UpdateAdvertisingData
 *
 * @brief       Setup or change advertising and scan response data.
 *
 *       NOTE:  if the return status from this function is SUCCESS,
 *              the task isn't complete until the GAP_ADV_DATA_UPDATE_DONE_EVENT
 *              is sent to the calling application task.
 *
 * input parameters
 *
 * @param       taskID - task ID of the app requesting the change
 * @param       adType - TRUE - advertisement data, FALSE  - scan response data
 * @param       dataLen - Octet length of advertData
 * @param       pAdvertData - advertising or scan response data
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS: data accepted,<BR>
 *              bleIncorrectMode: invalid profile role,<BR>
 */
extern bStatus_t GAP_UpdateAdvertisingData( u8 taskID,
                                            u8 adType,
                                            u8 dataLen,
                                            u8 *pAdvertData );

/*******************************************************************************
 * @fn          GAP_EndDiscoverable
 *
 * @brief       Stops advertising.
 *
 * input parameters
 *
 * @param       taskID - of task that called GAP_MakeDiscoverable
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS: stopping discoverable mode,<BR>
 *              bleIncorrectMode: not in discoverable mode,<BR>
 *              bleInvalidTaskID: not correct task<BR>
 */
extern bStatus_t GAP_EndDiscoverable( uint8 taskID );

/*-------------------------------------------------------------------
 * FUNCTIONS - Device Discovery
 */
/*******************************************************************************
 * @fn          GAP_MakeDiscoverable
 *
 * @brief       Setup or change advertising.  Also starts advertising.
 *
 * input parameters
 *
 * @param       taskID - used to return GAP_DISCOVERABLE_RESPONSE_EVENT
 * @param       pParams - advertising parameters
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS: advertising started,<BR>
 *              bleIncorrectMode: invalid profile role,<BR>
 *              bleAlreadyInRequestedMode: not available at this time,<BR>
 *              bleNotReady: advertising data isn't set up yet.<BR>
 */
 extern bStatus_t GAP_MakeDiscoverable( uint8 taskID, gapAdvertisingParams_t *pParams );

/*******************************************************************************
 * @fn          GAP_DeviceDiscoveryCancel
 *
 * @brief       Cancel an existing device discovery request.
 *
 * input parameters
 *
 * @param       taskID - used to return GAP_DEVICE_DISCOVERY_EVENT
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS: cancel started,<BR>
 *              bleInvalidTaskID: Not the task that started discovery,<BR>
 *              bleIncorrectMode: not in discovery mode<BR>
 */
extern bStatus_t GAP_DeviceDiscoveryCancel( uint8 taskID );

/*******************************************************************************
 * @fn          GAP_DeviceDiscoveryRequest
 *
 * @brief       Start a device discovery scan.
 *
 * input parameters
 *
 * @param       pParams - Device Discovery parameters
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS: scan started,<BR>
 *              bleIncorrectMode: invalid profile role,<BR>
 *              bleAlreadyInRequestedMode: not available<BR>
 */
extern bStatus_t GAP_DeviceDiscoveryRequest( gapDevDiscReq_t *pParams );

/*******************************************************************************
 * @fn          GAP_EstablishLinkReq
 *
 * @brief       Establish a link to a slave device.
 *
 * input parameters
 *
 * @param       pParams - link establishment parameters
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS: started establish link process,<BR>
 *              bleIncorrectMode: invalid profile role,<BR>
 *              bleNotReady: a scan is in progress,<BR>
 *              bleAlreadyInRequestedMode: can't process now,<BR>
 *              bleNoResources: Too many links<BR>
 */
 extern bStatus_t GAP_EstablishLinkReq( gapEstLinkReq_t *pParams );

/*******************************************************************************
 * @fn          GAP_CentDevMgrInit
 *
 * @brief       Initialize the GAP Central Dev Manager.
 *
 * input parameters
 *
 * @param       maxScanResponses - maximum number to scan responses
 *              we can receive during a device discovery.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      SUCCESS or bleMemAllocError
 */
extern bStatus_t GAP_CentDevMgrInit( uint8 maxScanResponses );

/**
 * @brief   Start the device in Observer role.  This function is typically
 *          called once during system startup.
 *
 * @param   pAppCallbacks - pointer to application callbacks
 *
 * @return  SUCCESS: Operation successful.<BR>
 *          bleAlreadyInRequestedMode: Device already started.<BR>
 */
extern bStatus_t GAPRole_ObserverStartDevice( gapObserverRoleCB_t *pAppCallbacks );

/**
 * @brief   Start a device discovery scan.
 *
 * @param   mode - discovery mode: @ref GAP_DEVDISC_MODE_DEFINES
 * @param   activeScan - TRUE to perform active scan
 * @param   whiteList - TRUE to only scan for devices in the white list
 *
 * @return  SUCCESS: Discovery scan started.<BR>
 *          bleIncorrectMode: Invalid profile role.<BR>
 *          bleAlreadyInRequestedMode: Not available.<BR>
 */
extern bStatus_t GAPRole_ObserverStartDiscovery( uint8 mode, uint8 activeScan, uint8 whiteList );

/**
 * @brief   Cancel a device discovery scan.
 *
 * @return  SUCCESS: Cancel started.<BR>
 *          bleInvalidTaskID: Not the task that started discovery.<BR>
 *          bleIncorrectMode: Not in discovery mode.<BR>
 */
extern bStatus_t GAPRole_ObserverCancelDiscovery( void );

/*-------------------------------------------------------------------
 * TASK API - These functions must only be called by tmos.
 */

/**
 * @internal
 *
 * @brief   Observer Profile Task initialization function.
 *
 * @param   taskId - Task ID.
 *
 * @return  void
 */
extern void GAPRole_ObserverInit( uint8 taskId );

/**
 * @internal
 *
 * @brief   Observer Profile Task event processing function.
 *
 * @param   taskId - Task ID
 * @param   events - Events.
 *
 * @return  events not processed
 */
extern uint16 GAPRole_ObserverProcessEvent( uint8 taskId, uint16 events );

/**
 *  CENTRAL_PROFILE_API Central Profile API Functions
 *
 */

/**
 * @brief   Start the device in Central role.  This function is typically
 *          called once during system startup.
 *
 * @param   pAppCallbacks - pointer to application callbacks
 *
 * @return  SUCCESS: Operation successful.<BR>
 *          bleAlreadyInRequestedMode: Device already started.<BR>
 */
extern bStatus_t GAPCentralRole_StartDevice( gapCentralRoleCB_t *pAppCallbacks );

/**
 * @brief   Terminate a link.
 *
 * @param   connHandle - connection handle of link to terminate
 *          or @ref GAP_CONN_HANDLE_DEFINES
 *
 * @return  SUCCESS: Terminate started.<BR>
 *          bleIncorrectMode: No link to terminate.<BR>
 */
extern bStatus_t GAPCentralRole_TerminateLink( uint16 connHandle );

/**
 * @brief   Establish a link to a peer device.
 *
 * @param   highDutyCycle -  TRUE to high duty cycle scan, FALSE if not
 * @param   whiteList - determines use of the white list: @ref GAP_WHITELIST_DEFINES
 * @param   addrTypePeer - address type of the peer device: @ref GAP_ADDR_TYPE_DEFINES
 * @param   peerAddr - peer device address
 *
 * @return  SUCCESS: started establish link process.<BR>
 *          bleIncorrectMode: invalid profile role.<BR>
 *          bleNotReady: a scan is in progress.<BR>
 *          bleAlreadyInRequestedMode: can抰 process now.<BR>
 *          bleNoResources: too many links.<BR>
 */
extern bStatus_t GAPCentralRole_EstablishLink( uint8 highDutyCycle, uint8 whiteList,
                                               uint8 addrTypePeer, uint8 *peerAddr );

/**
 * @brief   Update the link connection parameters.
 *
 * @param   connHandle - connection handle
 * @param   connIntervalMin - minimum connection interval in 1.25ms units
 * @param   connIntervalMax - maximum connection interval in 1.25ms units
 * @param   connLatency - number of LL latency connection events
 * @param   connTimeout - connection timeout in 10ms units
 *
 * @return  SUCCESS: Connection update started started.<BR>
 *          bleIncorrectMode: No connection to update.<BR>
 */
extern bStatus_t GAPCentralRole_UpdateLink( uint16 connHandle, uint16 connIntervalMin,
                                            uint16 connIntervalMax, uint16 connLatency,
                                            uint16 connTimeout );
/**
 * @brief   Start a device discovery scan.
 *
 * @param   mode - discovery mode: @ref GAP_DEVDISC_MODE_DEFINES
 * @param   activeScan - TRUE to perform active scan
 * @param   whiteList - TRUE to only scan for devices in the white list
 *
 * @return  SUCCESS: Discovery scan started.<BR>
 *          bleIncorrectMode: Invalid profile role.<BR>
 *          bleAlreadyInRequestedMode: Not available.<BR>
 */
extern bStatus_t GAPCentralRole_StartDiscovery( uint8 mode, uint8 activeScan, uint8 whiteList );

/**
 * @brief   Cancel a device discovery scan.
 *
 * @return  SUCCESS: Cancel started.<BR>
 *          bleInvalidTaskID: Not the task that started discovery.<BR>
 *          bleIncorrectMode: Not in discovery mode.<BR>
 */
extern bStatus_t GAPRole_CentralCancelDiscovery( void );

/**
 * @brief   Start periodic RSSI reads on a link.
 *
 * @param   connHandle - connection handle of link
 * @param   period - RSSI read period in 0.625 ms
 *
 * @return  SUCCESS: Terminate started.<BR>
 *          bleIncorrectMode: No link.<BR>
 *          bleNoResources: No resources.<BR>
 */
extern bStatus_t GAPCentralRole_StartRssi( uint16 connHandle, uint16 period );

/**
 * @brief   Cancel periodic RSSI reads on a link.
 *
 * @param   connHandle - connection handle of link
 *
 * @return  SUCCESS: Operation successful.<BR>
 *          bleIncorrectMode: No link.<BR>
 */
extern bStatus_t GAPRole_CentralCancelRssi(uint16 connHandle );

/*-------------------------------------------------------------------
 * TASK API - These functions must only be called by tmos.
 */

/**
 * @internal
 *
 * @brief   Central Profile Task initialization function.
 *
 * @param   taskId - Task ID.
 *
 * @return  void
 */
extern void GAPRole_CentralInit( uint8 taskId );

/**
 * @internal
 *
 * @brief   Central Profile Task event processing function.
 *
 * @param   taskId - Task ID
 * @param   events - Events.
 *
 * @return  events not processed
 */
extern uint16 GAPRole_CentralProcessEvent( uint8 taskId, uint16 events );

extern bStatus_t GAPRole_SetParameter( uint16 param, uint8 len, void *pValue );

/**
 * @brief       Get a GAP Role parameter.
 *
 *  NOTE: You can call this function with a GAP Parameter ID and it will get a
 *        GAP Parameter.  GAP Parameters are defined in (gap.h).  Also, the
 *        "pValue" field must point to a "uint16".
 *
 * @param       param - Profile parameter ID: @ref GAPROLE_PROFILE_PARAMETERS
 * @param       pValue - pointer to location to get the value.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return      SUCCESS or INVALIDPARAMETER (invalid paramID)
 */
extern bStatus_t GAPRole_GetParameter( uint16 param, void *pValue );

/**
 * @brief       Does the device initialization.  Only call this function once.
 *
 * @param       pAppCallbacks - pointer to application callbacks.
 *
 * @return      SUCCESS or bleAlreadyInRequestedMode  
 */
extern bStatus_t GAPRole_StartDevice( gapRolesCBs_t *pAppCallbacks );

/**
 * @brief       Terminates the existing connection.
 *
 * @return      SUCCESS or bleIncorrectMode
 */
extern bStatus_t GAPRole_TerminateConnection( void );

/**
 * @brief       Update the parameters of an existing connection
 *
 * @param       connInterval - the new connection interval
 * @param       latency - the new slave latency
 * @param       connTimeout - the new timeout value
 * @param       handleFailure - what to do if the update does not occur.
 *              Method may choose to terminate connection, try again, or take no action
 *
 * @return      SUCCESS, bleNotConnected or bleInvalidRange
 */
extern bStatus_t GAPRole_SendUpdateParam( uint16 minConnInterval, uint16 maxConnInterval,
                                          uint16 latency, uint16 connTimeout, uint8 handleFailure );

/**
 * @brief       Register application's callbacks.
 *
 * @param       pParamUpdateCB - pointer to param update callback.
 *
 * @return      none
 */
extern void GAPRole_PeripheralRegisterAppCBs( gapRolesParamUpdateCB_t pParamUpdateCB );

/**
 * @internal
 *
 * @brief       Initialization function for the GAP Role Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param       the ID assigned by tmos.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return      void
 */
extern void GAPRole_PeripheralInit( uint8 task_id );

/**
 * @internal
 *
 * @brief       GAP Role Task event processor.
 *          This function is called to process all events for the task.
 *          Events include timers, messages and any other user defined
 *          events.
 *
 * @param   task_id  - The tmos assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return      events not processed
 */
extern uint16 GAPRole_PeripheralProcessEvent( uint8 task_id, uint16 events );

/**
 * @brief       Does the device initialization.  Only call this function once.
 *
 * @param       pAppCallbacks - pointer to application callbacks.
 *
 * @return      SUCCESS or bleAlreadyInRequestedMode
 */
extern bStatus_t GAPRole_BroadcasterStartDevice( gapRolesCBs_t *pAppCallbacks );
  
/**
 * @internal
 *
 * @brief       Initialization function for the GAP Role Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param       the ID assigned by tmos.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return      void
 */
extern void GAPRole_BroadcasterInit( uint8 task_id );

/**
 * @internal
 *
 * @brief       GAP Role Task event processor.  
 *          This function is called to process all events for the task.
 *          Events include timers, messages and any other user defined 
 *          events.
 *
 * @param   task_id  - The tmos assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return      events not processed
 */
extern uint16 GAPRole_BroadcasterProcessEvent( uint8 task_id, uint16 events );

/**
 * @brief       Set a GAP Bond Manager parameter.
 *
 *  NOTE: You can call this function with a GAP Parameter ID and it will set the
 *        GAP Parameter.  GAP Parameters are defined in (gap.h).  Also,
 *        the "len" field must be set to the size of a "uint16" and the
 *        "pValue" field must point to a "uint16".
 *
 * @param       param - Profile parameter ID: @ref GAPBOND_PROFILE_PARAMETERS
 * @param       len - length of data to write
 * @param       pValue - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return      SUCCESS or INVALIDPARAMETER (invalid paramID)
 */
extern bStatus_t GAPBondMgr_SetParameter( uint16 param, uint8 len, void *pValue );

/**
 * @brief       Get a GAP Bond Manager parameter.
 *
 *  NOTE: You can call this function with a GAP Parameter ID and it will get a
 *        GAP Parameter.  GAP Parameters are defined in (gap.h).  Also, the
 *        "pValue" field must point to a "uint16".
 *
 * @param       param - Profile parameter ID: @ref GAPBOND_PROFILE_PARAMETERS
 * @param       pValue - pointer to location to get the value.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return      SUCCESS or INVALIDPARAMETER (invalid paramID)
 */
extern bStatus_t GAPBondMgr_GetParameter( uint16 param, void *pValue );

/**
 * @brief       Notify the Bond Manager that a connection has been made.
 *
 *   NOTE:      The GAP Peripheral/Central Role profile will
 *              call this function, if they are included in the project.
 *
 * @param       addrType - device's address type. Reference GAP_ADDR_TYPE_DEFINES in gap.h
 * @param       pDevAddr - device's address
 * @param       connHandle - connection handle
 * @param       role - master or slave role.  Reference GAP_PROFILE_ROLE_DEFINES in gap.h
 *
 * @return      SUCCESS, otherwise failure
 */
extern bStatus_t GAPBondMgr_LinkEst( uint8 addrType, uint8 *pDevAddr, uint16 connHandle, uint8 role );

/**
 * @brief       Notify the Bond Manager that a connection has been terminated.
 *
 *   NOTE:      The GAP Peripheral/Central Role profile will
 *              call this function, if they are included in the project.
 *
 * @param       connHandle - connection handle
 *
 * @return      none
 */
extern void GAPBondMgr_LinkTerm(uint16 connHandle);

/*********************************************************************
 * @brief       Notify the Bond Manager that a Slave Security Request is received.
 *
 *   NOTE:      The GAP Central Role profile will call this function,
 *              if it is included in the project.
 *
 * @param       connHandle - connection handle
 *
 * @return      none
 */
extern void GAPBondMgr_SlaveReqSecurity(uint16 connHandle);

/**
 * @brief       Resolve an address from bonding information.
 *
 * @param       addrType - device's address type. Reference GAP_ADDR_TYPE_DEFINES in gap.h
 * @param       pDevAddr - device's address
 * @param       pResolvedAddr - pointer to buffer to put the resolved address
 *
 * @return      bonding index (0 - (GAP_BONDINGS_MAX-1) if found,
 *              GAP_BONDINGS_MAX if not found
 */
extern uint8 GAPBondMgr_ResolveAddr( uint8 addrType, uint8 *pDevAddr, uint8 *pResolvedAddr );

/**
 * @brief       Set/clear the service change indication in a bond record.
 *
 * @param       connectionHandle - connection handle of the connected device or 0xFFFF
 *                                 if all devices in database.
 * @param       setParam - TRUE to set the service change indication, FALSE to clear it.
 *
 * @return      SUCCESS - bond record found and changed,<BR>
 *              bleNoResources - bond record not found (for 0xFFFF connectionHandle),<BR>
 *              bleNotConnected - connection not found - connectionHandle is invalid (for non-0xFFFF connectionHandle).
 */
extern bStatus_t GAPBondMgr_ServiceChangeInd( uint16 connectionHandle, uint8 setParam );

/**
 * @brief       Update the Characteristic Configuration in a bond record.
 *
 * @param       connectionHandle - connection handle of the connected device or 0xFFFF
 *                                 if all devices in database.
 * @param       attrHandle - attribute handle.
 * @param       value - characteristic configuration value.
 *
 * @return      SUCCESS - bond record found and changed,<BR>
 *              bleNoResources - bond record not found (for 0xFFFF connectionHandle),<BR>
 *              bleNotConnected - connection not found - connectionHandle is invalid (for non-0xFFFF connectionHandle).
 */
extern bStatus_t GAPBondMgr_UpdateCharCfg( uint16 connectionHandle, uint16 attrHandle, uint16 value );

/**
 * @brief       Register callback functions with the bond manager.
 *
 *   NOTE:      There is no need to register a passcode callback function
 *              if the passcode will be handled with the GAPBOND_DEFAULT_PASSCODE parameter.
 *
 * @param       pCB - pointer to callback function structure.
 *
 * @return      none
 */
extern void GAPBondMgr_Register( gapBondCBs_t *pCB );

/**
 * @brief       Respond to a passcode request.
 *
 * @param       connectionHandle - connection handle of the connected device or 0xFFFF
 *                                 if all devices in database.
 * @param       status - SUCCESS if passcode is available, otherwise see @ref SMP_PAIRING_FAILED_DEFINES.
 * @param       passcode - integer value containing the passcode.
 *
 * @return      SUCCESS - bond record found and changed,<BR>
 *              bleIncorrectMode - Link not found.
 */
extern bStatus_t GAPBondMgr_PasscodeRsp( uint16 connectionHandle, uint8 status, uint32 passcode );

/**
 * @brief       This is a bypass mechanism to allow the bond manager to process
 *              GAP messages.
 *
 *   NOTE:      This is an advanced feature and shouldn't be called unless
 *              the normal GAP Bond Manager task ID registration is overridden.
 *
 * @param       pMsg - GAP event message
 *
 * @return      TRUE if safe to deallocate incoming GAP message, FALSE otherwise.
 */
extern uint8 GAPBondMgr_ProcessGAPMsg( gapEventHdr_t *pMsg );

/**
 * @internal
 *
 * @brief       Initialization function for the GAP Bond Manager Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notification ... ).
 *
 * @param       the ID assigned by tmos.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return      void
 */
extern void GAPBondMgr_Init( uint8 task_id );

/**
 * @internal
 *
 * @brief       GAP Bond Manager Task event processor.
 *          This function is called to process all events for the task.
 *          Events include timers, messages and any other user defined
 *          events.
 *
 * @param   task_id  - The tmos assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return      events not processed
 */
extern uint16 GAPBondMgr_ProcessEvent( uint8 task_id, uint16 events );

/**************************************************************************************************
 */
#ifdef __cplusplus
}
#endif

#endif
