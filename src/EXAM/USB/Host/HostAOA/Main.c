/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/15
 * Description 		 : USB�豸ö��
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 ʹ�ô����̣����� CH57x_usbhost.h������ FOR_ROOT_UDISK_ONLY �� DISK_BASE_BUF_LEN �궨��
*/

#include "CH57x_common.h"
// ���ӳ��򷵻�״̬��
#define ERR_SUCCESS         0x00    // �����ɹ�
#define ERR_USB_CONNECT     0x15    /* ��⵽USB�豸�����¼�,�Ѿ����� */
#define ERR_USB_DISCON      0x16    /* ��⵽USB�豸�Ͽ��¼�,�Ѿ��Ͽ� */
#define ERR_USB_BUF_OVER    0x17    /* USB��������������������̫�໺������� */
#define ERR_USB_DISK_ERR    0x1F    /* USB�洢������ʧ��,�ڳ�ʼ��ʱ������USB�洢����֧��,�ڶ�д�����п����Ǵ����𻵻����Ѿ��Ͽ� */
#define ERR_USB_TRANSFER    0x20    /* NAK/STALL�ȸ����������0x20~0x2F */
#define ERR_USB_UNSUPPORT   0xFB    /*��֧�ֵ�USB�豸*/
#define ERR_USB_UNKNOWN     0xFE    /*�豸��������*/
#define ERR_AOA_PROTOCOL    0x41    /*Э��汾���� */ 

__align(4) UINT8  RxBuffer[ MAX_PACKET_SIZE ] ;      // IN, must even address
__align(4) UINT8  TxBuffer[ MAX_PACKET_SIZE ] ;      // OUT, must even address
extern UINT8  Com_Buffer[];
//AOA��ȡЭ��汾
__align(4) const UINT8  GetProtocol[] = { 0xc0,0x33,0x00,0x00,0x00,0x00,0x02,0x00 };
//�������ģʽ
__align(4) const UINT8  TouchAOAMode[] = { 0x40,0x35,0x00,0x00,0x00,0x00,0x00,0x00 };
/* AOA������鶨�� */
__align(4) const UINT8  Sendlen[]= {0,4,16,35,39,53,67};
//�ַ���ID,���ֻ�APP��ص��ַ�����Ϣ
__align(4) UINT8  StringID[] = {'W','C','H',0x00,                                                                                //manufacturer name
                      'W','C','H','U','A','R','T','D','e','m','o',0x00,                                   //model name
                      0x57,0x43,0x48,0x20,0x41,0x63,0x63,0x65,0x73,0x73,0x6f,0x72,0x79,0x20,0x54,0x65,0x73,0x74,0x00,     //description
                      '1','.','0',0x00 ,                                                                       //version
                      0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x63,0x68,0x2e,0x63,0x6e,0,//URI
                      0x57,0x43,0x48,0x41,0x63,0x63,0x65,0x73,0x73,0x6f,0x72,0x79,0x31,0x00                               //serial number
                     };  
//Ӧ�������ַ�������
__align(4) const UINT8  SetStringID[]= {0x40,0x34,0x00,0x00,0x00,0x00,0x04,0x00,
                        0x40,0x34,0x00,0x00,0x01,0x00,12,0x00,
                        0x40,0x34,0x00,0x00,0x02,0x00,19,0x00,
                        0x40,0x34,0x00,0x00,0x03,0x00,4,0x00,
                        0x40,0x34,0x00,0x00,0x04,0x00,0x0E,0x00,
                        0x40,0x34,0x00,0x00,0x05,0x00,0x0E,0x00
                       };
UINT8   TouchStartAOA(void);                           // ��������AOAģʽ
int main()
{	
	UINT8	s;
  UINT8  touchaoatm = 0; 
	SetSysClock( CLK_SOURCE_HSE_32MHz );
    PWR_UnitModCfg( ENABLE, UNIT_SYS_PLL );		// ��PLL
    DelayMs(5);
    
	GPIOA_SetBits(GPIO_Pin_9);
	GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
	GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
	UART1_DefInit();
	PRINT( "Start @ChipID=%02X\n", R8_CHIP_ID );
	
    pHOST_RX_RAM_Addr = RxBuffer;
    pHOST_TX_RAM_Addr = TxBuffer;
	USB_HostInit();
	PRINT( "Wait Device In\n" );
	while(1)
	{
		s = ERR_SUCCESS;
		if ( R8_USB_INT_FG & RB_UIF_DETECT ) {  // �����USB��������ж�����
			R8_USB_INT_FG = RB_UIF_DETECT ; 
			s = AnalyzeRootHub( );   
			if ( s == ERR_USB_CONNECT ) 		FoundNewDev = 1;
		}
		
		if ( FoundNewDev || s == ERR_USB_CONNECT ) {  // ���µ�USB�豸����
			FoundNewDev = 0;
			mDelaymS( 200 );  // ����USB�豸�ղ�����δ�ȶ�,�ʵȴ�USB�豸���ٺ���,������ζ���
			s = InitRootDevice();  // ��ʼ��USB�豸
			if((ThisUsbDev.DeviceVID == 0x18D1)&&(ThisUsbDev.DevicePID &0xff00)==0x2D00){
				PRINT("AOA Mode\n");
				ThisUsbDev.DeviceType = DEF_AOA_DEVICE;
			}else{	//�������AOA ���ģʽ�������������ģʽ.
				SetUsbSpeed( ThisUsbDev.DeviceSpeed );  // ���õ�ǰUSB�ٶ�
				s = TouchStartAOA();
				if(s == ERR_SUCCESS){
					if(touchaoatm<3)         //����AOA������������
					{		
                        FoundNewDev = 1;
                        touchaoatm++;
                        mDelaymS(500);      //���ְ�׿�豸�Զ��Ͽ����������Դ˴��������ʱ
                        continue;           //��ʵ������Բ�����ת��AOAЭ��涨���豸���Զ����½������ߡ�
					}
					//ִ�е��⣬˵�����ܲ�֧��AOA�����������豸
					PRINT("UNKOWN Device\n");
					SetUsbSpeed( 1 );  
					while(1);
				}
			}
			//if ( s != ERR_SUCCESS ) 	return( s );
		}
	}
}

//��������AOAģʽ
UINT8 TouchStartAOA(void)
{
	UINT8 len,s,i,Num;
    //��ȡЭ��汾��
    CopySetupReqPkg( (PCHAR)GetProtocol );
    s = HostCtrlTransfer( Com_Buffer, &len );  // ִ�п��ƴ���
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
	if(Com_Buffer[0]<2) return  ERR_AOA_PROTOCOL;

    //����ַ���
    for(i=0; i<6; i++)
    {
        Num=Sendlen[i];
        CopySetupReqPkg((PCHAR)&SetStringID[8*i]);
        s = HostCtrlTransfer(&StringID[Num], &len );  // ִ�п��ƴ���
        if ( s != ERR_SUCCESS )
        {
            return( s );
        }
    }	

    CopySetupReqPkg((PCHAR)TouchAOAMode);
    s = HostCtrlTransfer( Com_Buffer, &len );  // ִ�п��ƴ���
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    return ERR_SUCCESS;	 
}



