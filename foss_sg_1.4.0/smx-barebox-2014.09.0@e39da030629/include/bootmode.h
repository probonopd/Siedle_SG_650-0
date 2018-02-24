#ifndef __INCLUDE_BOOTMODE_H
#define __INCLUDE_BOOTMODE_H

#include <bootsource.h>
#include <common.h>

#ifdef CONFIG_BOOTMODE

int bootmode_register_handler(int (*handler)(enum bootsource src, int instance,
					     struct device_node *node,
					     void *data), void *data);

int bootmode_set(enum bootsource src, int instance,
                 struct device_node *node);

#else

static inline int bootmode_register_handler(int (*handler)(enum bootsource src,
					    int instance,
					    struct device_node *node,
					    void *data), void *data)
{
	return -EINVAL;
}

int bootmode_set(enum bootsource src, int instance,
		 struct device_node *node)
{
	return -ENODEV;
}

#endif

#endif /* __INCLUDE_BOOTMODE_H */
