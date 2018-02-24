/*
 * MPC85xx Internal Memory Map
 *
 * Copyright 2007-2011 Freescale Semiconductor, Inc.
 *
 * Copyright(c) 2002,2003 Motorola Inc.
 * Xianghua Xiao (x.xiao@motorola.com)
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#ifndef __IMMAP_85xx__
#define __IMMAP_85xx__

#include <asm/types.h>
#include <asm/fsl_lbc.h>
#include <asm/fsl_ifc.h>
#include <asm/config.h>

#define MPC85xx_LOCAL_OFFSET	0x0000
#define MPC85xx_ECM_OFFSET	0x1000
#define MPC85xx_DDR_OFFSET	0x2000
#define MPC85xx_LBC_OFFSET	0x5000
#define MPC85xx_PCI1_OFFSET	0x8000

#define MPC85xx_GPIO_OFFSET	0xf000
#define MPC85xx_IFC_OFFSET	0x1e000
#define MPC85xx_L2_OFFSET	0x20000
#ifdef FSL_TSECV2
#define TSEC1_OFFSET		0xB0000
#else
#define TSEC1_OFFSET		0x24000
#endif

#define MPC85xx_PIC_OFFSET	0x40000
#define MPC85xx_GUTS_OFFSET	0xe0000

#define MPC85xx_LOCAL_ADDR	(CFG_IMMR + MPC85xx_LOCAL_OFFSET)
#define MPC85xx_ECM_ADDR	(CFG_IMMR + MPC85xx_ECM_OFFSET)
#define MPC85xx_GUTS_ADDR	(CFG_IMMR + MPC85xx_GUTS_OFFSET)
#define MPC85xx_DDR_ADDR	(CFG_IMMR + MPC85xx_DDR_OFFSET)
#define LBC_ADDR		(CFG_IMMR + MPC85xx_LBC_OFFSET)
#define IFC_ADDR		(CFG_IMMR + MPC85xx_IFC_OFFSET)
#define MPC85xx_GPIO_ADDR	(CFG_IMMR + MPC85xx_GPIO_OFFSET)
#define MPC85xx_L2_ADDR		(CFG_IMMR + MPC85xx_L2_OFFSET)
#define MPC8xxx_PIC_ADDR	(CFG_IMMR + MPC85xx_PIC_OFFSET)

/* Local-Access Registers */
#define MPC85xx_LOCAL_BPTR_OFFSET	0x20 /* Boot Page Translation */

/* ECM Registers */
#define MPC85xx_ECM_EEBPCR_OFFSET	0x00 /* ECM CCB Port Configuration */
#define MPC85xx_ECM_EEDR_OFFSET		0xE00 /* ECM error detect register  */
#define MPC85xx_ECM_EEER_OFFSET		0xE08 /* ECM error enable register  */

/*
 * DDR Memory Controller Register Offsets
 */
/* Chip Select 0, 1,2, 3 Memory Bounds */
#define MPC85xx_DDR_CS0_BNDS_OFFSET		0x000
#define MPC85xx_DDR_CS1_BNDS_OFFSET		0x008
#define MPC85xx_DDR_CS2_BNDS_OFFSET		0x010
#define MPC85xx_DDR_CS3_BNDS_OFFSET		0x018
/* Chip Select 0, 1, 2, 3 Configuration */
#define MPC85xx_DDR_CS0_CONFIG_OFFSET		0x080
#define MPC85xx_DDR_CS1_CONFIG_OFFSET		0x084
#define MPC85xx_DDR_CS2_CONFIG_OFFSET		0x088
#define MPC85xx_DDR_CS3_CONFIG_OFFSET		0x08c
/* Chip Select 0, 1, 2, 3 Configuration 2 */
#define MPC85xx_DDR_CS0_CONFIG_2_OFFSET		0x0c0
#define MPC85xx_DDR_CS1_CONFIG_2_OFFSET		0x0c4
#define MPC85xx_DDR_CS2_CONFIG_2_OFFSET		0x0c8
#define MPC85xx_DDR_CS3_CONFIG_2_OFFSET		0x0cc
/* SDRAM Timing Configuration 0, 1, 2, 3 */
#define MPC85xx_DDR_TIMING_CFG_3_OFFSET		0x100
#define MPC85xx_DDR_TIMING_CFG_0_OFFSET		0x104
#define MPC85xx_DDR_TIMING_CFG_1_OFFSET		0x108
#define MPC85xx_DDR_TIMING_CFG_2_OFFSET		0x10c
/* SDRAM Control Configuration */
#define MPC85xx_DDR_SDRAM_CFG_OFFSET		0x110
#define MPC85xx_DDR_SDRAM_CFG_2_OFFSET		0x114
/* SDRAM Mode Configuration */
#define MPC85xx_DDR_SDRAM_MODE_OFFSET		0x118
#define MPC85xx_DDR_SDRAM_MODE_2_OFFSET		0x11c
/* SDRAM Mode Control */
#define MPC85xx_DDR_SDRAM_MD_CNTL_OFFSET	0x120
/* SDRAM Interval Configuration */
#define MPC85xx_DDR_SDRAM_INTERVAL_OFFSET	0x124
/* SDRAM Data initialization */
#define MPC85xx_DDR_SDRAM_DATA_INIT_OFFSET	0x128
/* SDRAM Clock Control */
#define MPC85xx_DDR_SDRAM_CLK_CNTL_OFFSET	0x130
/* training init and extended addr */
#define MPC85xx_DDR_SDRAM_INIT_ADDR_OFFSET	0x148
#define MPC85xx_DDR_SDRAM_INIT_ADDR_EXT_OFFSET	0x14c
/* SDRAM Timing Configuration 4,5 */
#define MPC85xx_DDR_TIMING_CFG_4_OFFSET		0x160
#define MPC85xx_DDR_TIMING_CFG_5_OFFSET		0x164
/* DDR ZQ calibration control */
#define MPC85xx_DDR_ZQ_CNTL_OFFSET		0x170
/* DDR write leveling control */
#define MPC85xx_DDR_WRLVL_CNTL_OFFSET		0x174
/* Self Refresh Counter */
#define MPC85xx_DDR_SR_CNTL_OFFSET		0x17c
/* DDR SDRAM Register Control Word */
#define MPC85xx_DDR_SDRAM_RCW_1_OFFSET		0x180
#define MPC85xx_DDR_SDRAM_RCW_2_OFFSET		0x184
/* DDR write leveling control */
#define MPC85xx_DDR_WRLVL_CNTL_2_OFFSET		0x190
#define MPC85xx_DDR_WRLVL_CNTL_3_OFFSET		0x194
/* DDR Control Driver */
#define MPC85xx_DDR_DDRCDR1_OFFSET		0xb28
#define MPC85xx_DDR_DDRCDR2_OFFSET		0xb2c
/* DDR IP block revision */
#define MPC85xx_DDR_IP_REV1_OFFSET		0xbf8
#define MPC85xx_DDR_IP_REV2_OFFSET		0xbfc
/* Memory Error Disable */
#define MPC85xx_DDR_ERR_DISABLE_OFFSET		0xe44
#define MPC85xx_DDR_ERR_INT_EN_OFFSET		0xe48

#define DDR_OFF(REGNAME)	(MPC85xx_DDR_##REGNAME##_OFFSET)

/*
 * GPIO Register Offsets
 */
#define MPC85xx_GPIO_GPDIR	0x00
#define MPC85xx_GPIO_GPDAT	0x08
#define MPC85xx_GPIO_GPDIR_OFFSET	0x00
#define MPC85xx_GPIO_GPDAT_OFFSET	0x08

/* Global Utilities Registers */
#define MPC85xx_GPIOCR_OFFSET	0x30
#define		MPC85xx_GPIOCR_GPOUT	0x00000200
#define MPC85xx_GPOUTDR_OFFSET	0x40
#define		MPC85xx_GPIOBIT(i)	(1 << (31 - i))
#define MPC85xx_GPINDR_OFFSET	0x50

#define MPC85xx_DEVDISR_OFFSET	0x70
#define		MPC85xx_DEVDISR_TSEC1	0x00000080
#define		MPC85xx_DEVDISR_TSEC2	0x00000040
#define		MPC85xx_DEVDISR_TSEC3	0x00000020

/*
 * L2 Cache Register Offsets
 */
#define MPC85xx_L2_CTL_OFFSET		0x0	/* L2 configuration 0 */
#define		MPC85xx_L2CTL_L2E		0x80000000
#define		MPC85xx_L2CTL_L2SRAM_ENTIRE	0x00010000
#define MPC85xx_L2_L2SRBAR0_OFFSET	0x100
#define MPC85xx_L2_L2ERRDIS_OFFSET	0xe44
#define		MPC85xx_L2ERRDIS_MBECC		0x00000008
#define		MPC85xx_L2ERRDIS_SBECC		0x00000004

/* PIC registers offsets */
#define MPC85xx_PIC_WHOAMI_OFFSET	0x090
#define MPC85xx_PIC_FRR_OFFSET		0x1000	/* Feature Reporting */
/* PIC registers fields values and masks. */
#define MPC8xxx_PICFRR_NCPU_MASK	0x00001f00
#define MPC8xxx_PICFRR_NCPU_SHIFT	8
#define MPC85xx_PICGCR_RST		0x80000000
#define MPC85xx_PICGCR_M		0x20000000

#define MPC85xx_PIC_IACK0_OFFSET	0x600a0	/* IRQ Acknowledge for
						   Processor 0 */

/* Global Utilities Register Offsets and field values */
#define MPC85xx_GUTS_PORPLLSR_OFFSET	0x0
#define		MPC85xx_PORPLLSR_DDR_RATIO		0x00003e00
#define		MPC85xx_PORPLLSR_DDR_RATIO_SHIFT	9
#define MPC85xx_GUTS_PORDEVSR2_OFFSET	0x14
#define		MPC85xx_PORDEVSR2_SEC_CFG		0x00000080
#define MPC85xx_GUTS_PMUXCR_OFFSET	0x60
#define		MPC85xx_PMUXCR_LCLK_IFC_CS3		0x000000C0
#define MPC85xx_GUTS_PMUXCR2_OFFSET	0x64
#define MPC85xx_GUTS_DEVDISR_OFFSET	0x70
#define		MPC85xx_DEVDISR_TB0	0x00004000
#define		MPC85xx_DEVDISR_TB1	0x00001000
#define MPC85xx_GUTS_RSTCR_OFFSET	0xb0

#define GFAR_BASE_ADDR		(CFG_IMMR + TSEC1_OFFSET)
#define MDIO_BASE_ADDR		(CFG_IMMR + 0x24000)

#define I2C1_BASE_ADDR		(CFG_IMMR + 0x3000)
#define I2C2_BASE_ADDR		(CFG_IMMR + 0x3100)

/* Global Timer Registers */
#define MPC8xxx_PIC_TFRR_OFFSET		0x10F0

#define PCI1_BASE_ADDR		(CFG_IMMR + MPC85xx_PCI1_OFFSET)
#endif /*__IMMAP_85xx__*/
