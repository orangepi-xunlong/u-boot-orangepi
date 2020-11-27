/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <asm/io.h>
#include <smc.h>
#include <sunxi_board.h>
#include "teesmc.h"
#include <securestorage.h>

DECLARE_GLOBAL_DATA_PTR;

#define ARM_SVC_CALL_COUNT          0x8000ff00
#define ARM_SVC_UID                 0x8000ff01
//0x8000ff02 reserved
#define ARM_SVC_VERSION             0x8000ff03
#define ARM_SVC_RUNNSOS             0x8000ff04

#define ARM_SVC_READ_SEC_REG        0x8000ff05
#define ARM_SVC_WRITE_SEC_REG       0x8000ff06

#define PSCI_CPU_OFF                0x84000002
#define PSCI_CPU_ON_AARCH32         0x84000003

#define SUNXI_CPU_ON_AARCH32        0x84000010
#define SUNXI_CPU_OFF_AARCH32       0x84000011
#define SUNXI_CPU_WFI_AARCH32       0x84000012
//arisc
#define ARM_SVC_ARISC_STARTUP                   0x8000ff10
#define ARM_SVC_ARISC_WAIT_READY                0x8000ff11
#define ARM_SVC_ARISC_READ_PMU                  0x8000ff12
#define ARM_SVC_ARISC_WRITE_PMU                 0x8000ff13
#define ARM_SVC_ARISC_FAKE_POWER_OFF_REQ_ARCH32 0x8000ff14

//efuse
#define ARM_SVC_EFUSE_READ         (0x8000fe00)
#define ARM_SVC_EFUSE_WRITE        (0x8000fe01)
#define ARM_SVC_EFUSE_PROBE_SECURE_ENABLE_AARCH32    (0x8000fe03)

/*
*note pResult is will
*/
u32 sunxi_smc_call(ulong arg0, ulong arg1, ulong arg2, ulong arg3, ulong pResult)
{
	u32 ret = 0;
	static u32 result[4] = {0};
	ret = __sunxi_smc_call(arg0,arg1,arg2,arg3);
	if(pResult != 0)
	{
		__asm volatile("str r0,[%0]": : "r" (result+0));
		__asm volatile("str r1,[%0]": : "r" (result+1));
		__asm volatile("str r2,[%0]": : "r" (result+2));
		__asm volatile("str r3,[%0]": : "r" (result+3));
		__asm volatile("": :  : "memory");
		*(u32*)pResult = (u32)result;
	}
	return ret;
}


int arm_svc_set_cpu_on(int cpu, uint entry)
{
	return sunxi_smc_call(PSCI_CPU_ON_AARCH32, cpu, entry, 0, 0);
}

int arm_svc_set_cpu_off(int cpu)
{
	return sunxi_smc_call(PSCI_CPU_OFF, cpu, 0, 0, 0);
}

int arm_svc_set_cpu_wfi(void)
{
	return sunxi_smc_call(SUNXI_CPU_WFI_AARCH32, 0, 0, 0, 0);
}

int arm_svc_version(u32* major, u32* minor)
{
	u32 *pResult = NULL;
	int ret = 0;
	ret = sunxi_smc_call(ARM_SVC_VERSION, 0, 0, 0, (ulong)&pResult);
	if(ret < 0)
	{
		return ret;
	}

	*major = pResult[0];
	*minor = pResult[1];
	return ret;
}

int arm_svc_call_count(void)
{
	return sunxi_smc_call(ARM_SVC_CALL_COUNT, 0, 0, 0,0);
}

int arm_svc_uuid(u32 *uuid)
{
	return sunxi_smc_call(ARM_SVC_UID, 0, 0, 0, (ulong)uuid);
}

int arm_svc_run_os(ulong kernel, ulong fdt, ulong arg2)
{
	return sunxi_smc_call(ARM_SVC_RUNNSOS, kernel, fdt, arg2,0);
}

u32 arm_svc_read_sec_reg(ulong reg)
{
	return sunxi_smc_call(ARM_SVC_READ_SEC_REG, reg, 0, 0, 0);
}

int arm_svc_write_sec_reg(u32 val,ulong reg)
{
	sunxi_smc_call(ARM_SVC_WRITE_SEC_REG, reg, val, 0,0);
	return 0;

}

int arm_svc_arisc_startup(ulong cfg_base)
{
	return sunxi_smc_call(ARM_SVC_ARISC_STARTUP,cfg_base, 0, 0,0);
}

int arm_svc_arisc_wait_ready(void)
{
	return sunxi_smc_call(ARM_SVC_ARISC_WAIT_READY, 0, 0, 0, 0);
}

int arm_svc_arisc_fake_poweroff(void)
{
	return sunxi_smc_call(ARM_SVC_ARISC_FAKE_POWER_OFF_REQ_ARCH32, 0, 0, 0, 0);
}

u32 arm_svc_arisc_read_pmu(ulong addr)
{
	return sunxi_smc_call(ARM_SVC_ARISC_READ_PMU,addr, 0, 0, 0);
}

int arm_svc_arisc_write_pmu(ulong addr,u32 value)
{
	return sunxi_smc_call(ARM_SVC_ARISC_WRITE_PMU,addr, value, 0, 0);
}

int arm_svc_efuse_read(void *key_buf, void *read_buf)
{
	return sunxi_smc_call(ARM_SVC_EFUSE_READ, (ulong)key_buf, (ulong)read_buf, 0, 0);
}

int arm_svc_efuse_write(void *key_buf)
{
	return sunxi_smc_call(ARM_SVC_EFUSE_WRITE, (ulong)key_buf, 0, 0, 0);
}

int arm_svc_probe_secure_mode(void)
{
	return sunxi_smc_call(ARM_SVC_EFUSE_PROBE_SECURE_ENABLE_AARCH32, 0, 0, 0, 0);
}


static __inline u32 smc_readl_normal(ulong addr)
{
	return readl(addr);
}
static __inline int smc_writel_normal(u32 value, ulong addr)
{
	writel(value, addr);
	return 0;
}

u32 (* smc_readl_pt)(ulong addr) = smc_readl_normal;
int (* smc_writel_pt)(u32 value, ulong addr) = smc_writel_normal;

u32 smc_readl(ulong addr)
{
	return smc_readl_pt(addr);
}

void smc_writel(u32 val,ulong addr)
{
	smc_writel_pt(val,addr);
}

int smc_efuse_readl(void *key_buf, void *read_buf)
{
	printf("smc_efuse_readl is just a interface,you must override it\n");
	return 0;
}

int smc_efuse_writel(void *key_buf)
{
	printf("smc_efuse_writel is just a interface,you must override it\n");
	return 0;
}


int smc_init(void)
{
	if ((sunxi_get_securemode() == SUNXI_SECURE_MODE_WITH_SECUREOS) || sunxi_probe_secure_monitor())
	{
		smc_readl_pt = arm_svc_read_sec_reg;
		smc_writel_pt = arm_svc_write_sec_reg;
	}

	return 0;
}

typedef struct _aes_function_info
{
	uint32_t  decrypt;
	uint32_t  key_addr;
	uint32_t  key_len;
	uint32_t  iv_map;
	uint32_t  key_mode;
}aes_function_info;

struct smc_tee_ssk_addr_group
{
	uint32_t out_tee;
	uint32_t in_tee;
	uint32_t private;
};

int smc_tee_ssk_encrypt(char *out_buf, char *in_buf, int len, int *out_len)
{
	struct smc_param param = { 0 };
	struct smc_tee_ssk_addr_group *tee_addr;
	int align_len = 0;

	align_len = ALIGN(len,16);
	param.a0 = TEESMC32_CALL_SSK_CRYP;
	param.a1 = TEESMC_PROBE_SHM_BASE;
	tee_smc_call(&param);

	if (param.a0 != 0) {
		printf("smc tee probe share memory base failed\n ");

		return -1;
	}
	tee_addr = (struct smc_tee_ssk_addr_group *)param.a1;
	tee_addr->in_tee = param.a1 + 0x100;
	tee_addr->out_tee = tee_addr->in_tee + 4096;
	memset((void *)tee_addr->in_tee,0x0,align_len);
	memcpy((void *)tee_addr->in_tee, in_buf, len);
	flush_cache(tee_addr->in_tee, align_len);
	flush_cache(tee_addr->out_tee, align_len);
	flush_cache((uint32_t)tee_addr, sizeof(struct smc_tee_ssk_addr_group));
	memset(&param, 0, sizeof(param));
	param.a0 = TEESMC32_CALL_SSK_CRYP;
	param.a1 = TEESMC_SSK_ENCRYPT;
	param.a2 = (uint32_t)tee_addr;
	param.a3 = align_len;
	tee_smc_call(&param);

	if (param.a0 != 0) {
		printf("smc tee encrypt with ssk failed\n ");

		return -1;
	}
	memcpy(out_buf, (void *)tee_addr->out_tee, align_len);
	*out_len = align_len;
	return 0;
}

int smc_tee_ssk_decrypt(char *out_buf, char *in_buf, int len)
{
	struct smc_param param = { 0 };
	struct smc_tee_ssk_addr_group *tee_addr;

	param.a0 = TEESMC32_CALL_SSK_CRYP;
	param.a1 = TEESMC_PROBE_SHM_BASE;
	tee_smc_call(&param);

	if (param.a0 != 0) {
		printf("smc tee probe share memory base failed\n ");

		return -1;
	}

	tee_addr = (struct smc_tee_ssk_addr_group *)param.a1;
	tee_addr->in_tee = param.a1 + 0x100;
	tee_addr->out_tee = tee_addr->in_tee + 4096;
	memcpy((void *)tee_addr->in_tee, in_buf, len);
	memset(&param, 0, sizeof(param));
	flush_cache(tee_addr->in_tee, len);
	flush_cache(tee_addr->out_tee, len);
	flush_cache((uint32_t)tee_addr, sizeof(struct smc_tee_ssk_addr_group));
	param.a0 = TEESMC32_CALL_SSK_CRYP;
	param.a1 = TEESMC_SSK_DECRYPT;
	param.a2 = (uint32_t)tee_addr;
	param.a3 = len;
	tee_smc_call(&param);

	if (param.a0 != 0) {
		printf("smc tee decrypt with ssk failed\n ");

		return -1;
	}
	memcpy(out_buf, (void *)tee_addr->out_tee, len);

	return 0;
}

int smc_tee_rssk_encrypt(char *out_buf, char *in_buf, int len, int *out_len)
{
	struct smc_param param = { 0 };
	struct smc_tee_ssk_addr_group *tee_addr;
	int align_len = 0;

	align_len = ALIGN(len,16);
	param.a0 = TEESMC32_CALL_SSK_CRYP;
	param.a1 = TEESMC_PROBE_SHM_BASE;
	tee_smc_call(&param);

	if (param.a0 != 0) {
		printf("smc tee probe share memory base failed\n ");

		return -1;
	}
	tee_addr = (struct smc_tee_ssk_addr_group *)param.a1;
	tee_addr->in_tee = param.a1 + 0x100;
	tee_addr->out_tee = tee_addr->in_tee + 4096;
	memset((void *)tee_addr->in_tee,0x0,align_len);
	memcpy((void *)tee_addr->in_tee, in_buf, len);
	flush_cache(tee_addr->in_tee, align_len);
	flush_cache(tee_addr->out_tee, align_len);
	flush_cache((uint32_t)tee_addr, sizeof(struct smc_tee_ssk_addr_group));
	memset(&param, 0, sizeof(param));
	param.a0 = TEESMC32_CALL_SSK_CRYP;
	param.a1 = TEESMC_RSSK_ENCRYPT;
	param.a2 = (uint32_t)tee_addr;
	param.a3 = align_len;
	tee_smc_call(&param);

	if (param.a0 != 0) {
		printf("smc tee encrypt with ssk failed\n ");

		return -1;
	}
	memcpy(out_buf, (void *)tee_addr->out_tee, align_len);
	*out_len = align_len;
	return 0;
}

int smc_aes_rssk_decrypt_to_keysram(void)
{
	struct smc_param param = { 0 };
	struct smc_tee_ssk_addr_group *tee_addr;

	param.a0 = TEESMC32_CALL_SSK_CRYP;
	param.a1 = TEESMC_PROBE_SHM_BASE;
	tee_smc_call(&param);

	if (param.a0 != 0) {
		printf("smc tee probe share memory base failed\n ");
		return -1;
	}

	tee_addr = (struct smc_tee_ssk_addr_group *)param.a1;
	tee_addr->in_tee = param.a1 + 0x100;
	tee_addr->out_tee = tee_addr->in_tee + 4096;
	//memcpy((void *)tee_addr->in_tee, in_buf, len);
	memset(&param, 0, sizeof(param));
	//flush_cache(tee_addr->in_tee, len);
	//flush_cache(tee_addr->out_tee, len);
	flush_cache((uint32_t)tee_addr, sizeof(struct smc_tee_ssk_addr_group));
	param.a0 = TEESMC32_CALL_SSK_CRYP;
	param.a1 = TEESMC_RSSK_DECRYPT;
	param.a2 = 0;
	param.a3 = SUNXI_HDCP_KEY_LEN;
	tee_smc_call(&param);

	if (param.a0 != 0) {
		printf("smc tee decrypt with ssk failed\n ");
		return -1;
	}
	return 0;

}

int smc_aes_algorithm(char *out_buf, char *in_buf, int data_len, char* pkey, int key_mode, int decrypt)
{
	struct smc_param param = { 0 };
	struct smc_tee_ssk_addr_group *tee_addr;
	aes_function_info* p_aes_info = NULL;
	int len = data_len;
	uint8_t* aes_key = NULL;

	param.a0 = TEESMC32_CALL_SSK_CRYP;
	param.a1 = TEESMC_PROBE_SHM_BASE;
	tee_smc_call(&param);

	if (param.a0 != 0) {
		printf("smc tee probe share memory base failed\n ");
		return -1;
	}

	tee_addr = (struct smc_tee_ssk_addr_group *)param.a1;
	tee_addr->in_tee = param.a1 + 0x100;
	tee_addr->out_tee = tee_addr->in_tee + 4096;
	tee_addr->private = tee_addr->out_tee + 4096;
	aes_key = (uint8_t*)(tee_addr->private +256);

	p_aes_info = (aes_function_info*)tee_addr->private;
	p_aes_info->decrypt = decrypt;
	p_aes_info->key_len = 128/8; //key len is 128 bit
	p_aes_info->key_addr = (uint32_t)aes_key;
	memcpy((void*)p_aes_info->key_addr,pkey,p_aes_info->key_len);

	memcpy((void *)tee_addr->in_tee, in_buf, len);
	memset(&param, 0, sizeof(param));
	flush_cache(tee_addr->in_tee, len);
	flush_cache(tee_addr->out_tee, len);
	flush_cache(tee_addr->private, sizeof(aes_function_info));
	flush_cache(p_aes_info->key_addr,p_aes_info->key_len);
	flush_cache((uint32_t)tee_addr, sizeof(struct smc_tee_ssk_addr_group));
	param.a0 = TEESMC32_CALL_SSK_CRYP;
	param.a1 = TEESMC_AES_CBC;
	param.a2 = (uint32_t)tee_addr;
	param.a3 = len;
	tee_smc_call(&param);

	if (param.a0 != 0) {
		printf("smc tee decrypt with ssk failed\n ");
		return -1;
	}
	memcpy(out_buf, (void *)tee_addr->out_tee, len);
	return 0;
}


int smc_tee_keybox_store(const char *name, char *in_buf, int len)
{
	struct smc_param param = { 0 };
	sunxi_secure_storage_info_t *key_box;

	param.a0 = TEESMC32_CALL_SSK_CRYP;
	param.a1 = TEESMC_PROBE_SHM_BASE;
	tee_smc_call(&param);

	if (param.a0 != 0) {
		printf("smc tee probe share memory base failed\n ");

		return -1;
	}

	key_box = (sunxi_secure_storage_info_t *)param.a1;
	memcpy(key_box, in_buf, len);
	printf("len=%d\n", len);
	printf("name=%s\n", key_box->name);
	printf("len=%d\n",  key_box->len);
	printf("encrypt=%d\n", key_box->encrypted);
	printf("write_protect=%d\n", key_box->write_protect);
	printf("******************\n");
	flush_cache((ulong)key_box, sizeof(sunxi_secure_storage_info_t));

	memset(&param, 0, sizeof(param));
	param.a0 = TEESMC32_CALL_SSK_CRYP;
	param.a1 = TEESMC_KEYBOX_STORE;
	param.a2 = (uint32_t)key_box;
	tee_smc_call(&param);

	if (param.a0 != 0) {
		printf("smc tee decrypt with ssk failed\n ");

		return -1;
	}

	return 0;
}

int smc_tee_probe_drm_configure(ulong *drm_base, ulong *drm_size)
{
	struct smc_param param = { 0 };

	param.a0 = TEESMC32_CALL_GET_DRM_INFO;
	param.a1 = TEESMC_DRM_PHY_INFO;
	tee_smc_call(&param);

	if (param.a0 != 0) {
		printf("drm config service not available: %X", (uint)param.a0);
		return -1;
	}

	*drm_base = param.a1;
	*drm_size = param.a2;

	return 0;
}

