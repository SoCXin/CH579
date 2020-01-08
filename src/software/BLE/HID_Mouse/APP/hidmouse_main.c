/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/11/12
* Description        : 蓝牙鼠标应用主函数及任务系统初始化
*******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include "CONFIG.h"
#include "CH57x_common.h"
#include "CH57xBLE_LIB.h"
#include "HAL.h"
#include "hiddev.h"
#include "hidmouse.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
__align(4) u32 MEM_BUF[BLE_MEMHEAP_SIZE/4];

#if (defined (BLE_MAC)) && (BLE_MAC == TRUE)
u8C MacAddr[6] = {0x84,0xC2,0xE4,0x03,0x02,0x02};
#endif

/*******************************************************************************
* Function Name  : main
* Description    : 主函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main( void ) 
{
#ifdef DEBUG
  GPIOA_SetBits(GPIO_Pin_9);
  GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
  UART1_DefInit( );
#endif   
  PRINT("%s\n",VER_LIB);
  CH57X_BLEInit( );
	HAL_Init( );
	GAPRole_PeripheralInit( );
	HidDev_Init( ); 
	HidEmu_Init( );
	while(1){
		TMOS_SystemProcess( );
	}
}

/******************************** endfile @ main ******************************/
