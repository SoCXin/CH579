/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/02/20
 * Description        : ģ��USB�����豸������֧��������
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"

#define DevEP0SIZE	0x40
// �豸������
const UINT8  MyDevDescr[] = { 
0x12,0x01,0x10,0x01,0x00,0x00,0x00,DevEP0SIZE,
0x3d,0x41,0x07,0x21,0x00,0x00,0x00,0x00,
0x00,0x01
};
// ����������
const UINT8  MyCfgDescr[] = { 
0x09,0x02,0x3b,0x00,0x02,0x01,0x00,0xA0,0x32,             //����������
0x09,0x04,0x00,0x00,0x01,0x03,0x01,0x01,0x00,             //�ӿ�������,����
0x09,0x21,0x11,0x01,0x00,0x01,0x22,0x3e,0x00,             //HID��������
0x07,0x05,0x81,0x03,0x08,0x00,0x0a,                       //�˵�������
0x09,0x04,0x01,0x00,0x01,0x03,0x01,0x02,0x00,             //�ӿ�������,���
0x09,0x21,0x10,0x01,0x00,0x01,0x22,0x34,0x00,             //HID��������
0x07,0x05,0x82,0x03,0x04,0x00,0x0a                        //�˵�������

};
// ����������
const UINT8  MyLangDescr[] = { 0x04, 0x03, 0x09, 0x04 };
// ������Ϣ
const UINT8  MyManuInfo[] = { 0x0E, 0x03, 'w', 0, 'c', 0, 'h', 0, '.', 0, 'c', 0, 'n', 0 };
// ��Ʒ��Ϣ
const UINT8  MyProdInfo[] = { 0x0C, 0x03, 'C', 0, 'H', 0, '5', 0, '7', 0, 'x', 0 };
/*HID�౨��������*/
const UINT8 KeyRepDesc[] =
{
    0x05,0x01,0x09,0x06,0xA1,0x01,0x05,0x07,
    0x19,0xe0,0x29,0xe7,0x15,0x00,0x25,0x01,
    0x75,0x01,0x95,0x08,0x81,0x02,0x95,0x01,
    0x75,0x08,0x81,0x01,0x95,0x03,0x75,0x01,
    0x05,0x08,0x19,0x01,0x29,0x03,0x91,0x02,
    0x95,0x05,0x75,0x01,0x91,0x01,0x95,0x06,
    0x75,0x08,0x26,0xff,0x00,0x05,0x07,0x19,
    0x00,0x29,0x91,0x81,0x00,0xC0
};
const UINT8 MouseRepDesc[] =
{
    0x05,0x01,0x09,0x02,0xA1,0x01,0x09,0x01,
    0xA1,0x00,0x05,0x09,0x19,0x01,0x29,0x03,
    0x15,0x00,0x25,0x01,0x75,0x01,0x95,0x03,
    0x81,0x02,0x75,0x05,0x95,0x01,0x81,0x01,
    0x05,0x01,0x09,0x30,0x09,0x31,0x09,0x38,
    0x15,0x81,0x25,0x7f,0x75,0x08,0x95,0x03,
    0x81,0x06,0xC0,0xC0
};

/**********************************************************/
UINT8   DevConfig,Ready;
UINT8   SetupReqCode;
UINT16  SetupReqLen;
const UINT8 *pDescr;
/*����������*/
UINT8 HIDMouse[4] = {0x0,0x0,0x0,0x0};
UINT8 HIDKey[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
/******** �û��Զ������˵�RAM ****************************************/
__align(4) UINT8 EP0_Databuf[64+64+64];	//ep0(64)+ep4_out(64)+ep4_in(64)
__align(4) UINT8 EP1_Databuf[64+64];	//ep1_out(64)+ep1_in(64)
__align(4) UINT8 EP2_Databuf[64+64];	//ep2_out(64)+ep2_in(64)
__align(4) UINT8 EP3_Databuf[64+64];	//ep3_out(64)+ep3_in(64)


void USB_DevTransProcess( void )
{
	UINT8  len, chtype;
	UINT8  intflag, errflag = 0;
	
	intflag = R8_USB_INT_FG;
	if( intflag & RB_UIF_TRANSFER )
	{
		switch ( R8_USB_INT_ST & ( MASK_UIS_TOKEN | MASK_UIS_ENDP ) )     // �����������ƺͶ˵��
		{
			case UIS_TOKEN_SETUP:
        R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
				len = R8_USB_RX_LEN;
        if ( len == sizeof( USB_SETUP_REQ ) ) 
				{
					SetupReqLen = pSetupReqPak->wLength;
          SetupReqCode = pSetupReqPak->bRequest;
					chtype = pSetupReqPak->bRequestType;
					
					len = 0;
          errflag = 0;
					if ( ( pSetupReqPak->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )   
          {
						switch(SetupReqCode){
							case 0x0a:break;		//���һ��Ҫ��
							case 0x09:break;
							default: errflag = 0xFF;
						}
							
              //errflag = 0xFF;				/* �Ǳ�׼���� */
          }
          else                            /* ��׼���� */
          {
						switch( SetupReqCode )  
						{
							case USB_GET_DESCRIPTOR:
							{							
								switch( ((pSetupReqPak->wValue)>>8) )
								{	
									case USB_DESCR_TYP_DEVICE:
										pDescr = MyDevDescr;
										len = MyDevDescr[0];
										break;
									
									case USB_DESCR_TYP_CONFIG:
										pDescr = MyCfgDescr;
										len = MyCfgDescr[2];
										break;
									case USB_DESCR_TYP_REPORT:
										if(((pSetupReqPak->wIndex)&0xff) == 0)                             //�ӿ�0����������
                        {
                            pDescr = KeyRepDesc;                                  //����׼���ϴ�
                            len = sizeof(KeyRepDesc);
                        }
                        else if(((pSetupReqPak->wIndex)&0xff) == 1)                        //�ӿ�1����������
                        {
                            pDescr = MouseRepDesc;                                //����׼���ϴ�
                            len = sizeof(MouseRepDesc);
                            Ready = 1;                                            //����и���ӿڣ��ñ�׼λӦ�������һ���ӿ�������ɺ���Ч

                        }
                        else
                        {
                            len = 0xff;                                           //������ֻ��2���ӿڣ���仰����������ִ��
                        }
										break;
									case USB_DESCR_TYP_STRING:
										switch( (pSetupReqPak->wValue)&0xff )
										{
											case 1:
												pDescr = MyManuInfo;
												len = MyManuInfo[0];
												break;
											case 2:
												pDescr = MyProdInfo;
												len = MyProdInfo[0];
												break;
											case 0:
												pDescr = MyLangDescr;
												len = MyLangDescr[0];
												break;
											default:
												errflag = 0xFF;                               // ��֧�ֵ��ַ���������
												break;
										}
										break;
									
									default :
										errflag = 0xff;
										break;
								}
								if( SetupReqLen>len )	SetupReqLen = len;		//ʵ�����ϴ��ܳ���
								len = (SetupReqLen >= DevEP0SIZE) ? DevEP0SIZE : SetupReqLen;  
								memcpy( pEP0_DataBuf, pDescr, len );   
								pDescr += len;
							}
								break;
							
							case USB_SET_ADDRESS:
								SetupReqLen = (pSetupReqPak->wValue)&0xff;
								break;
							
							case USB_GET_CONFIGURATION:
								pEP0_DataBuf[0] = DevConfig;
								if ( SetupReqLen > 1 )		SetupReqLen = 1;
								break;
							
							case USB_SET_CONFIGURATION:
								DevConfig = (pSetupReqPak->wValue)&0xff;
								break;
							
							case USB_CLEAR_FEATURE:
								if ( ( pSetupReqPak->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )    // �˵�
								{
									switch( (pSetupReqPak->wIndex)&0xff )
									{
									case 0x82:
										R8_UEP2_CTRL = (R8_UEP2_CTRL & ~( RB_UEP_T_TOG|MASK_UEP_T_RES )) | UEP_T_RES_NAK;
										break;
									case 0x02:
										R8_UEP2_CTRL = (R8_UEP2_CTRL & ~( RB_UEP_R_TOG|MASK_UEP_R_RES )) | UEP_R_RES_ACK;
										break;
									case 0x81:
										R8_UEP1_CTRL = (R8_UEP1_CTRL & ~( RB_UEP_T_TOG|MASK_UEP_T_RES )) | UEP_T_RES_NAK;
										break;
									case 0x01:
										R8_UEP1_CTRL = (R8_UEP1_CTRL & ~( RB_UEP_R_TOG|MASK_UEP_R_RES )) | UEP_R_RES_ACK;
										break;
									default:
										errflag = 0xFF;                                 // ��֧�ֵĶ˵�
										break;
									}
								}
								else	errflag = 0xFF;    
								break;
							
							case USB_GET_INTERFACE:
								pEP0_DataBuf[0] = 0x00;
								 if ( SetupReqLen > 1 )		SetupReqLen = 1;
								break;
							
							case USB_GET_STATUS:
								pEP0_DataBuf[0] = 0x00;
								pEP0_DataBuf[1] = 0x00;
								if ( SetupReqLen > 2 )		SetupReqLen = 2;
								break;
							
							default:
								errflag = 0xff;
								break;
						}
					}
				}
				else	errflag = 0xff;
				
				if( errflag == 0xff)		// �����֧��
				{
//					SetupReqCode = 0xFF;
                    R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;  // STALL
				}
				else
				{
					if( chtype & 0x80 )		// �ϴ�
					{
						len = (SetupReqLen>DevEP0SIZE) ? DevEP0SIZE : SetupReqLen;
						SetupReqLen -= len;
					}
					else	len = 0;		// �´�	
					
					R8_UEP0_T_LEN = len; 
                    R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;  // Ĭ�����ݰ���DATA1
				}				
				break;
			
			case UIS_TOKEN_IN:
                switch( SetupReqCode )
                {
                case USB_GET_DESCRIPTOR:
                    len = SetupReqLen >= DevEP0SIZE ? DevEP0SIZE : SetupReqLen;  // ���δ��䳤��
                    memcpy( pEP0_DataBuf, pDescr, len );                    /* �����ϴ����� */
                    SetupReqLen -= len;
                    pDescr += len;
                    R8_UEP0_T_LEN = len;
                    R8_UEP0_CTRL ^= RB_UEP_T_TOG;                             // ��ת
                    break;
                case USB_SET_ADDRESS:
                    R8_USB_DEV_AD = (R8_USB_DEV_AD&RB_UDA_GP_BIT) | SetupReqLen;
                    R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                    break;
                default:
                    R8_UEP0_T_LEN = 0;                                      // ״̬�׶�����жϻ�����ǿ���ϴ�0�������ݰ��������ƴ���
                    R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                    break;
                }				
				break;
			
			case UIS_TOKEN_OUT:
				len = R8_USB_RX_LEN;
				if(SetupReqCode == 0x09)
        {
						PRINT("[%s] Num Lock\t", (pEP0_DataBuf[0] & (1<<0)) ? "*" : " ");
						PRINT("[%s] Caps Lock\t", (pEP0_DataBuf[0] & (1<<1)) ? "*" : " ");
						PRINT("[%s] Scroll Lock\n", (pEP0_DataBuf[0] & (1<<2)) ? "*" : " ");
        }
				break;
			
			case UIS_TOKEN_OUT | 1:
				if ( R8_USB_INT_ST & RB_UIS_TOG_OK ) 
				{                       // ��ͬ�������ݰ�������
					R8_UEP1_CTRL ^= RB_UEP_R_TOG;
					len = R8_USB_RX_LEN;
					DevEP1_OUT_Deal( len );
				}
				break;
			
			case UIS_TOKEN_IN | 1:
				R8_UEP1_CTRL ^= RB_UEP_T_TOG;
				R8_UEP1_CTRL = (R8_UEP1_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
				break;
			
			case UIS_TOKEN_OUT | 2:
				if ( R8_USB_INT_ST & RB_UIS_TOG_OK ) 
				{                       // ��ͬ�������ݰ�������
					R8_UEP2_CTRL ^= RB_UEP_R_TOG;
					len = R8_USB_RX_LEN;
					DevEP2_OUT_Deal( len );
				}
				break;
			
			case UIS_TOKEN_IN | 2:
				R8_UEP2_CTRL ^= RB_UEP_T_TOG;
				R8_UEP2_CTRL = (R8_UEP2_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
				break;
			
			case UIS_TOKEN_OUT | 3:
				if ( R8_USB_INT_ST & RB_UIS_TOG_OK ) 
				{                       // ��ͬ�������ݰ�������
					R8_UEP3_CTRL ^= RB_UEP_R_TOG;
					len = R8_USB_RX_LEN;
					DevEP3_OUT_Deal( len );
				}
				break;
			
			case UIS_TOKEN_IN | 3:
				R8_UEP3_CTRL ^= RB_UEP_T_TOG;
				R8_UEP3_CTRL = (R8_UEP3_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
				break;
			
			case UIS_TOKEN_OUT | 4:
				if ( R8_USB_INT_ST & RB_UIS_TOG_OK ) 
				{   
					R8_UEP4_CTRL ^= RB_UEP_R_TOG;
					len = R8_USB_RX_LEN;
					DevEP4_OUT_Deal( len );
				}
				break;
			
			case UIS_TOKEN_IN | 4:
				R8_UEP4_CTRL ^=  RB_UEP_T_TOG;
				R8_UEP4_CTRL = (R8_UEP4_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
				break;
			
			default :
				break;
		}
		R8_USB_INT_FG = RB_UIF_TRANSFER;
	}
	else if( intflag & RB_UIF_BUS_RST )
	{
		R8_USB_DEV_AD = 0;
		R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
		R8_UEP1_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
		R8_UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
		R8_UEP3_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
		R8_USB_INT_FG |= RB_UIF_BUS_RST;
	}
	else if( intflag & RB_UIF_SUSPEND )
	{
		if ( R8_USB_MIS_ST & RB_UMS_SUSPEND ) {;}	// ����
		else		{;}								// ����
		R8_USB_INT_FG = RB_UIF_SUSPEND;
	}
	else
	{
		R8_USB_INT_FG = intflag;
	}
}



void DebugInit(void)		
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

int main()
{
    SetSysClock( CLK_SOURCE_HSE_32MHz );
    PWR_UnitModCfg( ENABLE, UNIT_SYS_PLL );		// ��PLL
    DelayMs(5);
    
    DebugInit();
    printf("start\n");

    pEP0_RAM_Addr = EP0_Databuf;
    pEP1_RAM_Addr = EP1_Databuf;
    pEP2_RAM_Addr = EP2_Databuf;
    pEP3_RAM_Addr = EP3_Databuf;

    USB_DeviceInit();
		
    NVIC_EnableIRQ( USB_IRQn );
    
    while(1){
		}
}

void DevEP1_OUT_Deal( UINT8 l )
{ /* �û����Զ��� */
	UINT8 i;
	
	for(i=0; i<l; i++)
	{
		pEP1_IN_DataBuf[i] = ~pEP1_OUT_DataBuf[i];
	}
	DevEP1_IN_Deal( l );
}

void DevEP2_OUT_Deal( UINT8 l )
{ /* �û����Զ��� */
	UINT8 i;
	
	for(i=0; i<l; i++)
	{
		pEP2_IN_DataBuf[i] = ~pEP2_OUT_DataBuf[i];
	}
	DevEP2_IN_Deal( l );
}

void DevEP3_OUT_Deal( UINT8 l )
{ /* �û����Զ��� */
	UINT8 i;
	
	for(i=0; i<l; i++)
	{
		pEP3_IN_DataBuf[i] = ~pEP3_OUT_DataBuf[i];
	}
	DevEP3_IN_Deal( l );
}

void DevEP4_OUT_Deal( UINT8 l )
{ /* �û����Զ��� */
	UINT8 i;
	
	for(i=0; i<l; i++)
	{
		pEP4_IN_DataBuf[i] = ~pEP4_OUT_DataBuf[i];
	}
	DevEP4_IN_Deal( l );
}

void USB_IRQHandler (void)		/* USB�жϷ������,ʹ�üĴ�����1 */
{
	USB_DevTransProcess();
}

