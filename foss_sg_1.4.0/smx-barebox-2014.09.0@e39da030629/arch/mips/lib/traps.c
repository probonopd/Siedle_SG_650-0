#include <common.h>

#include <asm/mipsregs.h>
#include <asm/ptrace.h>

void barebox_exc_handler(const struct pt_regs *regs);

/*
 * Trap codes from OpenBSD trap.h
 */
#define T_INT			0	/* Interrupt pending */
#define T_TLB_MOD		1	/* TLB modified fault */
#define T_TLB_LD_MISS		2	/* TLB miss on load or ifetch */
#define T_TLB_ST_MISS		3	/* TLB miss on a store */
#define T_ADDR_ERR_LD		4	/* Address error on a load or ifetch */
#define T_ADDR_ERR_ST		5	/* Address error on a store */
#define T_BUS_ERR_IFETCH	6	/* Bus error on an ifetch */
#define T_BUS_ERR_LD_ST		7	/* Bus error on a load or store */
#define T_SYSCALL		8	/* System call */
#define T_BREAK			9	/* Breakpoint */
#define T_RES_INST		10	/* Reserved instruction exception */
#define T_COP_UNUSABLE		11	/* Coprocessor unusable */
#define T_OVFLOW		12	/* Arithmetic overflow */
#define T_TRAP			13	/* Trap instruction */
#define T_VCEI			14	/* Virtual coherency instruction */
#define T_FPE			15	/* Floating point exception */
#define T_IWATCH		16	/* Inst. Watch address reference */
#define T_DWATCH		23	/* Data Watch address reference */
#define T_VCED			31	/* Virtual coherency data */

#define CR_EXC_CODE             0x0000007c
#define CR_EXC_CODE_SHIFT       2

static char *get_exc_name(u32 cause)
{
	switch ((cause & CR_EXC_CODE) >> CR_EXC_CODE_SHIFT) {

	case T_INT:
		return "interrupt pending";

	case T_TLB_MOD:
		return "TLB modified";

	case T_TLB_LD_MISS:
		return "TLB miss on load or ifetch";

	case T_TLB_ST_MISS:
		return "TLB miss on store";

	case T_ADDR_ERR_LD:
		return "address error on load or ifetch";

	case T_ADDR_ERR_ST:
		return "address error on store";

	case T_BUS_ERR_IFETCH:
		return "bus error on ifetch";

	case T_BUS_ERR_LD_ST:
		return "bus error on load or store";

	case T_SYSCALL:
		return "system call";

	case T_BREAK:
		return "breakpoint";

	case T_RES_INST:
		return "reserved instruction";

	case T_COP_UNUSABLE:
		return "coprocessor unusable";

	case T_OVFLOW:
		return "arithmetic overflow";

	case T_TRAP:
		return "trap instruction";

	case T_VCEI:
		return "virtual coherency instruction";

	case T_FPE:
		return "floating point";

	case T_IWATCH:
		return "iwatch";

	case T_DWATCH:
		return "dwatch";

	case T_VCED:
		return "virtual coherency data";
	}

	return "unknown exception";
}

void barebox_exc_handler(const struct pt_regs *regs)
{
	const int field = 2 * sizeof(unsigned long);
	unsigned int cause = regs->cp0_cause;
	int i;

	printf("\nOoops, %s!\n\n", get_exc_name(cause));

	/*
	 * Saved main processor registers
	 */
	for (i = 0; i < 32; ) {
		if ((i % 4) == 0)
			printf("$%2d   :", i);
		if (i == 0)
			printf(" %0*lx", field, 0UL);
		else if (i == 26 || i == 27)
			printf(" %*s", field, "");
		else
			printf(" %0*lx", field, regs->regs[i]);

		i++;
		if ((i % 4) == 0)
			printf("\n");
	}

	printf("Hi    : %0*lx\n", field, regs->hi);
	printf("Lo    : %0*lx\n", field, regs->lo);

	/*
	 * Saved cp0 registers
	 */
	printf("epc   : %0*lx\n", field, regs->cp0_epc);
	printf("ra    : %0*lx\n", field, regs->regs[31]);

	printf("Status: %08x\n", (uint32_t) regs->cp0_status);
	printf("Cause : %08x\n", cause);
	printf("Config: %08x\n\n", read_c0_config());

	hang();
}
