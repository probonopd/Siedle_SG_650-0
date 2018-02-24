/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <stdio.h>
#include <config.h>

#include <fuse/cuse_lowlevel.h>
#include <fuse/fuse_opt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <libdrm/drm.h>
#include <libdrm/drm_mode.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

#include "drm_fourcc_rgb_a.h"
#include "kmsfb.h"

static void fbdev_open(fuse_req_t req, struct fuse_file_info *fi)
{
	struct kms_fb *fb = fuse_req_userdata(req);

	fb->pos = 0;

	fuse_reply_open(req, fi);
}

static void fbdev_read(fuse_req_t req, size_t size, off_t off,
			 struct fuse_file_info *fi)
{
	struct kms_fb *fb = fuse_req_userdata(req);
	int start;
	void *buf;

	start = fb->pos;

	if (start >= fb->fb_fix.smem_len)
		size = 0;
	else if (start + size > fb->fb_fix.smem_len)
		size = fb->fb_fix.smem_len - start;

	buf = malloc(size);
	memcpy(buf, fb->fb + start, size);
	fuse_reply_buf(req, buf, size);
	free(buf);
	fb->pos += size;
}

static void fbdev_write(fuse_req_t req, const char *buf, size_t size,
			  off_t off, struct fuse_file_info *fi)
{
	struct kms_fb *fb = fuse_req_userdata(req);
	int start;

	start = fb->pos;

	if (start >= fb->fb_fix.smem_len)
		size = 0;
	else if (start + size > fb->fb_fix.smem_len)
		size = fb->fb_fix.smem_len - start;

	if (size == 0) {
		fuse_reply_err(req, ENOSPC);
		return;
	}

	memcpy(fb->fb + start, buf, size);

	fuse_reply_write(req, size);
	fb->pos += size;
}

static int wait_vblank_sync(struct kms_fb *fb)
{
	union drm_wait_vblank vblank = {};
	vblank.request.type = _DRM_VBLANK_RELATIVE;
	vblank.request.sequence = 1;
	return ioctl(drmfd, DRM_IOCTL_WAIT_VBLANK, &vblank);
}

static int wait_vblank(struct kms_fb *fb)
{
	char buffer[1024];
	int len, i, ret;
	struct drm_event *e;
	struct timeval timeout = { .tv_sec = 0, .tv_usec = 200000 };
	fd_set fds;

	if (!fb->can_wait)
		return 0;

	fb->can_wait = 0;

	FD_ZERO(&fds);
	FD_SET(drmfd, &fds);
	ret = select(drmfd + 1, &fds, NULL, NULL, &timeout);

	if (ret <= 0) {
		pr_err("select timed out or error\n");
		return -EINVAL;
	}

	len = read(drmfd, buffer, sizeof buffer);
	if (len == 0)
		return -EINVAL;
	if (len < sizeof *e)
		return -EINVAL;

	i = 0;
	while (i < len) {
		e = (struct drm_event *) &buffer[i];
		switch (e->type) {
		case DRM_EVENT_VBLANK:
			pr_debug("vblank\n");
			return 0;
			break;
		case DRM_EVENT_FLIP_COMPLETE:
			pr_debug("flip complete\n");
			return 0;
			break;
		default:
			break;
		}
		i += e->length;
	}

	return -EINVAL;
}

#define PICOSECONDS_PER_MILLISECOND	1000000000ULL

static void fb_var_screeninfo_to_drm_mode(const struct fb_var_screeninfo *fb_var,
					  struct drm_mode_modeinfo *mode)
{
	memset(mode, 0, sizeof(*mode));

	/* Convert pixclock in ps into clock in kHz */
	mode->clock = PICOSECONDS_PER_MILLISECOND / fb_var->pixclock;
	mode->hdisplay = fb_var->xres;
	mode->vdisplay = fb_var->yres;
	mode->hsync_start = mode->hdisplay + fb_var->right_margin;
	mode->vsync_start = mode->vdisplay + fb_var->lower_margin;
	mode->hsync_end = mode->hsync_start + fb_var->hsync_len;
	mode->vsync_end = mode->vsync_start + fb_var->vsync_len;
	mode->htotal = mode->hsync_end + fb_var->left_margin;
	mode->vtotal = mode->vsync_end + fb_var->upper_margin;
	mode->vrefresh = mode->clock * 1000 / (mode->htotal * mode->vtotal);

	if (fb_var->sync & FB_SYNC_HOR_HIGH_ACT)
		mode->flags |= DRM_MODE_FLAG_PHSYNC;
	else
		mode->flags |= DRM_MODE_FLAG_NHSYNC;
	if (fb_var->sync & FB_SYNC_VERT_HIGH_ACT)
		mode->flags |= DRM_MODE_FLAG_PVSYNC;
	else
		mode->flags |= DRM_MODE_FLAG_NVSYNC;
}

static void drm_mode_to_fb_var_screeninfo(const struct drm_mode_modeinfo *mode,
					  struct fb_var_screeninfo *fb_var)
{
	/* Convert clock in kHz into pixclock in ps */
	fb_var->pixclock = PICOSECONDS_PER_MILLISECOND / mode->clock;
	fb_var->xres = mode->hdisplay;
	fb_var->yres = mode->vdisplay;
	fb_var->left_margin = mode->htotal - mode->hsync_end;
	fb_var->right_margin = mode->hsync_start - mode->hdisplay;
	fb_var->upper_margin = mode->vtotal - mode->vsync_end;
	fb_var->lower_margin = mode->vsync_start - mode->vdisplay;
	fb_var->hsync_len = mode->hsync_end - mode->hsync_start;
	fb_var->vsync_len = mode->vsync_end - mode->vsync_start;

	if (mode->flags & DRM_MODE_FLAG_PHSYNC)
		fb_var->sync |= FB_SYNC_HOR_HIGH_ACT;
	if (mode->flags & DRM_MODE_FLAG_NHSYNC)
		fb_var->sync &= ~FB_SYNC_HOR_HIGH_ACT;
	if (mode->flags & DRM_MODE_FLAG_PVSYNC)
		fb_var->sync |= FB_SYNC_VERT_HIGH_ACT;
	if (mode->flags & DRM_MODE_FLAG_NVSYNC)
		fb_var->sync &= ~FB_SYNC_VERT_HIGH_ACT;
}

static int do_set_mode(struct kms_fb *fb, const struct fb_var_screeninfo *v)
{
	struct drm_mode_modeinfo mode;
	int ret;

	if (fb->xres != v->xres || fb->yres != v->yres) {
		pr_err("can't change framebuffer size\n");
		return -EINVAL;
	}

	fb_var_screeninfo_to_drm_mode(v, &mode);

	ret = kms_fb_set_mode(fb, &mode);
	if (ret < 0) {
		pr_err("failed to set mode %dx%d@%d\n",
		       mode.hdisplay, mode.vdisplay, mode.vrefresh);
		return ret;
	}

	drm_mode_to_fb_var_screeninfo(&mode, &fb->fb_var);

	return 0;
}

static void do_pageflip(struct kms_fb *fb, int offset)
{
	struct drm_mode_crtc_page_flip flip = {};
	struct drm_mode_set_plane set_plane = {};
	struct kms_slice *slice;
	int ret, i;

	if (offset == 0 || offset < 0)
		slice = &fb->slices[0];
	else if (offset == fb->yres)
		slice = &fb->slices[1];
	else if (offset == 2 * fb->yres)
		slice = &fb->slices[2];
	else {
		pr_err("unexpected offset: %d\n", offset);
		return;
	}

	if (fb->vsync.e == ALWAYS)
		wait_vblank(fb);

	for (i = 0; i < slice->num_windows; i++) {
		if (slice->windows[i].plane_id == 0) {
			flip.fb_id = offset < 0 ? fb->splash_fb_id : slice->fb_id;
			flip.crtc_id = slice->windows[i].crtc_id;
			flip.flags = fb->can_wait ? 0 : DRM_MODE_PAGE_FLIP_EVENT;
			flip.reserved = 0;

			ret = ioctl(drmfd, DRM_IOCTL_MODE_PAGE_FLIP, &flip);
			if ((fb->vsync.e != NEVER) && ret) {
				pr_err("page flip failed: %s\n", strerror(errno));
				return;
			}
		} else {
			union drm_wait_vblank vblank = {};

			set_plane.plane_id = slice->windows[i].plane_id;
			set_plane.crtc_id = slice->windows[i].crtc_id;
			set_plane.fb_id = offset < 0 ? fb->splash_fb_id : slice->fb_id;
			set_plane.flags = fb->can_wait ? 0 : DRM_MODE_PAGE_FLIP_EVENT;
			set_plane.crtc_x = slice->windows[i].xpos;
			set_plane.crtc_y = slice->windows[i].ypos;
			set_plane.crtc_w = slice->windows[i].xres;
			set_plane.crtc_h = slice->windows[i].yres;
			set_plane.src_x = slice->windows[i].xofs << 16;
			set_plane.src_y = slice->windows[i].yofs << 16;
			set_plane.src_w = slice->windows[i].xres << 16;
			set_plane.src_h = slice->windows[i].yres << 16;

			ret = ioctl(drmfd, DRM_IOCTL_MODE_SETPLANE, &set_plane);
			if (ret) {
				pr_err("plane flip failed: %s\n", strerror(errno));
				return;
			}

			vblank.request.type = _DRM_VBLANK_RELATIVE | _DRM_VBLANK_EVENT;
			ret = ioctl(drmfd, DRM_IOCTL_WAIT_VBLANK, &vblank);
			if ((fb->vsync.e != NEVER) && ret) {
				pr_err("vblank request failed: %s\n", strerror(errno));
				return;
			}
		}
	}

	fb->can_wait = 1;
}

static void fbdev_ioctl(fuse_req_t req, int cmd, void *arg,
			  struct fuse_file_info *fi, unsigned flags,
			  const void *in_buf, size_t in_bufsz, size_t out_bufsz)
{
	struct kms_fb *fb = fuse_req_userdata(req);
	int ret;

	switch (cmd) {
	case FBIOGET_VSCREENINFO:
		if (!out_bufsz) {
			struct iovec iov = { arg, sizeof(fb->fb_var) };

			fuse_reply_ioctl_retry(req, NULL, 0, &iov, 1);
		} else {
			fuse_reply_ioctl(req, 0, &fb->fb_var,
					 sizeof(fb->fb_var));
		}
		break;
	case FBIOPUT_VSCREENINFO:
		if (!in_bufsz || !out_bufsz) {
			struct iovec iov = { arg, sizeof(fb->fb_var) };

			fuse_reply_ioctl_retry(req, &iov, 1, &iov, 1);
		} else {
			const struct fb_var_screeninfo *v = in_buf;

			if (!fb->con) {
				fuse_reply_ioctl(req, -EINVAL, &fb->fb_var,
						 sizeof(fb->fb_var));
				break;
			}

			ret = do_set_mode(fb, v);
			fuse_reply_ioctl(req, ret, &fb->fb_var,
					 sizeof(fb->fb_var));
		}
		break;
	case FBIOPAN_DISPLAY:
		if (!in_bufsz) {
			struct iovec iov = { arg, sizeof(fb->fb_var) };

			fuse_reply_ioctl_retry(req, &iov, 1, NULL, 0);
		} else {
			const struct fb_var_screeninfo *v = in_buf;

			do_pageflip(fb, v->yoffset);
			fuse_reply_ioctl(req, 0, NULL, 0);
		}
		break;
	case FBIO_WAITFORVSYNC:
		if (fb->vsync.e != NEVER)
			wait_vblank_sync(fb);
		fuse_reply_ioctl(req, 0, NULL, 0);
		break;
	case FBIOGET_FSCREENINFO:
		if (!out_bufsz) {
			struct iovec iov = { arg, sizeof(fb->fb_fix) };

			fuse_reply_ioctl_retry(req, NULL, 0, &iov, 1);
		} else {
			fuse_reply_ioctl(req, 0, &fb->fb_fix,
					 sizeof(fb->fb_fix));
		}
		break;
	default:
		fuse_reply_err(req, EINVAL);
		break;
	}
}

static void fbdev_release(fuse_req_t req, struct fuse_file_info *fi)
{
	do_pageflip(fuse_req_userdata(req), -1);
	fuse_reply_err(req, 0);
}

static const struct cuse_lowlevel_ops fbdev_clop = {
	.open		= fbdev_open,
	.read		= fbdev_read,
	.write		= fbdev_write,
	.ioctl		= fbdev_ioctl,
	.release	= fbdev_release,
};

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

static void *bgi_thread(void *arg)
{
	struct kms_fb *fb = arg;
	char *dev_info_argv[2];
	char *argv[] = { "prgname", "-f", "-s" };
	struct cuse_info ci;
	int ret;

	ret = asprintf(&dev_info_argv[1], "MMAPFD=%d", fb->fd);
	if (ret < 0)
		return NULL;
	ret = asprintf(&dev_info_argv[0], "DEVNAME=fb%d", fb->id);
	if (ret < 0)
		return NULL;

	memset(&ci, 0, sizeof(ci));
	ci.dev_major = 0;
	ci.dev_minor = fb->id;
	ci.dev_info_argc = 2;
	ci.dev_info_argv = (const char **)dev_info_argv;
	ci.flags = CUSE_UNRESTRICTED_IOCTL;

	ret = cuse_lowlevel_main(ARRAY_SIZE(argv), &argv[0], &ci, &fbdev_clop, fb);

	return NULL;
}

int bgi_init(struct kms_fb *fb)
{
	int ret;
	pthread_t threadid;
	int yres;
	fb->can_wait = 0;

	switch (fb->format) {
	case DRM_FORMAT_RGB565_A8:
	case DRM_FORMAT_BGR565_A8:
		yres = fb->yres + (fb->yres + 1) / 2;
		break;
	case DRM_FORMAT_RGB888_A8:
	case DRM_FORMAT_BGR888_A8:
		yres = fb->yres + (fb->yres + 2) / 3;
		break;
	case DRM_FORMAT_RGBX8888_A8:
	case DRM_FORMAT_BGRX8888_A8:
		yres = fb->yres + (fb->yres + 3) / 4;
		break;
	default:
		yres = fb->yres;
		break;
	}

	fb->fb_fix.smem_start = fb->phys;
	fb->fb_fix.smem_len = fb->pitch * yres * fb->num_slices;
	fb->fb_fix.line_length = fb->pitch;
	fb->fb_fix.visual = 2;
	fb->fb_fix.ypanstep = 1;
	fb->fb_fix.ywrapstep = 1;

	fb->fb_var.height = 0xffffffff;
	fb->fb_var.width = 0xffffffff;
	fb->fb_var.xres = fb->xres;
	fb->fb_var.yres = yres;
	fb->fb_var.xres_virtual = fb->xres;
	fb->fb_var.yres_virtual = yres * fb->num_slices;
	fb->fb_var.bits_per_pixel = fb->bpp;

	switch (fb->format) {
	case DRM_FORMAT_RGB565:
	case DRM_FORMAT_RGB565_A8:
		fb->fb_var.transp.offset = 0;
		fb->fb_var.transp.length = 0;
		fb->fb_var.red.offset = 11;
		fb->fb_var.red.length = 5;
		fb->fb_var.green.offset = 5;
		fb->fb_var.green.length = 6;
		fb->fb_var.blue.offset = 0;
		fb->fb_var.blue.length = 5;
		break;
	case DRM_FORMAT_BGR565:
	case DRM_FORMAT_BGR565_A8:
		fb->fb_var.transp.offset = 0;
		fb->fb_var.transp.length = 0;
		fb->fb_var.red.offset = 0;
		fb->fb_var.red.length = 5;
		fb->fb_var.green.offset = 5;
		fb->fb_var.green.length = 6;
		fb->fb_var.blue.offset = 11;
		fb->fb_var.blue.length = 5;
		break;
	case DRM_FORMAT_ARGB1555:
		fb->fb_var.transp.offset = 15;
		fb->fb_var.transp.length = 1;
		fb->fb_var.red.offset = 10;
		fb->fb_var.red.length = 5;
		fb->fb_var.green.offset = 5;
		fb->fb_var.green.length = 5;
		fb->fb_var.blue.offset = 0;
		fb->fb_var.blue.length = 5;
		break;
	case DRM_FORMAT_ARGB4444:
		fb->fb_var.transp.offset = 12;
		fb->fb_var.transp.length = 4;
		fb->fb_var.red.offset = 8;
		fb->fb_var.red.length = 4;
		fb->fb_var.green.offset = 4;
		fb->fb_var.green.length = 4;
		fb->fb_var.blue.offset = 0;
		fb->fb_var.blue.length = 4;
		break;
	case DRM_FORMAT_RGB888:
	case DRM_FORMAT_RGB888_A8:
		fb->fb_var.transp.offset = 0;
		fb->fb_var.transp.length = 0;
		fb->fb_var.red.offset = 0;
		fb->fb_var.red.length = 8;
		fb->fb_var.green.offset = 8;
		fb->fb_var.green.length = 8;
		fb->fb_var.blue.offset = 16;
		fb->fb_var.blue.length = 8;
		break;
	case DRM_FORMAT_BGR888:
	case DRM_FORMAT_BGR888_A8:
		fb->fb_var.transp.offset = 0;
		fb->fb_var.transp.length = 0;
		fb->fb_var.red.offset = 16;
		fb->fb_var.red.length = 8;
		fb->fb_var.green.offset = 8;
		fb->fb_var.green.length = 8;
		fb->fb_var.blue.offset = 0;
		fb->fb_var.blue.length = 8;
		break;
	case DRM_FORMAT_XRGB8888:
	case DRM_FORMAT_ARGB8888:
		fb->fb_var.red.offset = 16;
		fb->fb_var.red.length = 8;
		fb->fb_var.green.offset = 8;
		fb->fb_var.green.length = 8;
		fb->fb_var.blue.offset = 0;
		fb->fb_var.blue.length = 8;
		fb->fb_var.transp.offset = 24;
		fb->fb_var.transp.length = 8;
		break;
	case DRM_FORMAT_XBGR8888:
	case DRM_FORMAT_ABGR8888:
		fb->fb_var.red.offset = 0;
		fb->fb_var.red.length = 8;
		fb->fb_var.green.offset = 8;
		fb->fb_var.green.length = 8;
		fb->fb_var.blue.offset = 16;
		fb->fb_var.blue.length = 8;
		fb->fb_var.transp.offset = 24;
		fb->fb_var.transp.length = 8;
		break;
	case DRM_FORMAT_RGBA8888:
	case DRM_FORMAT_RGBX8888:
	case DRM_FORMAT_RGBX8888_A8:
		fb->fb_var.red.offset = 24;
		fb->fb_var.red.length = 8;
		fb->fb_var.green.offset = 16;
		fb->fb_var.green.length = 8;
		fb->fb_var.blue.offset = 8;
		fb->fb_var.blue.length = 8;
		fb->fb_var.transp.offset = 0;
		fb->fb_var.transp.length = 8;
		break;
	case DRM_FORMAT_BGRA8888:
	case DRM_FORMAT_BGRX8888:
	case DRM_FORMAT_BGRX8888_A8:
		fb->fb_var.red.offset = 8;
		fb->fb_var.red.length = 8;
		fb->fb_var.green.offset = 16;
		fb->fb_var.green.length = 8;
		fb->fb_var.blue.offset = 24;
		fb->fb_var.blue.length = 8;
		fb->fb_var.transp.offset = 0;
		fb->fb_var.transp.length = 8;
		break;
	default:
		printf("unsupported BGI format: '%4.4s\n", (char *)&fb->format);
		break;
	}

	ret = pthread_create(&threadid, NULL, bgi_thread, fb);
	return ret;
}

int bgi_set_connector_crtc_mode(struct kms_fb *fb, struct drm_connector *con,
				uint32_t crtc, struct drm_mode_modeinfo *mode)
{
	if (mode) {
		if (fb->xres != mode->hdisplay || fb->yres != mode->vdisplay) {
			printf("can't set connector on non-fullscreen FB\n");
			return -1;
		}

		drm_mode_to_fb_var_screeninfo(mode, &fb->fb_var);
	} else {
		fb->fb_var.pixclock = 0;
		fb->fb_var.left_margin = 0;
		fb->fb_var.right_margin = 0;
		fb->fb_var.upper_margin = 0;
		fb->fb_var.lower_margin = 0;
		fb->fb_var.hsync_len = 0;
		fb->fb_var.vsync_len = 0;
		fb->fb_var.sync = 0;
	}

	fb->crtc = crtc;
	fb->con = con;

	return 0;
}
