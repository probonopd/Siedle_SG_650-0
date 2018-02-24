/*
 * Copyright (C) 2013 Sascha Hauer, Pengutronix
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation.
 *
 */

#include <environment.h>
#include <bootsource.h>
#include <partition.h>
#include <common.h>
#include <sizes.h>
#include <gpio.h>
#include <init.h>
#include <of.h>
#include <mach/bbu.h>
#include <fec.h>
#include <nand.h>
#include <globalvar.h>

#include <linux/micrel_phy.h>

#include <mach/iomux-mx6.h>
#include <mach/imx6.h>
#include <mach/bbu.h>

#define PHYFLEX_MODULE_REV_1	0x1
#define PHYFLEX_MODULE_REV_2	0x2

#define GPIO_2_11_PD_CTL	MX6_PAD_CTL_PUS_100K_DOWN | MX6_PAD_CTL_PUE | MX6_PAD_CTL_PKE | \
				MX6_PAD_CTL_SPEED_MED | MX6_PAD_CTL_DSE_40ohm | MX6_PAD_CTL_HYS

#define MX6Q_PAD_SD4_DAT3__GPIO_2_11_PD (_MX6Q_PAD_SD4_DAT3__GPIO_2_11 | MUX_PAD_CTRL(GPIO_2_11_PD_CTL))
#define MX6DL_PAD_SD4_DAT3__GPIO_2_11 IOMUX_PAD(0x0734, 0x034C, 5, 0x0000, 0, GPIO_2_11_PD_CTL)

#define MX6_PHYFLEX_ERR006282	IMX_GPIO_NR(2, 11)

static void phyflex_err006282_workaround(void)
{
	/*
	 * Boards beginning with 1362.2 have the SD4_DAT3 pin connected
	 * to the CMIC. If this pin isn't toggled within 10s the boards
	 * reset. The pin is unconnected on older boards, so we do not
	 * need a check for older boards before applying this fixup.
	 */

	gpio_direction_output(MX6_PHYFLEX_ERR006282, 0);
	mdelay(2);
	gpio_direction_output(MX6_PHYFLEX_ERR006282, 1);
	mdelay(2);
	gpio_set_value(MX6_PHYFLEX_ERR006282, 0);

	if (cpu_is_mx6q())
		mxc_iomux_v3_setup_pad(MX6Q_PAD_SD4_DAT3__GPIO_2_11_PD);
	else if (cpu_is_mx6dl())
		mxc_iomux_v3_setup_pad(MX6DL_PAD_SD4_DAT3__GPIO_2_11);

	gpio_direction_input(MX6_PHYFLEX_ERR006282);
}

static unsigned int get_module_rev(void)
{
	unsigned int val = 0;

	val = gpio_get_value(IMX_GPIO_NR(2, 12));
	val |= (gpio_get_value(IMX_GPIO_NR(2, 13)) << 1);
	val |= (gpio_get_value(IMX_GPIO_NR(2, 14)) << 2);
	val |= (gpio_get_value(IMX_GPIO_NR(2, 15)) << 3);

	return 16 - val;
}

static void mmd_write_reg(struct phy_device *dev, int device, int reg, int val)
{
	phy_write(dev, 0x0d, device);
	phy_write(dev, 0x0e, reg);
	phy_write(dev, 0x0d, (1 << 14) | device);
	phy_write(dev, 0x0e, val);
}

static int ksz9031rn_phy_fixup(struct phy_device *dev)
{
	if (get_module_rev() == PHYFLEX_MODULE_REV_2)
		mmd_write_reg(dev, 2, 8, 0x039F);

	return 0;
}

#include <mach/imx6-regs.h>
#include <mfd/imx6q-iomuxc-gpr.h>

/* Output 50 MHz enet_ref clk on pad GPIO_16 */
static int imx6_rmii_init(void)
{
	u32 val;
	void __iomem *base = (void *)MX6_IOMUXC_BASE_ADDR;

	val = readl(base + IOMUXC_GPR1) | IMX6Q_GPR1_ENET_CLK_SEL_ANATOP;
	writel(val, base + IOMUXC_GPR1);

	return 0;
}

u32 bootcount;
#define MX6_SNVS_LPGPR 0x68

static int bootcount_set(struct param_d *param, void *priv)
{
	void __iomem *base = priv;
	writel(bootcount, base + MX6_SNVS_LPGPR);

	return 0;
}

static int bootcount_get(struct param_d *param, void *priv)
{
	void __iomem *base = priv;
	bootcount = readl(base + MX6_SNVS_LPGPR);

	return 0;
}

static void update_bootcount(void)
{
	struct param_d *param;
	void *priv = (void *)MX6_SNVS_BASE_ADDR;

	param = dev_add_param_int(&global_device, "system.bootcount", bootcount_set,
				  bootcount_get, &bootcount, "%u", priv);
	if (!param)
		return;

	bootcount_get(param, priv);
	bootcount++;
	bootcount_set(param, priv);
}

static int phytec_pfla02_init(void)
{
	char *envdev;

	if (!of_machine_is_compatible("phytec,imx6q-pfla02") &&
			!of_machine_is_compatible("phytec,imx6dl-pfla02") &&
			!of_machine_is_compatible("phytec,imx6s-pfla02") &&
			!of_machine_is_compatible("siedle,imx6q-avp"))
		return 0;

	phyflex_err006282_workaround();

	printf("Module Revision: %u\n", get_module_rev());

	imx6_bbu_internal_spi_i2c_register_handler("spiflash", "/dev/m25p0.barebox",
				BBU_HANDLER_FLAG_DEFAULT);

	if (of_machine_is_compatible("siedle,imx6q-dcip2")) {
		barebox_set_hostname("dcip2");
		imx6_bbu_internal_mmc_register_handler("mmc0", "/dev/mmc0", 0);
	}
	if (of_machine_is_compatible("phytec,imx6x-pbab01")) {
		barebox_set_hostname("pbab01");
		imx6_bbu_internal_mmc_register_handler("mmc2", "/dev/mmc2", 0);
	}
	if (of_machine_is_compatible("siedle,imx6q-bipg650-0m")) {
		barebox_set_hostname("bipg650");
		imx6_bbu_internal_mmc_register_handler("mmc2", "/dev/mmc2", 0);
	}
	if (of_machine_is_compatible("siedle,imx6q-avp")) {
		barebox_set_hostname("avp");
		imx6_bbu_internal_mmc_register_handler("mmc2", "/dev/mmc2", 0);
	}

	phy_register_fixup_for_uid(PHY_ID_KSZ9031, 0x00fffff0,
				   ksz9031rn_phy_fixup);

	imx6_bbu_nand_register_handler("nand", BBU_HANDLER_FLAG_DEFAULT);

	switch (bootsource_get()) {
	case BOOTSOURCE_NAND:
		devfs_add_partition("nand0", 0, SZ_512K,
				DEVFS_PARTITION_FIXED, "self_raw");
		dev_add_bb_dev("self_raw", "self0");
		devfs_add_partition("nand0", SZ_512K, SZ_128K,
				DEVFS_PARTITION_FIXED, "env_raw");
		dev_add_bb_dev("env_raw", "env0");
		envdev = "NAND";
		break;
        case BOOTSOURCE_MMC:
		device_detect_by_name("mmc2");
		of_device_enable_path("/chosen/environment-mmc");
		devfs_add_partition("mmc2", 0, SZ_512K,
					DEVFS_PARTITION_FIXED, "self0");
		devfs_add_partition("mmc2", SZ_512K, SZ_64K,
					DEVFS_PARTITION_FIXED, "env0");
		envdev = "MMC";
                break;
        default:
        case BOOTSOURCE_SPI:
                of_device_enable_path("/chosen/environment-spi");
		devfs_add_partition("m25p0", 0, SZ_512K, DEVFS_PARTITION_FIXED, "self0");
		devfs_add_partition("m25p0", SZ_512K, SZ_64K,
			DEVFS_PARTITION_FIXED, "env0");
		envdev = "SPI-NOR";
                break;
        }

	pr_notice("Using environment in %s Flash\n", envdev);

	update_bootcount();

	return 0;
}
device_initcall(phytec_pfla02_init);

static int siedle_avp_core_init(void)
{
	if (!of_machine_is_compatible("siedle,imx6q-avp"))
		return 0;

	imx6_rmii_init();

	return 0;
}
postcore_initcall(siedle_avp_core_init);
