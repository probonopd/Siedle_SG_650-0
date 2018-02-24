/*
 * i.MX drm driver - parallel display implementation
 *
 * Copyright (C) 2012 Sascha Hauer, Pengutronix
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <common.h>
#include <fb.h>
#include <io.h>
#include <driver.h>
#include <malloc.h>
#include <errno.h>
#include <init.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <asm-generic/div64.h>
#include <linux/clk.h>
#include <mach/imx6-regs.h>
#include <mach/imx53-regs.h>

#include "imx-ipu-v3.h"
#include "ipuv3-plane.h"

#define LDB_CH0_MODE_EN_TO_DI0		(1 << 0)
#define LDB_CH0_MODE_EN_TO_DI1		(3 << 0)
#define LDB_CH0_MODE_EN_MASK		(3 << 0)
#define LDB_CH1_MODE_EN_TO_DI0		(1 << 2)
#define LDB_CH1_MODE_EN_TO_DI1		(3 << 2)
#define LDB_CH1_MODE_EN_MASK		(3 << 2)
#define LDB_SPLIT_MODE_EN		(1 << 4)
#define LDB_DATA_WIDTH_CH0_24		(1 << 5)
#define LDB_BIT_MAP_CH0_JEIDA		(1 << 6)
#define LDB_DATA_WIDTH_CH1_24		(1 << 7)
#define LDB_BIT_MAP_CH1_JEIDA		(1 << 8)
#define LDB_DI0_VS_POL_ACT_LOW		(1 << 9)
#define LDB_DI1_VS_POL_ACT_LOW		(1 << 10)
#define LDB_BGREF_RMODE_INT		(1 << 15)

struct imx_ldb;

struct imx_ldb_channel {
	struct imx_ldb *ldb;
	int chno;
	int mode_valid;
	struct display_timings *modes;
	struct ipu_output output;
};

struct imx_ldb_data {
	void __iomem *base;
	int (*prepare)(struct imx_ldb_channel *imx_ldb_ch, int di);
	unsigned ipu_mask;
};

struct imx_ldb {
	struct device_d *dev;
	u32 interface_pix_fmt;
	int mode_valid;
	struct imx_ldb_channel channel[2];
	u32 ldb_ctrl;
	void __iomem *base;
	const struct imx_ldb_data *soc_data;
};

enum {
	LVDS_BIT_MAP_SPWG,
	LVDS_BIT_MAP_JEIDA
};

static const char * const imx_ldb_bit_mappings[] = {
	[LVDS_BIT_MAP_SPWG]  = "spwg",
	[LVDS_BIT_MAP_JEIDA] = "jeida",
};

static const int of_get_data_mapping(struct device_node *np)
{
	const char *bm;
	int ret, i;

	ret = of_property_read_string(np, "fsl,data-mapping", &bm);
	if (ret < 0)
		return ret;

	for (i = 0; i < ARRAY_SIZE(imx_ldb_bit_mappings); i++)
		if (!strcasecmp(bm, imx_ldb_bit_mappings[i]))
			return i;

	return -EINVAL;
}

static int imx_ldb_prepare(struct ipu_output *output, struct fb_videomode *mode, int di)
{
	struct imx_ldb_channel *imx_ldb_ch = container_of(output, struct imx_ldb_channel, output);
	struct imx_ldb *ldb = imx_ldb_ch->ldb;

	if (PICOS2KHZ(mode->pixclock) > 85000) {
		dev_warn(ldb->dev,
			 "%s: mode exceeds 85 MHz pixel clock\n", __func__);
	}

	ldb->soc_data->prepare(imx_ldb_ch, di);

	/* FIXME - assumes straight connections DI0 --> CH0, DI1 --> CH1 */
	if (imx_ldb_ch == &ldb->channel[0]) {
		if (mode->sync & FB_SYNC_VERT_HIGH_ACT)
			ldb->ldb_ctrl |= LDB_DI0_VS_POL_ACT_LOW;
		else
			ldb->ldb_ctrl &= ~LDB_DI0_VS_POL_ACT_LOW;
	}

	if (imx_ldb_ch == &ldb->channel[1]) {
		if (mode->sync & FB_SYNC_VERT_HIGH_ACT)
			ldb->ldb_ctrl |= LDB_DI1_VS_POL_ACT_LOW;
		else
			ldb->ldb_ctrl &= ~LDB_DI1_VS_POL_ACT_LOW;
	}

	if (imx_ldb_ch == &ldb->channel[0]) {
		ldb->ldb_ctrl &= ~LDB_CH0_MODE_EN_MASK;
		ldb->ldb_ctrl |= LDB_CH0_MODE_EN_TO_DI0;
	}

	if (imx_ldb_ch == &ldb->channel[1]) {
		ldb->ldb_ctrl &= ~LDB_CH1_MODE_EN_MASK;
		ldb->ldb_ctrl |= LDB_CH1_MODE_EN_TO_DI1;
	}

	writel(ldb->ldb_ctrl, ldb->base);

	return 0;
}

static int imx6q_ldb_prepare(struct imx_ldb_channel *imx_ldb_ch, int di)
{
	struct clk *diclk, *ldbclk;
	struct imx_ldb *ldb = imx_ldb_ch->ldb;
	int ret, ipuno, dino;
	char *clkname;
	void __iomem *gpr3 = (void *)MX6_IOMUXC_BASE_ADDR + 0xc;
	uint32_t val;
	int shift;

	ipuno = ((di >> 1) & 1) + 1;
	dino = di & 0x1;

	clkname = asprintf("ipu%d_di%d_sel", ipuno, dino);
	diclk = clk_lookup(clkname);
	free(clkname);
	if (IS_ERR(diclk)) {
		dev_err(ldb->dev, "failed to get di clk: %s\n", strerror(PTR_ERR(diclk)));
		return PTR_ERR(diclk);
	}

	clkname = asprintf("ldb_di%d_podf", imx_ldb_ch->chno);
	ldbclk = clk_lookup(clkname);
	free(clkname);
	if (IS_ERR(ldbclk)) {
		dev_err(ldb->dev, "failed to get ldb clk: %s\n", strerror(PTR_ERR(ldbclk)));
		return PTR_ERR(ldbclk);
	}

	ret = clk_set_parent(diclk, ldbclk);
	if (ret) {
		dev_err(ldb->dev, "failed to set display clock parent: %s\n", strerror(-ret));
		return ret;
	}

	val = readl(gpr3);
	shift = (imx_ldb_ch->chno == 0) ? 6 : 8;
	val &= ~(3 << shift);
	val |= di << shift;
	writel(val, gpr3);

	return 0;
}

static int imx53_ldb_prepare(struct imx_ldb_channel *imx_ldb_ch, int di)
{
	struct clk *diclk, *ldbclk;
	struct imx_ldb *ldb = imx_ldb_ch->ldb;
	int ret, dino;
	char *clkname;

	dino = di & 0x1;

	clkname = asprintf("ipu_di%d_sel", dino);
	diclk = clk_lookup(clkname);
	free(clkname);
	if (IS_ERR(diclk)) {
		dev_err(ldb->dev, "failed to get di clk: %s\n", strerror(PTR_ERR(diclk)));
		return PTR_ERR(diclk);
	}

	clkname = asprintf("ldb_di%d_div", imx_ldb_ch->chno);
	ldbclk = clk_lookup(clkname);
	free(clkname);
	if (IS_ERR(ldbclk)) {
		dev_err(ldb->dev, "failed to get ldb clk: %s\n", strerror(PTR_ERR(ldbclk)));
		return PTR_ERR(ldbclk);
	}

	ret = clk_set_parent(diclk, ldbclk);
	if (ret) {
		dev_err(ldb->dev, "failed to set display clock parent: %s\n", strerror(-ret));
		return ret;
	}

	return 0;
}

static struct imx_ldb_data imx_ldb_data_imx6q = {
	.base = (void *)MX6_IOMUXC_BASE_ADDR + 0x8,
	.prepare = imx6q_ldb_prepare,
	.ipu_mask = 0xf,
};

static struct imx_ldb_data imx_ldb_data_imx53 = {
	.base = (void *)MX53_IOMUXC_BASE_ADDR + 0x8,
	.prepare = imx53_ldb_prepare,
	.ipu_mask = 0x3,
};

static struct ipu_output_ops imx_ldb_ops = {
	.prepare = imx_ldb_prepare,
};

static int imx_ldb_probe(struct device_d *dev)
{
	struct device_node *np = dev->device_node;
	struct device_node *child;
	struct imx_ldb *imx_ldb;
	int ret, i;
	int dual = 0;
	int datawidth;
	int mapping;
	const struct imx_ldb_data *devtype;

	ret = dev_get_drvdata(dev, (unsigned long *)&devtype);
	if (ret)
		return ret;

	imx_ldb = xzalloc(sizeof(*imx_ldb));
	imx_ldb->base = devtype->base;
	imx_ldb->soc_data = devtype;

	for_each_child_of_node(np, child) {
		struct imx_ldb_channel *channel;

		ret = of_property_read_u32(child, "reg", &i);
		if (ret || i < 0 || i > 1)
			return -EINVAL;

		if (dual && i > 0) {
			dev_warn(dev, "dual-channel mode, ignoring second output\n");
			continue;
		}

		if (!of_device_is_available(child))
			continue;

		channel = &imx_ldb->channel[i];
		channel->ldb = imx_ldb;
		channel->chno = i;

		ret = of_property_read_u32(child, "fsl,data-width", &datawidth);
		if (ret)
			datawidth = 0;
		else if (datawidth != 18 && datawidth != 24)
			return -EINVAL;

		mapping = of_get_data_mapping(child);
		switch (mapping) {
		case LVDS_BIT_MAP_SPWG:
			if (datawidth == 24) {
				if (i == 0 || dual)
					imx_ldb->ldb_ctrl |= LDB_DATA_WIDTH_CH0_24;
				if (i == 1 || dual)
					imx_ldb->ldb_ctrl |= LDB_DATA_WIDTH_CH1_24;
			}
			break;
		case LVDS_BIT_MAP_JEIDA:
			if (datawidth == 18) {
				dev_err(dev, "JEIDA standard only supported in 24 bit\n");
				return -EINVAL;
			}
			if (i == 0 || dual)
				imx_ldb->ldb_ctrl |= LDB_DATA_WIDTH_CH0_24 | LDB_BIT_MAP_CH0_JEIDA;
			if (i == 1 || dual)
				imx_ldb->ldb_ctrl |= LDB_DATA_WIDTH_CH1_24 | LDB_BIT_MAP_CH1_JEIDA;
			break;
		default:
			dev_err(dev, "data mapping not specified or invalid\n");
			return -EINVAL;
		}

		channel->output.ops = &imx_ldb_ops;
		channel->output.di_clkflags = IPU_DI_CLKMODE_EXT | IPU_DI_CLKMODE_SYNC;
		channel->output.out_pixel_fmt = (datawidth == 24) ?
			V4L2_PIX_FMT_RGB24 : V4L2_PIX_FMT_BGR666;
		channel->output.modes = of_get_display_timings(child);
		channel->output.name = asprintf("ldb-%d", i);
		channel->output.ipu_mask = devtype->ipu_mask;

		ipu_register_output(&channel->output);
	}

	return 0;
}

static struct of_device_id imx_ldb_dt_ids[] = {
	{ .compatible = "fsl,imx6q-ldb", (unsigned long)&imx_ldb_data_imx6q},
	{ .compatible = "fsl,imx53-ldb", (unsigned long)&imx_ldb_data_imx53},
	{ /* sentinel */ }
};

static struct driver_d imx_ldb_driver = {
	.probe		= imx_ldb_probe,
	.of_compatible	= imx_ldb_dt_ids,
	.name		= "imx-ldb",
};
device_platform_driver(imx_ldb_driver);

MODULE_DESCRIPTION("i.MX LVDS display driver");
MODULE_AUTHOR("Sascha Hauer, Pengutronix");
MODULE_LICENSE("GPL");
