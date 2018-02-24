/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __INCLUDE_RESET_SOURCE_H
# define __INCLUDE_RESET_SOURCE_H

enum reset_src_type {
	RESET_UKWN,	/* maybe the SoC cannot detect the reset source */
	RESET_POR,	/* Power On Reset (cold start) */
	RESET_RST,	/* generic ReSeT (warm start) */
	RESET_WDG,	/* watchdog */
	RESET_WKE,	/* wake-up (some SoCs can handle this) */
	RESET_JTAG,	/* JTAG reset */
};

#ifdef CONFIG_RESET_SOURCE
void reset_source_set(enum reset_src_type);
enum reset_src_type reset_source_get(void);
#else
static inline void reset_source_set(enum reset_src_type unused)
{
}

static inline enum reset_src_type reset_source_get(void)
{
	return RESET_UKWN;
}
#endif

#endif /* __INCLUDE_RESET_SOURCE_H */
