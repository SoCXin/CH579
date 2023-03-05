/********************************** (C) COPYRIGHT *******************************
 * File Name          : app_mesh_config.c
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2022/06/22
 * Description        :
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/******************************************************************************/
/* ͷ�ļ����� */
#include "CONFIG.h"
#include "app_mesh_config.h"
#include "MESH_LIB.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
const ble_mesh_cfg_t app_mesh_cfg = {
    .common_cfg.adv_buf_count = CONFIG_MESH_ADV_BUF_COUNT_DEF,
    .common_cfg.rpl_count = CONFIG_MESH_RPL_COUNT_DEF,
    .common_cfg.allow_rpl_cycle = CONFIG_MESH_ALLOW_RPL_CYCLE,
    .common_cfg.allow_same_addr = CONFIG_MESH_ALLOW_SAME_ADDR,
    .common_cfg.ivu_divider = CONFIG_MESH_IVU_DIVIDER_DEF,
    .common_cfg.mod_key_count = CONFIG_MESH_MOD_KEY_COUNT_DEF,
    .common_cfg.mod_group_count = CONFIG_MESH_MOD_GROUP_COUNT_DEF,

    .proxy_cfg.pxyfilter_count = CONFIG_MESH_PROXY_FILTER_DEF,

    .net_cfg.msgcache_count = CONFIG_MESH_MSG_CACHE_DEF,
    .net_cfg.subnet_count = CONFIG_MESH_SUBNET_COUNT_DEF,
    .net_cfg.appkey_count = CONFIG_MESH_APPKEY_COUNT_DEF,
    .net_cfg.unseg_length = CONFIG_MESH_UNSEG_LENGTH_DEF,
    .net_cfg.txseg_max = CONFIG_MESH_TX_SEG_DEF,
    .net_cfg.txseg_count = CONFIG_MESH_TX_SEG_COUNT_DEF,
    .net_cfg.rxseg_count = CONFIG_MESH_RX_SEG_COUNT_DEF,
    .net_cfg.rxsdu_max = CONFIG_MESH_RX_SDU_DEF,
    .net_cfg.label_count = CONFIG_MESH_LABEL_COUNT_DEF,

    .store_cfg.seq_store_rate = CONFIG_MESH_SEQ_STORE_RATE_DEF,
    .store_cfg.rpl_store_rate = CONFIG_MESH_RPL_STORE_RATE_DEF,
    .store_cfg.store_rate = CONFIG_MESH_STORE_RATE_DEF,

    .friend_cfg.frndseg_rx = CONFIG_MESH_FRIEND_SEG_RX_COUNT_DEF,
    .friend_cfg.frndsub_size = CONFIG_MESH_FRIEND_SUB_SIZE_DEF,
    .friend_cfg.frndlpn_count = CONFIG_MESH_FRIEND_LPN_COUNT_DEF,
    .friend_cfg.frndqueue_size = CONFIG_MESH_QUEUE_SIZE_DEF,
    .friend_cfg.frndrecv_win = CONFIG_MESH_FRIEND_RECV_WIN_DEF,

    .lpn_cfg.lpnmin_size = CONFIG_MESH_LPN_REQ_QUEUE_SIZE_DEF,
    .lpn_cfg.lpnrssi_factor = 0,
    .lpn_cfg.lpnrecv_factor = 0,
    .lpn_cfg.lpnpoll_interval = CONFIG_MESH_LPN_POLLINTERVAL_DEF,
    .lpn_cfg.lpnpoll_timeout = CONFIG_MESH_LPN_POLLTIMEOUT_DEF,
    .lpn_cfg.lpnrecv_delay = CONFIG_MESH_LPN_RECV_DELAY_DEF,
    .lpn_cfg.lpnretry_timeout = CONFIG_MESH_RETRY_TIMEOUT_DEF,

    .prov_cfg.node_count = CONFIG_MESH_PROV_NODE_COUNT_DEF,
    .rf_cfg.rf_accessAddress = CONFIG_MESH_RF_ACCESSADDRESS,
    .rf_cfg.rf_channel_37 = CONFIG_MESH_RF_CHANNEL_37,
    .rf_cfg.rf_channel_38 = CONFIG_MESH_RF_CHANNEL_38,
    .rf_cfg.rf_channel_39 = CONFIG_MESH_RF_CHANNEL_39,
};

/*********************************************************************
 * @fn      read_flash
 *
 * @brief   read flash
 *
 * @param   offset  - ��ַƫ��
 * @param   data    - ����ָ��
 * @param   len     - ����
 *
 * @return  always success
 */
int read_flash(int offset, void *data, unsigned int len)
{
	memcpy(data, (const void *)offset, len);
	return 0;
}

/*********************************************************************
 * @fn      write_flash
 *
 * @brief   write flash
 *
 * @param   offset  - ��ַƫ��
 * @param   data    - ����ָ��
 * @param   len     - ����
 *
 * @return  state
 */
int write_flash(int offset, const void *data, unsigned int len)
{
	UINT8 status;
	
	status = FlashWriteBuf(offset, (PUINT32)data, len);
	if (status != 0)
	{
		return -EACCES;
	}
	
	return 0;
}

/*********************************************************************
 * @fn      erase_flash
 *
 * @brief   erase flash
 *
 * @param   offset  - ��ַƫ��
 * @param   len     - ����
 *
 * @return  state
 */
int erase_flash(int offset, unsigned int len)
{
	UINT8 status;

	status = FlashBlockErase(offset);
	if (status != 0)
	{
		return -EACCES;
	}
	
	return 0;
}

/*********************************************************************
 * @fn      flash_write_protection
 *
 * @brief   no used
 *
 * @param   enable  - no uesd
 *
 * @return  always success
 */
int flash_write_protection(BOOL enable)
{
    return 0;
}

const struct device app_dev = {
    .api = {
        .read = read_flash,
        .write = write_flash,
        .erase = erase_flash,
        .write_protection = flash_write_protection,
    },
    .info = {
        .nvs_sector_cnt = CONFIG_MESH_SECTOR_COUNT_DEF,
        .nvs_write_size = sizeof(int),
        .nvs_sector_size = CONFIG_MESH_SECTOR_SIZE_DEF,
        .nvs_store_baddr = CONFIG_MESH_NVS_ADDR_DEF,
    },
};

/******************************** endfile @ main ******************************/
