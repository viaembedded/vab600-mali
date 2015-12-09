/*
 * Copyright (C) 2010, 2012 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "drm/drmP.h"
#include "mali_drm.h"
#include "mali_drv.h"

#ifndef DRM_SMAN_H
struct mali_memreq {
	__u32 offset;
	__u32 size;
};

struct mali_memblock {
	struct drm_mm_node mm_node;
	struct mali_memreq req;
	struct list_head owner_list;
};

void mali_malloc(struct mali_memreq *req)
{
	req->offset = req->size = 0;
}

void mali_free(u32 base)
{
	return;
}
#endif

#define VIDEO_TYPE 0
#define MEM_TYPE 1

#define MALI_MM_ALIGN_SHIFT 4
#define MALI_MM_ALIGN_MASK ( (1 << MALI_MM_ALIGN_SHIFT) - 1)

#define DRM_IOCTL_DEF(ioctl, _func, _flags) \
	[DRM_IOCTL_NR(ioctl)] = {.cmd = ioctl, .func = _func, .flags = _flags, .cmd_drv = 0}

#ifdef DRM_SMAN_H
static void *mali_sman_mm_allocate( void *private, unsigned long size, unsigned alignment )
{
	printk(KERN_ERR "DRM: %s\n", __func__);
	//struct mali_memreq req;

	//req.size = size;
	/* XXX _ump_ukk_allocate( _ump_uk_allocate_s *user_interaction ); */
	//if (req.size == 0) return NULL;
	//else return (void *)(unsigned long)~req.offset;
	return NULL;
}

static void mali_sman_mm_free( void *private, void *ref )
{
	printk(KERN_ERR "DRM: %s\n", __func__);
	/*ump_free(~((unsigned long)ref));*/

}

static void mali_sman_mm_destroy( void *private )
{
	printk(KERN_ERR "DRM: %s\n", __func__);
}

static unsigned long mali_sman_mm_offset( void *private, void *ref )
{
	printk(KERN_ERR "DRM: %s\n", __func__);
	return ~((unsigned long)ref);
}
#endif

static int mali_fb_init( struct drm_device *dev, void *data, struct drm_file *file_priv )
{
	drm_mali_private_t *dev_priv = dev->dev_private;
	drm_mali_fb_t *fb = data;
#ifdef DRM_SMAN_H
	int ret;
#endif
	printk(KERN_ERR "DRM: %s\n", __func__);

	mutex_lock(&dev->struct_mutex);
#ifdef DRM_SMAN_H
	{
		struct drm_sman_mm sman_mm;
		sman_mm.private = (void *)0xFFFFFFFF;
		sman_mm.allocate = mali_sman_mm_allocate;
		sman_mm.free = mali_sman_mm_free;
		sman_mm.destroy = mali_sman_mm_destroy;
		sman_mm.offset = mali_sman_mm_offset;
		ret = drm_sman_set_manager(&dev_priv->sman, VIDEO_TYPE, &sman_mm);
	}

	if (ret) {
		DRM_ERROR("VRAM memory manager initialisation error\n");
		mutex_unlock(&dev->struct_mutex);
		return ret;
	}
#else
	/* Unconditionally init the drm_mm, even though we don't use it when the
	 * fb mali driver is available - make cleanup easier. */
	drm_mm_init(&dev_priv->vram_mm, 0, fb->size >> MALI_MM_ALIGN_SHIFT);
#endif

	dev_priv->vram_initialized = 1;
	dev_priv->vram_offset = fb->offset;

	mutex_unlock(&dev->struct_mutex);
	DRM_DEBUG("offset = %u, size = %u\n", fb->offset, fb->size);

	return 0;
}

#ifdef DRM_SMAN_H
static int mali_drm_alloc(struct drm_device *dev, struct drm_file *file_priv, void *data, int pool)
{
	drm_mali_private_t *dev_priv = dev->dev_private;
	drm_mali_mem_t *mem = data;
	int retval = 0;
	struct drm_memblock_item *item;
	printk(KERN_ERR "DRM: %s\n", __func__);

	mutex_lock(&dev->struct_mutex);

	if (0 == dev_priv->vram_initialized )
	{
		DRM_ERROR("Attempt to allocate from uninitialized memory manager.\n");
		mutex_unlock(&dev->struct_mutex);
		return -EINVAL;
	}

	mem->size = (mem->size + MALI_MM_ALIGN_MASK) >> MALI_MM_ALIGN_SHIFT;
	item = drm_sman_alloc(&dev_priv->sman, pool, mem->size, 0,
			      (unsigned long)file_priv);

	mutex_unlock(&dev->struct_mutex);
	if (item) 
	{
		mem->offset = dev_priv->vram_offset + (item->mm->offset(item->mm, item->mm_info) << MALI_MM_ALIGN_SHIFT);
		mem->free = item->user_hash.key;
		mem->size = mem->size << MALI_MM_ALIGN_SHIFT;
	} else {
		mem->offset = 0;
		mem->size = 0;
		mem->free = 0;
		retval = -ENOMEM;
	}

	DRM_DEBUG("alloc %d, size = %d, offset = %d\n", pool, mem->size, mem->offset);

	return retval;
}

static int mali_drm_free(struct drm_device *dev, void *data, struct drm_file *file_priv)
{
	drm_mali_private_t *dev_priv = dev->dev_private;
	drm_mali_mem_t *mem = data;
	int ret;
	printk(KERN_ERR "DRM: %s\n", __func__);

	mutex_lock(&dev->struct_mutex);
	ret = drm_sman_free_key(&dev_priv->sman, mem->free);
	mutex_unlock(&dev->struct_mutex);
	DRM_DEBUG("free = 0x%lx\n", mem->free);

	return ret;
}

#else
static int mali_drm_alloc(struct drm_device *dev, struct drm_file *file, void *data, int pool)
{
	drm_mali_private_t *dev_priv = dev->dev_private;
	drm_mali_mem_t *mem = data;
	int retval = 0, user_key;
	struct mali_memblock *item;
	struct mali_file_private *file_priv = file->driver_priv;
	unsigned long offset;

	mutex_lock(&dev->struct_mutex);

	if (0 == dev_priv->vram_initialized) {
		DRM_ERROR
		    ("Attempt to allocate from uninitialized memory manager.\n");
		mutex_unlock(&dev->struct_mutex);
		return -EINVAL;
	}

	item = kzalloc(sizeof(*item), GFP_KERNEL);
	if (!item) {
		retval = -ENOMEM;
		goto fail_alloc;
	}

	mem->size = (mem->size + MALI_MM_ALIGN_MASK) >> MALI_MM_ALIGN_SHIFT;
	if (pool == MEM_TYPE) {
		retval = drm_mm_insert_node(&dev_priv->mem_mm,
					    &item->mm_node,
					    mem->size, 0);
		offset = item->mm_node.start;
	} else {
		item->req.size = mem->size;
		mali_malloc(&item->req);
		if (item->req.size == 0)
			retval = -ENOMEM;
		offset = item->req.offset;
	}
	if (retval)
		goto fail_alloc;

again:
	if (idr_pre_get(&dev_priv->object_idr, GFP_KERNEL) == 0) {
		retval = -ENOMEM;
		goto fail_idr;
	}

	retval = idr_get_new_above(&dev_priv->object_idr, item, 1, &user_key);
	if (retval == -EAGAIN)
		goto again;
	if (retval)
		goto fail_idr;

	list_add(&item->owner_list, &file_priv->obj_list);
	mutex_unlock(&dev->struct_mutex);

	mem->offset = ((pool == 0) ?
		      dev_priv->vram_offset : dev_priv->mem_offset) +
	    (offset << MALI_MM_ALIGN_SHIFT);
	mem->free = user_key;
	mem->size = mem->size << MALI_MM_ALIGN_SHIFT;

	return 0;

fail_idr:
	drm_mm_remove_node(&item->mm_node);
fail_alloc:
	kfree(item);
	mutex_unlock(&dev->struct_mutex);

	mem->offset = 0;
	mem->size = 0;
	mem->free = 0;

	DRM_DEBUG("alloc %d, size = %d, offset = %d\n", pool, mem->size,
		  mem->offset);

	return retval;
}

static int mali_drm_free(struct drm_device *dev, void *data, struct drm_file *file_priv)
{
	drm_mali_private_t *dev_priv = dev->dev_private;
	drm_mali_mem_t *mem = data;
	struct mali_memblock *obj;

	printk(KERN_ERR "DRM: %s\n", __func__);

	mutex_lock(&dev->struct_mutex);
	obj = idr_find(&dev_priv->object_idr, mem->free);
	if (obj == NULL) {
		mutex_unlock(&dev->struct_mutex);
		return -EINVAL;
	}

	idr_remove(&dev_priv->object_idr, mem->free);
	list_del(&obj->owner_list);
	if (drm_mm_node_allocated(&obj->mm_node))
		drm_mm_remove_node(&obj->mm_node);
	else
		mali_free(obj->req.offset);
	kfree(obj);
	mutex_unlock(&dev->struct_mutex);
	DRM_DEBUG("free = 0x%lx\n", mem->free);

	return 0;
}
#endif

static int mali_fb_alloc(struct drm_device *dev, void *data, struct drm_file *file_priv)
{
	printk(KERN_ERR "DRM: %s\n", __func__);
	return mali_drm_alloc(dev, file_priv, data, VIDEO_TYPE);
}

static int mali_ioctl_mem_init(struct drm_device *dev, void *data, struct drm_file *file_priv)
{
	drm_mali_private_t *dev_priv = dev->dev_private;
	drm_mali_mem_t *mem= data;
#ifdef DRM_SMAN_H
	int ret;
#endif
	dev_priv = dev->dev_private;
	printk(KERN_ERR "DRM: %s\n", __func__);

	mutex_lock(&dev->struct_mutex);
#ifdef DRM_SMAN_H
	ret = drm_sman_set_range(&dev_priv->sman, MEM_TYPE, 0, mem->size >> MALI_MM_ALIGN_SHIFT);

	if (ret) {
		DRM_ERROR("MEM memory manager initialisation error\n");
		mutex_unlock(&dev->struct_mutex);
		return ret;
	}
#else
	drm_mm_init(&dev_priv->mem_mm, 0, mem->size >> MALI_MM_ALIGN_SHIFT);
	dev_priv->mem_initialized = 1;
	dev_priv->mem_offset = mem->offset;
#endif

	//dev_priv->agp_initialized = 1;
	//dev_priv->agp_offset = agp->offset;
	mutex_unlock(&dev->struct_mutex);

	//DRM_DEBUG("offset = %u, size = %u\n", agp->offset, agp->size);
	return 0;
}

static int mali_ioctl_mem_alloc(struct drm_device *dev, void *data,
			       struct drm_file *file_priv)
{

	printk(KERN_ERR "DRM: %s\n", __func__);
	return mali_drm_alloc(dev, file_priv, data, MEM_TYPE);
}

#if 0
static drm_local_map_t *mem_reg_init(struct drm_device *dev)
{
	struct drm_map_list *entry;
	drm_local_map_t *map;
	printk(KERN_ERR "DRM: %s\n", __func__);

	list_for_each_entry(entry, &dev->maplist, head) {
		map = entry->map;
		if (!map)
			continue;
		if (map->type == _DRM_REGISTERS) {
			return map;
		}
	}
	return NULL;
}
#endif

int mali_idle(struct drm_device *dev)
{
	drm_mali_private_t *dev_priv = dev->dev_private;
	/*
	uint32_t idle_reg;
	unsigned long end;
	int i;
	*/
	printk(KERN_ERR "DRM: %s\n", __func__);

	if (dev_priv->idle_fault)
		return 0;

	#if 0
	if (dev_priv->mmio == NULL) {
		dev_priv->mmio = mali_reg_init(dev);
		if (dev_priv->mmio == NULL) {
			DRM_ERROR("Could not find register map.\n");
			return 0;
		}
	}
	#endif

	return 0;
}


void mali_lastclose(struct drm_device *dev)
{
	drm_mali_private_t *dev_priv = dev->dev_private;
	printk(KERN_ERR "DRM: %s\n", __func__);

	if (!dev_priv) return;

	mutex_lock(&dev->struct_mutex);
#ifdef DRM_SMAN_H
	drm_sman_cleanup(&dev_priv->sman);
	dev_priv->vram_initialized = 0;
#else
	if (dev_priv->vram_initialized) {
		drm_mm_takedown(&dev_priv->vram_mm);
		dev_priv->vram_initialized = 0;
	}
	if (dev_priv->mem_initialized) {
		drm_mm_takedown(&dev_priv->mem_mm);
		dev_priv->mem_initialized = 0;
	}
#endif
	dev_priv->mmio = NULL;
	mutex_unlock(&dev->struct_mutex);
}

#ifdef DRM_SMAN_H
void mali_reclaim_buffers_locked(struct drm_device * dev, struct drm_file *file_priv)
{
	drm_mali_private_t *dev_priv = dev->dev_private;
	printk(KERN_ERR "DRM: %s\n", __func__);

	mutex_lock(&dev->struct_mutex);
	if (drm_sman_owner_clean(&dev_priv->sman, (unsigned long)file_priv)) 
	{
		mutex_unlock(&dev->struct_mutex);
		return;
	}

	if (dev->driver->dma_quiescent) 
	{
		dev->driver->dma_quiescent(dev);
	}

	drm_sman_owner_cleanup(&dev_priv->sman, (unsigned long)file_priv);
	mutex_unlock(&dev->struct_mutex);
	return;
}
#else
void mali_reclaim_buffers_locked(struct drm_device * dev, struct drm_file *file)
{
	struct mali_file_private *file_priv = file->driver_priv;
	struct mali_memblock *entry, *next;

	printk(KERN_ERR "DRM: %s\n", __func__);

	mutex_lock(&dev->struct_mutex);
	if (list_empty(&file_priv->obj_list))
	{
		mutex_unlock(&dev->struct_mutex);
		return;
	}

	if (dev->driver->dma_quiescent)
	{
		dev->driver->dma_quiescent(dev);
	}

	list_for_each_entry_safe(entry, next, &file_priv->obj_list,
				 owner_list) {
		list_del(&entry->owner_list);
		if (drm_mm_node_allocated(&entry->mm_node))
			drm_mm_remove_node(&entry->mm_node);
		else
			mali_free(entry->req.offset);
		kfree(entry);
	}

	mutex_unlock(&dev->struct_mutex);
	return;
}
#endif

struct drm_ioctl_desc mali_ioctls[] = {
#ifdef DRM_SMAN_H
	DRM_IOCTL_DEF(DRM_MALI_FB_ALLOC, mali_fb_alloc, DRM_AUTH),
	DRM_IOCTL_DEF(DRM_MALI_FB_FREE, mali_drm_free, DRM_AUTH),
	DRM_IOCTL_DEF(DRM_MALI_MEM_INIT, mali_ioctl_mem_init, DRM_AUTH | DRM_MASTER | DRM_ROOT_ONLY),
	DRM_IOCTL_DEF(DRM_MALI_MEM_ALLOC, mali_ioctl_mem_alloc, DRM_AUTH),
	DRM_IOCTL_DEF(DRM_MALI_MEM_FREE, mali_drm_free, DRM_AUTH),
	DRM_IOCTL_DEF(DRM_MALI_FB_INIT, mali_fb_init, DRM_AUTH | DRM_MASTER | DRM_ROOT_ONLY),
#else
	DRM_IOCTL_DEF_DRV(MALI_FB_ALLOC, mali_fb_alloc, DRM_AUTH),
	DRM_IOCTL_DEF_DRV(MALI_FB_FREE, mali_drm_free, DRM_AUTH),
	DRM_IOCTL_DEF_DRV(MALI_MEM_INIT, mali_ioctl_mem_init, DRM_AUTH | DRM_MASTER | DRM_ROOT_ONLY),
	DRM_IOCTL_DEF_DRV(MALI_MEM_ALLOC, mali_ioctl_mem_alloc, DRM_AUTH),
	DRM_IOCTL_DEF_DRV(MALI_MEM_FREE, mali_drm_free, DRM_AUTH),
	DRM_IOCTL_DEF_DRV(MALI_FB_INIT, mali_fb_init, DRM_AUTH | DRM_MASTER | DRM_ROOT_ONLY),
#endif
};

int mali_max_ioctl = DRM_ARRAY_SIZE(mali_ioctls);
