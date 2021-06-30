/** @file
 *  @brief Bluetooth Mesh Health Client Model APIs.
 */

#ifndef _MESH_HEALTH_CLI_H_
#define _MESH_HEALTH_CLI_H_

/**
 * @brief Bluetooth Mesh
 * @defgroup bt_mesh_health_cli Bluetooth Mesh Health Client Model
 * @ingroup bt_mesh
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "mesh_types.h"
#include "mesh_kernel.h"

struct hlthEventHdr
{
	u32_t opcode;
	u8_t status;
};

struct bt_mesh_cfg_fault_status {
	u8_t test_id;
	u16_t company_id;
	size_t fault_count;
	const u8_t * fault_array;
};

struct health_attention_status {
	u8_t attention;
};

struct health_period_status {
	u8_t divisor;
};

union hlthEvent_t
{
	struct bt_mesh_cfg_fault_status fault;
	struct health_attention_status atten;
	struct health_period_status div;
};

typedef struct 
{
	struct hlthEventHdr cfgHdr;
	union hlthEvent_t cfgEvent;
}cfg_hlth_status_t;

typedef void (*health_rsp_handler_t)(const cfg_hlth_status_t *);

/** Mesh Health Client Model Context */
struct bt_mesh_health_cli {
	struct bt_mesh_model *model;

	void (*current_status)(struct bt_mesh_health_cli *cli, u16_t addr,
			       u8_t test_id, u16_t cid, u8_t *faults,
			       size_t fault_count);
	
	u32_t op_req;
	u32_t op_pending;

	health_rsp_handler_t handler;

	struct k_delayed_work timeout;
};

#define BLE_MESH_MODEL_HEALTH_CLI(cli_data) \
	BLE_MESH_MODEL_CB(BLE_MESH_MODEL_ID_HEALTH_CLI, NULL, NULL, cli_data, NULL)

extern int bt_mesh_health_cli_set(struct bt_mesh_model *model);

extern int bt_mesh_health_fault_get(u16_t net_idx, u16_t addr, u16_t app_idx, u16_t company_id);

extern int bt_mesh_health_fault_clear(u16_t net_idx, u16_t addr, u16_t app_idx, u16_t company_id);

extern int bt_mesh_health_fault_clear_unack(u16_t net_idx, u16_t addr, u16_t app_idx, u16_t company_id);

extern int bt_mesh_health_fault_test(u16_t net_idx, u16_t addr, u16_t app_idx, const u8_t * test_id, u16_t company_id);

extern int bt_mesh_health_fault_test_unack(u16_t net_idx, u16_t addr, u16_t app_idx, const u8_t * test_id, u16_t company_id);

extern int bt_mesh_health_period_get(u16_t net_idx, u16_t addr, u16_t app_idx);

extern int bt_mesh_health_period_set(u16_t net_idx, u16_t addr, u16_t app_idx, u8_t divisor);

extern int bt_mesh_health_period_set_unack(u16_t net_idx, u16_t addr, u16_t app_idx, u8_t divisor);

extern int bt_mesh_health_attention_get(u16_t net_idx, u16_t addr, u16_t app_idx);

extern int bt_mesh_health_attention_set(u16_t net_idx, u16_t addr, u16_t app_idx, u8_t attention);

extern int bt_mesh_health_attention_set_unack(u16_t net_idx, u16_t addr, u16_t app_idx, u8_t attention);

extern s32_t bt_mesh_health_cli_timeout_get(void);

extern void bt_mesh_health_cli_timeout_set(s32_t timeout);

#ifdef __cplusplus
}
#endif
/**
 * @}
 */

#endif /* _MESH_HEALTH_CLI_H_ */
