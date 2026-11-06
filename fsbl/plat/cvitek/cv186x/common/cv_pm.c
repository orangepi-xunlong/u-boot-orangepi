/*
 * Copyright (c) 2015-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch_helpers.h>
#include <assert.h>
#include <debug.h>
#include <console.h>
#include <string.h>
#include <gicv2.h>
#include <platform_def.h>
#include <platform.h>
#include <psci.h>
#include <mmio.h>
#include <plat_arm.h>
#include <delay_timer.h>
#include <stdio.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

/*
 * The secure entry point to be used on warm reset.
 */
static unsigned long secure_entrypoint;
static uint32_t screl3;

/* Make composite power state parameter till power level 0 */
#if PSCI_EXTENDED_STATE_ID

#define bm_make_pwrstate_lvl0(lvl0_state, pwr_lvl, type)                                                               \
	(((lvl0_state) << PSTATE_ID_SHIFT) | ((type) << PSTATE_TYPE_SHIFT))
#else
#define bm_make_pwrstate_lvl0(lvl0_state, pwr_lvl, type)                                                               \
	(((lvl0_state) << PSTATE_ID_SHIFT) | ((pwr_lvl) << PSTATE_PWR_LVL_SHIFT) | ((type) << PSTATE_TYPE_SHIFT))
#endif /* PSCI_EXTENDED_STATE_ID */

#define bm_make_pwrstate_lvl1(lvl1_state, lvl0_state, pwr_lvl, type)                                                   \
	(((lvl1_state) << PLAT_LOCAL_PSTATE_WIDTH) | bm_make_pwrstate_lvl0(lvl0_state, pwr_lvl, type))

/*
 *  The table storing the valid idle power states. Ensure that the
 *  array entries are populated in ascending order of state-id to
 *  enable us to use binary search during power state validation.
 *  The table must be terminated by a NULL entry.
 */
static const unsigned int bm_pm_idle_states[] = {
	/* State-id - 0x01 */
	bm_make_pwrstate_lvl1(PLAT_LOCAL_STATE_RUN, PLAT_LOCAL_STATE_RET, MPIDR_AFFLVL0, PSTATE_TYPE_STANDBY),
	/* State-id - 0x02 */
	bm_make_pwrstate_lvl1(PLAT_LOCAL_STATE_RUN, PLAT_LOCAL_STATE_OFF, MPIDR_AFFLVL0, PSTATE_TYPE_POWERDOWN),
	/* State-id - 0x22 */
	bm_make_pwrstate_lvl1(PLAT_LOCAL_STATE_OFF, PLAT_LOCAL_STATE_OFF, MPIDR_AFFLVL1, PSTATE_TYPE_POWERDOWN),
	0,
};

static uintptr_t g_ncore_base;
enum {
	BM_CORE_DOWN = 0,
	BM_CORE_UP = 1,
};
/* set ncore enable/disable for cluster1/2 */
void bm_ncore_enable_cluster(int32_t en)
{
	uint32_t value, mask;

	mask = ((1 << PLATFORM_CLUSTER_COUNT) - 1);
	if (en) {
		value = mmio_read_32(g_ncore_base + NCORE_DIRUCASER0);
		mmio_write_32(g_ncore_base + NCORE_DIRUCASER0, value | mask);
		mmio_write_32(g_ncore_base + NCORE_DIRUMRHER, 0x1);
		mmio_write_32(g_ncore_base + NCORE_DIRUSFER, 0x1);

		value = mmio_read_32(g_ncore_base + NCORE_CSADSER);
		mmio_write_32(g_ncore_base + NCORE_CSADSER, value | mask);
	} else {
		value = mmio_read_32(g_ncore_base + NCORE_DIRUCASER0);
		mmio_write_32(g_ncore_base + NCORE_DIRUCASER0, value & ~mask);
		mmio_write_32(g_ncore_base + NCORE_DIRUMRHER, 0);
		mmio_write_32(g_ncore_base + NCORE_DIRUSFER, 0);

		value = mmio_read_32(g_ncore_base + NCORE_CSADSER);
		mmio_write_32(g_ncore_base + NCORE_CSADSER, value & ~mask);
	}
}

void bm_ncore_init(uintptr_t ncore_base)
{
	g_ncore_base = ncore_base;
}

static void suspend_set_irq_routel3(void)
{
	screl3 = read_scr_el3();

	write_scr_el3(screl3 | SCR_IRQ_BIT | SCR_FIQ_BIT);
	isb();
	dsb();
}

static void suspend_restore_irqel3(void)
{
	write_scr_el3(screl3);
	isb();
	dsb();
}

/*******************************************************************************
 * Platform handler called to check the validity of the power state
 * parameter. The power state parameter has to be a composite power state.
 ******************************************************************************/
static int bm_validate_power_state(unsigned int power_state, psci_power_state_t *req_state)
{
	unsigned int state_id;
	int i;

	assert(req_state);

	/*
	 *  Currently we are using a linear search for finding the matching
	 *  entry in the idle power state array. This can be made a binary
	 *  search if the number of entries justify the additional complexity.
	 */
	for (i = 0; !!bm_pm_idle_states[i]; i++) {
		if (power_state == bm_pm_idle_states[i])
			break;
	}

	/* Return error if entry not found in the idle state array */
	if (!bm_pm_idle_states[i])
		return PSCI_E_INVALID_PARAMS;

	i = 0;
	state_id = psci_get_pstate_id(power_state);

	/* Parse the State ID and populate the state info parameter */
	while (state_id) {
		req_state->pwr_domain_state[i++] = state_id & PLAT_LOCAL_PSTATE_MASK;
		state_id >>= PLAT_LOCAL_PSTATE_WIDTH;
	}

	return PSCI_E_SUCCESS;
}

DEFINE_BAKERY_LOCK(console_lock);

static void print_entry(const char *caller)
{
	unsigned int idx = plat_my_core_pos();
	bakery_lock_get(&console_lock);
	NOTICE("CPU :%d %s\n", idx, caller);
	bakery_lock_release(&console_lock);
}

/*******************************************************************************
 * Platform handler called to check the validity of the non secure
 * entrypoint.
 ******************************************************************************/
static int bm_validate_ns_entrypoint(uintptr_t entrypoint)
{
	/*
	 * Check if the non secure entrypoint lies within the non
	 * secure DRAM.
	 */
	NOTICE("ns_entrypoint=0x%lx\n", entrypoint);
	if ((entrypoint >= NS_DRAM0_BASE) && (entrypoint < (NS_DRAM0_BASE + NS_DRAM0_SIZE)))
		return PSCI_E_SUCCESS;
	return PSCI_E_INVALID_ADDRESS;
}

/*******************************************************************************
 * Platform handler called when a CPU is about to enter standby.
 ******************************************************************************/
static void bm_cpu_standby(plat_local_state_t cpu_state)
{
	print_entry(__func__);
	assert(cpu_state == PLAT_LOCAL_STATE_RET);

	/*
	 * Enter standby state
	 * dsb is good practice before using wfi to enter low power states
	 */
	dsb();
	wfi();
}

/*******************************************************************************
 * Platform handler called when a power domain is about to be turned on. The
 * mpidr determines the CPU to be turned on.
 ******************************************************************************/
static int bm_pwr_domain_on(u_register_t mpidr)
{
	int rc = PSCI_E_SUCCESS;
	unsigned int pos = plat_core_pos_by_mpidr(mpidr);
	uint64_t *hold_base = (uint64_t *)PLAT_BM_HOLD_BASE;

	print_entry(__func__);

	mmio_clrbits_32(PM_NON_PRIMARY_CPU_HOLD, 0x1 << pos);
	hold_base[pos] = PLAT_BM_HOLD_STATE_GO;
	dsb();
	sev();

	return rc;
}

/*******************************************************************************
 * Platform handler called when a power domain is about to be turned off. The
 * target_state encodes the power state that each level should transition to.
 ******************************************************************************/
void bm_pwr_domain_off(const psci_power_state_t *target_state)
{
    print_entry(__func__);
}

/*******************************************************************************
 * Platform handler called when a power domain is about to be suspended. The
 * target_state encodes the power state that each level should transition to.
 ******************************************************************************/
void bm_pwr_domain_suspend(const psci_power_state_t *target_state)
{
	print_entry(__func__);
	suspend_set_irq_routel3();
}

/*******************************************************************************
 * Platform handler called when a power domain has just been powered on after
 * being turned off earlier. The target_state encodes the low power state that
 * each level has woken up from.
 ******************************************************************************/
void bm_pwr_domain_on_finish(const psci_power_state_t *target_state)
{
	print_entry(__func__);
	assert(target_state->pwr_domain_state[MPIDR_AFFLVL0] == PLAT_LOCAL_STATE_OFF);

	bm_ncore_enable_cluster(BM_CORE_UP);
	/* TODO: This setup is needed only after a cold boot */
	gicv2_pcpu_distif_init();

	/* Enable the gic cpu interface */
	gicv2_cpuif_enable();
}

/*******************************************************************************
 * Platform handler called when a power domain has just been powered on after
 * having been suspended earlier. The target_state encodes the low power state
 * that each level has woken up from.
 ******************************************************************************/
void bm_pwr_domain_suspend_finish(const psci_power_state_t *target_state)
{
	print_entry(__func__);
	suspend_restore_irqel3();
	bm_ncore_enable_cluster(BM_CORE_UP);
	/* Two GIC related initialized functions should be applied after CPU reset*/
	/* Global gic distributor init => gicv2_distif_init() */
	/* Per cpu gic distributor setup => gicv2_pcpu_distif_init() */
	//plat_arm_gic_init();
}

#if 0
__asm__(".section .rodata\n"
	".global rtc_core_sram_bin\n"
	".global rtc_core_sram_bin_end\n"
	".type rtc_core_sram_bin,%object\n"
	".type rtc_core_sram_bin_end,%object\n"
	".balign 16\n"
	"rtc_core_sram_bin:\n"
	".incbin \"" RTC_CORE_SRAM_BIN_PATH "\"\n"
	".balign 16\n"
	"rtc_core_sram_bin_end:\n"
	".text\n");
#endif

extern uint8_t rtc_core_sram_bin[];
extern uint8_t rtc_core_sram_bin_end[];

void rtc_latch_pinmux_settings(void)
{
	mmio_write_32(0x05027084, 0x77);	//Force PWR_BUTTON1 to PWR_GPIO[8] and PWR_BUTTON1
	mmio_write_32(0x05027088, 0x77);	//Latch pinmux (PWR_ON)
	mmio_write_32(0x0502708C, 0x0);	//Latch pinmux (PWR_WAKEUP0)
	mmio_write_32(0x05027090, 0x0);	//Latch pinmux (PWR_WAKEUP1)
	mmio_write_32(0x05027094, 0x0);	//Latch pinmux (CLK25M)
	mmio_write_32(0x05027098, 0x0);	//Latch pinmux (CLK32K)
	mmio_write_32(0x0502709C, 0x0);	//Latch pinmux (IIC2_SCL)
	mmio_write_32(0x050270A0, 0x0);	//Latch pinmux (IIC2_SDA)
	mmio_write_32(0x050270A4, 0x0);	//Latch pinmux (PWR_GPIO0)
	mmio_write_32(0x050270A8, 0x0);	//Latch pinmux (PWR_GPIO1)
	mmio_write_32(0x050270AC, 0x0);	//Latch pinmux (PWR_GPIO2)
	mmio_write_32(0x050270B0, 0x0);	//Latch pinmux (SD1_CLK)
	mmio_write_32(0x050270B4, 0x0);	//Latch pinmux (SD1_CMD)
	mmio_write_32(0x050270B8, 0x0);	//Latch pinmux (SD1_D0)
	mmio_write_32(0x050270BC, 0x0);	//Latch pinmux (SD1_D1)
	mmio_write_32(0x050270C0, 0x0);	//Latch pinmux (SD1_D2)
	mmio_write_32(0x050270C4, 0x0);	//Latch pinmux (SD1_D3)
	mmio_write_32(0x050270C8, 0x0);	//Latch pinmux (UART2_CTS)
	mmio_write_32(0x050270CC, 0x0);	//Latch pinmux (UART2_RTS)
	mmio_write_32(0x050270D0, 0x0);	//Latch pinmux (UART2_RX)
	mmio_write_32(0x050270D4, 0x0);	//Latch pinmux (UART2_TX)
	printf("%s\n", __func__);
};

void rtc_power_saving_settings_for_suspend(void)
{
	mmio_write_32(0x05027074, 0x09040104);	// Set VDDC_RTC LDO to 0.7V
	mmio_write_32(0x05026090, 0x4000);	// Extend power off sequence
	mmio_write_32(0x050260A8, 0x5000);	// Extend power off sequence

	mmio_write_32(0x05025084, 0x00030003);	// m51 iso_en, sd_iso_en
	mmio_write_32(0x05025080, 0x00000000);	// m51 pd, sd power pd
	mmio_write_32(0x05025028, 0x20120C00);	// rtc_sys_req off when RTC suspend
	mmio_write_32(0x05025098, 0x000009B6);	// sram => pd ahb,sdio,mcu , slp rtc
	mmio_write_32(0x05025018, 0x8107FFFD);	// [31] reg_rtcsys_rstn_src_sel=1
	printf("%s\n", __func__);
}

/*******************************************************************************
 * Copy suspend code into RTC CORE SRAM
 ******************************************************************************/
	extern void plat_resume_entry(void);

void plat_primary_suspend(void)
{
	disable_mmu_el3();

	// bypass to xtal & pll pd
	mmio_write_32(0x28102198, 0xffffffff);
	mmio_write_32(0x2810219c, 0xffffffff);
	mmio_write_32(0x281021a0, 0xffffffff);
	mmio_write_32(0x281021a4, 0xffffffff);
	mmio_write_32(0x281021a8, 0xffffffff);
	mmio_clrbits_32(0x28102400, 0x5400);
	mmio_clrbits_32(0x28102500, 0x5400);
	mmio_clrbits_32(0x28102600, 0x5400);
	mmio_clrbits_32(0x28102700, 0x5400);
	mmio_clrbits_32(0x28102900, 0x5400);
	isb();
	dsbsy();
	wfi();
	// pll up & switch to pll
	mmio_setbits_32(0x28102400, 0x5400);
	mmio_setbits_32(0x28102500, 0x5400);
	mmio_setbits_32(0x28102600, 0x5400);
	mmio_setbits_32(0x28102700, 0x5400);
	mmio_setbits_32(0x28102900, 0x5400);
	mmio_write_32(0x28102198, 0);
	mmio_write_32(0x2810219c, 0);
	mmio_write_32(0x281021a0, 0);
	mmio_write_32(0x281021a4, 0);
	mmio_write_32(0x281021a8, 0);

	plat_resume_entry();
	mdelay(500);
	while (1) {
		NOTICE("Return from suspend code.\n");
		mdelay(100);
	}
}

void plat_non_primary_suspend_entry(void)
{
	extern void plat_off_mmu(void);
	unsigned int idx = plat_my_core_pos();

	plat_off_mmu();
	mmio_setbits_32(PM_NON_PRIMARY_CPU_HOLD, 0x1 << idx);
	do {
		wfe();
	} while (mmio_read_32(PM_NON_PRIMARY_CPU_HOLD) & (0x1 << idx));
	plat_resume_entry();
}

/*******************************************************************************
 * Handler called at the end of the power domain suspend sequence. The
 * target_state encodes the power state that each level should transition to.
 ******************************************************************************/
__dead2 void bm_pwr_domain_pwr_down_wfi(const psci_power_state_t *target_state)
{
	unsigned int idx = plat_my_core_pos();

	if (idx > 0) {
		/* for non-primary CPU, we jump to Rom for mailbox polling */
		plat_non_primary_suspend_entry();
	} else {
		/* for primary CPU, we jump to Dram Self-refresh setting */
		plat_primary_suspend();
	}
	/* Should never reach here */
	panic();
}

/*******************************************************************************
 * Platform handlers to shutdown/reboot the system
 ******************************************************************************/
static void __dead2 bm_system_off(void)
{
	if (mmio_read_32(REG_RTC_BASE + RTC_INFO0) == CVI_SAPD_FLAG) {
		NOTICE("Send sapd request\n");
		rtc_latch_pinmux_settings();
		rtc_power_saving_settings_for_suspend();
		/* Enable power suspend wakeup source mask */
		mmio_write_32(REG_RTC_BASE + 0x3C, 0x1); // 1 = select prdata from 32K domain
		mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0_UNLOCKKEY, 0xAB18);
		mmio_write_32(REG_RTC_BASE + RTC_EN_PWR_WAKEUP, 0x3F);
		NOTICE("RTC_EN_PWR_WAKEUP=0x%x\n", mmio_read_32(REG_RTC_BASE + RTC_EN_PWR_WAKEUP));
		mmio_write_32(REG_RTC_BASE + RTC_EN_SUSPEND_REQ, 0x01);
		while (mmio_read_32(REG_RTC_BASE + RTC_EN_SUSPEND_REQ) != 0x01)
			;
		/* Send suspend request to RTC */
		mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0, 0x00800080);
		panic();
	} else {
		mmio_write_32(REG_RTC_BASE + RTC_EN_SHDN_REQ, 0x01);
		while (mmio_read_32(REG_RTC_BASE + RTC_EN_SHDN_REQ) != 0x01)
			;
		mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0_UNLOCKKEY, 0xAB18);
		mmio_setbits_32(REG_RTC_CTRL_BASE + RTC_CTRL0, 0xFFFF0800 | (0x1 << 0));
		panic();
	}
}

static void __dead2 bm_system_reset(void)
{
	// clear spinand sv vec in case stuck at next boot.
	mmio_setbits_32(CLKGEN_BASE + 0x168, 0x1f0000); // Enable clk_tpu
	// enable tpu
	mmio_setbits_32(TPUSYS_REG_CLK_ENABLE, 0x1);
	mmio_setbits_32(TPUSYS_TPU0_REGMEM_REGION2_CLK_ENABLE, 0x1);
	memset((void *)BM_IO_BUF_BASE, 0x0, BM_IO_BUF_SIZE); //8KB

	mmio_write_32(REG_RTC_BASE + RTC_EN_PWR_CYC_REQ, 0x01);
	while (mmio_read_32(REG_RTC_BASE + RTC_EN_PWR_CYC_REQ) != 0x01)
		;
	mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0_UNLOCKKEY, 0xAB18);
	mmio_setbits_32(REG_RTC_CTRL_BASE + RTC_CTRL0, 0xFFFF0800 | (0x1 << 3));
	panic();
}

void bm_get_sys_suspend_power_state(psci_power_state_t *req_state)
{
	int i;

	for (i = MPIDR_AFFLVL0; i <= PLAT_MAX_PWR_LVL; i++)
		req_state->pwr_domain_state[i] = PLAT_MAX_OFF_STATE;
}

static const plat_psci_ops_t plat_bm_psci_pm_ops = { .cpu_standby = bm_cpu_standby,
						     .pwr_domain_on = bm_pwr_domain_on,
						     .pwr_domain_off = bm_pwr_domain_off,
						     .pwr_domain_suspend = bm_pwr_domain_suspend,
						     .pwr_domain_on_finish = bm_pwr_domain_on_finish,
						     .pwr_domain_suspend_finish = bm_pwr_domain_suspend_finish,
						     .pwr_domain_pwr_down_wfi = bm_pwr_domain_pwr_down_wfi,
						     .system_off = bm_system_off,
						     .system_reset = bm_system_reset,
						     .validate_power_state = bm_validate_power_state,
						     .validate_ns_entrypoint = bm_validate_ns_entrypoint,
						     .get_sys_suspend_power_state = bm_get_sys_suspend_power_state };

int plat_setup_psci_ops(uintptr_t sec_entrypoint, const plat_psci_ops_t **psci_ops)
{
	uintptr_t *mailbox = (void *)PLAT_BM_TRUSTED_MAILBOX_BASE;

	*mailbox = sec_entrypoint;
	secure_entrypoint = (unsigned long)sec_entrypoint;
	*psci_ops = &plat_bm_psci_pm_ops;

	return 0;
}

#pragma GCC diagnostic pop
