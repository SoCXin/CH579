
#ifndef _BLE_MESH_TRACE_H_
#define _BLE_MESH_TRACE_H_

#include "CH579SFR.h"
#include <stdio.h>

/* Define common tracing for all */
#ifndef LOG_LEVEL_NONE
#define LOG_LEVEL_NONE 0
#endif /* LOG_LEVEL_ERROR */

/* Define common tracing for all */
#ifndef LOG_LEVEL_ERROR
#define LOG_LEVEL_ERROR 1
#endif /* LOG_LEVEL_ERROR */

#ifndef LOG_LEVEL_WARN
#define LOG_LEVEL_WARN 2
#endif /* LOG_LEVEL_WARN */

#ifndef LOG_LEVEL_INFO
#define LOG_LEVEL_INFO 3
#endif /* LOG_LEVEL_INFO */

#ifndef LOG_LEVEL_DEBUG
#define LOG_LEVEL_DEBUG 4
#endif /* LOG_LEVEL_DEBUG */

#ifndef LOG_LEVEL_VERBOSE
#define LOG_LEVEL_VERBOSE 5
#endif /*LOG_LEVEL_VERBOSE */

#ifdef CONFIG_BLE_MESH_STACK_TRACE_LEVEL
#define MESH_LOG_LEVEL CONFIG_BLE_MESH_STACK_TRACE_LEVEL
#else
#define MESH_LOG_LEVEL LOG_LEVEL_VERBOSE
#endif

#define MESH_TRACE_TAG "BLE_MESH"

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif /* MAX(a, b) */

#define BLE_MESH_LOG_LEVEL_CHECK(LAYER, LEVEL) (MAX(LAYER##_LOG_LEVEL, BLE_MESH_LOG_LOCAL_LEVEL_MAPPING) >= LOG_LEVEL_##LEVEL)

#ifdef NOLIB_DEBUG
#define printk	tfp_printf
#elif defined(DEBUG)
#define printk	PRINT
#else
#define printk(...)
#endif //NOLIB_DEBUG

#define _STRINGIFY(x) #x
#define STRINGIFY(s) _STRINGIFY(s)

#ifndef __ASSERT
#define __ASSERT(test, fmt, ...)                       													\
    do                                                 													\
    {                                                  													\
        if (!(test))                                   													\
        {                                              													\
            printk("ASSERTION FAIL [%s] @ %s:%d:\n\t", 													\
                   _STRINGIFY(test),                   													\
                   __FILE__,                           													\
                   __LINE__);                          													\
            printk(fmt, ##__VA_ARGS__);                													\
            for (;;)                                   													\
                ;                                      													\
        }                                              													\
    } while ((0))
#endif

#if ((MESH_LOG_LEVEL) > LOG_LEVEL_NONE)
#define MODLOG_ERROR(ml_mod_, ml_msg_, ...) 															\
		printk((ml_msg_), ##__VA_ARGS__)
#else
#define MODLOG_ERROR(ml_mod_, ml_msg_, ...)
#endif

#if ((MESH_LOG_LEVEL) > LOG_LEVEL_ERROR)
#define MODLOG_WARN(ml_mod_, ml_msg_, ...) 																\
		printk((ml_msg_), ##__VA_ARGS__)
#else
#define MODLOG_WARN(ml_mod_, ml_msg_, ...)
#endif

#if ((MESH_LOG_LEVEL) > LOG_LEVEL_WARN)
#define MODLOG_INFO(ml_mod_, ml_msg_, ...) 																\
		printk((ml_msg_), ##__VA_ARGS__)
#else
#define MODLOG_INFO(ml_mod_, ml_msg_, ...)
#endif

#if ((MESH_LOG_LEVEL) > LOG_LEVEL_INFO)
#define MODLOG_DEBUG(ml_mod_, ml_msg_, ...) 															\
		printk((ml_msg_), ##__VA_ARGS__)
#else
#define MODLOG_DEBUG(ml_mod_, ml_msg_, ...)
#endif
		
#define BLE_HS_LOG(lvl, fmt, ...) 																		\
    MODLOG_##lvl(MESH_TRACE_TAG, fmt, __VA_ARGS__)

#define BT_DBG(fmt, ...)                                                              					\
    do                                                                                					\
    {                                                                                				 	\
        if (IS_ENABLED(BT_DBG_ENABLED))                                               					\
        {                                                                             					\
            BLE_HS_LOG(DEBUG, "%s>%s>" fmt "\n", "dbg", __FUNCTION__, ##__VA_ARGS__); 					\
        }                                                                             					\
    }while (0)

#define BT_INFO(fmt, ...) 	BLE_HS_LOG(INFO, "%s>%s>" fmt "\n", "inf", __FUNCTION__, ##__VA_ARGS__)
#define BT_WARN(fmt, ...)	BLE_HS_LOG(WARN, "%s>%s>" fmt "\n", "wrn", __FUNCTION__, ##__VA_ARGS__)
#define BT_ERR(fmt, ...) 	BLE_HS_LOG(ERROR, "%s>%s>" fmt "\n", "err", __FUNCTION__, ##__VA_ARGS__)

#ifndef __ASSERT_NO_MSG
#define __ASSERT_NO_MSG(x)
#endif

#endif /* _BLE_MESH_TRACE_H_ */
