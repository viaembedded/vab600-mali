# Copyright (c) 2012 WonderMedia Technologies, Inc. All rights reserved.

DRIVER_NAME    ?= DX910-SW-99002
DRIVER_VERSION ?= r3p2-01rel0

KVER  ?= 3.0.8
KDIR  ?= $(HOME)/kernel/ANDROID_$(KVER)
BUILD ?= release
REL ?= $(shell date +%Y%m%d)-1

ARCH_CPU  ?= armv7
ARCH_MALI ?= m400
TARGET_PLATFORM ?= wm3481
USING_UMP ?= 1
USING_PMM ?= 1
USING_MALI_RUN_TIME_PM ?= 1
USING_GPU_UTILIZATION  ?= 1
USING_STRIP ?= 1

MALI_DRM  ?= 1

CONFIG ?= $(ARCH_CPU)-$(TARGET_PLATFORM)-$(ARCH_MALI)
CONFIG_SMP := $(shell cat $(KDIR)/.config|grep 'CONFIG_SMP=y'|cut -d= -f2)
CONFIG_GATOR := $(shell cat $(KDIR)/.config|grep 'CONFIG_CONTEXT_SWITCH_TRACER=y'|cut -d= -f2)

ifeq ($(CONFIG_SMP),y)
	REL := $(REL).smp
endif

ifeq ($(CONFIG_GATOR),y)
	REL := $(REL).gator
endif

ifneq ($(BUILD),release)
	REL := $(REL).debug
endif

# Please enable these items in kernel's menuconfig
# CONFIG_PM, CONFIG_HAS_EARLYSUSPEND, CONFIG_PM_RUNTIME, CONFIG_PM_SLEEP

COMMON_DEFINES =

COMMON_DEFINES = ONLY_ZBT=0 USING_OS_MEMORY=1 TARGET_PLATFORM=$(TARGET_PLATFORM) \
	USING_PMM=$(USING_PMM) USING_MALI_RUN_TIME_PM=$(USING_MALI_RUN_TIME_PM) \
	USING_MMU=1 USING_UMP=$(USING_UMP) BUILD=$(BUILD) \
	USING_GPU_UTILIZATION=$(USING_GPU_UTILIZATION) \
	KDIR=$(KDIR)

MALI_DEFINES = $(COMMON_DEFINES)
UMP_DEFINES  = $(COMMON_DEFINES) CONFIG=$(CONFIG)

UMP_DIR      := $(PWD)/driver/src/devicedrv/ump
MALI_DIR     := $(PWD)/driver/src/devicedrv/mali
MALI_DRM_DIR := $(PWD)/driver/src/egl/x11/drm_module/mali_drm

all:
ifeq ($(USING_UMP),1)
	$(UMP_DEFINES) make -C $(UMP_DIR)
endif
	$(MALI_DEFINES) make -C $(MALI_DIR)
	rm -rf out
	mkdir -p out/lib/modules/$(KVER)
ifeq ($(USING_STRIP),1)
	$(STRIP) --strip-unneeded $(MALI_DIR)/mali.ko
	$(STRIP) --strip-unneeded $(UMP_DIR)/ump.ko
endif
	cp -a $(MALI_DIR)/mali.ko         out/lib/modules/$(KVER)/
	cp -a $(UMP_DIR)/ump.ko           out/lib/modules/$(KVER)/
	ln -s /lib/modules/$(KVER)/mali.ko     out/lib/modules/mali.ko
	ln -s /lib/modules/$(KVER)/ump.ko      out/lib/modules/ump.ko
ifeq ($(MALI_DRM),1)
	$(MALI_DEFINES) make -C $(MALI_DRM_DIR)
	cp -a $(MALI_DRM_DIR)/mali_drm.ko out/lib/modules/$(KVER)/
	ln -s /lib/modules/$(KVER)/mali_drm.ko out/lib/modules/mali_drm.ko
endif
	chmod 644 out/lib/modules/$(KVER)/*.ko
	tar zcvf $(DRIVER_NAME)-$(DRIVER_VERSION)-$(REL).armel.tgz -C out/ .

clean:
ifeq ($(USING_UMP),1)
	$(UMP_DEFINES) make -C $(UMP_DIR) clean
endif
ifeq ($(MALI_DRM),1)
	$(MALI_DEFINES) make -C $(MALI_DRM_DIR) clean
endif
	$(MALI_DEFINES) make -C $(MALI_DIR) clean
	rm -rf out
	find -type l -name arch -exec rm {} \;

