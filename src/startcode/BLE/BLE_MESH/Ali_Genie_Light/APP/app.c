/********************************** (C) COPYRIGHT *******************************
* File Name          : app.c
* Author             : WCH
* Version            : V1.0
* Date               : 2019/12/15
* Description 
*******************************************************************************/

#include "mesh_error.h"
#include "CH57x_common.h"

#define BT_DBG_ENABLED 1

#include "cfg_srv.h"
#include "health_srv.h"
#include "mesh_trace.h"
#include "mesh_types.h"
#include "mesh_access.h"
#include "mesh_main.h"
#include "mesh_cfg_pub.h"
#include "tm_genie.h"
#include "mesh_slist.h"
#include "HAL.h"
#include "mesh_kernel.h"
#include "common_trace.h"
#include "als_vendor_model.h"
#include "generic_onoff_model.h"
#include "dbg.h"

#define ADV_TIMEOUT		K_MINUTES(10)

#define DBG_INT_1		K_MSEC(100)
#define DBG_INT_2		K_MSEC(500)

#define SELENCE_ADV_ON	0x01
#define SELENCE_ADV_OF	0x00

static struct k_delayed_work adv_timer;
static struct k_delayed_work dbg_timer;

static struct bt_mesh_cfg_srv cfg_srv = {
    .relay = BLE_MESH_RELAY_ENABLED,
    .beacon = BLE_MESH_BEACON_DISABLED,
    .frnd = BLE_MESH_FRIEND_NOT_SUPPORTED,
    .gatt_proxy = BLE_MESH_GATT_PROXY_ENABLED,
    .default_ttl = 3,

    /* 8 transmissions with 10ms interval */
    .net_transmit = BLE_MESH_TRANSMIT(7, 10),
    .relay_retransmit = BLE_MESH_TRANSMIT(7, 10),
};

static struct bt_mesh_health_srv health_srv;

BLE_MESH_HEALTH_PUB_DEFINE(health_pub, 8);

static struct bt_mesh_model root_models[] = {
    BLE_MESH_MODEL_CFG_SRV(&cfg_srv),
    BLE_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub),
	BLE_MESH_MODEL(BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, gen_onoff_op, NULL, NULL),
};

static struct bt_mesh_elem elements[] = {
    BLE_MESH_ELEM(0, root_models, vnd_models),
};

static const struct bt_mesh_comp comp = {
    .cid = 0x0739,
    .elem = elements,
    .elem_count = ARRAY_SIZE(elements),
};

static inline void silen_adv_set(u8_t flag)
{
	tm_uuid[13] &= ~BIT(0);
	
	tm_uuid[13] |= (BIT_MASK(1) & flag);
}

static inline bool is_silent_adv(void)
{
	return tm_uuid[13] & BIT_MASK(1);
}

static void link_open(bt_mesh_prov_bearer_t bearer)
{
	BT_INFO("%d", k_uptime_get_32());
	
	set_led_state(PROV_PIN, FALSE);

	k_delayed_work_cancel(&dbg_timer);
	k_delayed_work_cancel(&adv_timer);
}

static void prov_complete(u16_t net_idx, u16_t addr, u8_t flags, u32_t iv_index)
{
	tmosTimer rand_timer;
	BT_INFO("%d", k_uptime_get_32());
	
	set_led_state(MSG_PIN, TRUE);
		
	rand_timer = K_SECONDS(5) + (tmos_rand() % K_SECONDS(6));	
	k_delayed_work_submit(&adv_timer, (s32_t)rand_timer);
}

static inline void prov_enable(void)
{
	silen_adv_set(SELENCE_ADV_OF);
	
	bt_mesh_prov_enable(BLE_MESH_PROV_ADV);
	
	k_work_submit(&dbg_timer.work);
	k_delayed_work_submit(&adv_timer, ADV_TIMEOUT);
}

static void prov_reset(void)
{
	BT_DBG("");
	
	set_led_state(MSG_PIN, FALSE);

	prov_enable();
}

extern void bt_mesh_store_mod_sub(struct bt_mesh_model *mod);
extern void bt_mesh_store_mod_bind(struct bt_mesh_model *mod);

static void link_close(bt_mesh_prov_bearer_t bearer)
{
	BT_DBG("");

	if (!bt_mesh_is_provisioned())
	{
		prov_enable();
	}
	else
	{
		/* For Light Subscription group address */
		root_models[2].groups[0] = (u16_t)0xC000;
		root_models[2].groups[1] = (u16_t)0xCFFF;
		bt_mesh_store_mod_sub(&root_models[2]);
	
		root_models[2].keys[0] = (u16_t)0x0000;
		bt_mesh_store_mod_bind(&root_models[2]);
		
		/* For Light Subscription group address */
		vnd_models[0].groups[0] = (u16_t)0xC000;
		vnd_models[0].groups[1] = (u16_t)0xCFFF;
		bt_mesh_store_mod_sub(&vnd_models[0]);
	
		vnd_models[0].keys[0] = (u16_t)0x0000;
		bt_mesh_store_mod_bind(&vnd_models[0]);
	}
}
									
static const struct bt_mesh_prov prov = {
    .uuid 			= tm_uuid,
    .static_val_len = ARRAY_SIZE(static_key),
	.static_val		= static_key,
	.complete 		= prov_complete,
	.link_open		= link_open,
	.link_close		= link_close,
	.reset			= prov_reset,
};

u32 MEM_BUF[BLE_MEMHEAP_SIZE / 4];
static u8_t MESH_MEM[1024 * 2];

#define LOCAL_ADV_NAME	"WCH-BLE-MESH"

extern const ble_mesh_cfg_t mesh_cfg;
extern const struct device dev;

static void blemesh_on_sync(void)
{
    int err;
	mem_info_t info;
	
	info.base_addr = MESH_MEM;
	info.mem_len = ARRAY_SIZE(MESH_MEM);
	
	err = bt_mesh_cfg_set(&mesh_cfg, &dev, MacAddr, &info);
	if (err)
	{
		BT_ERR("Unable set configuration (err:%d)", err);
		return;
	}

    err = bt_mesh_init(&prov, &comp, NULL);
    if (err)
    {
        BT_ERR("Initializing mesh failed (err %d)", err);
        return;
    }
	
	als_vendor_init();
	
	BT_DBG("Bluetooth initialized");

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS))
    {
        settings_load();
    }

    if (bt_mesh_is_provisioned())
    {
        BT_INFO("Mesh network restored from flash");
    }
    else
    {
        prov_enable();
    }

    BT_DBG("Mesh initialized");
}

static void send_led_state(void)
{
	struct indicate_param param = {
		.trans_cnt = 0x09,
		.period = K_MSEC(100),
		.send_ttl = BLE_MESH_TTL_DEFAULT,
		.tid = als_avail_tid_get(),
	};

	BT_DBG("");

	if (!bt_mesh_is_provisioned())
	{
		BT_ERR("Local Dev Unprovisioned");
		return;
	}

	toggle_led_state(MSG_PIN);

	send_led_indicate(&param);
}

static void reset_ind_end(int err, void *cb_data)
{
	BT_DBG("err %d", err);
	
	bt_mesh_reset();
}

static const struct bt_adv_ind_send_cb reset_cb = {
	.end = reset_ind_end,
};

static void send_reset_indicate(void)
{
	struct net_buf *buf;
	struct indicate_param param = {
		.trans_cnt = 0x09,
		.period = K_MSEC(300),
		.cb = &reset_cb,
		
		.send_ttl = BLE_MESH_TTL_DEFAULT,

		.tid = als_avail_tid_get(),
	};

	if (!bt_mesh_is_provisioned())
	{
		BT_ERR("Local Dev Unprovisioned");
		return;
	}

	buf = bt_mesh_ind_create(16, K_NO_WAIT);
	if (!buf)
	{
		BT_ERR("Unable allocate buffers");
		return;
	}

	/* Init indication opcode */
	bt_mesh_model_msg_init(&buf->b, OP_VENDOR_MESSAGE_ATTR_INDICATION);
    
	/* Add tid field */
	net_buf_simple_add_u8(&buf->b, param.tid);
	
	/* Add event report opcode */
	net_buf_simple_add_le16(&buf->b, ALI_GEN_ATTR_TYPE_EVENT_TRIGGER);
	
	/* Add reset event */
	net_buf_simple_add_u8(&buf->b, ALI_EVT_RESET);
	
	bt_mesh_indicate_send(buf, &param);
	
	net_buf_unref(buf);
}

#define	HAL_KEY_SEND_MSG	BIT(0)
#define	HAL_KEY_RESET		BIT(1)

static void keyPress(uint8 keys, uint8 state)
{
	BT_DBG("%d", keys);

	switch (keys)
	{
		case HAL_KEY_RESET:
			send_reset_indicate();
		break;
		case HAL_KEY_SEND_MSG:
			send_led_state();
		break;
	}
}

extern int bt_mesh_scan_disable(void);
extern int bt_mesh_proxy_prov_disable(bool disconnect);

static void adv_timer_cb(struct k_work *work)
{
	BT_DBG("");
	struct indicate_param param = {
		.trans_cnt = 0x03,
		.period = K_MSEC(400),
		
		.send_ttl = BLE_MESH_TTL_DEFAULT,
		
		.tid = als_avail_tid_get(),
	};

	if (bt_mesh_is_provisioned())
	{
		send_led_indicate(&param);
		return;
	}

	silen_adv_set(SELENCE_ADV_ON);
	
	/* Disable Scanner not response Privisioner message */
	bt_mesh_scan_disable();
//	bt_mesh_proxy_prov_disable(false);
}

static void dbg_timer_cb(struct k_work *work)
{
	s32_t delay;

	if (bt_mesh_is_provisioned())
	{
		BT_WARN("Ignore toggle led, local dev already provisoned");
		return;
	}

	toggle_led_state(PROV_PIN);

	if (is_silent_adv())
	{
		delay = DBG_INT_2;
	}
	else
	{
		delay = DBG_INT_1;
	}

	k_delayed_work_submit(&dbg_timer, delay);
}

static void gpio_init(void)
{
	set_led_state(PROV_PIN, FALSE);

	/** provisioned complete indicate led */
	GPIOB_ModeCfg(PROV_PIN, GPIO_ModeOut_PP_5mA);
	set_led_state(PROV_PIN, FALSE);
	
	set_led_state(MSG_PIN, FALSE);

	/** unprovisioned indicate led */
	GPIOB_ModeCfg(MSG_PIN, GPIO_ModeOut_PP_5mA);
	set_led_state(MSG_PIN, FALSE);
	
	set_led_state(GPIO_Pin_12, FALSE);
	set_led_state(GPIO_Pin_13, FALSE);

	/** reserved for future used */
	GPIOB_ModeCfg(GPIO_Pin_12, GPIO_ModeOut_PP_5mA);
	GPIOB_ModeCfg(GPIO_Pin_13, GPIO_ModeOut_PP_5mA);
	
	set_led_state(GPIO_Pin_12, FALSE);
	set_led_state(GPIO_Pin_13, FALSE);
}

int main()
{
#if defined(DEBUG) || defined(NOLIB_DEBUG)
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    UART1_DefInit();
    UART1_BaudRateCfg(115200);
    R8_UART1_FCR = (2 << 6) | RB_FCR_TX_FIFO_CLR |
					RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
    R8_UART1_LCR = RB_LCR_WORD_SZ;
    R8_UART1_IER = RB_IER_TXD_EN;
    R8_UART1_DIV = 1;
    UART1_ByteTrigCfg(UART_1BYTE_TRIG);
#endif

#if 1
	gpio_init();
#endif

#if 0
	FlashBlockErase(CONFIG_MESH_NVS_ADDR_DEF + 512 * 0);
	FlashBlockErase(CONFIG_MESH_NVS_ADDR_DEF + 512 * 1);
	FlashBlockErase(CONFIG_MESH_NVS_ADDR_DEF + 512 * 2);
	FlashBlockErase(CONFIG_MESH_NVS_ADDR_DEF + 512 * 3);

	for(uint16 i=0;i<2048;i++)
	{
		if(i%512 == 0)
		PRINT("\nidx :%u\n",i/512);
		
		PRINT("%02x ",*(char *)(CONFIG_MESH_NVS_ADDR_DEF+i));
	}
	
	while(1);
#endif
	
	CH57X_BLEInit();
	HAL_Init();
	
	HAL_KeyInit();
	HalKeyConfig(FALSE, keyPress);
	
	k_delayed_work_init(&adv_timer, adv_timer_cb);
	
	k_delayed_work_init(&dbg_timer, dbg_timer_cb);
	
	bt_mesh_lib_init();

#if 1
	dbg_init();
#endif

	blemesh_on_sync();

	while(1)
	{
		TMOS_SystemProcess();
	}
}
