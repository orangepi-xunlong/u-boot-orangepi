/*
 * Copyright (c) 2014, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __IO_I2C_DL_H__
#define __IO_I2C_DL_H__

#include <errno.h>
#include <stdint.h>
#include <stdio.h> /* For ssize_t */
#include <uuid.h>
#include <io_storage.h>

typedef struct io_i2c_dl_ops {
	size_t	(*read)(unsigned long lba, uintptr_t buf, size_t size);
} io_i2c_dl_ops_t;

typedef struct io_i2c_dl_dev_spec {
	io_i2c_dl_ops_t	ops;
} io_i2c_dl_dev_spec_t;

int register_io_dev_i2c_dl(const io_dev_connector_t **dev_con);

#endif /* __IO_I2C_DL_H__ */
