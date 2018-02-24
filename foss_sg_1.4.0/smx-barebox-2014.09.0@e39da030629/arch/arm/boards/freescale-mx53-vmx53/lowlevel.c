#include <common.h>
#include <mach/esdctl.h>
#include <asm/barebox-arm-head.h>
#include <asm/barebox-arm.h>

extern char __dtb_imx53_voipac_bsb_start[];

ENTRY_FUNCTION(start_imx53_vmx53, r0, r1, r2)
{
	void *fdt;

	arm_cpu_lowlevel_init();

	fdt = __dtb_imx53_voipac_bsb_start - get_runtime_offset();

	imx53_barebox_entry(fdt);
}
