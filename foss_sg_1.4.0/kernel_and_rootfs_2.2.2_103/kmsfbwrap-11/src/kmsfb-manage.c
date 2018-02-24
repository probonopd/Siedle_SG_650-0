/*
 * kmsfb-manage - Tool to create framebuffers on a drm device which can then
 *                be used by applications using libkmsfbwrap.
 *
 * Copyright (C) 2012 Sascha Hauer <s.hauer@pengutronix.de>, Pengutronix
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include <stdint.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <drm.h>
#include <drm_fourcc.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/mman.h>
#include <inttypes.h>

#include <xf86drm.h>
#include <xf86drmMode.h>

#include "drm_fourcc_rgb_a.h"
#include "kmsfb.h"

int debug;
int quiet;

#define print(fmt, ...) \
	do { \
		if (!quiet) \
			printf(fmt, ##__VA_ARGS__); \
	} while (0)

struct drm_connector {
	uint32_t id;
	int encoder_id;
	int type;
	struct drm_mode_modeinfo *modes;
	int num_modes;
	uint64_t *prop_values;
	uint32_t *props;
	int count_props;
	int *encoders;
	int num_encoders;
};

struct drm_resource {
	struct drm_mode_crtc *crtcs;
	int num_crtcs;
	struct drm_connector *connectors;
	int num_connectors;
	struct drm_mode_get_encoder *encoders;
	int num_encoders;
	int num_fbs;
	struct drm_mode_fb_cmd *fbs;
	int num_planes;
	struct drm_mode_get_plane *planes;
};

struct drm_property {
	uint32_t prop_id;
	uint32_t flags;
	char name[DRM_PROP_NAME_LEN];
	int count_values;
	uint64_t *values; /* store the blob lengths */
	int count_enums;
	struct drm_mode_property_enum *enums;
	int count_blobs;
	uint32_t *blob_ids; /* store the blob IDs */
};

struct drm_property_blob {
	uint32_t id;
	uint32_t length;
	void *data;
};

int drmfd;

static struct {
	int num_kms_fbs;
	struct kms_fb *kms_fbs[MAX_KMS_FBS];
} globals;

static int kms_data_val_set(struct kms_data *kd, const char *key, const char *value)
{
	if (!strcmp(key, "xres"))
		kd->xres = strtoul(value, NULL, 0);
	else if (!strcmp(key, "yres"))
		kd->yres = strtoul(value, NULL, 0);
	else if (!strcmp(key, "xofs"))
		kd->xofs = strtoul(value, NULL, 0);
	else if (!strcmp(key, "yofs"))
		kd->yofs = strtoul(value, NULL, 0);
	else if (!strcmp(key, "bpp")) {
		if (kd->format) {
			pr_err("bpp and format are mutually exclusive\n");
			return -EINVAL;
		}
		kd->bpp = strtoul(value, NULL, 0);
	} else if (!strcmp(key, "pitch"))
		kd->pitch = strtoul(value, NULL, 0);
	else if (!strcmp(key, "fbid")) {
		char *end;
		int fbref = strtoul(value, &end, 0);
		if (end == value) {
			pr_err("'%s'is not a valid fb no.\n", value);
			return -EINVAL;
		}
		if (fbref >= globals.num_kms_fbs) {
			pr_err("fb no. %d does not exist\n", fbref);
			return -EINVAL;
		}
		kd->kms_fb = globals.kms_fbs[fbref];
		if (!kd->xres)
			kd->xres = globals.kms_fbs[fbref]->xres;
		if (!kd->yres)
			kd->yres = globals.kms_fbs[fbref]->yres;
	} else if (!strcmp(key, "crtc"))
		kd->crtc = strtoul(value, NULL, 0);
	else if (!strcmp(key, "plane"))
		kd->plane = strtoul(value, NULL, 0);
	else  if (!strcmp(key, "connector"))
		kd->connector = strtoul(value, NULL, 0);
	else  if (!strcmp(key, "encoder"))
		kd->encoder = strtoul(value, NULL, 0);
	else if (!strcmp(key, "3d"))
		kd->triplebuf = 1;
	else if (!strcmp(key, "fill"))
		kd->fill = 1;
	else if (!strcmp(key, "file")) {
		if (strlen(value) > MAX_FILENAME) {
			pr_err("Filename '%s' is to long.\n", value);
			return -EINVAL;
		}
		strcpy(kd->file, value);
	}
	else if (!strcmp(key, "vsync"))
		kd->vsync = strtoul(value, NULL, 0);
	else if (!strcmp(key, "xpos"))
		kd->xpos = strtoul(value, NULL, 0);
	else if (!strcmp(key, "ypos"))
		kd->ypos = strtoul(value, NULL, 0);
	else if (!strcmp(key, "alpha"))
		kd->alpha = strtoul(value, NULL, 0);
	else if (!strcmp(key, "fps"))
		kd->fps = strtoul(value, NULL, 0);
	else if (!strcmp(key, "format")) {
		if (kd->bpp) {
			pr_err("format and bpp are mutually exclusive\n");
			return -EINVAL;
		}
		if (!strcasecmp(value, "xrgb8888"))
			kd->format = DRM_FORMAT_XRGB8888;
		else if (!strcasecmp(value, "argb8888"))
			kd->format = DRM_FORMAT_ARGB8888;
		else if (!strcasecmp(value, "xbgr8888"))
			kd->format = DRM_FORMAT_XBGR8888;
		else if (!strcasecmp(value, "abgr8888"))
			kd->format = DRM_FORMAT_ABGR8888;
		else if (!strcasecmp(value, "rgbx8888"))
			kd->format = DRM_FORMAT_RGBX8888;
		else if (!strcasecmp(value, "rgba8888"))
			kd->format = DRM_FORMAT_RGBA8888;
		else if (!strcasecmp(value, "bgrx8888"))
			kd->format = DRM_FORMAT_BGRX8888;
		else if (!strcasecmp(value, "bgra8888"))
			kd->format = DRM_FORMAT_BGRA8888;
		else if (!strcasecmp(value, "rgbx8888_a8"))
			kd->format = DRM_FORMAT_RGBX8888_A8;
		else if (!strcasecmp(value, "bgrx8888_a8"))
			kd->format = DRM_FORMAT_BGRX8888_A8;
		else if (!strcasecmp(value, "rgb888"))
			kd->format = DRM_FORMAT_RGB888;
		else if (!strcasecmp(value, "bgr888"))
			kd->format = DRM_FORMAT_BGR888;
		else if (!strcasecmp(value, "rgb888_a8"))
			kd->format = DRM_FORMAT_RGB888_A8;
		else if (!strcasecmp(value, "bgr888_a8"))
			kd->format = DRM_FORMAT_BGR888_A8;
		else if (!strcasecmp(value, "argb4444"))
			kd->format = DRM_FORMAT_ARGB4444;
		else if (!strcasecmp(value, "rgb565"))
			kd->format = DRM_FORMAT_RGB565;
		else if (!strcasecmp(value, "bgr565"))
			kd->format = DRM_FORMAT_BGR565;
		else if (!strcasecmp(value, "rgb565_a8"))
			kd->format = DRM_FORMAT_RGB565_A8;
		else if (!strcasecmp(value, "bgr565_a8"))
			kd->format = DRM_FORMAT_BGR565_A8;
		else if (!strcasecmp(value, "argb1555"))
			kd->format = DRM_FORMAT_ARGB1555;
		else {
			pr_err("Invalid format: '%s'\n", value);
			return -EINVAL;
		}
	} else
		return -EINVAL;
	return 0;
}

static int parse_key_value(const char *str, struct kms_data *kd)
{
	int done = 0, ret;
	char *tok = strdup(str);
	char *key;
	char *value;

	while (*tok) {
		key = tok;

		while (*tok && *tok != '=')
			tok++;

		if (!*tok)
			return EINVAL;

		*tok = 0;
		tok++;

		value = tok;

		while (*tok && *tok != ',' && *tok != '\n')
			tok++;

		if (!*tok)
			done = 1;

		*tok = 0;
		tok++;

		pr_debug("key: %s value: %s\n", key, value);

		ret = kms_data_val_set(kd, key, value);
		if (ret)
			return ret;

		if (done)
			return 0;
	}

	return 0;
}

static struct drm_connector *drm_find_connector(struct drm_resource *res, int con_id)
{
	int i;

	for (i = 0; i < res->num_connectors; i++)
		if ((!con_id && res->connectors[i].num_modes > 0) || res->connectors[i].id == con_id)
			return &res->connectors[i];
	return NULL;
}

static void drm_find_max_res(struct drm_resource *res, uint32_t *xres, uint32_t *yres)
{
	int i, j;

	for (i = 0; i < res->num_connectors; i++) {
		struct drm_connector *con = &res->connectors[i];
		for (j = 0; j < con->num_modes; j++) {
			struct drm_mode_modeinfo *mode = &con->modes[j];
			if (xres && *xres < mode->hdisplay)
				*xres = mode->hdisplay;
			if (yres && *yres < mode->vdisplay)
				*yres = mode->vdisplay;
		}
	}
}

static struct drm_mode_get_encoder *drm_find_encoder(struct drm_resource *res, uint32_t enc_id)
{
	int i;

	for (i = 0; i < res->num_encoders; i++)
		if (res->encoders[i].encoder_id == enc_id)
			return &res->encoders[i];
	return NULL;
}

static struct drm_mode_modeinfo *drm_find_mode(struct drm_connector *con,
		int xres, int yres, int fps)
{
	int i;

	for (i = 0; i < con->num_modes; i++) {
		struct drm_mode_modeinfo *mode = &con->modes[i];
		if (mode->hdisplay == xres && mode->vdisplay == yres &&
		    (!fps || mode->vrefresh == fps))
			return mode;
	}

	return NULL;
}

static int drm_mode_get_crtc(int fd, int id,
		struct drm_mode_crtc *crtc)
{
	int err;

	crtc->crtc_id = id;

	err = ioctl(fd, DRM_IOCTL_MODE_GETCRTC, crtc);
	if (err) {
		perror("DRM_IOCTL_MODE_GETCRTC");
		return err;
	}

	return 0;
}

#define VOID2U64(x) ((uint64_t)(unsigned long)(x))
#define U642VOID(x) ((void *)(unsigned long)(x))

static int drm_mode_get_encoder(int fd, int id,
		struct drm_mode_get_encoder *enc)
{
	;
	int err;

	memset(enc, 0, sizeof(struct drm_mode_get_encoder));

	enc->encoder_id = id;

	err = ioctl(fd, DRM_IOCTL_MODE_GETENCODER, enc);
	if (err) {
		perror("DRM_IOCTL_MODE_GETENCODER");
		return err;
	}

	return 0;
}

static char *connector_type_strings[] = {
	[DRM_MODE_CONNECTOR_Unknown] = "Unknown",
	[DRM_MODE_CONNECTOR_VGA] = "VGA",
	[DRM_MODE_CONNECTOR_DVII] = "DVI-I",
	[DRM_MODE_CONNECTOR_DVID] = "DVI-D",
	[DRM_MODE_CONNECTOR_DVIA] = "DVI-A",
	[DRM_MODE_CONNECTOR_Composite] = "Composite",
	[DRM_MODE_CONNECTOR_SVIDEO] = "SVIDEO",
	[DRM_MODE_CONNECTOR_LVDS] = "LVDS",
	[DRM_MODE_CONNECTOR_Component] = "Component",
	[DRM_MODE_CONNECTOR_9PinDIN] = "DIN",
	[DRM_MODE_CONNECTOR_DisplayPort] = "DP",
	[DRM_MODE_CONNECTOR_HDMIA] = "HDMI-A",
	[DRM_MODE_CONNECTOR_HDMIB] = "HDMI-B",
	[DRM_MODE_CONNECTOR_TV] = "TV",
	[DRM_MODE_CONNECTOR_eDP] = "eDP",
#ifdef DRM_MODE_CONNECTOR_VIRTUAL
	[DRM_MODE_CONNECTOR_VIRTUAL] = "Virtual",
	[DRM_MODE_CONNECTOR_DSI] = "DSI",
#endif
};

static void *memdup(void *src, size_t size)
{
	void *dest;

	dest = malloc(size);
	if (!dest)
		return NULL;
	memcpy(dest, src, size);

	return dest;
}

static struct drm_property *drm_mode_get_property(int fd, int property_id)
{
	struct drm_mode_get_property prop;
	struct drm_property *property;
	int err;

	prop.prop_id = property_id;
	prop.count_enum_blobs = 0;
	prop.count_values = 0;
	prop.flags = 0;
	prop.enum_blob_ptr = 0;
	prop.values_ptr = 0;

	err = ioctl(fd, DRM_IOCTL_MODE_GETPROPERTY, &prop);
	if (err)
		return NULL;

	if (prop.count_values)
		prop.values_ptr = VOID2U64(malloc(prop.count_values * sizeof(uint64_t)));

	if (prop.count_enum_blobs && (prop.flags & DRM_MODE_PROP_ENUM))
		prop.enum_blob_ptr = VOID2U64(malloc(prop.count_enum_blobs *
					sizeof(struct drm_mode_property_enum)));

	if (prop.count_enum_blobs && (prop.flags & DRM_MODE_PROP_BLOB)) {
		prop.values_ptr = VOID2U64(malloc(prop.count_enum_blobs * sizeof(uint32_t)));
		prop.enum_blob_ptr = VOID2U64(malloc(prop.count_enum_blobs * sizeof(uint32_t)));
	}

	err = ioctl(fd, DRM_IOCTL_MODE_GETPROPERTY, &prop);
	if (err)
		return NULL;

	property = malloc(sizeof(*property));
	if (!property)
		return NULL;

	property->prop_id = prop.prop_id;
	property->count_values = prop.count_values;

	property->flags = prop.flags;
	if (prop.count_values)
		property->values = memdup(U642VOID(prop.values_ptr),
				prop.count_values * sizeof(uint64_t));
	if (prop.flags & DRM_MODE_PROP_ENUM) {
		property->count_enums = prop.count_enum_blobs;
		property->enums = memdup(U642VOID(prop.enum_blob_ptr),
				prop.count_enum_blobs * sizeof(struct drm_mode_property_enum));
	} else if (prop.flags & DRM_MODE_PROP_BLOB) {
		property->values = memdup(U642VOID(prop.values_ptr),
				prop.count_enum_blobs * sizeof(uint32_t));
		property->blob_ids = memdup(U642VOID(prop.enum_blob_ptr),
				prop.count_enum_blobs * sizeof(uint32_t));
		property->count_blobs = prop.count_enum_blobs;
	}
	strncpy(property->name, prop.name, DRM_PROP_NAME_LEN);
	property->name[DRM_PROP_NAME_LEN-1] = 0;

	free(U642VOID(prop.values_ptr));
	free(U642VOID(prop.enum_blob_ptr));

	return property;
}

static int print_property(int fd, struct drm_property *props, uint64_t value)
{
	const char *name = NULL;
	int j;

	printf("\t\tProperty: %s\n", props->name);
	printf("\t\t\tid           : %i\n", props->prop_id);
	printf("\t\t\tflags        : %i\n", props->flags);
	printf("\t\t\tcount_values : %d\n", props->count_values);


	if (props->count_values) {
		printf("\t\t\tvalues       :");
		for (j = 0; j < props->count_values; j++)
			printf(" %" PRIu64, props->values[j]);
		printf("\n");
	}


	printf("\t\t\tcount_enums  : %d\n", props->count_enums);

	if (props->flags & DRM_MODE_PROP_BLOB) {
		drmModePropertyBlobPtr blob;

		blob = drmModeGetPropertyBlob(fd, value);
		if (blob)
			printf("blob is %d length, %08X\n", blob->length, *(uint32_t *)blob->data);
	} else {
		for (j = 0; j < props->count_enums; j++) {
			printf("\t\t\t\t%lld = %s\n", props->enums[j].value, props->enums[j].name);
			if (props->enums[j].value == value)
				name = props->enums[j].name;
		}

		if (props->count_enums && name) {
			printf("\t\t\tcon_value    : %s\n", name);
		} else {
			printf("\t\t\tcon_value    : %" PRIu64 "\n", value);
		}
	}

	return 0;
}

static int drm_mode_get_connector(int fd, int id, struct drm_connector *drmcon)
{
	struct drm_mode_get_connector conn;
	int err;

	memset(&conn, 0, sizeof(struct drm_mode_get_connector));

	conn.connector_id = id;

	err = ioctl(fd, DRM_IOCTL_MODE_GETCONNECTOR, &conn);
	if (err) {
		perror("DRM_IOCTL_MODE_GETCONNECTOR");
		return err;
	}

	if (conn.count_props) {
		conn.props_ptr = VOID2U64(malloc(conn.count_props*sizeof(uint32_t)));
		if (!conn.props_ptr)
			goto err_allocs;
		conn.prop_values_ptr = VOID2U64(malloc(conn.count_props*sizeof(uint64_t)));
		if (!conn.prop_values_ptr)
			goto err_allocs;
	}

	if (conn.count_modes) {
		conn.modes_ptr = VOID2U64(malloc(conn.count_modes*sizeof(struct drm_mode_modeinfo)));
		if (!conn.modes_ptr)
			goto err_allocs;
	}

	if (conn.count_encoders) {
		conn.encoders_ptr = VOID2U64(malloc(conn.count_encoders*sizeof(uint32_t)));
		if (!conn.encoders_ptr)
			goto err_allocs;
	}

	err = ioctl(fd, DRM_IOCTL_MODE_GETCONNECTOR, &conn);
	if (err) {
		perror("DRM_IOCTL_MODE_GETCONNECTOR");
		return err;
	}

	drmcon->id = id;
	drmcon->num_modes = conn.count_modes;
	drmcon->encoder_id = conn.encoder_id;
	drmcon->type = conn.connector_type;

	drmcon->modes = U642VOID(conn.modes_ptr);
	drmcon->num_modes = conn.count_modes;

	drmcon->count_props  = conn.count_props;
	drmcon->props        = U642VOID(conn.props_ptr);
	drmcon->prop_values  = U642VOID(conn.prop_values_ptr);

	drmcon->encoders     = U642VOID(conn.encoders_ptr);
	drmcon->num_encoders = conn.count_encoders;

	return 0;

err_allocs:
	return -ENOMEM;
}

static int drm_mode_get_fb(int fd, uint32_t fb_id, struct drm_mode_fb_cmd *f)
{
	int err;

	memset(f, 0, sizeof(*f));

	f->fb_id = fb_id;

	if ((err = ioctl(fd, DRM_IOCTL_MODE_GETFB, f))) {
		perror("DRM_IOCTL_MODE_GETFB");
		return err;
	}

	return 0;
}

static int drm_mode_get_plane(int fd, int id,
		struct drm_mode_get_plane *plane)
{
	int err;

	plane->plane_id = id;

	err = ioctl(fd, DRM_IOCTL_MODE_GETPLANE, plane);
	if (err) {
		perror("DRM_IOCTL_MODE_GETPLANE");
		return err;
	}

	return 0;
}

static struct drm_resource *drm_mode_get_resources(int fd)
{
	struct drm_mode_card_res res;
	struct drm_mode_get_plane_res plane_res;
	int err;
	unsigned int i;
	struct drm_resource *drmres;

	drmres = zalloc(sizeof(*drmres));

	memset(&res, 0, sizeof(struct drm_mode_card_res));

	err = ioctl(fd, DRM_IOCTL_MODE_GETRESOURCES, &res);
	if (err) {
		perror("DRM_IOCTL_MODE_GETRESOURCES 1");
		return NULL;
	}

	drmres->num_crtcs = res.count_crtcs;
	drmres->num_connectors = res.count_connectors;
	drmres->num_encoders = res.count_encoders;
	drmres->num_fbs = res.count_fbs;

	if (res.count_crtcs) {
		res.crtc_id_ptr = VOID2U64(malloc(res.count_crtcs * sizeof(uint32_t)));
		drmres->crtcs = zalloc(sizeof(struct drm_mode_crtc) * res.count_crtcs);
	}

	if (res.count_encoders) {
		res.encoder_id_ptr = VOID2U64(malloc(res.count_encoders * sizeof(uint32_t)));
		drmres->encoders = zalloc(sizeof(struct drm_mode_get_encoder) * res.count_encoders);
	}

	if (res.count_connectors) {
		res.connector_id_ptr = VOID2U64(malloc(res.count_connectors * sizeof(uint32_t)));
		drmres->connectors = zalloc(sizeof(struct drm_connector) * res.count_connectors);
	}

	if (res.count_fbs) {
		res.fb_id_ptr = VOID2U64(malloc(res.count_fbs * sizeof(uint32_t)));
		drmres->fbs = zalloc(sizeof(struct drm_mode_fb_cmd) * res.count_fbs);
	}

	err = ioctl(fd, DRM_IOCTL_MODE_GETRESOURCES, &res);
	if (err) {
		perror("DRM_IOCTL_MODE_GETRESOURCES 2");
		return NULL;
	}

	for (i = 0; i < res.count_crtcs; i++) {
		uint32_t *crtcs = U642VOID(res.crtc_id_ptr);
		int id = crtcs[i];
		drm_mode_get_crtc(fd, id, &drmres->crtcs[i]);
	}

	for (i = 0; i < res.count_connectors; i++) {
		uint32_t *cons = U642VOID(res.connector_id_ptr);
		int id = cons[i];
		drm_mode_get_connector(fd, id, &drmres->connectors[i]);
	}

	for (i = 0; i < res.count_encoders; i++) {
		uint32_t *enc = U642VOID(res.encoder_id_ptr);
		int id = enc[i];
		drm_mode_get_encoder(fd, id, &drmres->encoders[i]);
	}

	for (i = 0; i < res.count_fbs; i++) {
		uint32_t *fb = U642VOID(res.fb_id_ptr);
		int id = fb[i];
		drm_mode_get_fb(fd, id, &drmres->fbs[i]);
	}

	memset(&plane_res, 0, sizeof(struct drm_mode_get_plane_res));

	err = ioctl(fd, DRM_IOCTL_MODE_GETPLANERESOURCES, &plane_res);
	if (err) {
		perror("DRM_IOCTL_MODE_GETPLANERESOURCES 1");
		return NULL;
	}

	drmres->num_planes = plane_res.count_planes;

	if (plane_res.count_planes) {
		plane_res.plane_id_ptr = VOID2U64(zalloc(sizeof(uint32_t) * plane_res.count_planes));
		drmres->planes = zalloc(sizeof(struct drm_mode_get_plane) * plane_res.count_planes);
	}

	err = ioctl(fd, DRM_IOCTL_MODE_GETPLANERESOURCES, &plane_res);
	if (err) {
		perror("DRM_IOCTL_MODE_GETPLANERESOURCES 2");
		return NULL;
	}

	for (i = 0; i < plane_res.count_planes; i++) {
		uint32_t *planes = U642VOID(plane_res.plane_id_ptr);
		int id = planes[i];
		drm_mode_get_plane(fd, id, &drmres->planes[i]);
	}

	return drmres;
}

static int drm_mode_get_dumb(int fd, struct kms_fb *fb, int create_splash)
{
	struct drm_mode_create_dumb dumb;
	struct drm_mode_map_dumb map;
	int err;
	struct drm_prime_handle prime = {};

	memset(&dumb, 0, sizeof(dumb));
	memset(&map, 0, sizeof(map));

	if (!fb->xres || !fb->yres || !fb->format) {
		pr_err("invalid fb settings: xres=%d, yres=%d, format=%4.4s\n",
				fb->xres, fb->yres, (char *)&fb->format);
		return -EINVAL;
	}

	dumb.width = fb->xres;
	switch (fb->format) {
	case DRM_FORMAT_RGB565_A8:
	case DRM_FORMAT_BGR565_A8:
		dumb.height = fb->yres + (fb->yres + 1) / 2;
		dumb.bpp = 16;
		break;
	case DRM_FORMAT_RGB888_A8:
	case DRM_FORMAT_BGR888_A8:
		dumb.height = fb->yres + (fb->yres + 2) / 3;
		dumb.bpp = 24;
		break;
	case DRM_FORMAT_RGBX8888_A8:
	case DRM_FORMAT_BGRX8888_A8:
		dumb.height = fb->yres + (fb->yres + 3) / 4;
		dumb.bpp = 32;
		break;
	default:
		dumb.height = fb->yres * fb->num_slices;
		dumb.bpp = fb->bpp;
	}
	dumb.flags = 0;

	err = ioctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &dumb);
	if (err) {
		perror("DRM_IOCTL_MODE_CREATE_DUMB");
		return -errno;
	}

	map.handle = dumb.handle;
	map.offset = 0x5359485000000000;

	print("dumb handle: %d\n", dumb.handle);

	err = ioctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &map);
	if (err) {
		perror("DRM_IOCTL_MODE_MAP_DUMB");
		return -errno;
	}

	fb->phys = map.offset >> 32;
	map.offset &= 0xffffffff;

	prime.handle = dumb.handle;
	prime.flags = DRM_CLOEXEC;

	err = ioctl(fd, DRM_IOCTL_PRIME_HANDLE_TO_FD, &prime);
	if (err) {
		perror("DRM_IOCTL_PRIME_HANDLE_TO_FD");
		fb->fd = 0;
	}
	else {
		fb->fd = prime.fd;
	}

	fb->fb = mmap(NULL, dumb.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, map.offset);
	if (fb->fb == MAP_FAILED) {
		perror("mmap");
		return -errno;
	}

	fb->gem_name = dumb.handle;
	fb->pitch = dumb.pitch;

	if (create_splash) {
		dumb.width = fb->xres;
		dumb.height = fb->yres;
		dumb.bpp = fb->bpp;
		dumb.flags = 0;

		err = ioctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &dumb);
		if (err) {
			perror("DRM_IOCTL_MODE_CREATE_DUMB");
			return -errno;
		}

		map.handle = dumb.handle;

		print("splash dumb handle: %d\n", dumb.handle);

		err = ioctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &map);
		if (err) {
			perror("DRM_IOCTL_MODE_MAP_DUMB");
			return -errno;
		}

		fb->splash_fb = mmap(NULL, dumb.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, map.offset);
		if (fb->splash_fb == MAP_FAILED) {
			perror("mmap");
			return -errno;
		}

		fb->splash_gem_name = dumb.handle;
		fb->splash_pitch = dumb.pitch;
	}

	return 0;
}

static int drm_mode_set_crtc(int fd, uint32_t crtc_id, uint32_t fb_id,
			     uint32_t x, uint32_t y, uint32_t *connectors,
			     int count, struct drm_mode_modeinfo *mode)
{
	struct drm_mode_crtc crtc;
	int err;

	memset(&crtc, 0, sizeof(struct drm_mode_crtc));

	crtc.x             = x;
	crtc.y             = y;
	crtc.crtc_id       = crtc_id;
	crtc.fb_id         = fb_id;
	crtc.set_connectors_ptr = VOID2U64(connectors);
	crtc.count_connectors = count;
	if (mode) {
		memcpy(&crtc.mode, mode, sizeof(struct drm_mode_modeinfo));
		crtc.mode_valid = 1;
	} else {
		crtc.mode_valid = 0;
	}

	err = ioctl(fd, DRM_IOCTL_MODE_SETCRTC, &crtc);
	if (err) {
		perror("DRM_IOCTL_MODE_SETCRTC");
		return err;
	}

	return 0;
}

static int drm_mode_set_plane(int fd, uint32_t plane_id, uint32_t crtc_id,
			      uint32_t fb_id, uint32_t src_x, uint32_t src_y,
			      uint32_t crtc_x, uint32_t crtc_y, uint32_t w,
			      uint32_t h)
{
	struct drm_mode_set_plane plane;
	int err;

	memset(&plane, 0, sizeof(plane));

	plane.plane_id = plane_id;
	plane.crtc_id = crtc_id;
	plane.fb_id = fb_id;
	plane.flags = 0;

	plane.crtc_x = crtc_x;
	plane.crtc_y = crtc_y;
	plane.crtc_w = w;
	plane.crtc_h = h;
	plane.src_x = src_x << 16;
	plane.src_y = src_y << 16;
	plane.src_w = w << 16;
	plane.src_h = h << 16;

	err = ioctl(fd, DRM_IOCTL_MODE_SETPLANE, &plane);
	if (err) {
		perror("DRM_IOCTL_MODE_SETPLANE");
		return err;
	}

	return 0;
}

int kms_fb_set_mode(struct kms_fb *fb, struct drm_mode_modeinfo *mode)
{
	struct drm_mode_modeinfo *mode_adjust;

	mode_adjust = drm_find_mode(fb->con, fb->xres, fb->yres, mode->vrefresh);
	if (!mode_adjust) {
		printf("could not find mode %dx%d@%d\n",
		       fb->xres, fb->yres, mode->vrefresh);
	} else {
		mode_adjust = drm_find_mode(fb->con, fb->xres, fb->yres, 0);
	}
	if (!mode_adjust) {
		printf("could not find any mode %dx%d, forcing %dx%d@%d\n",
		       fb->xres, fb->yres, mode->hdisplay, mode->vdisplay,
		       mode->vrefresh);
	}

	return drm_mode_set_crtc(drmfd, fb->crtc, fb->splash_fb_id,
				 0, 0, &fb->con->id, 1, mode);
}

static struct option long_options[] = {
	{
		.name = "crtc",
		.has_arg = 1,
		.flag = 0,
		.val = 'c',
	}, {
		.name = "fb",
		.has_arg = 0,
		.flag = 0,
		.val = 'f',
	}, {
		.name = "encoder",
		.has_arg = 1,
		.flag = 0,
		.val = 'e',
	}, {
		.name = "connector",
		.has_arg = 1,
		.flag = 0,
		.val = 'p',
	}, {
		.name = 0,
		.has_arg = 0,
		.flag = 0,
		.val = 0,
	}
};


static void line(const char *str)
{
	if (quiet)
		return;
	printf("------------------------ %s ------------------------\n", str);
}

static int create_drm_fbs(struct kms_fb *kms_fb)
{
	uint32_t bo_handles[4] = { };
	uint32_t pitches[4] = { };
	uint32_t offsets[4] = { };
	int err, i;

	switch (kms_fb->format) {
	case DRM_FORMAT_RGB565_A8:
	case DRM_FORMAT_BGR565_A8:
		bo_handles[1] = kms_fb->gem_name;
		pitches[1] = (kms_fb->pitch + 1) / 2;
		offsets[1] = kms_fb->pitch * kms_fb->yres;
		break;
	case DRM_FORMAT_RGB888_A8:
	case DRM_FORMAT_BGR888_A8:
		bo_handles[1] = kms_fb->gem_name;
		pitches[1] = (kms_fb->pitch + 2) / 3;
		offsets[1] = kms_fb->pitch * kms_fb->yres;
		break;
	case DRM_FORMAT_RGBX8888_A8:
	case DRM_FORMAT_BGRX8888_A8:
		bo_handles[1] = kms_fb->gem_name;
		pitches[1] = (kms_fb->pitch + 3) / 4;
		offsets[1] = kms_fb->pitch * kms_fb->yres;
		break;
	case DRM_FORMAT_ARGB4444:
	case DRM_FORMAT_ARGB1555:
	case DRM_FORMAT_RGB565:
	case DRM_FORMAT_BGR565:
	case DRM_FORMAT_RGB888:
	case DRM_FORMAT_BGR888:
	case DRM_FORMAT_XRGB8888:
	case DRM_FORMAT_ARGB8888:
	case DRM_FORMAT_XBGR8888:
	case DRM_FORMAT_ABGR8888:
	case DRM_FORMAT_RGBX8888:
	case DRM_FORMAT_RGBA8888:
	case DRM_FORMAT_BGRX8888:
	case DRM_FORMAT_BGRA8888:
		break;
	default:
		fprintf(stderr, "illegal format: '%4.4s'\n",
			(char *)&kms_fb->format);
		return -EINVAL;
	}

	bo_handles[0] = kms_fb->gem_name;
	pitches[0] = kms_fb->pitch;

	for (i = 0; i < kms_fb->num_slices; i++) {
		offsets[0] = (kms_fb->pitch * kms_fb->yres * i);

		err = drmModeAddFB2(drmfd, kms_fb->xres, kms_fb->yres, kms_fb->format, bo_handles, pitches, offsets,
		                    &kms_fb->slices[i].fb_id, 0);
		if (err) {
			fprintf(stderr, "drmModeAddFB2 failed: %d\n", err);
			return err;
		}
	}

	if (kms_fb->splash_fb) {
		bo_handles[0] = kms_fb->splash_gem_name;
		pitches[0] = kms_fb->splash_pitch;
		offsets[0] = 0;
		err = drmModeAddFB2(drmfd, kms_fb->xres, kms_fb->yres, kms_fb->format, bo_handles, pitches, offsets,
		                    &kms_fb->splash_fb_id, 0);

		if (err) {
			fprintf(stderr, "drmModeAddFB2 failed: %d\n", err);
			return err;
		}
	} else {
		kms_fb->splash_fb_id = kms_fb->slices[0].fb_id;
	}

	return 0;

}

static int create_window(struct drm_resource *res, const char *in)
{
	struct drm_connector *drmcon;
	struct drm_mode_get_encoder *drmenc;
	struct drm_connector *con;
	struct drm_mode_modeinfo *mode;
	struct kms_data kd = {};
	int singular_slices;
	int ret, i, p;

	ret = parse_key_value(in, &kd);
	if (ret)
		return ret;


	drmcon = drm_find_connector(res, kd.connector);
	if (!drmcon) {
		printf("specified connector %d does not exist\n", kd.connector);
		return -1;
	}
	kd.connector = drmcon->id;

	if (!kd.encoder) {
		if (drmcon->encoder_id) {
			print("using current encoder %d\n", drmcon->encoder_id);
		} else if (drmcon->num_encoders > 0) {
			drmcon->encoder_id = drmcon->encoders[0];
			print("using first available encoder %d\n", drmcon->encoder_id);
		}
		kd.encoder = drmcon->encoder_id;
	}

	drmenc = drm_find_encoder(res, kd.encoder);
	if (!drmenc) {
		printf("specified encoder id %d does not exist\n", kd.encoder);
		return -1;
	}


	if (!kd.crtc) {
		if (drmenc->crtc_id > 0) {
			print("using current crtc %d\n", drmenc->crtc_id);
			kd.crtc = drmenc->crtc_id;
		} else {
			for (i = 0, p = drmenc->possible_crtcs;
			     p && (i < res->num_crtcs); p >>= 1, i++) {
				if (p & 1) {
					kd.crtc = res->crtcs[i].crtc_id;
					print("using first possible crtc %d\n",
					       kd.crtc);
					break;
				}
			}
		}
	}

	/* do initial modeset for new window */
	con = drm_find_connector(res, kd.connector);
	if (!con)
		return -EINVAL;
	mode = drm_find_mode(con, kd.xres, kd.yres, kd.fps);
	if (!mode) {
		if (kd.fps) {
			printf("could not find mode %dx%d@%d\n",
			       kd.xres, kd.yres, kd.fps);
		} else {
			printf("could not find mode %dx%d\n", kd.xres, kd.yres);
		}
		return -EINVAL;
	}

	ret = drm_mode_set_crtc(drmfd, kd.crtc, kd.kms_fb->splash_fb_id,
	                        kd.xofs, kd.yofs, &kd.connector, 1, mode);
	if (ret)
		perror("drm_mode_set_crtc");

	singular_slices = 0;
	for (i = 0; i < kd.kms_fb->num_slices; i++) {
		kd.kms_fb->slices[i].windows[kd.kms_fb->slices[i].num_windows].crtc_id = kd.crtc;
		kd.kms_fb->slices[i].windows[kd.kms_fb->slices[i].num_windows].plane_id = 0;
		kd.kms_fb->slices[i].num_windows++;
		if (kd.kms_fb->slices[i].num_windows == 1)
			singular_slices++;
	}

	/*
	 * allow to set display dimings on singular full-screen windows,
	 * allow to read display timings on all full-screen windows.
	 */
	if (kd.xres != kd.kms_fb->xres || kd.yres != kd.kms_fb->yres) {
		con = NULL;
		mode = NULL;
	} else if (singular_slices != kd.kms_fb->num_slices) {
		con = NULL;
	}
	bgi_set_connector_crtc_mode(kd.kms_fb, con, kd.crtc, mode);

	return ret;
}

static int create_overlay(struct drm_resource *res, const char *in)
{
	struct kms_data kd = {};
	struct drm_mode_get_plane *drmplane;
	int ret;
	unsigned int i;

	ret = parse_key_value(in, &kd);
	if (ret)
		return ret;

	if (!kd.plane) {
		kd.plane = res->planes[0].plane_id;
		print("using first plane %d\n", kd.plane);
	}

	for (i = 0; i < res->num_planes; i++) {
		if (res->planes[i].plane_id == kd.plane)
			break;
	}
	if (i >= res->num_planes) {
		fprintf(stderr, "invalid plane id %d\n", kd.plane);
		return -1;
	}
	drmplane = &res->planes[i];

	if (!kd.crtc) {
		for (i = 0; i < res->num_crtcs; i++) {
			if (drmplane->possible_crtcs & (1 << i))
				break;
		}
		if (i >= res->num_crtcs) {
			fprintf(stderr, "plane %d without possible crtc",
				kd.plane);
			return -1;
		}
		kd.crtc = res->crtcs[i].crtc_id;
		print("using first possible crtc %d\n", kd.crtc);
	}

	ret = drm_mode_set_plane(drmfd, kd.plane, kd.crtc,
				 kd.kms_fb->splash_fb_id,
			         kd.xofs, kd.yofs,
				 kd.xpos, kd.ypos,
				 kd.xres, kd.yres);
	if (ret)
		perror("drm_mode_set_crtc");

	for (i = 0; i < kd.kms_fb->num_slices; i++) {
		struct kms_slice *slice = &kd.kms_fb->slices[i];
		struct kms_window *window = &slice->windows[slice->num_windows];
		window->crtc_id = kd.crtc;
		window->plane_id = kd.plane;
		window->xofs = kd.xofs;
		window->yofs = kd.yofs;
		window->xres = kd.xres;
		window->yres = kd.yres;
		window->xres = kd.xres;
		window->yres = kd.yres;
		slice->num_windows++;
	}

	return ret;
}

static void dump_resource(int fd, struct drm_resource *res)
{
	int i, j;

	printf("CRTCs:\n");

	for (i = 0; i < res->num_crtcs; i++) {
		struct drm_mode_crtc *crtc = &res->crtcs[i];
		struct drm_mode_modeinfo *mode = &crtc->mode;
		printf("\tid: %d\n", crtc->crtc_id);
		printf("\t\tfb id: %d\n", crtc->fb_id);
		printf("\t\tx: %d\n", crtc->x);
		printf("\t\ty: %d\n", crtc->y);
		if (crtc->mode_valid)
			printf("\t\tcurrent mode: %dx%d@%d\n",
					mode->hdisplay, mode->vdisplay, mode->vrefresh);
	}

	printf("Connectors:\n");
	for (i = 0; i < res->num_connectors; i++) {
		struct drm_connector *con = &res->connectors[i];
		printf("\tid %d\n", con->id);
		printf("\t\tcurrent encoder: %d\n", con->encoder_id);
		printf("\t\ttype: %s\n", connector_type_strings[con->type]);
		for (j = 0; j < con->num_modes; j++) {
			struct drm_mode_modeinfo *mode = &con->modes[j];
			printf("\t\t\tmode%d: %dx%d@%d\n",
					j, mode->hdisplay, mode->vdisplay, mode->vrefresh);
		}

		for (j = 0; j < con->count_props; j++) {
			struct drm_property *props = drm_mode_get_property(fd, con->props[j]);
			if (props)
				print_property(fd, props, con->prop_values[j]);
		}
	}

	printf("Encoders: %d\n", res->num_encoders);
	for (i = 0; i < res->num_encoders; i++) {
		struct drm_mode_get_encoder *enc = &res->encoders[i];
		printf("\tid: %d\n", enc->encoder_id);
		printf("\t\tcrtc id: %d\n", enc->crtc_id);
		printf("\t\tpossible crtcs: 0x%08x\n", enc->possible_crtcs);
		printf("\t\tpossible clones: 0x%08x\n", enc->possible_clones);
	}

	printf("Framebuffers: %d\n", res->num_fbs);
	for (i = 0; i < res->num_fbs; i++) {
		struct drm_mode_fb_cmd *fb = &res->fbs[i];
		printf("\tid: %d\n", fb->fb_id);
		printf("\t\twidth: %d\n", fb->width);
		printf("\t\theight: %d\n", fb->height);
		printf("\t\tpitch: %d\n", fb->pitch);
	}

	printf("Planes: %d\n", res->num_planes);
	for (i = 0; i < res->num_planes; i++) {
		struct drm_mode_get_plane *plane = &res->planes[i];
		printf("\tid: %d\n", plane->plane_id);
	}
}

struct cmd {
	const char *name;
	const char *help;
	int (*fn)(int fd, int argc, char **argv);
};

static void *convert_rgb2fb(unsigned char *rgb, unsigned int count, int bpp)
{
	void *result;
	uint32_t *fbbuf;
	int i;

	switch (bpp) {
	case 32:
		fbbuf = (uint32_t *)malloc(count * sizeof(*fbbuf));

		for (i = 0; i < count; i++) {
			fbbuf[i] = ((rgb[i*3] << 16) & 0xff0000) |
				   ((rgb[i*3+1] << 8) & 0xff00) |
				   ((rgb[i*3+2]) & 0xff);
		}
		result = (void *)fbbuf;
		break;
	default:
		fprintf(stderr, "Unsupported video mode! You've got: %dbpp\n", bpp);
		return NULL;
	}

	return result;
}

static int fb_splash(struct kms_fb *fb, char *filename)
{
	unsigned char *alpha;
	unsigned char *line;
	unsigned char *fbbuf;
	struct splash img;
	int y;
	int cpp;
	int ret;

	memset(&img, 0, sizeof(struct splash));

	ret = png_load(filename, &img, &alpha);
	if (ret)
		return -EINVAL;

	fbbuf = convert_rgb2fb(img.rgb, img.width * img.height, fb->bpp);
	if (!fbbuf)
		return -ENOMEM;

	cpp = fb->bpp / 8;

	line = fb->splash_fb ? fb->splash_fb : fb->fb;
	if (img.width < fb->xres || img.height < fb->yres) {
		for (y = 0; y < img.height; y++) {
			memcpy(line, fbbuf, img.width * cpp);
			line += fb->pitch;
			fbbuf += img.width * cpp;
		}
	} else {
		memcpy(line, fbbuf, img.width * img.height * cpp);
	}

	return 0;
}

static int fb_create(struct drm_resource *res, int fd, const char *cmd)
{
	int ret;
	struct kms_fb *fb;
	struct kms_data kd = { 0 };

	ret = parse_key_value(cmd, &kd);
	if (ret)
		return ret;

	if (!kd.format) {
		switch (kd.bpp) {
		case 24:
			kd.format = DRM_FORMAT_RGB888;
			break;
		case 32:
			kd.format = kd.alpha ? DRM_FORMAT_ARGB8888 :
					       DRM_FORMAT_XRGB8888;
			break;
		default:
			kd.format = kd.alpha ? DRM_FORMAT_ARGB1555 :
					       DRM_FORMAT_RGB565;
			break;
		}
	}

	switch (kd.format) {
	case DRM_FORMAT_RGB565:
	case DRM_FORMAT_BGR565:
	case DRM_FORMAT_ARGB1555:
	case DRM_FORMAT_BGR565_A8:
	case DRM_FORMAT_RGB565_A8:
	case DRM_FORMAT_ARGB4444:
		kd.bpp = 16;
		break;
	case DRM_FORMAT_RGB888:
	case DRM_FORMAT_BGR888:
	case DRM_FORMAT_RGB888_A8:
	case DRM_FORMAT_BGR888_A8:
		kd.bpp = 24;
		break;
	case DRM_FORMAT_XRGB8888:
	case DRM_FORMAT_ARGB8888:
	case DRM_FORMAT_XBGR8888:
	case DRM_FORMAT_ABGR8888:
	case DRM_FORMAT_RGBX8888:
	case DRM_FORMAT_RGBA8888:
	case DRM_FORMAT_BGRX8888:
	case DRM_FORMAT_BGRA8888:
	case DRM_FORMAT_RGBX8888_A8:
	case DRM_FORMAT_BGRX8888_A8:
		kd.bpp = 32;
		break;
	default:
		pr_err("Invalid format: '%s'\n", (char *)&kd.format);
		return -EINVAL;
	}

	line("create framebuffer");

	fb = zalloc(sizeof(*fb));

	fb->xres = kd.xres;
	fb->yres = kd.yres;
	fb->pitch = kd.pitch;
	fb->bpp = kd.bpp;
	fb->format = kd.format;
	fb->num_slices = kd.triplebuf ? 3 : 1;
	fb->vsync.val = kd.vsync;
	fb->id = globals.num_kms_fbs;

	if (fb->xres == 0 && fb->yres == 0)
		drm_find_max_res(res, &fb->xres, &fb->yres);

	/* if tripple buffering is enabled _and_ a splash image is shown
	   then put the image on a separate framebuffer. This way, any
	   changes to the real framebuffer (e.g. clearing) are hidden until
	   the first page flip. */
	ret = drm_mode_get_dumb(fd, fb, kd.triplebuf && strlen(kd.file) > 0);
	if (ret)
		return ret;

	/* create DRM FBs */
	ret = create_drm_fbs(fb);
	if (ret)
		return ret;

	if (strlen(kd.file) > 0)
		fb_splash(fb, kd.file);

	if (kd.fill) {
		int x, y;

		if (kd.format == DRM_FORMAT_XRGB8888 ||
		    kd.format == DRM_FORMAT_ARGB8888 ||
		    kd.format == DRM_FORMAT_XBGR8888 ||
		    kd.format == DRM_FORMAT_ABGR8888) {
			uint32_t *pix;

			for (y = 0; y < fb->yres; y++) {
				for (x = 0; x < fb->xres; x++) {
					pix = fb->fb + x * 4 + y * fb->pitch;
					*pix = ((x & 0xff) << 8) |
						(0xff - (y & 0xff)) |
						(((0xff - x) & 0xff) << 16) |
						((((x ^ y) & 1) ? 0x00 : 0xff) << 24);
				}
			}
		} else if (kd.format == DRM_FORMAT_RGBX8888 ||
			   kd.format == DRM_FORMAT_RGBA8888 ||
			   kd.format == DRM_FORMAT_BGRX8888 ||
			   kd.format == DRM_FORMAT_BGRA8888 ||
			   kd.format == DRM_FORMAT_RGBX8888_A8 ||
			   kd.format == DRM_FORMAT_BGRX8888_A8) {
			uint32_t *pix;
			uint8_t *alpha;

			for (y = 0; y < fb->yres; y++) {
				for (x = 0; x < fb->xres; x++) {
					pix = fb->fb + x * 4 + y * fb->pitch;
					*pix = ((x & 0xff) << 16) |
						((0xff - (y & 0xff)) << 8) |
						(((0xff - x) & 0xff) << 24) |
						(((x ^ y) & 1) ? 0x00 : 0xff);
					if (kd.format == DRM_FORMAT_RGBX8888 ||
					    kd.format == DRM_FORMAT_RGBA8888 ||
					    kd.format == DRM_FORMAT_BGRX8888 ||
					    kd.format == DRM_FORMAT_BGRA8888)
						continue;
					alpha = fb->fb + fb->yres * fb->pitch +
						x + y * ((fb->pitch + 3) / 4);
					*alpha = ((x ^ y) & 1) ? 0x00 : 0xff;
				}
			}
		} else if (kd.format == DRM_FORMAT_BGR888 ||
			   kd.format == DRM_FORMAT_RGB888 ||
			   kd.format == DRM_FORMAT_BGR888_A8 ||
			   kd.format == DRM_FORMAT_RGB888_A8) {
			uint8_t *pix, *alpha;

			for (y = 0; y < fb->yres; y++) {
				for (x = 0; x < fb->xres; x++) {
					pix = fb->fb + x * 3 + y * fb->pitch;
					pix[0] = (0xff - x) & 0xff;
					pix[1] = x & 0xff;
					pix[2] = 0xff - (y & 0xff);
					if (kd.format == DRM_FORMAT_BGR888 ||
					    kd.format == DRM_FORMAT_RGB888)
						continue;
					alpha = fb->fb + fb->yres * fb->pitch +
						x + y * ((fb->pitch + 2) / 3);
					*alpha = ((x ^ y) & 1) ? 0x00 : 0xff;
				}
			}
		} else if (kd.format == DRM_FORMAT_RGB565 ||
			   kd.format == DRM_FORMAT_BGR565 ||
			   kd.format == DRM_FORMAT_RGB565_A8 ||
			   kd.format == DRM_FORMAT_BGR565_A8) {
			uint16_t *pix;
			uint8_t *alpha;

			for (y = 0; y < fb->yres; y++) {
				for (x = 0; x < fb->xres; x++) {
					pix = fb->fb + x * 2 + y * fb->pitch;
					*pix = ((x & 0x3f) << 5) |
						(0x1f - (y & 0x1f)) |
						(((0x1f - x) & 0x1f) << 11);
					if (kd.format == DRM_FORMAT_RGB565 ||
					    kd.format == DRM_FORMAT_BGR565)
						continue;
					alpha = fb->fb + fb->yres * fb->pitch +
						x + y * ((fb->pitch + 1) / 2);
					*alpha = ((x ^ y) & 1) ? 0x00 : 0xff;
				}
			}
		} else if (kd.format == DRM_FORMAT_ARGB1555) {
			uint16_t *pix;

			for (y = 0; y < fb->yres; y++) {
				for (x = 0; x < fb->xres; x++) {
					pix = fb->fb + x * 2 + y * fb->pitch;
					*pix = ((x & 0x1f) << 5) |
						(0x1f - (y & 0x1f)) |
						(((0x1f - x) & 0x1f) << 10) |
						((((x ^ y) & 1) ? 0 : 1) << 15);
				}
			}
		}
	}

	globals.kms_fbs[globals.num_kms_fbs] = fb;
	globals.num_kms_fbs++;

	ret = bgi_init(fb);
	if (ret)
		return ret;

	print("created framebuffer: %dx%d, pitch %d, format %4.4s\n",
			fb->xres, fb->yres, fb->pitch, (char *)&fb->format);

	return 0;
}

static void usage(const char *prgname)
{
	printf("usage: %s [OPTIONS]\n"
		"\n"
		"options:\n"
		"-f <fbdesc>          create a framebuffer with given resolution\n"
		"   recognized variables for <fbdesc>:\n"
		"      xres, yres [required]: resolution\n"
		"      bpp        [optional]: bits per pixel (default 16)\n"
		"      pitch      [optional]: line pitch in bytes\n"
		"      3d         [optional]: create a framebuffer suitable for 3D rendering\n"
		"      fill       [optional]: fill framebuffer with debug pattern\n"
		"      file       [optional]: load file into framebuffer\n"
		"      vsync      [optional]: vsync mode:\n"
		"                                0 - (default) don't wait for vsync when panning, but honor WAITFORVSYNC\n"
		"                                1 - never wait for vsync\n"
		"                                2 - always wait for vsync\n"
		"      alpha      [optional]: set to 1 to enable alpha transparency (default: 0)\n"
		"      format     [optional]: drm format (default rgb565)\n"
		"-s                   show information about current drm state\n"
		"                     (can be given multiple times, executed in order of options)\n"
		"-w <windesc>         create a DRM window into a framebuffer\n"
		"   recognized variables for <windesc>:\n"
		"      fbid       [required]: ID of the FB created with -f\n"
		"      connector, encoder, crtc [required]: chain of DRM object IDs onto which the window should be bound\n"
		"      xres, yres [optional]: resolution of the window (default: full resolution of referenced FB)\n"
		"      xofs, yofs [optional]: offset in pixels into the FB\n"
		"      fps        [optional]: select a frame rate in Hz\n"
		"-o <ovldesc>         create a DRM window into a framebuffer using a partial overlay plane\n"
		"   recognized variables for <ovldesc:\n"
		"     fbid       [required]: ID of the FB created with -f\n"
		"      crtc, plane [optional]: DRM object IDs onto which the window should be bound\n"
		"      xres, yres [optional]: resolution of the window (default: full resolution of referenced FB)\n"
		"      xofs, yofs [optional]: offset in pixels into the FB\n"
		"      xpos, ypos [optional]: top left position of the overlay plane (default: 0,0)\n"
		"-d                   debug\n"
		"-q                   quiet\n"
		, prgname);
	exit(1);
}

#define MAX_ARGS 256

static int open_master(void)
{
	struct drm_mode_card_res res;
	char buf[64];
	int i, fd;

	for (i = 0; i < DRM_MAX_MINOR; i++) {
		sprintf(buf, DRM_DEV_NAME, DRM_DIR_NAME, i);
		if ((fd = open(buf, O_RDWR, 0)) >= 0) {
			memset(&res, 0, sizeof(struct drm_mode_card_res));
			if (!ioctl(fd, DRM_IOCTL_MODE_GETRESOURCES, &res))
				return fd;
			close(fd);
		}
	}

	return -1;
}

int main(int argc, char *argv[])
{
	int fd;
	int c;
	struct drm_resource *res;
	int err;
	char **parse_args;
	pid_t kmsfbpid;
	char *cmdline;
	char *cmdlinev[MAX_ARGS];

	kmsfbpid = getpid();
	if (kmsfbpid == 1) {
		pid_t pid;

		pid = fork();
		if (pid > 0) {
			print("kmsfb-manage forking init\n");
			execl("/sbin/init", "/sbin/init", NULL);
			exit(1);
		}
	}

	if (argc <= 1) {
		int f;
		int ret;
		ssize_t read_bytes;
		struct stat st;
		const char *fname = "/etc/kmsfb-manage.conf";
		char *cmdline_ptr;

		f = open(fname, O_RDONLY);
		if (f == -1) {
			fprintf(stderr, "Failed to open '%s' (%s)!\n", fname, strerror(errno));
			usage(argv[0]);
			return 1;
		}

		ret = fstat(f, &st);
		if (ret) {
			fprintf(stderr, "Failed to stat '%s' (%s)!\n", fname, strerror(errno));
			return 1;
		}

		cmdline = malloc(st.st_size + 1);
		if (!cmdline) {
			fprintf(stderr, "Failed to allocate memory to read in default arguments!\n");
			return 1;
		}

		read_bytes = read(f, cmdline, st.st_size);
		close(f);
		if (read_bytes != st.st_size) {
			fprintf(stderr, "kmsfb-manage could not read from file!\n");
			return 1;
		}
		cmdline[st.st_size] = '\0';

		cmdlinev[0] = argv[0];
		argc = 1;
		cmdline_ptr = strtok(cmdline, " \n\r");
		while (cmdline_ptr && argc < MAX_ARGS) {
			cmdlinev[argc] = cmdline_ptr;
			cmdline_ptr = strtok(NULL, " \n\r");
			++argc;
		}
		if (argc == MAX_ARGS) {
			fprintf(stderr, "too many arguments (max %d)!\n", MAX_ARGS);
			return 1;
		}
		parse_args = cmdlinev;
	} else {
		parse_args = argv;
	}

	fd = open_master();
	if (fd < 0) {
		perror("open");
		exit(1);
	}

	drmfd = fd;

	while (1) {
		int option_index = 0;
		c = getopt_long(argc, parse_args, "f:w:o:hsdq",
                        long_options, &option_index);
		if (c == -1)
			break;
		res = drm_mode_get_resources(fd);
		if (!res)
			exit(1);
		switch (c) {
		case 'f':
			err = fb_create(res, fd, optarg);
			if (err)
				exit(1);

			break;
		case 's':
			line("Current Layout");
			dump_resource(fd, res);
			return 0;
		case 'w':
			err = create_window(res, optarg);
			if (err)
				exit(1);
			break;
		case 'o':
			err = create_overlay(res, optarg);
			if (err)
				exit(1);
			break;
		case 'h':
			usage(parse_args[0]);
			break;
		case 'd':
			debug = 1;
			break;
		case 'q':
			quiet = 1;
			break;
		default:
			printf("getopt returned %d %c\n", c, c);
		}
	}

	while (!sleep(3600));

	return 0;
}
