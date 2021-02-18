/** @file
 *  @brief Bluetooth Mesh Configuration Server Model APIs.
 */

#ifndef _BLE_MESH_CFG_SRV_H_
#define _BLE_MESH_CFG_SRV_H_

#include "mesh_access.h"
/**
 * @brief Bluetooth Mesh
 * @defgroup bt_mesh_cfg_srv Bluetooth Mesh Configuration Server Model
 * @ingroup bt_mesh
 * @{
 */

/** Mesh Configuration Server Model Context */
struct bt_mesh_cfg_srv
{
    struct bt_mesh_model *model;

    u8_t net_transmit;     /* Network Transmit state */
    u8_t relay;            /* Relay Mode state */
    u8_t relay_retransmit; /* Relay Retransmit state */
    u8_t beacon;           /* Secure Network Beacon state */
    u8_t gatt_proxy;       /* GATT Proxy state */
    u8_t frnd;             /* Friend state */
    u8_t default_ttl;      /* Default TTL */

    struct bt_mesh_hb_pub hb_pub;

    struct bt_mesh_hb_sub hb_sub;
};

#define BLE_MESH_MODEL_CFG_SRV(srv_data)                                  \
    BLE_MESH_MODEL_CB(BLE_MESH_MODEL_ID_CFG_SRV, NULL, NULL, \
                     srv_data, NULL)
/**
 * @}
 */

#endif /* __BLE_MESH_CFG_SRV_H */
