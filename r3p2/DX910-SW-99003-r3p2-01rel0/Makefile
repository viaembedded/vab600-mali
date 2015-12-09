# Build top Makefile for Mali
# Written by WonderMedia Technologies, Inc.

MALI_BUILD_TOP ?= $(PWD)/..
PKG_NAME = DX910-SW-99003
PKG_VERS = r3p2-01rel0
DESTDIR ?= $(PWD)/out

export MALI_BUILD_TOP PKG_NAME PKG_VERS

ifeq ($(shell $(CC) -v 2>&1|grep Target|cut -d\  -f2), arm-linux-gnueabihf)
	ARCH = armhf
else
	ARCH = armel
endif

all:
	@(cd x11/xf86-video-mali-0.0.1; ./autogen.sh; ./configure --host=arm-none-linux --enable-shared --prefix=/usr; make clean; make)

clean:
	@(cd x11/xf86-video-mali-0.0.1; make clean)
	@rm -rf $(PWD)/out

distclean:
	@(cd x11/xf86-video-mali-0.0.1; make distclean)

install:
	@(cd x11/xf86-video-mali-0.0.1; make DESTDIR=$(DESTDIR) install)
	@rm $(DESTDIR)/usr/lib/xorg/modules/drivers/mali_drv.la
	@chmod +x prebuilt/etc/rcS.d/S60mali
	@cp -a prebuilt/etc $(DESTDIR)/
	@tar zcvf $(PKG_NAME)-$(PKG_VERS)-`date +"%Y%m%d"`-1.$(ARCH).tgz -C $(DESTDIR) .
