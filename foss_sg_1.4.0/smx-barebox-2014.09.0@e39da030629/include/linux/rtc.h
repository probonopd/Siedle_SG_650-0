/*
 * Generic RTC interface.
 * This version contains the part of the user interface to the Real Time Clock
 * service. It is used with both the legacy mc146818 and also  EFI
 * Struct rtc_time and first 12 ioctl by Paul Gortmaker, 1996 - separated out
 * from <linux/mc146818rtc.h> to this file for 2.4 kernels.
 *
 * Copyright (C) 1999 Hewlett-Packard Co.
 * Copyright (C) 1999 Stephane Eranian <eranian@hpl.hp.com>
 */
#ifndef _LINUX_RTC_H_
#define _LINUX_RTC_H_

#include <common.h>
#include <linux/types.h>

extern int rtc_month_days(unsigned int month, unsigned int year);
extern int rtc_valid_tm(struct rtc_time *tm);
extern int rtc_tm_to_time(struct rtc_time *tm, unsigned long *time);
extern void rtc_time_to_tm(unsigned long time, struct rtc_time *tm);

struct rtc_class_ops;

struct rtc_device {
	struct device_d *dev;
	struct device_d class_dev;
	struct list_head list;

	const struct rtc_class_ops *ops;
};

struct rtc_class_ops {
	int (*read_time)(struct rtc_device *, struct rtc_time *);
	int (*set_time)(struct rtc_device *, struct rtc_time *);
};

extern int rtc_register(struct rtc_device *rtcdev);

extern int rtc_read_time(struct rtc_device *rtc, struct rtc_time *tm);
extern int rtc_set_time(struct rtc_device *rtc, struct rtc_time *tm);

static inline bool is_leap_year(unsigned int year)
{
	return (!(year % 4) && (year % 100)) || !(year % 400);
}

#endif /* _LINUX_RTC_H_ */
