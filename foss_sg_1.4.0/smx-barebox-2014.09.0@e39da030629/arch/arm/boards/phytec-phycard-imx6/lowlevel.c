/*
 * Copyright (C) 2014 Christian Hemp <c.hemp@phytec.de>
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
#include <debug_ll.h>
#include <common.h>
#include <sizes.h>
#include <io.h>
#include <asm/barebox-arm-head.h>
#include <asm/barebox-arm.h>
#include <asm/sections.h>
#include <asm/cache.h>
#include <asm/mmu.h>
#include <mach/imx6-mmdc.h>
#include <mach/imx6.h>

static inline void setup_uart(void)
{
	void __iomem *ccmbase = IOMEM(MX6_CCM_BASE_ADDR);
	void __iomem *uartbase = IOMEM(MX6_UART3_BASE_ADDR);
	void __iomem *iomuxbase = IOMEM(MX6_IOMUXC_BASE_ADDR);

	writel(0x4, iomuxbase + 0x01f8);

	writel(0xffffffff, ccmbase + 0x68);
	writel(0xffffffff, ccmbase + 0x6c);
	writel(0xffffffff, ccmbase + 0x70);
	writel(0xffffffff, ccmbase + 0x74);
	writel(0xffffffff, ccmbase + 0x78);
	writel(0xffffffff, ccmbase + 0x7c);
	writel(0xffffffff, ccmbase + 0x80);

	writel(0x00000000, uartbase + 0x80);
	writel(0x00004027, uartbase + 0x84);
	writel(0x00000704, uartbase + 0x88);
	writel(0x00000a81, uartbase + 0x90);
	writel(0x0000002b, uartbase + 0x9c);
	writel(0x00013880, uartbase + 0xb0);
	writel(0x0000047f, uartbase + 0xa4);
	writel(0x0000c34f, uartbase + 0xa8);
	writel(0x00000001, uartbase + 0x80);

	putc_ll('>');
}

extern char __dtb_imx6q_phytec_pbaa03_start[];

static void __noreturn start_imx6q_phytec_pbaa03_common(uint32_t size)
{
	void *fdt;

	imx6_cpu_lowlevel_init();

	arm_setup_stack(0x00920000 - 8);

	if (IS_ENABLED(CONFIG_DEBUG_LL))
		setup_uart();

	fdt = __dtb_imx6q_phytec_pbaa03_start - get_runtime_offset();

	barebox_arm_entry(0x10000000, size, fdt);
}

ENTRY_FUNCTION(start_phytec_pbaa03_1gib, r0, r1, r2)
{
	start_imx6q_phytec_pbaa03_common(SZ_1G);
}

ENTRY_FUNCTION(start_phytec_pbaa03_1gib_1bank, r0, r1, r2)
{
	start_imx6q_phytec_pbaa03_common(SZ_1G);
}

ENTRY_FUNCTION(start_phytec_pbaa03_2gib, r0, r1, r2)
{
	start_imx6q_phytec_pbaa03_common(SZ_2G);
}
