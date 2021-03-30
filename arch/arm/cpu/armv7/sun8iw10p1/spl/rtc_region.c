/*
**********************************************************************************************************************
*
*						           the Embedded Secure Bootloader System
*
*
*						       Copyright(C), 2006-2014, Allwinnertech Co., Ltd.
*                                           All Rights Reserved
*
* File    :
*
* By      :
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#include <common.h>
#include <asm/io.h>
#include <asm/arch/platform.h>
#include "rtc_region.h"
static u32 before_crc;
static u32 after_crc;

extern void timer_exit(void);
extern void boot0_jmp_other(unsigned int addr);
extended_standby_t *pextended_standby =
  (extended_standby_t *) (STANDBY_SPACE_BASE_ADDR + EXTENDED_STANDBY_BASE_OFFSET);
pm_dram_para_t *ppm_dram_para;

s32
standby_dram_crc_enable(pm_dram_para_t *pdram_state)
{
  return pdram_state->crc_en;
}

u32
standby_dram_crc(pm_dram_para_t *pdram_state)
{
  u32 *pdata;
  u32 crc = 0;

  pdata = (u32 *) (pdram_state->crc_start);
  printf ("src:0x%x \n", (unsigned int) pdata);
  printf ("len addr = 0x%x, len:0x%x \n",
	  (unsigned int) (&(pdram_state->crc_len)), pdram_state->crc_len);
  while (pdata < (u32 *) (pdram_state->crc_start + pdram_state->crc_len)) {
      crc += *pdata;
      pdata++;
    }
  printf ("crc finish...\n");

  return crc;
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
uint
rtc_region_probe_fel_flag (void)
{
  uint fel_flag, reg_value;
  int i;

  fel_flag = readl (RTC_GENERAL_PURPOSE_REG (2));
  printf ("fel flag  = 0x%x\n", fel_flag);
  for (i = 0; i < 8; i++) {
      reg_value = readl (RTC_GENERAL_PURPOSE_REG (i));
      printf ("rtc[%d] value = 0x%x\n", i, reg_value);
    }

  return fel_flag;
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
void
rtc_region_clear_fel_flag (void)
{
  writel (0, RTC_GENERAL_PURPOSE_REG (2));
}

/* probe standby flag
 * the flag is seted by kernel code when enter standby
 */
static int
probe_super_standby_flag (void)
{
  uint reg_value = 0;
  int standby_flag = 0;
  reg_value = readl (RTC_STANDBY_FLAG_REG);
  standby_flag = (reg_value & ~(0xfffe0000)) >> 16;
  /* printf("RTC_Standby flag is 0x%x ,super_standby flag  = 0x%x\n",reg_value,standby_flag); */
  writel (0, RTC_STANDBY_FLAG_REG);
  return standby_flag;
}

void
handler_super_standby (void)
{
  if (probe_super_standby_flag ()) {
      timer_exit();
      ppm_dram_para =
	(pm_dram_para_t *) (&(pextended_standby->soc_dram_state));
      if (standby_dram_crc_enable (ppm_dram_para)) {
	  before_crc = readl(DRAM_CRC_REG_ADDR);
	  after_crc = standby_dram_crc (ppm_dram_para);
	  if (after_crc != before_crc) {
	      printf ("before_crc = 0x%x, after_crc = 0x%x.\n", before_crc,
		      after_crc);
	      printf ("dram crc error...\n");
	      asm ("b .");
	    }
	}
      dram_enable_all_master();
      printf ("find standby flag,jump to addr 0x%x \n",
	      readl (RTC_STANDBY_SOFT_ENTRY_REG));
      boot0_jmp_other (readl (RTC_STANDBY_SOFT_ENTRY_REG));
    }
}
