/*
 * Copyright (C) 2013 Philipp Zabel
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <bootsource.h>
#include <common.h>

static int (*bootmode_handler)(enum bootsource src, int instance,
			       struct device_node *node, void *data) = NULL;
static void *bootmode_handler_data = NULL;

int bootmode_register_handler(int (*handler)(enum bootsource src, int instance,
                                             struct device_node *node,
					     void *data), void *data)
{
	bootmode_handler = handler;
	bootmode_handler_data = data;

	return 0;
}

int bootmode_set(enum bootsource src, int instance,
		 struct device_node *node)
{
	if (!bootmode_handler)
		return -ENODEV;

	return bootmode_handler(src, instance, node, bootmode_handler_data);
}
