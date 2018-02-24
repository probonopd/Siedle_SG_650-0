#include <common.h>
#include <command.h>
#include <errno.h>
#include <malloc.h>
#include <getopt.h>
#include <fb.h>
#include <gui/image_renderer.h>
#include <gui/graphic_utils.h>

static int do_splash(int argc, char *argv[])
{
	struct surface s;
	struct screen sc;
	int ret, opt, fd;
	char *fbdev = "/dev/fb0";
	char *image_file;
	int offscreen = 0;
	u32 bg_color = 0x00000000;
	bool do_bg = false;

	memset(&s, 0, sizeof(s));
	memset(&sc, 0, sizeof(sc));

	s.x = -1;
	s.y = -1;
	s.width = -1;
	s.height = -1;

	while((opt = getopt(argc, argv, "f:x:y:ob:")) > 0) {
		switch(opt) {
		case 'f':
			fbdev = optarg;
			break;
		case 'b':
			bg_color = simple_strtoul(optarg, NULL, 0);
			do_bg = true;
			break;
		case 'x':
			s.x = simple_strtoul(optarg, NULL, 0);
			break;
		case 'y':
			s.y = simple_strtoul(optarg, NULL, 0);
		case 'o':
			offscreen = 1;
		}
	}

	if (optind == argc) {
		printf("no filename given\n");
		return 1;
	}
	image_file = argv[optind];

	fd = fb_open(fbdev, &sc, offscreen);
	if (fd < 0) {
		perror("fd_open");
		return 1;
	}

	if (sc.offscreenbuf) {
		if (do_bg)
			memset_pixel(&sc.info, sc.offscreenbuf, bg_color,
					sc.s.width * sc.s.height);
		else
			memcpy(sc.offscreenbuf, sc.fb, sc.fbsize);
	} else if (do_bg) {
		memset_pixel(&sc.info, sc.fb, bg_color, sc.s.width * sc.s.height);
	}

	if (image_renderer_file(&sc, &s, image_file) < 0)
		ret = 1;

	screen_blit(&sc);

	fb_close(&sc);

	return ret;
}

BAREBOX_CMD_HELP_START(splash)
BAREBOX_CMD_HELP_TEXT("This command displays a graphics image of either bitmap (.bmp) format")
BAREBOX_CMD_HELP_TEXT("or Portable Network Graphics (.png) format on the framebuffer.")
BAREBOX_CMD_HELP_TEXT("Currently images with 8 and 24 bit color depth are supported.")
BAREBOX_CMD_HELP_TEXT("")
BAREBOX_CMD_HELP_TEXT("Options:")
BAREBOX_CMD_HELP_OPT ("-f FB\t",    "framebuffer device (default /dev/fb0)")
BAREBOX_CMD_HELP_OPT ("-x XOFFS", "x offset (default center)")
BAREBOX_CMD_HELP_OPT ("-y YOFFS", "y offset (default center)")
BAREBOX_CMD_HELP_OPT ("-b COLOR", "background color in 0xttrrggbb")
BAREBOX_CMD_HELP_OPT ("-o\t",       "render offscreen")
BAREBOX_CMD_HELP_END

BAREBOX_CMD_START(splash)
	.cmd		= do_splash,
	BAREBOX_CMD_DESC("display a BMP or PNG splash image")
	BAREBOX_CMD_OPTS("[-fxyno] FILE")
	BAREBOX_CMD_GROUP(CMD_GRP_CONSOLE)
	BAREBOX_CMD_HELP(cmd_splash_help)
BAREBOX_CMD_END
