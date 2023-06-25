/*
 * PMIC management for epaper power control HAL
 *
 *      Copyright (C) 2009 Dimitar Dimitrov, MM Solutions
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 *
 */

#ifndef TPS65185_H
#define TPS65185_H

struct pmic_sess;

#define PMIC_DEFAULT_DWELL_TIME_MS	1111
#define PMIC_DEFAULT_VCOMOFF_TIME_MS	20
#define PMIC_DEFAULT_V3P3OFF_TIME_MS	-1

#if defined(FB_OMAP3EP_PAPYRUS_PM_VZERO)
  #define PAPYRUS_STANDBY_DWELL_TIME	4 /*sec*/
#else
  #define PAPYRUS_STANDBY_DWELL_TIME	0
#endif

#define	SEQ_VDD(index)		((index % 4) << 6)
#define SEQ_VPOS(index)		((index % 4) << 4)
#define SEQ_VEE(index)		((index % 4) << 2)
#define SEQ_VNEG(index)		((index % 4) << 0)

/* power up seq delay time */
#define UDLY_3ms(index)		(0x00 << ((index % 4) * 2))
#define UDLY_6ms(index)		(0x01 << ((index % 4) * 2))
#define UDLY_9ms(index)		(0x10 << ((index % 4) * 2))
#define UDLY_12ms(index)	(0x11 << ((index % 4) * 2))

/* power down seq delay time */
#define DDLY_6ms(index)		(0x00 << ((index % 4) * 2))
#define DDLY_12ms(index)	(0x01 << ((index % 4) * 2))
#define DDLY_24ms(index)	(0x10 << ((index % 4) * 2))
#define DDLY_48ms(index)	(0x11 << ((index % 4) * 2))

#define NUMBER_PMIC_REGS	10

struct ebc_pwr_ops {
	int (*power_on)(void);
	int (*power_down)(void);
};

int tps65185_temperature_get(int *temp);
int register_ebc_pwr_ops(struct ebc_pwr_ops *ops);
int tps65185_vcom_set(int vcom_mv);
int tps65185_init(void);
void  tps65185_exit(void);

#endif	/* TPS65185_H */
