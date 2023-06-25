/*
 * (C) Copyright 2000-2999
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Author: qinjian <qinjian@allwinnertech.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <asm/io.h>
#include <efuse_map.h>
#include <sunxi_board.h>

#ifdef CONFIG_ARCH_SUN8IW12P1
#define SECURE_BIT_OFFSET 31
#else
#define SECURE_BIT_OFFSET 11
#endif

#define SBROM_ACCELERATION_ENABLE_BIT 29

#define EFUSE_DBG_E 0
#if EFUSE_DBG_E
static void efuse_dump(char *str, unsigned char *data, int len, int align)
{
	int i = 0;
	if (str)
		printf("\n%s: ", str);
	for (i = 0; i < len; i++) {
		if ((i % align) == 0) {
			printf("\n");
		}
		printf("%x", *(data++));
	}
	printf("\n");
}
#define EFUSE_DBG printf
#define EFUSE_DBG_DUMP efuse_dump
#define EFUSE_DUMP_LEN 16
#else
#define EFUSE_DBG_DUMP(...)                                                    \
	do {                                                                   \
	} while (0);
#define EFUSE_DBG(...)                                                         \
	do {                                                                   \
	} while (0);
#endif

#ifndef EFUSE_READ_PROTECT
#define EFUSE_READ_PROTECT EFUSE_CHIPCONFIG
#define EFUSE_WRITE_PROTECT EFUSE_CHIPCONFIG
#endif
/* internal struct */
typedef struct efuse_key_map_new {
	char name[SUNXI_KEY_NAME_LEN];
	int offset;
	int size; /* unit: bit */
	int rd_fbd_offset; /*read protect*/
	int burned_flg_offset; /*write protect*/
	int sw_rule;
} efuse_key_map_new_t;
/*It can not be seen.*/
#define EFUSE_PRIVATE (0)
/*After burned ,cpu can not access.*/
#define EFUSE_NACCESS (1 << 1)
#define EFUSE_RW (2 << 1)
#define EFUSE_RO (3 << 1)
#define EFUSE_ACL_SET_BRUN_BIT (1 << 29)
#define EFUSE_ACL_SET_RD_FORBID_BIT (1 << 30)
#define EFUSE_BRUN_RD_OFFSET_MASK (0xFFFFFF)

#define EFUSE_DEF_ITEM(name, offset, size_bits, rd_offset, burn_offset, acl)   \
	{                                                                      \
		name, offset, size_bits, rd_offset, burn_offset, acl           \
	}
#if defined(CONFIG_MACH_SUN50IW11)
static efuse_key_map_new_t g_key_info[] = {
	EFUSE_DEF_ITEM(EFUSE_CHIPID_NAME, EFUSE_CHIPID, 128, -1, -1, EFUSE_RO),
	EFUSE_DEF_ITEM(EFUSE_ROTPK_NAME, EFUSE_ROTPK, 256, -1, -1, EFUSE_RO),
	EFUSE_DEF_ITEM("", 0, 0, 0, 0, EFUSE_PRIVATE),
};
#elif defined(CONFIG_ARCH_SUN8IW17P1)
static efuse_key_map_new_t g_key_info[] = {
	EFUSE_DEF_ITEM(EFUSE_THM_SENSOR_NAME, 0x14, 96, 2, 2, EFUSE_RO),
	EFUSE_DEF_ITEM(EFUSE_ROTPK_NAME, 0x70, 256, -1, -1, EFUSE_RO),
	EFUSE_DEF_ITEM("sensor", 0x14, 96, 2, 2, EFUSE_RO),
	EFUSE_DEF_ITEM(EFUSE_FT_ZONE_NAME, 0x20, 128, 3, 3, EFUSE_RW),
	EFUSE_DEF_ITEM(EFUSE_TV_OUT_NAME, 0x30, 32, 4, 4, EFUSE_RO),
	EFUSE_DEF_ITEM(EFUSE_OEM_NAME, 0x34, 96, 4, 4, EFUSE_RO),
	EFUSE_DEF_ITEM("", 0, 0, 0, 0, EFUSE_PRIVATE),
};
#elif defined(CONFIG_MRCH_SUN8IW19P1)
static efuse_key_map_new_t g_key_info[] = {
	EFUSE_DEF_ITEM(EFUSE_CHIPID_NAME, 0x0, 128, -1, 0, EFUSE_RO),
	EFUSE_DEF_ITEM(EFUSE_THM_SENSOR_NAME, EFUSE_THERMAL_SENSOR, 64, -1, 0, EFUSE_RO),
	EFUSE_DEF_ITEM(EFUSE_FT_ZONE_NAME, EFUSE_TF_ZONE, 128, -1, 0, EFUSE_RO),
	EFUSE_DEF_ITEM("psensor", EFUSE_PSENSOR, 32, -1, 15, EFUSE_RO),
	EFUSE_DEF_ITEM("anti_blushing", EFUSE_ANTI_BLUSHING, 32, -1, 15, EFUSE_RO),
	EFUSE_DEF_ITEM(EFUSE_RESERVED_NAME, EFUSE_CLIENT_RESERVE, 256, -1, 12, EFUSE_RO),
	EFUSE_DEF_ITEM("", 0, 0, 0, 0, EFUSE_PRIVATE),
};
#elif defined(CONFIG_MRCH_SUN8IW7P1)
static efuse_key_map_new_t g_key_info[] = {
	EFUSE_DEF_ITEM(EFUSE_ROTPK_NAME, 0x64, 256, -1, -1, EFUSE_RO),
	EFUSE_DEF_ITEM("", 0, 0, 0, 0, EFUSE_PRIVATE),
};
#elif defined(CONFIG_MRCH_SUN8IW18P1)
static efuse_key_map_new_t g_key_info[] = {
	EFUSE_DEF_ITEM(EFUSE_ROTPK_NAME, 0x40, 256, -1, -1, EFUSE_RO),
	EFUSE_DEF_ITEM("", 0, 0, 0, 0, EFUSE_PRIVATE),
};
#else
#ifndef SCC_ROTPK_BURNED_FLAG
#define SCC_ROTPK_BURNED_FLAG		(12)
#endif
/*Please extend key_maps for new arch here*/
static efuse_key_map_new_t g_key_info[] = {
#ifdef EFUSE_ROTPK
	EFUSE_DEF_ITEM(EFUSE_ROTPK_NAME, EFUSE_ROTPK, 256, -1, SCC_ROTPK_BURNED_FLAG, EFUSE_RO),
#endif
#ifdef EFUSE_SSK
	EFUSE_DEF_ITEM(EFUSE_SSK_NAME, EFUSE_SSK, SID_SSK_SIZE, -1, SCC_SSK_BURNED_FLAG, EFUSE_RO),
#endif
#ifdef EFUSE_OEM_PROGRAM
	EFUSE_DEF_ITEM(EFUSE_OEM_NAME, EFUSE_OEM_PROGRAM, SID_OEM_PROGRAM_SIZE, -1, -1, EFUSE_RO),
#endif
#ifdef EFUSE_OEM_PROGRAM_SECURE
	EFUSE_DEF_ITEM(EFUSE_OEM_SEC_NAME, EFUSE_OEM_PROGRAM_SECURE, SID_OEM_PROGRAM_SECURE_SIZE, -1, -1, EFUSE_RO),
#endif
	EFUSE_DEF_ITEM("", 0, 0, 0, 0, EFUSE_PRIVATE),
};
#endif

__weak int set_efuse_voltage(int vol)
{
	return 0;
}

/*Please reference 1728 spec page11 to know why to add this function
*burn efuse :efuse sram can not get the latest value
*unless via sid read or reboot.
*/
static uint __sid_reg_read_key(uint key_index)
{
	uint reg_val;
	reg_val = readl(SID_PRCTL);
	reg_val &= ~((0x1ff << 16) | 0x3);
	reg_val |= key_index << 16;
	writel(reg_val, SID_PRCTL);
	reg_val &= ~((0xff << 8) | 0x3);
	reg_val |= (SID_OP_LOCK << 8) | 0x2;
	writel(reg_val, SID_PRCTL);
	while (readl(SID_PRCTL) & 0x2) {
		;
	}
	reg_val &= ~((0x1ff << 16) | (0xff << 8) | 0x3);
	writel(reg_val, SID_PRCTL);
	reg_val = readl(SID_RDKEY);
	return reg_val;
}

uint sid_read_key(uint key_index)
{
	return __sid_reg_read_key(key_index);
}

void sid_program_key(uint key_index, uint key_value)
{
	uint reg_val;

	set_efuse_voltage(1900);
#ifdef EFUSE_HV_SWITCH
	writel(1, EFUSE_HV_SWITCH);
#endif
	writel(key_value, SID_PRKEY);
	reg_val = readl(SID_PRCTL);
	reg_val &= ~((0x1ff << 16) | 0x3);
	reg_val |= key_index << 16;
	writel(reg_val, SID_PRCTL);
	reg_val &= ~((0xff << 8) | 0x3);
	reg_val |= (SID_OP_LOCK << 8) | 0x1;
	writel(reg_val, SID_PRCTL);
	while (readl(SID_PRCTL) & 0x1) {
		;
	}
	reg_val &= ~((0x1ff << 16) | (0xff << 8) | 0x3);
	writel(reg_val, SID_PRCTL);

	set_efuse_voltage(1800);
#ifdef EFUSE_HV_SWITCH
	writel(0, EFUSE_HV_SWITCH);
#endif

	return;
}

#define EFUSE_BURN_MAX_TRY_CNT 3
static int uni_burn_key(uint key_index, uint key_value)
{
	uint key_burn_bitmask = ~(sid_read_key(key_index)) & key_value;
	int fail	      = 0;

	while (key_burn_bitmask) {
		sid_program_key(key_index, key_burn_bitmask);

		if (fail > EFUSE_BURN_MAX_TRY_CNT) {
			EFUSE_DBG("[efuse] warn: **uni_burn_key failed **");
			return -1;
		}
		key_burn_bitmask &= (~(__sid_reg_read_key(key_index)));
		fail++;
	}
	return 0;
}

int sid_set_security_mode(void)
{
#if defined(EFUSE_LCJS)
#ifdef CONFIG_ARCH_SUN50IW1P1
	return uni_burn_key(EFUSE_LCJS, ((0x1 << SECURE_BIT_OFFSET) |
				  (0x1 << SBROM_ACCELERATION_ENABLE_BIT)));
#else
	return uni_burn_key(EFUSE_LCJS, (0x1 << SECURE_BIT_OFFSET));
#endif
#elif defined(EFUSE_ANTI_BRUSH)
	return uni_burn_key(EFUSE_ANTI_BRUSH, (0x1 << ANTI_BRUSH_BIT_OFFSET));
#else
	return 0;
#endif
}

int sid_probe_security_mode(void)
{
#if defined(EFUSE_LCJS)
	return (sid_read_key(EFUSE_LCJS) >> SECURE_BIT_OFFSET) & 1;
#elif defined(EFUSE_ANTI_BRUSH)
	return (sid_read_key(EFUSE_ANTI_BRUSH) >> ANTI_BRUSH_BIT_OFFSET) & 1;
#else
	return 0;
#endif
}

#ifdef SID_SECURE_MODE
int sid_get_security_status(void)
{
	return readl(SID_SECURE_MODE) & 0x1;
}
#else
int sid_get_security_status(void)
{
	return sid_probe_security_mode();
}
#endif

static void _set_cfg_flg(int efuse_cfg_base, int bit_offset)
{
	uni_burn_key(efuse_cfg_base, (1 << bit_offset));
	return;
}

static int _get_burned_flag(efuse_key_map_new_t *key_map)
{
	if (key_map->burned_flg_offset < 0) {
		return 0;
	} else {
		return (sid_read_key(EFUSE_WRITE_PROTECT) >>
			(key_map->burned_flg_offset &
			 EFUSE_BRUN_RD_OFFSET_MASK)) &
		       1;
	}
}

static int __sw_acl_ck(efuse_key_map_new_t *key_map, int burn)
{
	if (key_map->sw_rule == EFUSE_PRIVATE) {
		EFUSE_DBG("\n[efuse]%s: PRIVATE\n", key_map->name);
		return EFUSE_ERR_PRIVATE;
	}
	if (burn == 0) {
		if (key_map->sw_rule == EFUSE_NACCESS) {
			EFUSE_DBG("\n[efuse]%s:NACCESS\n", key_map->name);
			return EFUSE_ERR_NO_ACCESS;
		}
	} else {
		/*If already burned:*/
		if (_get_burned_flag(key_map)) {
			if ((key_map->sw_rule == EFUSE_NACCESS) ||
			    (key_map->sw_rule == EFUSE_RO)) {
				EFUSE_DBG("\n[efuse]%s: already burned\n",
					  key_map->name);
				return EFUSE_ERR_ALREADY_BURNED;
			}
		}
		if (key_map->sw_rule == EFUSE_RW) {
			key_map->burned_flg_offset |= EFUSE_ACL_SET_BRUN_BIT;
			key_map->rd_fbd_offset |= EFUSE_ACL_SET_RD_FORBID_BIT;
		}
	}
	return 0;
}

/*Efuse access control rule check.*/
static int __efuse_acl_ck(efuse_key_map_new_t *key_map, int burn)
{
	/*For normal solution only check EFUSE_PRIVATE,other will be seemed as EFUSE_RW */
	if (sid_get_security_status() == 0) {
		if (key_map->sw_rule == EFUSE_PRIVATE) {
			return EFUSE_ERR_PRIVATE;
		}
		return 0;
	}
	int ret = __sw_acl_ck(key_map, burn);
	if (ret) {
		return ret;
	}
	if (burn) {
		if (_get_burned_flag(key_map)) {
			/*already burned*/
			pr_err("[efuse]%s:already burned\n", key_map->name);
			EFUSE_DBG("config reg:0x%x\n",
				  sid_read_key(EFUSE_WRITE_PROTECT));
			return EFUSE_ERR_ALREADY_BURNED;
		}

	} else {
		if ((key_map->rd_fbd_offset >= 0) &&
		    ((sid_read_key(EFUSE_READ_PROTECT) >>
		      key_map->rd_fbd_offset) &
		     1)) {
			/*Read is not allowed because of the read forbidden bit was set*/
			pr_err("[efuse]%s forbid bit set\n", key_map->name);
			EFUSE_DBG("config reg:0x%x\n",
				  sid_read_key(EFUSE_READ_PROTECT));
			return EFUSE_ERR_READ_FORBID;
		}
	}
	return 0;
}

/*get a uint value from unsigned char *k_src*/
static unsigned int _get_uint_val(unsigned char *k_src)
{
	unsigned int test = 0x12345678;
	if ((unsigned long)k_src & 0x3) {
		/*big endian*/
		if (*(unsigned char *)(&test) == 0x12) {
			memcpy((void *)&test, (void *)k_src, 4);
			return test;
		} else {
			test = ((*(k_src + 3)) << 24) | ((*(k_src + 2)) << 16) |
			       ((*(k_src + 1)) << 8) | (*k_src);
			return test;
		}
	} else {
		return *(unsigned int *)k_src;
	}
}

int sunxi_efuse_write(void *key_inf)
{
	efuse_key_info_t *list = (efuse_key_info_t *)key_inf;
	unsigned char *k_src   = NULL;
	unsigned int niddle = 0, tmp_data = 0, k_d_lft = 0;
	efuse_key_map_new_t *key_map = g_key_info;

	if (list == NULL) {
		EFUSE_DBG("[efuse] error: key_inf is null\n");
		return EFUSE_ERR_ARG;
	}
	/* search key via name*/
	for (; key_map->size != 0; key_map++) {
		if (!memcmp(list->name, key_map->name, strlen(key_map->name))) {
			EFUSE_DBG("key name = %s\n", key_map->name);
			EFUSE_DBG("key offset = 0x%x\n", key_map->offset);
			/* check if there is enough space to store the key*/
			if ((key_map->size >> 3) < list->len) {
				EFUSE_DBG("key name = %s\n", key_map->name);
				EFUSE_DBG("[efuse] error: no enough space\
					, dst size(%d), src size(%d)\n",
					  key_map->size >> 3, list->len);
				return EFUSE_ERR_KEY_SIZE_TOO_BIG;
			}
			break;
		}
	}

	if (key_map->size == 0) {
		pr_err("[sunxi_efuse_write] error: unknow key\n");
		return EFUSE_ERR_KEY_NAME_WRONG;
	}

	int ret = __efuse_acl_ck(key_map, 1);
	if (ret) {
		pr_err("[sunxi_efuse_write] error: NO ACCESS\n");
		return ret;
	}

	EFUSE_DBG_DUMP(list->name, list->key_data, list->len, EFUSE_DUMP_LEN);
	niddle  = key_map->offset;
	k_d_lft = list->len;
	k_src   = list->key_data;

	while (k_d_lft >= 4) {
		tmp_data = _get_uint_val(k_src);
		EFUSE_DBG("offset:0x%x val:0x%x\n", niddle, tmp_data);
		if (tmp_data) {
			if (uni_burn_key(niddle, tmp_data)) {
				return EFUSE_ERR_BURN_TIMING;
			}
		}
		k_d_lft -= 4;
		niddle += 4;
		k_src += 4;
	}

	if (k_d_lft) {
		uint mask = (1UL << (k_d_lft << 3)) - 1;
		tmp_data  = _get_uint_val(k_src);
		mask &= tmp_data;
		EFUSE_DBG("offset:0x%x val:0x%x\n", niddle, mask);
		if (mask) {
			if (uni_burn_key(niddle, mask)) {
				return EFUSE_ERR_BURN_TIMING;
			}
		}
	}
	/*Already burned bit: Set this bit to indicate it is already burned.*/
	if ((key_map->burned_flg_offset >= 0) &&
	    (key_map->burned_flg_offset <= EFUSE_BRUN_RD_OFFSET_MASK) &&
	    sid_get_security_status()) {
		_set_cfg_flg(EFUSE_WRITE_PROTECT, key_map->burned_flg_offset);
	}
	/*Read forbidden bit: Set to indicate cpu can not access this key again.*/
	if ((key_map->rd_fbd_offset >= 0) &&
	    (key_map->rd_fbd_offset <= EFUSE_BRUN_RD_OFFSET_MASK) &&
	    sid_get_security_status()) {
		_set_cfg_flg(EFUSE_READ_PROTECT, key_map->rd_fbd_offset);
	}
	return 0;
}

/*This API assume the caller already
*prepared enough buffer to receive data.
*Because the lenth of key is exported as MACRO*/
#define EFUSE_ROUND_UP(x, y) ((((x) + ((y)-1)) / (y)) * (y))
int sunxi_efuse_read(void *key_name, void *rd_buf, int *len)
{
	efuse_key_map_new_t *key_map = g_key_info;
	uint tmp = 0, i = 0, k_u32_l = 0, bit_lft = 0;
	int offset = 0, tmp_sz = 0;
	__attribute__((unused)) int show_status     = 0;
	unsigned int *u32_p = (unsigned int *)rd_buf;
	unsigned char *u8_p = (unsigned char *)rd_buf;

	*len = 0;
	if (!(key_name && rd_buf)) {
		EFUSE_DBG("[efuse] error arg check fail\n");
		return EFUSE_ERR_ARG;
	}
	/* search key via name*/
	for (; key_map->size != 0; key_map++) {
		if (!memcmp(key_name, key_map->name, strlen(key_map->name))) {
			break;
		}
	}

	if (key_map->size == 0) {
		EFUSE_DBG("[efuse] error: unknow key name\n");
		return EFUSE_ERR_KEY_NAME_WRONG;
	}

	int ret = __efuse_acl_ck(key_map, 0);
	if (ret) {
		EFUSE_DBG("[sunxi_efuse_write] error: acl check fail\n");
		return ret;
	}

	EFUSE_DBG("key name:%s key size:%d key offset:0x%X\n", key_map->name,
		  key_map->size, key_map->offset);
	k_u32_l = key_map->size / 32;
	bit_lft = key_map->size % 32;
	offset  = key_map->offset;
	for (i = 0; i < k_u32_l; i++) {
		tmp = sid_read_key(offset);
		if (((unsigned long)rd_buf & 0x3) == 0) {
			u32_p[i] = tmp;
		} else {
			memcpy((void *)(u8_p + i * 4), (void *)(&tmp), 4);
		}
		offset += 4;
		tmp_sz += 4;
	}

	if (bit_lft) {
		EFUSE_DBG("bit lft is %d\n", bit_lft);
		tmp = sid_read_key(offset);
		memcpy((void *)(u8_p + k_u32_l * 4), (void *)(&tmp),
		       EFUSE_ROUND_UP(bit_lft, 8)/8);
		tmp_sz += EFUSE_ROUND_UP(bit_lft, 8)/8;
	}
	*len = tmp_sz;

	return 0;
}

#ifndef SID_ROTPK_CTRL
int sunxi_efuse_get_rotpk_status(void)
{
	return -1;
}
#else
int sunxi_efuse_get_rotpk_status(void)
{
	int ret;
	ret = (readl(SID_ROTPK_CTRL) & (1 << SID_ROTPK_EFUSED_BIT)) ==
		(1 << SID_ROTPK_EFUSED_BIT);
	return ret;
}
#endif

int sunxi_efuse_verify_rotpk(u8 *hash)
{
#ifdef SID_ROTPK_CMP_RET_BIT
	int i;
	u32 *tmp = (u32 *)hash;
	u32 val;

	for (i = 0; i < 8 ; i++) {
		writel(tmp[i], SID_ROTPK_VALUE(i));
	}

	val = readl(SID_ROTPK_CTRL);
	val |= (0x1U << 31);
	writel(val, SID_ROTPK_CTRL);

	for (i = 0; i < 30; i++) {
		;
	}

	val = readl(SID_ROTPK_CTRL);
	if (val & (1 << SID_ROTPK_EFUSED_BIT)) {
		if (val & (1 << SID_ROTPK_CMP_RET_BIT)) {
			return 0;
		}
		return -2;
	}
	pr_err("rotpk not init\n");
	return 0;
#else
	return -1;
#endif
}

#ifdef SID_GET_SOC_VER
int sunxi_efuse_get_soc_ver(void)
{
	int ret;
	ret = (readl(SID_EFUSE) >> SID_SOC_VER_OFFSET) & SID_SOC_VER_MASK;
	return ret;
}
#else
int sunxi_efuse_get_soc_ver(void)
{
	return 0;
}
#endif
