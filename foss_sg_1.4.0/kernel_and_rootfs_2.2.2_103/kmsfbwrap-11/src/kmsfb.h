#ifndef __KMSFB_H
#define __KMSFB_H

#include <stdlib.h>
#include <stdint.h>
#include <linux/fb.h>

#define pr_err(fmt, arg...)	fprintf(stderr, fmt, ##arg)
#define pr_debug(fmt, arg...)					\
		do {						\
			if (debug)				\
				fprintf(stderr, fmt, ##arg);	\
		} while (0);

struct drm_connector;
struct drm_mode_modeinfo;

extern int drmfd;
extern int debug;

static inline void *zalloc(size_t size)
{
	return calloc(1, size);
}

#define MAX_KMS_FBS 8
#define MAX_KMS_WINDOWS 8
#define MAX_FILENAME 256

struct kms_window {
	uint32_t crtc_id;
	uint32_t plane_id;
	uint32_t xofs;
	uint32_t yofs;
	uint32_t xres;
	uint32_t yres;
	int32_t xpos;
	int32_t ypos;
};

struct kms_slice {
	int num_windows;
	struct kms_window windows[MAX_KMS_WINDOWS];

	uint32_t fb_id;
};

struct kms_fb {
	int id;

	uint32_t xres, yres, pitch, splash_pitch;
	uint32_t format;
	uint32_t bpp;

	int num_slices;
	struct kms_slice slices[3];

	uint32_t gem_name, splash_gem_name;
	unsigned long phys;
	int fd;
	void *fb, *splash_fb;
	uint32_t splash_fb_id;

	union {
		int val;
		enum {
			DEFAULT		= 0,
			NEVER		= 1,
			ALWAYS		= 2,
		} e;
	} vsync;
	int can_wait;

	struct fb_fix_screeninfo fb_fix;
	struct fb_var_screeninfo fb_var;
	unsigned long pos;

	struct drm_connector *con;
	uint32_t crtc;
};

struct kms_data {
	uint32_t xres, yres, pitch;
	const char *device;

	uint32_t xofs, yofs;
	uint32_t xpos, ypos;
	uint32_t bpp;
	uint32_t format;

	uint32_t crtc;
	uint32_t plane;
	uint32_t connector;
	uint32_t encoder;
	struct kms_fb *kms_fb;

	int triplebuf;
	int fill;
	char file[MAX_FILENAME];
	int vsync;
	int alpha;
	int fps;
};

struct splash {
	int width, height;
	unsigned char *rgb;
	unsigned char *alpha;
};

int png_load(char *name, struct splash *splash, unsigned char **alpha);

int bgi_init(struct kms_fb *fb);
int bgi_set_connector_crtc_mode(struct kms_fb *fb, struct drm_connector *con,
				uint32_t crtc, struct drm_mode_modeinfo *mode);

int kms_fb_set_mode(struct kms_fb *fb, struct drm_mode_modeinfo *mode);

#endif /* __KMSFB_H */
