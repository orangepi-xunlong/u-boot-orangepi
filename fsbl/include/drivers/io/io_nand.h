/*
 * Copyright (c) 2014, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __IO_NAND_H__
#define __IO_NAND_H__

#include <errno.h>
#include <stdint.h>
#include <stdio.h> /* For ssize_t */
#include <uuid.h>
#include <io_storage.h>
#include <io_driver.h>

typedef struct io_nand_ops {
	size_t	(*read)(unsigned long lba, uintptr_t buf, size_t size);
} io_nand_ops_t;

typedef struct io_nand_dev_spec {
	io_nand_ops_t	ops;
} io_nand_dev_spec_t;

int register_io_dev_nand(const io_dev_connector_t **dev_con);

#endif /* __IO_I2C_DL_H__ */
