#ifndef __MACH_GENERIC_H
#define __MACH_GENERIC_H

#include <linux/compiler.h>
#include <linux/types.h>

u64 imx_uid(void);

void imx25_boot_save_loc(void __iomem *ccm_base);
void imx35_boot_save_loc(void __iomem *ccm_base);
void imx27_boot_save_loc(void __iomem *sysctrl_base);
void imx51_boot_save_loc(void __iomem *src_base);
void imx53_boot_save_loc(void __iomem *src_base);
void imx6_boot_save_loc(void __iomem *src_base);

int imx1_init(void);
int imx21_init(void);
int imx25_init(void);
int imx27_init(void);
int imx31_init(void);
int imx35_init(void);
int imx51_init(void);
int imx53_init(void);
int imx6_init(void);

int imx1_devices_init(void);
int imx21_devices_init(void);
int imx25_devices_init(void);
int imx27_devices_init(void);
int imx31_devices_init(void);
int imx35_devices_init(void);
int imx51_devices_init(void);
int imx53_devices_init(void);
int imx6_devices_init(void);

void imx6_cpu_lowlevel_init(void);

enum bootsource;
struct device_node;

int imx6_bootmode_set(enum bootsource src, int instance,
		      struct device_node *node, void *data);

/* There's a off-by-one betweem the gpio bank number and the gpiochip */
/* range e.g. GPIO_1_5 is gpio 5 under linux */
#define IMX_GPIO_NR(bank, nr)		(((bank) - 1) * 32 + (nr))

#define IMX_CPU_IMX1	1
#define IMX_CPU_IMX21	21
#define IMX_CPU_IMX25	25
#define IMX_CPU_IMX27	27
#define IMX_CPU_IMX31	31
#define IMX_CPU_IMX35	35
#define IMX_CPU_IMX51	51
#define IMX_CPU_IMX53	53
#define IMX_CPU_IMX6	6

extern unsigned int __imx_cpu_type;

#ifdef CONFIG_ARCH_IMX1
# ifdef imx_cpu_type
#  undef imx_cpu_type
#  define imx_cpu_type __imx_cpu_type
# else
#  define imx_cpu_type IMX_CPU_IMX1
# endif
# define cpu_is_mx1()		(imx_cpu_type == IMX_CPU_IMX1)
#else
# define cpu_is_mx1()		(0)
#endif

#ifdef CONFIG_ARCH_IMX21
# ifdef imx_cpu_type
#  undef imx_cpu_type
#  define imx_cpu_type __imx_cpu_type
# else
#  define imx_cpu_type IMX_CPU_IMX21
# endif
# define cpu_is_mx21()		(imx_cpu_type == IMX_CPU_IMX21)
#else
# define cpu_is_mx21()		(0)
#endif

#ifdef CONFIG_ARCH_IMX25
# ifdef imx_cpu_type
#  undef imx_cpu_type
#  define imx_cpu_type __imx_cpu_type
# else
#  define imx_cpu_type IMX_CPU_IMX25
# endif
# define cpu_is_mx25()		(imx_cpu_type == IMX_CPU_IMX25)
#else
# define cpu_is_mx25()		(0)
#endif

#ifdef CONFIG_ARCH_IMX27
# ifdef imx_cpu_type
#  undef imx_cpu_type
#  define imx_cpu_type __imx_cpu_type
# else
#  define imx_cpu_type IMX_CPU_IMX27
# endif
# define cpu_is_mx27()		(imx_cpu_type == IMX_CPU_IMX27)
#else
# define cpu_is_mx27()		(0)
#endif

#ifdef CONFIG_ARCH_IMX31
# ifdef imx_cpu_type
#  undef imx_cpu_type
#  define imx_cpu_type __imx_cpu_type
# else
#  define imx_cpu_type IMX_CPU_IMX31
# endif
# define cpu_is_mx31()		(imx_cpu_type == IMX_CPU_IMX31)
#else
# define cpu_is_mx31()		(0)
#endif

#ifdef CONFIG_ARCH_IMX35
# ifdef imx_cpu_type
#  undef imx_cpu_type
#  define imx_cpu_type __imx_cpu_type
# else
#  define imx_cpu_type IMX_CPU_IMX35
# endif
# define cpu_is_mx35()		(imx_cpu_type == IMX_CPU_IMX35)
#else
# define cpu_is_mx35()		(0)
#endif

#ifdef CONFIG_ARCH_IMX51
# ifdef imx_cpu_type
#  undef imx_cpu_type
#  define imx_cpu_type __imx_cpu_type
# else
#  define imx_cpu_type IMX_CPU_IMX51
# endif
# define cpu_is_mx51()		(imx_cpu_type == IMX_CPU_IMX51)
#else
# define cpu_is_mx51()		(0)
#endif

#ifdef CONFIG_ARCH_IMX53
# ifdef imx_cpu_type
#  undef imx_cpu_type
#  define imx_cpu_type __imx_cpu_type
# else
#  define imx_cpu_type IMX_CPU_IMX53
# endif
# define cpu_is_mx53()		(imx_cpu_type == IMX_CPU_IMX53)
#else
# define cpu_is_mx53()		(0)
#endif

#ifdef CONFIG_ARCH_IMX6
# ifdef imx_cpu_type
#  undef imx_cpu_type
#  define imx_cpu_type __imx_cpu_type
# else
#  define imx_cpu_type IMX_CPU_IMX6
# endif
# define cpu_is_mx6()		(imx_cpu_type == IMX_CPU_IMX6)
#else
# define cpu_is_mx6()		(0)
#endif

#define cpu_is_mx23()	(0)
#define cpu_is_mx28()	(0)

#endif /* __MACH_GENERIC_H */
