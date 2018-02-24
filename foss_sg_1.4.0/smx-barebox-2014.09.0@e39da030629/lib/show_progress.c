/*
 * show_progress.c - simple progress bar functions
 *
 * Copyright (c) 2010 Sascha Hauer <s.hauer@pengutronix.de>, Pengutronix
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
#include <fs.h>
#include <progress.h>
#include <asm-generic/div64.h>
#include <linux/stringify.h>

#define HASHES_PER_LINE	65

static int printed;
static int progress_max;
static int spin;

void show_progress(int now)
{
	char spinchr[] = "\\|/-";

	if (now < 0) {
		printf("%c\b", spinchr[spin++ % (sizeof(spinchr) - 1)]);
		return;
	}

	if (progress_max && progress_max != FILESIZE_MAX) {
		uint64_t tmp = (int64_t)now * HASHES_PER_LINE;
		do_div(tmp, progress_max);
		now = tmp;
	}

	while (printed < now) {
		if (!(printed % HASHES_PER_LINE) && printed)
			printf("\n\t");
		printf("#");
		printed++;
	}
}

void init_progression_bar(int max)
{
	printed = 0;
	progress_max = max;
	spin = 0;
	if (progress_max && progress_max != FILESIZE_MAX)
		printf("\t[%"__stringify(HASHES_PER_LINE)"s]\r\t[", "");
	else
		printf("\t");
}
