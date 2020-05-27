/********************************** (C) COPYRIGHT *******************************
* File Name          : CH57x_int.c
* Author             : WCH
* Version            : V1.0
* Date               : 2020/03/18
* Description 
*******************************************************************************/

#include "CH57x_common.h"


//__attribute__((section("NMICode")))
void NMI_Handler( void )    
{
    UINT32  i=0;
    
    while(1)
    { 
        if(R8_BAT_STATUS & RB_BAT_STAT_LOWER)   i = 0;
        else i++;
        if( i>10000 )        break;
    }
    
    /* 执行系统复位 */
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R8_RST_WDOG_CTRL = RB_SOFTWARE_RESET;
    R8_SAFE_ACCESS_SIG = 0; 
}


