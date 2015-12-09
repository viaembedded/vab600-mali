/*
 * Copyright (C) 2010 ARM Limited. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/ioctl.h>
#include "xorg-server.h"
#include "xf86.h"
#include "xf86Crtc.h"

#include "mali_def.h"
#include "mali_fbdev.h"
#include "mali_lcd.h"

#define FIX_VGA
#define USE_EDID_CACHE

/* VIA VEPD patch for video output */
//#define VIA_VEPD

#ifdef USE_EDID_CACHE
#define EDID_CACHE_SIZE 512
static unsigned char edid_cache[EDID_CACHE_SIZE];
#endif

#ifdef VIA_VEPD
/*
#define IGNORE(a) (a=a)
*/
#define OUTPUT_NAME	"DVI-I"
#define mode_array_size (sizeof(VIASupportMode) / sizeof(VIASUPPORTMODE))
#define KHZ2PICOS(a)    (1000000000UL/(a))

typedef struct _VIASUPPORTMODE {
	char  Name[25];
	int   HActive;
	int   VActive;
	int   pixclk;
} VIASUPPORTMODE;

VIASUPPORTMODE VIASupportMode[] = {
	{"640x480@60 DMT/CEA861",       640,    480,    25175},
	{"640x480@60 CVT",              640,    480,    23750},
	{"640x480@75 DMT ",             640,    480,    31500},
	{"640x480@75 CVT",              640,    480,    30750},
	{"800x480@60 CVT",              800,    480,    29500},
	{"800x600@60 DMT",              800,    600,    40000},
	{"800x600@60 CVT ",             800,    600,    38250},
	{"800x600@75 DMT",              800,    600,    49500},
	{"800x600@75 CVT",              800,    600,    49000},
	{"1024x768@60 DMT",             1024,   768,    65000},
	{"1024x768@60 CVT",             1024,   768,    63500},
	{"1024x768@75 DMT",             1024,   768,    78750},
	{"1024x768@75 CVT ",            1024,   768,    82000},
	{"1280x720@60 CEA861",          1280,   720,    74250},
	{"1280x720@60 CVT",             1280,   720,    74500},
	{"1280x768@60 DMT/CVT",         1280,   768,    79500},
	{"1280x1024@60 DMT",            1280,   1024,   108000},
	{"1280x1024@60 CVT",            1280,   1024,   109000},
	{"1280x1024@75 DMT",            1280,   1024,   135000},
	{"1280x1024@75 CVT",            1280,   1024,   138750},
	{"1360x768@60",                 1360,   768,    85500},
	{"1400x1050@60 DMT/CVT",        1400,   1050,   121750},
	{"1400x1050@60+R",              1400,   1050,   101000},
	{"1440x900@60 DMT/CVT",         1440,   900,    106500},
	{"1440x900@75 DMT/CVT",         1440,   900,    136750},
	{"1680x1050@60 DMT/CVT",        1680,   1050,   146250},
	{"1920x1080p@60",               1920,   1080,   148500},
	{"1920x1080i@60",               1920,   1080,   74250},
	{"",                             0,      0,      0},
};
#endif /* VIA_VEPD */

static void fbdev_lcd_crtc_dpms(xf86CrtcPtr crtc, int mode)
{
	IGNORE(crtc);
	IGNORE(mode);
}

static Bool fbdev_lcd_crtc_lock(xf86CrtcPtr crtc)
{
	IGNORE( crtc );

	return TRUE;
}

static void fbdev_lcd_crtc_unlock(xf86CrtcPtr crtc)
{
	IGNORE( crtc );
}

static Bool fbdev_lcd_crtc_mode_fixup(xf86CrtcPtr crtc, DisplayModePtr mode, DisplayModePtr adjusted_mode)
{
	IGNORE( crtc );
	IGNORE( mode );
	IGNORE( adjusted_mode );

	return TRUE;
}

static void fbdev_lcd_crtc_prepare(xf86CrtcPtr crtc)
{
	IGNORE( crtc );
}

static void fbdev_lcd_crtc_mode_set(xf86CrtcPtr crtc, DisplayModePtr mode, DisplayModePtr adjusted_mode, int x, int y)
{
	IGNORE( crtc );
	IGNORE( mode );
	IGNORE( adjusted_mode );
	IGNORE( x );
	IGNORE( y );
}

static void fbdev_lcd_crtc_commit(xf86CrtcPtr crtc)
{
	IGNORE( crtc );
}

static void fbdev_lcd_crtc_gamma_set(xf86CrtcPtr crtc, CARD16 *red, CARD16 *green, CARD16 *blue, int size)
{
	IGNORE( crtc );
	IGNORE( red );
	IGNORE( green );
	IGNORE( blue );
	IGNORE( size );
}

static void fbdev_lcd_crtc_set_origin(xf86CrtcPtr crtc, int x, int y)
{
	IGNORE( crtc );
	IGNORE( x );
	IGNORE( y );
}

static const xf86CrtcFuncsRec fbdev_lcd_crtc_funcs =
{
	.dpms = fbdev_lcd_crtc_dpms,
	.save = NULL,
	.restore = NULL,
	.lock = fbdev_lcd_crtc_lock,
	.unlock = fbdev_lcd_crtc_unlock,
	.mode_fixup = fbdev_lcd_crtc_mode_fixup,
	.prepare = fbdev_lcd_crtc_prepare,
	.mode_set = fbdev_lcd_crtc_mode_set,
	.commit = fbdev_lcd_crtc_commit,
	.gamma_set = fbdev_lcd_crtc_gamma_set,
	.shadow_allocate = NULL,
	.shadow_create = NULL,
	.shadow_destroy = NULL,
	.set_cursor_colors = NULL,
	.set_cursor_position = NULL,
	.show_cursor = NULL,
	.hide_cursor = NULL,
	.load_cursor_image = NULL,
	.load_cursor_argb = NULL,
	.destroy = NULL,
	.set_mode_major = NULL,
	.set_origin = fbdev_lcd_crtc_set_origin,
};

static void fbdev_lcd_output_dpms(xf86OutputPtr output, int mode)
{
	MaliPtr fPtr = MALIPTR(output->scrn);

	if ( mode == DPMSModeOn )
	{
		ioctl(fPtr->fb_lcd_fd, FBIOBLANK, FB_BLANK_UNBLANK);
	}
	else if( mode == DPMSModeOff )
	{
		ioctl(fPtr->fb_lcd_fd, FBIOBLANK, FB_BLANK_POWERDOWN);
	}
}

static void fbdev_lcd_output_save(xf86OutputPtr output)
{
	IGNORE( output );
}

static void fbdev_lcd_output_restore(xf86OutputPtr output)
{
	IGNORE( output );
}

#ifdef VIA_VEPD
static int IsValidMode(int xres, int yres, int pixclk)
{
	unsigned int i;
	IGNORE(pixclk);
	for(i = 0; i < mode_array_size; i++)
	{
		if (xres < VIASupportMode[i].HActive)
			return -1;
		if ((VIASupportMode[i].HActive == xres) &&
		    (VIASupportMode[i].VActive == yres))//&&
		    // (VIASupportMode[i].pixclk  == pixclk))
			return i;
	}

	return -1;

}
#endif

static int fbdev_lcd_output_mode_valid(xf86OutputPtr output, DisplayModePtr pMode)
{
#ifdef VIA_VEPD
	int rate;
	ScrnInfoPtr pScrn = output->scrn;
#endif
	IGNORE( output );
	IGNORE( pMode );

#ifndef VIA_VEPD
	/* TODO: return MODE_ERROR in case of unsupported mode */
	xf86DrvMsg(0, X_INFO, "Mode %i x %i valid\n", pMode->HDisplay, pMode->VDisplay );

	return MODE_OK;
#else
	if ((pScrn->bitsPerPixel == 24) || (pScrn->bitsPerPixel == 32))
	{
		if (((pMode->HDisplay == 1920) &&  (pMode->VDisplay == 1080)) ||
		    ((pMode->HDisplay == 1680) &&  (pMode->VDisplay == 1050)))
		{
			xf86DrvMsg(0, X_INFO, "In %d depth, mode %d x %d is not supported\n ",
				pScrn->bitsPerPixel, pMode->HDisplay, pMode->VDisplay);
			return MODE_BAD;
		}
	}

	if (IsValidMode(pMode->HDisplay, pMode->VDisplay, pMode->Clock) < 0)
	{
		xf86DrvMsg(0, X_INFO, "%d bpp, Mode %i x %i  pixclk: %i not valid\n", pScrn->bitsPerPixel,
			pMode->HDisplay, pMode->VDisplay, pMode->Clock);
		return MODE_BAD;
	}

	rate = (int)((pMode->Clock * 1000) / ( pMode->HTotal * pMode->VTotal));
	if (((rate >= 59) && (rate <= 61)) || ((rate >= 74) && (rate <= 76)))
		return MODE_OK;

	return MODE_BAD;
#endif
}

static Bool fbdev_lcd_output_mode_fixup(xf86OutputPtr output, DisplayModePtr mode, DisplayModePtr adjusted_mode)
{
	IGNORE( output );
	IGNORE( mode );
	IGNORE( adjusted_mode );

	return TRUE;
}

static void fbdev_lcd_output_prepare(xf86OutputPtr output)
{
	IGNORE( output );
}

static void fbdev_lcd_output_commit(xf86OutputPtr output)
{
	output->funcs->dpms(output, DPMSModeOn);
}

static void fbdev_lcd_output_mode_set(xf86OutputPtr output, DisplayModePtr mode, DisplayModePtr adjusted_mode)
{
	MaliPtr fPtr = MALIPTR(output->scrn);
#ifdef VIA_VEPD
	ScrnInfoPtr pScrn = output->scrn;
	int fps = (int)((mode->Clock * 1000) / ( mode->HTotal * mode->VTotal));
#endif

	IGNORE( output );
	IGNORE( mode );
	IGNORE( adjusted_mode );

	if ( ioctl( fPtr->fb_lcd_fd, FBIOGET_VSCREENINFO, &fPtr->fb_lcd_var ) < 0 )
	{
		xf86DrvMsg(0, X_INFO, "Unable to get VSCREENINFO\n");
	}

	fPtr->fb_lcd_var.xres = mode->HDisplay;
	fPtr->fb_lcd_var.yres = mode->VDisplay;
	fPtr->fb_lcd_var.xres_virtual = mode->HDisplay;
	fPtr->fb_lcd_var.yres_virtual = mode->VDisplay*2;
#ifndef VIA_VEPD
	xf86DrvMsg(0, X_INFO, "Changing mode to %i %i %i %i\n", fPtr->fb_lcd_var.xres, fPtr->fb_lcd_var.yres, fPtr->fb_lcd_var.xres_virtual, fPtr->fb_lcd_var.yres_virtual);
#else
	//fPtr->fb_lcd_var.pixclock = KHZ2PICOS(mode->Clock);
	if (pScrn->bitsPerPixel < 16)
		pScrn->bitsPerPixel = 16;
	fPtr->fb_lcd_var.bits_per_pixel = pScrn->bitsPerPixel;

	fPtr->fb_lcd_var.pixclock = mode->Clock;
	fPtr->fb_lcd_var.hsync_len    =  mode->CrtcHSyncEnd   - mode->CrtcHSyncStart;
	fPtr->fb_lcd_var.left_margin  =  mode->CrtcHBlankEnd  - mode->CrtcHSyncEnd;
	fPtr->fb_lcd_var.right_margin =  mode->CrtcHSyncStart - mode->CrtcHBlankStart;
	fPtr->fb_lcd_var.vsync_len    =  mode->CrtcVSyncEnd   - mode->CrtcVSyncStart;
	fPtr->fb_lcd_var.upper_margin =  mode->CrtcVBlankEnd  - mode->CrtcVSyncEnd;
	fPtr->fb_lcd_var.lower_margin =  mode->CrtcVSyncStart - mode->CrtcVBlankStart;
	if (fPtr->fb_lcd_var.hsync_len && fPtr->fb_lcd_var.left_margin
		&& fPtr->fb_lcd_var.right_margin && fPtr->fb_lcd_var.vsync_len
		&& fPtr->fb_lcd_var.upper_margin && fPtr->fb_lcd_var.lower_margin)
			fPtr->fb_lcd_var.reserved[0] = 1;
	else
			fPtr->fb_lcd_var.reserved[0] = 0;

	xf86DrvMsg(0, X_INFO, "Setting timing for %dx%d@%d......\n", mode->HDisplay, mode->VDisplay, fps);
	xf86DrvMsg(0, X_INFO, "h_sync=%d, h_back_porch=%d, h_pixels=%d, h_front_porch=%d\n",
		fPtr->fb_lcd_var.hsync_len, fPtr->fb_lcd_var.left_margin,
		fPtr->fb_lcd_var.xres, fPtr->fb_lcd_var.right_margin);
	xf86DrvMsg(0, X_INFO, "v_sync=%d, v_back_proch=%d, v_pixels=%d, v_front_porch=%d \n",
		fPtr->fb_lcd_var.vsync_len, fPtr->fb_lcd_var.upper_margin,  fPtr->fb_lcd_var.yres,
		fPtr->fb_lcd_var.lower_margin);
	xf86DrvMsg(0, X_INFO, "Changing mode to %i %i %i %i\n", fPtr->fb_lcd_var.xres,
		fPtr->fb_lcd_var.yres, fPtr->fb_lcd_var.xres_virtual, fPtr->fb_lcd_var.yres_virtual);
#endif

	if ( ioctl( fPtr->fb_lcd_fd, FBIOPUT_VSCREENINFO, &fPtr->fb_lcd_var ) < 0 )
	{
		xf86DrvMsg(0, X_INFO, "Unable to set mode!\n");
	}

}

static xf86OutputStatus fbdev_lcd_output_detect(xf86OutputPtr output)
{
	IGNORE( output );

	return XF86OutputStatusConnected;
}

DisplayModePtr fbdev_make_mode( int xres, int yres, DisplayModePtr prev )
{
	DisplayModePtr mode_ptr;
	unsigned int hactive_s = xres;
	unsigned int vactive_s = yres;

	mode_ptr = xnfcalloc(1, sizeof(DisplayModeRec));

	mode_ptr->HDisplay = hactive_s;
	mode_ptr->HSyncStart = hactive_s + 20;
	mode_ptr->HSyncEnd = hactive_s + 40;
	mode_ptr->HTotal = hactive_s + 80;

	mode_ptr->VDisplay = vactive_s;
	mode_ptr->VSyncStart = vactive_s + 20;
	mode_ptr->VSyncEnd = vactive_s + 40;
	mode_ptr->VTotal = vactive_s + 80;

	mode_ptr->VRefresh = 60.0;

	mode_ptr->Clock = (int) (mode_ptr->VRefresh * mode_ptr->VTotal * mode_ptr->HTotal / 1000.0);

	mode_ptr->type = M_T_DRIVER;

	xf86SetModeDefaultName(mode_ptr);

	mode_ptr->next = NULL;
	mode_ptr->prev = prev;

	return mode_ptr;
}

#ifdef VIA_VEPD
#define USE_VPP_IOCTL
#endif

#ifndef USE_VPP_IOCTL
static DisplayModePtr fbdev_lcd_output_get_modes(xf86OutputPtr output)
{
	MaliPtr fPtr = MALIPTR(output->scrn);
	DisplayModePtr mode_ptr;
	ScrnInfoPtr pScrn = output->scrn;

	unsigned int hactive_s = fPtr->fb_lcd_var.xres;
	unsigned int vactive_s = fPtr->fb_lcd_var.yres;

	if ( pScrn->modes != NULL )
	{
		/* Use the modes supplied by the implementation if available */
		DisplayModePtr mode, first = mode = pScrn->modes;
		DisplayModePtr modeptr = NULL, modeptr_prev = NULL, modeptr_first = NULL;
		do {
			int xres = mode->HDisplay;
			int yres = mode->VDisplay;

			xf86DrvMsg(0, X_INFO, "Adding mode: %i x %i\n", xres, yres);

			if ( modeptr_first == NULL )
			{
				modeptr_first = fbdev_make_mode( xres, yres, NULL );
				modeptr = modeptr_first;
			}
			else
			{
				modeptr->next = fbdev_make_mode( xres, yres, modeptr_prev);
				modeptr = modeptr->next;
			}
			modeptr_prev = modeptr;

			mode = mode->next;
		} while (mode != NULL && mode != first);

		return modeptr_first;
	}

	mode_ptr = xnfcalloc(1, sizeof(DisplayModeRec));

	mode_ptr->HDisplay = hactive_s;
	mode_ptr->HSyncStart = hactive_s + 20;
	mode_ptr->HSyncEnd = hactive_s + 40;
	mode_ptr->HTotal = hactive_s + 80;

	mode_ptr->VDisplay = vactive_s;
	mode_ptr->VSyncStart = vactive_s + 20;
	mode_ptr->VSyncEnd = vactive_s + 40;
	mode_ptr->VTotal = vactive_s + 80;

	mode_ptr->VRefresh = 60.0;

	mode_ptr->Clock = (int) (mode_ptr->VRefresh * mode_ptr->VTotal * mode_ptr->HTotal / 1000.0);

	mode_ptr->type = M_T_DRIVER;

	xf86SetModeDefaultName(mode_ptr);

	mode_ptr->next = NULL;
	mode_ptr->prev = NULL;

	return mode_ptr;
}
#else /* USE_VPP_IOCTL */
typedef enum {
	VPP_VOUT_SDA = 0,
	VPP_VOUT_SDD = 1,
	VPP_VOUT_LCD = 2,
	VPP_VOUT_DVI = 3,
	VPP_VOUT_HDMI = 4,
	VPP_VOUT_DVO2HDMI = 5,
	VPP_VOUT_LVDS = 6,
	VPP_VOUT_VGA = 7,
	VPP_VOUT_MAX
} vpp_vout_t;

typedef struct {
	vpp_vout_t mode;
	int size;
	unsigned char *buf;
} vpp_vout_edid_t;

#define VPPIO_MAGIC             'f'
#define VPPIO_VOUT_BASE         0x10
#define VPPIO_VOGET_EDID        _IOR(VPPIO_MAGIC, VPPIO_VOUT_BASE + 8, vpp_vout_edid_t)
#define EDID_BLOCK_MAX          4
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#include <xorg/xf86DDC.h>

#ifdef FIX_VGA
/*
 * If EVB board cannot detect EDID from VGA, fallback to builtin EDID.
 */

/* Unknown VGA monitor
 *
 * 640x480  @60Hz
 * 800x600  @60Hz
 * 1024x768 @60Hz
 */
static unsigned char builtin_vga_edid[] = {
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x36, 0x7f, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x0c, 0x01, 0x03, 0x00, 0x28, 0x1e, 0x00, 0xea, 0x78, 0xa0, 0x56, 0x48, 0x9a, 0x26, 0x12,
	0x48, 0x4c, 0xff, 0x21, 0x08, 0x00, 0x61, 0x40, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x6f, 0x12, 0x00, 0x00, 0x40, 0x00, 0x00, 0x30, 0x00, 0x00,
	0x00, 0x00, 0x28, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe3,
};
#endif

static DisplayModePtr fbdev_lcd_output_get_modes(xf86OutputPtr output)
{
	ScrnInfoPtr pScrn = output->scrn;
	DisplayModePtr modes = NULL;
	xf86MonPtr edid_mon;
	int fd, ret;
	size_t len;
	int volist[] = { VPP_VOUT_HDMI, VPP_VOUT_DVI, VPP_VOUT_VGA, VPP_VOUT_LCD, };
	int i;

	vpp_vout_edid_t lcd_edid;
	memset(&lcd_edid, 0, sizeof(vpp_vout_edid_t));
	len = 128 * EDID_BLOCK_MAX;
	/* lcd_edid.buf = xcalloc(1, len); */
	lcd_edid.buf = calloc(1, len);

	if (!lcd_edid.buf)
	{
		xf86DrvMsg(pScrn->scrnIndex, X_INFO, "malloc buf for edid failed\n");
		return modes;
	}

	fd = open("/dev/fb1", O_RDWR);
	if (fd < 0)
	{
		xf86DrvMsg(pScrn->scrnIndex, X_INFO, "Can not open fb1\n");
		/* xfree(lcd_edid.buf); */
		free(lcd_edid.buf);
		return modes;
	}

#ifdef USE_EDID_CACHE
	if ((edid_cache[0] == 0x00) && (edid_cache[1] == 0xff))
	{
		edid_mon = xf86InterpretEDID(pScrn->scrnIndex, edid_cache);
		if (edid_mon)
		{
			xf86OutputSetEDID(output, edid_mon);
			modes = xf86OutputGetEDIDModes(output);
			if (modes)
			{
				xf86DrvMsg(pScrn->scrnIndex, X_INFO, "Read cached EDID\n");
			}
		}
	}
#endif

	if (!modes)
	{
		for (i = 0; i < 4; i++)
		{
			memset(lcd_edid.buf, 0, len);
			lcd_edid.mode =  volist[i];
			lcd_edid.size =  len;

			ret = ioctl(fd, VPPIO_VOGET_EDID, &lcd_edid);
			if (ret < 0 || lcd_edid.size == 0)
				continue;

			if ((lcd_edid.buf[0] == 0x00) && (lcd_edid.buf[1] == 0xff))
			{
				edid_mon = xf86InterpretEDID(pScrn->scrnIndex, lcd_edid.buf);
				if (edid_mon)
				{
					xf86OutputSetEDID(output, edid_mon);
					modes = xf86OutputGetEDIDModes(output);
					if (modes)
					{
						xf86DrvMsg(pScrn->scrnIndex, X_INFO, "Read EDID from VO #%d\n",i);
#ifdef USE_EDID_CACHE
						memcpy(edid_cache, lcd_edid.buf, lcd_edid.size);
#endif
						break;
					}
				}
			}
		}
	}

#ifdef FIX_VGA
	if (!modes)
	{
		edid_mon = xf86InterpretEDID(pScrn->scrnIndex, builtin_vga_edid);
		if (edid_mon)
		{
			xf86OutputSetEDID(output, edid_mon);
			modes = xf86OutputGetEDIDModes(output);
			if (modes)
			{
				xf86DrvMsg(pScrn->scrnIndex, X_INFO, "Read EDID from builtin VGA\n");
#ifdef USE_EDID_CACHE
				memcpy(edid_cache, builtin_vga_edid, sizeof(builtin_vga_edid));
#endif
			}
		}
	}
#endif

	/* xfree(lcd_edid.buf); */
	free(lcd_edid.buf);
	close(fd);

	return modes;
}
#endif /* USE_VPP_IOCTL */

#ifdef RANDR_GET_CRTC_INTERFACE
static xf86CrtcPtr fbdev_lcd_output_get_crtc(xf86OutputPtr output)
{
	return output->crtc;
}
#endif

static void fbdev_lcd_output_destroy(xf86OutputPtr output)
{
	IGNORE( output );
}

static const xf86OutputFuncsRec fbdev_lcd_output_funcs =
{
	.create_resources = NULL,
	.dpms = fbdev_lcd_output_dpms,
	.save = fbdev_lcd_output_save,
	.restore = fbdev_lcd_output_restore,
	.mode_valid = fbdev_lcd_output_mode_valid,
	.mode_fixup = fbdev_lcd_output_mode_fixup,
	.prepare = fbdev_lcd_output_prepare,
	.commit = fbdev_lcd_output_commit,
	.mode_set = fbdev_lcd_output_mode_set,
	.detect = fbdev_lcd_output_detect,
	.get_modes = fbdev_lcd_output_get_modes,
#ifdef RANDR_12_INTERFACE
	.set_property = NULL,
#endif
#ifdef RANDR_13_INTERFACE
	.get_property = NULL,
#endif
#ifdef RANDR_GET_CRTC_INTERFACE
	.get_crtc = fbdev_lcd_output_get_crtc,
#endif
	.destroy = fbdev_lcd_output_destroy,
};


Bool FBDEV_lcd_init(ScrnInfoPtr pScrn)
{
	xf86CrtcPtr crtc;
	xf86OutputPtr output;

#ifdef USE_EDID_CHACHE
	memset(edid_cache, 0, EDID_CACHE_SIZE);
#endif

	crtc = xf86CrtcCreate(pScrn, &fbdev_lcd_crtc_funcs);

	if(crtc == NULL) return FALSE;

#ifndef VIA_VEPD
	output = xf86OutputCreate(pScrn, &fbdev_lcd_output_funcs, "LCD");
#else
	output = xf86OutputCreate(pScrn, &fbdev_lcd_output_funcs, OUTPUT_NAME);
#endif

	if(output == NULL) return FALSE;

	output->possible_crtcs = (1 << 0);

	return TRUE;
}
