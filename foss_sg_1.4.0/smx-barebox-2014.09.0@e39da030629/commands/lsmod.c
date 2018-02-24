#include <common.h>
#include <command.h>
#include <complete.h>
#include <module.h>

static int do_lsmod(int argc, char *argv[])
{
	struct module *mod;

	for_each_module(mod)
		printf("%s\n", mod->name);

	return 0;
}

BAREBOX_CMD_START(lsmod)
	.cmd		= do_lsmod,
	BAREBOX_CMD_DESC("list loaded barebox modules")
	BAREBOX_CMD_GROUP(CMD_GRP_MISC)
	BAREBOX_CMD_COMPLETE(empty_complete)
BAREBOX_CMD_END
