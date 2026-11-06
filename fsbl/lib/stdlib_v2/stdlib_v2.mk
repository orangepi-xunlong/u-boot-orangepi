#
# Copyright (c) 2016-2017, ARM Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

STDLIB_SRCS_v2	:=	$(addprefix lib/stdlib_v2/,	\
			misc.c				\
			abort.c				\
			assert.c			\
			exit.c				\
			mem.c				\
			printf.c			\
			putchar.c			\
			puts.c				\
			sscanf.c			\
			strchr.c			\
			strcmp.c			\
			strlen.c			\
			strncmp.c			\
			strnlen.c			\
			subr_prf.c			\
			timingsafe_bcmp.c)

ifneq (${ARCH},riscv)
STDLIB_SRCS_v2 += $(addprefix lib/stdlib_v2/,memcpy.S)
endif

STDLIB_INCLUDES_V2	+=	-Iinclude/lib/stdlib_v2		\
			-Iinclude/lib/stdlib_v2/sys
