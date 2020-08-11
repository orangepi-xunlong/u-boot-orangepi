/*
 *  drivers/arisc/arisc.c
 *
 * Copyright (c) 2012 Allwinner.
 * 2012-05-01 Written by sunny (sunny@allwinnertech.com).
 * 2012-10-01 Written by superm (superm@allwinnertech.com).
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "arisc_i.h"
#include <smc.h>
#include <sunxi_board.h>

//if all value are zero , compile will not allocate space to this global variable.
//use them before uboot Relocation will cause error.
struct dts_cfg dts_cfg =
{
	.dram_para = {0x1,0x1}
};
struct dts_cfg_64 dts_cfg_64 =
{
	.dram_para = {0x1,0x1}
};
unsigned int arisc_debug_level = 2;
#if 0 //boot load scp,so not need this param
static int get_image_addr_size(phys_addr_t *image_addr, size_t *image_size)
{
	struct spare_boot_head_t *header;
	phys_addr_t offset;
	size_t size;

	/* Obtain a reference to the image by querying the platform layer */
        header = &uboot_spare_head;
        offset = header->boot_ext[2].data[0];
        size = header->boot_ext[2].data[1];

        if(0 == offset || 0 == size)
        {
                printf("arisc image file not found\n");
                return -1;
        }

	*image_addr = (ptrdiff_t)CONFIG_SYS_TEXT_BASE + offset;
	*image_size = size;

	return 0;
}
#endif
int arisc_dvfs_cfg_vf_table(void)
{
	u32    value = 0;
	int    index = 0;
	int    nodeoffset;
	u32    vf_table_size = 0;
	char   vf_table_main_key[64];
	char   vf_table_sub_key[64];
	u32    vf_table_count = 0;
	u32    vf_table_type = 0;

	//nodeoffset = fdt_node_offset_by_compatible(working_fdt, -1, "allwinner,dvfs_table");
	nodeoffset = fdt_path_offset(working_fdt, "/dvfs_table");

	if (IS_ERR_VALUE(nodeoffset)) {
		ARISC_ERR("get [allwinner,dvfs_table] device node error\n");
		return -EINVAL;
	}

	if (IS_ERR_VALUE(fdt_getprop_u32(working_fdt, nodeoffset, "vf_table_count", &vf_table_count))) {
		ARISC_LOG("%s: support only one vf_table\n", __func__);
		sprintf(vf_table_main_key, "%s", "/dvfs_table");
	} else {
		//vf_table_type = sunxi_get_soc_bin();
		sprintf(vf_table_main_key, "%s%d", "/vf_table", vf_table_type);
	}
	ARISC_INF("%s: vf table type [%d=%s]\n", __func__, vf_table_type, vf_table_main_key);

	//nodeoffset = fdt_node_offset_by_compatible(working_fdt, -1, vf_table_main_key);
	nodeoffset = fdt_path_offset(working_fdt, vf_table_main_key);
	if (IS_ERR_VALUE(nodeoffset)) {
		ARISC_ERR("get [%s] device node error\n", vf_table_main_key);
		return -EINVAL;
	}

	/* parse system config v-f table information */
	if (IS_ERR_VALUE(fdt_getprop_u32(working_fdt, nodeoffset, "lv_count", &vf_table_size))) {
		ARISC_ERR("parse system config dvfs_table size fail\n");
		return -EINVAL;
	}
	for (index = 0; index < vf_table_size; index++) {
		sprintf(vf_table_sub_key, "lv%d_freq", index + 1);
		if (!IS_ERR_VALUE(fdt_getprop_u32(working_fdt, nodeoffset, vf_table_sub_key, &value))) {
			dts_cfg.vf[index].freq = value;
		}
		ARISC_INF("%s: freq [%s-%d=%d]\n", __func__, vf_table_sub_key, index, value);
		sprintf(vf_table_sub_key, "lv%d_volt", index + 1);
		if (!IS_ERR_VALUE(fdt_getprop_u32(working_fdt, nodeoffset, vf_table_sub_key, &value))) {
			dts_cfg.vf[index].voltage = value;
		}
		ARISC_INF("%s: volt [%s-%d=%d]\n", __func__, vf_table_sub_key, index, value);
	}

	return 0;
}

static int sunxi_arisc_parse_cfg(void)
{
	u32 value[4] = {0, 0, 0, 0};
	int nodeoffset;
	/* parse arisc_space node */
	nodeoffset = fdt_path_offset(working_fdt, "/soc/arisc_space");
	if (IS_ERR_VALUE(nodeoffset)) {
		ARISC_ERR("get [allwinner,arisc_space] device node error\n");
		return -EINVAL;
	}
	if (IS_ERR_VALUE(fdt_getprop_u32(working_fdt, nodeoffset, "space4", value))) {
		ARISC_ERR("get arisc_space space4 error\n");
		return -EINVAL;
	}

	dts_cfg.space.msgpool_dst = (phys_addr_t)value[0];
	dts_cfg.space.msgpool_offset = (phys_addr_t)value[1];
	dts_cfg.space.msgpool_size = (size_t)value[2];
	ARISC_INF("arisc_space space4 msgpool_dst:0x%p, msgpool_offset:0x%p, msgpool_size:0x%zx, \n",
		(void *)dts_cfg.space.msgpool_dst, (void *)dts_cfg.space.msgpool_offset, dts_cfg.space.msgpool_size);

	/* parse msgbox node */
	nodeoffset = fdt_path_offset(working_fdt,  "/soc/msgbox");
	if (IS_ERR_VALUE(nodeoffset)) {
		ARISC_ERR("get [allwinner,msgbox] device node error\n");
		return -EINVAL;
	}

	if (IS_ERR_VALUE(fdt_getprop_u32(working_fdt, nodeoffset, "reg", value))) {
		ARISC_ERR("get msgbox reg error\n");
		return -EINVAL;
	}

	dts_cfg.msgbox.base = (phys_addr_t)value[1];
	dts_cfg.msgbox.size = (size_t)value[3];

	dts_cfg.msgbox.status = fdtdec_get_is_enabled(working_fdt, nodeoffset);

	ARISC_INF("msgbox base:0x%p, size:0x%zx, status:%u\n",
		(void *)dts_cfg.msgbox.base, dts_cfg.msgbox.size, dts_cfg.msgbox.status);

	/* parse hwspinlock node */
	//nodeoffset = fdt_node_offset_by_compatible(working_fdt, -1, "allwinner,sunxi-hwspinlock");
	nodeoffset = fdt_path_offset(working_fdt, "/soc/hwspinlock");
	if (IS_ERR_VALUE(nodeoffset)) {
		ARISC_ERR("get [allwinner,sunxi-hwspinlock] device node error\n");
		return -EINVAL;
	}

	if (IS_ERR_VALUE(fdt_getprop_u32(working_fdt, nodeoffset, "reg", value))) {
		ARISC_ERR("get sunxi-hwspinlock reg error\n");
		return -EINVAL;
	}

	dts_cfg.hwspinlock.base = (phys_addr_t)value[1];
	dts_cfg.hwspinlock.size = (size_t)value[3];

	dts_cfg.hwspinlock.status = fdtdec_get_is_enabled(working_fdt, nodeoffset);

	ARISC_INF("hwspinlock base:0x%p, size:0x%zx, status:%u\n",
		(void *)dts_cfg.hwspinlock.base, dts_cfg.hwspinlock.size, dts_cfg.hwspinlock.status);

	return 0;
}

static void sunxi_arisc_transform_cfg(void)
{

	memcpy((void *)&dts_cfg_64.dram_para, (const void *)&dts_cfg.dram_para, sizeof(struct dram_para));
	memcpy((void *)&dts_cfg_64.vf, (const void *)&dts_cfg.vf, sizeof(struct arisc_freq_voltage) * ARISC_DVFS_VF_TABLE_MAX);

	dts_cfg_64.space.sram_dst = (u64)dts_cfg.space.sram_dst;
	dts_cfg_64.space.sram_offset = (u64)dts_cfg.space.sram_offset;
	dts_cfg_64.space.sram_size = (u64)dts_cfg.space.sram_size;
	dts_cfg_64.space.dram_dst = (u64)dts_cfg.space.dram_dst;
	dts_cfg_64.space.dram_offset = (u64)dts_cfg.space.dram_offset;
	dts_cfg_64.space.dram_size = (u64)dts_cfg.space.dram_size;
	dts_cfg_64.space.para_dst = (u64)dts_cfg.space.para_dst;
	dts_cfg_64.space.para_offset = (u64)dts_cfg.space.para_offset;
	dts_cfg_64.space.para_size = (u64)dts_cfg.space.para_size;
	dts_cfg_64.space.msgpool_dst = (u64)dts_cfg.space.msgpool_dst;
	dts_cfg_64.space.msgpool_offset = (u64)dts_cfg.space.msgpool_offset;
	dts_cfg_64.space.msgpool_size = (u64)dts_cfg.space.msgpool_size;
	dts_cfg_64.space.standby_dst = (u64)dts_cfg.space.standby_dst;
	dts_cfg_64.space.standby_offset = (u64)dts_cfg.space.standby_offset;
	dts_cfg_64.space.standby_size = (u64)dts_cfg.space.standby_size;

	dts_cfg_64.image.base = (u64)dts_cfg.image.base;
	dts_cfg_64.image.size = (u64)dts_cfg.image.size;

	dts_cfg_64.prcm.base = (u64)dts_cfg.prcm.base;
	dts_cfg_64.prcm.size = (u64)dts_cfg.prcm.size;

	dts_cfg_64.cpuscfg.base = (u64)dts_cfg.cpuscfg.base;
	dts_cfg_64.cpuscfg.size = (u64)dts_cfg.cpuscfg.size;

	dts_cfg_64.msgbox.base = (u64)dts_cfg.msgbox.base;
	dts_cfg_64.msgbox.size = (u64)dts_cfg.msgbox.size;
	dts_cfg_64.msgbox.irq = dts_cfg.msgbox.irq;
	dts_cfg_64.msgbox.status = dts_cfg.msgbox.status;

	dts_cfg_64.hwspinlock.base = (u64)dts_cfg.hwspinlock.base;
	dts_cfg_64.hwspinlock.size = (u64)dts_cfg.hwspinlock.size;
	dts_cfg_64.hwspinlock.irq = dts_cfg.hwspinlock.irq;
	dts_cfg_64.hwspinlock.status = dts_cfg.hwspinlock.status;

	dts_cfg_64.s_uart.base = (u64)dts_cfg.s_uart.base;
	dts_cfg_64.s_uart.size = (u64)dts_cfg.s_uart.size;
	dts_cfg_64.s_uart.irq = dts_cfg.s_uart.irq;
	dts_cfg_64.s_uart.status = dts_cfg.s_uart.status;

#if defined CONFIG_ARCH_SUN50IW2P1
	dts_cfg_64.s_twi.base = (u64)dts_cfg.s_twi.base;
	dts_cfg_64.s_twi.size = (u64)dts_cfg.s_twi.size;
	dts_cfg_64.s_twi.irq = dts_cfg.s_twi.irq;
	dts_cfg_64.s_twi.status = dts_cfg.s_twi.status;
#else
	dts_cfg_64.s_rsb.base = (u64)dts_cfg.s_rsb.base;
	dts_cfg_64.s_rsb.size = (u64)dts_cfg.s_rsb.size;
	dts_cfg_64.s_rsb.irq = dts_cfg.s_rsb.irq;
	dts_cfg_64.s_rsb.status = dts_cfg.s_rsb.status;
#endif
	dts_cfg_64.s_jtag.base = (u64)dts_cfg.s_jtag.base;
	dts_cfg_64.s_jtag.size = (u64)dts_cfg.s_jtag.size;
	dts_cfg_64.s_jtag.irq = dts_cfg.s_jtag.irq;
	dts_cfg_64.s_jtag.status = dts_cfg.s_jtag.status;

	dts_cfg_64.s_cir.base = (u64)dts_cfg.s_cir.base;
	dts_cfg_64.s_cir.size = (u64)dts_cfg.s_cir.size;
	dts_cfg_64.s_cir.irq = dts_cfg.s_cir.irq;
	dts_cfg_64.s_cir.status = dts_cfg.s_cir.status;
	memcpy(&dts_cfg_64.s_cir.ir_key, &dts_cfg.s_cir.ir_key, sizeof(ir_key_t));
	memcpy(&dts_cfg_64.start_os, &dts_cfg.start_os, sizeof(box_start_os_cfg_t));
	memcpy((void *)&dts_cfg_64.pmu, (const void *)&dts_cfg.pmu, sizeof(struct pmu_cfg));
	memcpy((void *)&dts_cfg_64.power, (const void *)&dts_cfg.power, sizeof(struct power_cfg));
	flush_dcache_range((unsigned long)&dts_cfg_64, ((unsigned long)&dts_cfg_64 + sizeof(struct dts_cfg_64)));

}

int sunxi_arisc_probe(void)
{
	if(!sunxi_probe_secure_monitor())
	{
		return 0;
	}

	ARISC_LOG("arisc initialize\n");
	sunxi_arisc_parse_cfg();

	/* because the uboot is arch32,
	 * and the runtime server is arch64,
	 * should transform the dts_cfg to dts_cfg_64.
	 */
	sunxi_arisc_transform_cfg();
	ARISC_LOG("arisc para ok\n");

	if (arm_svc_arisc_startup((ulong)&dts_cfg_64)) {
		/* arisc initialize failed */
		ARISC_ERR("sunxi-arisc driver startup failed\n");
	} else {
		/* arisc initialize succeeded */
		ARISC_LOG("sunxi-arisc driver startup succeeded\n");
	}

	return 0;
}

int sunxi_arisc_wait_ready(void)
{
	if(get_boot_work_mode()!= WORK_MODE_BOOT)
		return 0;

	arm_svc_arisc_wait_ready();
	return 0;
}

int sunxi_arisc_fake_poweroff(void)
{
	arm_svc_arisc_fake_poweroff();

	return 0;
}
