/*
************************************************************************************************************************
*                                          Boot rom
*                                         Seucre Boot
*
*                             Copyright(C), 2006-2013, AllWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name   : Base.h
*
* Author      : glhuang
*
* Version     : 0.0.1
*
* Date        : 2013.09.05
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* glhuang       2013.09.05       0.0.1        build the file
*
************************************************************************************************************************
*/
#include "common.h"
#include "sbrom_toc.h"
#include "boot_type.h"
#include "openssl_ext.h"
#include "asm/arch/clock.h"
#include "asm/arch/ss.h"
#include "asm/arch/timer.h"
#include "asm/arch/uart.h"
#include "asm/arch/rtc_region.h"
#include "asm/arch/mmu.h"
#include "asm/arch/gic.h"
#include "private_toc.h"
#include "sbrom_toc.h"
#include "../libs/sbrom_libs.h"
#include <asm/arch/dram.h>
#include <private_toc.h>
#include <sunxi_cfg.h>
#include <asm/arch/key.h>
#include <asm/arch/base_pmu.h>
#ifdef CONFIG_SUNXI_MULITCORE_BOOT
#include <asm/arch/platsmp.h>
#endif
#ifdef  CONFIG_SUNXI_KEY_LADDER
#include "../keyladder/keyladder.h"
#endif
extern void disp_init(void);
static int pmu_type = 0;

extern void sid_read_rotpk(void *dst);
extern void sunxi_certif_mem_reset(void);
extern int sunxi_certif_probe_pubkey(X509 *x, sunxi_key_t *pubkey);
extern void ndump(u8 *buf, int count);

static void print_commit_log(void);
static int sbromsw_toc1_traverse(u32 dram_size);

static int sbromsw_clear_env(void);
static int sunxi_root_certif_pk_verify(sunxi_certif_info_t *sunxi_certif, u8 *buf, u32 len);
#ifdef SUNXI_OTA_TEST
static int sbromsw_print_ota_test(void);
#endif

extern __s32 boot_set_gpio(void  *user_gpio_list, __u32 group_count_max, __s32 set_gpio);

static int check_update_key(int key_value);
static int uart_input_value, lradc_input_value;
extern int debug_enable;


sbrom_toc0_config_t *toc0_config = (sbrom_toc0_config_t *)(CONFIG_TOC0_CONFIG_ADDR);
extern char sbromsw_hash_value[64];


int mmc_config_addr = (int)(((sbrom_toc0_config_t *)CONFIG_TOC0_CONFIG_ADDR)->storage_data + 160);

void __attribute__((weak)) set_gpio_gate(void)
{
	return;
}

int __attribute__((weak)) probe_power_key(void)
{
	return 0;
}

typedef struct _toc_name_addr_tbl
{
	char name[32];
	uint addr;
}toc_name_addr_tbl;

static toc_name_addr_tbl toc_name_addr[] = {
	{ITEM_SCP_NAME,SCP_SRAM_BASE},
#ifdef CONFIG_RELOCATE_PARAMETER
	{ITEM_PARAMETER_NAME,CONFIG_SUNXI_PARAMETER_ADDR},
#endif
#ifdef CONFIG_SUNXI_ESM_HDCP
	{ITEM_ESM_IMG_NAME,SUNXI_ESM_IMG_BUFF_ADDR},
#endif
#ifdef CONFIG_SUNXI_MULITCORE_BOOT
	{ITEM_LOGO_NAME,SUNXI_LOGO_COMPRESSED_LOGO_BUFF},
	{ITEM_SHUTDOWNCHARGE_LOGO_NAME,SUNXI_SHUTDOWN_CHARGE_COMPRESSED_LOGO_BUFF},
	{ITEM_ANDROIDCHARGE_LOGO_NAME,SUNXI_ANDROID_CHARGE_COMPRESSED_LOGO_BUFF},
#endif
	{ITEM_DTB_NAME,CONFIG_DTB_STORE_IN_DRAM_BASE},
	{ITEM_SOCCFG_NAME,CONFIG_SOCCFG_STORE_IN_DRAM_BASE},
	{ITEM_BDCFG_NAME,CONFIG_BDCFG_STORE_IN_DRAM_BASE},
	{ITEM_BDCFG_FEX_NAME,CONFIG_BDCFG_FEX_STORE_IN_DRAM_BASE}
};

static int get_item_addr(char* name, uint* addr)
{
	int i = 0;
	for(i = 0; i < ARRAY_SIZE(toc_name_addr); i++)
	{
		if(0 == strcmp(toc_name_addr[i].name, name))
		{
			*addr =  toc_name_addr[i].addr;
			return 0;
		}
	}
	return -1;
}

#define   KEY_DELAY_MAX          (8)
#define   KEY_DELAY_EACH_TIME    (40)
#define   KEY_MAX_COUNT_GO_ON    ((KEY_DELAY_MAX * 1000)/(KEY_DELAY_EACH_TIME))
#define HEADER_OFFSET  (0x4000)

static int check_update_key(int key_value)
{
	int time_tick = 0;
	int count = 0;
	int power_key;

	if(key_value <= 0)
		return 0;

	probe_power_key();			//clear power key status
	do
	{
		time_tick++;
		__msdelay(KEY_DELAY_EACH_TIME);
		//detect power key status for fel mode
		power_key = probe_power_key();
		if (power_key > 0)
		{
			count ++;
		}
		if(count == 3)
		{
			pr_force("you can loosen the key to update now\n");
			//jump to fel
			return -1;
		}

		if(time_tick > KEY_MAX_COUNT_GO_ON)
		{
			pr_force("time out\n");

			return 0;
		}
	} while(sunxi_key_read() > 0); //press key and not loosen

	pr_force("key not pressed anymore\n");
	return 0;
}

static void print_commit_log(void)
{
	pr_force("HELLO! BOOT0 is starting!\n");
	pr_force("sbrom commit : %s \n",sbromsw_hash_value);
}

void sbromsw_entry(void)
{
	toc0_private_head_t *toc0 = (toc0_private_head_t *)CONFIG_SBROMSW_BASE;
	uint dram_size;
	int  ret, flag;
	__maybe_unused int  cpu1_power_on = 0;

#ifdef CONFIG_SUNXI_KEY_LADDER
	key_ladder_init(toc0);
#endif
	timer_init();
	sunxi_serial_init(toc0_config->uart_port, toc0_config->uart_ctrl, 2);
    print_commit_log();

	if( toc0_config->enable_jtag )
	{
		boot_set_gpio((normal_gpio_cfg *)toc0_config->jtag_gpio, 5, 1);
	}

	uart_input_value = set_debugmode_flag();
	sunxi_key_init();

	pmu_type = pmu_init(toc0_config->power_mode);
#ifdef CONFIG_SUNXI_MULITCORE_BOOT
	set_pll_voltage(CONFIG_SUNXI_CORE_VOL);
#endif
	set_pll_in_secure_mode();
	set_gpio_gate();

	printf("try to probe rtc region\n");
#ifdef SUNXI_OTA_TEST
	sbromsw_print_ota_test();
#endif

	//detect step1: rtc
	flag = rtc_region_probe_fel_flag();
        pr_msg("flag=0x%x\n", flag);
	if(flag == SUNXI_RUN_EFEX_FLAG)
	{
		pr_force("probe fel flag\n");
		rtc_region_clear_fel_flag();

		goto __sbromsw_entry_err0;
	}
	//detect step2: uart input
	if (uart_input_value == '2') {
		pr_force("detected user input 2\n");
		goto __sbromsw_entry_err0;
	}
	//detect step3: key input
	lradc_input_value = sunxi_key_read();
	if (check_update_key(lradc_input_value) ) {
		pr_force("detect power key status for fel mode \n");
		goto __sbromsw_entry_err;
	}
	//dram init
#ifdef FPGA_PLATFORM
	dram_size = mctl_init((void *)toc0_config->dram_para);
#else
	dram_size = init_DRAM(0, (void *)toc0_config->dram_para);
#endif
	if (dram_size)
	{
		//toc0_config->dram_para[4] &= 0xffff0000;
		//toc0_config->dram_para[4] |= (dram_size & 0xffff);
		printf("init dram ok, size=%dM\n", dram_size);
	}
	else
	{
		printf("init dram fail\n");

		goto __sbromsw_entry_err;
	}

	mmu_setup();

	printf("init heap\n");
	create_heap(CONFIG_HEAP_BASE, CONFIG_HEAP_SIZE);
	if(toc0->platform[0] & 0xf0)
	{
		printf("read toc0 from emmc backup\n");
	}
	ret = sunxi_flash_init(toc0->platform[0] & 0x0f);
	if(ret)
	{
		pr_error("error: %s:%d\n", __func__, __LINE__);
		goto __sbromsw_entry_err;
	}
	ret = toc1_init();
	if(ret)
	{
		pr_error("error: %s:%d\n", __func__, __LINE__);
		goto __sbromsw_entry_err;
	}

#ifdef CONFIG_SUNXI_HDMI_IN_BOOT0
	ret = fetch_parameter();
	if(ret) {
		pr_error("error: %s:%d\n", __func__, __LINE__);
		goto __sbromsw_entry_err;
	}

	disp_init();
#endif

	ret = sbromsw_toc1_traverse(dram_size);
	if(ret)
	{
		pr_error("error: %s:%d\n", __func__, __LINE__);
		goto __sbromsw_entry_err;
	}

__sbromsw_entry_err:
__sbromsw_entry_err0:

	printf_all();
	sbromsw_clear_env();

	boot0_jump(SUNXI_FEL_ADDR_IN_SECURE);
}

#define  SUNXI_X509_CERTIFF_MAX_LEN   (4096)
static int sbromsw_toc1_traverse(u32 dram_size)
{
	sbrom_toc1_item_group item_group;
	int ret;
	uint len, i;
        __maybe_unused int dram_para_offset = 0;
        __maybe_unused void *dram_para_addr = (void *)toc0_config->dram_para;
	u8 buffer[SUNXI_X509_CERTIFF_MAX_LEN];

	sunxi_certif_info_t  root_certif;
	sunxi_certif_info_t  sub_certif;
	u8  hash_of_file[256];
	uint optee_entry=0;
	uint monitor_entry=0, uboot_entry=0;
	__maybe_unused struct spare_boot_head_t *bfh = NULL;
	__maybe_unused struct spare_monitor_head *monitor_head = NULL;

	if (toc1_item_traverse())
		return -1;

	pr_msg("probe root certif\n");
	sunxi_ss_open();

	memset(buffer, 0, SUNXI_X509_CERTIFF_MAX_LEN);
	len = toc1_item_read_rootcertif(buffer, SUNXI_X509_CERTIFF_MAX_LEN);
	if(!len)
	{
		pr_error("error: cant read rootkey certif\n");

		return -1;
	}
	if(sunxi_root_certif_pk_verify(&root_certif, buffer, len))
	{
		pr_error("root certif pk verify failed\n");
		return -1;
	}
	else
	{
		pr_msg("certif valid: the root key is valid\n");
	}
	if(sunxi_certif_verify_itself(&root_certif, buffer, len))
	{
		pr_error("root certif verify itself failed\n");
		return -1;
	}
	do
	{
		memset(&item_group, 0, sizeof(sbrom_toc1_item_group));
		ret = toc1_item_probe_next(&item_group);
		if(ret < 0)
		{
			pr_error("%s: error in toc1_item_probe_next\n", __func__);
			return -1;
		}
		else if(ret == 0)
		{
			pr_msg("sbromsw_toc1_traverse find out all items\n");

			pr_notice("monitor entry=0x%x\n", monitor_entry);
			pr_notice("uboot entry=0x%x\n", uboot_entry);
			pr_notice("optee entry=0x%x\n", optee_entry);

			monitor_head  = (struct spare_monitor_head *)monitor_entry;
			monitor_head->secureos_base = optee_entry;
#ifdef CONFIG_SUNXI_MULITCORE_BOOT
			bfh = (struct spare_boot_head_t *)uboot_entry;
			bfh->boot_ext[0].data[0] = pmu_type;
			bfh->boot_ext[0].data[1] = uart_input_value;
			bfh->boot_ext[0].data[2] = lradc_input_value;
			bfh->boot_ext[0].data[3] = debug_enable;
#elif defined(CONFIG_SUNXI_MULTI_POWER_MODE)
			bfh = (struct spare_boot_head_t *)uboot_entry;
			bfh->boot_ext[0].data[0] = pmu_type;
#endif
			go_exec(monitor_entry, uboot_entry, optee_entry, dram_size);
			return 0;
		}
		if(item_group.bin_certif)
		{
			memset(buffer, 0, SUNXI_X509_CERTIFF_MAX_LEN);
			len = toc1_item_read(item_group.bin_certif, buffer, SUNXI_X509_CERTIFF_MAX_LEN);
			if(!len)
			{
				pr_error("%s error: cant read content key certif\n", __func__);

				return -1;
			}
			if(sunxi_certif_verify_itself(&sub_certif, buffer, len))
			{
				pr_error("%s error: cant verify the content certif\n", __func__);

				return -1;
			}
			for(i=0;i<root_certif.extension.extension_num;i++)
			{
				if(!strcmp((const char *)root_certif.extension.name[i], item_group.bin_certif->name))
				{
					pr_msg("find %s key stored in root certif\n", item_group.bin_certif->name);

					if(memcmp(root_certif.extension.value[i], sub_certif.pubkey.n+1, sub_certif.pubkey.n_len-1))
					{
						pr_error("%s key n is incompatible\n", item_group.bin_certif->name);
						pr_error(">>>>>>>key in rootcertif<<<<<<<<<<\n");
						ndump((u8 *)root_certif.extension.value[i], sub_certif.pubkey.n_len-1);
						pr_error(">>>>>>>key in certif<<<<<<<<<<\n");
						ndump((u8 *)sub_certif.pubkey.n+1, sub_certif.pubkey.n_len-1);

						return -1;
					}
					if(memcmp(root_certif.extension.value[i] + sub_certif.pubkey.n_len-1, sub_certif.pubkey.e, sub_certif.pubkey.e_len))
					{
						pr_error("%s key e is incompatible\n", item_group.bin_certif->name);
						pr_error(">>>>>>>key in rootcertif<<<<<<<<<<\n");
						ndump((u8 *)root_certif.extension.value[i] + sub_certif.pubkey.n_len-1, sub_certif.pubkey.e_len);
						pr_error(">>>>>>>key in certif<<<<<<<<<<\n");
						ndump((u8 *)sub_certif.pubkey.e, sub_certif.pubkey.e_len);

						return -1;
					}
					break;
				}
			}
			if(i==root_certif.extension.extension_num)
			{
				pr_error("cant find %s key stored in root certif", item_group.bin_certif->name);

				return -1;
			}
		}

		if(item_group.binfile)
		{
			uint addr = 0;
			if(0 == get_item_addr(item_group.binfile->name, &addr))
			{
				if (!strcmp(item_group.binfile->name, ITEM_SCP_NAME))
				{
#ifdef CONFIG_SUNXI_ARISC_EXIST
					sunxi_flash_read(item_group.binfile->data_offset/512, (CONFIG_SYS_SRAMA2_SIZE+511)/512, (void *)SCP_SRAM_BASE);
					sunxi_flash_read((item_group.binfile->data_offset+0x18000)/512, (SCP_DRAM_SIZE+511)/512, (void *)SCP_DRAM_BASE);
					memcpy((void *)(SCP_SRAM_BASE+HEADER_OFFSET+SCP_DRAM_PARA_OFFSET),dram_para_addr,SCP_DARM_PARA_NUM * sizeof(int));
					sunxi_deassert_arisc();
#endif
				}
#ifdef CONFIG_RELOCATE_PARAMETER
				else if(!strcmp(item_group.binfile->name, ITEM_PARAMETER_NAME))
				{
					sunxi_flash_read(item_group.binfile->data_offset/512, (BOOT_PARAMETER_SIZE+511)/512, (void *)CONFIG_SUNXI_PARAMETER_ADDR);
				}
#endif
#ifdef CONFIG_SUNXI_ESM_HDCP
                                else if(!strcmp(item_group.binfile->name, ITEM_ESM_IMG_NAME))
				{
					*(uint *)(SUNXI_ESM_IMG_SIZE_ADDR) = item_group.binfile->data_len;
					sunxi_flash_read(item_group.binfile->data_offset/512, (item_group.binfile->data_len+511)/512, (void *)SUNXI_ESM_IMG_BUFF_ADDR);
				}
#endif
				else
				{
#ifdef CONFIG_SUNXI_MULITCORE_BOOT
					if (strncmp(item_group.binfile->name, ITEM_LOGO_NAME, sizeof(ITEM_LOGO_NAME)) == 0)
					{
						*(uint *)(SUNXI_LOGO_COMPRESSED_LOGO_SIZE_ADDR) = item_group.binfile->data_len;
					}
					else if (strncmp(item_group.binfile->name, ITEM_SHUTDOWNCHARGE_LOGO_NAME, sizeof(ITEM_SHUTDOWNCHARGE_LOGO_NAME)) == 0)
					{
						*(uint *)(SUNXI_SHUTDOWN_CHARGE_COMPRESSED_LOGO_SIZE_ADDR) =item_group.binfile->data_len;
					}
					else if (strncmp(item_group.binfile->name, ITEM_ANDROIDCHARGE_LOGO_NAME, sizeof(ITEM_ANDROIDCHARGE_LOGO_NAME)) == 0)
					{
						*(uint *)(SUNXI_ANDROID_CHARGE_COMPRESSED_LOGO_SIZE_ADDR) = item_group.binfile->data_len;
					}
#endif
					sunxi_flash_read(item_group.binfile->data_offset/512,
						(item_group.binfile->data_len+511)/512,
						(void *)addr);
				}
			}
			else
			{
				//读出bin文件内容到内存
				len = sunxi_flash_read(item_group.binfile->data_offset/512, (item_group.binfile->data_len+511)/512, (void *)item_group.binfile->run_addr);
				if(!len)
				{
					pr_error("%s error: cant read bin file\n", __func__);

					return -1;
				}
				//计算文件hash
				memset(hash_of_file, 0, sizeof(hash_of_file));
				ret = sunxi_sha_calc(hash_of_file, sizeof(hash_of_file), (u8 *)item_group.binfile->run_addr, item_group.binfile->data_len);
				//ret = sunxi_sha_calc(hash_of_file, sizeof(hash_of_file), (u8 *)0x2a000000, item_group.binfile->data_len);
				if(ret)
				{
					pr_error("calc sha256 with hardware err\n");

					return -1;
				}
				//使用内容证书的扩展项，和文件hash进行比较
				//开始比较文件hash(小机端阶段计算得到)和证书hash(PC端计算得到)
				if(memcmp(hash_of_file, sub_certif.extension.value[0], 32))
				{
					pr_error("%s:hash compare is not correct\n",item_group.binfile->name);
					pr_error(">>>>>>>hash of file<<<<<<<<<<\n");
					ndump((u8 *)hash_of_file, 32);
					pr_error(">>>>>>>hash in certif<<<<<<<<<<\n");
					ndump((u8 *)sub_certif.extension.value[0], 32);

					return -1;
				}

				pr_msg("ready to run %s\n", item_group.binfile->name);
				if(!strcmp(item_group.binfile->name, "monitor"))
					monitor_entry = item_group.binfile->run_addr;
				else if(!strcmp(item_group.binfile->name, "u-boot"))
					uboot_entry = item_group.binfile->run_addr;
				else if(!strcmp(item_group.binfile->name, "optee"))
					optee_entry = item_group.binfile->run_addr;
			}
		}
	}
	while(1);

	return 0;
}


#ifdef SUNXI_OTA_TEST
static int sbromsw_print_ota_test(void)
{
	printf("*********************************************\n");
	printf("*********************************************\n");
	printf("*********************************************\n");
	printf("*********************************************\n");
	printf("********[OTA TEST]:update toc0 sucess********\n");
	printf("*********************************************\n");
	printf("*********************************************\n");
	printf("*********************************************\n");
	printf("*********************************************\n");
	return 0;
}
#endif

static int sbromsw_clear_env(void)
{
//	gic_exit();
	reset_pll();
	mmu_turn_off();

	return 0;
}

/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
#define RSA_BIT_WITDH 2048
static int sunxi_certif_pubkey_check( sunxi_key_t  *pubkey )
{
	char efuse_hash[256] , rotpk_hash[256];
	char all_zero[32];

	char pk[RSA_BIT_WITDH/8 * 2 + 256]; /*For the stupid sha padding */

#ifdef CONFIG_SUNXI_KEY_LADDER
	int ret = sunxi_key_ladder_verify(pubkey);
	if (ret != 2) {
		return ret;
	}
#endif
	sid_read_rotpk(efuse_hash);
	memset(all_zero, 0, 32);
	if( ! memcmp(all_zero, efuse_hash,32 ) )
		return 0 ; /*Don't check if rotpk efuse is empty*/
	else{
		memset(pk, 0x91, sizeof(pk));
		char *align = (char *)(((u32)pk+31)&(~31));
		if( *(pubkey->n) ){
			memcpy(align, pubkey->n, pubkey->n_len);
			memcpy(align+pubkey->n_len, pubkey->e, pubkey->e_len);
		}else{
			memcpy(align, pubkey->n+1, pubkey->n_len-1);
			memcpy(align+pubkey->n_len-1, pubkey->e, pubkey->e_len);
		}

		if(sunxi_sha_calc( (u8 *)rotpk_hash, 32, (u8 *)align, RSA_BIT_WITDH/8*2 ))
		{
			printf("sunxi_sha_calc: calc  pubkey sha256 with hardware err\n");
			return -1;
		}

		if(memcmp(rotpk_hash, efuse_hash, 32)){
			pr_error("certif pk dump\n");
			ndump((u8 *)align , RSA_BIT_WITDH/8*2 );

			pr_error("calc certif pk hash dump\n");
			ndump((u8 *)rotpk_hash,32);

			pr_error("efuse pk dump\n");
			ndump((u8 *)efuse_hash,32);

			pr_error("sunxi_certif_pubkey_check: pubkey hash check err\n");
			return -1;
		}
		return 0 ;
	}

}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :  buf: 证书存放起始   len：数据长度
*
*    return        :
*
*    note          :  证书自校验
*
*
************************************************************************************************************
*/
static int sunxi_root_certif_pk_verify(sunxi_certif_info_t *sunxi_certif, u8 *buf, u32 len)
{
	X509 *certif;
	int  ret;

	//内存初始化
	sunxi_certif_mem_reset();
	//创建证书
	ret = sunxi_certif_create(&certif, buf, len);
	if(ret < 0)
	{
		pr_error("fail to create a certif\n");

		return -1;
	}
	//获取证书公钥
	ret = sunxi_certif_probe_pubkey(certif, &sunxi_certif->pubkey);
	if(ret)
	{
		pr_error("fail to probe the public key\n");

		return -1;
	}
	ret = sunxi_certif_pubkey_check(&sunxi_certif->pubkey);
	if(ret){
		pr_error("fail to check the public key hash against efuse\n");

		return -1;
	}

	sunxi_certif_free(certif);

	return 0;
}

