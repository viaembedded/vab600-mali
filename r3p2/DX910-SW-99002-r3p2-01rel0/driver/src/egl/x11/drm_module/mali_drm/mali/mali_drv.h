/*
 * Copyright (C) 2010, 2012 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef _MALI_DRV_H_
#define _MALI_DRV_H_

#define DRIVER_AUTHOR		"ARM"
#define DRIVER_NAME		"mali_drm"
#define DRIVER_DESC		"DRM module for Mali-200, Mali-400"
#define DRIVER_DATE		"20100520"
#define DRIVER_MAJOR		0
#define DRIVER_MINOR		1
#define DRIVER_PATCHLEVEL	0

#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,4,5)
#include "drm/drm_sman.h"
#else
#include "drm/drm_mm.h"
#endif

typedef struct drm_mali_private 
{
	drm_local_map_t *mmio;
	unsigned int idle_fault;
#ifdef DRM_SMAN_H
	struct drm_sman sman;
#else
	struct drm_mm vram_mm;
	struct drm_mm mem_mm;
	/** Mapping of userspace keys to mm objects */
	struct idr object_idr;
#endif
	int vram_initialized;
	unsigned long vram_offset;
#ifndef DRM_SMAN_H
	int mem_initialized;
	unsigned long mem_offset;
#endif
} drm_mali_private_t;

extern int mali_idle(struct drm_device *dev);
extern void mali_reclaim_buffers_locked(struct drm_device *dev, struct drm_file *file_priv);
extern void mali_lastclose(struct drm_device *dev);
extern struct drm_ioctl_desc mali_ioctls[];
extern int mali_max_ioctl;

#endif /* _MALI_DRV_H_ */
