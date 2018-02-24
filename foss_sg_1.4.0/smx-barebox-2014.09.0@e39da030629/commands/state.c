/*
 * Copyright (C) 2012 Jan Luebbe <j.luebbe@pengutronix.de>
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
#include <getopt.h>
#include <command.h>
#include <state.h>

static int do_state(int argc, char *argv[])
{
	int opt, ret = 0;
	struct state *state = NULL;
	int do_save = 0, do_load = 0;
	const char *statename = "state";

	while ((opt = getopt(argc, argv, "sl")) > 0) {
		switch (opt) {
		case 's':
			do_save = 1;
			break;
		case 'l':
			do_load = 1;
			break;
		default:
			return COMMAND_ERROR_USAGE;
		}
	}

	if (!do_save && !do_load) {
		state_info();
		return 0;
	}

	if (optind < argc)
		statename = argv[optind];

	state = state_by_name(statename);
	if (!state) {
		printf("cannot find state %s\n", statename);
		return -ENOENT;
	}

	if (do_save) {
		if (!state) {
			printf("no statename given\n");
			return -ENOENT;
		}

		ret = state_save(state);
	}

	if (do_load) {
		if (!state) {
			printf("no statename given\n");
			return -ENOENT;
		}

		ret = state_load(state);
	}

	return ret;
}

BAREBOX_CMD_HELP_START(state)
BAREBOX_CMD_HELP_TEXT("Handle state information")
BAREBOX_CMD_HELP_TEXT("")
BAREBOX_CMD_HELP_TEXT("Options:")
BAREBOX_CMD_HELP_OPT ("-s", "save state")
BAREBOX_CMD_HELP_OPT ("-l", "load state")
BAREBOX_CMD_HELP_END

BAREBOX_CMD_START(state)
	.cmd		= do_state,
	BAREBOX_CMD_DESC("handle state information")
	BAREBOX_CMD_OPTS("[-sl] [STATENAME]")
	BAREBOX_CMD_GROUP(CMD_GRP_MISC)
	BAREBOX_CMD_HELP(cmd_state_help)
BAREBOX_CMD_END
