#include "tempsen.h"

void tempsen_init(void)
{
	unsigned int regval;

	/* clear all interrupt status */
	regval = TEMPSEN_GET(sta_tempsen_intr_raw);
	TEMPSEN_SET(sta_tempsen_intr_clr, regval);

	/* clear max result */
	TEMPSEN_SET(clr_tempsen_ch0_max_result, 1);
	TEMPSEN_SET(clr_tempsen_ch1_max_result, 1);

	/* set chop period to 3:1024T */
	TEMPSEN_SET(reg_tempsen_chopsel, 0x3);

	/* set acc period to 2:2048T*/
	TEMPSEN_SET(reg_tempsen_accsel, 0x2);

	/* set tempsen clock divider to 25M/(0x31+1)= 0.5M ,T=2us */
	TEMPSEN_SET(reg_tempsen_cyc_clkdiv, 0x31);

	/* set reg_tempsen_auto_cycle */
	TEMPSEN_SET(reg_tempsen_auto_cycle, 0x100000);

	/* set ddr hi/lo threshold */
	// TEMPSEN_SET(reg_tempsen_ddr_hi_th, 0x400); //85 C
	// TEMPSEN_SET(reg_tempsen_ddr_lo_th, 0x3E4); //75 C

	/* enable ddr auto refresh rate ctrl signal output */
	// TEMPSEN_SET(reg_tempsen_ddr_out_en, 1);

	/* enable tempsen channel */
	TEMPSEN_SET(reg_tempsen_sel, 0x1);
	TEMPSEN_SET(reg_tempsen_en, 1);
}

unsigned int read_temp(void)
{
	unsigned int temp, result;

	while (!(TEMPSEN_GET(sta_tempsen_intr_raw) & CHAN0)) {
		__asm__ volatile("nop");
	}

	temp = TEMPSEN_GET(sta_tempsen_ch0_result);
	result = (temp * 1000) * 716 / 2048 - 273000;
	return result;
}
