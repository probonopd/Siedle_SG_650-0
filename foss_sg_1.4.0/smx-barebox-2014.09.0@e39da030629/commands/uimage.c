#include <common.h>
#include <command.h>
#include <image.h>
#include <libbb.h>
#include <fcntl.h>
#include <fs.h>
#include <malloc.h>
#include <errno.h>
#include <getopt.h>
#include <libfile.h>

static int uimage_fd;

static int uimage_flush(void *buf, unsigned int len)
{
	int ret;

	ret = write_full(uimage_fd, buf, len);

	return ret;
}

static int do_uimage(int argc, char *argv[])
{
	struct uimage_handle *handle;
	int ret;
	int verify = 0;
	int fd;
	int opt;
	char *extract = NULL;
	int info = 0;
	int image_no = 0;

	while ((opt = getopt(argc, argv, "ve:in:")) > 0) {
		switch (opt) {
		case 'v':
			verify = 1;
			break;
		case 'i':
			info = 1;
			break;
		case 'e':
			extract = optarg;
			break;
		case 'n':
			image_no = simple_strtoul(optarg, NULL, 0);
			break;
		}
	}

	if (optind == argc)
		return COMMAND_ERROR_USAGE;

	handle = uimage_open(argv[optind]);
	if (!handle)
		return 1;

	if (info) {
		printf("Image at %s:\n", argv[optind]);
		uimage_print_contents(handle);
	}

	if (verify) {
		printf("verifying data CRC... ");
		ret = uimage_verify(handle);
		if (ret)
			goto err;
		printf("ok\n");
	}

	if (extract) {
		fd = open(extract, O_WRONLY | O_CREAT | O_TRUNC);
		if (fd < 0) {
			perror("open");
			ret = fd;
			goto err;
		}
		uimage_fd = fd;
		ret = uimage_load(handle, image_no, uimage_flush);
		if (ret) {
			printf("loading uImage failed with %d\n", ret);
			close(fd);
			goto err;
		}

		close(fd);
	}
err:
	uimage_close(handle);

	return ret ? 1 : 0;
}

BAREBOX_CMD_HELP_START(uimage)
BAREBOX_CMD_HELP_TEXT("Options:")
BAREBOX_CMD_HELP_OPT ("-i\t",  "show information about image")
BAREBOX_CMD_HELP_OPT ("-v\t",  "verify image")
BAREBOX_CMD_HELP_OPT ("-e OUTFILE",  "extract image to OUTFILE")
BAREBOX_CMD_HELP_OPT ("-n NO\t",  "use image number NO in multifile image")
BAREBOX_CMD_HELP_END

BAREBOX_CMD_START(uimage)
	.cmd		= do_uimage,
	BAREBOX_CMD_DESC("extract/verify uImage")
	BAREBOX_CMD_OPTS("[-vien] FILE")
	BAREBOX_CMD_GROUP(CMD_GRP_BOOT)
	BAREBOX_CMD_HELP(cmd_uimage_help)
BAREBOX_CMD_END
