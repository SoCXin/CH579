#ifndef _BLE_MESH_KERNEL_H_
#define _BLE_MESH_KERNEL_H_

#include "mesh_types.h"
#include "mesh_slist.h"
#include "mesh_config.h"
#include "mesh_util.h"

struct k_work {

	/** Internal reserved use */
	u32_t internal[3];
};

/**
 * @typedef k_work_handler_t
 * @brief Work item handler function type.
 *
 * A work item's handler function is executed by a workqueue's thread
 * when the work item is processed by the workqueue.
 *
 * @param work Address of the work item.
 *
 * @return N/A
 */
typedef void (*k_work_handler_t)(struct k_work *work);

struct k_delayed_work {
	struct k_work work;
	
	/** Internal reserved use */
	u32_t  internal[4];
};

/**
 * @brief Generate null timeout delay.
 *
 * This macro generates a timeout delay that that instructs a kernel API
 * not to wait if the requested operation cannot be performed immediately.
 *
 * @return Timeout delay value.
 */
#define K_NO_WAIT       0

/**
 * @brief Generate timeout delay from milliseconds.
 *
 * This macro generates a timeout delay that that instructs a kernel API
 * to wait up to @a ms milliseconds to perform the requested operation.
 *
 * @param ms Duration in milliseconds.
 *
 * @return Timeout delay value.
 */
#define K_MSEC(ms)      (ms)

/**
 * @brief Generate timeout delay from seconds.
 *
 * This macro generates a timeout delay that that instructs a kernel API
 * to wait up to @a s seconds to perform the requested operation.
 *
 * @param s Duration in seconds.
 *
 * @return Timeout delay value.
 */
#define K_SECONDS(s)    K_MSEC((s) * 1000U)

/**
 * @brief Generate timeout delay from minutes.
 *
 * This macro generates a timeout delay that that instructs a kernel API
 * to wait up to @a m minutes to perform the requested operation.
 *
 * @param m Duration in minutes.
 *
 * @return Timeout delay value.
 */
#define K_MINUTES(m)    K_SECONDS((m) * 60)

/**
 * @brief Generate timeout delay from hours.
 *
 * This macro generates a timeout delay that that instructs a kernel API
 * to wait up to @a h hours to perform the requested operation.
 *
 * @param h Duration in hours.
 *
 * @return Timeout delay value.
 */
#define K_HOURS(h)      K_MINUTES((h) * 60)

/**
 * @brief Generate infinite timeout delay.
 *
 * This macro generates a timeout delay that that instructs a kernel API
 * to wait as long as necessary to perform the requested operation.
 *
 * @return Timeout delay value.
 */
#define K_FOREVER       (-1)

/**
 * @brief Get system uptime (32-bit version).
 *
 * This routine returns the lower 32-bits of the elapsed time since the system
 * booted, in milliseconds.
 *
 * This routine can be more efficient than k_uptime_get(), as it reduces the
 * need for interrupt locking and 64-bit math. However, the 32-bit result
 * cannot hold a system uptime time larger than approximately 50 days, so the
 * caller must handle possible rollovers.
 *
 * @return Current uptime.
 */
u32_t k_uptime_get_32(void);

/**
 * @brief Get system uptime.
 *
 * This routine returns the elapsed time since the system booted,
 * in milliseconds.
 *
 * @return Current uptime.
 */
s64_t k_uptime_get(void);

void k_work_init(struct k_work *work, k_work_handler_t handler);

void k_delayed_work_init(struct k_delayed_work *work, k_work_handler_t handler);

int k_delayed_work_cancel(struct k_delayed_work *work);

void k_work_submit(struct k_work *work);

int k_delayed_work_submit(struct k_delayed_work *work, s32_t delay);

s32_t k_delayed_work_remaining_get(struct k_delayed_work *work);

#endif /* _BLE_MESH_KERNEL_H_ */

