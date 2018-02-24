/*
 * drivers/misc/sram.c - generic memory mapped SRAM driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <errno.h>
#include <driver.h>
#include <malloc.h>
#include <init.h>

struct sram {
	struct resource *res;
	char *name;
	struct cdev cdev;
};

static struct file_operations memops = {
	.read  = mem_read,
	.write = mem_write,
	.memmap = generic_memmap_rw,
	.lseek = dev_lseek_default,
};

static int sram_probe(struct device_d *dev)
{
	struct sram *sram;
	struct resource *res;
	void __iomem *base;
	int ret;

	base = dev_request_mem_region(dev, 0);
	if (!base)
		return -EBUSY;

	sram = xzalloc(sizeof(*sram));

	sram->cdev.name = asprintf("sram%d",
			cdev_find_free_index("sram"));

	res = dev_get_resource(dev, IORESOURCE_MEM, 0);

	sram->cdev.size = (unsigned long)resource_size(res);
	sram->cdev.ops = &memops;
	sram->cdev.dev = dev;

	ret = devfs_create(&sram->cdev);
	if (ret)
		return ret;

	return 0;
}

static __maybe_unused struct of_device_id sram_dt_ids[] = {
	{
		.compatible = "mmio-sram",
	}, {
	},
};

static struct driver_d sram_driver = {
	.name = "mmio-sram",
	.probe = sram_probe,
	.of_compatible = sram_dt_ids,
};
device_platform_driver(sram_driver);
