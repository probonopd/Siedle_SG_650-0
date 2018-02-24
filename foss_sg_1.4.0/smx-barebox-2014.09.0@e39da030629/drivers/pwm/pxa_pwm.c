/*
 * simple driver for PWM (Pulse Width Modulator) controller
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * 2008-02-13	initial version eric miao <eric.miao@marvell.com>
 * 2012         Robert Jarzmik <robert.jarzmik@free.fr>
 */

#include <common.h>
#include <init.h>
#include <errno.h>
#include <io.h>
#include <pwm.h>

#include <mach/hardware.h>
#include <mach/clock.h>
#include <mach/pxa-regs.h>
#include <mach/regs-pwm.h>
#include <asm-generic/div64.h>
#include <linux/compiler.h>

/* PWM registers and bits definitions */
#define PWMCR		(0x00)
#define PWMDCR		(0x04)
#define PWMPCR		(0x08)

#define PWMCR_SD	(1 << 6)
#define PWMDCR_FD	(1 << 10)

struct pxa_pwm_chip {
	struct pwm_chip chip;
	void __iomem *iobase;
	int id;
	int duty_ns;
	int period_ns;
};

static struct pxa_pwm_chip *to_pxa_pwm_chip(struct pwm_chip *chip)
{
	return container_of(chip, struct pxa_pwm_chip, chip);
}

/*
 * period_ns    = 10^9 * (PRESCALE + 1) * (PV + 1) / PWM_CLK_RATE
 * duty_ns      = 10^9 * (PRESCALE + 1) * DC / PWM_CLK_RATE
 * PWM_CLK_RATE = 13 MHz
 */
static int pxa_pwm_config(struct pwm_chip *chip, int duty_ns, int period_ns)
{
	unsigned long long c;
	unsigned long period_cycles, prescale, pv, dc;
	struct pxa_pwm_chip *pxa_pwm = to_pxa_pwm_chip(chip);

	c = pxa_get_pwmclk();
	c = c * period_ns;
	do_div(c, 1000000000);
	period_cycles = c;

	if (period_cycles < 1)
		period_cycles = 1;
	prescale = (period_cycles - 1) / 1024;
	pv = period_cycles / (prescale + 1) - 1;

	if (prescale > 63)
		return -EINVAL;

	if (duty_ns == period_ns)
		dc = PWMDCR_FD;
	else
		dc = (pv + 1) * duty_ns / period_ns;

	pxa_pwm->duty_ns = duty_ns;
	pxa_pwm->period_ns = period_ns;

	/* NOTE: the clock to PWM has to be enabled first
	 * before writing to the registers
	 */
	__raw_writel(prescale, pxa_pwm->iobase + PWMCR);
	__raw_writel(dc, pxa_pwm->iobase + PWMDCR);
	__raw_writel(pv, pxa_pwm->iobase + PWMPCR);

	return 0;
}

static int pxa_pwm_enable(struct pwm_chip *chip)
{
	struct pxa_pwm_chip *pxa_pwm = to_pxa_pwm_chip(chip);

	switch (pxa_pwm->id) {
	case 0:
	case 2:
		CKEN |= CKEN_PWM0;
		break;
	case 1:
	case 3:
		CKEN |= CKEN_PWM1;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static void pxa_pwm_disable(struct pwm_chip *chip)
{
	struct pxa_pwm_chip *pxa_pwm = to_pxa_pwm_chip(chip);

	switch (pxa_pwm->id) {
	case 0:
	case 2:
		CKEN &= ~CKEN_PWM0;
		break;
	case 1:
	case 3:
		CKEN &= ~CKEN_PWM1;
		break;
	default:
		break;
	}
}

static struct pwm_ops pxa_pwm_ops = {
	.config = pxa_pwm_config,
	.enable = pxa_pwm_enable,
	.disable = pxa_pwm_disable,
};

static int pxa_pwm_probe(struct device_d *dev)
{
	struct pxa_pwm_chip *chip;

	chip = xzalloc(sizeof(*chip));
	chip->chip.devname = asprintf("pwm%d", dev->id);
	chip->chip.ops = &pxa_pwm_ops;
	chip->iobase = dev_request_mem_region(dev, 0);
	chip->id = dev->id;
	dev->priv = chip;

	return pwmchip_add(&chip->chip, dev);
}

static struct driver_d pxa_pwm_driver = {
	.name  = "pxa_pwm",
	.probe = pxa_pwm_probe,
};

static int __init pxa_pwm_init_driver(void)
{
	CKEN &= ~CKEN_PWM0 & ~CKEN_PWM1;
	platform_driver_register(&pxa_pwm_driver);
	return 0;
}

device_initcall(pxa_pwm_init_driver);
