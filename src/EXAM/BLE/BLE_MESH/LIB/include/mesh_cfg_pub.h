/* Header guard */
#ifndef MESH_CFG_PUB_H__
#define MESH_CFG_PUB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mesh_types.h"

typedef struct
{
    u8_t * base_addr;
    u32_t mem_len;
}mem_info_t;

typedef struct
{
	u8_t adv_buf_count;
	u8_t rpl_count;
	u8_t ivu_divider;
} ble_mesh_common_cfg_t;

typedef struct
{
	u8_t pxyfilter_count;
} ble_mesh_proxy_cfg_t;

typedef struct
{
	u8_t msgcache_count;

	u8_t subnet_count;
	u8_t appkey_count;

	u8_t txseg_max;
	u8_t txseg_count;

	u8_t rxseg_count;
	u16_t rxsdu_max;

	u8_t label_count;
} ble_mesh_net_cfg_t;

typedef struct
{
	u16_t seq_store_rate;
	u16_t rpl_store_rate;
	u16_t store_rate;
} ble_mesh_store_cfg_t;

typedef struct
{
	u8_t frndseg_rx;
	u8_t frndsub_size;
	u8_t frndlpn_count;
	u8_t frndqueue_size;
	u16_t frndrecv_win;
} ble_mesh_friend_cfg_t;

typedef struct
{
	u8_t lpnmin_size;
	u8_t lpnrssi_factor;
	u8_t lpnrecv_factor;
	u16_t lpnpoll_timeout;
	u16_t lpnrecv_delay;
	u16_t lpnretry_timeout;
} ble_mesh_lpn_cfg_t;

typedef struct
{
	u8_t node_count;
} ble_mesh_prov_cfg_t;

typedef struct
{
	ble_mesh_common_cfg_t common_cfg;
	ble_mesh_proxy_cfg_t proxy_cfg;
	ble_mesh_net_cfg_t net_cfg;
	ble_mesh_store_cfg_t store_cfg;
	ble_mesh_friend_cfg_t friend_cfg;
	ble_mesh_lpn_cfg_t lpn_cfg;
	ble_mesh_prov_cfg_t prov_cfg;
} ble_mesh_cfg_t;

/**@brief Net Cached Size */
#define CONFIG_MESH_ADV_BUF_COUNT_MIN 			(6)
#define CONFIG_MESH_ADV_BUF_COUNT_DEF 			(10)
#define CONFIG_MESH_ADV_BUF_COUNT_MAX 			(40)

/**@brief Rpl Size */
#define CONFIG_MESH_RPL_COUNT_MIN 				(6)
#define CONFIG_MESH_RPL_COUNT_DEF 				(6)
#define CONFIG_MESH_RPL_COUNT_MAX 				(20)

/**@brief IV Update State Timer Divider */
#define CONFIG_MESH_IVU_DIVIDER_MIN 			(1)
#define CONFIG_MESH_IVU_DIVIDER_DEF 			(96)
#define CONFIG_MESH_IVU_DIVIDER_MAX 			(96)

/**@brief Proxy Filter Size */
#define CONFIG_MESH_PROXY_FILTER_MIN 			(2)
#define CONFIG_MESH_PROXY_FILTER_DEF 			(5)
#define CONFIG_MESH_PROXY_FILTER_MAX 			(20)

/**@brief Message Cache Size */
#define CONFIG_MESH_MSG_CACHE_MIN 				(3)
#define CONFIG_MESH_MSG_CACHE_DEF 				(20)
#define CONFIG_MESH_MSG_CACHE_MAX 				(40)

/**@brief Subnet Count */
#define CONFIG_MESH_SUBNET_COUNT_MIN 			(1)
#define CONFIG_MESH_SUBNET_COUNT_DEF 			(2)
#define CONFIG_MESH_SUBNET_COUNT_MAX 			(4)

/**@brief Application Keys Count */
#define CONFIG_MESH_APPKEY_COUNT_MIN 			(1)
#define CONFIG_MESH_APPKEY_COUNT_DEF 			(3)
#define CONFIG_MESH_APPKEY_COUNT_MAX 			(5)

/**@brief Number of segment packets transmitted per Message */
#define CONFIG_MESH_TX_SEG_MIN 					(2)
#define CONFIG_MESH_TX_SEG_DEF 					(8)
#define CONFIG_MESH_TX_SEG_MAX 					(32)

/**@brief Number of transmitted seg-message concurrently */
#define CONFIG_MESH_TX_SEG_COUNT_MIN 			(1)
#define CONFIG_MESH_TX_SEG_COUNT_DEF 			(2)
#define CONFIG_MESH_TX_SEG_COUNT_MAX 			(4)

/**@brief Number of received seg-message concurrently */
#define CONFIG_MESH_RX_SEG_COUNT_MIN 			(1)
#define CONFIG_MESH_RX_SEG_COUNT_DEF 			(2)
#define CONFIG_MESH_RX_SEG_COUNT_MAX 			(4)

/**@brief Number octers of segment packets received per Message */
#define CONFIG_MESH_RX_SDU_MIN 					(12)
#define CONFIG_MESH_RX_SDU_DEF 					(192)
#define CONFIG_MESH_RX_SDU_MAX 					(384)

/**@brief Number of Virtual Address */
#define CONFIG_MESH_LABEL_COUNT_MIN 			(1)
#define CONFIG_MESH_LABEL_COUNT_DEF 			(2)
#define CONFIG_MESH_LABEL_COUNT_MAX 			(4)

/**@brief Number of Sector Count Used for Store Infomation */
#define CONFIG_MESH_SECTOR_COUNT_MIN 			(2)
#define CONFIG_MESH_SECTOR_COUNT_DEF 			(3)

/**@brief Default Address of Store Mesh Infomation */
#define CONFIG_MESH_NVS_ADDR_DEF				(0x3E800)

/**@brief Number of seconds to store after RPL has been updated */
#define CONFIG_MESH_RPL_STORE_RATE_MIN 			(5)
#define CONFIG_MESH_RPL_STORE_RATE_DEF 			(60)
#define CONFIG_MESH_RPL_STORE_RATE_MAX 			(3600)

/**@brief Number of seconds to store after seq has been updated */
#define CONFIG_MESH_SEQ_STORE_RATE_MIN 			(5)
#define CONFIG_MESH_SEQ_STORE_RATE_DEF 			(60)
#define CONFIG_MESH_SEQ_STORE_RATE_MAX 			(3600)

/**@brief Number of seconds to store after any information has been updated */
#define CONFIG_MESH_STORE_RATE_MIN 				(2)
#define CONFIG_MESH_STORE_RATE_DEF 				(2)
#define CONFIG_MESH_STORE_RATE_MAX 				(5)

/**@brief Number of segment packets received per Message */
#define CONFIG_MESH_FRIEND_SEG_RX_COUNT_MIN 	(1)
#define CONFIG_MESH_FRIEND_SEG_RX_COUNT_DEF 	(2)
#define CONFIG_MESH_FRIEND_SEG_RX_COUNT_MAX 	(4)

/**@brief Number of Subscribe size support by friend */
#define CONFIG_MESH_FRIEND_SUB_SIZE_MIN 		(1)
#define CONFIG_MESH_FRIEND_SUB_SIZE_DEF 		(4)
#define CONFIG_MESH_FRIEND_SUB_SIZE_MAX 		(8)

/**@brief Number of Friendship support by friend */
#define CONFIG_MESH_FRIEND_LPN_COUNT_MIN 		(1)
#define CONFIG_MESH_FRIEND_LPN_COUNT_DEF 		(1)
#define CONFIG_MESH_FRIEND_LPN_COUNT_MAX 		(4)

/**@brief Number of Cache Mesh Message support by friend */
#define CONFIG_MESH_QUEUE_SIZE_MIN 				(2)
#define CONFIG_MESH_QUEUE_SIZE_DEF 				(4)
#define CONFIG_MESH_QUEUE_SIZE_MAX 				(30)

/**@brief Friend receive window */
#define CONFIG_MESH_FRIEND_RECV_WIN_MIN 		(1)
#define CONFIG_MESH_FRIEND_RECV_WIN_DEF 		(10)
#define CONFIG_MESH_FRIEND_RECV_WIN_MAX 		(255)

/**@brief LPN Required Queue size(Log) of Friend */
#define CONFIG_MESH_LPN_REQ_QUEUE_SIZE_MIN 		(2)
#define CONFIG_MESH_LPN_REQ_QUEUE_SIZE_DEF 		(2)
#define CONFIG_MESH_LPN_REQ_QUEUE_SIZE_MAX 		(20)

/**@brief LPN Polltimeout */
#define CONFIG_MESH_LPN_POLLTIMEOUT_MIN 		(30)
#define CONFIG_MESH_LPN_POLLTIMEOUT_DEF 		(200)
#define CONFIG_MESH_LPN_POLLTIMEOUT_MAX 		(400)

/**@brief LPN support receive delay */
#define CONFIG_MESH_LPN_RECV_DELAY_MIN			(100)
#define CONFIG_MESH_LPN_RECV_DELAY_DEF			(100)
#define CONFIG_MESH_LPN_RECV_DELAY_MAX			(400)

/**@brief Number of seconds after friendship has been terminated */
#define CONFIG_MESH_RETRY_TIMEOUT_MIN			(3)
#define CONFIG_MESH_RETRY_TIMEOUT_DEF			(3)
#define CONFIG_MESH_RETRY_TIMEOUT_MAX			(60)

/**@brief Number of device can be provisioning by provisioner */
#define CONFIG_MESH_PROV_NODE_COUNT_MIN			(1)
#define CONFIG_MESH_PROV_NODE_COUNT_DEF			(2)


/** @brief Configuration Mesh network
 *
 *  Configure network parameters, such as number of subnets, 
 *	number of buffers, storage time, etc
 *
 *  @param p_cfg Device Configuration information.
 *  @param p_dev Flash Store information.
 *  @param p_mac User defined MAC address, or NULL by default,
 *  @param info  Memory information
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
extern int bt_mesh_cfg_set(ble_mesh_cfg_t const *p_cfg, void const *p_dev,
					u8_t const * p_mac, mem_info_t *info);

#ifdef __cplusplus
}
#endif
#endif // MESH_CFG_PUB_H__
