/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/11/12
* Description        : 
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include "CONFIG.h"
#include "CH57x_common.h"
#include "HAL.h"
#include "RF_PHY.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
__align(4) u32 MEM_BUF[BLE_MEMHEAP_SIZE/4];

/*******************************************************************************
* Function Name  : main
* Description    : 主函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main( void ) 
{
  GPIOA_ModeCfg( GPIO_Pin_All, GPIO_ModeIN_PU );
  GPIOB_ModeCfg( GPIO_Pin_All&(~(GPIO_Pin_11|GPIO_Pin_10)), GPIO_ModeIN_PU );
#ifdef DEBUG
  GPIOA_SetBits( bTXD1 );
  GPIOA_ModeCfg( bTXD1, GPIO_ModeOut_PP_5mA );
	UART1_DefInit( );
#endif  
  PRINT("start.\n");
  {
    PRINT("%s\n",VER_LIB);
  }
  CH57X_BLEInit( );
  HAL_Init(  );
  RF_RoleInit( );
  RF_Init( );
	while(1){
		TMOS_SystemProcess( );
	}
}

/******************************** endfile @ main ******************************/
