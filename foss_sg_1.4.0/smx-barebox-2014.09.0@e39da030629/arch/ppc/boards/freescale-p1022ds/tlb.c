/*
 * Copyright 2011 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 */

#include <common.h>
#include <mach/mmu.h>

struct fsl_e_tlb_entry tlb_table[] = {
	/* TLB 0 - for temp stack in cache */
	FSL_SET_TLB_ENTRY(0, CFG_INIT_RAM_ADDR, CFG_INIT_RAM_ADDR,
			  MAS3_SX | MAS3_SW | MAS3_SR, 0,
			  0, 0, BOOKE_PAGESZ_4K, 0),
	FSL_SET_TLB_ENTRY(0, CFG_INIT_RAM_ADDR + (4 * 1024),
			  CFG_INIT_RAM_ADDR + (4 * 1024),
			  MAS3_SX | MAS3_SW | MAS3_SR, 0,
			  0, 0, BOOKE_PAGESZ_4K, 0),
	FSL_SET_TLB_ENTRY(0, CFG_INIT_RAM_ADDR + (8 * 1024),
			  CFG_INIT_RAM_ADDR + (8 * 1024),
			  MAS3_SX | MAS3_SW | MAS3_SR, 0,
			  0, 0, BOOKE_PAGESZ_4K, 0),
	FSL_SET_TLB_ENTRY(0, CFG_INIT_RAM_ADDR + (12 * 1024),
			  CFG_INIT_RAM_ADDR + (12 * 1024),
			  MAS3_SX | MAS3_SW | MAS3_SR, 0,
			  0, 0, BOOKE_PAGESZ_4K, 0),

	/* TLB 1 */
	/* *I*** - Covers boot page */
	FSL_SET_TLB_ENTRY(1, 0xfffff000, 0xfffff000,
			  MAS3_SX | MAS3_SW | MAS3_SR, MAS2_I | MAS2_G,
			  0, 0, BOOKE_PAGESZ_4K, 1),

	/* *I*G* - CCSRBAR */
	FSL_SET_TLB_ENTRY(1, CFG_CCSRBAR, CFG_CCSRBAR_PHYS,
			  MAS3_SX | MAS3_SW | MAS3_SR, MAS2_I | MAS2_G,
			  0, 1, BOOKE_PAGESZ_1M, 1),

	/* W**G* - Flash/promjet, localbus */
	/* This will be changed to *I*G* after relocation to RAM. */
	FSL_SET_TLB_ENTRY(1, CFG_BOOT_BLOCK, CFG_BOOT_BLOCK_PHYS,
			  MAS3_SX | MAS3_SR, MAS2_M | MAS2_W | MAS2_G,
			  0, 2, BOOKE_PAGESZ_256M, 1),

	FSL_SET_TLB_ENTRY(1, CFG_PIXIS_BASE, CFG_PIXIS_BASE_PHYS,
			  MAS3_SX | MAS3_SW | MAS3_SR, MAS2_I | MAS2_G,
			  0, 7, BOOKE_PAGESZ_4K, 1),
};

int num_tlb_entries = ARRAY_SIZE(tlb_table);
