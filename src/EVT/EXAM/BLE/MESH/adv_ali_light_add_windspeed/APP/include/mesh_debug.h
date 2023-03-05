/********************************** (C) COPYRIGHT *******************************
* File Name          : mesh_debug.h
* Author             : WCH
* Version            : V1.0
* Date               : 2018/11/12
* Description        : 
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef mesh_debug_H
#define mesh_debug_H

#ifdef __cplusplus
extern "C"
{
#endif
/******************************************************************************/

#include "stdio.h"
/******************************************************************************/

#define BT_DBG_ENABLED					1

#define BT_DBG(X...) 																						\
        if (0)                                         \
        {                                                           \
            printf("DEBUG %s> ",  __FUNCTION__); 																		\
            printf(X); 																		\
            printf("\n"); 																		\
        }                                                           \


#define BT_ERR(X...) 																						\
        if (BT_DBG_ENABLED)                                         \
        {                                                           \
            printf("ERROR %s> ",  __FUNCTION__); 																		\
            printf(X); 																		\
            printf("\n"); 																		\
        }                                                           \


#define BT_WARN(X...) 																					\
        if (BT_DBG_ENABLED)                                         \
        {                                                           \
            printf("WARNNING %s> ",  __FUNCTION__); 																		\
            printf(X); 																		\
            printf("\n"); 																		\
        }                                                           \

#define BT_PRT(X...) 																								\
        if (0)                                         \
        {                                                           \
            printf("PRINT %s> ",  __FUNCTION__); 																		\
            printf(X); 																		\
            printf("\n"); 																		\
       }                                                           \
				
#define BT_CRYPTO(X...) 																								\
        if (0)                                         \
        {                                                           \
            printf("CRYPTO %s> ",  __FUNCTION__); 																		\
            printf(X); 																		\
            printf("\n"); 																		\
       }                                                           \

#define BT_BUF(X...) 																								\
        if (0)                                         \
        {                                                           \
            printf("BUF %s> ",  __FUNCTION__); 																		\
            printf(X); 																		\
            printf("\n"); 																		\
       }                                                           \

#define BT_NET(X...) 																								\
        if (0)                                         \
        {                                                           \
            printf("NET %s> ",  __FUNCTION__); 																		\
            printf(X); 																		\
            printf("\n"); 																		\
       }                                                           \

#define BT_TRANS(X...) 																								\
        if (0)                                         \
        {                                                           \
            printf("TRANS %s> ",  __FUNCTION__); 																		\
            printf(X); 																		\
            printf("\n"); 																		\
       }                                                           \

#define BT_PROXY(X...) 																								\
        if (0)                                         \
        {                                                           \
            printf("PROXY %s> ",  __FUNCTION__); 																		\
            printf(X); 																		\
            printf("\n"); 																		\
       }                                                           \

#define BT_INFO(X...) 																						\
        if (0)                                         \
        {                                                           \
            printf("INFO %s> ",  __FUNCTION__); 																		\
            printf(X); 																		\
            printf("\n"); 																		\
        }                                                           \
				
#define NET_BUF_SIMPLE_DBG(X...) 																						\
        if (0)                                         \
        {                                                           \
            printf("NET_BUF_SIMPLE_DBG %s> ",  __FUNCTION__); 																		\
            printf(X); 																		\
            printf("\n"); 																		\
        }                                                           \
				
#define NET_BUF_DBG(X...) 																						\
        if (0)                                         \
        {                                                           \
            printf("NET_BUF_DBG  "X); 																		\
            printf("\n"); 																		\
        }                                                           \

				
#define BT_WORK_DBG(X...) 																								\
        if (0)                                         \
        {                                                           \
            printf("WKDBG %s> ",  __FUNCTION__); 																		\
            printf(X); 																		\
            printf("\n"); 																		\
       }                                                           \
				
#define BT_TEST(X...) 																								\
        if (BT_DBG_ENABLED)                                         \
        {                                                           \
            printf("TEST %s> ",  __FUNCTION__); 																		\
            printf(X); 																		\
            printf("\n"); 																		\
       }                                                           \
				

#define LOG_DBG(X...) 																						\
        if (0)                                         \
        {                                                           \
            printf("NV DEBUG %s> ",  __FUNCTION__); 																		\
            printf(X); 																		\
            printf("\n"); 																		\
        }                                                           \


#define LOG_ERR(X...) 																						\
        if (BT_DBG_ENABLED)                                         \
        {                                                           \
            printf("NV ERROR %s> ",  __FUNCTION__); 																		\
            printf(X); 																		\
            printf("\n"); 																		\
        }                                                           \

#define LOG_INF(X...) 																						\
        if (0)                                         \
        {                                                           \
            printf("NV INFO %s> ",  __FUNCTION__); 																		\
            printf(X); 																		\
            printf("\n"); 																		\
        }                                                           \

#define BT_RF(X...) 																						\
        if (0)                                         \
        {                                                           \
            printf("RF %s> ",  __FUNCTION__); 																		\
            printf(X); 																		\
            printf("\n"); 																		\
        }                                                           \

const char *bt_hex(const void *buf, size_t len);
				
/******************************************************************************/





#ifdef __cplusplus
}
#endif

#endif
