# ARM Mali driver for VIA VAB-600

Driver source code for Mali graphics used on the [VIA VAB-600][vab600]
board.

Due to ARM's licensing requirements, only some part of the driver is open source,
while the core `libMali.so` and `libUMP.so` libraries are proprietary and provided
as precompiled binaries.

_This release is a work in progress._

[vab600]: http://www.viatech.com/en/boards/pico-itx/vab-600/ "VAB-600 product page"

## Release r3p2

Building the `r3p2` release requires approximately these steps. The installed packages
and settings are to cross-compile on Ubuntu 15.10, but other Linux distributions should
be similar. The instructions are given for compiling for Hard Float (`armhf`) build.

Install the basic required packages:

    sudo apt-get build-essential automake libtool xorg-dev libdrm-dev xutils-dev

Install the tools for cross-compilation. Please note, that gcc 4.x is supported for
these drivers, the 5.x line does not work.

    sudo apt-get install gcc-4.9-arm-linux-gnueabihf g++-4.9-arm-linux-gnueabihf

Need to manually add the symlinks for the relevant binaries. For `armhf`:

    cd /usr/bin/
    for f in arm-linux-gnueabihf-*-*; do echo $f; sudo ln -s $f ${f%%-4.9}; done

### Kernel Preparation

Prepare the VAB-600 Linux kernel in a directory. Either you have it already, copy it
from the BSP, or clone it from the [VAB-600 Kernel BSP Github repo][vab600kernel]. In
this guide we'll assume to have it at `vab600-kernel-bsp` at the root of this repository.

Prepare the kernel by running the following command

Kernel:

    make ARCH=arm VAB-600_Linux_defconfig
    make ARCH=arm menuconfig  # Change General Setup > Cross Compiler Tool prefix
                              # the toolchain installed above:
			      # arm-linux-gnueabihf-
    make ARCH=arm prepare
    make ARCH=arm modules

### Quick Steps

You can use the provided experimental Makefile to prepare the entire driver package
in one go. In the `r3p2` directory run

    make

There are a number of parameters to adjust, and their default value is:

    KVER=3.0.8                         # Kernel version from the previous step
    KDIR=$(PWD)/../vab600-kernel-bsp   # Kernel directory from the previous step
    ABI=armhf                          # Currently can create only armhf drivers, not armel
    CROSS_COMPILE=arm-linux-gnueabihf- # Cross compile toolchain prefix from setup

In most cases you would need to adjust the values of `KDIR` or `CROSS_COMPILE`, and then
you'd need to run:

    make KDIR=<kernel directory> CROSS_COMPILE=<cross compiler prefix>

The resulting driver package will be generated at `vab600-libMali-r3p2-armhf.tar.gz`.

### Detailed Steps

Below are the detailed steps to compile the drivers piecemeal, if for any reason the
quick steps would fail.

#### Kernel Drivers (DX910-SW-99002)

In `vab600-mali/r3p2/DX910-SW-99002-r3p2-01rel0/` now you can run the following command
to compile the Mali kernel driver:

    make KVER=3.0.8 KDIR=${PWD}/../../vab600-kernel-bsp/ TARGET_PLATFORM=wm3481 CROSS_COMPILE=arm-linux-gnueabihf- STRIP=arm-linux-gnueabihf-strip BUILD=release

The resulting drivers will be packaged up in a file such as `DX910-SW-99002-r3p2-01rel0-20151209-1.tgz`
in the same directory (different file name according to the compilation date).

### X11 Drivers (DX910-SW-99003)

Enter the `vab600-mali/r3p2/DX910-SW-99003-r3p2-01rel0/` directory. Symlink the appropriate precompiled modules proprietary drivers to the `prebuilt` directory, e.g. the `armhf` drivers such as:

    ln -s ${PWD}/../libMali/armhf/usr prebuilt/usr

Then compile the drivers withe following commands:

    make CC=arm-linux-gnueabihf-gcc CXX=arm-linux-gnueabihf-g++ AR=arm-linux-gnueabihf-ar RANLIB=arm-linux-gnueabihf-ranlib STRIP=arm-linux-gnueabihf-strip
    make CC=arm-linux-gnueabihf-gcc CXX=arm-linux-gnueabihf-g++ AR=arm-linux-gnueabihf-ar RANLIB=arm-linux-gnueabihf-ranlib STRIP=arm-linux-gnueabihf-strip install

The resulting drivers will be packed up in a file such as `DX910-SW-99003-r3p2-01rel0-20151209-1.armhf.tgz`
in the same direct (different file name according to the compilation date).

[vab600kernel]: https://github.com/viaembedded/vab600-kernel-bsp "viaembedded/vab600-kernel-bsp"
