# BLCP_PATH = ../fast_image_mcu/riscv/output/fast_image_mcu.bin
# ifeq ("$(wildcard $(BLCP_PATH))","")
# BLCP_PATH = test/empty.bin
# endif
FIP_COMPRESS ?= lzma

CHIP_CONF_PATH = ${BUILD_PLAT}/chip_conf.bin

ifeq (${BOOT_CPU},aarch64)
ifeq (${RELEASE_VER},1)
MONITOR_PATH = plat/${CHIP_ARCH}/bl31.bin
BL32_PATH = plat/${CHIP_ARCH}/bl32.bin
else
MONITOR_PATH = ${BUILD_PLAT}/bl31.bin
BL32_PATH = ${BUILD_PLAT}/bl32.bin
endif
BLMCU_PATH = plat/cvitek/${CHIP_ARCH}/common/prebuilt/cv186x_mcu_fw.bin
else ifeq (${BOOT_CPU},riscv)
MONITOR_PATH = ../opensbi/build/platform/generic/firmware/fw_dynamic.bin
endif

fip%: export BLCP_IMG_RUNADDR=0x05200200
fip%: export BLCP_PARAM_LOADADDR=0
fip%: export NAND_INFO=00000000
fip%: export NOR_INFO=$(shell printf '%72s' | tr ' ' 'FF')
fip%: export DDR_PARAM_TEST_PATH = test/cv181x/ddr_param.bin

${BUILD_PLAT}:
	@mkdir -p '${BUILD_PLAT}'

gen-chip-conf:
	$(print_target)
	${Q}./plat/cvitek/${CHIP_ARCH}/common/chip_conf.py ${CHIP_CONF_PATH}

macro_to_env = ${NM} '${BLMACROS_ELF}' | awk '/DEF_${1}/ { rc = 1; print "${1}=0x" $$1 } END { exit !rc }' >> ${BUILD_PLAT}/blmacros.env

blmacros-env: blmacros
	$(print_target)
	${Q}> ${BUILD_PLAT}/blmacros.env  # clear .env first
	${Q}$(call macro_to_env,MONITOR_RUNADDR)
	${Q}$(call macro_to_env,BL32_RUNADDR)
	${Q}$(call macro_to_env,BLMCU_RUNADDR)
	${Q}$(call macro_to_env,BLCP_2ND_RUNADDR)

fip: fip-all

ifeq (${RELEASE_VER},1)
BL31_TARGET :=
else
BL31_TARGET := bl31
endif

fip-dep: ${BL31_TARGET} bl2 blmacros-env gen-chip-conf

fip-simple: fip-dep
	$(print_target)
	${Q}echo "  [GEN] fip.bin"
	${Q}${FIPTOOL} -v genfip \
		'${BUILD_PLAT}/fip.bin' \
		--CHIP_CONF='${CHIP_CONF_PATH}' \
		--NOR_INFO='${NOR_INFO}' \
		--NAND_INFO='${NAND_INFO}'\
		--BL2='${BUILD_PLAT}/bl2.bin'
	${Q}echo "  [LS] " $$(ls -l '${BUILD_PLAT}/fip.bin')

fip-all: fip-dep
	$(print_target)
	${Q}echo "  [GEN] fip.bin"
	${Q}. ${BUILD_PLAT}/blmacros.env && \
	${FIPTOOL} -v genfip \
		'${BUILD_PLAT}/fip.bin' \
		--MONITOR_RUNADDR="$${MONITOR_RUNADDR}" \
		--BL32_RUNADDR="$${BL32_RUNADDR}" \
		--BLMCU_RUNADDR="$${BLMCU_RUNADDR}" \
		--BLCP_2ND_RUNADDR="$${BLCP_2ND_RUNADDR}" \
		--CHIP_CONF='${CHIP_CONF_PATH}' \
		--NOR_INFO='${NOR_INFO}' \
		--NAND_INFO='${NAND_INFO}'\
		--BL2='${BUILD_PLAT}/bl2.bin' \
		--BLCP_IMG_RUNADDR=${BLCP_IMG_RUNADDR} \
		--BLCP_PARAM_LOADADDR=${BLCP_PARAM_LOADADDR} \
		--BLCP=${BLCP_PATH} \
		--DDR_PARAM='${DDR_PARAM_TEST_PATH}' \
		--BLCP_2ND='${BLCP_2ND_PATH}' \
		--MONITOR='${MONITOR_PATH}' \
		--BL32='${BL32_PATH}' \
		--BLMCU='${BLMCU_PATH}' \
		--LOADER_2ND='${LOADER_2ND_PATH}' \
		--compress='${FIP_COMPRESS}'
	${Q}echo "  [LS] " $$(ls -l '${BUILD_PLAT}/fip.bin')
