/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.c
* Author             : 
* Version            : 
* Date               : 
* Description 		   : Æ¬ÉÏflash CodeÇøDataFlashÇøµÄ²Á/¶Á/Ð´
*******************************************************************************/

#include "CH57x_common.h"

#define  TEST_CODESIZE          100
#define  TEST_DATAFLASHSIZE     4
#define  TEST_CODEADDR          0x00004000
#define  TEST_DATAFLASHADDR     0x0000

UINT32   TestBuf[ TEST_CODESIZE ];

void DebugInit(void)		
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

int main()
{ 
    UINT16   i;
    PUINT32  p32;
    UINT8    buf[8];
    PUINT16  p16;

/* ÅäÖÃ´®¿Úµ÷ÊÔ */   
    DebugInit();
    PRINT( "Start @ChipID=%02X\n", R8_CHIP_ID );    

/*  */
    PRINT( "GetUniqueID: ");     
    GetUniqueID( buf );
    p16 = (PUINT16)buf;
    if( *(p16+3) == (*(p16+0) +  *(p16+1) + *(p16+2)) )     // ÅÐ¶ÏÐ£ÑéºÍ
    {
        for(i=0; i<6; i++)
        {
            PRINT("%02x ", buf[i]);
        }PRINT("\n");
    }
    else
    {
        PRINT( "UniqueID is error or none !!! \n");  
    }
    

/* »ñÈ¡ÍøÂçMACµØÖ· */  
    PRINT( "GetMACAddress: ");     
    GetMACAddress( buf );
    for(i=0; i<6; i++)
    {
        PRINT("%02x ", buf[i]);
    }PRINT("\n");
    
    
/* ²âÊÔ code ²ÁÐ´¶Á */
    p32 = (PUINT32)TEST_CODEADDR;
    PRINT( "Read addr=0x%08lx :\n", (UINT32)p32 );    
    for(i=0; i<TEST_CODESIZE; i++)
    {
        PRINT("%08lx ",*p32++);
    }PRINT("\n");
    
    p32 = (PUINT32)TEST_CODEADDR;
    PRINT( "Earse addr=0x%08lx : ", (UINT32)p32 );
    i = CodeFlash_BlockEarse( (UINT32)p32 );
    if( i == SUCCESS )  PRINT("success..\n");
    else                PRINT("failed..\n");
    
    for(i=0; i<TEST_CODESIZE; i++)
    {
        TestBuf[i] = i;
        TestBuf[i] = (TestBuf[i]<<8) | i;
        TestBuf[i] = (TestBuf[i]<<8) | i;
        TestBuf[i] = (TestBuf[i]<<8) | i;
    }
    
    p32 = (PUINT32)TEST_CODEADDR;
    PRINT( "Write addr=0x%08lx : ", (UINT32)p32 );
    i = CodeFlash_WriteBuf( (UINT32)p32, TestBuf, TEST_CODESIZE*4 );
    if( i == SUCCESS )  PRINT("success..\n");
    else                PRINT("failed..\n");  

    p32 = (PUINT32)TEST_CODEADDR;
    PRINT( "Read addr=0x%08lx :\n", (UINT32)p32 );    
    for(i=0; i<TEST_CODESIZE; i++)
    {
        PRINT("%08lx ",*p32++);
    }PRINT("\n");
    

/* ²âÊÔ dataflash ²ÁÐ´¶Á */
    p32 = (PUINT32)(TEST_DATAFLASHADDR+DATA_FLASH_ADDR);
    PRINT( "Read dataflash addr=0x%08lx :\n", (UINT32)p32 );    
    for(i=0; i<TEST_DATAFLASHSIZE; i++)
    {
        PRINT("%08lx ",*p32++);
    }PRINT("\n");
    
    p32 = (PUINT32)TEST_DATAFLASHADDR;
    PRINT( "Earse dataflash addr=0x%08lx : ", (UINT32)p32 );
    i = DataFlash_BlockEarse( (UINT32)p32 );
    if( i == SUCCESS )  PRINT("success..\n");
    else                PRINT("failed..\n");
    
    for(i=0; i<TEST_DATAFLASHSIZE; i++)
    {
        TestBuf[i] = i;
        TestBuf[i] = (TestBuf[i]<<8) | i;
        TestBuf[i] = (TestBuf[i]<<8) | i;
        TestBuf[i] = (TestBuf[i]<<8) | i;
    }
    
    p32 = (PUINT32)TEST_DATAFLASHADDR;
    PRINT( "Write dataflash addr=0x%08lx : ", (UINT32)p32 );
    for(i=0; i<TEST_DATAFLASHSIZE; i++)
    {
        if( DataFlash_WriteDW( (UINT32)p32++, TestBuf[i])!=SUCCESS )  
        {
            break;
        }            
    }
    if( i==TEST_DATAFLASHSIZE ) PRINT("success..\n");
    else                        PRINT("failed..\n");
    
    p32 = (PUINT32)(TEST_DATAFLASHADDR+DATA_FLASH_ADDR);
    PRINT( "Read dataflash addr=0x%08lx :\n", (UINT32)p32 );    
    for(i=0; i<TEST_DATAFLASHSIZE; i++)
    {
        PRINT("%08lx ",*p32++);
    }PRINT("\n");    
    
    while(1);    
}






