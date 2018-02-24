/*
 * ln.c - make links between files
 *
 * Copyright (c) 2012 Jean-Christophe PLAGNIOL-VILLARD <plagnioj@jcrosoft.com>
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
#include <command.h>
#include <libgen.h>
#include <environment.h>
#include <fs.h>
#include <errno.h>

static int do_ln(int argc, char *argv[])
{
	if (argc != 3)
		return COMMAND_ERROR_USAGE;

	if (symlink(argv[1], argv[2]) < 0) {
		perror("ln");
		return 1;
	}
	return 0;
}

BAREBOX_CMD_START(ln)
	.cmd		= do_ln,
	BAREBOX_CMD_DESC("create symlink (make a new name for a file)")
	BAREBOX_CMD_OPTS("SRC DEST")
	BAREBOX_CMD_GROUP(CMD_GRP_FILE)
BAREBOX_CMD_END
