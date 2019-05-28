/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/12/11
* Description        : 直接测试程序主函数及任务系统初始化
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include "CONFIG.h"
#include "CH57x_common.h"
#include "CH57xBLE_LIB.h"
#include "HAL.h"
#include "directtest.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */

pTaskEventHandlerFn tasksArr[] = {
  TMOS_CbTimerProcessEvent,
  HAL_ProcessEvent,
  LL_ProcessEvent,
  TEST_ProcessEvent  
};
tmosTaskID TASK_CNT =  sizeof( tasksArr ) / sizeof( tasksArr[0] );

/*********************************************************************
 * @fn      TMOS_InitTasks
 *
 * @brief   This function invokes the initialization function for each task.
 *
 * @param   void
 *
 * @return  none
 */
void TMOS_InitTasks( void )
{
  UINT8 taskID = 0;
  /* tmos Task */
  TMOS_Init( taskID++ );
  /* Hal Task */
  Hal_Init( taskID++ );
  /* LL Task */
  LL_Init( taskID++ );
  //
  TEST_Init( taskID++ );
}

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
	TMOS_InitTasks( );
	while(1){
		TMOS_SystemProcess( );
	}
}

/******************************** endfile @ main ******************************/
