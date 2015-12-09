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

#ifndef __WM3498_H__
#define __WM3498_H__

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

/* mali 400 */

/* SPI: Shared Peripheral Interrupt */
#define IRQ_SPI(x)	(x + 32)
#define IRQ_MALI_PMU	IRQ_SPI(82)
#define IRQ_MALI_GPMMU	IRQ_SPI(83)
#define IRQ_MALI_PPMMU1	IRQ_SPI(91)
#define IRQ_MALI_PP1	IRQ_SPI(92)
#define IRQ_MALI_GP	IRQ_SPI(93)
#define IRQ_MALI_PPMMU0	IRQ_SPI(94)
#define IRQ_MALI_PP0	IRQ_SPI(95)

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

#endif /* __WM3498_H__ */
