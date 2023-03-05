/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_clk.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2022/12/05
 * Description        : 
 ********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef __CH57x_CLK_H__
#define __CH57x_CLK_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "CH579SFR.h"
#include "core_cm0.h"
#include <stdbool.h>

typedef enum
{
	CLK_SOURCE_LSI = 0,
	CLK_SOURCE_LSE,
	CLK_SOURCE_HSE_32MHz,
	CLK_SOURCE_HSE_16MHz,
	CLK_SOURCE_HSE_8MHz,
	CLK_SOURCE_HSI_32MHz,				//�ϵ�Ĭ�����	
	CLK_SOURCE_HSI_16MHz,
	CLK_SOURCE_HSI_8MHz,
	CLK_SOURCE_PLL_40MHz,
	CLK_SOURCE_PLL_32MHz,
	CLK_SOURCE_PLL_24MHz,
	CLK_SOURCE_PLL_20MHz,
	CLK_SOURCE_PLL_16MHz,
	
}SYS_CLKTypeDef;


typedef enum
{
	Clk32M_HSI = 0,
	Clk32M_HSE,
	
}HClk32MTypeDef;

typedef enum
{
	Clk32K_LSI = 0,
	Clk32K_LSE,
	
}LClk32KTypeDef;

typedef enum
{
	HSE_RCur_75 = 0,
	HSE_RCur_100,
    HSE_RCur_125,
    HSE_RCur_150
	
}HSECurrentTypeDef;

typedef enum
{
	HSECap_10p = 0,
	HSECap_12p,  HSECap_14p,  HSECap_16p,  HSECap_18p,  
    HSECap_20p,  HSECap_22p,  HSECap_24p
	
}HSECapTypeDef;

typedef enum
{
	LSE_RCur_70 = 0,
	LSE_RCur_100,
    LSE_RCur_140,
    LSE_RCur_200
	
}LSECurrentTypeDef;

typedef enum
{
	LSECap_2p = 0,
	LSECap_13p,  LSECap_14p,  LSECap_15p,  LSECap_16p,  
    LSECap_17p,  LSECap_18p,  LSECap_19p,  LSECap_20p,
    LSECap_21p,  LSECap_22p,  LSECap_23p,  LSECap_24p,
    LSECap_25p,  LSECap_26p,  LSECap_27p
	
}LSECapTypeDef;

#define  MAX_DAY		0x00004000 
#define	 MAX_2_SEC		0x0000A8C0
//#define	 MAX_SEC		0x545FFFFF	

#define BEGYEAR													2020
#define	IsLeapYear(yr)									(!((yr) % 400) || (((yr) % 100) && !((yr) % 4)))
#define	YearLength(yr)									(IsLeapYear(yr) ? 366 : 365)
#define monthLength(lpyr,mon)						(((mon)==1) ? (28+(lpyr)) : (((mon)>6) ? (((mon)&1)?31:30) : (((mon)&1)?30:31)))

/**
  * @brief  rtc timer mode period define
  */
typedef enum
{
	Period_0_125_S = 0,			// 0.125s ����
	Period_0_25_S,				// 0.25s ����
	Period_0_5_S,				// 0.5s ����
	Period_1_S,					// 1s ����
	Period_2_S,					// 2s ����
	Period_4_S,					// 4s ����
	Period_8_S,					// 8s ����
	Period_16_S,				// 16s ����
}RTC_TMRCycTypeDef;	 
	 

/**
  * @brief  rtc interrupt event define
  */
typedef enum
{
	RTC_TRIG_EVENT = 0,			// RTC �����¼�
	RTC_TMR_EVENT,				// RTC ���ڶ�ʱ�¼�

}RTC_EVENTTypeDef;	 

/**
  * @brief  rtc interrupt event define
  */
typedef enum
{
	RTC_TRIG_MODE = 0,			// RTC ����ģʽ
	RTC_TMR_MODE,				// RTC ���ڶ�ʱģʽ

}RTC_MODETypeDef;

__STATIC_INLINE bool SYS_IsClkXT32MPon(void)
{
	return (R8_HFCK_PWR_CTRL & RB_CLK_XT32M_PON) ? true : false;
}

__STATIC_INLINE bool SYS_IsClkINT32MPon(void)
{
	return (R8_HFCK_PWR_CTRL & RB_CLK_INT32M_PON) ? true : false;
}

__STATIC_INLINE bool SYS_IsPLLPon(void)
{
	return (R8_HFCK_PWR_CTRL & RB_CLK_PLL_PON) ? true : false;
}

__STATIC_INLINE bool SYS_IsClkXT32MPon(void);
__STATIC_INLINE bool SYS_IsClkINT32MPon(void);
__STATIC_INLINE bool SYS_IsPLLPon(void);
void SYS_ClkXT32MPon(void);
void SYS_ClkINT32MPon(void);
void SYS_PLLPon(void);

void SystemInit(void);							/* ϵͳʱ�ӳ�ʼ�� */	 
void SetSysClock( SYS_CLKTypeDef sc);			/* ����ϵͳ����ʱ�� */
UINT32 GetSysClock( void );						/* ��ȡ��ǰϵͳʱ�� */	
void HClk32M_Select( HClk32MTypeDef hc);		/* 32M ��Ƶʱ����Դ */
void LClk32k_Power(LClk32KTypeDef hc, bool enable);		/* 32K ��Ƶ������Դ���� */
void LClk32K_Select( LClk32KTypeDef hc);		/* 32K ��Ƶʱ����Դ */

void HSECFG_Current( HSECurrentTypeDef c );     /* HSE���� ƫ�õ������� */
void HSECFG_Capacitance( HSECapTypeDef c );     /* HSE���� ���ص������� */
void LSECFG_Current( LSECurrentTypeDef c );     /* LSE���� ƫ�õ������� */
void LSECFG_Capacitance( LSECapTypeDef c );     /* LSE���� ���ص������� */

UINT16 Calibration_LSI( void );				/* �ⲿ32Mʱ��У׼�ڲ�32Kʱ�� */

	 
void RTC_InitTime( UINT16 y, UINT16 mon, UINT16 d, UINT16 h, UINT16 m, UINT16 s );			/* RTCʱ�ӳ�ʼ����ǰʱ�� */
void RTC_GetTime( PUINT16 py, PUINT16 pmon, PUINT16 pd, PUINT16 ph, PUINT16 pm, PUINT16 ps );		/* ��ȡ��ǰʱ�� */
	 
void RTC_SetCycle32k( UINT32 cyc );							/* ����LSE/LSIʱ�ӣ����õ�ǰRTC ������ */	 
UINT32 RTC_GetCycle32k( void );				                /* ����LSE/LSIʱ�ӣ���ȡ��ǰRTC ������ */

void RTC_TRIGFunCfg( UINT32 cyc );							/* RTC����ģʽ���ü��ʱ��,����LSE/LSIʱ�ӣ�ƥ�������� */
void RTC_TMRFunCfg( RTC_TMRCycTypeDef t );					/* RTC��ʱģʽ���� */
void RTC_ModeFunDisable( RTC_MODETypeDef m );               /* RTC ģʽ���ܹر� */

UINT8 RTC_GetITFlag( RTC_EVENTTypeDef f );					/* ��ȡRTC�жϱ�־ */	 
void RTC_ClearITFlag( RTC_EVENTTypeDef f );					/* ���RTC�жϱ�־ */ 

	 

	 
#ifdef __cplusplus
}
#endif

#endif  // __CH57x_CLK_H__	

