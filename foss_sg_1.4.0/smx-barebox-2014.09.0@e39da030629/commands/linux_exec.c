/*
 * Copyright (c) 2011 Jean-Christophe PLAGNIOL-VILLARD <plagnioj@jcrosoft.com>
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
#include <malloc.h>
#include <command.h>
#include <mach/linux.h>

static int do_linux_exec(int argc, char *argv[])
{
	int ret;
	char **newargv;
	char *newenv[] = { NULL };
	int i, j;

	if (argc < 2)
		return 1;

	newargv = xzalloc(sizeof(char*) * argc);

	for (j = 0, i = 1; i < argc; i++, j++)
		newargv[j] = argv[i];

	newargv[j] = NULL;
	ret = linux_execve(argv[1], newargv, newenv);

	free(newargv);

	if (ret)
		return 1;

	return 0;
}

BAREBOX_CMD_START(linux_exec)
	.cmd		= do_linux_exec,
	BAREBOX_CMD_DESC("execute a command on the host")
	BAREBOX_CMD_OPTS("COMMAND")
	BAREBOX_CMD_GROUP(CMD_GRP_MISC)
BAREBOX_CMD_END
