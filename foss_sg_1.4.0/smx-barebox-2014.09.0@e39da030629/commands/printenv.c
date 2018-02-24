/*
 * Copyright (c) 2007 Sascha Hauer <s.hauer@pengutronix.de>, Pengutronix
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
#include <errno.h>
#include <environment.h>

static int do_printenv(int argc, char *argv[])
{
	struct variable_d *v;
	struct env_context *c;

	if (argc == 2) {
		const char *val = getenv(argv[1]);
		if (val) {
			printf("%s=%s\n", argv[1], val);
			return 0;
		}
		printf("## Error: \"%s\" not defined\n", argv[1]);
		return 1;
	}

	c = get_current_context();
	printf("locals:\n");
	list_for_each_entry(v, &c->local, list)
		printf("%s=%s\n", var_name(v), var_val(v));

	printf("globals:\n");
	c = get_current_context();
	while (c) {
		list_for_each_entry(v, &c->global, list)
			printf("%s=%s\n", var_name(v), var_val(v));
		c = c->parent;
	}

	return 0;
}

BAREBOX_CMD_HELP_START(printenv)
BAREBOX_CMD_HELP_TEXT("If an argument is given, printenv prints the content of an environment")
BAREBOX_CMD_HELP_TEXT("variable to the terminal. If no argument is specified, all variables are")
BAREBOX_CMD_HELP_TEXT("printed.")
BAREBOX_CMD_HELP_END

BAREBOX_CMD_START(printenv)
	.cmd		= do_printenv,
	BAREBOX_CMD_DESC("print value of environment variables")
	BAREBOX_CMD_OPTS("[VARIABLE]")
	BAREBOX_CMD_GROUP(CMD_GRP_ENV)
	BAREBOX_CMD_HELP(cmd_printenv_help)
BAREBOX_CMD_END
