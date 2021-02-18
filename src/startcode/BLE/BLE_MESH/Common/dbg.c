/********************************** (C) COPYRIGHT *******************************
* File Name          : dbg.c
* Author             : WCH
* Version            : V1.0
* Date               : 2019/12/15
* Description 
*******************************************************************************/

#include "dbg.h"
#include "CH57x_common.h"
#include "CH57xBLE_LIB.H"

static tmosTaskID wdt_task;

void wdt_prvGetRegistersFromStack(uint32 lr, uint32 sp)
{
	uint32 stack_pointer = sp, saved_regs_addr = stack_pointer;
	
	while(1)
	{
		PRINT("addr[0x%08x]\n", ((uint32 *)saved_regs_addr)[6]);
		DelayMs(1000);
	}
}

void nmi_prvGetRegistersFromStack(uint32 lr, uint32 sp)
{
	uint32 stack_pointer = sp, saved_regs_addr = stack_pointer;
	
	while(1)
	{
		PRINT("nmi battery_status: 0x%02x addr[0x%08x]\n", R8_BAT_STATUS, ((uint32 *)saved_regs_addr)[6]);
		DelayMs(1000);
	}
}

void hdt_prvGetRegistersFromStack(uint32 lr, uint32 sp)
{
	uint32 stack_pointer = sp, saved_regs_addr = stack_pointer;
	
	while(1)
	{
		PRINT("hardfault addr[0x%08x]\n", ((uint32 *)saved_regs_addr)[6]);
		DelayMs(1000);
	}
}

void HardFault_Handler(void)
{
	PRINT("HardFault_Handler\n");
}

__asm void WDT_IRQHandler(void)
{
	IMPORT wdt_prvGetRegistersFromStack

	MOV R0, LR
    MOV R1, SP
	
	LDR R2, =wdt_prvGetRegistersFromStack
    BLX     R2
}

static tmosEvents wdt_handler(tmosTaskID task, tmosEvents evt)
{
	WWDG_SetCounter(0x00);

	tmos_start_task(wdt_task, 0x01, MS1_TO_SYSTEM_TIME(100));

	return 0;
}

void dbg_init(void)
{
	WWDG_ResetCfg(FALSE);
	WWDG_ITCfg(TRUE);
	NVIC_SetPriority(WDOG_BAT_IRQn, 3);
	NVIC_EnableIRQ(WDOG_BAT_IRQn);
	
	wdt_task = TMOS_ProcessEventRegister(wdt_handler);
	tmos_set_event(wdt_task, 1);
}
