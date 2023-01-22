/********************************** (C) COPYRIGHT *******************************
 * File Name          : ota.h
 * Author             : WCH
 * Version            : V1.10
 * Date               : 2018/12/14
 * Description        : oad������ö���
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/


/******************************************************************************/
#ifndef __OTA_H
#define __OTA_H

 
/* ------------------------------------------------------------------------------------------------
 *                                             OTA FLASH
 * ------------------------------------------------------------------------------------------------
 */
 
/* �����û�code���ֳ����飬ǰ146K�ͺ�8K������ֱ����imageAPP��imageOTA ʣ��96KΪ������*/

/* FLASH���� */
#define FLASH_BLOCK_SIZE                512

/* imageAPP���� */
#define IMAGE_APP_FLAG                  0x01
#define IMAGE_APP_START_ADD             0 
#define IMAGE_APP_SIZE                  0x23800 
#define IMAGE_APP_ENTRY_ADD             (IMAGE_APP_START_ADD + 4)

/* imageOTA���� */
#define IMAGE_OTA_FLAG                  0x02
#define IMAGE_OTA_START_ADD             (IMAGE_APP_START_ADD + IMAGE_APP_SIZE)
#define IMAGE_OTA_SIZE                  0x2000 
#define IMAGE_OTA_ENTRY_ADD             (IMAGE_OTA_START_ADD + 4)

 
/* IAP���� */ 
/* ����ΪIAP��������� */
#define CMD_IAP_PROM                    0x80                                    // IAP�������
#define CMD_IAP_ERASE                   0x81                                    // IAP��������
#define CMD_IAP_VERIFY                  0x82                                    // IAPУ������
#define CMD_IAP_END                     0x83                                    // IAP������־
#define CMD_IAP_INFO                    0x84                                    // IAP��ȡ�豸��Ϣ


/* ����֡���ȶ��� */
#define IAP_LEN                         247 
 
/* �����DataFlash���OTA��Ϣ */
typedef struct
{
    unsigned char ImageFlag;            //��¼�ĵ�ǰ��image��־
	unsigned char Revd[3];
}OTADataFlashInfo_t; 

/* OTA IAPͨѶЭ�鶨�� */
/* ��ַʹ��4��ƫ�� */
typedef union 
{
    struct
    {
        unsigned char cmd;              /* ������ 0x81 */
        unsigned char len;              /* �������ݳ��� */
        unsigned char addr[2];          /* ������ַ */
		unsigned char block_num[2];     /* �������� */
		
    } erase;                            /* �������� */
    struct
    {
        unsigned char cmd;              /* ������ 0x83 */
        unsigned char len;              /* �������ݳ��� */
        unsigned char status[2];        /* ���ֽ�״̬������ */
    } end;                              /* �������� */
    struct
    {
        unsigned char cmd;              /* ������ 0x82 */
        unsigned char len;              /* �������ݳ��� */
        unsigned char addr[2];          /* У���ַ */
        unsigned char buf[IAP_LEN-4];   /* У������ */
    } verify;                           /* У������ */
    struct
    {
        unsigned char cmd;              /* ������ 0x80 */
        unsigned char len;              /* �������ݳ��� */
        unsigned char addr[2];          /* ��ַ */
        unsigned char buf[IAP_LEN-4];   /* �������� */
    } program;                          /* ������� */
	struct
    {
        unsigned char cmd;              /* ������ 0x84 */
        unsigned char len;              /* �������ݳ��� */
        unsigned char buf[IAP_LEN-2];   /* �������� */
    } info;                             /* ������� */
    struct
    {
        unsigned char buf[IAP_LEN];     /* �������ݰ�*/
    } other;
} OTA_IAP_CMD_t;


/* ��¼��ǰ��Image */
extern unsigned char CurrImageFlag;


#endif

/******************************** endfile @ oad ******************************/
