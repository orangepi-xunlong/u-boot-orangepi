#ifndef __IO_SPI_NAND_H__
#define __IO_SPI_NAND_H__

#include <errno.h>
#include <stdint.h>
#include <stdio.h> /* For ssize_t */
#include <uuid.h>
#include <io_storage.h>
#include <io_driver.h>

typedef struct io_spi_nand_ops {
	size_t	(*read)(unsigned long lba, uintptr_t buf, size_t size);
} io_spi_nand_ops_t;

typedef struct io_spi_nand_dev_spec {
	io_spi_nand_ops_t	ops;
} io_spi_nand_dev_spec_t;

int register_io_dev_spi_nand(const io_dev_connector_t **dev_con);

#endif /* __IO_SPI_NAND_H__ */
