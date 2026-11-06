/*
 * Copyright (c) 2014, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __IO_FATFS_H__
#define __IO_FATFS_H__

#include <errno.h>
#include <stdint.h>
#include <stdio.h> /* For ssize_t */
#include <uuid.h>
#include <io_storage.h>

int register_io_dev_fatfs(const io_dev_connector_t **dev_con);

#endif /* __IO_FATFS_H__ */
