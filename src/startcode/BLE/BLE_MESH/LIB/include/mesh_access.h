/** @file
 *  @brief Bluetooth Mesh Access Layer APIs.
 */

#ifndef _BLE_MESH_ACCESS_H_
#define _BLE_MESH_ACCESS_H_

#include "mesh_types.h"
#include "mesh_buf.h"
#include "mesh_config.h"
#include "mesh_kernel.h"
/**
 * @brief Bluetooth Mesh Access Layer
 * @defgroup bt_mesh_access Bluetooth Mesh Access Layer
 * @ingroup bt_mesh
 * @{
 */
 
#define BLE_MESH_KEY_PRIMARY 0x0000
#define BLE_MESH_KEY_ANY     0xffff

#define BLE_MESH_ADDR_UNASSIGNED 0x0000
#define BLE_MESH_ADDR_ALL_NODES 0xffff
#define BLE_MESH_ADDR_PROXIES 0xfffc
#define BLE_MESH_ADDR_FRIENDS 0xfffd
#define BLE_MESH_ADDR_RELAYS 0xfffe

#define BLE_MESH_KEY_UNUSED 0xffff
#define BLE_MESH_KEY_DEV 0xfffe
#define BLE_MESH_KEY_DEV_LOCAL BLE_MESH_KEY_DEV
#define BLE_MESH_KEY_DEV_REMOTE 0xfffd
#define BLE_MESH_KEY_DEV_ANY 0xfffc

#define BLE_MESH_IS_DEV_KEY(key) (key == BLE_MESH_KEY_DEV_LOCAL || \
                                  key == BLE_MESH_KEY_DEV_REMOTE)
/** Helper to define a mesh element within an array.
 *
 *  In case the element has no SIG or Vendor models the helper
 *  macro BLE_MESH_MODEL_NONE can be given instead.
 *
 *  @param _loc       Location Descriptor.
 *  @param _mods      Array of models.
 *  @param _vnd_mods  Array of vendor models.
 */
#define BLE_MESH_ELEM(_loc, _mods, _vnd_mods)     \
    {                                             \
        .loc = (_loc),                            \
        .model_count = ARRAY_SIZE(_mods),         \
        .models = (_mods),                        \
        .vnd_model_count = ARRAY_SIZE(_vnd_mods), \
        .vnd_models = (_vnd_mods),                \
    }

/** Helper to define a mesh element within an array.
 *
 *  In case the element has no SIG or Vendor models the helper
 *
 *  @param _loc       Location Descriptor.
 *  @param _mods      Array of models.
 *  @param _vnd_mods  Array of vendor models.
 */
#define BLE_MESH_ELEMENT(_loc, mod_cut, _mods, vnd_cut, _vnd_mods) \
    {                                                              \
        .loc = (_loc),                                             \
        .model_count = (mod_cut),                                  \
        .models = (_mods),                                         \
        .vnd_model_count = (vnd_cut),                              \
        .vnd_models = (_vnd_mods),                                 \
    }

/** Abstraction that describes a Mesh Element */
struct bt_mesh_elem
{
    /* Unicast Address. Set at runtime during provisioning. */
    u16_t addr;

    /* Location Descriptor (GATT Bluetooth Namespace Descriptors) */
    const u16_t loc;

    const u8_t model_count;
    const u8_t vnd_model_count;

    struct bt_mesh_model *const models;
    struct bt_mesh_model *const vnd_models;
};

/* Foundation Models */
#define BLE_MESH_MODEL_ID_CFG_SRV 0x0000
#define BLE_MESH_MODEL_ID_CFG_CLI 0x0001
#define BLE_MESH_MODEL_ID_HEALTH_SRV 0x0002
#define BLE_MESH_MODEL_ID_HEALTH_CLI 0x0003

/* Models from the Mesh Model Specification */
#define BLE_MESH_MODEL_ID_GEN_ONOFF_SRV 0x1000
#define BLE_MESH_MODEL_ID_GEN_ONOFF_CLI 0x1001
#define BLE_MESH_MODEL_ID_GEN_LEVEL_SRV 0x1002
#define BLE_MESH_MODEL_ID_GEN_LEVEL_CLI 0x1003
#define BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV 0x1004
#define BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI 0x1005
#define BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV 0x1006
#define BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV 0x1007
#define BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI 0x1008
#define BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV 0x1009
#define BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV 0x100a
#define BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI 0x100b
#define BLE_MESH_MODEL_ID_GEN_BATTERY_SRV 0x100c
#define BLE_MESH_MODEL_ID_GEN_BATTERY_CLI 0x100d
#define BLE_MESH_MODEL_ID_GEN_LOCATION_SRV 0x100e
#define BLE_MESH_MODEL_ID_GEN_LOCATION_SETUPSRV 0x100f
#define BLE_MESH_MODEL_ID_GEN_LOCATION_CLI 0x1010
#define BLE_MESH_MODEL_ID_GEN_ADMIN_PROP_SRV 0x1011
#define BLE_MESH_MODEL_ID_GEN_MANUFACTURER_PROP_SRV 0x1012
#define BLE_MESH_MODEL_ID_GEN_USER_PROP_SRV 0x1013
#define BLE_MESH_MODEL_ID_GEN_CLIENT_PROP_SRV 0x1014
#define BLE_MESH_MODEL_ID_GEN_PROP_CLI 0x1015
#define BLE_MESH_MODEL_ID_SENSOR_SRV 0x1100
#define BLE_MESH_MODEL_ID_SENSOR_SETUP_SRV 0x1101
#define BLE_MESH_MODEL_ID_SENSOR_CLI 0x1102
#define BLE_MESH_MODEL_ID_TIME_SRV 0x1200
#define BLE_MESH_MODEL_ID_TIME_SETUP_SRV 0x1201
#define BLE_MESH_MODEL_ID_TIME_CLI 0x1202
#define BLE_MESH_MODEL_ID_SCENE_SRV 0x1203
#define BLE_MESH_MODEL_ID_SCENE_SETUP_SRV 0x1204
#define BLE_MESH_MODEL_ID_SCENE_CLI 0x1205
#define BLE_MESH_MODEL_ID_SCHEDULER_SRV 0x1206
#define BLE_MESH_MODEL_ID_SCHEDULER_SETUP_SRV 0x1207
#define BLE_MESH_MODEL_ID_SCHEDULER_CLI 0x1208
#define BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV 0x1300
#define BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV 0x1301
#define BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI 0x1302
#define BLE_MESH_MODEL_ID_LIGHT_CTL_SRV 0x1303
#define BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV 0x1304
#define BLE_MESH_MODEL_ID_LIGHT_CTL_CLI 0x1305
#define BLE_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV 0x1306
#define BLE_MESH_MODEL_ID_LIGHT_HSL_SRV 0x1307
#define BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV 0x1308
#define BLE_MESH_MODEL_ID_LIGHT_HSL_CLI 0x1309
#define BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV 0x130a
#define BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV 0x130b
#define BLE_MESH_MODEL_ID_LIGHT_XYL_SRV 0x130c
#define BLE_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV 0x130d
#define BLE_MESH_MODEL_ID_LIGHT_XYL_CLI 0x130e
#define BLE_MESH_MODEL_ID_LIGHT_LC_SRV 0x130f
#define BLE_MESH_MODEL_ID_LIGHT_LC_SETUPSRV 0x1310
#define BLE_MESH_MODEL_ID_LIGHT_LC_CLI 0x1311

#define OP_APP_KEY_ADD                     BLE_MESH_MODEL_OP_1(0x00)
#define OP_APP_KEY_UPDATE                  BLE_MESH_MODEL_OP_1(0x01)
#define OP_DEV_COMP_DATA_STATUS            BLE_MESH_MODEL_OP_1(0x02)
#define OP_MOD_PUB_SET                     BLE_MESH_MODEL_OP_1(0x03)
#define OP_HEALTH_CURRENT_STATUS           BLE_MESH_MODEL_OP_1(0x04)
#define OP_HEALTH_FAULT_STATUS             BLE_MESH_MODEL_OP_1(0x05)
#define OP_HEARTBEAT_PUB_STATUS            BLE_MESH_MODEL_OP_1(0x06)
#define OP_APP_KEY_DEL                     BLE_MESH_MODEL_OP_2(0x80, 0x00)
#define OP_APP_KEY_GET                     BLE_MESH_MODEL_OP_2(0x80, 0x01)
#define OP_APP_KEY_LIST                    BLE_MESH_MODEL_OP_2(0x80, 0x02)
#define OP_APP_KEY_STATUS                  BLE_MESH_MODEL_OP_2(0x80, 0x03)
#define OP_ATTENTION_GET                   BLE_MESH_MODEL_OP_2(0x80, 0x04)
#define OP_ATTENTION_SET                   BLE_MESH_MODEL_OP_2(0x80, 0x05)
#define OP_ATTENTION_SET_UNREL             BLE_MESH_MODEL_OP_2(0x80, 0x06)
#define OP_ATTENTION_STATUS                BLE_MESH_MODEL_OP_2(0x80, 0x07)
#define OP_DEV_COMP_DATA_GET               BLE_MESH_MODEL_OP_2(0x80, 0x08)
#define OP_BEACON_GET                      BLE_MESH_MODEL_OP_2(0x80, 0x09)
#define OP_BEACON_SET                      BLE_MESH_MODEL_OP_2(0x80, 0x0a)
#define OP_BEACON_STATUS                   BLE_MESH_MODEL_OP_2(0x80, 0x0b)
#define OP_DEFAULT_TTL_GET                 BLE_MESH_MODEL_OP_2(0x80, 0x0c)
#define OP_DEFAULT_TTL_SET                 BLE_MESH_MODEL_OP_2(0x80, 0x0d)
#define OP_DEFAULT_TTL_STATUS              BLE_MESH_MODEL_OP_2(0x80, 0x0e)
#define OP_FRIEND_GET                      BLE_MESH_MODEL_OP_2(0x80, 0x0f)
#define OP_FRIEND_SET                      BLE_MESH_MODEL_OP_2(0x80, 0x10)
#define OP_FRIEND_STATUS                   BLE_MESH_MODEL_OP_2(0x80, 0x11)
#define OP_GATT_PROXY_GET                  BLE_MESH_MODEL_OP_2(0x80, 0x12)
#define OP_GATT_PROXY_SET                  BLE_MESH_MODEL_OP_2(0x80, 0x13)
#define OP_GATT_PROXY_STATUS               BLE_MESH_MODEL_OP_2(0x80, 0x14)
#define OP_KRP_GET                         BLE_MESH_MODEL_OP_2(0x80, 0x15)
#define OP_KRP_SET                         BLE_MESH_MODEL_OP_2(0x80, 0x16)
#define OP_KRP_STATUS                      BLE_MESH_MODEL_OP_2(0x80, 0x17)
#define OP_MOD_PUB_GET                     BLE_MESH_MODEL_OP_2(0x80, 0x18)
#define OP_MOD_PUB_STATUS                  BLE_MESH_MODEL_OP_2(0x80, 0x19)
#define OP_MOD_PUB_VA_SET                  BLE_MESH_MODEL_OP_2(0x80, 0x1a)
#define OP_MOD_SUB_ADD                     BLE_MESH_MODEL_OP_2(0x80, 0x1b)
#define OP_MOD_SUB_DEL                     BLE_MESH_MODEL_OP_2(0x80, 0x1c)
#define OP_MOD_SUB_DEL_ALL                 BLE_MESH_MODEL_OP_2(0x80, 0x1d)
#define OP_MOD_SUB_OVERWRITE               BLE_MESH_MODEL_OP_2(0x80, 0x1e)
#define OP_MOD_SUB_STATUS                  BLE_MESH_MODEL_OP_2(0x80, 0x1f)
#define OP_MOD_SUB_VA_ADD                  BLE_MESH_MODEL_OP_2(0x80, 0x20)
#define OP_MOD_SUB_VA_DEL                  BLE_MESH_MODEL_OP_2(0x80, 0x21)
#define OP_MOD_SUB_VA_OVERWRITE            BLE_MESH_MODEL_OP_2(0x80, 0x22)
#define OP_NET_TRANSMIT_GET                BLE_MESH_MODEL_OP_2(0x80, 0x23)
#define OP_NET_TRANSMIT_SET                BLE_MESH_MODEL_OP_2(0x80, 0x24)
#define OP_NET_TRANSMIT_STATUS             BLE_MESH_MODEL_OP_2(0x80, 0x25)
#define OP_RELAY_GET                       BLE_MESH_MODEL_OP_2(0x80, 0x26)
#define OP_RELAY_SET                       BLE_MESH_MODEL_OP_2(0x80, 0x27)
#define OP_RELAY_STATUS                    BLE_MESH_MODEL_OP_2(0x80, 0x28)
#define OP_MOD_SUB_GET                     BLE_MESH_MODEL_OP_2(0x80, 0x29)
#define OP_MOD_SUB_LIST                    BLE_MESH_MODEL_OP_2(0x80, 0x2a)
#define OP_MOD_SUB_GET_VND                 BLE_MESH_MODEL_OP_2(0x80, 0x2b)
#define OP_MOD_SUB_LIST_VND                BLE_MESH_MODEL_OP_2(0x80, 0x2c)
#define OP_LPN_TIMEOUT_GET                 BLE_MESH_MODEL_OP_2(0x80, 0x2d)
#define OP_LPN_TIMEOUT_STATUS              BLE_MESH_MODEL_OP_2(0x80, 0x2e)
#define OP_HEALTH_FAULT_CLEAR              BLE_MESH_MODEL_OP_2(0x80, 0x2f)
#define OP_HEALTH_FAULT_CLEAR_UNREL        BLE_MESH_MODEL_OP_2(0x80, 0x30)
#define OP_HEALTH_FAULT_GET                BLE_MESH_MODEL_OP_2(0x80, 0x31)
#define OP_HEALTH_FAULT_TEST               BLE_MESH_MODEL_OP_2(0x80, 0x32)
#define OP_HEALTH_FAULT_TEST_UNREL         BLE_MESH_MODEL_OP_2(0x80, 0x33)
#define OP_HEALTH_PERIOD_GET               BLE_MESH_MODEL_OP_2(0x80, 0x34)
#define OP_HEALTH_PERIOD_SET               BLE_MESH_MODEL_OP_2(0x80, 0x35)
#define OP_HEALTH_PERIOD_SET_UNREL         BLE_MESH_MODEL_OP_2(0x80, 0x36)
#define OP_HEALTH_PERIOD_STATUS            BLE_MESH_MODEL_OP_2(0x80, 0x37)
#define OP_HEARTBEAT_PUB_GET               BLE_MESH_MODEL_OP_2(0x80, 0x38)
#define OP_HEARTBEAT_PUB_SET               BLE_MESH_MODEL_OP_2(0x80, 0x39)
#define OP_HEARTBEAT_SUB_GET               BLE_MESH_MODEL_OP_2(0x80, 0x3a)
#define OP_HEARTBEAT_SUB_SET               BLE_MESH_MODEL_OP_2(0x80, 0x3b)
#define OP_HEARTBEAT_SUB_STATUS            BLE_MESH_MODEL_OP_2(0x80, 0x3c)
#define OP_MOD_APP_BIND                    BLE_MESH_MODEL_OP_2(0x80, 0x3d)
#define OP_MOD_APP_STATUS                  BLE_MESH_MODEL_OP_2(0x80, 0x3e)
#define OP_MOD_APP_UNBIND                  BLE_MESH_MODEL_OP_2(0x80, 0x3f)
#define OP_NET_KEY_ADD                     BLE_MESH_MODEL_OP_2(0x80, 0x40)
#define OP_NET_KEY_DEL                     BLE_MESH_MODEL_OP_2(0x80, 0x41)
#define OP_NET_KEY_GET                     BLE_MESH_MODEL_OP_2(0x80, 0x42)
#define OP_NET_KEY_LIST                    BLE_MESH_MODEL_OP_2(0x80, 0x43)
#define OP_NET_KEY_STATUS                  BLE_MESH_MODEL_OP_2(0x80, 0x44)
#define OP_NET_KEY_UPDATE                  BLE_MESH_MODEL_OP_2(0x80, 0x45)
#define OP_NODE_IDENTITY_GET               BLE_MESH_MODEL_OP_2(0x80, 0x46)
#define OP_NODE_IDENTITY_SET               BLE_MESH_MODEL_OP_2(0x80, 0x47)
#define OP_NODE_IDENTITY_STATUS            BLE_MESH_MODEL_OP_2(0x80, 0x48)
#define OP_NODE_RESET                      BLE_MESH_MODEL_OP_2(0x80, 0x49)
#define OP_NODE_RESET_STATUS               BLE_MESH_MODEL_OP_2(0x80, 0x4a)
#define OP_SIG_MOD_APP_GET                 BLE_MESH_MODEL_OP_2(0x80, 0x4b)
#define OP_SIG_MOD_APP_LIST                BLE_MESH_MODEL_OP_2(0x80, 0x4c)
#define OP_VND_MOD_APP_GET                 BLE_MESH_MODEL_OP_2(0x80, 0x4d)
#define OP_VND_MOD_APP_LIST                BLE_MESH_MODEL_OP_2(0x80, 0x4e)

 /* Heartbeat Publication */
struct bt_mesh_hb_pub {
	struct k_delayed_work timer;

    u16_t dst;
    u16_t count;
    u8_t  period;
    u8_t  ttl;
    u16_t feat;
    u16_t net_idx;
};

/* Heartbeat Subscription */
struct bt_mesh_hb_sub {
    s64_t  expiry;

    u16_t src;
    u16_t dst;
    u16_t count;
    u8_t  min_hops;
    u8_t  max_hops;

    /* Optional subscription tracking function */
    void (*func)(u8_t hops, u16_t feat);
};

/** Message sending context. */
struct bt_mesh_msg_ctx
{
    /** NetKey Index of the subnet to send the message on. */
    u16_t net_idx;

    /** AppKey Index to encrypt the message with. */
    u16_t app_idx;

    /** Remote address. */
    u16_t addr;

    /** Destination address of a received message. Not used for sending. */
    u16_t recv_dst;

    /** RSSI of received packet. Not used for sending. */
    s8_t recv_rssi;

    /** Received TTL value. Not used for sending. */
    u8_t recv_ttl : 7;

    /** Force sending reliably by using segment acknowledgement */
    u8_t send_rel : 1;

    /** TTL, or BLE_MESH_TTL_DEFAULT for default TTL. */
    u8_t send_ttl;
};

struct bt_mesh_model_op
{
    /* OpCode encoded using the BLE_MESH_MODEL_OP_* macros */
    const u32_t opcode;

    /* Minimum required message length */
    const size_t min_len;

    /* Message handler for the opcode */
    void (*const func)(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple *buf);
};

#define BLE_MESH_MODEL_OP_1(b0) (b0)
#define BLE_MESH_MODEL_OP_2(b0, b1) (((b0) << 8) | (b1))
#define BLE_MESH_MODEL_OP_3(b0, cid) ((((b0) << 16) | 0xc00000) | (cid))

#define BLE_MESH_MODEL_OP_END \
    {                         \
        0, 0, NULL            \
    }
#define BLE_MESH_MODEL_NO_OPS ((struct bt_mesh_model_op[]){BLE_MESH_MODEL_OP_END})

/** Helper to define an empty model array 
* WARN 1/2 -> 0 so sizeof `struct bt_mesh_model *` 
* must less than `struct bt_mesh_model`*/
#define BLE_MESH_MODEL_NONE ((struct bt_mesh_model *)NULL)

/** @def BLE_MESH_MODEL_CB
 *
 * @brief Composition data SIG model entry with callback functions.
 *
 * @param _id Model ID.
 * @param _op Array of model opcode handlers.
 * @param _pub Model publish parameters.
 * @param _user_data User data for the model.
 * @param _cb Callback structure, or NULL to keep no callbacks.
 */
#define BLE_MESH_MODEL_CB(_id, _op, _pub, _user_data, _cb) \
    {                                                      \
        .id = (_id),                                       \
        .op = _op,                                         \
        .keys = {BLE_MESH_KEY_UNUSED},                     \
        .pub = _pub,                                       \
        .groups = {BLE_MESH_ADDR_UNASSIGNED},              \
        .user_data = _user_data,                           \
        .cb = _cb,                                         \
    }

/** @def BLE_MESH_MODEL_VND_CB
 *
 * @brief Composition data vendor model entry with callback functions.
 *
 * @param _company Company ID.
 * @param _id Model ID.
 * @param _op Array of model opcode handlers.
 * @param _pub Model publish parameters.
 * @param _user_data User data for the model.
 * @param _cb Callback structure, or NULL to keep no callbacks.
 */
#define BLE_MESH_MODEL_VND_CB(_company, _id, _op, _pub, _user_data, _cb) \
    {                                                                    \
        .vnd.company = (_company),                                       \
        .vnd.id = (_id),                                                 \
        .op = _op,                                                       \
        .pub = _pub,                                                     \
        .keys = {BLE_MESH_KEY_UNUSED},                                   \
        .groups = {BLE_MESH_ADDR_UNASSIGNED},                            \
        .user_data = _user_data,                                         \
        .cb = _cb,                                                       \
    }

/** @def BLE_MESH_MODEL
 *
 * @brief Composition data SIG model entry.
 *
 * @param _id Model ID.
 * @param _op Array of model opcode handlers.
 * @param _pub Model publish parameters.
 * @param _user_data User data for the model.
 */
#define BLE_MESH_MODEL(_id, _op, _pub, _user_data) \
    BLE_MESH_MODEL_CB(_id, _op, _pub, _user_data, NULL)

/** @def BLE_MESH_MODEL_VND
 *
 * @brief Composition data vendor model entry.
 *
 * @param _company Company ID.
 * @param _id Model ID.
 * @param _op Array of model opcode handlers.
 * @param _pub Model publish parameters.
 * @param _user_data User data for the model.
 */
#define BLE_MESH_MODEL_VND(_company, _id, _op, _pub, _user_data) \
    BLE_MESH_MODEL_VND_CB(_company, _id, _op, _pub, _user_data, NULL)

/** @def BLE_MESH_TRANSMIT
 *
 *  @brief Encode transmission count & interval steps.
 *
 *  @param count   Number of retransmissions (first transmission is excluded).
 *  @param int_ms  Interval steps in milliseconds. Must be greater than 0
 *                 and a multiple of 10.
 *
 *  @return Mesh transmit value that can be used e.g. for the default
 *          values of the configuration model data.
 */
#define BLE_MESH_TRANSMIT(count, int_ms) ((count) | (((int_ms / 10) - 1) << 3))

/** @def BLE_MESH_TRANSMIT_COUNT
 *
 *  @brief Decode transmit count from a transmit value.
 *
 *  @param transmit Encoded transmit count & interval value.
 *
 *  @return Transmission count (actual transmissions is N + 1).
 */
#define BLE_MESH_TRANSMIT_COUNT(transmit) (((transmit) & (u8_t)BIT_MASK(3)))

/** @def BLE_MESH_TRANSMIT_INT
 *
 *  @brief Decode transmit interval from a transmit value.
 *
 *  @param transmit Encoded transmit count & interval value.
 *
 *  @return Transmission interval in milliseconds.
 */
#define BLE_MESH_TRANSMIT_INT(transmit) ((((transmit) >> 3) + 1) * 10)

/** @def BLE_MESH_PUB_TRANSMIT
 *
 *  @brief Encode Publish Retransmit count & interval steps.
 *
 *  @param count   Number of retransmissions (first transmission is excluded).
 *  @param int_ms  Interval steps in milliseconds. Must be greater than 0
 *                 and a multiple of 50.
 *
 *  @return Mesh transmit value that can be used e.g. for the default
 *          values of the configuration model data.
 */
#define BLE_MESH_PUB_TRANSMIT(count, int_ms) BLE_MESH_TRANSMIT(count, (int_ms) / 5)

/** @def BLE_MESH_PUB_TRANSMIT_COUNT
 *
 *  @brief Decode Pubhlish Retransmit count from a given value.
 *
 *  @param transmit Encoded Publish Retransmit count & interval value.
 *
 *  @return Retransmission count (actual transmissions is N + 1).
 */
#define BLE_MESH_PUB_TRANSMIT_COUNT(transmit) BLE_MESH_TRANSMIT_COUNT(transmit)

/** @def BLE_MESH_PUB_TRANSMIT_INT
 *
 *  @brief Decode Publish Retransmit interval from a given value.
 *
 *  @param transmit Encoded Publish Retransmit count & interval value.
 *
 *  @return Transmission interval in milliseconds.
 */
#define BLE_MESH_PUB_TRANSMIT_INT(transmit) ((((transmit) >> 3) + 1) * 50)

/** Model publication context. */
struct bt_mesh_model_pub
{
    /** The model the context belongs to. Initialized by the stack. */
    struct bt_mesh_model *mod;
    u16_t addr;         /**< Publish Address. */
	u16_t key:12,       /**< Publish AppKey Index. */
	      cred:1,       /**< Friendship Credentials Flag. */
	      send_rel:1;   /**< Force reliable sending (segment acks) */

    u8_t  ttl;          /**< Publish Time to Live. */
    u8_t  retransmit;   /**< Retransmit Count & Interval Steps. */
    u8_t  period;       /**< Publish Period. */
    u16_t period_div: 4, /**< Divisor for the Period. */
          fast_period: 1, /**< Use FastPeriodDivisor */
          count: 3;     /**< Retransmissions left. */


    u32_t period_start; /**< Start of the current period. */

    /** @brief Publication buffer, containing the publication message.
     *
     *  This will get correctly created when the publication context
     *  has been defined using the BLE_MESH_MODEL_PUB_DEFINE macro.
     *
     *  BLE_MESH_MODEL_PUB_DEFINE(name, update, size);
     */
    struct net_buf_simple *msg;

    /** @brief Callback for updating the publication buffer.
     *
     *  When set to NULL, the model is assumed not to support
     *  periodic publishing. When set to non-NULL the callback
     *  will be called periodically and is expected to update
     *  @ref bt_mesh_model_pub.msg with a valid publication
     *  message.
     *
     *  @param mod The Model the Publication Context belogs to.
     *
     *  @return Zero on success or (negative) error code otherwise.
     */
    int (*update)(struct bt_mesh_model *mod);

    /** Publish Period Timer. Only for stack-internal use. */
    struct k_delayed_work timer;
};

/** @def BLE_MESH_MODEL_PUB_DEFINE
 *
 *  Define a model publication context.
 *
 *  @param _name Variable name given to the context.
 *  @param _update Optional message update callback (may be NULL).
 *  @param _msg_len Length of the publication message.
 */
#define BLE_MESH_MODEL_PUB_DEFINE(_name, _update, _msg_len)          \
    NET_BUF_SIMPLE_DEFINE_STATIC(bt_mesh_pub_msg_##_name, _msg_len); \
    static struct bt_mesh_model_pub _name = {                        \
        .update = _update,                                           \
        .msg = &bt_mesh_pub_msg_##_name,                             \
    }

typedef ssize_t (*settings_read_cb)(void *cb_arg, void *data, size_t len);

/** Model callback functions. */
struct bt_mesh_model_cb
{
    /** @brief Set value handler of user data tied to the model.
	 *
	 * @sa settings_handler::h_set
	 *
	 * @param model Model to set the persistent data of.
	 * @param len_rd The size of the data found in the backend.
	 * @param read_cb Function provided to read the data from the backend.
	 * @param cb_arg Arguments for the read function provided by the
	 * backend.
	 *
	 * @return 0 on success, error otherwise.
	 */
    int (*const settings_set)(struct bt_mesh_model *model,
                              size_t len_rd, settings_read_cb read_cb,
                              void *cb_arg);

    /** @brief Callback called when the mesh is started.
	 *
	 *  This handler gets called after the node has been provisioned, or
	 *  after all mesh data has been loaded from persistent storage.
	 *
	 *  When this callback fires, the mesh model may start its behavior,
	 *  and all Access APIs are ready for use.
	 *
	 *  @param model      Model this callback belongs to.
	 *  @param first_time Flag indicating whether this is the first time the
	 *                    node is started. If false, provisioning data was
	 *                    loaded from persistent storage.
     */
	int (*const start)(struct bt_mesh_model *model, bool first_time);

    /** @brief Model init callback.
	 *
	 * Called on every model instance during mesh initialization.
	 *
	 * @param model Model to be initialized.
	 *
	 * @return 0 on success, error otherwise.
	 */
    int (*const init)(struct bt_mesh_model *model);

    /** @brief Model reset callback.
	 *
	 * Called when the mesh node is reset. All model data is deleted on
	 * reset, and the model should clear its state.
	 *
	 * @param model Model this callback belongs to.
	 */
    void (*const reset)(struct bt_mesh_model *model);
};

#pragma anon_unions

/** Abstraction that describes a Mesh Model instance */
struct bt_mesh_model
{
    union {
        const u16_t id;
        struct
        {
            u16_t id;
            u16_t company;
        } vnd;
    };

    /* Internal information, mainly for persistent storage */
    u8_t elem_idx;  /* Belongs to Nth element */
    u8_t model_idx; /* Is the Nth model in the element */
    u16_t flags;    /* Information about what has changed */

    /* Model Publication */
    struct bt_mesh_model_pub *const pub;

    /* AppKey List */
    u16_t keys[CONFIG_BLE_MESH_MOD_KEY_COUNT];

    /* Subscription List (group or virtual addresses) */
    u16_t groups[CONFIG_BLE_MESH_MOD_GROUP_COUNT];

    const struct bt_mesh_model_op *op;

    /* Model callback structure. */
    const struct bt_mesh_model_cb *cb;

    /* Model-specific user data */
    void *user_data;
};

struct bt_mesh_send_cb
{
    void (*start)(u16_t duration, int err, void *cb_data);
    void (*end)(int err, void *cb_data);
};

extern void bt_mesh_model_msg_init(struct net_buf_simple *msg, u32_t opcode);

/** Special TTL value to request using configured default TTL */
#define BLE_MESH_TTL_DEFAULT 0xff

/** Maximum allowed TTL value */
#define BLE_MESH_TTL_MAX 0x7f

/**
 * @brief Send an Access Layer message.
 *
 * @param model     Mesh (client) Model that the message belongs to.
 * @param ctx       Message context, includes keys, TTL, etc.
 * @param msg       Access Layer payload (the actual message to be sent).
 * @param cb        Optional "message sent" callback.
 * @param cb_data   User data to be passed to the callback.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
extern int bt_mesh_model_send(struct bt_mesh_model *model,	        \
                          struct bt_mesh_msg_ctx *ctx,				\
                          struct net_buf_simple *msg,				\
                          const struct bt_mesh_send_cb *cb,			\
                          void *cb_data);

/**
 * @brief Send a model publication message.
 *
 * Before calling this function, the user needs to ensure that the model
 * publication message (@ref bt_mesh_model_pub.msg) contains a valid
 * message to be sent. Note that this API is only to be used for
 * non-period publishing. For periodic publishing the app only needs
 * to make sure that @ref bt_mesh_model_pub.msg contains a valid message
 * whenever the @ref bt_mesh_model_pub.update callback is called.
 *
 * @param model  Mesh (client) Model that's publishing the message.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
extern int bt_mesh_model_publish(struct bt_mesh_model *model);

/**
 * @brief Get the element that a model belongs to.
 *
 * @param mod  Mesh model.
 *
 * @return Pointer to the element that the given model belongs to.
 */
extern struct bt_mesh_elem * bt_mesh_model_elem(struct bt_mesh_model *mod);

/** @brief Find a SIG model.
 *
 * @param elem Element to search for the model in.
 * @param id Model ID of the model.
 *
 * @return A pointer to the Mesh model matching the given parameters, or NULL
 * if no SIG model with the given ID exists in the given element.
 */
extern struct bt_mesh_model * bt_mesh_model_find(const struct bt_mesh_elem *elem, u16_t id);

/** @brief Find a vendor model.
 *
 * @param elem Element to search for the model in.
 * @param company Company ID of the model.
 * @param id Model ID of the model.
 *
 * @return A pointer to the Mesh model matching the given parameters, or NULL
 * if no vendor model with the given ID exists in the given element.
 */
extern struct bt_mesh_model * bt_mesh_model_find_vnd(const struct bt_mesh_elem *elem, u16_t company, u16_t id);

/** @brief Get whether the model is in the primary element of the device.
 *
 * @param mod Mesh model.
 *
 * @return true if the model is on the primary element, false otherwise.
 */
static inline bool bt_mesh_model_in_primary(const struct bt_mesh_model *mod)
{
    return (mod->elem_idx == 0);
}

/** @brief Immediately store the model's user data in persistent storage.
 *
 * @param mod Mesh model.
 * @param vnd This is a vendor model.
 * @param data Model data to store, or NULL to delete any model data.
 * @param data_len Length of the model data.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
extern int bt_mesh_model_data_store(struct bt_mesh_model *mod, bool vnd, const void *data, size_t data_len);
/** Node Composition */
struct bt_mesh_comp
{
    u16_t cid;
    u16_t pid;
    u16_t vid;

    size_t elem_count;
    struct bt_mesh_elem *elem;
};
/**
 * @}
 */

#endif /* __BLE_MESH_ACCESS_H */
