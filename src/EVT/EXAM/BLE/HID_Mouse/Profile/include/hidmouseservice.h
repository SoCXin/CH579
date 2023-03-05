/********************************** (C) COPYRIGHT *******************************
 * File Name          : hidmouseservice.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/10
 * Description        :      
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef HIDMOUSESERVICE_H
#define HIDMOUSESERVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

// Number of HID reports defined in the service
#define HID_NUM_REPORTS          3

// HID Report IDs for the service
#define HID_RPT_ID_MOUSE_IN      0  // Mouse input report ID
#define HID_RPT_ID_FEATURE       0  // Feature report ID

// HID feature flags
#define HID_FEATURE_FLAGS             HID_FLAGS_REMOTE_WAKE

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */


/*********************************************************************
 * API FUNCTIONS
 */

/*********************************************************************
 * @fn      Hid_AddService
 *
 * @brief   Initializes the HID service for keyboard by registering
 *          GATT attributes with the GATT server.
 *
 * @param   none
 *
 * @return  Success or Failure
 */
extern bStatus_t Hid_AddService(void);

/*********************************************************************
 * @fn      Hid_SetParameter
 *
 * @brief   Set a HID parameter.
 *
 * @param   id     - HID report ID.
 * @param   type   - HID report type.
 * @param   uuid   - attribute uuid.
 * @param   len    - length of data to right.
 * @param   pValue - pointer to data to write.  This is dependent on
 *          the input parameters and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  GATT status code.
 */
extern uint8 Hid_SetParameter(uint8 id, uint8 type, uint16 uuid, uint8 len, 
                                 void *pValue);

/*********************************************************************
 * @fn      Hid_GetParameter
 *
 * @brief   Get a HID parameter.
 *
 * @param   id     - HID report ID.
 * @param   type   - HID report type.
 * @param   uuid   - attribute uuid.
 * @param   pLen   - length of data to be read.
 * @param   pValue - pointer to data to get.  This is dependent on
 *          the input parameters and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  GATT status code.
 */
extern uint8 Hid_GetParameter(uint8 id, uint8 type, uint16 uuid, uint16 *pLen, void *pValue);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif 
