#include <psptypes.h>
#include <pspdisplay.h>
#include <pspge.h>
#include <assert.h>
#include "device.h"
#include "fb.h"
#include "genfont.h"
#include "genmem.h"

static PSD  fb_open(PSD psd);
static void fb_close(PSD psd);
static void fb_setportrait(PSD psd, int portraitmode);
/* static void fb_setpalette(PSD psd,int first, int count, MWPALENTRY *palette); */
static void gen_getscreeninfo(PSD psd,PMWSCREENINFO psi);

//constants
#if MWPIXEL_FORMAT == MWPF_TRUECOLOR555

#define		PIXELMODE	1
#define		LINESIZE	512
#define		FRAMESIZE	0x44000

#elif MWPIXEL_FORMAT == MWPF_TRUECOLOR8888

#define		PIXELMODE	3
#define		LINESIZE	512
#define		FRAMESIZE	0x88000

#endif

#define PSP_XRES 480
#define PSP_YRES 272

/* char *vramtop=(char *)0x04000000; */

static int status;		/* 0=never inited, 1=once inited, 2=inited. */

extern SUBDRIVER fbportrait_left, fbportrait_right, fbportrait_down;
static PSUBDRIVER pdrivers[4] = { /* portrait mode subdrivers*/
	NULL, &fbportrait_left, &fbportrait_right, &fbportrait_down
};

SCREENDEVICE	scrdev = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL,
	fb_open,
	fb_close,
	gen_getscreeninfo,
	NULL,                   /* fb_setpalette, */
	NULL,			/* DrawPixel subdriver*/
	NULL,			/* ReadPixel subdriver*/
	NULL,			/* DrawHorzLine subdriver*/
	NULL,			/* DrawVertLine subdriver*/
	NULL,			/* FillRect subdriver*/
	gen_fonts,
	NULL,			/* Blit subdriver*/
	NULL,			/* PreSelect*/
	NULL,			/* DrawArea subdriver*/
	NULL,			/* SetIOPermissions*/
	gen_allocatememgc,
	NULL,/* fb_mapmemgc, */
	gen_freememgc,
	NULL,			/* StretchBlit subdriver*/
	fb_setportrait		/* SetPortrait*/
};

static PSD
fb_open(PSD psd)
{
/*   extern SUBDRIVER fblinear16; */

/*   PSUBDRIVER subdriver=&fblinear16; */

  PSUBDRIVER subdriver=select_fb_subdriver(psd);

  assert(status < 2);

  psd->xres = psd->xvirtres = PSP_XRES;
  psd->yres = psd->yvirtres = PSP_YRES;
  psd->planes = 1;

#if MWPIXEL_FORMAT == MWPF_TRUECOLOR555
  psd->bpp = 16;
  psd->ncolors = 1<<15;
  psd->pixtype=MWPF_TRUECOLOR555;
#elif MWPIXEL_FORMAT == MWPF_TRUECOLOR8888
  psd->bpp = 32;
  psd->ncolors = 1<<31;
  psd->pixtype=MWPF_TRUECOLOR8888;
#else
  goto fail;
#endif
  
  psd->linelen = LINESIZE;
  psd->size = FRAMESIZE;
  psd->addr = (void *) (0x40000000 | sceGeEdramGetAddr());

  psd->portrait = MWPORTRAIT_NONE;

  psd->flags = PSF_SCREEN | PSF_HAVEBLIT;
  if (psd->bpp == 16)
    psd->flags |= PSF_HAVEOP_COPY;

  if(!set_subdriver(psd, subdriver, TRUE)) {
/*     EPRINTF("Driver initialize failed type %d visual %d bpp %d\n", */
/* 	    type, visual, psd->bpp); */
    goto fail;
  }

  pdrivers[0] = psd->orgsubdriver = subdriver;

  sceDisplaySetMode(0,PSP_XRES,PSP_YRES);
  sceDisplaySetFrameBuf(psd->addr,LINESIZE,PIXELMODE,1);

  status = 2;
  return psd;	/* success*/  

 fail:
  return NULL;
}

/* close framebuffer*/
static void
fb_close(PSD psd)
{
  /* if not opened, return*/
  if(status != 2)
    return;
  status = 1;

  sceDisplaySetFrameBuf(0,0,0,1);
}


static void
fb_setportrait(PSD psd, int portraitmode)
{
  psd->portrait = portraitmode;

  /* swap x and y in left or right portrait modes*/
  if (portraitmode & (MWPORTRAIT_LEFT|MWPORTRAIT_RIGHT)) {
    /* swap x, y*/
    psd->xvirtres = psd->yres;
    psd->yvirtres = psd->xres;
  } else {
    /* normal x, y*/
    psd->xvirtres = psd->xres;
    psd->yvirtres = psd->yres;
  }
  /* assign portrait subdriver which calls original subdriver*/
  if (portraitmode == MWPORTRAIT_DOWN)
    portraitmode = 3;	/* change bitpos to index*/
  set_subdriver(psd, pdrivers[portraitmode], FALSE);
}

static void
gen_getscreeninfo(PSD psd,PMWSCREENINFO psi)
{
	psi->rows = psd->yvirtres;
	psi->cols = psd->xvirtres;
	psi->planes = psd->planes;
	psi->bpp = psd->bpp;
	psi->ncolors = psd->ncolors;
	psi->fonts = NUMBER_FONTS;
	psi->portrait = psd->portrait;
	psi->fbdriver = TRUE;	/* running fb driver, can direct map*/

	psi->pixtype = psd->pixtype;
	switch (psd->pixtype) {
	case MWPF_TRUECOLOR8888:
	case MWPF_TRUECOLOR0888:
	case MWPF_TRUECOLOR888:
		psi->rmask 	= 0xff0000;
		psi->gmask 	= 0x00ff00;
		psi->bmask	= 0x0000ff;
		break;
	case MWPF_TRUECOLOR565:
		psi->rmask 	= 0xf800;
		psi->gmask 	= 0x07e0;
		psi->bmask	= 0x001f;
		break;
	case MWPF_TRUECOLOR555:
		psi->rmask 	= 0x7c00;
		psi->gmask 	= 0x03e0;
		psi->bmask	= 0x001f;
		break;
	case MWPF_TRUECOLOR332:
		psi->rmask 	= 0xe0;
		psi->gmask 	= 0x1c;
		psi->bmask	= 0x03;
		break;
	case MWPF_PALETTE:
	default:
		psi->rmask 	= 0xff;
		psi->gmask 	= 0xff;
		psi->bmask	= 0xff;
		break;
	}

	psi->xdpcm=39;
	psi->ydpcm=39;	
}

PSUBDRIVER 
select_fb_subdriver(PSD psd)
{
/* 	PSUBDRIVER  driver = NULL; */
	extern SUBDRIVER fblinear16;
	extern SUBDRIVER fblinear32alpha;

#if MWPIXEL_FORMAT == MWPF_TRUECOLOR555
	return &fblinear16;
#elif MWPIXEL_FORMAT == MWPF_TRUECOLOR8888
	return &fblinear32alpha;
#else
	return NULL;
#endif
}
