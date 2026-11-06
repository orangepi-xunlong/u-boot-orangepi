#include <arch_helpers.h>
#include <bl_common.h>
#include <cryptodma.h>
#include <cvsp.h>
#include <debug.h>
#include <delay_timer.h>
#include <generic_delay_timer.h>
#include <optee_msg.h>
#include <optee_smc.h>
#include <opteed_private.h>
#include <platform.h>
#include <platform_cvsp.h>
#include <platform_def.h>
#include <spinlock.h>
#include <stdlib.h>
#include <string.h>
#include <console.h>
#include <teesmc_opteed.h>
#include <teesmc_opteed_macros.h>

#include "mbedtls/mbedtls_common.h"
#include "mbedtls/aes.h"
#include "mbedtls/sha256.h"

#include "cvsp_private.h"
#include "cv_private.h"

#include "tpu_security.h"

#define ENABLE_CVSP_DEBUG 0

static uint32_t test_mfr_secret_id;
uint32_t get_test_mfr_secret_id(void)
{
	return test_mfr_secret_id;
}

#if ENABLE_CVSP_DEBUG
static bmsp_args_t *test_memdump(uint64_t func, uint64_t arg1, uint64_t arg2,
				 uint64_t arg3, uint64_t arg4, uint64_t arg5,
				 uint64_t arg6, uint64_t arg7)
{
	size_t s;
	void *p;

	p = (void *)arg2;
	s = arg3;

	tf_printf("p=%p s=0x%lx\n", p, s);
	inv_dcache_range((uintptr_t)p, s);
	tf_print_buffer(p, s, "p");

	return bmsp_set_smc_args(TEESMC_OPTEED_RETURN_CALL_DONE, 0, 0, 0, 0, 0,
				 0, 0);
}

static bmsp_args_t *test_memset(uint64_t func, uint64_t arg1, uint64_t arg2,
				uint64_t arg3, uint64_t arg4, uint64_t arg5,
				uint64_t arg6, uint64_t arg7)
{
	uint32_t v;
	size_t s;
	void *p;

	p = (void *)arg2;
	s = arg3;
	v = arg4;

	tf_printf("p=%p s=0x%lx v=%x\n", p, s, v);
	memset(p, v, s);
	flush_dcache_range((uintptr_t)p, s);
	tf_print_buffer(p, s, "p");

	return bmsp_set_smc_args(TEESMC_OPTEED_RETURN_CALL_DONE, 0, 0, 0, 0, 0,
				 0, 0);
}

static bmsp_args_t *test_delay(uint64_t func, uint64_t arg1, uint64_t arg2,
			       uint64_t arg3, uint64_t arg4, uint64_t arg5,
			       uint64_t arg6, uint64_t arg7)
{
	uint32_t v;

	v = arg2;

	tf_printf("mdelay(%d);\n", v);
	mdelay(v);
	tf_printf("done\n");

	return bmsp_set_smc_args(TEESMC_OPTEED_RETURN_CALL_DONE, 0, 0, 0, 0, 0,
				 0, 0);
}

unsigned long simple_strtoul(const char *cp, char **endp, unsigned int base);
long simple_strtol(const char *cp, char **endp, unsigned int base);

static bmsp_args_t *debug_cmd(uint64_t func, uint64_t arg1, uint64_t arg2,
			      uint64_t arg3, uint64_t arg4, uint64_t arg5,
			      uint64_t arg6, uint64_t arg7)
{
	int32_t ret = 0;
	uint32_t v;
	const char *cmd;
	const char *arg;

	char test_mfr_secret_id_cmd[] = "set_mfr_secret_id";
	char test_delay[] = "delay";

	tf_printf("[%s:%d]\n", __func__, __LINE__);
	tf_printf("arg0=0x%lx 0x%lx 0x%lx 0x%lx\n", func, arg1, arg2, arg3);
	tf_printf("arg4=0x%lx 0x%lx 0x%lx 0x%lx\n", arg4, arg5, arg6, arg7);

	cmd = (const char *)arg2;
	tf_printf("cmd=%s\n", cmd);

	if (memcmp(test_mfr_secret_id_cmd, cmd,
		   sizeof(test_mfr_secret_id_cmd) - 1) == 0) {
		arg = cmd + sizeof(test_mfr_secret_id_cmd) - 1;
		test_mfr_secret_id = simple_strtoul(arg, NULL, 0);
		tf_printf("test_mfr_secret_id=0x%x\n", test_mfr_secret_id);
	} else if (memcmp(test_delay, cmd, sizeof(test_delay) - 1) == 0) {
		arg = cmd + sizeof(test_delay) - 1;
		v = simple_strtoul(arg, NULL, 0);
		tf_printf("mdelay(%d)\n", v);
		mdelay(v);
		tf_printf("done\n");
	}

	tf_printf("[%s:%d] ret=0x%x\n", __func__, __LINE__, ret);

	return bmsp_set_smc_args(TEESMC_OPTEED_RETURN_CALL_DONE, ret, 0, 0, 0,
				 0, 0, 0);
}
#endif /* ENABLE_CVSP_DEBUG */

static bmsp_args_t *enable_console(uint64_t func, uint64_t arg1, uint64_t arg2,
			      uint64_t arg3, uint64_t arg4, uint64_t arg5,
			      uint64_t arg6, uint64_t arg7)
{
	int32_t ret = 0;

	tf_printf("[%s:%d] arg2=%ld\n", __func__, __LINE__, arg2);

	if (arg2) {
		uint32_t clk_in_hz = plat_bm_get_uart_clock();

		console_init(PLAT_BOOT_UART_BASE, clk_in_hz,
			     PLAT_BM_CONSOLE_BAUDRATE);

	} else {
		console_uninit();
	}

	return bmsp_set_smc_args(TEESMC_OPTEED_RETURN_CALL_DONE, ret, 0, 0, 0,
				 0, 0, 0);
}

static bmsp_args_t *smc_call_test(uint64_t func, uint64_t arg1, uint64_t arg2,
				  uint64_t arg3, uint64_t arg4, uint64_t arg5,
				  uint64_t arg6, uint64_t arg7)
{
	uint32_t linear_id = plat_my_core_pos();

	tf_printf("%s(): DEBUG=%d linear_id=%u [0x%lx 0x%lx]\n", __func__, ENABLE_CVSP_DEBUG, linear_id,
		  func, arg1);

	switch (arg1) {
	case 0x7F000001:
		return enable_console(func, arg1, arg2, arg3, arg4, arg5, arg6,
				 arg7);
#if ENABLE_CVSP_DEBUG
	case 0x1F000000:
		return debug_cmd(func, arg1, arg2, arg3, arg4, arg5, arg6,
				 arg7);

	case 0x7F000014:
		return test_memdump(func, arg1, arg2, arg3, arg4, arg5, arg6,
				    arg7);
	case 0x7F000015:
		return test_memset(func, arg1, arg2, arg3, arg4, arg5, arg6,
				   arg7);
	case 0x7F000016:
		return test_delay(func, arg1, arg2, arg3, arg4, arg5, arg6,
				  arg7);
#endif
	default:
		return bmsp_set_smc_args(TEESMC_OPTEED_RETURN_CALL_DONE, -1, 0,
					 0, 0, 0, 0, 0);
	}
}

bmsp_args_t *cvsp_smc_call_debug_handler(uint64_t func, uint64_t arg1,
					 uint64_t arg2, uint64_t arg3,
					 uint64_t arg4, uint64_t arg5,
					 uint64_t arg6, uint64_t arg7)
{
	return smc_call_test(func, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
