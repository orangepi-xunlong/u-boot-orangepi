/*
 * Papyrus epaper power control HAL
 *
 *      Copyright (C) 2009 Dimitar Dimitrov, MM Solutions
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License. See the file COPYING in the main directory of this archive for
 *  more details.
 *
 * TPS6518x power control is facilitated using I2C control and WAKEUP GPIO
 * pin control. The other VCC GPIO Papyrus' signals must be tied to ground.
 *
 * TODO:
 *	- Instead of polling, use interrupts to signal power up/down
 *	  acknowledge.
 */

#include <common.h>
#include <i2c.h>
#include <malloc.h>
#include <power/tps65185.h>
#include <linux/printk.h>

#define PAPYRUS2_1P1_I2C_ADDRESS		0x68
#define msleep(a)	udelay(a * 1000)

/***********记得要配********************
CONFIG_I2C0_ENABLE=y
CONFIG_SYS_SUNXI_I2C0_SPEED=400000
CONFIG_SYS_SUNXI_I2C0_SLAVE=0x68
***************************************/

extern void papyrus_set_i2c_address(int address);

int old_twi_id;

#define TPSDEBUG(n, args...)				\
	do {						\
		if (n == 1)					\
		pr_debug(args);		\
		else if (n == 2) \
		pr_err(args);		\
	} while (0)

#define _debug(args...)	TPSDEBUG(1, args)
#define _info(args...)	TPSDEBUG(1, args)
#define _warn(args...)	TPSDEBUG(2, args)
#define _err(args...)	TPSDEBUG(2, args)

typedef struct {
	char  gpio_name[32];
	int port;
	int port_num;
	int mul_sel;
	int pull;
	int drv_level;
	int data;
	int gpio;
} eink_gpio_set_t;

extern int eink_sys_gpio_request(eink_gpio_set_t *gpio_list, u32 group_count_max);
extern int eink_sys_gpio_set_direction(u32 p_handler, u32 direction, const char *gpio_name);
extern int eink_sys_gpio_set_value(u32 p_handler, u32 value_to_gpio, const char *gpio_name);
extern int eink_sys_gpio_release(int p_handler, s32 if_release_to_default_status);
extern int eink_sys_script_get_item(char *main_name, char *sub_name, int value[], int type);
extern int eink_sys_power_enable(char *name);

struct pmic_driver {
	const char *id;

	int vcom_min;
	int vcom_max;
	int vcom_step;

	int (*hw_read_temperature)(struct pmic_sess *sess, int *t);
	bool (*hw_power_ack)(struct pmic_sess *sess);
	void (*hw_power_req)(struct pmic_sess *sess, bool up);

	int (*set_enable)(struct pmic_sess *sess, int enable);
	int (*set_vcom_voltage)(struct pmic_sess *sess, int vcom_mv);
	int (*set_vcom1)(struct pmic_sess *sess, uint8_t vcom1);
	int (*set_vcom2)(struct pmic_sess *sess, uint8_t vcom2);
	int (*set_vadj)(struct pmic_sess *sess, uint8_t vadj);
	int (*set_int_en1)(struct pmic_sess *sess, uint8_t int_en1);
	int (*set_int_en2)(struct pmic_sess *sess, uint8_t int_en2);
	int (*set_upseq0)(struct pmic_sess *sess, uint8_t upseq0);
	int (*set_upseq1)(struct pmic_sess *sess, uint8_t upseq1);
	int (*set_dwnseq0)(struct pmic_sess *sess, uint8_t dwnseq0);
	int (*set_dwnseq1)(struct pmic_sess *sess, uint8_t dwnseq1);
	int (*set_tmst1)(struct pmic_sess *sess, uint8_t tmst1);
	int (*set_tmst2)(struct pmic_sess *sess, uint8_t tmst2);

	int (*set_vp_adjust)(struct pmic_sess *sess, uint8_t vp_adjust);
	int (*set_vn_adjust)(struct pmic_sess *sess, uint8_t vn_adjust);
	int (*set_vcom_adjust)(struct pmic_sess *sess, uint8_t vcom_adjust);
	int (*set_pwr_seq0)(struct pmic_sess *sess, uint8_t pwr_seq0);
	int (*set_pwr_seq1)(struct pmic_sess *sess, uint8_t pwr_seq1);
	int (*set_pwr_seq2)(struct pmic_sess *sess, uint8_t pwr_seq2);
	int (*set_tmst_config)(struct pmic_sess *sess, uint8_t tmst_config);
	int (*set_tmst_os)(struct pmic_sess *sess, uint8_t tmst_os);
	int (*set_tmst_hyst)(struct pmic_sess *sess, uint8_t tmst_hyst);

	int (*hw_vcom_switch)(struct pmic_sess *sess, bool state);

	int (*hw_set_dvcom)(struct pmic_sess *sess, int state);

	void (*hw_cleanup)(struct pmic_sess *sess);

	bool (*hw_standby_dwell_time_ready)(struct pmic_sess *sess);
	int (*hw_pm_sleep)(struct pmic_sess *sess);
	int (*hw_pm_resume)(struct pmic_sess *sess);

};

typedef enum {
	MASTER_PMIC_ID = 0,
	SLAVE_PMIC_ID = 1,
	MAX_PMIC_NUM = 2
} PMIC_ID;

struct pmic_sess {
	PMIC_ID id;
	bool powered;
	//struct delayed_work powerdown_work;
	unsigned int dwell_time_ms;
	//struct delayed_work vcomoff_work;
	unsigned int vcomoff_time_ms;
	int v3p3off_time_ms;
	const struct pmic_driver *drv;
	void *drvpar;
	int temp_man_offset;
	int revision;
	int is_inited;
};

struct pmic_config {
	u32 used_flag;
	char name[64];
	u32 twi_id;
	u32 twi_addr;
	eink_gpio_set_t wake_up_pin;
	eink_gpio_set_t vcom_ctl_pin;
	eink_gpio_set_t power_up_pin;
	eink_gpio_set_t power_good_pin;
	eink_gpio_set_t int_pin;
	char pmic_power_name[64];
};

struct pmic_mgr {
	struct pmic_config config[MAX_PMIC_NUM];
	struct pmic_sess sess[MAX_PMIC_NUM];
};

struct pmic_mgr *g_pmic_mgr;

#define TPS65185_I2C_NAME "tps65185"
#define TPS65185_SLAVE_I2C_NAME "tps65185_slave"

#define PAPYRUS_VCOM_MAX_MV		0
#define PAPYRUS_VCOM_MIN_MV		-5110

#define CONTRUL_POWER_UP_PING (1)
#define PAPYRUS_POWER_UP_DELAY_MS   2

/* After waking up from sleep, Papyrus
   waits for VN to be discharged and all
   voltage ref to startup before loading
   the default EEPROM settings. So accessing
   registers too early after WAKEUP could
   cause the register to be overridden by
   default values */
#define PAPYRUS_EEPROM_DELAY_MS 50
/* Papyrus WAKEUP pin must stay low for
   a minimum time */
#define PAPYRUS_SLEEP_MINIMUM_MS 110
/* Temp sensor might take a little time to
   settle eventhough the status bit in TMST1
   state conversion is done - if read too early
   0C will be returned instead of the right temp */
#define PAPYRUS_TEMP_READ_TIME_MS 10

/* Powerup sequence takes at least 24 ms - no need to poll too frequently */
#define HW_GET_STATE_INTERVAL_MS 24

#define INVALID_GPIO -1

struct papyrus_sess {
	u8 bus_id;
	u8 i2c_addr;
	u8 enable_reg_shadow;
	u8 enable_reg;
	u8 vadj;
	u8 vcom1;
	u8 vcom2;
	u8 vcom2off;
	u8 int_en1;
	u8 int_en2;
	u8 upseq0;
	u8 upseq1;
	u8 dwnseq0;
	u8 dwnseq1;
	u8 tmst1;
	u8 tmst2;

	/* Custom power up/down sequence settings */
	struct {
		/* If options are not valid we will rely on HW defaults. */
		bool valid;
		unsigned int dly[8];
	} seq;

	unsigned int v3p3off_time_ms;
	int wake_up_pin;
	int vcom_ctl_pin;
	int power_up_pin;
	/* True if a high WAKEUP brings Papyrus out of reset. */
	int wakeup_active_high;
	int vcomctl_active_high;
	int power_active_heght;
	//struct regulator *pmic_power_ldo;
};

#define tps65185_SPEED	(400 * 1000)

#define PAPYRUS_ADDR_TMST_VALUE		0x00
#define PAPYRUS_ADDR_ENABLE		0x01
#define PAPYRUS_ADDR_VADJ		0x02
#define PAPYRUS_ADDR_VCOM1_ADJUST	0x03
#define PAPYRUS_ADDR_VCOM2_ADJUST	0x04
#define PAPYRUS_ADDR_INT_ENABLE1	0x05
#define PAPYRUS_ADDR_INT_ENABLE2	0x06
#define PAPYRUS_ADDR_INT_STATUS1	0x07
#define PAPYRUS_ADDR_INT_STATUS2	0x08
#define PAPYRUS_ADDR_UPSEQ0		0x09
#define PAPYRUS_ADDR_UPSEQ1		0x0a
#define PAPYRUS_ADDR_DWNSEQ0		0x0b
#define PAPYRUS_ADDR_DWNSEQ1		0x0c
#define PAPYRUS_ADDR_TMST1		0x0d
#define PAPYRUS_ADDR_TMST2		0x0e
#define PAPYRUS_ADDR_PG_STATUS		0x0f
#define PAPYRUS_ADDR_REVID		0x10

// INT_ENABLE1
#define PAPYRUS_INT_ENABLE1_ACQC_EN	1
#define PAPYRUS_INT_ENABLE1_PRGC_EN 0

// INT_STATUS1
#define PAPYRUS_INT_STATUS1_ACQC	1
#define PAPYRUS_INT_STATUS1_PRGC	0

// VCOM2_ADJUST
#define PAPYRUS_VCOM2_ACQ	7
#define PAPYRUS_VCOM2_PROG	6
#define PAPYRUS_VCOM2_HIZ	5

#define PAPYRUS_MV_TO_VCOMREG(MV)	((MV) / 10)

#define V3P3_EN_MASK	0x20
#define PAPYRUS_V3P3OFF_DELAY_MS 10//100

struct papyrus_hw_state {
	u8 tmst_value;
	u8 int_status1;
	u8 int_status2;
	u8 pg_status;
};

static int papyrus_resume_flag;

int pmic_i2c_read_bus(int bus_num, uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	int ret = 0;

	ret = i2c_set_bus_num(bus_num);
	if (ret) {
		_err("%s: set i2c bus(%d) fail\n", __func__, ret);
		return -1;
	}

	ret = i2c_read(chip, addr, alen, buffer, len);
	if (ret) {
		_err("read i2c%d fail, chip=0x%x, addr=0x%x, reg=0x%x, len=%d\n",
		     bus_num, (int)chip, addr, alen, len);
		return -2;
	}
	return ret;
}

int pmic_i2c_write_bus(int bus_num, uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	int ret = 0;

	ret = i2c_set_bus_num(bus_num);
	if (ret) {
		_err("%s: set i2c bus(%d) fail\n", __func__, ret);
		return -1;
	}

	ret = i2c_write(chip, addr, alen, buffer, len);
	if (ret) {
		_err("write i2c%d fail, chip=0x%x, addr=0x%x, reg=0x%x, len=%d\n",
		     bus_num, (int)chip, addr, alen, len);
		return -2;
	}

	return ret;
}

static int papyrus_hw_setreg(struct papyrus_sess *sess, uint8_t regaddr, uint8_t val)
{
	int stat;

	if (!sess) {
		_err("%s: input para is null\n", __func__);
		return -1;
	}

	stat = pmic_i2c_write_bus(sess->bus_id, sess->i2c_addr, regaddr, 1, &val, 1);

	return stat;
}

static int papyrus_hw_getreg(struct papyrus_sess *sess, uint8_t regaddr, uint8_t *val)
{
	int stat;

	if (!sess) {
		_err("%s: input para is null\n", __func__);
		return -1;
	}

	stat = pmic_i2c_read_bus(sess->bus_id, sess->i2c_addr, regaddr, 1, val, 1);

	return stat;
}

static void papyrus_hw_get_pg(struct papyrus_sess *sess,
			      struct papyrus_hw_state *hwst)
{
	int stat;

	stat = papyrus_hw_getreg(sess,
				 PAPYRUS_ADDR_PG_STATUS, &hwst->pg_status);
	if (stat)
		_err("papyrus: I2C error: %d\n", stat);
}

/*
   static void papyrus_hw_get_state(struct papyrus_sess *sess, struct papyrus_hw_state *hwst)
   {
   int stat;

   stat = papyrus_hw_getreg(sess,
   PAPYRUS_ADDR_TMST_VALUE, &hwst->tmst_value);
   stat |= papyrus_hw_getreg(sess,
   PAPYRUS_ADDR_INT_STATUS1, &hwst->int_status1);
   stat |= papyrus_hw_getreg(sess,
   PAPYRUS_ADDR_INT_STATUS2, &hwst->int_status2);
   stat |= papyrus_hw_getreg(sess,
   PAPYRUS_ADDR_PG_STATUS, &hwst->pg_status);
   if (stat)
   _err("papyrus: I2C error: %d\n", stat);
   }
   */

static void papyrus_hw_send_powerup(struct papyrus_sess *sess)
{
	int stat = 0;

	// set VADJ
	stat |= papyrus_hw_setreg(sess, PAPYRUS_ADDR_VADJ, sess->vadj);

	// set UPSEQs & DWNSEQs
	stat |= papyrus_hw_setreg(sess, PAPYRUS_ADDR_UPSEQ0, sess->upseq0);
	stat |= papyrus_hw_setreg(sess, PAPYRUS_ADDR_UPSEQ1, sess->upseq1);
	stat |= papyrus_hw_setreg(sess, PAPYRUS_ADDR_DWNSEQ0, sess->dwnseq0);
	stat |= papyrus_hw_setreg(sess, PAPYRUS_ADDR_DWNSEQ1, sess->dwnseq1);

	// commit it, so that we can adjust vcom through "Rk_ebc_power_control_Release_v1.1"
	//stat |= papyrus_hw_setreg(sess, PAPYRUS_ADDR_VCOM1_ADJUST, sess->vcom1);
	//stat |= papyrus_hw_setreg(sess, PAPYRUS_ADDR_VCOM2_ADJUST, sess->vcom2);

#if 0
	/* Enable 3.3V switch to the panel */
	sess->enable_reg_shadow |= V3P3_EN_MASK;
	stat |= papyrus_hw_setreg(sess, PAPYRUS_ADDR_ENABLE, sess->enable_reg_shadow);
	msleep(sess->v3p3off_time_ms);
#endif

	/* switch to active mode, keep 3.3V & VEE & VDDH & VPOS & VNEG alive,
	 * don't enable vcom buffer
	 */
	sess->enable_reg_shadow = (0x80 | 0x20 | 0x0F);
	stat |= papyrus_hw_setreg(sess, PAPYRUS_ADDR_ENABLE, sess->enable_reg_shadow);
	if (stat)
		_err("papyrus: I2C error: %d\n", stat);
	return;
}

static void papyrus_hw_send_powerdown(struct papyrus_sess *sess)
{
	/* switch to standby mode, keep 3.3V & VEE & VDDH & VPOS & VNEG alive,
	 * don't enable vcom buffer
	 */
	sess->enable_reg_shadow = (0x40 | 0x20 | 0x0F);
	papyrus_hw_setreg(sess, PAPYRUS_ADDR_ENABLE, sess->enable_reg_shadow);

#if 0
	/* 3.3V switch must be turned off last */
	msleep(sess->v3p3off_time_ms);
	sess->enable_reg_shadow &= ~V3P3_EN_MASK;
	stat |= papyrus_hw_setreg(sess, PAPYRUS_ADDR_ENABLE, sess->enable_reg_shadow);
	if (stat)
		_err("papyrus: I2C error: %d\n", stat);
#endif

	return;
}

static int papyrus_hw_read_temperature(struct pmic_sess *pmsess, int *t)
{
	struct papyrus_sess *sess = (struct papyrus_sess *)pmsess->drvpar;
	int stat;
	int ntries = 50;
	u8 tb;

	if (pmsess->id != MASTER_PMIC_ID) {
		*t = -1;
		_err("only master pmic have NTC\n");
		return -1;
	}

	stat = papyrus_hw_setreg(sess, PAPYRUS_ADDR_TMST1, 0x80);
	do {
		stat = papyrus_hw_getreg(sess,
					 PAPYRUS_ADDR_TMST1, &tb);
	} while (!stat && ntries-- && (((tb & 0x20) == 0) || (tb & 0x80)));

	if (stat) {
		*t = -1;  //communication fail, return -1 degree
		_err("papyrus_hw_read_temperature fail *t = %d stat = %d\n", *t, stat);
		return stat;
	}

	msleep(PAPYRUS_TEMP_READ_TIME_MS);
	stat = papyrus_hw_getreg(sess, PAPYRUS_ADDR_TMST_VALUE, &tb);
	*t = (int)(int8_t)tb;

	return stat;
}

static int papyrus_hw_get_revid(struct papyrus_sess *sess)
{
	int stat;
	u8 revid;

	stat = papyrus_hw_getreg(sess, PAPYRUS_ADDR_REVID, &revid);
	if (stat) {
		_err("papyrus: I2C error: %d\n", stat);
		return stat;
	} else
		return revid;
}

static int papyrus_hw_arg_init(struct papyrus_sess *sess)
{
	sess->vadj = 0x03;

	sess->upseq0 = SEQ_VEE(0) | SEQ_VNEG(1) | SEQ_VPOS(2) | SEQ_VDD(3);
	sess->upseq1 = UDLY_3ms(0) | UDLY_3ms(0) | UDLY_3ms(0) | UDLY_3ms(0);

	sess->dwnseq0 = SEQ_VDD(0) | SEQ_VPOS(1) | SEQ_VNEG(2) | SEQ_VEE(3);
	sess->dwnseq1 = DDLY_6ms(0) | DDLY_6ms(0) | DDLY_6ms(0) | DDLY_6ms(0);

	sess->vcom1 = (PAPYRUS_MV_TO_VCOMREG(2500) & 0x00FF);
	sess->vcom2 = ((PAPYRUS_MV_TO_VCOMREG(2500) & 0x0100) >> 8);

	return 0;
}

static int papyrus_hw_init(struct papyrus_sess *sess, PMIC_ID pmic_id)
{
	if (pmic_id != MASTER_PMIC_ID) {
		_info("not master pmic, do not control gpio\n");
		return 0;
	}
	eink_sys_gpio_set_direction(sess->wake_up_pin, 1, "tps65185_wakeup");
	eink_sys_gpio_set_value(sess->wake_up_pin, sess->wakeup_active_high, "tps65185_wakeup");
	msleep(PAPYRUS_POWER_UP_DELAY_MS);

#if (CONTRUL_POWER_UP_PING)
	//make sure power up is low

	eink_sys_gpio_set_direction(sess->power_up_pin, 1, "tps65185_powerup");
	eink_sys_gpio_set_value(sess->power_up_pin, !sess->power_active_heght, "tps65185_powerup");
	//eink_sys_gpio_set_value(sess->power_up_pin, sess->power_active_heght, "tps65185_powerup");
#endif

	eink_sys_gpio_set_direction(sess->vcom_ctl_pin, 1, "tps65185_vcom");
	eink_sys_gpio_set_value(sess->vcom_ctl_pin, sess->vcomctl_active_high, "tps65185_vcom");
	msleep(PAPYRUS_EEPROM_DELAY_MS);

	return 0;
}

static void papyrus_hw_power_req(struct pmic_sess *pmsess, bool up)
{
	struct papyrus_sess *sess = pmsess->drvpar;

	pr_debug("papyrus: i2c pwr req: %d\n", up);
	if (up) {
		papyrus_hw_send_powerup(sess);
	} else {
		papyrus_hw_send_powerdown(sess);
	}
	return;
}

static bool papyrus_hw_power_ack(struct pmic_sess *pmsess)
{
	struct papyrus_sess *sess = pmsess->drvpar;
	struct papyrus_hw_state hwst;
	int st;
	int retries_left = 10;

	do {
		papyrus_hw_get_pg(sess, &hwst);

		_debug("hwst: tmst_val=%d, ist1=%02x, ist2=%02x, pg=%02x\n",
		       hwst.tmst_value, hwst.int_status1,
				hwst.int_status2, hwst.pg_status);
		hwst.pg_status &= 0xfa;
		if (hwst.pg_status == 0xfa)
			st = 1;
		else if (hwst.pg_status == 0x00)
			st = 0;
		else {
			st = -1;	/* not settled yet */
			msleep(HW_GET_STATE_INTERVAL_MS);
		}
		retries_left--;
	} while ((st == -1) && retries_left);

	if ((st == -1) && !retries_left)
		_err("papyrus: power up/down settle error (PG = %02x)\n", hwst.pg_status);

	return !!st;
}

static void papyrus_hw_cleanup(struct papyrus_sess *sess)
{
	eink_sys_gpio_release(sess->wake_up_pin, 1);
	eink_sys_gpio_release(sess->vcom_ctl_pin, 1);
#if (CONTRUL_POWER_UP_PING)
	eink_sys_gpio_release(sess->power_up_pin, 1);
#endif
}

/* -------------------------------------------------------------------------*/

static int papyrus_set_enable(struct pmic_sess *pmsess, int enable)
{
	struct papyrus_sess *sess = pmsess->drvpar;

	sess->enable_reg = enable;
	return 0;
}

static int papyrus_set_vcom_voltage(struct pmic_sess *pmsess, int vcom_mv)
{
	struct papyrus_sess *sess = pmsess->drvpar;

	sess->vcom1 = (PAPYRUS_MV_TO_VCOMREG(-vcom_mv) & 0x00FF);
	sess->vcom2 = ((PAPYRUS_MV_TO_VCOMREG(-vcom_mv) & 0x0100) >> 8);
	return 0;
}

static int papyrus_set_vcom1(struct pmic_sess *pmsess, uint8_t vcom1)
{
	struct papyrus_sess *sess = pmsess->drvpar;

	sess->vcom1 = vcom1;
	return 0;
}

static int papyrus_set_vcom2(struct pmic_sess *pmsess, uint8_t vcom2)
{
	struct papyrus_sess *sess = pmsess->drvpar;
	// TODO; Remove this temporary solution to set custom vcom-off mode
	//       Add PMIC setting when this is to be a permanent feature
	_debug("papyrus_set_vcom2 vcom2off 0x%02x\n", vcom2);
	sess->vcom2off = vcom2;
	return 0;
}

static int papyrus_set_vadj(struct pmic_sess *pmsess, uint8_t vadj)
{
	struct papyrus_sess *sess = pmsess->drvpar;

	sess->vadj = vadj;
	return 0;
}

static int papyrus_set_int_en1(struct pmic_sess *pmsess, uint8_t int_en1)
{
	struct papyrus_sess *sess = pmsess->drvpar;

	sess->int_en1 = int_en1;
	return 0;
}

static int papyrus_set_int_en2(struct pmic_sess *pmsess, uint8_t int_en2)
{
	struct papyrus_sess *sess = pmsess->drvpar;

	sess->int_en2 = int_en2;
	return 0;
}

static int papyrus_set_upseq0(struct pmic_sess *pmsess, uint8_t upseq0)
{
	struct papyrus_sess *sess = pmsess->drvpar;

	sess->upseq0 = upseq0;
	return 0;
}

static int papyrus_set_upseq1(struct pmic_sess *pmsess, uint8_t upseq1)
{
	struct papyrus_sess *sess = pmsess->drvpar;

	sess->upseq1 = upseq1;
	return 0;
}

static int papyrus_set_dwnseq0(struct pmic_sess *pmsess, uint8_t dwnseq0)
{
	struct papyrus_sess *sess = pmsess->drvpar;

	sess->dwnseq0 = dwnseq0;
	return 0;
}

static int papyrus_set_dwnseq1(struct pmic_sess *pmsess, uint8_t dwnseq1)
{
	struct papyrus_sess *sess = pmsess->drvpar;

	sess->dwnseq1 = dwnseq1;
	return 0;
}

static int papyrus_set_tmst1(struct pmic_sess *pmsess, uint8_t tmst1)
{
	struct papyrus_sess *sess = pmsess->drvpar;

	sess->tmst1 = tmst1;
	return 0;
}

static int papyrus_set_tmst2(struct pmic_sess *pmsess, uint8_t tmst2)
{
	struct papyrus_sess *sess = pmsess->drvpar;

	sess->tmst2 = tmst2;
	return 0;
}

static int papyrus_vcom_switch(struct pmic_sess *pmsess, bool state)
{
	struct papyrus_sess *sess = pmsess->drvpar;
	int stat;

	sess->enable_reg_shadow &= ~((1u << 4) | (1u << 6) | (1u << 7));
	sess->enable_reg_shadow |= (state ? 1u : 0) << 4;

	stat = papyrus_hw_setreg(sess, PAPYRUS_ADDR_ENABLE,
				 sess->enable_reg_shadow);

	/* set VCOM off output */
	if (!state && sess->vcom2off != 0) {
		stat = papyrus_hw_setreg(sess, PAPYRUS_ADDR_VCOM2_ADJUST,
					 sess->vcom2off);
	}

	return stat;
}

static bool papyrus_standby_dwell_time_ready(struct pmic_sess *pmsess)
{
	return true;
}

static int papyrus_pm_sleep(struct pmic_sess *sess)
{
	struct papyrus_sess *s = sess->drvpar;

	_info("PMIC%d: enter to sleep\n", sess->id);

	if (sess->id == MASTER_PMIC_ID) {
		eink_sys_gpio_set_direction(s->vcom_ctl_pin, 1, "tps65185_vcom");
		eink_sys_gpio_set_value(s->vcom_ctl_pin, !s->vcomctl_active_high, "tps65185_vcom");

		eink_sys_gpio_set_direction(s->wake_up_pin, 1, "tps65185_wakeup");
		eink_sys_gpio_set_value(s->wake_up_pin, !s->wakeup_active_high, "tps65185_wakeup");
		papyrus_resume_flag = 0;
	}

	return 0;
}

static int papyrus_pm_resume(struct pmic_sess *sess)
{
	struct papyrus_sess *s = sess->drvpar;

	_info("PMIC%d: resume from sleep\n", sess->id);

	if (sess->id == MASTER_PMIC_ID) {
		eink_sys_gpio_set_direction(s->wake_up_pin, 1, "tps65185_wakeup");
		eink_sys_gpio_set_value(s->wake_up_pin, s->wakeup_active_high, "tps65185_wakeup");

		eink_sys_gpio_set_direction(s->vcom_ctl_pin, 1, "tps65185_vcom");
		eink_sys_gpio_set_value(s->vcom_ctl_pin, s->vcomctl_active_high, "tps65185_vcom");
		papyrus_resume_flag = 1;
	}

	return 0;
}

static int papyrus_probe(struct pmic_sess *pmsess, struct pmic_config *config)
{
	struct papyrus_sess *sess = NULL;
	struct papyrus_sess *tmp_sess = NULL;
	struct pmic_sess *tmp_pmsess = NULL;
	PMIC_ID id = 0;
	int stat;

	if ((!pmsess) || (!config)) {
		_err("PMIC%d: input param is null\n", id);
		return -1;
	}

	sess = malloc(sizeof(*sess));
	if (!sess)
		return -2;

	sess->bus_id = config->twi_id;
	sess->i2c_addr = config->twi_addr;

	if (config->wake_up_pin.gpio != INVALID_GPIO) {
		sess->wake_up_pin = eink_sys_gpio_request(&config->wake_up_pin, 1);
	}

	if (config->vcom_ctl_pin.gpio != INVALID_GPIO) {
		sess->vcom_ctl_pin = eink_sys_gpio_request(&config->vcom_ctl_pin, 1);
	}

	if (config->power_up_pin.gpio != INVALID_GPIO) {
		sess->power_up_pin = eink_sys_gpio_request(&config->power_up_pin, 1);
	}

	sess->wakeup_active_high = 1;
	sess->vcomctl_active_high = 1;
	sess->power_active_heght = 1;

	sess->enable_reg_shadow = 0;
	eink_sys_power_enable(config->pmic_power_name);

	if (pmsess->v3p3off_time_ms == -1)
		sess->v3p3off_time_ms = PAPYRUS_V3P3OFF_DELAY_MS;
	else
		sess->v3p3off_time_ms = pmsess->v3p3off_time_ms;

	papyrus_hw_arg_init(sess);

	pmsess->drvpar = sess;

	if (pmsess->id == MASTER_PMIC_ID) {
		stat = papyrus_hw_init(sess, pmsess->id);
		if (stat)
			goto free_sess;

		for (id = 0; id < MAX_PMIC_NUM; id++) {
			if (g_pmic_mgr->config[id].used_flag != 1) {
				continue;
			}

			tmp_pmsess = &g_pmic_mgr->sess[id];
			tmp_sess = (struct papyrus_sess *)tmp_pmsess->drvpar;

			stat = papyrus_hw_setreg(tmp_sess, PAPYRUS_ADDR_ENABLE, tmp_sess->enable_reg_shadow);
			if (stat) {
				_err("%s: fail to set addr enable\n", __func__);
				goto free_sess;
			}

			tmp_pmsess->revision = papyrus_hw_get_revid(tmp_sess);
			_info("PMIC%d: get revision = 0x%08x\n", id, tmp_pmsess->revision);
		}
	}

	return 0;

free_sess:
	if (sess)
		free(sess);
	return stat;
}

static void papyrus_remove(struct pmic_sess *pmsess)
{
	struct papyrus_sess *sess = pmsess->drvpar;

	if (pmsess->id == MASTER_PMIC_ID)
		papyrus_hw_cleanup(sess);

	if (sess)
		free(sess);
	pmsess->drvpar = 0;
}

const struct pmic_driver pmic_driver_tps65185_i2c = {
	.id = "tps65185-i2c",

	.vcom_min = PAPYRUS_VCOM_MIN_MV,
	.vcom_max = PAPYRUS_VCOM_MAX_MV,
	.vcom_step = 10,

	.hw_read_temperature = papyrus_hw_read_temperature,
	.hw_power_ack = papyrus_hw_power_ack,
	.hw_power_req = papyrus_hw_power_req,

	.set_enable = papyrus_set_enable,
	.set_vcom_voltage = papyrus_set_vcom_voltage,
	.set_vcom1 = papyrus_set_vcom1,
	.set_vcom2 = papyrus_set_vcom2,
	.set_vadj = papyrus_set_vadj,
	.set_int_en1 = papyrus_set_int_en1,
	.set_int_en2 = papyrus_set_int_en2,
	.set_upseq0 = papyrus_set_upseq0,
	.set_upseq1 = papyrus_set_upseq1,
	.set_dwnseq0 = papyrus_set_dwnseq0,
	.set_dwnseq1 = papyrus_set_dwnseq1,
	.set_tmst1 = papyrus_set_tmst1,
	.set_tmst2 = papyrus_set_tmst2,

	.hw_vcom_switch = papyrus_vcom_switch,

	.hw_cleanup = papyrus_remove,

	.hw_standby_dwell_time_ready = papyrus_standby_dwell_time_ready,
	.hw_pm_sleep = papyrus_pm_sleep,
	.hw_pm_resume = papyrus_pm_resume,
};

static int tps65185_probe(void)
{
	int ret = -1;
	PMIC_ID pmic_id = MASTER_PMIC_ID;
	struct pmic_sess *sess;
	struct pmic_config *config;

	sess = &g_pmic_mgr->sess[pmic_id];
	config = &g_pmic_mgr->config[pmic_id];

	/* must config the i2c enable */
	old_twi_id = i2c_get_bus_num();
	ret = i2c_set_bus_num(config->twi_id);
	if (ret) {
		_err("[%s]: set i2c bus(%d) fail\n", __func__, ret);
		return -1;
	}

	ret = papyrus_probe(sess, config);
	if (ret != 0) {
		_err("hw_init master pmic failed.");
		return -1;
	}

	papyrus_resume_flag = 1;
	sess->is_inited = 1;

	_info("hw_init master pmic ok\n");

	return 0;
}

static int tps65185_slave_probe(void)
{
	int ret = -1;
	PMIC_ID pmic_id = SLAVE_PMIC_ID;
	struct pmic_sess *sess;
	struct pmic_config *config;

	sess = &g_pmic_mgr->sess[pmic_id];
	config = &g_pmic_mgr->config[pmic_id];

	/* must config the i2c enable */
	ret = i2c_set_bus_num(config->twi_id);
	if (ret) {
		_err("[%s]: set i2c bus(%d) fail\n", __func__, ret);
		return -1;
	}

	ret = papyrus_probe(sess, config);
	if (ret != 0) {
		_err("hw_init slave pmic failed.");
		return -1;
	}

	papyrus_resume_flag = 1;
	sess->is_inited = 1;

	_info("hw_init slave pmic ok\n");

	return 0;
}

static int tps65185_remove(void)
{
	struct pmic_sess *pmic_sess = &g_pmic_mgr->sess[MASTER_PMIC_ID];
	struct papyrus_sess *sess = NULL;

	sess = (struct papyrus_sess *)pmic_sess->drvpar;
	pmic_driver_tps65185_i2c.hw_cleanup(pmic_sess);

	if (sess)
		free(sess);

	pmic_sess->drvpar = NULL;

	return 0;
}

static int tps65185_slave_remove(void)
{
	struct pmic_sess *pmic_sess = &g_pmic_mgr->sess[SLAVE_PMIC_ID];
	struct papyrus_sess *sess = NULL;

	if (g_pmic_mgr->config[SLAVE_PMIC_ID].used_flag != 1) {
		return 0;
	}

	sess = (struct papyrus_sess *)pmic_sess->drvpar;
	pmic_driver_tps65185_i2c.hw_cleanup(pmic_sess);

	if (sess)
		free(sess);

	pmic_sess->drvpar = NULL;

	return 0;
}

static int tps65185_get_config(struct pmic_config *config, int i)
{
	char *pmic_node_array[MAX_PMIC_NUM] = {"tps65185", "tps65185_slave"};
	s32 value = 0;
	int ret = -1;
	eink_gpio_set_t  *gpio_info;

	if ((!config) || (i >= MAX_PMIC_NUM)) {
		_err("%s: input param is wrong\n", __func__);
		return -1;
	}

	ret = eink_sys_script_get_item(pmic_node_array[i], "tps65185_used", &value, 1);
	if (ret != 1)
		_err("PMIC%d: get tps65185_used fail\n", i);
	else {
		config->used_flag = value;
		pr_debug("PMIC%d: tps65185_used = %d\n", i, config->used_flag);
		if (config->used_flag == 0)
			return 0;
	}

	memset(config->name, 0, sizeof(config->name));
	ret = eink_sys_script_get_item(pmic_node_array[i], "tps65185_name",
				       (int *)config->name, 2);
	if (ret != 2)
		_err("PMIC%d: get tps65185_name fail\n", i);
	else
		pr_debug("PMIC%d: tps65185_name = %s\n", i, config->name);

	ret = eink_sys_script_get_item(pmic_node_array[i], "tps65185_twi_id", &value, 1);
	if (ret != 1)
		_err("PMIC%d: get tps65185_twi_id fail\n", i);
	else {
		config->twi_id = value;
		pr_debug("PMIC%d: tps65185_twi_id = %d\n", i, config->twi_id);
	}

	ret = eink_sys_script_get_item(pmic_node_array[i], "tps65185_twi_addr", &value, 1);
	if (ret != 1)
		_err("PMIC%d: get tps65185_twi_addr fail\n", i);
	else {
		config->twi_addr = value;
		pr_debug("PMIC%d: tps65185_twi_addr = 0x%x\n", i, config->twi_addr);
	}

	gpio_info = &config->wake_up_pin;
	ret = eink_sys_script_get_item(pmic_node_array[i], "tps65185_wakeup",
				       (int *)gpio_info, 3);
	if (ret != 3)
		_err("PMIC%d: tps65185_wakeup is invalid.\n", i);
	else
		pr_debug("PMIC%d: tps65185_wakeup port=%d port_num = %d\n",
			 i, config->wake_up_pin.port,
					config->wake_up_pin.port_num);

	gpio_info = &config->vcom_ctl_pin;
	ret = eink_sys_script_get_item(pmic_node_array[i], "tps65185_vcom", (int *)gpio_info, 3);
	if (ret != 3)
		_err("PMIC%d: tps65185_vcom is invalid.\n", i);
	else
		pr_debug("PMIC%d: tps65185_vcom port=%d port_num = %d\n", i,
			 config->vcom_ctl_pin.port, config->vcom_ctl_pin.port_num);

	gpio_info = &config->power_up_pin;
	ret = eink_sys_script_get_item(pmic_node_array[i], "tps65185_powerup", (int *)gpio_info, 3);
	if (ret != 3)
		_err("PMIC%d: tps65185_powerup is invalid.\n", i);
	else
		pr_debug("PMIC%d: tps65185_powerup port=%d port_num = %d\n", i,
			 config->power_up_pin.port, config->power_up_pin.port_num);

#if 0
	gpio_info = &config->power_good_pin;
	ret = eink_sys_script_get_item(pmic_node_array[i], "tps65185_powergood", (int *)gpio_info, 3);
	if (ret != 3)
		_err("PMIC%d: tps65185_powergood is invalid.\n", i);
	else
		pr_debug("PMIC%d: tps65185_powergood port=%d port_num = %d\n", i,
			 config->power_good_pin.port, config->power_good_pin.port_num);

	gpio_info = &config->int_pin;
	ret = eink_sys_script_get_item(pmic_node_array[i], "tps65185_int", (int *)gpio_info, 3);
	if (ret != 3)
		_err("PMIC%d: tps65185_int is invalid.\n", i);
	else
		_info("PMIC%d: tps65185_int port=%d port_num = %d\n", i,
		      config->int_pin.port, config->int_pin.port_num);
#endif
	memset(config->pmic_power_name, 0, sizeof(config->pmic_power_name));
	ret = eink_sys_script_get_item(pmic_node_array[i], "tps65185_ldo", (int *)config->pmic_power_name, 2);
	if (ret != 2)
		_err("PMIC%d: get tps65185_ldo fail\n", i);
	else
		pr_debug("PMIC%d: tps65185_ldo = %s\n", i, config->pmic_power_name);

	return 0;
}

static int tps65185_manager_init(void)
{
	int i = 0;
	int ret = -1;

	g_pmic_mgr = malloc(sizeof(*g_pmic_mgr));
	if (!g_pmic_mgr) {
		_err("%s: fail to alloc memory\n", __func__);
		return -1;
	}

	memset(g_pmic_mgr, 0, sizeof(*g_pmic_mgr));
	for (i = 0; i < MAX_PMIC_NUM; i++) {
		g_pmic_mgr->sess[i].id = i;
		ret = tps65185_get_config(&g_pmic_mgr->config[i], i);
		if (ret) {
			_err("PMIC%d: get config fail, ret=%d\n", i, ret);
			break;
		}
	}

	/* master pmic is always used */
	g_pmic_mgr->config[MASTER_PMIC_ID].used_flag = 1;

	return ret;
}

static void tps65185_manager_exit(void)
{
	if (g_pmic_mgr)
		free(g_pmic_mgr);
	return;
}

int tps65185_init(void)
{
	int ret = -1;

	ret = tps65185_manager_init();
	if (ret) {
		_err("pmic manager init fail, ret=%d", ret);
		return -1;
	}

	if (g_pmic_mgr->config[SLAVE_PMIC_ID].used_flag) {
		ret = tps65185_slave_probe();
		if (ret) {
			_err("fail to init slave pmic, ret=%d\n", ret);
			return -2;
		}
		_info("init slave pmic ok\n");
	}

	ret = tps65185_probe();
	if (ret) {
		_err("fail to init master pmic, ret=%d\n", ret);
	} else {
		pr_debug("init master pmic ok\n");
	}

	return ret;
}

void  tps65185_exit(void)
{
	if (g_pmic_mgr->config[SLAVE_PMIC_ID].used_flag) {
		tps65185_slave_remove();
	}

	if (g_pmic_mgr->config[MASTER_PMIC_ID].used_flag) {
		tps65185_remove();
	}

	tps65185_manager_exit();

	return;
}

void tps65185_return_main_i2c(void)
{
	i2c_set_bus_num(old_twi_id);
}

int tps65185_vcom_set(int vcom_mv)
{
	struct pmic_sess *master_pmsess = NULL, *tmp_pmsess = NULL;
	struct papyrus_sess *master_sess = NULL, *tmp_sess = NULL;
	PMIC_ID id = 0;
	u8 rev_val = 0;
	int stat = 0;
	int cnt = 20;

	if (g_pmic_mgr->sess[MASTER_PMIC_ID].is_inited != 1) {
		_err("master pmic has not initial yet\n");
		return -2;
	}

	master_pmsess = &g_pmic_mgr->sess[MASTER_PMIC_ID];
	master_sess = (struct papyrus_sess *)master_pmsess->drvpar;

	//wakeup 65185, sleep mode --> standby mode
#if (CONTRUL_POWER_UP_PING)
	//make sure power up is low
	eink_sys_gpio_set_direction(master_sess->power_up_pin, 1, "tps65185_powerup");
	eink_sys_gpio_set_value(master_sess->power_up_pin, !master_sess->power_active_heght, "tps65185_powerup");
	msleep(2);
#endif
	eink_sys_gpio_set_direction(master_sess->wake_up_pin, 1, "tps65185_wakeup");
	eink_sys_gpio_set_value(master_sess->wake_up_pin, master_sess->wakeup_active_high, "tps65185_wakeup");

	msleep(10);

	for (id = 0; id < MAX_PMIC_NUM; id++) {
		if ((g_pmic_mgr->config[id].used_flag != 1) || (g_pmic_mgr->sess[id].is_inited != 1)) {
			continue;
		}

		tmp_pmsess = &g_pmic_mgr->sess[id];
		tmp_sess = (struct papyrus_sess *)tmp_pmsess->drvpar;

		// Set vcom voltage
		pmic_driver_tps65185_i2c.set_vcom_voltage((struct pmic_sess *)tmp_pmsess, vcom_mv);
		stat |= papyrus_hw_setreg(tmp_sess, PAPYRUS_ADDR_VCOM1_ADJUST, tmp_sess->vcom1);
		stat |= papyrus_hw_setreg(tmp_sess, PAPYRUS_ADDR_VCOM2_ADJUST, tmp_sess->vcom2);
		pr_debug("sess->vcom1 = 0x%x sess->vcom2 = 0x%x\n", tmp_sess->vcom1, tmp_sess->vcom2);

		// PROGRAMMING
		tmp_sess->vcom2 |= 1 << PAPYRUS_VCOM2_PROG;
		stat |= papyrus_hw_setreg(tmp_sess, PAPYRUS_ADDR_VCOM2_ADJUST, tmp_sess->vcom2);
		rev_val = 0;
		while (1) {
			pr_debug("PAPYRUS_ADDR_INT_STATUS1 = 0x%x\n", rev_val);
			stat |= papyrus_hw_getreg(tmp_sess, PAPYRUS_ADDR_INT_STATUS1, &rev_val);
			if (!(rev_val & (1 << PAPYRUS_INT_STATUS1_PRGC)))
				break;
			if (cnt-- <= 0) {
				_err("PMIC%d: set vcom timeout\n", id);
				break;
			}
			msleep(10);
		}
	}

	if (stat)
		_err("papyrus: I2C error: %d\n", stat);
	else
		pr_info("[%s] set vcom %d v success!\n", __func__, vcom_mv);

	return 0;
}

static int tps65185_power_on(void)
{
	struct pmic_sess *tmp_pmsess = NULL;
	PMIC_ID id = 0;

	for (id = 0; id < MAX_PMIC_NUM; id++) {
		if ((g_pmic_mgr->config[id].used_flag != 1) || (g_pmic_mgr->sess[id].is_inited != 1)) {
			continue;
		}

		tmp_pmsess = &g_pmic_mgr->sess[id];
		pmic_driver_tps65185_i2c.hw_power_req((struct pmic_sess *)tmp_pmsess, 1);
	}

	return 0;
}

static int tps65185_power_down(void)
{
	struct pmic_sess *tmp_pmsess = NULL;
	PMIC_ID id = 0;

	for (id = 0; id < MAX_PMIC_NUM; id++) {
		if ((g_pmic_mgr->config[id].used_flag != 1) || (g_pmic_mgr->sess[id].is_inited != 1)) {
			continue;
		}

		tmp_pmsess = &g_pmic_mgr->sess[id];
		pmic_driver_tps65185_i2c.hw_power_req((struct pmic_sess *)tmp_pmsess, 0);
	}

	return 0;
}

int tps65185_temperature_get(int *temp)
{
	struct pmic_sess *master_pmsess = NULL;

	master_pmsess = &g_pmic_mgr->sess[MASTER_PMIC_ID];
	if (master_pmsess->is_inited)
		return pmic_driver_tps65185_i2c.hw_read_temperature(master_pmsess, temp);
	else
		return 0;
}

int register_ebc_pwr_ops(struct ebc_pwr_ops *ops)
{
	ops->power_on = tps65185_power_on;
	ops->power_down = tps65185_power_down;
	return 0;
}
