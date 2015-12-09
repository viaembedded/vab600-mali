/*
 * Copyright (C) 2011 WonderMedia Technologies, Inc. All rights reserved.
 *
 * Copyright (C) 2010 ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __WM3481_H__
#define __WM3481_H__

#ifndef ARCH_WMT
#define ARCH_WMT
#endif /* ARCH_WMT */

#ifndef PAGE_SHIFT
#define PAGE_SHIFT	12
#endif /* PAGE_SHIFT */

/* Note: these value will be overrided later */
#define CFG_OS_MEMORY_BASE		(0x40000000)
#define CFG_OS_MEMORY_SIZE		(256 << 20)
#define CFG_MEMORY_BASE			(0)
#define CFG_MEMORY_SIZE			(0)
#define CFG_MEM_VALIDATION_BASE		(0)
#define CFG_MEM_VALIDATION_SIZE		(0)

/* Note: IRQ auto detection (setting irq to -1) only works if the IRQ is not shared with any other hardware resource */

/* Mali 200 */
/*
#define IRQ_M200	100
#define IRQ_M200_GP2	101
#define IRQ_M200_MMU	102
*/

/* mali 400 */
#define IRQ_MALI_PMU         89
#define IRQ_MALI_GPMMU       90
#define IRQ_MALI_GP          100
#define IRQ_MALI_PPMMU0      101
#define IRQ_MALI_PP0         102

#ifndef ONLY_ZBT
#define ONLY_ZBT	0
#endif

extern unsigned long num_physpages;
extern unsigned int mali_memory_base;
extern unsigned int mali_memory_size;
extern unsigned int mali_mem_validation_base;
extern unsigned int mali_mem_validation_size;
extern unsigned int mali_os_memory_size;
extern unsigned int mali_ump_secure_id;
extern unsigned int (*mali_get_ump_secure_id)(unsigned int addr, unsigned int size);
extern void         (*mali_put_ump_secure_id)(unsigned int ump_id);

#if 0
static _mali_osk_resource_t arch_configuration [] =
{
	{
		.type = PMU,
		.description = "Mali-400 PMU",
		.base = 0xd8082000,
		.irq = IRQ_MALI_PMU,
		.mmu_id = 0
	},
	{
		.type = MALI400GP,
		.description = "Mali-400 GP",
		.base = 0xd8080000,
		.irq = IRQ_MALI_GP,
		.mmu_id = 1
	},
	{
		.type = MALI400PP,
		.description = "Mali-400 PP",
		.base = 0xd8088000,
		.irq = IRQ_MALI_PP0,
		.mmu_id = 2
	},
	{
		.type = MMU,
		.description = "Mali-400 MMU for GP",
		.base = 0xd8083000,
		.irq = IRQ_MALI_GPMMU,
		.mmu_id = 1
	},
	{
		.type = MMU,
		.description = "Mali-400 MMU for PP",
		.base = 0xd8084000,
		.irq = IRQ_MALI_PPMMU0,
		.mmu_id = 2
	},
	{
		.type = OS_MEMORY,
		.description = "OS Memory",
		.cpu_usage_adjust = 0,
		.alloc_order = 10, /* Lowest preference for this memory */
		.base = CFG_OS_MEMORY_BASE,
		.size = CFG_OS_MEMORY_SIZE,
		.flags = _MALI_CPU_WRITEABLE | _MALI_CPU_READABLE |
			 _MALI_MMU_READABLE | _MALI_MMU_WRITEABLE
	},
	{
		.type = MEMORY,
		.description = "Mali SDRAM remapped to baseboard",
		.cpu_usage_adjust = 0,
		.alloc_order = 0, /* Highest preference for this memory */
		.base = CFG_MEMORY_BASE,
		.size = CFG_MEMORY_SIZE,
		.flags = _MALI_CPU_WRITEABLE | _MALI_CPU_READABLE |
			 _MALI_MMU_READABLE | _MALI_MMU_WRITEABLE
	},
	{
		.type = MEM_VALIDATION,
		.description = "Framebuffer",
		.base = CFG_MEM_VALIDATION_BASE,
		.size = CFG_MEM_VALIDATION_SIZE,
		.flags = _MALI_CPU_WRITEABLE | _MALI_CPU_READABLE |
			 _MALI_MMU_READABLE | _MALI_MMU_WRITEABLE
	},
	{
		.type = MALI400L2,
		.description = "Mali-400 L2 cache",
		.base = 0xd8081000
	},
};

static void load_default_settings( void )
{
	int n;
	int i;

	n = sizeof(arch_configuration) / sizeof(arch_configuration[0]);
	for (i = 0; i < n; i++) {
		if (arch_configuration[i].type == OS_MEMORY) {
			arch_configuration[i].size =
				num_physpages << (PAGE_SHIFT-1);
			if (mali_os_memory_size)
				arch_configuration[i].size =
					mali_os_memory_size;
		}
		if (arch_configuration[i].type == MEMORY) {
			arch_configuration[i].base = mali_memory_base;
			arch_configuration[i].size = mali_memory_size;
		} else if (arch_configuration[i].type == MEM_VALIDATION) {
			arch_configuration[i].base = mali_mem_validation_base;
			arch_configuration[i].size = mali_mem_validation_size;
		}
	}
}
#endif
#endif /* __WM3481_H__ */
