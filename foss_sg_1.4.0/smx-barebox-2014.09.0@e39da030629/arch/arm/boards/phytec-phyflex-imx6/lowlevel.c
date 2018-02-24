/*
 * Copyright (C) 2013 Sascha Hauer <s.hauer@pengutronix.de>
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
#include <image-metadata.h>
#include <asm/barebox-arm-head.h>
#include <asm/barebox-arm.h>
#include <asm/sections.h>
#include <asm/cache.h>
#include <asm/mmu.h>
#include <mach/imx6-mmdc.h>
#include <mach/imx6.h>

static inline void setup_uart(void)
{
	void __iomem *ccmbase = (void *)MX6_CCM_BASE_ADDR;
	void __iomem *uartbase = (void *)MX6_UART5_BASE_ADDR;
	void __iomem *iomuxbase = (void *)MX6_IOMUXC_BASE_ADDR;

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

extern char __dtb_imx6q_phytec_pbab01_start[];
extern char __dtb_imx6dl_phytec_pbab01_start[];
extern char __dtb_imx6s_phytec_pbab01_start[];
extern char __dtb_imx6q_siedle_dcip2_evalboard_start[];
extern char __dtb_imx6q_siedle_bipg650_0m_start[];
extern char __dtb_imx6q_siedle_avp_start[];

BAREBOX_IMD_TAG_STRING(phyflex_mx6_memsize_512M, IMD_TYPE_PARAMETER, "memsize=512", 0);
BAREBOX_IMD_TAG_STRING(phyflex_mx6_memsize_1G, IMD_TYPE_PARAMETER, "memsize=1024", 0);
BAREBOX_IMD_TAG_STRING(phyflex_mx6_memsize_2G, IMD_TYPE_PARAMETER, "memsize=2048", 0);
BAREBOX_IMD_TAG_STRING(phyflex_mx6_memsize_4G, IMD_TYPE_PARAMETER, "memsize=4096", 0);
BAREBOX_IMD_TAG_STRING(siedle_avp_memsize_512M, IMD_TYPE_PARAMETER, "memsize=512", 0);

ENTRY_FUNCTION(start_phytec_pbab01_1gib, r0, r1, r2)
{
	void *fdt;

	imx6_cpu_lowlevel_init();

	arm_setup_stack(0x00920000 - 8);

	IMD_USED(phyflex_mx6_memsize_1G);

	if (IS_ENABLED(CONFIG_DEBUG_LL))
		setup_uart();

	fdt = __dtb_imx6q_phytec_pbab01_start - get_runtime_offset();

	barebox_arm_entry(0x10000000, SZ_1G, fdt);
}

ENTRY_FUNCTION(start_phytec_pbab01_1gib_1bank, r0, r1, r2)
{
	void *fdt;

	imx6_cpu_lowlevel_init();

	arm_setup_stack(0x00920000 - 8);

	fdt = __dtb_imx6q_phytec_pbab01_start - get_runtime_offset();

	barebox_arm_entry(0x10000000, SZ_1G, fdt);
}

ENTRY_FUNCTION(start_phytec_pbab01_2gib, r0, r1, r2)
{
	void *fdt;

	imx6_cpu_lowlevel_init();

	arm_setup_stack(0x00920000 - 8);

	IMD_USED(phyflex_mx6_memsize_2G);

	if (IS_ENABLED(CONFIG_DEBUG_LL))
		setup_uart();

	fdt = __dtb_imx6q_phytec_pbab01_start - get_runtime_offset();

	barebox_arm_entry(0x10000000, SZ_2G, fdt);
}

ENTRY_FUNCTION(start_phytec_pbab01_4gib, r0, r1, r2)
{
	void *fdt;

	imx6_cpu_lowlevel_init();

	arm_setup_stack(0x00920000 - 8);

	IMD_USED(phyflex_mx6_memsize_4G);

	fdt = __dtb_imx6q_phytec_pbab01_start - get_runtime_offset();

	barebox_arm_entry(0x10000000, 0xEFFFFFF8, fdt);
}

ENTRY_FUNCTION(start_phytec_pbab01dl_1gib, r0, r1, r2)
{
	void *fdt;

	imx6_cpu_lowlevel_init();

	arm_setup_stack(0x00920000 - 8);

	IMD_USED(phyflex_mx6_memsize_1G);

	fdt = __dtb_imx6dl_phytec_pbab01_start - get_runtime_offset();

	barebox_arm_entry(0x10000000, SZ_1G, fdt);
}

ENTRY_FUNCTION(start_phytec_pbab01s_512mb, r0, r1, r2)
{
	void *fdt;

	imx6_cpu_lowlevel_init();

	arm_setup_stack(0x00920000 - 8);

	IMD_USED(phyflex_mx6_memsize_512M);

	fdt = __dtb_imx6s_phytec_pbab01_start - get_runtime_offset();

	barebox_arm_entry(0x10000000, SZ_512M, fdt);
}

ENTRY_FUNCTION(start_siedle_dcip2_evalboard, r0, r1, r2)
{
	void *fdt;

	imx6_cpu_lowlevel_init();

	arm_setup_stack(0x00920000 - 8);

	fdt = __dtb_imx6q_siedle_dcip2_evalboard_start - get_runtime_offset();

	barebox_arm_entry(0x10000000, SZ_1G, fdt);
}

ENTRY_FUNCTION(start_siedle_bipg650_0m, r0, r1, r2)
{
	void *fdt;

	imx6_cpu_lowlevel_init();

	arm_setup_stack(0x00920000 - 8);

	IMD_USED(phyflex_mx6_memsize_1G);

	fdt = __dtb_imx6q_siedle_bipg650_0m_start - get_runtime_offset();

	barebox_arm_entry(0x10000000, SZ_1G, fdt);
}

ENTRY_FUNCTION(start_siedle_bipg650_0m_1bank, r0, r1, r2)
{
	void *fdt;

	imx6_cpu_lowlevel_init();

	arm_setup_stack(0x00920000 - 8);

	IMD_USED(phyflex_mx6_memsize_1G);

	fdt = __dtb_imx6q_siedle_bipg650_0m_start - get_runtime_offset();

	barebox_arm_entry(0x10000000, SZ_1G, fdt);
}

ENTRY_FUNCTION(start_siedle_avp, r0, r1, r2)
{
	void *fdt;

	imx6_cpu_lowlevel_init();

	arm_setup_stack(0x00920000 - 8);

	IMD_USED(siedle_avp_memsize_512M);

	fdt = __dtb_imx6q_siedle_avp_start - get_runtime_offset();

	barebox_arm_entry(0x10000000, SZ_512M, fdt);
}
