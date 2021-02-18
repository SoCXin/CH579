/** @file
 *  @brief Bluetooth Mesh Profile APIs.
 */

#ifndef _BLE_MESH_MAIN_H_
#define _BLE_MESH_MAIN_H_

#include "mesh_types.h"
#include "mesh_util.h"
#include "mesh_access.h"

/**
 * @brief Bluetooth Mesh Provisioning
 * @defgroup bt_mesh_prov Bluetooth Mesh Provisioning
 * @ingroup bt_mesh
 * @{
 */

typedef enum {
    BLE_MESH_NO_OUTPUT       = 0,
    BLE_MESH_BLINK           = BIT(0),
    BLE_MESH_BEEP            = BIT(1),
    BLE_MESH_VIBRATE         = BIT(2),
    BLE_MESH_DISPLAY_NUMBER  = BIT(3),
    BLE_MESH_DISPLAY_STRING  = BIT(4),
} bt_mesh_output_action_t;

typedef enum {
    BLE_MESH_NO_INPUT      = 0,
    BLE_MESH_PUSH          = BIT(0),
    BLE_MESH_TWIST         = BIT(1),
    BLE_MESH_ENTER_NUMBER  = BIT(2),
    BLE_MESH_ENTER_STRING  = BIT(3),
} bt_mesh_input_action_t;

typedef enum {
    BLE_MESH_PROV_ADV   = BIT(0),
    BLE_MESH_PROV_GATT  = BIT(1),
} bt_mesh_prov_bearer_t;

typedef enum {
    BLE_MESH_PROV_OOB_OTHER     = BIT(0),
    BLE_MESH_PROV_OOB_URI       = BIT(1),
    BLE_MESH_PROV_OOB_2D_CODE   = BIT(2),
    BLE_MESH_PROV_OOB_BAR_CODE  = BIT(3),
    BLE_MESH_PROV_OOB_NFC       = BIT(4),
    BLE_MESH_PROV_OOB_NUMBER    = BIT(5),
    BLE_MESH_PROV_OOB_STRING    = BIT(6),
    /* 7 - 10 are reserved */
    BLE_MESH_PROV_OOB_ON_BOX    = BIT(11),
    BLE_MESH_PROV_OOB_IN_BOX    = BIT(12),
    BLE_MESH_PROV_OOB_ON_PAPER  = BIT(13),
    BLE_MESH_PROV_OOB_IN_MANUAL = BIT(14),
    BLE_MESH_PROV_OOB_ON_DEV    = BIT(15),
} bt_mesh_prov_oob_info_t;

typedef struct {
    u8_t type;
    u8_t val[6];
} bt_mesh_addr_t;

struct bt_mesh_conn {
    u16_t    handle;
    bt_mesh_atomic_t ref;
};

typedef union {
    struct pb_adv
    {
        const u32_t *uri_hash;
    } adv;

    struct pb_gatt
    {
        const bt_mesh_addr_t *addr;
    } gatt;
} unprivison_info_t;

/** Provisioning properties & capabilities. */
struct bt_mesh_prov {
    /** The UUID that's used when advertising as unprovisioned */
    const u8_t *uuid;

    /** Optional URI. This will be advertised separately from the
     *  unprovisioned beacon, however the unprovisioned beacon will
     *  contain a hash of it so the two can be associated by the
     *  provisioner.
     */
    const char *uri;

    /** Out of Band information field. */
    bt_mesh_prov_oob_info_t oob_info;

    /** Flag indicates whether unprovisioned devices support OOB public key */
    bool oob_pub_key;

    /** @brief Set device OOB public key.
     *
     *  This callback notifies the application to
     *  set OOB public key & private key pair.
     */
    void (*oob_pub_key_cb)(void);

    /** Static OOB value */
    const u8_t *static_val;
    /** Static OOB value length */
    u8_t        static_val_len;

    /** Maximum size of Output OOB supported */
    u8_t        output_size;
    /** Supported Output OOB Actions */
    u16_t       output_actions;

    /* Maximum size of Input OOB supported */
    u8_t        input_size;
    /** Supported Input OOB Actions */
    u16_t       input_actions;

    /** @brief Output of a number is requested.
     *
     *  This callback notifies the application to
     *  output the given number using the given action.
     *
     *  @param act Action for outputting the number.
     *  @param num Number to be out-put.
     *
     *  @return Zero on success or negative error code otherwise
     */
    int         (*output_number)(bt_mesh_output_action_t act, u32_t num);

    /** @brief Output of a string is requested.
     *
     *  This callback notifies the application to
     *  display the given string to the user.
     *
     *  @param str String to be displayed.
     *
     *  @return Zero on success or negative error code otherwise
     */
    int         (*output_string)(const char *str);

    /** @brief Input is requested.
     *
     *  This callback notifies the application to request
     *  input from the user using the given action. The
     *  requested input will either be a string or a number, and
     *  the application needs to consequently call the
     *  bt_mesh_input_string() or bt_mesh_input_number() functions
     *  once the data has been acquired from the user.
     *
     *  @param act Action for inputting data.
     *  @param num Maximum size of the in-put data.
     *
     *  @return Zero on success or negative error code otherwise
     */
    int         (*input)(bt_mesh_input_action_t act, u8_t size);

    /** @brief Unprovisioned beacon has been received.
	 *
	 * This callback notifies the application that an unprovisioned
	 * beacon has been received.
	 *
     * @param bearer Provision Bearer
	 * @param uuid UUID
	 * @param oob_info OOB Information
	 * @param info  Point to uri_Hash or bt_addr information.
	 */
    void (*unprovisioned_beacon)(bt_mesh_prov_bearer_t bearer,
                                 const u8_t uuid[16], bt_mesh_prov_oob_info_t oob_info,
                                 const unprivison_info_t *info);

    /** @brief Indicate That Peer OOB Public Key is required.
	 *
	 * This callback notifies that local provisioner select OOB public key method,
     * and request peer oob public key.
	 */
    void        (*pear_oob_pub_key)(void);

    /** @brief Provisioning link has been opened.
     *
     *  This callback notifies the application that a provisioning
     *  link has been opened on the given provisioning bearer.
     *
     *  @param bearer Provisioning bearer.
     */
    void        (*link_open)(bt_mesh_prov_bearer_t bearer);

    /** @brief Provisioning link has been closed.
     *
     *  This callback notifies the application that a provisioning
     *  link has been closed on the given provisioning bearer.
     *
     *  @param bearer Provisioning bearer.
     */
    void        (*link_close)(bt_mesh_prov_bearer_t bearer);

    /** @brief Provisioning is complete.
     *
     *  This callback notifies the application that provisioning has
     *  been successfully completed, and that the local node has been
     *  assigned the specified NetKeyIndex and primary element address.
     *
     *  @param net_idx  NetKeyIndex given during provisioning.
     *  @param addr     Primary element address.
     *  @param flags    Key Refresh & IV Update flags
     *  @param iv_index IV Index.
     */
    void        (*complete)(u16_t net_idx, u16_t addr, u8_t flags, u32_t iv_index);

    /** @brief A new node has been added to the provisioning database.
	 *
	 *  This callback notifies the application that provisioning has
	 *  been successfully completed, and that a node has been assigned
	 *  the specified NetKeyIndex and primary element address.
	 *
	 *  @param net_idx NetKeyIndex given during provisioning.
	 *  @param addr Primary element address.
	 *  @param num_elem Number of elements that this node has.
	 */
	void        (*node_added)(u16_t net_idx, u16_t addr, u8_t num_elem);

    /** @brief Node has been reset.
     *
     *  This callback notifies the application that the local node
     *  has been reset and needs to be reprovisioned. The node will
     *  not automatically advertise as unprovisioned, rather the
     *  bt_mesh_prov_enable() API needs to be called to enable
     *  unprovisioned advertising on one or more provisioning bearers.
     */
    void        (*reset)(void);
};

/** Proxy Client Callback Handler */
struct bt_mesh_proxy_cli
{
    /** @brief Connection has been established.
     *
     *  This callback notifies the application that the local node
     *  (Provisoner Client or Proxy Client) has been established to
     *  remote device(or node).
     * 
     *  @param conn handle of this connection.
     */
    void (*connected)(struct bt_mesh_conn *conn);

    /** @brief Connection has been terminated.
     *
     *  This callback notifies the application that the local node
     *  (Provisoner Client or Proxy Client) has been terminated to
     *  remote device(or node).
     * 
     *  @param conn handle of this connection.
     *  @param reason The reason of termanated of connection.
     */
    void (*disconnected)(struct bt_mesh_conn *conn, u8_t reason);

    /** @brief Valid network identity beacon received.
     *
     *  This callback notifies the application that the local node
     *  (Proxy Client) has been receive valid network id belong some
     *  network with the local node.
     * 
     *  @param addr The remote bluetooth address.
     *  @param net_idx Network index belong same network.
     */
    void (*net_id_recv)(const bt_mesh_addr_t *addr, u16_t net_idx);

    /** @brief Valid Node identity beacon received.
     *
     *  This callback notifies the application that the local node
     *  (Proxy Client) has been receive valid node id belong some
     *  network with local node.
     * 
     *  @param bt_addr The remote bluetooth address.
     *  @param net_idx Network index belong same network.
     *  @param addr The remote node address.
     */
    void (*node_id_recv)(const bt_mesh_addr_t *bt_addr, u16_t net_idx, u16_t addr);
};

/* The following APIs are for BLE Mesh Node */

/** @brief Provide provisioning input OOB string.
 *
 *  This is intended to be called after the bt_mesh_prov input callback
 *  has been called with BLE_MESH_ENTER_STRING as the action.
 *
 *  @param str String.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
extern int bt_mesh_input_string(const char *str);

/** @brief Provide provisioning input OOB number.
 *
 *  This is intended to be called after the bt_mesh_prov input callback
 *  has been called with BLE_MESH_ENTER_NUMBER as the action.
 *
 *  @param num Number.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
extern int bt_mesh_input_number(u32_t num);

/** @brief Enable specific provisioning bearers
 *
 *  Enable one or more provisioning bearers.
 *
 *  @param Bit-wise OR of provisioning bearers.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
extern int bt_mesh_prov_enable(bt_mesh_prov_bearer_t bearers);

/** @brief Enable specific provisioning bearers
 *
 *  Enable one or more provisioning bearers.
 *
 *  @param Bit-wise OR of provisioning bearers.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
extern int bt_mesh_provisioner_enable(bt_mesh_prov_bearer_t bearers);

/** @brief Disable specific provisioning bearers
 *
 *  Disable one or more provisioning bearers.
 *
 *  @param Bit-wise OR of provisioning bearers.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
extern int bt_mesh_provisioner_disable(bt_mesh_prov_bearer_t bearers, bool force);

/** @brief Indicate whether provisioner is enabled
 *
 *  @return true - enabled, false - disabled.
 */
extern int bt_mesh_is_provisioner_en(void);

/**
 * @}
 */

/**
 * @brief Bluetooth Mesh
 * @defgroup bt_mesh Bluetooth Mesh
 * @ingroup bluetooth
 * @{
 */
 
#define BLE_MESH_ADDR_IS_UNICAST(addr) 		((addr) && (addr) < 0x8000)
#define BLE_MESH_ADDR_IS_GROUP(addr) 		((addr) >= 0xc000 && (addr) <= 0xff00)
#define BLE_MESH_ADDR_IS_VIRTUAL(addr) 		((addr) >= 0x8000 && (addr) < 0xc000)
#define BLE_MESH_ADDR_IS_RFU(addr) 			((addr) >= 0xff00 && (addr) <= 0xfffb)

/* Primary Network Key index */
#define BLE_MESH_NET_PRIMARY                 0x000

#define BLE_MESH_RELAY_DISABLED              0x00
#define BLE_MESH_RELAY_ENABLED               0x01
#define BLE_MESH_RELAY_NOT_SUPPORTED         0x02

#define BLE_MESH_BEACON_DISABLED             0x00
#define BLE_MESH_BEACON_ENABLED              0x01

#define BLE_MESH_GATT_PROXY_DISABLED         0x00
#define BLE_MESH_GATT_PROXY_ENABLED          0x01
#define BLE_MESH_GATT_PROXY_NOT_SUPPORTED    0x02

#define BLE_MESH_FRIEND_DISABLED             0x00
#define BLE_MESH_FRIEND_ENABLED              0x01
#define BLE_MESH_FRIEND_NOT_SUPPORTED        0x02

#define BLE_MESH_NODE_IDENTITY_STOPPED       0x00
#define BLE_MESH_NODE_IDENTITY_RUNNING       0x01
#define BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED 0x02

/* Features */
#define BLE_MESH_FEAT_RELAY                  BIT(0)
#define BLE_MESH_FEAT_PROXY                  BIT(1)
#define BLE_MESH_FEAT_FRIEND                 BIT(2)
#define BLE_MESH_FEAT_LOW_POWER              BIT(3)
#define BLE_MESH_FEAT_SUPPORTED              (BLE_MESH_FEAT_RELAY |     \
                                              BLE_MESH_FEAT_PROXY |     \
                                              BLE_MESH_FEAT_FRIEND |    \
                                              BLE_MESH_FEAT_LOW_POWER)

/** @brief Initialize Mesh support
 *
 *  After calling this API, the node will not automatically advertise as
 *  unprovisioned, rather the bt_mesh_prov_enable() API needs to be called
 *  to enable unprovisioned advertising on one or more provisioning bearers.
 *
 *  @param prov Node provisioning information.
 *  @param comp Node Composition.
 *  @param cli  Proxy Client
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
extern int bt_mesh_init(const struct bt_mesh_prov *prov, const struct bt_mesh_comp *comp, const struct bt_mesh_proxy_cli *cli);

/** @brief Reset the state of the local Mesh node.
 *
 *  Resets the state of the node, which means that it needs to be
 *  reprovisioned to become an active node in a Mesh network again.
 *
 *  After calling this API, the node will not automatically advertise as
 *  unprovisioned, rather the bt_mesh_prov_enable() API needs to be called
 *  to enable unprovisioned advertising on one or more provisioning bearers.
 *
 */
extern void bt_mesh_reset(void);

/** @brief Suspend the Mesh network temporarily.
 *
 *  This API can be used for power saving purposes, but the user should be
 *  aware that leaving the local node suspended for a long period of time
 *  may cause it to become permanently disconnected from the Mesh network.
 *  If at all possible, the Friendship feature should be used instead, to
 *  make the node into a Low Power Node.
 *
 *  @return 0 on success, or (negative) error code on failure.
 */
extern int bt_mesh_suspend(void);

/** @brief Resume a suspended Mesh network.
 *
 *  This API resumes the local node, after it has been suspended using the
 *  bt_mesh_suspend() API.
 *
 *  @return 0 on success, or (negative) error code on failure.
 */
extern int bt_mesh_resume(void);

/** @brief Provision the local Mesh Node.
 *
 *  This API should normally not be used directly by the application. The
 *  only exception is for testing purposes where manual provisioning is
 *  desired without an actual external provisioner.
 *
 *  @param net_key  Network Key
 *  @param net_idx  Network Key Index
 *  @param flags    Provisioning Flags
 *  @param iv_index IV Index
 *  @param addr     Primary element address
 *  @param dev_key  Device Key
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
extern int bt_mesh_provision(const u8_t net_key[16], u16_t net_idx, u8_t flags, u32_t iv_index, u16_t addr, const u8_t dev_key[16]);

/** @brief Provision a Mesh Node using PB-ADV
 *
 * @param uuid    UUID
 * @param net_idx Network Key Index
 * @param addr    Address to assign to remote device. If addr is 0, the lowest
 *                available address will be chosen.
 * @param attention_duration The attention duration to be send to remote device
 *
 * @return Zero on success or (negative) error code otherwise.
 */
extern int bt_mesh_provision_adv(const u8_t uuid[16], u16_t net_idx, u16_t addr, u8_t attention_duration);

/** @brief Check if the local node has been provisioned.
 *
 *  This API can be used to check if the local node has been provisioned
 *  or not. It can e.g. be helpful to determine if there was a stored
 *  network in flash, i.e. if the network was restored after calling
 *  settings_load().
 *
 *  @return True if the node is provisioned. False otherwise.
 */
extern bool bt_mesh_is_provisioned(void);

/** @brief Toggle the IV Update test mode
 *
 *  This API is only available if the IV Update test mode has been enabled
 *  in Kconfig. It is needed for passing most of the IV Update qualification
 *  test cases.
 *
 *  @param enable true to enable IV Update test mode, false to disable it.
 */
extern void bt_mesh_iv_update_test(bool enable);

/** @brief Toggle the IV Update state
 *
 *  This API is only available if the IV Update test mode has been enabled
 *  in Kconfig. It is needed for passing most of the IV Update qualification
 *  test cases.
 *
 *  @return true if IV Update In Progress state was entered, false otherwise.
 */
extern bool bt_mesh_iv_update(void);

/** @brief Toggle the Low Power feature of the local device
 *
 *  Enables or disables the Low Power feature of the local device. This is
 *  exposed as a run-time feature, since the device might want to change
 *  this e.g. based on being plugged into a stable power source or running
 *  from a battery power source.
 *
 *  @param enable  true to enable LPN functionality, false to disable it.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
extern int bt_mesh_lpn_set(bool enable);

/** @brief Send out a Friend Poll message.
 *
 *  Send a Friend Poll message to the Friend of this node. If there is no
 *  established Friendship the function will return an error.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
extern int bt_mesh_lpn_poll(void);

/** @brief Register a callback for Friendship changes.
 *
 *  Registers a callback that will be called whenever Friendship gets
 *  established or is lost.
 *
 *  @param cb Function to call when the Friendship status changes.
 */
extern void bt_mesh_lpn_set_cb(void (*cb)(u16_t friend_addr, bool established));

/** @brief Initialize Mesh Stack Library.
 *
 *  Initialize Mesh Stack Library, this proceduce only run 
 *  after bt lib inited successfully.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
extern u8_t bt_mesh_lib_init(void);

/** @brief Set Local Device Name.
 *
 *  This function only be call when local device support proxy feature and enabled.
 * 
 *  @param name Adv name in advertiser package field (less than 16).
 * 
 *  @return Zero on success or (negative) error code otherwise.
 */
extern int bt_mesh_set_device_name(const char *name);

/** @brief Recover From Flash.
 *
 *  Recover Whole Mesh Information include ( dev_key, net_key, app_key, addr, sub, pub, etc..). 
 * 
 *  @return Zero on success or (negative) error code otherwise.
 */
extern int settings_load(void);

/**
 * @defgroup version_apis Version APIs
 * @ingroup kernel_apis
 * @{
 *
 * The kernel version has been converted from a string to a four-byte
 * quantity that is divided into two parts.
 *
 * Part 1: The three most significant bytes represent the kernel's
 * numeric version, x.y.z. These fields denote:
 *       x -- major release
 *       y -- minor release
 *       z -- patchlevel release
 * Each of these elements must therefore be in the range 0 to 255, inclusive.
 *
 * Part 2: The least significant byte is reserved for future use.
 */
#define SYS_KERNEL_VER_MAJOR(ver)       (((ver) >> 24) & 0xFF)
#define SYS_KERNEL_VER_MINOR(ver)       (((ver) >> 16) & 0xFF)
#define SYS_KERNEL_VER_PATCHLEVEL(ver)  (((ver) >> 8) & 0xFF)

/* kernel version routines */

/**
 * @brief Return the kernel version of the present build
 *
 * The kernel version is a four-byte value, whose format is described in the
 * defined.
 *
 * @return kernel version
 */
extern u32_t sys_kernel_version_get(void);

/**
 * @}
 */
#endif /* _BLE_MESH_MAIN_H_ */
