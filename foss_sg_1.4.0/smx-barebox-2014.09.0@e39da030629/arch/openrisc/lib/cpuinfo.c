/*
 * (C) Copyright 2011, Stefan Kristiansson <stefan.kristiansson@saunalahti.fi>
 * (C) Copyright 2011, Julius Baxter <julius@opencores.org>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <common.h>
#include <command.h>
#include <asm/system.h>
#include <asm/cache.h>
#include <asm/openrisc_exc.h>

/* CPUID */
#define OR1KSIM	0x00
#define OR1200	0x12
#define MOR1KX	0x01
#define ALTOR32	0x32
#define OR10	0x10

static volatile int illegal_instruction;

static void illegal_instruction_handler(void)
{
	ulong *epcr = (ulong *)mfspr(SPR_EPCR_BASE);

	/* skip over the illegal instruction */
	mtspr(SPR_EPCR_BASE, (ulong)(++epcr));
	illegal_instruction = 1;
}

static int checkinstructions(void)
{
	ulong ra = 1, rb = 1, rc;

	exception_install_handler(EXC_ILLEGAL_INSTR,
				illegal_instruction_handler);

	illegal_instruction = 0;
	asm volatile("l.mul %0,%1,%2" : "=r" (rc) : "r" (ra), "r" (rb));
	printf("           Hardware multiplier: %s\n",
		illegal_instruction ? "no" : "yes");

	illegal_instruction = 0;
	asm volatile("l.div %0,%1,%2" : "=r" (rc) : "r" (ra), "r" (rb));
	printf("           Hardware divider: %s\n",
		illegal_instruction ? "no" : "yes");

	exception_free_handler(EXC_ILLEGAL_INSTR);

	return 0;
}

static void cpu_implementation(ulong vr2, char *string)
{
	switch (vr2 >> 24) {

	case OR1KSIM:
		sprintf(string, "or1ksim");
		break;
	case OR1200:
		sprintf(string, "OR1200");
		break;
	case MOR1KX:
		sprintf(string, "mor1kx v%u.%u - ", (uint)((vr2 >> 16) & 0xff),
			(uint)((vr2 >> 8) & 0xff));

		if ((uint)(vr2 & 0xff) == 1)
			strcat(string, "cappuccino");
		else if ((uint)(vr2 & 0xff) == 2)
			strcat(string, "espresso");
		else if ((uint)(vr2 & 0xff) == 3)
			strcat(string, "prontoespresso");
		else
			strcat(string, "unknwown");

		break;
	case ALTOR32:
		sprintf(string, "AltOr32");
		break;
	case OR10:
		sprintf(string, "OR10");
		break;
	default:
		sprintf(string, "unknown");
	}
}

int checkcpu(void)
{
	ulong upr = mfspr(SPR_UPR);
	ulong vr = mfspr(SPR_VR);
	ulong vr2 = mfspr(SPR_VR2);
	ulong iccfgr = mfspr(SPR_ICCFGR);
	ulong dccfgr = mfspr(SPR_DCCFGR);
	ulong immucfgr = mfspr(SPR_IMMUCFGR);
	ulong dmmucfgr = mfspr(SPR_DMMUCFGR);
	ulong cpucfgr = mfspr(SPR_CPUCFGR);
	uint ver = (vr & SPR_VR_VER) >> 24;
	uint rev = vr & SPR_VR_REV;
	uint block_size;
	uint ways;
	uint sets;

	char impl_str[50];

	printf("CPU:   OpenRISC-%x00 (rev %d) @ %d MHz\n",
		ver, rev, (CONFIG_SYS_CLK_FREQ / 1000000));

	if (vr2) {
		cpu_implementation(vr2, impl_str);
		printf("       Implementation: %s\n", impl_str);
	}

	if (upr & SPR_UPR_DCP) {
		block_size = (dccfgr & SPR_DCCFGR_CBS) ? 32 : 16;
		ways = 1 << (dccfgr & SPR_DCCFGR_NCW);
		printf("       D-Cache: %d bytes, %d bytes/line, %d way(s)\n",
		       checkdcache(), block_size, ways);
	} else {
		printf("       D-Cache: no\n");
	}

	if (upr & SPR_UPR_ICP) {
		block_size = (iccfgr & SPR_ICCFGR_CBS) ? 32 : 16;
		ways = 1 << (iccfgr & SPR_ICCFGR_NCW);
		printf("       I-Cache: %d bytes, %d bytes/line, %d way(s)\n",
		       checkicache(), block_size, ways);
	} else {
		printf("       I-Cache: no\n");
	}

	if (upr & SPR_UPR_DMP) {
		sets = 1 << ((dmmucfgr & SPR_DMMUCFGR_NTS) >> 2);
		ways = (dmmucfgr & SPR_DMMUCFGR_NTW) + 1;
		printf("       DMMU: %d sets, %d way(s)\n",
		       sets, ways);
	} else {
		printf("       DMMU: no\n");
	}

	if (upr & SPR_UPR_IMP) {
		sets = 1 << ((immucfgr & SPR_IMMUCFGR_NTS) >> 2);
		ways = (immucfgr & SPR_IMMUCFGR_NTW) + 1;
		printf("       IMMU: %d sets, %d way(s)\n",
		       sets, ways);
	} else {
		printf("       IMMU: no\n");
	}

	printf("       MAC unit: %s\n",
		(upr & SPR_UPR_MP) ? "yes" : "no");
	printf("       Debug unit: %s\n",
		(upr & SPR_UPR_DUP) ? "yes" : "no");
	printf("       Performance counters: %s\n",
		(upr & SPR_UPR_PCUP) ? "yes" : "no");
	printf("       Power management: %s\n",
		(upr & SPR_UPR_PMP) ? "yes" : "no");
	printf("       Interrupt controller: %s\n",
		(upr & SPR_UPR_PICP) ? "yes" : "no");
	printf("       Timer: %s\n",
		(upr & SPR_UPR_TTP) ? "yes" : "no");
	printf("       Custom unit(s): %s\n",
		(upr & SPR_UPR_CUP) ? "yes" : "no");

	printf("       Supported instructions:\n");
	printf("           ORBIS32: %s\n",
		(cpucfgr & SPR_CPUCFGR_OB32S) ? "yes" : "no");
	printf("           ORBIS64: %s\n",
		(cpucfgr & SPR_CPUCFGR_OB64S) ? "yes" : "no");
	printf("           ORFPX32: %s\n",
		(cpucfgr & SPR_CPUCFGR_OF32S) ? "yes" : "no");
	printf("           ORFPX64: %s\n",
		(cpucfgr & SPR_CPUCFGR_OF64S) ? "yes" : "no");

	checkinstructions();

	return 0;
}

static int do_cpuinfo(int argc, char *argv[])
{
	checkcpu();
	return 0;
}

BAREBOX_CMD_START(cpuinfo)
	.cmd            = do_cpuinfo,
	BAREBOX_CMD_DESC("show CPU information")
	BAREBOX_CMD_GROUP(CMD_GRP_INFO)
BAREBOX_CMD_END
