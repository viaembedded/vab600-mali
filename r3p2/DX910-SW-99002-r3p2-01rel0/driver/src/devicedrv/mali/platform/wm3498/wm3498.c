/*
 * This confidential and proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2009-2010, 2012 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

/**
 * Copyright (C) 2012 WonderMedia Technologies, Inc. All rights reserved.
 *
 * @file wm3498.c
 * Platform specific Mali driver functions for WonderMedia WM8950 based platforms
 */

#include <linux/platform_device.h>
#include <linux/version.h>
#include <linux/pm.h>
#ifdef CONFIG_PM_RUNTIME
#include <linux/pm_runtime.h>
#endif
#include <asm/io.h>
#include <linux/mali/mali_utgard.h>
#include "mali_kernel_common.h"

#include "wm3498.h"
#include "wm3498_pmm.h"

#define HAVE_X11
#define NUM_GPUS 2

static void mali_platform_device_release(struct device *device);
/*
static u32 mali_read_phys(u32 phys_addr);
static void mali_write_phys(u32 phys_addr, u32 value);
*/
static int mali_os_suspend(struct device *device);
static int mali_os_resume(struct device *device);
static int mali_os_freeze(struct device *device);
static int mali_os_thaw(struct device *device);
#ifdef CONFIG_PM_RUNTIME
static int mali_runtime_suspend(struct device *device);
static int mali_runtime_resume(struct device *device);
static int mali_runtime_idle(struct device *device);
#endif

#ifdef HAVE_X11
/*
extern unsigned int mali_ump_secure_id;
*/
extern unsigned int (*mali_get_ump_secure_id)(unsigned int addr, unsigned int size);
extern void         (*mali_put_ump_secure_id)(unsigned int ump_id);

static unsigned int _mali_osk_get_ump_secure_id(unsigned int addr, unsigned int size);
static void         _mali_osk_put_ump_secure_id(unsigned int ump_id);
#endif

static struct resource mali_gpu_resources[] =
{
#if (NUM_GPUS == 2)
	MALI_GPU_RESOURCES_MALI400_MP2_PMU(0xd8080000,
					   IRQ_MALI_GP, IRQ_MALI_GPMMU,
					   IRQ_MALI_PP0, IRQ_MALI_PPMMU0,
					   IRQ_MALI_PP1, IRQ_MALI_PPMMU1)
#else
	MALI_GPU_RESOURCES_MALI400_MP1_PMU(0xd8080000,
					   IRQ_MALI_GP, IRQ_MALI_GPMMU,
					   IRQ_MALI_PP0, IRQ_MALI_PPMMU0)
#endif
};

static struct dev_pm_ops mali_gpu_device_type_pm_ops =
{
	.suspend = mali_os_suspend,
	.resume = mali_os_resume,
	.freeze = mali_os_freeze,
	.thaw = mali_os_thaw,
#ifdef CONFIG_PM_RUNTIME
	.runtime_suspend = mali_runtime_suspend,
	.runtime_resume = mali_runtime_resume,
	.runtime_idle = mali_runtime_idle,
#endif
};

static struct device_type mali_gpu_device_device_type =
{
	.pm = &mali_gpu_device_type_pm_ops,
};

static struct platform_device mali_gpu_device =
{
	.name = MALI_GPU_NAME_UTGARD,
	.id = 0,
	.dev.release = mali_platform_device_release,

	.dev.type = &mali_gpu_device_device_type,
};

static struct mali_gpu_device_data mali_gpu_data =
{
	.dedicated_mem_start = CFG_MEMORY_BASE,
	.dedicated_mem_size  = CFG_MEMORY_SIZE,
	.fb_start = CFG_MEM_VALIDATION_BASE,
	.fb_size  = CFG_MEM_VALIDATION_SIZE,
	.shared_mem_size = CFG_OS_MEMORY_SIZE,
};

int mali_platform_device_register(void)
{
	int err = -1;

	MALI_DEBUG_PRINT(4, ("mali_platform_device_register() called\n"));

	/* Detect present Mali GPU and connect the correct resources to the device */
	MALI_DEBUG_PRINT(4, ("Registering Mali-400 device\n"));
	err = platform_device_add_resources(&mali_gpu_device, mali_gpu_resources,
		sizeof(mali_gpu_resources) / sizeof(mali_gpu_resources[0]));

	/* dynamic detection */
	mali_gpu_data.dedicated_mem_start = mali_memory_base;
	mali_gpu_data.dedicated_mem_size = mali_memory_size;
	mali_gpu_data.fb_start = mali_mem_validation_base;
	mali_gpu_data.fb_size = mali_mem_validation_size;

#ifdef HAVE_X11
	mali_get_ump_secure_id = &_mali_osk_get_ump_secure_id;
	mali_put_ump_secure_id = &_mali_osk_put_ump_secure_id;
	/*
	mali_ump_secure_id = _mali_osk_get_ump_secure_id(mali_fb_start, mali_fb_size);
	*/
#endif /* HAVE_X11 */

	if (0 == err)
	{
		err = platform_device_add_data(&mali_gpu_device, &mali_gpu_data, sizeof(mali_gpu_data));
		if (0 == err)
		{
			mali_platform_init();
			/* Register the platform device */
			err = platform_device_register(&mali_gpu_device);
			if (0 == err)
			{
				//mali_platform_init();
#ifdef CONFIG_PM_RUNTIME
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
				pm_runtime_set_autosuspend_delay(&(mali_gpu_device.dev), 1000);
				pm_runtime_use_autosuspend(&(mali_gpu_device.dev));
#endif
				pm_runtime_enable(&(mali_gpu_device.dev));
#endif

				return 0;
			}
		}

		platform_device_unregister(&mali_gpu_device);
	}

	return err;
}

void mali_platform_device_unregister(void)
{
	MALI_DEBUG_PRINT(4, ("mali_platform_device_unregister() called\n"));

#ifdef HAVE_X11
	/*
	_mali_osk_put_ump_secure_id(mali_ump_secure_id);
	mali_ump_secure_id = UMP_INVALID_SECURE_ID;
	*/
	mali_get_ump_secure_id = NULL;
	mali_put_ump_secure_id = NULL;
#endif /* HAVE_X11 */

	mali_platform_deinit();

	platform_device_unregister(&mali_gpu_device);
}

static void mali_platform_device_release(struct device *device)
{
	MALI_DEBUG_PRINT(4, ("mali_platform_device_release() called\n"));
}

static int mali_os_suspend(struct device *device)
{
	int ret = 0;

	MALI_DEBUG_PRINT(4, ("mali_os_suspend() called\n"));

	if (NULL != device->driver &&
	    NULL != device->driver->pm &&
	    NULL != device->driver->pm->suspend)
	{
		/* Need to notify Mali driver about this event */
		ret = device->driver->pm->suspend(device);
	}

	mali_platform_power_mode_change(MALI_POWER_MODE_DEEP_SLEEP);

	return ret;
}

static int mali_os_resume(struct device *device)
{
	int ret = 0;

	MALI_DEBUG_PRINT(4, ("mali_os_resume() called\n"));

	mali_platform_power_mode_change(MALI_POWER_MODE_ON);

	if (NULL != device->driver &&
	    NULL != device->driver->pm &&
	    NULL != device->driver->pm->resume)
	{
		/* Need to notify Mali driver about this event */
		ret = device->driver->pm->resume(device);
	}

	return ret;
}

static int mali_os_freeze(struct device *device)
{
	int ret = 0;

	MALI_DEBUG_PRINT(4, ("mali_os_freeze() called\n"));

	if (NULL != device->driver &&
	    NULL != device->driver->pm &&
	    NULL != device->driver->pm->freeze)
	{
		/* Need to notify Mali driver about this event */
		ret = device->driver->pm->freeze(device);
	}

	return ret;
}

static int mali_os_thaw(struct device *device)
{
	int ret = 0;

	MALI_DEBUG_PRINT(4, ("mali_os_thaw() called\n"));

	if (NULL != device->driver &&
	    NULL != device->driver->pm &&
	    NULL != device->driver->pm->thaw)
	{
		/* Need to notify Mali driver about this event */
		ret = device->driver->pm->thaw(device);
	}

	return ret;
}

#ifdef CONFIG_PM_RUNTIME
static int mali_runtime_suspend(struct device *device)
{
	int ret = 0;

	MALI_DEBUG_PRINT(4, ("mali_runtime_suspend() called\n"));

	if (NULL != device->driver &&
	    NULL != device->driver->pm &&
	    NULL != device->driver->pm->runtime_suspend)
	{
		/* Need to notify Mali driver about this event */
		ret = device->driver->pm->runtime_suspend(device);
	}

	mali_platform_power_mode_change(MALI_POWER_MODE_LIGHT_SLEEP);

	return ret;
}

static int mali_runtime_resume(struct device *device)
{
	int ret = 0;

	MALI_DEBUG_PRINT(4, ("mali_runtime_resume() called\n"));

	mali_platform_power_mode_change(MALI_POWER_MODE_ON);

	if (NULL != device->driver &&
	    NULL != device->driver->pm &&
	    NULL != device->driver->pm->runtime_resume)
	{
		/* Need to notify Mali driver about this event */
		ret = device->driver->pm->runtime_resume(device);
	}

	return ret;
}

static int mali_runtime_idle(struct device *device)
{
	MALI_DEBUG_PRINT(4, ("mali_runtime_idle() called\n"));

	if (NULL != device->driver &&
	    NULL != device->driver->pm &&
	    NULL != device->driver->pm->runtime_idle)
	{
		/* Need to notify Mali driver about this event */
		int ret = device->driver->pm->runtime_idle(device);
		if (0 != ret)
		{
			return ret;
		}
	}

	pm_runtime_suspend(device);

	return 0;
}
#endif

/* For X11 EXA DRM */

#ifdef HAVE_X11
#include "mali_uk_types.h"
#include "mali_osk_list.h"
#include "ump_kernel_interface.h"

extern ump_dd_handle ump_dd_handle_create_from_phys_blocks(
		ump_dd_physical_block *blocks, unsigned long num_blocks);

struct ext_mem_blk {
	_mali_osk_list_t list;
	ump_dd_handle memh;
	ump_secure_id id;
};
static _mali_osk_list_t list_head;
static int sidgetcnt = 0;
static int sidputcnt = 0;

static unsigned int _mali_osk_get_ump_secure_id(unsigned int addr, unsigned int size)
{
	/* get ump secure id */
	ump_dd_handle ump_wrapped_buffer;
	ump_dd_physical_block ump_memory_description;
	ump_secure_id ump_id = UMP_INVALID_SECURE_ID;

	ump_memory_description.addr = addr;
	ump_memory_description.size = size;
	ump_wrapped_buffer = ump_dd_handle_create_from_phys_blocks(&ump_memory_description, 1);

	if (!sidgetcnt)
		_mali_osk_list_init(&list_head);

	if (ump_wrapped_buffer) {
		struct ext_mem_blk *blk = _mali_osk_malloc(sizeof(struct ext_mem_blk));
		if (blk) {
			ump_id = ump_dd_secure_id_get(ump_wrapped_buffer);
			blk->memh = ump_wrapped_buffer;
			blk->id = ump_id;
			_mali_osk_list_init(&blk->list);
			_mali_osk_list_add(&blk->list ,&list_head);
		}
	}
	sidgetcnt++;

	return ump_id;
}

static void _mali_osk_put_ump_secure_id(unsigned int ump_id)
{
	_mali_osk_list_t *list = &list_head;
	struct ext_mem_blk *blk;

	if (ump_id == UMP_INVALID_SECURE_ID)
		return;

	if (_mali_osk_list_empty(&list_head))
		return;

	list = list->next;

	while (list && list != &list_head) {
		blk = _MALI_OSK_LIST_ENTRY(list, struct ext_mem_blk, list);
		if (blk && blk->id == ump_id) {
			if (blk->memh)
				ump_dd_reference_release(blk->memh);
			_mali_osk_list_del(&blk->list);
			_mali_osk_free(blk);
			break;
		}
		list = list->next;
	}
	sidputcnt++;
}

#endif /* HAVE_X11 */
