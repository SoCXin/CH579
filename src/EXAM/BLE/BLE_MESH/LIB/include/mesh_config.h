#ifndef MESH_CONFIG_H
#define MESH_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "mesh_cfg_pub.h"

/* Support Mesh Relay Feature */
#define CONFIG_BLE_MESH_RELAY                                               1

/* Support Mesh Proxy Feature */
#define CONFIG_BLE_MESH_PROXY                                               1

/* Support Mesh Proxy Client Feature */
#define CONFIG_BLE_MESH_PROXY_CLI                                           1

/* Support Mesh Provisoning Over ADV Bearer */
#define CONFIG_BLE_MESH_PB_ADV                                              1

/* Support Mesh Provisioning Over GATT Bearer */
#define CONFIG_BLE_MESH_PB_GATT                                             1

/* Support Mesh Information Store&Restore */
#define CONFIG_BLE_MESH_SETTINGS                                          	1

/* Support Mesh Information Store&Restore Over Flash */
#define CONFIG_SETTINGS_NVS												    1

/* Maximum support bind application key count every mode */
#define CONFIG_BLE_MESH_MOD_KEY_COUNT                                       3

/* Maximum support subscription count every model */
#define CONFIG_BLE_MESH_MOD_GROUP_COUNT                                     6

/* Support Mesh Friend Feature */
#define CONFIG_BLE_MESH_FRIEND                                              1

/* Support Mesh Low Power Node Feature */
#define CONFIG_BLE_MESH_LOW_POWER                                           1

/* Maximum support subscription count in LPN mode */
#define CONFIG_BLE_MESH_LPN_GROUPS                                          8

/* Active IV Update Test Mode (Used by PTS) */
#define CONFIG_BLE_MESH_IV_UPDATE_TEST                                    	1

/* Support Mesh Message Hardware Encryption */
#define CONFIG_BLE_MESH_AES_HARDWARE										1

/* Support Mesh Provisioner Role */
#define CONFIG_BLE_MESH_PROVISIONER                                         1

/* Maximum support Provisioning device count */
#define CONFIG_BLE_MESH_NODE_COUNT                                          2

#ifdef __cplusplus
}
#endif

#endif
