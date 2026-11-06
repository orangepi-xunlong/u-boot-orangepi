#include lib/cpu/${BOOT_CPU}/cpu-ops.mk

BL2_ASFLAGS +=\
	$(BL2_CPPFLAGS) \
	-DRISCV \
	-D__ASSEMBLY__ \
	-march=rv64imafdcvxthead -mstrict-align \
	-mcmodel=medany \
	-mabi=lp64d \
	-ffreestanding  \
	-Wa,--fatal-warnings

BL2_CFLAGS += \
	$(BL2_CPPFLAGS) \
	-DRISCV \
	-march=rv64imafdcvxthead \
	-mcmodel=medany \
	-mabi=lp64d \
	-ffreestanding -fno-builtin -Wall -std=gnu99 \
	-Os -ffunction-sections -fdata-sections \
	-fno-delete-null-pointer-checks

TF_LDFLAGS += \
	--fatal-warnings -Os \
	--gc-sections \
	${TF_LDFLAGS_aarch64}

CPU_SOURCES := \
	lib/cpus/${BOOT_CPU}/cpu_helper.c \
	lib/cpus/${BOOT_CPU}/misc_helpers.c \
	lib/cpus/${BOOT_CPU}/cache.c \
	drivers/delay_timer/${BOOT_CPU}/delay_timer.c \
	bl2/arch/${BOOT_CPU}/bl2_helper.c \
	drivers/cvitek/uart/uart_dw.c

BL2_CPU_SOURCES := \
	bl2/arch/${BOOT_CPU}/bl2_entrypoint.S
