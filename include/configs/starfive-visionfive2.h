// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2021 Shanghai StarFive Technology Co., Ltd.
 * YanHong  Wang <yanhong.wang@starfivetech.com>
 */


#ifndef _STARFIVE_VISIONFIVE2_H
#define _STARFIVE_VISIONFIVE2_H

#include <version.h>
#include <linux/sizes.h>

#ifdef CONFIG_SPL

#define CONFIG_SPL_MAX_SIZE		0x00040000
#define CONFIG_SPL_BSS_START_ADDR	0x08040000
#define CONFIG_SPL_BSS_MAX_SIZE		0x00010000
#define CONFIG_SYS_SPL_MALLOC_START	0x42000000

#define CONFIG_SYS_SPL_MALLOC_SIZE	0x00800000

#define CONFIG_SPL_STACK		(0x08000000 + 0x00180000 - \
					 GENERATED_GBL_DATA_SIZE)
#define STARFIVE_SPL_BOOT_LOAD_ADDR	0x60000000
#endif

#define CONFIG_SYS_BOOTM_LEN            SZ_64M


#define CONFIG_SYS_CACHELINE_SIZE 64

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_CBSIZE	1024	/* Console I/O Buffer Size */

/*
 * Print Buffer Size
 */
#define CONFIG_SYS_PBSIZE					\
	(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)

/*
 * max number of command args
 */
#define CONFIG_SYS_MAXARGS	16

/*
 * Boot Argument Buffer Size
 */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE

/*
 * Size of malloc() pool
 * 512kB is suggested, (CONFIG_ENV_SIZE + 128 * 1024) was not enough
 */
#define CONFIG_SYS_MALLOC_LEN		SZ_8M

#define CONFIG_SYS_SDRAM_BASE		0x40000000

/* Init Stack Pointer */
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SYS_SDRAM_BASE + SZ_8M)

#define CONFIG_SYS_LOAD_ADDR		(CONFIG_SYS_SDRAM_BASE + SZ_16M)
#define CONFIG_STANDALONE_LOAD_ADDR	(CONFIG_SYS_SDRAM_BASE + SZ_16M)

#define CONFIG_SYS_PCI_64BIT		/* enable 64-bit PCI resources */

/*
 * Ethernet
 */
#ifdef CONFIG_CMD_NET
#define CONFIG_DW_ALTDESCRIPTOR
#define CONFIG_ARP_TIMEOUT	500
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_IPADDR		192.168.120.230
#define CONFIG_IP_DEFRAG
#ifndef CONFIG_NET_MAXDEFRAG
#define CONFIG_NET_MAXDEFRAG	16384
#endif
#endif

/* HACK these should have '#if defined (stuff) around them like zynqp*/
#define BOOT_TARGET_DEVICES(func) func(MMC, mmc, 0) func(DHCP, dhcp, na)

#include <config_distro_bootcmd.h>


#include <environment/distro/sf.h>

#define TYPE_GUID_LOADER1	"5B193300-FC78-40CD-8002-E86C45580B47"
#define TYPE_GUID_LOADER2	"2E54B353-1271-4842-806F-E436D6AF6985"
#define TYPE_GUID_SYSTEM	"0FC63DAF-8483-4772-8E79-3D69D8477DE4"

#define CPU_VOL_1020_SET \
	"cpu_vol_1020_set=" 			\
	"fdt set /opp-table-0/opp-1500000000 opp-microvolt <1020000>;\0"

#define CPU_VOL_1040_SET \
	"cpu_vol_1040_set="			\
	"fdt set /opp-table-0/opp-1500000000 opp-microvolt <1040000>;\0"

#define CPU_VOL_1060_SET \
	"cpu_vol_1060_set="			\
	"fdt set /opp-table-0/opp-1500000000 opp-microvolt <1060000>;\0"

#define CPU_SPEED_1250_SET \
	"cpu_speed_1250_set="			\
	"fdt rm /opp-table-0/opp-375000000;"	\
	"fdt rm /opp-table-0/opp-500000000;"	\
	"fdt rm /opp-table-0/opp-750000000;"	\
	"fdt rm /opp-table-0/opp-1500000000;\0"

#define CPU_SPEED_1500_SET \
	"cpu_speed_1500_set="			\
	"fdt rm /opp-table-0/opp-312500000;"	\
	"fdt rm /opp-table-0/opp-417000000;"	\
	"fdt rm /opp-table-0/opp-625000000;"	\
	"fdt rm /opp-table-0/opp-1250000000;\0"

#define CPU_FREQ_VOL_SET \
	"cpu_vol_set="						\
	"if test ${cpu_max_vol} = 1000000; then "		\
		"run cpu_speed_1250_set; "			\
	"else "							\
		"run cpu_speed_1500_set; "			\
		"if test ${cpu_max_vol} = 1060000; then "	\
			"run cpu_vol_1060_set; "		\
		"elif test ${cpu_max_vol} = 1020000; then "	\
			"run cpu_vol_1020_set; "		\
		"else "						\
			"run cpu_vol_1040_set; "		\
		"fi; "						\
	"fi; \0"

#define CMA_SIZE_SET \
	"cma_start=70000000\0"					\
	"cma_1g=b000000\0"					\
	"cma_2g=20000000\0"					\
	"cma_4g=40000000\0"	 				\
	"cma_8g=60000000\0"					\
	"cma_node=/reserved-memory/linux,cma\0"			\
	"cma_ddr1g_set="					\
	"fdt set ${cma_node} size <0x0 0x${cma_1g}>;"		\
	"fdt set ${cma_node} alloc-ranges <0x0 0x${cma_start} 0x0 0x${cma_1g}>;\0" \
	"cma_ddr2g_set="					\
	"fdt set ${cma_node} size <0x0 0x${cma_2g}>;"		\
	"fdt set ${cma_node} alloc-ranges <0x0 0x${cma_start} 0x0 0x${cma_2g}>;\0" \
	"cma_ddr4g_set="					\
	"fdt set ${cma_node} size <0x0 0x${cma_4g}>;"		\
	"fdt set ${cma_node} alloc-ranges <0x0 0x${cma_start} 0x0 0x${cma_4g}>;\0" \
	"cma_ddr8g_set="					\
	"fdt set ${cma_node} size <0x0 0x${cma_8g}>;"		\
	"fdt set ${cma_node} alloc-ranges <0x0 0x${cma_start} 0x0 0x${cma_8g}>;\0" \
	"cma_resize="						\
	"if test ${memory_size} -eq 40000000; then "		\
		"run cma_ddr1g_set;"				\
	"elif test ${memory_size} -eq 80000000; then "		\
		"run cma_ddr2g_set;"				\
	"elif test ${memory_size} -eq 100000000; then "		\
		"run cma_ddr4g_set;"				\
	"elif test ${memory_size} -ge 200000000; then "		\
		"run cma_ddr8g_set;"				\
	"fi; \0 "

#define PARTS_DEFAULT							\
	"name=loader1,start=17K,size=1M,type=${type_guid_gpt_loader1};" \
	"name=loader2,size=4MB,type=${type_guid_gpt_loader2};"		\
	"name=system,size=-,bootable,type=${type_guid_gpt_system};"

#define CHIPA_GMAC_SET	\
	"chipa_gmac_set="	\
	"fdt set /soc/ethernet@16030000/ethernet-phy@0 tx_inverted_10 <0x0>;"	\
	"fdt set /soc/ethernet@16030000/ethernet-phy@0 tx_inverted_100 <0x0>;"	\
	"fdt set /soc/ethernet@16030000/ethernet-phy@0 tx_inverted_1000 <0x0>;"	\
	"fdt set /soc/ethernet@16030000/ethernet-phy@0 tx_delay_sel <0x9>;"	\
	"fdt set /soc/ethernet@16040000/ethernet-phy@1 tx_inverted_10 <0x0>;"	\
	"fdt set /soc/ethernet@16040000/ethernet-phy@1 tx_inverted_100 <0x0>;"	\
	"fdt set /soc/ethernet@16040000/ethernet-phy@1 tx_inverted_1000 <0x0>;"	\
	"fdt set /soc/ethernet@16040000/ethernet-phy@1 tx_delay_sel <0x9> \0"

#define VISIONFIVE2_MEM_SET	\
	"visionfive2_mem_set="	\
	"fdt memory ${memory_addr} ${memory_size};" \
	"run cma_resize; \0"

#define CHIPA_SET	\
	"chipa_set="				\
	"if test ${chip_vision} = A; then "	\
		"run chipa_gmac_set;"		\
	"fi; \0"				\
	"chipa_set_uboot="			\
	"fdt addr ${uboot_fdt_addr};"		\
	"run chipa_set;\0"			\
	"chipa_set_linux="			\
	"fdt addr ${fdt_addr_r};"		\
	"run visionfive2_mem_set;"		\
	"run chipa_set;\0"

#define VF2_SDK_BOOTENV			\
	"bootenv=uEnv.txt\0"		\
	"bootenv_sdk=vf2_uEnv.txt\0"	\
	"boot_devs=mmc nvme\0"		\
	"emmc_devnum=0\0" 		\
	"sd_devnum=1\0"			\
	"mmc_devnum_l=1 0\0"		\
	"nvme_devnum_l=0 0\0"

#define JH7110_SDK_BOOTENV		\
	"bootdir=/boot\0"		\
	"bootpart=3\0"			\
	"rootpart=4\0"			\
	"load_sdk_uenv="		\
		"fatload ${bootdev} ${devnum}:${bootpart} ${loadaddr} ${bootenv_sdk};"	\
		"env import -t ${loadaddr} ${filesize}; \0"				\
	"mmc_test_and_boot="				\
		"if mmc dev ${devnum}; then "	\
			"echo Try booting from MMC${devnum} ...; "	\
			"setenv sdev_blk mmcblk${devnum}p${rootpart};"	\
			"run load_sdk_uenv; run boot2;"	\
		"fi;\0"							\
	"bootenv_mmc="					\
		"setenv bootdev mmc;"			\
		"if test ${bootmode} = flash; then "	\
			"for mmc_devnum in ${mmc_devnum_l}; do " \
				"setenv devnum ${mmc_devnum}; " \
				"run mmc_test_and_boot;"	\
			"done;"					\
		"fi; "						\
		"if test ${bootmode} = sd; then "	\
			"setenv devnum ${sd_devnum};" 	\
			"run mmc_test_and_boot;"	\
		"fi; " 					\
		"if test ${bootmode} = emmc; then "	\
			"setenv devnum  ${emmc_devnum};"\
			"run mmc_test_and_boot;"	\
		"fi; \0"				\
	"bootenv_nvme="					\
		"if test ${bootmode} = flash; then "	\
			"for nvme_devnum in ${nvme_devnum_l}; do " \
				"setenv devnum ${nvme_devnum};" \
				"if pci enum; then "		\
					"nvme scan; "		\
				"fi; "				\
				"if nvme dev ${devnum}; then "	\
					"echo Try booting from NVME${devnum} ...; "	\
					"setenv bootdev nvme;"	\
					"setenv sdev_blk nvme${devnum}n1p${rootpart};"	\
					"run load_sdk_uenv; run boot2;"	\
				"fi; "				\
			"done; "				\
		"fi; \0"					\
	"sdk_boot_env="			\
		"for bootdev_s in ${boot_devs}; do "	\
		"run bootenv_${bootdev_s}; "		\
		"done;\0"				\
	"fdtfile=" CONFIG_DEFAULT_FDT_FILE "\0"

#define JH7110_DISTRO_BOOTENV	\
	"bootdir=/boot\0"	\
	"bootpart=3\0"		\
	"rootpart=4\0"		\
	"load_distro_uenv="						\
		"fatload ${bootdev} ${devnum}:${bootpart} ${loadaddr} /${bootenv}; " \
		"env import ${loadaddr} ${filesize}; \0" \
	"fdt_loaddtb="		\
		"fatload ${bootdev} ${devnum}:${bootpart} ${fdt_addr_r} /dtbs/${fdtfile}; fdt addr ${fdt_addr_r}; \0" \
	"fdt_sizecheck="	\
		"fatsize ${bootdev} ${devnum}:${bootpart} /dtbs/${fdtfile}; \0" \
	"set_fdt_distro="	\
		"run chipa_set_linux; run cpu_vol_set;" \
		"fatwrite ${bootdev} ${devnum}:${bootpart} ${fdt_addr_r} /dtbs/${fdtfile} ${filesize}; \0" \
	"bootcmd_distro="	\
		"run load_distro_uenv; " \
		"run fdt_loaddtb; run fdt_sizecheck; run set_fdt_distro; "	\
		"sysboot ${bootdev} ${devnum}:${bootpart} fat ${scriptaddr} /${boot_syslinux_conf}; \0" \
	"distro_mmc_test_and_boot="					\
		"if mmc dev ${devnum}; then "				\
			"echo Try booting from MMC${devnum} ...; "	\
			"run bootcmd_distro;"				\
		"fi;\0" 						\
	"distro_bootenv_mmc="					\
		"setenv bootdev mmc;"			\
		"if test ${bootmode} = flash; then "	\
			"for mmc_devnum in ${mmc_devnum_l}; do "\
				"setenv devnum ${mmc_devnum}; " \
				"run distro_mmc_test_and_boot;" \
			"done;" 				\
		"fi; "						\
		"if test ${bootmode} = sd; then "	\
			"setenv devnum ${sd_devnum};"	\
			"run distro_mmc_test_and_boot;" \
		"fi; "					\
		"if test ${bootmode} = emmc; then "	\
			"setenv devnum	${emmc_devnum};"\
			"run distro_mmc_test_and_boot;" \
		"fi; \0"				\
	"distro_bootenv_nvme="				\
		"if test ${bootmode} = flash; then "	\
			"for nvme_devnum in ${nvme_devnum_l}; do " \
				"setenv devnum ${nvme_devnum};" \
				"if pci enum; then "		\
					"nvme scan; "		\
				"fi; "				\
				"if nvme dev ${devnum}; then "	\
					"echo Try booting from NVME${devnum} ...; "	\
					"setenv bootdev nvme;"	\
					"run bootcmd_distro; "	\
				"fi; "				\
			"done; "				\
		"fi; \0"					\
	"distro_boot_env="		\
		"echo Tring booting distro ...;"		\
		"for bootdev_s in ${boot_devs}; do "		\
			"run distro_bootenv_${bootdev_s}; "	\
		"done; \0"

#define CONFIG_EXTRA_ENV_SETTINGS			\
	"fdt_high=0xffffffffffffffff\0"			\
	"initrd_high=0xffffffffffffffff\0"		\
	"kernel_addr_r=0x40200000\0"			\
	"kernel_comp_addr_r=0x5a000000\0"		\
	"kernel_comp_size=0x4000000\0"			\
	"fdt_addr_r=0x46000000\0"			\
	"scriptaddr=0x43900000\0"			\
	"script_offset_f=0x1fff000\0"			\
	"script_size_f=0x1000\0"			\
	"pxefile_addr_r=0x45900000\0"			\
	"ramdisk_addr_r=0x46100000\0"			\
	"fdtoverlay_addr_r=0x4f000000\0"		\
	"loadaddr=0x60000000\0"				\
	VF2_SDK_BOOTENV					\
	JH7110_SDK_BOOTENV				\
	JH7110_DISTRO_BOOTENV				\
	CHIPA_GMAC_SET					\
	CHIPA_SET					\
	CPU_VOL_1020_SET				\
	CPU_VOL_1040_SET				\
	CPU_VOL_1060_SET				\
	CPU_SPEED_1250_SET				\
	CPU_SPEED_1500_SET				\
	CPU_FREQ_VOL_SET				\
	CMA_SIZE_SET					\
	VISIONFIVE2_MEM_SET				\
	"type_guid_gpt_loader1=" TYPE_GUID_LOADER1 "\0" \
	"type_guid_gpt_loader2=" TYPE_GUID_LOADER2 "\0" \
	"type_guid_gpt_system=" TYPE_GUID_SYSTEM "\0"	\
	"partitions=" PARTS_DEFAULT "\0"		\
	BOOTENV						\
	BOOTENV_SF

#define CONFIG_SYS_BAUDRATE_TABLE {9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600}
#define CONFIG_SYS_LOADS_BAUD_CHANGE 1		/* allow baudrate change */

/* 6.25MHz RTC clock, StarFive JH7110*/
#define CONFIG_SYS_HZ_CLOCK	4000000

#define __io

#define memset_io(c, v, l)	memset((c), (v), (l))
#define memcpy_fromio(a, c, l)	memcpy((a), (c), (l))
#define memcpy_toio(c, a, l)	memcpy((c), (a), (l))

#define CONFIG_VIDEO_BMP_LOGO
#define CONFIG_VIDEO_LOGO
#define CONFIG_BMP_16BPP
#define CONFIG_BMP_24BPP
#define CONFIG_BMP_32BPP

#endif /* _STARFIVE_EVB_H */

