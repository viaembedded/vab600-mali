/*
 * Copyright (C) 2010-2011 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * @file mali_platform.c
 * Platform specific Mali driver functions for a default platform
 */
#include "mali_kernel_common.h"
#include "mali_osk.h"
/*
#include "mali_platform.h"
*/
#include "mali_pmu.h"
#include "linux/mali/mali_utgard.h"
#include "wm3481_pmm.h"

extern int mali_platform_init_impl(void *data);
extern int mali_platform_deinit_impl(void *data);
extern int mali_platform_powerdown_impl(u32 cores);
extern int mali_platform_powerup_impl(u32 cores);
extern int mali_gpu_utilization_handler_impl(u32 utilization);

static u32 pwr_stat_on = 0;

_mali_osk_errcode_t mali_platform_init(void)
{
    MALI_DEBUG_PRINT(4, ("mali_platform_init\n"));
    mali_platform_init_impl(NULL);
    mali_platform_powerup_impl(1);
    MALI_SUCCESS;
}

_mali_osk_errcode_t mali_platform_deinit(void)
{
    MALI_DEBUG_PRINT(4, ("mali_platform_deinit\n"));
    mali_platform_powerdown_impl(1);
    mali_platform_deinit_impl(NULL);
    MALI_SUCCESS;
}

static _mali_osk_errcode_t mali_platform_powerdown(u32 cores)
{
    MALI_DEBUG_PRINT(4, ("mali_platform_powerdown\n"));
    mali_platform_powerdown_impl(cores);
    MALI_SUCCESS;
}

static _mali_osk_errcode_t mali_platform_powerup(u32 cores)
{
    MALI_DEBUG_PRINT(4, ("mali_platform_powerup\n"));
    mali_platform_powerup_impl(cores);
    MALI_SUCCESS;
}

_mali_osk_errcode_t mali_platform_power_mode_change(mali_power_mode power_mode)
{
    MALI_DEBUG_PRINT(4, ("mali_platform_power_mode_change\n"));
    switch (power_mode) {
    case MALI_POWER_MODE_ON:
        if (pwr_stat_on == 0) {
            mali_platform_powerup(1);
            mali_pmu_powerup();
            pwr_stat_on = 1;
        }
        break;
    case MALI_POWER_MODE_LIGHT_SLEEP:
    case MALI_POWER_MODE_DEEP_SLEEP:
        if (pwr_stat_on == 1) {
            mali_pmu_powerdown();
            mali_platform_powerdown(1);
            pwr_stat_on = 0;
        }
        break;
    default:
        break;
    }
    MALI_SUCCESS;
}

void mali_gpu_utilization_handler(u32 utilization)
{
    MALI_DEBUG_PRINT(4, ("mali_gpu_utilization_handler\n"));
    mali_gpu_utilization_handler_impl(utilization);
}

/*
#if MALI_POWER_MGMT_TEST_SUITE
u32 pmu_get_power_up_down_info(void)
{
	return 4095;

}
#endif
*/
