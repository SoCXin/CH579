


#ifndef __CH57x_SYS_H__
#define __CH57x_SYS_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "CH579SFR.h"
#include "core_cm0.h"

	 
	 
/**
  * @brief  rtc interrupt event define
  */
typedef enum
{
	RST_STATUS_SW = 0,			// ������λ
	RST_STATUS_RPOR,			// �ϵ縴λ
	RST_STATUS_WTR,				// ���Ź���ʱ��λ
	RST_STATUS_MR,				// �ⲿ�ֶ���λ
	RST_STATUS_LRM0,			// ���Ѹ�λ-����λ����
	RST_STATUS_GPWSM,			// �µ�ģʽ���Ѹ�λ
	RST_STATUS_LRM1,			//	���Ѹ�λ-���Ź�����
	RST_STATUS_LRM2,			//	���Ѹ�λ-�ֶ���λ����

}SYS_ResetStaTypeDef;

/**
  * @brief  rtc interrupt event define
  */
typedef enum
{
	INFO_ROM_READ = 0,			// FlashROM ����������� �Ƿ�ɶ�
	INFO_RESET_EN = 2,			// RST#�ⲿ�ֶ���λ���빦���Ƿ���
	INFO_BOOT_EN,				// ϵͳ�������� BootLoader �Ƿ���
	INFO_DEBUG_EN,				// ϵͳ������Խӿ��Ƿ���
	INFO_LOADER,				// ��ǰϵͳ�Ƿ���Bootloader ��
	STA_SAFEACC_ACT,			// ��ǰϵͳ�Ƿ��ڰ�ȫ����״̬������RWA�������򲻿ɷ���

}SYS_InfoStaTypeDef;
	 



#define SYS_GetChipID()				R8_CHIP_ID									/* ��ȡоƬID�࣬һ��Ϊ�̶�ֵ */
#define SYS_GetAccessID()			R8_SAFE_ACCESS_ID							/* ��ȡ��ȫ����ID��һ��Ϊ�̶�ֵ */
UINT8 SYS_GetInfoSta( SYS_InfoStaTypeDef i );									/* ��ȡ��ǰϵͳ��Ϣ״̬ */
// refer to SYS_ResetStaTypeDef
#define SYS_GetLastResetSta()		(R8_RESET_STATUS&RB_RESET_FLAG)				/* ��ȡϵͳ�ϴθ�λ״̬ */
void SYS_ResetExecute( void );													/* ִ��ϵͳ������λ */
#define SYS_ResetKeepBuf( d )		(R8_GLOB_RESET_KEEP = d)					/* �����ֶ���λ�� ������λ�� ���Ź���λ������ͨ���Ѹ�λ��Ӱ�� */

void SYS_DisableAllIrq( PUINT32 pirqv);									        /* �ر������жϣ���������ǰ�ж�ֵ */
void SYS_RecoverIrq( UINT32 irq_status );									    /* �ָ�֮ǰ�رյ��ж�ֵ */
UINT32 SYS_GetSysTickCnt( void );												/* ��ȡ��ǰϵͳ(SYSTICK)����ֵ */

#define  WWDG_SetCounter( c )		(R8_WDOG_COUNT = c)							/* ���ؿ��Ź�������ֵ�������� */
// DISABLE-����������ж�  ENABLE-��������ж�
#define  WWDG_ITCfg( s )			((s)?(R8_RST_WDOG_CTRL|=RB_WDOG_INT_EN):(R8_RST_WDOG_CTRL&=~RB_WDOG_INT_EN))		/* ���Ź�����ж�ʹ�� */
// DISABLE-�������λ      ENABLE-���ϵͳ��λ
#define  WWDG_ResetCfg( s )			((s)?(R8_RST_WDOG_CTRL|=RB_WDOG_RST_EN):(R8_RST_WDOG_CTRL&=~RB_WDOG_RST_EN))		/* ���Ź������λʹ�� */
#define  WWDG_GetFlowFlag()			(R8_RST_WDOG_CTRL&RB_WDOG_INT_FLAG)			/* ��ȡ��ǰ���Ź���ʱ�������־ */
#define  WWDG_ClearFlag()			(R8_RST_WDOG_CTRL|=RB_WDOG_INT_FLAG)			/* ������Ź��жϱ�־�����¼��ؼ���ֵҲ����� */


void DelsyUs( UINT16 t );		/* uS ��ʱ */
void DelsyMs( UINT16 t );		/* mS ��ʱ */


#ifdef __cplusplus
}
#endif

#endif  // __CH57x_SYS_H__	
