/*#####################################################################
# File Describe:rtl8363.c
# Author: flyranchaoflyranchao
# Created Time:flyranchao@allwinnertech.com
# Created Time:2021年08月18日 星期三 11时47分09秒
#====================================================================*/
// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2000-2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

/*
 * Misc boot support
 */
#include <common.h>
#include <command.h>
#include <net.h>
#include<rtk_types.h>
#include<rate.h>
#include<l2.h>
#include<rtl8367c_asicdrv_i2c.h>
#include<rtl8367c_base.h>
#include<vlan.h>
#include<rtl8367c_asicdrv_meter.h>
#include<port.h>
#include<rtl8367c_asicdrv_igmp.h>
#include<stat.h>
#include<oam.h>
#include<rtl8367c_asicdrv_trunking.h>
#include<rtl8367c_asicdrv_acl.h>
#include<rtl8367c_asicdrv_mirror.h>
#include<led.h>
#include<smi.h>
#include<rtl8367c_asicdrv_vlan.h>
#include<rtl8367c_asicdrv_fc.h>
#include<rtl8367c_asicdrv_dot1x.h>
#include<qos.h>
#include<leaky.h>
#include<rtl8367c_asicdrv_storm.h>
#include<rtl8367c_asicdrv_eav.h>
#include<rtl8367c_asicdrv_phy.h>
#include<rtl8367c_asicdrv_green.h>
#include<rtk_switch.h>
#include<rtl8367c_asicdrv_svlan.h>
#include<rtl8367c_asicdrv_portIsolation.h>
#include<rtl8367c_asicdrv_scheduling.h>
#include<interrupt.h>
#include<rtl8367c_asicdrv_mib.h>
#include<rtl8367c_asicdrv_oam.h>
#include<rtl8367c_asicdrv_inbwctrl.h>
#include<rtl8367c_reg.h>
#include<rtl8367c_asicdrv_led.h>
#include<rtk_cpu.h>
#include<acl.h>
#include<rtl8367c_asicdrv_interrupt.h>
#include<eee.h>
#include<ptp.h>
#include<rtk_error.h>
#include<svlan.h>
#include<rtl8367c_asicdrv_lut.h>
#include<rtl8367c_asicdrv_misc.h>
#include<rtl8367c_asicdrv_cputag.h>
#include<rtl8367c_asicdrv_qos.h>
#include<mirror.h>
#include<rtl8367c_asicdrv_port.h>
#include<dot1x.h>
#include<storm.h>
#include<rldp.h>
#include<igmp.h>
#include<trap.h>
#include<rtk_i2c.h>
#include<rtl8367c_asicdrv_hsb.h>
#include<trunk.h>
#include<rtl8367c_asicdrv.h>
#include<rtl8367c_asicdrv_unknownMulticast.h>
//#include<rtl8367c_asicdrv_rldp.h>
#include<rtl8367c_asicdrv_eee.h>
#include<rtl8367c_asicdrv_rma.h>

static int do_rtl8363(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	rtk_port_mac_ability_t mac_cfg;
	rtk_mode_ext_t mode;

	if(rtk_switch_init() != RT_ERR_OK)
	{
		printf("rtk switch init failed!\n");
		return -1;
	}
	mode = MODE_EXT_RGMII;
	mac_cfg.forcemode = MAC_NORMAL;
	mac_cfg.speed = SPD_100M;
	mac_cfg.duplex = FULL_DUPLEX;
	mac_cfg.link = PORT_LINKUP;
	mac_cfg.nway = DISABLED;
	mac_cfg.txpause = ENABLED;
	mac_cfg.rxpause = ENABLED;

	if(rtk_port_macForceLinkExt_set(EXT_PORT0, mode, &mac_cfg) != RT_ERR_OK)
	{
		printf("macForceLinkExt set failed!\n");
	        return -1;
	}

	rtk_port_rgmiiDelayExt_set(EXT_PORT0, 1, 0);
	rtk_port_phyEnableAll_set(ENABLED);
	return 0;
}
/* -------------------------------------------------------------------- */

U_BOOT_CMD(
	rtl8363, CONFIG_SYS_MAXARGS, 1,	do_rtl8363,
	"start application at address 'addr'",
	"addr [arg ...]\n    - start application at address 'addr'\n"
	"      passing 'arg' as arguments"
);
