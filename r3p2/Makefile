# Experimental Makefile to generate a combined driver package
# for the VIA VAB-600 Mali driver

####
# Adjustable variables
####
KVER  ?= 3.0.8
KDIR  ?= $(PWD)/../vab600-kernel-bsp
# Values can be: armel / armhf
ABI   ?= armhf
CROSS_COMPILE ?= arm-linux-gnueabihf-
####

CC     = $(CROSS_COMPILE)gcc
CXX    = $(CROSS_COMPILE)g++
AR     = $(CROSS_COMPILE)ar
RANLIB = $(CROSS_COMPILE)ranlib
STRIP  = $(CROSS_COMPILE)strip

export CROSS_COMPILE CC CX AR RANLIB STRIP

# Helper variables
DRIVER_VERSION ?= r3p2-01rel0
TARGETS = DX910-SW-99002-$(DRIVER_VERSION) DX910-SW-99003-$(DRIVER_VERSION)
PACKAGENAME = vab600-libMali-$(DRIVER_VERSION)-$(ABI).tar

all: $(TARGETS)
	tar -cvf $(PACKAGENAME) -C libMali/$(ABI)/ . && \
	tar -rvf $(PACKAGENAME) -C DX910-SW-99002-$(DRIVER_VERSION)/out/ . && \
	tar -rvf $(PACKAGENAME) -C DX910-SW-99003-$(DRIVER_VERSION)/out/ . && \
	if [ -e "$(PACKAGENAME).gz" ]; then rm $(PACKAGENAME).gz; fi
	gzip $(PACKAGENAME)

DX910-SW-99002-$(DRIVER_VERSION):
	cd $@ && $(MAKE) TARGET_PLATFORM=wm3481 KVER=$(KVER) KDIR=$(KDIR) CROSS_COMPILE=$(CROSS_COMPILE) STRIP=$(STRIP)

DX910-SW-99003-$(DRIVER_VERSION):
	cd $@ ; \
	if [ -h "prebuilt/usr" ]; then rm "prebuilt/usr"; fi; \
	ln -s $(PWD)/libMali/$(ABI)/usr prebuilt/usr; \
	$(MAKE) && $(MAKE) install

clean:
	for target in $(TARGETS); do \
	  cd "$(PWD)/$$target"; \
          if [ -d "out" ]; then rm -rf "out"; fi \
        done
	if [ -e "$(PACKAGENAME).gz" ]; then rm $(PACKAGENAME).gz; fi

.PHONY: all clean $(TARGETS)
