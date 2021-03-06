#include <psptypes.h>
#include <pspctrl.h>
#include "device.h"

#define	SCALE		2	/* default scaling factor for acceleration */
#define	THRESH		5	/* default threshhold for acceleration */

#define UPPER_THRESHOLD  0xcf
#define LOWER_THRESHOLD  0x2f

#define ANALOG_CENER 0x7F
#define ANALOG_BASE 0x10000

static int  KBD_Open(KBDDEVICE *pkd);
static void KBD_Close(void);
static void KBD_GetModifierInfo(MWKEYMOD *modifiers, MWKEYMOD *curmodifiers);
static int  KBD_Read(MWKEY *kbuf, MWKEYMOD *modifiers, MWSCANCODE *scancode);
static int  KBD_Poll(void);

static int  	MOU_Open(MOUSEDEVICE *pmd);
static void 	MOU_Close(void);
static int  	MOU_GetButtonInfo(void);
static void	MOU_GetDefaultAccel(int *pscale,int *pthresh);
static int  	MOU_Read(MWCOORD *dx, MWCOORD *dy, MWCOORD *dz,int *bp);
static int	MOU_Poll(void);

KBDDEVICE kbddev = {
	KBD_Open,
	KBD_Close,
	KBD_GetModifierInfo,
	KBD_Read,
	KBD_Poll
};

MOUSEDEVICE mousedev = {
	MOU_Open,
	MOU_Close,
	MOU_GetButtonInfo,
	MOU_GetDefaultAccel,
	MOU_Read,
	MOU_Poll
};

u32 new_pad;
u32 old_pad;
u32 now_pad;
/* ctrl_data_t paddata; */
unsigned int button_timers[32];
int analog_x=0, analog_y=0;

/* void HomeScreen(); */

static void readpad(void)
{
/*         static int n=0; */
        ctrl_data_t paddata;

        sceCtrlSetSamplingCycle(0);
        sceCtrlSetSamplingMode(1);
        sceCtrlReadBufferPositive(&paddata, 1);
        // kmg
        // Analog pad state
        analog_x = paddata.analog_x;
        analog_y = paddata.analog_y;

        now_pad = paddata.buttons;
}

static int button_hit(void)
{
  int i;

  readpad();

  new_pad = 0;

  for(i=0;i<32;i++){
    int k=1<<i;
    if(k==CTRL_CROSS || k==CTRL_SQUARE)
      continue;

    if(now_pad&k){
      if(old_pad&now_pad&k){
	if(button_timers[i]>=25){
	  new_pad|=k;
	  button_timers[i]=20;
	}else
	  button_timers[i]++;
      }else{
	new_pad|=k;
	button_timers[i]=0;
      }
    }else{
      new_pad&=~k;
      button_timers[i]=0;
    }
    
  }
  
  old_pad = now_pad;

  return new_pad;
}

static int
KBD_Open(KBDDEVICE *pkd)
{
	return 1;

}

/*
 * Close the keyboard.
 */
static void
KBD_Close(void)
{
}

static void
KBD_GetModifierInfo(MWKEYMOD * modifiers, MWKEYMOD * curmodifiers)
{
	if (modifiers)
		*modifiers = 0;	/* no modifiers available */
	if (curmodifiers)
		*curmodifiers = 0;
}

static int
KBD_Read(MWKEY * kbuf, MWKEYMOD * modifiers, MWSCANCODE * scancode)
{
  if(!button_hit())
    return 0;

  if(new_pad & CTRL_DOWN) { 
    *kbuf = MWKEY_DOWN; 
    return 1;
  }
  if(new_pad & CTRL_UP) {
    *kbuf = MWKEY_UP; 
    return 1;
  }
  if(new_pad & CTRL_RIGHT) { 
    *kbuf = MWKEY_RIGHT; 
    return 1;
  }
  if(new_pad & CTRL_LEFT) {
    *kbuf = MWKEY_LEFT; 
    return 1;
  }
  if(new_pad & CTRL_CIRCLE) { 
    *kbuf = MWKEY_ENTER; 
    return 1;
  }
  if(new_pad & CTRL_TRIANGLE) {
    *kbuf = MWKEY_CANCEL; 
    return 1;
  }
  if(new_pad & CTRL_START) {
    *kbuf = MWKEY_MENU; 
    return 1;
  }
  if(new_pad & CTRL_SELECT) {
    *kbuf = MWKEY_TAB; 
    return 1;
  }

  return 0;
}

static int
KBD_Poll(void)
{
  if(button_hit())
    return 1;
  else
    return 0;
}

static int
MOU_Open(MOUSEDEVICE *pmd)
{
	return 1;

}

/*
 * Close the mouse.
 */
static void
MOU_Close(void)
{
}

/*
 * Get mouse buttons supported
 */
static int
MOU_GetButtonInfo(void)
{
	return MWBUTTON_L | MWBUTTON_R;
}

static int
analog_step(int n){
/*   return (n-ANALOG_CENER)/ANALOG_BASE; */
/*   int t=(n-ANALOG_CENER)/ANALOG_BASE; */
/*   return (t==0)?0: */
/*     (t>0)?(t*t+ANALOG_BASE/2)/ANALOG_BASE: */
/*     -(t*t+ANALOG_BASE/2)/ANALOG_BASE; */
  int t=n-ANALOG_CENER;
  return (t==0)?0:
    (t>0)?(t*t*t+ANALOG_BASE/2)/ANALOG_BASE:
    (t*t*t-ANALOG_BASE/2)/ANALOG_BASE;

}


/*
 * Get default mouse acceleration settings
 */
static void
MOU_GetDefaultAccel(int *pscale,int *pthresh)
{
	*pscale = SCALE;
	*pthresh = THRESH;
}

static int
MOU_Read(MWCOORD *dx, MWCOORD *dy, MWCOORD *dz, int *bp)
{
  readpad();

/*   *dx=(analog_x > UPPER_THRESHOLD)?(analog_x - UPPER_THRESHOLD)/3: */
/*     (analog_x < LOWER_THRESHOLD)?(analog_x - LOWER_THRESHOLD)/3:0; */
/*   *dy=(analog_y > UPPER_THRESHOLD)?(analog_y - UPPER_THRESHOLD)/3: */
/*     (analog_y < LOWER_THRESHOLD)?(analog_y - LOWER_THRESHOLD)/3:0; */
  *dx=analog_step(analog_x);
  *dy=analog_step(analog_y);
  *dz=0;

  *bp=0;
  if(now_pad&CTRL_CROSS)
    *bp|=MWBUTTON_L;
  if(now_pad&CTRL_SQUARE)
    *bp|=MWBUTTON_R;

  return 1;
}

static int
MOU_Poll(void)
{
	return 1;
}
