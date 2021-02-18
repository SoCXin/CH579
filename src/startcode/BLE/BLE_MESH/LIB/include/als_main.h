/** @file
 *  @brief Alibaba Tmall Genie APIs
 */

#ifndef __ALS_MAIN_H
#define __ALS_MAIN_H

#include "mesh_types.h"
#include "mesh_buf.h"

#define BZ_CMD_CTRL 0x0
#define BZ_CMD_STATUS 0x1
#define BZ_CMD_QUERY 0x2
#define BZ_CMD_REPLY 0x3
#define BZ_CMD_EXT_DOWN 0xd
#define BZ_CMD_EXT_UP 0xe
#define BZ_CMD_ERR 0xf

#define BZ_CMD_AUTH 0x10
#define BZ_CMD_AUTH_RAND 0x11
#define BZ_CMD_AUTH_REQ 0x12
#define BZ_CMD_AUTH_RSP 0x13
#define BZ_CMD_AUTH_CFM 0x14
#define BZ_CMD_AUTH_KEY 0x15
#define BZ_CMD_AUTH_REKEY 0x16
#define BZ_CMD_AUTH_REKEY_RSP 0x17

#define BZ_CMD_TYPE_OTA 0x20
#define BZ_CMD_OTA_VER_REQ 0x20
#define BZ_CMD_OTA_VER_RSP 0x21
#define BZ_CMD_OTA_REQ 0x22
#define BZ_CMD_OTA_RSP 0x23
#define BZ_CMD_OTA_PUB_SIZE 0x24
#define BZ_CMD_OTA_CHECK_RESULT 0x25
#define BZ_CMD_OTA_UPDATE_PROCESS 0x26
#define BZ_CMD_OTA_SIZE 0x27
#define BZ_CMD_OTA_DONE 0x28
#define BZ_CMD_OTA_DATA 0x2f

/** Received Context */
struct bt_als_ctx
{
    u8_t msg_id;
    u8_t enc;
    u8_t ver;

    u8_t cmd;
	
	u8_t frame_seq;
    u8_t total_frame;
    u8_t frame_length;
};

struct bt_ind_send_cb
{
    void (*start)(int err, void *cb_data);
    void (*end)(int err, void *cb_data);
};

struct bt_als_cfg
{
    /** Company Identify */
    u16_t cid;

    /** Product Identify */
    u32_t pid;

    /** Mac Address */
    u8_t mac[6];
	
	/** Secret Information */
	u8_t secret[16];

    /** Currently Library Version */
    u32_t version;
};

/** OTA Callback functions */
struct bt_als_ota_cb
{
	/** @breaf OTA Start is requested
	 *  
	 *  This callback notifies the application to start
	 *  OTA Proceduce.
	 *
	 *  @param image_size The Remote OTA Package Length.
	 *  @param crc The Remote OTA Ckeck Value.
	 *
	 *  @return Zero on start procedure or other value to reject.
	*/
	int (*start)(u32_t image_size, u16_t crc);
	
	/** @breaf OTA Trans Receive
	 *  
	 *  This callback notifies the application to receive
	 *  OTA Packages.
	 *
	 *  @param data Data received.
	 *  @param len Received data length.
	*/
	void (*recv)(const u8_t *data, u16_t len);
	
	/** @breaf OTA Check is requested
	 *  
	 *  This callback notifies the application to Check
	 *  OTA Package.
	 *
	 *  @param crc_check The CRC Check function.
	 *
	 *  @return Zero on check successful or other value when failed.
	*/
	int (*auth)(u16_t (*crc_check)(const u8_t * data, u32_t size));
	
	/** @breaf OTA Trans End is requested
	 *  
	 *  This callback notifies the application the OTA trans
	 *  has been closed.
	 *
	 *  @param recv_len Received OTA Octers length.
	 *
	 *  @note The Callback will be call when link has been terminated
	 *        even OTA not been transfer completed.
	*/
	void (*end)(u32_t recv_len);
};

struct bt_als_cmd_op
{
    const u8_t cmd;

    /* Minimum required message length */
    const size_t min_len;

    /* Message handler for the cmd */
    void (*const func)(struct bt_als_ctx *ctx,
                       struct net_buf_simple *buf);
};

void als_ota_init(const struct bt_als_cfg *cfg,
			const struct bt_als_ota_cb *ota_cb);

void als_ota_start(void);

#endif
