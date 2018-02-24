/*
 * bootargs.c - concatenate Linux bootargs
 *
 * Copyright (c) 2012 Sascha Hauer <s.hauer@pengutronix.de>, Pengutronix
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <common.h>
#include <boot.h>
#include <malloc.h>
#include <magicvar.h>
#include <globalvar.h>
#include <environment.h>

static char *linux_bootargs;
static int linux_bootargs_overwritten;

/*
 * This returns the Linux bootargs
 *
 * There are two ways to handle bootargs. The old legacy way is to use the
 * 'bootargs' environment variable. The new and more flexible way is to use
 * global variables beginning with "global.linux.bootargs." and
 * "global.linux.mtdparts.". These variables will be concatenated together to
 * the resulting bootargs. If there are no "global.linux.bootargs." variables
 * we fall back to "bootargs"
 */
const char *linux_bootargs_get(void)
{
	char *bootargs, *mtdparts;

	if (linux_bootargs_overwritten)
		return linux_bootargs;

	free(linux_bootargs);

	bootargs = globalvar_get_match("linux.bootargs.", " ");
	if (!strlen(bootargs))
		return getenv("bootargs");

	mtdparts = globalvar_get_match("linux.mtdparts.", ";");

	if (strlen(mtdparts)) {
		linux_bootargs = asprintf("%s mtdparts=%s", bootargs, mtdparts);
		free(bootargs);
		free(mtdparts);
	} else {
		free(mtdparts);
		linux_bootargs = bootargs;
	}

	return linux_bootargs;
}

int linux_bootargs_overwrite(const char *bootargs)
{
	if (bootargs) {
		free(linux_bootargs);
		linux_bootargs = xstrdup(bootargs);
		linux_bootargs_overwritten = 1;
	} else {
		linux_bootargs_overwritten = 0;
	}

	return 0;
}

BAREBOX_MAGICVAR_NAMED(global_linux_bootargs_, global.linux.bootargs.*, "Linux bootargs variables");
BAREBOX_MAGICVAR_NAMED(global_linux_mtdparts_, global.linux.mtdparts.*, "Linux mtdparts variables");
