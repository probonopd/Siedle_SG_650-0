/*
 * Code based on:
 *
 * fbv  --  simple image viewer for the linux framebuffer
 * Copyright (C) 2000, 2001, 2003  Mateusz Golicz
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <errno.h>
#include <png.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "kmsfb.h"

#define PNG_BYTES_TO_CHECK 4

#if (PNG_LIBPNG_VER < 10500)
#define png_jmpbuf(png_ptr)	((png_ptr)->jmpbuf)
#endif

int png_load(char *name, struct splash *image, unsigned char **alpha)
{
	int bit_depth, color_type, interlace_type;
	int number_passes,pass, trans = 0;
	png_uint_32 width, height;
	unsigned char *fbptr;
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int i;
	unsigned char *rp;
	png_bytep rptr[2];
	FILE *fh;
	int ret;
	char id[4];

	fh = fopen(name,"rb");
	if (fh == NULL)
		return -ENOENT;

	ret = fread(&id, 1, 4, fh);

	if (ret != 4 || !(id[1] == 'P' && id[2]=='N' && id[3]=='G'))
		return -EINVAL;

	rewind(fh);

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL)
		return -EINVAL;

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(fh);

		return -EINVAL;
	}

	rp=0;
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

		if(rp)
			free(rp);

		fclose(fh);

		return -EINVAL;
	}

	png_init_io(png_ptr,fh);

	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,&interlace_type, NULL, NULL);

	image->rgb = (unsigned char*) malloc(width * height * 3);
	image->width = width;
	image->height = height;

	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_expand(png_ptr);

	if (bit_depth < 8)
		png_set_packing(png_ptr);

	if (color_type == PNG_COLOR_TYPE_GRAY || color_type== PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
		trans = 1;
		png_set_tRNS_to_alpha(png_ptr);
	}

	if (bit_depth == 16)
		png_set_strip_16(png_ptr);

	number_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr,info_ptr);

	if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA || color_type == PNG_COLOR_TYPE_RGB_ALPHA || trans) {
		unsigned char * alpha_buffer = (unsigned char*) malloc(width * height);
		unsigned char * aptr;

		rp = (unsigned char*) malloc(width * 4);
		rptr[0] = (png_bytep) rp;

		*alpha = alpha_buffer;

		for (pass = 0; pass < number_passes; pass++) {
			fbptr = image->rgb;
			aptr = alpha_buffer;

			for(i=0; i<height; i++) {
				unsigned int n;
				unsigned char *trp = rp;

				png_read_rows(png_ptr, rptr, NULL, 1);

				for(n = 0; n < width; n++, fbptr += 3, trp += 4) {
					memcpy(fbptr, trp, 3);
					*(aptr++) = trp[3];
				}
			}
		}
		free(rp);
	} else {
		for (pass = 0; pass < number_passes; pass++) {
			fbptr = image->rgb;
			for(i=0; i<height; i++, fbptr += width*3) {
			    rptr[0] = (png_bytep) fbptr;
				png_read_rows(png_ptr, rptr, NULL, 1);
			}
		}
	}

	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	fclose(fh);

	return 0;
}
