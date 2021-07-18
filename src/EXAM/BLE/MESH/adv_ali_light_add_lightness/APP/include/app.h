/********************************** (C) COPYRIGHT *******************************
* File Name          : app.h
* Author             : WCH
* Version            : V1.0
* Date               : 2018/11/12
* Description        : 
*******************************************************************************/

#ifndef app_H
#define app_H

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#define		APP_SILENT_ADV_EVT									(1<<0)

/******************************************************************************/

void App_Init(void);
void send_led_state(void);
void send_reset_indicate(void);

/******************************************************************************/

/******************************************************************************/


#ifdef __cplusplus
}
#endif

#endif