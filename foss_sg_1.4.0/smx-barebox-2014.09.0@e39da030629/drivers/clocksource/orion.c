/*
 * Copyright
 * (C) 2013 Sebastian Hesselbarth <sebastian.hesselbarth@gmail.com>
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

#include <common.h>
#include <init.h>
#include <clock.h>
#include <linux/clk.h>
#include <io.h>

#define TIMER_CTRL		0x00
#define  TIMER0_EN		BIT(0)
#define  TIMER0_RELOAD_EN	BIT(1)
#define  TIMER1_EN		BIT(2)
#define  TIMER1_RELOAD_EN	BIT(3)
#define TIMER0_RELOAD		0x10
#define TIMER0_VAL		0x14
#define TIMER1_RELOAD		0x18
#define TIMER1_VAL		0x1c

static __iomem void *timer_base;

static uint64_t orion_clocksource_read(void)
{
	return 0 - __raw_readl(timer_base + TIMER0_VAL);
}

static struct clocksource clksrc = {
	.read	= orion_clocksource_read,
	.mask	= CLOCKSOURCE_MASK(32),
	.shift	= 10,
};

static int orion_timer_probe(struct device_d *dev)
{
	struct clk *tclk;
	uint32_t val;

	timer_base = dev_request_mem_region(dev, 0);
	tclk = clk_get(dev, NULL);

	/* setup TIMER0 as free-running clock source */
	__raw_writel(~0, timer_base + TIMER0_VAL);
	__raw_writel(~0, timer_base + TIMER0_RELOAD);
	val = __raw_readl(timer_base + TIMER_CTRL);
	__raw_writel(val | TIMER0_EN | TIMER0_RELOAD_EN,
		     timer_base + TIMER_CTRL);

	clksrc.mult = clocksource_hz2mult(clk_get_rate(tclk), clksrc.shift);
	init_clock(&clksrc);

	return 0;
}

static struct of_device_id orion_timer_dt_ids[] = {
	{ .compatible = "marvell,orion-timer", },
	{ }
};

static struct driver_d orion_timer_driver = {
	.name = "orion-timer",
	.probe = orion_timer_probe,
	.of_compatible = DRV_OF_COMPAT(orion_timer_dt_ids),
};

static int orion_timer_init(void)
{
	return platform_driver_register(&orion_timer_driver);
}
postcore_initcall(orion_timer_init);
