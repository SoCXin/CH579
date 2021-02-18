/** @file
 *  @brief Bluetooth Mesh Configuration Client Model APIs.
 */

#ifndef _MESH_CFG_CLI_H_
#define _MESH_CFG_CLI_H_

/**
 * @brief Bluetooth Mesh
 * @defgroup bt_mesh_cfg_cli Bluetooth Mesh Configuration Client Model
 * @ingroup bt_mesh
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "mesh_types.h"
#include "mesh_kernel.h"

struct bt_mesh_cfg_comp_data
{
	struct net_buf_simple *buf;
};

struct bt_mesh_cfg_relay
{
	u8_t transmit;
};

/** @def BT_MESH_PUB_PERIOD_100MS
 *
 *  @brief Helper macro to encode model publication period in units of 100ms
 *
 *  @param steps Number of 100ms steps.
 *
 *  @return Encoded value that can be assigned to bt_mesh_cfg_mod_pub.period
 */
#define BT_MESH_PUB_PERIOD_100MS(steps)  ((steps) & BIT_MASK(6))

/** @def BT_MESH_PUB_PERIOD_SEC
 *
 *  @brief Helper macro to encode model publication period in units of 1 second
 *
 *  @param steps Number of 1 second steps.
 *
 *  @return Encoded value that can be assigned to bt_mesh_cfg_mod_pub.period
 */
#define BT_MESH_PUB_PERIOD_SEC(steps)   (((steps) & BIT_MASK(6)) | (1 << 6))

/** @def BT_MESH_PUB_PERIOD_10SEC
 *
 *  @brief Helper macro to encode model publication period in units of 10
 *  seconds
 *
 *  @param steps Number of 10 second steps.
 *
 *  @return Encoded value that can be assigned to bt_mesh_cfg_mod_pub.period
 */
#define BT_MESH_PUB_PERIOD_10SEC(steps) (((steps) & BIT_MASK(6)) | (2 << 6))

/** @def BT_MESH_PUB_PERIOD_10MIN
 *
 *  @brief Helper macro to encode model publication period in units of 10
 *  minutes
 *
 *  @param steps Number of 10 minute steps.
 *
 *  @return Encoded value that can be assigned to bt_mesh_cfg_mod_pub.period
 */
#define BT_MESH_PUB_PERIOD_10MIN(steps) (((steps) & BIT_MASK(6)) | (3 << 6))

struct bt_mesh_cfg_mod_pub {
	u16_t  addr;
	u16_t  app_idx;
	bool   cred_flag;
	u8_t   ttl;
	u8_t   period;
	u8_t   transmit;
};

struct bt_mesh_cfg_mod_pub_va {
	u16_t  addr[16];
	u16_t  app_idx;
	bool   cred_flag;
	u8_t   ttl;
	u8_t   period;
	u8_t   transmit;
};

struct bt_mesh_cfg_app_list
{
	u16_t elem_addr;
	u16_t mod_id;
	u16_t cid;
	const u16_t *app_lists;
};

struct bt_mesh_cfg_hb_sub {
	u16_t src;
	u16_t dst;
	u8_t  period;
	u8_t  count;
	u8_t  min;
	u8_t  max;
};

struct bt_mesh_cfg_hb_pub {
	u16_t dst;
	u8_t  count;
	u8_t  period;
	u8_t  ttl;
	u16_t feat;
	u16_t net_idx;
};

struct bt_mesh_cfg_lpn_timeout
{
	u16_t addr;
	u32_t polltimeout;
};

struct bt_mesh_cfg_net_transmit
{
	u8_t transmit;
};

struct bt_mesh_node_id
{
	u8_t identity;
	u16_t net_idx;
};

struct bt_mesh_krp
{
	u8_t phase;
	u16_t net_idx;
};

struct bt_mesh_app_key_list
{
	u16_t net_idx;
	const u16_t *app_key_lists;
};

struct bt_mesh_sub_list
{
	u16_t elem_addr;
	u16_t cid;
	u16_t mod_id;
	const u16_t *addr;
};

struct cfgEventHdr
{
	u8_t status;
	u32_t opcode;
};

union cfgEvent_t
{
	struct bt_mesh_cfg_comp_data comp;
	struct bt_mesh_cfg_relay relay;
	struct bt_mesh_cfg_mod_pub m_pub;
	struct bt_mesh_cfg_hb_pub h_pub;
	struct bt_mesh_cfg_hb_sub sub;
	struct bt_mesh_cfg_lpn_timeout lpn;
	struct bt_mesh_cfg_net_transmit transmit;
	struct bt_mesh_node_id id;
	struct bt_mesh_krp krp;
	struct bt_mesh_cfg_app_list aplist;
	struct bt_mesh_app_key_list apklist;
	struct bt_mesh_sub_list sublist;
};

typedef struct 
{
	struct cfgEventHdr cfgHdr;
	union cfgEvent_t cfgEvent;
}cfg_cli_status_t;

typedef void (*cfg_cli_rsp_handler_t)(const cfg_cli_status_t *val);

/** Mesh Configuration Client Model Context */
struct bt_mesh_cfg_cli {
	struct bt_mesh_model	*model;

	cfg_cli_rsp_handler_t	handler;

	u32_t	op_req;
	u32_t 	op_pending;

	struct k_delayed_work timeout;
};

#define BLE_MESH_MODEL_CFG_CLI(cli_data)	\
	BLE_MESH_MODEL_CB(BLE_MESH_MODEL_ID_CFG_CLI, NULL, NULL, cli_data, NULL)

extern int bt_mesh_cfg_comp_data_get(u16_t net_idx, u16_t addr, u8_t page);

extern int bt_mesh_cfg_beacon_get(u16_t net_idx, u16_t addr);

extern int bt_mesh_cfg_beacon_set(u16_t net_idx, u16_t addr, u8_t val);

extern int bt_mesh_cfg_ttl_get(u16_t net_idx, u16_t addr);

extern int bt_mesh_cfg_ttl_set(u16_t net_idx, u16_t addr, u8_t val);

extern int bt_mesh_cfg_lpn_polltimeout_get(u16_t net_idx, u16_t addr, u16_t lpn_addr);

extern int bt_mesh_cfg_friend_get(u16_t net_idx, u16_t addr);

extern int bt_mesh_cfg_friend_set(u16_t net_idx, u16_t addr, u8_t val);

extern int bt_mesh_cfg_gatt_proxy_get(u16_t net_idx, u16_t addr);

extern int bt_mesh_cfg_gatt_proxy_set(u16_t net_idx, u16_t addr, u8_t val);

extern int bt_mesh_cfg_net_transmit_get(u16_t net_idx, u16_t addr);

extern int bt_mesh_cfg_net_transmit_get(u16_t net_idx, u16_t addr);

extern int bt_mesh_cfg_net_transmit_set(u16_t net_idx, u16_t addr, u8_t val);

extern int bt_mesh_cfg_node_reset(u16_t net_idx, u16_t addr);

extern int bt_mesh_cfg_node_id_get(u16_t net_idx, u16_t addr, u16_t id_net_idx);

extern int bt_mesh_cfg_node_id_set(u16_t net_idx, u16_t addr, u16_t id_net_idx, u8_t val);

extern int bt_mesh_cfg_krp_get(u16_t net_idx, u16_t addr, u16_t krp_net_idx);

extern int bt_mesh_cfg_krp_set(u16_t net_idx, u16_t addr, u16_t krp_net_idx, u8_t val);

extern int bt_mesh_cfg_relay_get(u16_t net_idx, u16_t addr);

extern int bt_mesh_cfg_relay_set(u16_t net_idx, u16_t addr, u8_t new_relay, u8_t new_transmit);

extern int bt_mesh_cfg_net_key_get(u16_t net_idx, u16_t addr);

extern int bt_mesh_cfg_net_key_add(u16_t net_idx, u16_t addr, u16_t key_net_idx, const u8_t net_key[16]);

extern int bt_mesh_cfg_net_key_update(u16_t net_idx, u16_t addr, u16_t key_net_idx, const u8_t net_key[16]);

extern int bt_mesh_cfg_net_key_delete(u16_t net_idx, u16_t addr, u16_t key_net_idx);

extern int bt_mesh_cfg_app_key_add(u16_t net_idx, u16_t addr, u16_t key_net_idx, u16_t key_app_idx, const u8_t app_key[16]);

extern int bt_mesh_cfg_app_key_update(u16_t net_idx, u16_t addr, u16_t key_net_idx, u16_t key_app_idx, const u8_t app_key[16]);

extern int bt_mesh_cfg_app_key_delete(u16_t net_idx, u16_t addr, u16_t key_net_idx, u16_t key_app_idx);

extern int bt_mesh_cfg_app_key_get(u16_t net_idx, u16_t addr, u16_t key_net_idx);

extern int bt_mesh_cfg_mod_app_bind(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t mod_app_idx, u16_t mod_id);

extern int bt_mesh_cfg_mod_app_unbind(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t mod_app_idx, u16_t mod_id);

extern int bt_mesh_cfg_mod_app_bind_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t mod_app_idx, u16_t mod_id, u16_t cid);

extern int bt_mesh_cfg_mod_app_unbind_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t mod_app_idx, u16_t mod_id, u16_t cid);

extern int bt_mesh_cfg_mod_pub_get(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t mod_id);

extern int bt_mesh_cfg_mod_pub_get_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t mod_id, u16_t cid);

extern int bt_mesh_cfg_mod_pub_set(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t mod_id, const struct bt_mesh_cfg_mod_pub *pub);

extern int bt_mesh_cfg_mod_pub_set_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t mod_id, u16_t cid, const struct bt_mesh_cfg_mod_pub *pub);

extern int bt_mesh_cfg_mod_pub_va_set(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t mod_id, const struct bt_mesh_cfg_mod_pub_va *pub);

extern int bt_mesh_cfg_mod_pub_va_set_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t mod_id, u16_t cid, const struct bt_mesh_cfg_mod_pub_va *pub);

extern int bt_mesh_cfg_mod_sub_add(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t sub_addr, u16_t mod_id);

extern int bt_mesh_cfg_mod_sub_add_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t sub_addr, u16_t mod_id, u16_t cid);

extern int bt_mesh_cfg_mod_sub_get(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t mod_id);

extern int bt_mesh_cfg_mod_sub_get_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t mod_id, u16_t cid);

extern int bt_mesh_cfg_mod_sub_del(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t sub_addr, u16_t mod_id);

extern int bt_mesh_cfg_mod_sub_del_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t sub_addr, u16_t mod_id, u16_t cid);

extern int bt_mesh_cfg_mod_sub_delall(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t mod_id);

extern int bt_mesh_cfg_mod_sub_delall_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t mod_id, u16_t cid);

extern int bt_mesh_cfg_mod_sub_overwrite(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t sub_addr, u16_t mod_id);

extern int bt_mesh_cfg_mod_sub_overwrite_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t sub_addr, u16_t mod_id, u16_t cid);

extern int bt_mesh_cfg_mod_sub_va_add(u16_t net_idx, u16_t addr, u16_t elem_addr, const u8_t label[16], u16_t mod_id, u16_t virt_addr);

extern int bt_mesh_cfg_mod_sub_va_add_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr, const u8_t label[16], u16_t mod_id, u16_t cid, u16_t virt_addr);

extern int bt_mesh_cfg_mod_sub_va_del(u16_t net_idx, u16_t addr, u16_t elem_addr, const u8_t label[16], u16_t mod_id, u16_t virt_addr);

extern int bt_mesh_cfg_mod_sub_va_del_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr, const u8_t label[16], u16_t mod_id, u16_t cid, u16_t virt_addr);

extern int bt_mesh_cfg_mod_sub_va_overwrite(u16_t net_idx, u16_t addr, u16_t elem_addr, const u8_t label[16], u16_t mod_id, u16_t virt_addr);

extern int bt_mesh_cfg_mod_sub_va_overwrite_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr, const u8_t label[16], u16_t mod_id, u16_t cid, u16_t virt_addr);

extern int bt_mesh_cfg_mod_app_get_sig(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t mod_id);

extern int bt_mesh_cfg_mod_app_get_vnd(u16_t net_idx, u16_t addr, u16_t elem_addr, u16_t mod_id, u16_t cid);

extern int bt_mesh_cfg_hb_pub_set(u16_t net_idx, u16_t addr, const struct bt_mesh_cfg_hb_pub *pub);

extern int bt_mesh_cfg_hb_pub_get(u16_t net_idx, u16_t addr);

extern int bt_mesh_cfg_hb_sub_set(u16_t net_idx, u16_t addr, const struct bt_mesh_cfg_hb_sub *sub);

extern int bt_mesh_cfg_hb_sub_get(u16_t net_idx, u16_t addr);

extern s32_t bt_mesh_cfg_cli_timeout_get(void);

extern void bt_mesh_cfg_cli_timeout_set(s32_t timeout);

#ifdef __cplusplus
}
#endif
/**
 * @}
 */

#endif /* _MESH_CFG_CLI_H_ */
