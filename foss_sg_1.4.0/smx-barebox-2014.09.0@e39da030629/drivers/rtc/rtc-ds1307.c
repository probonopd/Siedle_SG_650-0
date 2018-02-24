/*
 * rtc-ds1307.c - RTC driver for some mostly-compatible I2C chips.
 *
 *  This code was ported from linux-3.15 kernel by Antony Pavlov.
 *
 *  Copyright (C) 2005 James Chapman (ds1337 core)
 *  Copyright (C) 2006 David Brownell
 *  Copyright (C) 2009 Matthias Fuchs (rx8025 support)
 *  Copyright (C) 2012 Bertrand Achard (nvram access fixes)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <common.h>
#include <init.h>
#include <driver.h>
#include <xfuncs.h>
#include <errno.h>
#include <i2c/i2c.h>
#include <rtc.h>
#include <linux/rtc.h>
#include <linux/bcd.h>

/*
 * We can't determine type by probing, but if we expect pre-Linux code
 * to have set the chip up as a clock (turning on the oscillator and
 * setting the date and time), Linux can ignore the non-clock features.
 * That's a natural job for a factory or repair bench.
 */
enum ds_type {
	ds_1307,
	ds_1338,
	last_ds_type /* always last */
};

/* RTC registers don't differ much, except for the century flag */
#define DS1307_REG_SECS		0x00	/* 00-59 */
#	define DS1307_BIT_CH		0x80
#	define DS1340_BIT_nEOSC		0x80
#define DS1307_REG_MIN		0x01	/* 00-59 */
#define DS1307_REG_HOUR		0x02	/* 00-23, or 1-12{am,pm} */
#	define DS1307_BIT_12HR		0x40	/* in REG_HOUR */
#	define DS1307_BIT_PM		0x20	/* in REG_HOUR */
#	define DS1340_BIT_CENTURY_EN	0x80	/* in REG_HOUR */
#	define DS1340_BIT_CENTURY	0x40	/* in REG_HOUR */
#define DS1307_REG_WDAY		0x03	/* 01-07 */
#define DS1307_REG_MDAY		0x04	/* 01-31 */
#define DS1307_REG_MONTH	0x05	/* 01-12 */
#	define DS1337_BIT_CENTURY	0x80	/* in REG_MONTH */
#define DS1307_REG_YEAR		0x06	/* 00-99 */

/*
 * Other registers (control, status, alarms, trickle charge, NVRAM, etc)
 * start at 7, and they differ a LOT. Only control and status matter for
 * basic RTC date and time functionality; be careful using them.
 */
#define DS1307_REG_CONTROL	0x07		/* or ds1338 */
#	define DS1307_BIT_OUT		0x80
#	define DS1338_BIT_OSF		0x20
#	define DS1307_BIT_SQWE		0x10
#	define DS1307_BIT_RS1		0x02
#	define DS1307_BIT_RS0		0x01

struct ds1307 {
	struct rtc_device	rtc;
	u8			offset; /* register's offset */
	u8			regs[11];
	enum ds_type		type;
	unsigned long		flags;
	struct i2c_client	*client;
	s32 (*read_block_data)(const struct i2c_client *client, u8 command,
			       u8 length, u8 *values);
	s32 (*write_block_data)(const struct i2c_client *client, u8 command,
				u8 length, const u8 *values);
};

static struct platform_device_id ds1307_id[] = {
	{ "ds1307", ds_1307 },
	{ "ds1338", ds_1338 },
	{ "pt7c4338", ds_1307 },
	{ }
};

/*----------------------------------------------------------------------*/

#define BLOCK_DATA_MAX_TRIES 10

static s32 ds1307_read_block_data_once(const struct i2c_client *client,
				       u8 command, u8 length, u8 *values)
{
	s32 i, data;

	for (i = 0; i < length; i++) {
		data = i2c_smbus_read_byte_data(client, command + i);
		if (data < 0)
			return data;
		values[i] = data;
	}
	return i;
}

static s32 ds1307_read_block_data(const struct i2c_client *client, u8 command,
				  u8 length, u8 *values)
{
	u8 oldvalues[255];
	s32 ret;
	int tries = 0;

	dev_dbg(&client->dev, "ds1307_read_block_data (length=%d)\n", length);
	ret = ds1307_read_block_data_once(client, command, length, values);
	if (ret < 0)
		return ret;
	do {
		if (++tries > BLOCK_DATA_MAX_TRIES) {
			dev_err(&client->dev,
				"ds1307_read_block_data failed\n");
			return -EIO;
		}
		memcpy(oldvalues, values, length);
		ret = ds1307_read_block_data_once(client, command, length,
						  values);
		if (ret < 0)
			return ret;
	} while (memcmp(oldvalues, values, length));
	return length;
}

static s32 ds1307_write_block_data(const struct i2c_client *client, u8 command,
				   u8 length, const u8 *values)
{
	u8 currvalues[255];
	int tries = 0;

	dev_dbg(&client->dev, "ds1307_write_block_data (length=%d)\n", length);
	do {
		s32 i, ret;

		if (++tries > BLOCK_DATA_MAX_TRIES) {
			dev_err(&client->dev,
				"ds1307_write_block_data failed\n");
			return -EIO;
		}
		for (i = 0; i < length; i++) {
			ret = i2c_smbus_write_byte_data(client, command + i,
							values[i]);
			if (ret < 0)
				return ret;
		}
		ret = ds1307_read_block_data_once(client, command, length,
						  currvalues);
		if (ret < 0)
			return ret;
	} while (memcmp(currvalues, values, length));
	return length;
}

static inline struct ds1307 *to_ds1307_priv(struct rtc_device *rtcdev)
{
	return container_of(rtcdev, struct ds1307, rtc);
}

static int ds1307_get_time(struct rtc_device *rtcdev, struct rtc_time *t)
{
	struct device_d *dev = rtcdev->dev;
	struct ds1307 *ds1307 = to_ds1307_priv(rtcdev);
	int		tmp;

	/* read the RTC date and time registers all at once */
	tmp = ds1307->read_block_data(ds1307->client,
		ds1307->offset, 7, ds1307->regs);
	if (tmp != 7) {
		dev_err(dev, "%s error %d\n", "read", tmp);
		return -EIO;
	}

	dev_dbg(dev, "%s: %7ph\n", "read", ds1307->regs);

	t->tm_sec = bcd2bin(ds1307->regs[DS1307_REG_SECS] & 0x7f);
	t->tm_min = bcd2bin(ds1307->regs[DS1307_REG_MIN] & 0x7f);
	tmp = ds1307->regs[DS1307_REG_HOUR] & 0x3f;
	t->tm_hour = bcd2bin(tmp);
	t->tm_wday = bcd2bin(ds1307->regs[DS1307_REG_WDAY] & 0x07) - 1;
	t->tm_mday = bcd2bin(ds1307->regs[DS1307_REG_MDAY] & 0x3f);
	tmp = ds1307->regs[DS1307_REG_MONTH] & 0x1f;
	t->tm_mon = bcd2bin(tmp) - 1;

	/* assume 20YY not 19YY, and ignore DS1337_BIT_CENTURY */
	t->tm_year = bcd2bin(ds1307->regs[DS1307_REG_YEAR]) + 100;

	dev_dbg(dev, "%s secs=%d, mins=%d, "
		"hours=%d, mday=%d, mon=%d, year=%d, wday=%d\n",
		"read", t->tm_sec, t->tm_min,
		t->tm_hour, t->tm_mday,
		t->tm_mon, t->tm_year, t->tm_wday);

	/* initial clock setting can be undefined */
	return rtc_valid_tm(t);
}

static int ds1307_set_time(struct rtc_device *rtcdev, struct rtc_time *t)
{
	struct device_d *dev = rtcdev->dev;
	struct ds1307 *ds1307 = to_ds1307_priv(rtcdev);
	int		result;
	int		tmp;
	u8		*buf = ds1307->regs;

	dev_dbg(dev, "%s secs=%d, mins=%d, "
		"hours=%d, mday=%d, mon=%d, year=%d, wday=%d\n",
		"write", t->tm_sec, t->tm_min,
		t->tm_hour, t->tm_mday,
		t->tm_mon, t->tm_year, t->tm_wday);

	buf[DS1307_REG_SECS] = bin2bcd(t->tm_sec);
	buf[DS1307_REG_MIN] = bin2bcd(t->tm_min);
	buf[DS1307_REG_HOUR] = bin2bcd(t->tm_hour);
	buf[DS1307_REG_WDAY] = bin2bcd(t->tm_wday + 1);
	buf[DS1307_REG_MDAY] = bin2bcd(t->tm_mday);
	buf[DS1307_REG_MONTH] = bin2bcd(t->tm_mon + 1);

	/* assume 20YY not 19YY */
	tmp = t->tm_year - 100;
	buf[DS1307_REG_YEAR] = bin2bcd(tmp);

	dev_dbg(dev, "%s: %7ph\n", "write", buf);

	result = ds1307->write_block_data(ds1307->client,
		ds1307->offset, 7, buf);
	if (result < 0) {
		dev_err(dev, "%s error %d\n", "write", result);
		return result;
	}
	return 0;
}

static const struct rtc_class_ops ds13xx_rtc_ops = {
	.read_time	= ds1307_get_time,
	.set_time	= ds1307_set_time,
};

static int ds1307_probe(struct device_d *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct ds1307		*ds1307;
	int			err = -ENODEV;
	int			tmp;
	unsigned char		*buf;
	unsigned long driver_data;

	ds1307 = xzalloc(sizeof(struct ds1307));

	err = dev_get_drvdata(dev, &driver_data);
	if (err)
		goto exit;

	ds1307->client = client;
	ds1307->type = driver_data;

	buf = ds1307->regs;

	ds1307->read_block_data = ds1307_read_block_data;
	ds1307->write_block_data = ds1307_write_block_data;

read_rtc:
	/* read RTC registers */
	tmp = ds1307->read_block_data(client, ds1307->offset, 8, buf);
	if (tmp != 8) {
		dev_dbg(&client->dev, "read error %d\n", tmp);
		err = -EIO;
		goto exit;
	}

	/*
	 * minimal sanity checking; some chips (like DS1340) don't
	 * specify the extra bits as must-be-zero, but there are
	 * still a few values that are clearly out-of-range.
	 */
	tmp = ds1307->regs[DS1307_REG_SECS];
	switch (ds1307->type) {
	case ds_1307:
		/* clock halted?  turn it on, so clock can tick. */
		if (tmp & DS1307_BIT_CH) {
			i2c_smbus_write_byte_data(client, DS1307_REG_SECS, 0);
			dev_warn(&client->dev, "SET TIME!\n");
			goto read_rtc;
		}
		break;
	case ds_1338:
		/* clock halted?  turn it on, so clock can tick. */
		if (tmp & DS1307_BIT_CH)
			i2c_smbus_write_byte_data(client, DS1307_REG_SECS, 0);

		/* oscillator fault?  clear flag, and warn */
		if (ds1307->regs[DS1307_REG_CONTROL] & DS1338_BIT_OSF) {
			i2c_smbus_write_byte_data(client, DS1307_REG_CONTROL,
					ds1307->regs[DS1307_REG_CONTROL]
					& ~DS1338_BIT_OSF);
			dev_warn(&client->dev, "SET TIME!\n");
			goto read_rtc;
		}
		break;
	default:
		break;
	}

	tmp = ds1307->regs[DS1307_REG_HOUR];
	switch (ds1307->type) {
	default:
		if (!(tmp & DS1307_BIT_12HR))
			break;

		/*
		 * Be sure we're in 24 hour mode.  Multi-master systems
		 * take note...
		 */
		tmp = bcd2bin(tmp & 0x1f);
		if (tmp == 12)
			tmp = 0;
		if (ds1307->regs[DS1307_REG_HOUR] & DS1307_BIT_PM)
			tmp += 12;
		i2c_smbus_write_byte_data(client,
				ds1307->offset + DS1307_REG_HOUR,
				bin2bcd(tmp));
	}

	ds1307->rtc.ops = &ds13xx_rtc_ops;
	ds1307->rtc.dev = dev;

	err = rtc_register(&ds1307->rtc);

exit:
	return err;
}

static struct driver_d ds1307_driver = {
	.name	= "rtc-ds1307",
	.probe		= ds1307_probe,
	.id_table	= ds1307_id,
};

static int __init ds1307_init(void)
{
	return i2c_driver_register(&ds1307_driver);
}
device_initcall(ds1307_init);
