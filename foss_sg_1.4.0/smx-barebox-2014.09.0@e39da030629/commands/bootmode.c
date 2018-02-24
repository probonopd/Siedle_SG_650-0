/*
 * bootmode.c - bootmode command
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <bootmode.h>
#include <bootsource.h>
#include <common.h>
#include <command.h>
#include <complete.h>
#include <driver.h>
#include <getopt.h>
#include <linux/ctype.h>

struct bootsource_name {
	enum bootsource source;
	char *name;
};

static struct bootsource_name bootsources[] = {
	{ BOOTSOURCE_NAND,   "nand" },
	{ BOOTSOURCE_NOR,    "nor"  },
	{ BOOTSOURCE_MMC,    "mmc"  },
	{ BOOTSOURCE_I2C,    "i2c"  },
	{ BOOTSOURCE_SPI,    "m25p" },
	{ BOOTSOURCE_SERIAL, "cs"   },
	{ BOOTSOURCE_HD,     "ahci" },
};

static enum bootsource get_bootsource(struct device_d *dev, int *instance)
{
	int len, i, id;
	const char *start, *end;

	start = dev_name(dev);
	end = start + strlen(start);
	if (!isdigit(*(end-1)))
		return BOOTSOURCE_UNKNOWN;
	while (isdigit(*(end-1)) && end > start)
		end--;

	for (i = 0; i < ARRAY_SIZE(bootsources); i++) {
		len = strlen(bootsources[i].name);
		if (end - start > len)
			len = end - start;
		if (strncmp(bootsources[i].name, start, len) != 0)
			continue;

		if (instance) {
			struct device_d *parent = dev->parent;

			if (bootsources[i].source == BOOTSOURCE_SPI && parent)
				parent = parent->parent;

			if (parent && parent->device_node) {
				const char *alias;

				alias = of_alias_get(parent->device_node);
				if (alias) {
					start = alias;
					end = start + strlen(start);
					while (isdigit(*(end-1)) && end > start)
						end--;
				}
			}
			id = simple_strtol(end, 0, 10);
			if (id >= 0)
				*instance = id;
		}
		return bootsources[i].source;
	}

	return BOOTSOURCE_UNKNOWN;
}

static int do_bootmode(int argc, char *argv[])
{
	struct device_d *dev;
	struct device_node *node = NULL;
	enum bootsource bootsource;
	int opt, instance = BOOTSOURCE_INSTANCE_UNKNOWN;
	int option_list = 0;

	while ((opt = getopt(argc, argv, "l")) > 0) {
		switch (opt) {
		case 'l':
			option_list = 1;
			break;
		}
	}

	if (option_list) {
		printf("possible boot devices:\n");
		for_each_device(dev) {
			bootsource = get_bootsource(dev, &instance);
			if (bootsource == BOOTSOURCE_UNKNOWN)
				continue;
			if (bootsource == bootsource_get() && instance == bootsource_get_instance())
				printf("* %s\n", dev_name(dev));
			else
				printf("  %s\n", dev_name(dev));
		}
		return 0;
	}

	if (argc == optind)
		return COMMAND_ERROR_USAGE;

	dev = get_device_by_name(argv[optind]);
	if (!dev)
		return -ENODEV;

	bootsource = get_bootsource(dev, &instance);
	if (bootsource == BOOTSOURCE_UNKNOWN)
		return -EINVAL;

	if (dev->parent)
		node = dev->parent->device_node;

	return bootmode_set(bootsource, instance, node);
}

BAREBOX_CMD_HELP_START(bootmode)
BAREBOX_CMD_HELP_TEXT("Set device to boot from after next reset\n")
BAREBOX_CMD_HELP_TEXT("")
BAREBOX_CMD_HELP_TEXT("Options:")
BAREBOX_CMD_HELP_OPT ("-l",  "list boot devices")
BAREBOX_CMD_HELP_END

BAREBOX_CMD_START(bootmode)
	.cmd		= do_bootmode,
	BAREBOX_CMD_DESC("select boot device")
	BAREBOX_CMD_OPTS("[-l] DEVICE")
	BAREBOX_CMD_GROUP(CMD_GRP_BOOT)
	BAREBOX_CMD_COMPLETE(device_complete)
	BAREBOX_CMD_HELP(cmd_bootmode_help)
BAREBOX_CMD_END
