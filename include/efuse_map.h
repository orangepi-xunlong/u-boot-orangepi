/*
 *SPDX-License-Identifier: GPL-2.0+
 */
#ifndef __EFUSE_MAP_H__
#define __EFUSE_MAP_H__
#include <asm/arch/efuse.h>

#define EFUSE_CHIPID_NAME            "chipid"
#define EFUSE_BROM_CONF_NAME         "brom_conf"
#define EFUSE_BROM_TRY_NAME          "brom_try"
#define EFUSE_THM_SENSOR_NAME        "thermal_sensor"

#define EFUSE_FT_ZONE_NAME           "ft_zone"
#define EFUSE_TV_OUT_NAME            "tvout"
#define EFUSE_OEM_NAME               "oem"
#define EFUSE_OEM_SEC_NAME			"oem_secure"
#define EFUSE_EMAC_NAME              "emac"

#define EFUSE_WR_PROTECT_NAME        "write_protect"
#define EFUSE_RD_PROTECT_NAME        "read_protect"
#define EFUSE_IN_NAME                "in"
#define EFUSE_ID_NAME                "id"
#define EFUSE_ROTPK_NAME             "rotpk"
#define EFUSE_SSK_NAME               "ssk"
#define EFUSE_RSSK_NAME              "rssk"
#define EFUSE_HDCP_HASH_NAME         "hdcp_hash"
#define EFUSE_HDCP_PKF_NAME          "hdcp_pkf"
#define EFUSE_HDCP_DUK_NAME          "hdcp_duk"
#define EFUSE_EK_HASH_NAME           "ek_hash"
#define EFUSE_SN_NAME                "sn"
#define EFUSE_NV1_NAME               "nv1"
#define EFUSE_NV2_NAME               "nv2"
#define EFUSE_BACKUP_KEY_NAME        "backup_key"
#define EFUSE_RSAKEY_HASH_NAME       "rsakey_hash"
#define EFUSE_RENEW_NAME             "renewability"
#define EFUSE_OPT_ID_NAME            "operator_id"
#define EFUSE_LIFE_CYCLE_NAME        "life_cycle"
#define EFUSE_JTAG_SECU_NAME         "jtag_security"
#define EFUSE_JTAG_ATTR_NAME         "jtag_attr"
#define EFUSE_CHIP_CONF_NAME         "chip_config"
#define EFUSE_RESERVED_NAME          "reserved"
#define EFUSE_RESERVED2_NAME         "reserved2"
#define EFUSE_HUK_NAME               "huk"

/* For KeyLadder */
#define EFUSE_KL_SCK0_NAME           "keyladder_sck0"
#define EFUSE_KL_KEY0_NAME           "keyladder_master_key0"
#define EFUSE_KL_SCK1_NAME           "keyladder_sck1"
#define EFUSE_KL_KEY1_NAME           "keyladder_master_key1"

#define SUNXI_KEY_NAME_LEN	64
#define EFUSE_CHIPID_BYTE            ((SID_CHIPID_SIZE+7)>>3)
#define EFUSE_OEM_BYTE               ((SID_OEM_PROGRAM_SIZE+7)>>3)
#define EFUSE_NV1_BYTE               ((SID_NV1_SIZE +7)>>3)
#define EFUSE_NV2_BYTE               ((SID_NV2_SIZE +7)>>3)

#define EFUSE_RSAKEY_HASH_BYTE       ((SID_RSAKEY_HASH_SIZE +7)>>3)
#define EFUSE_THM_SENSOR_BYTE        ((SID_THERMAL_SIZE +7)>>3)
#define EFUSE_RENEW_BYTE             ((SID_RENEWABILITY_SIZE+7)>>3)
#define EFUSE_IN_BYTE                ((SID_IN_SIZE+7)>>3)
#define EFUSE_OPT_ID_BYTE            ((SID_IDENTIFY_SIZE+7)>>3)
#define EFUSE_ID_BYTE                ((SID_ID_SIZE+7)>>3)
#define EFUSE_ROTPK_BYTE			 ((SID_ROTPK_SIZE+7)>>3)
#define EFUSE_SSK_BYTE               ((SID_SSK_SIZE+7)>>3)
#define EFUSE_RSSK_BYTE              ((SID_RSSK_SIZE+7)>>3)
#define EFUSE_HDCP_HASH_BYTE         ((SID_HDCP_HASH_SIZE+7)>>3)
#define EFUSE_EK_HASH_BYTE           ((SID_EK_HASH_SIZE+7)>>3)
#define EFUSE_SN_BYTE                ((SID_SN_SIZE+7)>>3)
#define EFUSE_SCK0_BRUN_BYTE         ((256+256)>>3)
#define EFUSE_SCK1_BRUN_BYTE         ((256+256)>>3)

typedef enum efuse_err
{
	EFUSE_ERR_ARG = -1,
	EFUSE_ERR_KEY_NAME_WRONG = -2,
	EFUSE_ERR_KEY_SIZE_TOO_BIG = -3,
	EFUSE_ERR_PRIVATE = -4,
	EFUSE_ERR_ALREADY_BURNED = -5,
	EFUSE_ERR_READ_FORBID = -6,
	EFUSE_ERR_BURN_TIMING = -7,
	EFUSE_ERR_NO_ACCESS = -8,
}
efuse_err_e;

/*  breif: unify api to write Efuse key map.
* OTP bit one time burn feature: this API only care the unburned bits of key_buf->data
* So it is strongly suggested you read the value first,then set the unburned bits you want.
* In param:key_buf(efuse_key_info_t) key_buf->data should be aligned to 4Byte.
* Return val: 0==success ;negetive value is err number:ref efuse_err_e)
*/
int sunxi_efuse_write(void *key_buf);

/* breif: unify api to read Efuse key map.
*In param:key_name
*In param:rd_buf,should be aligned to 4Byte.
*return value : 0-success; other-fail
*is err number:ref efuse_err_e
*/
int sunxi_efuse_read(void *key_name, void *rd_buf, int *len);

/*reference this struct when call api:sunxi_efuse_write*/
typedef struct
{
	char name[SUNXI_KEY_NAME_LEN];
	u32 len; /*byte*/
	u32 res;
	u8  *key_data;
}
efuse_key_info_t;

#endif /*__EFUSE_MAP_H__*/

