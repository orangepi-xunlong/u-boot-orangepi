/*
 * (C) Copyright 2013-2016
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * wangwei <wangwei@allwinnertech.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <power/sunxi/axp.h>

typedef struct _pmu_info
{
	char pmu_name[16];
	int  pmu_id;
}sunxi_pmu_info;

sunxi_pmu_info g_pmu_info[] =\
{
	{"axp22x", (0x68>>1)},
	{"axp81X", 0x11},
	{"axp809", 0x12},
	{"axp2585", 0x13},
	{"axp1506", 0x14},
	{"axp259", 0x36},
	{"axp806", 0x36}
};

int do_pmu_debug(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u8 pmu_reg = 0, pmu_val = 0, pmu_id = 0;
	if(argc < 2)
	{
		goto __error_end;
	}

	pmu_id = simple_strtol(argv[2], NULL, 16);

	if(!strcmp(argv[1], "list"))
	{
		int i = 0;
		printf("%-12s  %-12s \n", "-name-", "-id-");
		for(i = 0; i < ARRAY_SIZE(g_pmu_info); i++)
		{
			printf("%-12s  %-12x \n", g_pmu_info[i].pmu_name, g_pmu_info[i].pmu_id);
		}
	}
	else if(!strcmp(argv[1], "read"))
	{
		if(argc < 4)
		{
			goto __error_end;
		}
		pmu_reg = simple_strtol(argv[3], NULL, 16);
		if(!axp_i2c_read(pmu_id,pmu_reg, &pmu_val))
			printf("pmu reg 0x%x = 0x%x\n", pmu_reg, pmu_val);
		else
			printf("pmu read reg fail\n");
	}
	else if(!strcmp(argv[1], "write"))
	{
		if(argc < 5)
		{
			goto __error_end;
		}
		pmu_reg = simple_strtol(argv[3], NULL, 16);
		pmu_val  = simple_strtol(argv[4], NULL, 16);
		if(!axp_i2c_write(pmu_id,pmu_reg, pmu_val))
			printf("pmu write 0x%x = 0x%x\n", pmu_reg, pmu_val);
		else
			printf("pmu write reg fail\n");
	}
	else
	{
		goto __error_end;
	}
	return 0;
__error_end:
	printf("***invalid para***\n");
	return -1;
}

U_BOOT_CMD(
	pmu, 5, 0, do_pmu_debug,
	"do a pmu test",
	"\n"
	"   list               --show supported pmu\n"
	"   read  id reg       --read  pmu reg\n"
	"   write id reg val   --write pmu reg"
);
