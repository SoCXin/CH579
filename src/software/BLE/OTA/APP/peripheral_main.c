/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/11/12
* Description        : 外设从机应用主函数及任务系统初始化
*******************************************************************************/




/******************************************************************************/
/* 头文件包含 */
#include "CONFIG.h"
#include "CH57x_common.h"
#include "CH57xBLE_LIB.h"
#include "HAL.h"
#include "GATTprofile.h"
#include "Peripheral.h"
#include "OTA.h"

/* 记录当前的Image */
unsigned char CurrImageFlag = 0xff;

__align(8) unsigned char flash_buf[FLASH_BLOCK_SIZE];

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
pTaskEventHandlerFn tasksArr[] = {
    TMOS_CbTimerProcessEvent,
    HAL_ProcessEvent,
    LL_ProcessEvent,
    L2CAP_ProcessEvent,
    GAP_ProcessEvent,    
    GATT_ProcessEvent,                                                
    SM_ProcessEvent,                                                 
    GAPBondMgr_ProcessEvent,                                         
    GATTServApp_ProcessEvent,                                        

    GAPRole_PeripheralProcessEvent,                                            
    SimpleBLEPeripheral_ProcessEvent,                                 
};

uint8 TASK_CNT =  sizeof( tasksArr ) / sizeof( tasksArr[0] );

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
    
    /* Hal Task */
    TMOS_Init( taskID++ );
    Hal_Init( taskID++ );
    /* LL Task */
    LL_Init( taskID++ );
    /* L2CAP Task */
    L2CAP_Init( taskID++ );
    /* GAP Task */
    GAP_Init( taskID++ );
    /* GATT Task */
    GATT_Init( taskID++ );
    /* SM Task */
    SM_Init( taskID++ );
    GAPBondMgr_Init( taskID++ );
    GATTServApp_Init( taskID++ );
    /* Profiles */
    GAPRole_PeripheralInit( taskID++ );
    /* Application */
    SimpleBLEPeripheral_Init( taskID++ );
}

/* 注意：关于程序升级后flash的操作必须先执行，不开启任何中断，防止操作中断和失败 */
/*******************************************************************************
* Function Name  : ReadImageFlag
* Description    : 读取当前的程序的Image标志，DataFlash如果为空，就默认是ImageA
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ReadImageFlag(void)
{
    UINT8 *p_image_flash;
    
    p_image_flash = (UINT8 *)OTA_DATAFLASH_ADD;
    CurrImageFlag = *p_image_flash;
    
    /* 程序第一次执行，或者没有更新过，以后更新后在擦除DataFlash */
    if( CurrImageFlag != IMAGE_A_FLAG && CurrImageFlag != IMAGE_B_FLAG )
    {
        CurrImageFlag = IMAGE_A_FLAG;
    }
    
    PRINT("Image Flag %02x\n",CurrImageFlag);
}

/*******************************************************************************
* Function Name  : ImageVectorsCopy
* Description    : 执行中断向量的搬移，仅当ImageA升级ImageB时执行
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ImageVectorsCopy(void)
{
	unsigned int vectors_entry;
	
	/* 读取当前的程序的入口地址 */
	vectors_entry = *(unsigned int *)IMAGE_A_ENTRY_ADD;
	
    /* ImageA->ImageB，当前的是ImageB，向量入口是ImageA，需要搬移 */
    if( (CurrImageFlag == IMAGE_B_FLAG) && (vectors_entry < IMAGE_B_START_ADD) )
    {
		unsigned int i;
		unsigned char *p_flash;
		
        /* 读取ImageB的代码第一块 */
        p_flash = (UINT8 *)IMAGE_B_START_ADD;
        for(i=0; i<FLASH_BLOCK_SIZE; i++) flash_buf[i] = p_flash[i];
		
        /* 擦除ImageA代码的第一块 */
        CodeFlash_BlockEarse(IMAGE_A_START_ADD);
		
        /* 将ImageB中断向量覆盖ImageA中断向量 */
        CodeFlash_WriteBuf(IMAGE_A_START_ADD,(PUINT32)&flash_buf[0],FLASH_BLOCK_SIZE);
		
		PRINT("ImageB vectors entry copy complete %08x \n",vectors_entry);
    }
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
    ReadImageFlag();
	ImageVectorsCopy();
    CH57X_BLEInit( );
    TMOS_InitTasks( );
    while(1){
        TMOS_SystemProcess( );
    }
}

/******************************** endfile @ main ******************************/
