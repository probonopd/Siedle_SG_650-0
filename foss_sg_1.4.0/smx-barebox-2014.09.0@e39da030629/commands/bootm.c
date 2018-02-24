/*
 * (C) Copyright 2000-2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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

/*
 * Boot support
 */
#include <common.h>
#include <driver.h>
#include <command.h>
#include <image.h>
#include <malloc.h>
#include <environment.h>
#include <asm/byteorder.h>
#include <xfuncs.h>
#include <getopt.h>
#include <fcntl.h>
#include <fs.h>
#include <errno.h>
#include <boot.h>
#include <of.h>
#include <rtc.h>
#include <init.h>
#include <of.h>
#include <magicvar.h>
#include <uncompress.h>
#include <memory.h>
#include <filetype.h>
#include <binfmt.h>
#include <globalvar.h>
#include <magicvar.h>
#include <asm-generic/memory_layout.h>

#define BOOTM_OPTS_COMMON "ca:e:vo:fd"

#ifdef CONFIG_CMD_BOOTM_INITRD
#define BOOTM_OPTS BOOTM_OPTS_COMMON "L:r:"
#else
#define BOOTM_OPTS BOOTM_OPTS_COMMON
#endif

static int do_bootm(int argc, char *argv[])
{
	int opt;
	struct bootm_data data = {};
	int ret = 1;
	const char *oftree = NULL, *initrd_file = NULL, *os_file = NULL;

	data.initrd_address = UIMAGE_INVALID_ADDRESS;
	data.os_address = UIMAGE_SOME_ADDRESS;
	data.verify = 0;
	data.verbose = 0;

	oftree = getenv("global.bootm.oftree");
	os_file = getenv("global.bootm.image");
	getenv_ul("global.bootm.image.loadaddr", &data.os_address);
	getenv_ul("global.bootm.initrd.loadaddr", &data.initrd_address);
	if (IS_ENABLED(CONFIG_CMD_BOOTM_INITRD))
		initrd_file = getenv("global.bootm.initrd");

	while ((opt = getopt(argc, argv, BOOTM_OPTS)) > 0) {
		switch(opt) {
		case 'c':
			data.verify = 1;
			break;
#ifdef CONFIG_CMD_BOOTM_INITRD
		case 'L':
			data.initrd_address = simple_strtoul(optarg, NULL, 0);
			break;
		case 'r':
			initrd_file = optarg;
			break;
#endif
		case 'a':
			data.os_address = simple_strtoul(optarg, NULL, 0);
			break;
		case 'e':
			data.os_entry = simple_strtoul(optarg, NULL, 0);
			break;
		case 'v':
			data.verbose++;
			break;
		case 'o':
			oftree = optarg;
			break;
		case 'f':
			data.force = 1;
			break;
		case 'd':
			data.dryrun = 1;
			break;
		default:
			break;
		}
	}

	if (optind != argc)
		os_file = argv[optind];

	if (!os_file || !*os_file) {
		printf("no boot image given\n");
		goto err_out;
	}

	if (initrd_file && !*initrd_file)
		initrd_file = NULL;

	if (oftree && !*oftree)
		oftree = NULL;

	data.os_file = os_file;
	data.oftree_file = oftree;
	data.initrd_file = initrd_file;

	ret = bootm_boot(&data);
	if (ret) {
		printf("handler failed with: %s\n", strerror(-ret));
		goto err_out;
	}

	if (data.dryrun)
		printf("Dryrun. Aborted\n");

err_out:
	return ret ? 1 : 0;
}

BAREBOX_CMD_HELP_START(bootm)
BAREBOX_CMD_HELP_TEXT("Options:")
BAREBOX_CMD_HELP_OPT ("-c\t",  "crc check uImage data")
BAREBOX_CMD_HELP_OPT ("-d\t",  "dry run: check data, but do not run")
BAREBOX_CMD_HELP_OPT ("-f\t",  "load images even if type is undetectable")
#ifdef CONFIG_CMD_BOOTM_INITRD
BAREBOX_CMD_HELP_OPT ("-r INITRD","specify an initrd image")
BAREBOX_CMD_HELP_OPT ("-L ADDR\t","specify initrd load address")
#endif
BAREBOX_CMD_HELP_OPT ("-a ADDR\t","specify os load address")
BAREBOX_CMD_HELP_OPT ("-e OFFS\t","entry point to the image relative to start (0)")
#ifdef CONFIG_OFTREE
BAREBOX_CMD_HELP_OPT ("-o DTB\t","specify open firmware device tree")
#endif
#ifdef CONFIG_CMD_BOOTM_VERBOSE
BAREBOX_CMD_HELP_OPT ("-v\t","verbose")
#endif
BAREBOX_CMD_HELP_END

BAREBOX_CMD_START(bootm)
	.cmd		= do_bootm,
	BAREBOX_CMD_DESC("boot an application image")
	BAREBOX_CMD_OPTS("[-cdf"
#ifdef CONFIG_CMD_BOOTM_INITRD
					  "rL"
#endif
					  "ae"
#ifdef CONFIG_OFTREE
					  "o"
#endif
#ifdef CONFIG_CMD_BOOTM_VERBOSE
					  "v"
#endif
					  "] IMAGE")
	BAREBOX_CMD_GROUP(CMD_GRP_BOOT)
	BAREBOX_CMD_HELP(cmd_bootm_help)
BAREBOX_CMD_END

BAREBOX_MAGICVAR(bootargs, "Linux kernel parameters");
BAREBOX_MAGICVAR_NAMED(global_bootm_image, global.bootm.image, "bootm default boot image");
BAREBOX_MAGICVAR_NAMED(global_bootm_image_loadaddr, global.bootm.image.loadaddr, "bootm default boot image loadaddr");
BAREBOX_MAGICVAR_NAMED(global_bootm_initrd, global.bootm.initrd, "bootm default initrd");
BAREBOX_MAGICVAR_NAMED(global_bootm_initrd_loadaddr, global.bootm.initrd.loadaddr, "bootm default initrd loadaddr");

static struct binfmt_hook binfmt_uimage_hook = {
	.type = filetype_uimage,
	.exec = "bootm",
};

static int binfmt_uimage_init(void)
{
	return binfmt_register(&binfmt_uimage_hook);
}
fs_initcall(binfmt_uimage_init);
