/*
 * Copyright (C) 2010, 2012 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/version.h>
#include "drm/drmP.h"
#include "mali_drm.h"
#include "mali_drv.h"

static struct platform_device *pdev;

#if 0
static int mali_platform_drm_probe(struct platform_device *pdev)
{
	return 0;
}

static int mali_platform_drm_remove(struct platform_device *pdev)
{
	return 0;
}

static int mali_platform_drm_suspend(struct platform_device *dev, pm_message_t state)
{
	return 0;
}

static int mali_platform_drm_resume(struct platform_device *dev)
{
	return 0;
}
#endif

static char mali_drm_device_name[] = "mali_drm";

#if 0
static struct platform_driver platform_drm_driver = {
	.probe = mali_platform_drm_probe,
	.remove = mali_platform_drm_remove,
	.suspend = mali_platform_drm_suspend,
	.resume = mali_platform_drm_resume,
	.driver = {
		.name = mali_drm_device_name,
		.owner = THIS_MODULE,
	},
};
#endif

#if 0
static const struct drm_device_id dock_device_ids[] = {
	{"MALIDRM", 0},
	{"", 0},
};
#endif

static int mali_driver_load(struct drm_device *dev, unsigned long chipset)
{
	int ret = 0;
	/* unsigned long base, size; */
	drm_mali_private_t *dev_priv;
	printk(KERN_ERR "DRM: mali_driver_load start\n");
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,32)
	dev_priv = drm_calloc(1, sizeof(drm_mali_private_t), DRM_MEM_DRIVER);
#else
	dev_priv = drm_calloc_large(1, sizeof(drm_mali_private_t));
#endif
	if ( dev_priv == NULL ) return -ENOMEM;

#ifndef DRM_SMAN_H
	idr_init(&dev_priv->object_idr);
#endif
	dev->dev_private = (void *)dev_priv;

	if ( NULL == dev->platformdev )
	{
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,32)
		dev->platformdev = platform_device_register_simple(mali_drm_device_name, 0, NULL, 0);
		pdev = dev->platformdev;
#else
		dev->platformdev = pdev;
#endif
	}

	#if 0
	base = drm_get_resource_start(dev, 1 );
	size = drm_get_resource_len(dev, 1 );
	#endif
#ifdef DRM_SMAN_H
	ret = drm_sman_init(&dev_priv->sman, 2, 12, 8);
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,32)
	if ( ret ) drm_free(dev_priv, sizeof(dev_priv), DRM_MEM_DRIVER);
#else
	if ( ret ) drm_free_large(dev_priv);
#endif
	//if ( ret ) kfree( dev_priv );
#endif /* DRM_SMAN */

	printk(KERN_ERR "DRM: mali_driver_load done\n");

	return ret;
}

static int mali_driver_unload( struct drm_device *dev )
{
	drm_mali_private_t *dev_priv = dev->dev_private;
	printk(KERN_ERR "DRM: mali_driver_unload start\n");

#ifdef DRM_SMAN_H
	drm_sman_takedown(&dev_priv->sman);
#else
	idr_remove_all(&dev_priv->object_idr);
	idr_destroy(&dev_priv->object_idr);
#endif
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,32)
	drm_free(dev_priv, sizeof(*dev_priv), DRM_MEM_DRIVER);
#else
	drm_free_large(dev_priv);
#endif
	//kfree( dev_priv );
	printk(KERN_ERR "DRM: mali_driver_unload done\n");

	return 0;
}

#ifndef DRM_SMAN_H
static const struct file_operations mali_driver_fops = {
	.owner = THIS_MODULE,
	.open = drm_open,
	.release = drm_release,
	.unlocked_ioctl = drm_ioctl,
	.mmap = drm_mmap,
	.poll = drm_poll,
	.fasync = drm_fasync,
	.llseek = noop_llseek,
};

static int mali_driver_open(struct drm_device *dev, struct drm_file *file)
{
	struct mali_file_private *file_priv;

	DRM_DEBUG_DRIVER("\n");
	file_priv = kmalloc(sizeof(*file_priv), GFP_KERNEL);
	if (!file_priv)
		return -ENOMEM;

	file->driver_priv = file_priv;

	INIT_LIST_HEAD(&file_priv->obj_list);

	return 0;
}

void mali_driver_postclose(struct drm_device *dev, struct drm_file *file)
{
	struct mali_file_private *file_priv = file->driver_priv;

	kfree(file_priv);
}
#endif

static struct drm_driver driver = 
{
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,32)
	.driver_features = DRIVER_USE_PLATFORM_DEVICE,
#else
	/*
	.driver_features = DRIVER_MODESET | DRIVER_HAVE_DMA | DRIVER_FB_DMA,
	*/
	.driver_features = DRIVER_FB_DMA,
#endif
	.load = mali_driver_load,
	.unload = mali_driver_unload,
#ifndef DRM_SMAN_H
	.open = mali_driver_open,
	.postclose = mali_driver_postclose,
#endif
	.context_dtor = NULL,
	.dma_quiescent = mali_idle,
	.reclaim_buffers = NULL,
	.reclaim_buffers_idlelocked = mali_reclaim_buffers_locked,
	.lastclose = mali_lastclose,
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,32)
	.get_map_ofs = drm_core_get_map_ofs,
	.get_reg_ofs = drm_core_get_reg_ofs,
#endif
	.ioctls = mali_ioctls,
#ifdef DRM_SMAN_H
	.fops = {
		 .owner = THIS_MODULE,
		 .open = drm_open,
		 .release = drm_release,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39)
		 .ioctl = drm_ioctl,
#else
		 .unlocked_ioctl = drm_ioctl,
#endif
		 .mmap = drm_mmap,
		 .poll = drm_poll,
		 .fasync = drm_fasync,
	},
#else
	.fops = &mali_driver_fops,
#endif
	.name = DRIVER_NAME,
	.desc = DRIVER_DESC,
	.date = DRIVER_DATE,
	.major = DRIVER_MAJOR,
	.minor = DRIVER_MINOR,
	.patchlevel = DRIVER_PATCHLEVEL,
};

static int __init mali_init(void)
{
	driver.num_ioctls = mali_max_ioctl;
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,32)
	return drm_init(&driver);
#else
	pdev = platform_device_register_simple(mali_drm_device_name, 0, NULL, 0);
	return drm_platform_init(&driver, pdev);
#endif
}

static void __exit mali_exit(void)
{
	platform_device_unregister( pdev );
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,32)
	drm_exit(&driver);
#else
	drm_platform_exit(&driver, pdev);
#endif
}

module_init(mali_init);
module_exit(mali_exit);

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_AUTHOR("ARM Ltd.");
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL and additional rights");
