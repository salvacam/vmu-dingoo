#include <stdarg.h>
#include <string.h>

#include <SDL/SDL.h>
#include "prototypes.h"

#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <stdio.h>
#include <fcntl.h>

#define VMU_IMG "vmu-dingoo.bmp"
#define VMU_GAME "SCROLL.VMS"

#define WIDTH 48*2
#define HEIGHT 40*2

#define XOFFSET 36
#define YOFFSET 85

extern void scroller();
extern void putpixel();

extern SDL_Surface *screen;
extern int salir;

SDL_Joystick *stick;
unsigned short fg, bg;
int sound_freq = -1;

void error_msg(char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  //pspDebugScreenPrintf(fmt, va);
  //pspDebugScreenPrintf("\n");
  printf(fmt, va);
  printf("\n");
  va_end(va);
}

int last_index_of(char *str, char c) {
	int i = strlen(str) - 1;
	while(i > 0 && str[i] != c) i--;
	return i;
}

static void sound_callback(void *userdata, Uint8 *stream, int len)
{
  if(sound_freq <= 0)
    memset(stream, 0, len);
  else {
    int i;
    static char v = 0x7f;
    static int f = 0;
    for(i=0; i<len; i++) {
      f += sound_freq;
      while(f >= 32768) {
	v ^= 0xff;
	f -= 32768;
      }
      *stream++ = v;
    }
  }
}

void init_sound()
{

  static SDL_AudioSpec desired = {
	#ifdef MIYOO
    44100,
	#else
    65536,
  #endif
    AUDIO_S16,
    1,
    0, 256, 0, 0,
    sound_callback,
    NULL
  };

  if (!SDL_WasInit(SDL_INIT_AUDIO)) {
	  if(SDL_Init(SDL_INIT_AUDIO)<0)
	    fprintf(stderr, "Failed to SDL_Init, sound disabled.\n");
	  else if(SDL_OpenAudio(&desired, NULL)<0)
	    fprintf(stderr, "Failed to SDL_OpenAudio, sound disabled.\n");
	  else
	    SDL_PauseAudio(0);
  }

	/* volume */
 /* int volume = 30;
  long mixer;
  int realVol=(volume<<8)+volume ; // 0 < volume < 255 
  mixer = open("/dev/mixer", O_RDWR);
  if (mixer != -1)
  {
		     ioctl(mixer, SOUND_MIXER_WRITE_VOLUME , &realVol);
			    close(mixer);
  }
*/

}

int dingoo_vms(void *data) {
	SDL_Surface *vmu;
	char **argv;

	argv = (char **)data;

	vmu = SDL_LoadBMP(VMU_IMG);	
	SDL_BlitSurface(vmu, NULL, screen, NULL);
	SDL_Flip(screen);

	SDL_Rect display_rect = { XOFFSET, YOFFSET, WIDTH, HEIGHT };
	SDL_FillRect(screen, &display_rect, bg);

	SDL_Flip(screen);
	
	fg = SDL_MapRGB(screen->format, 0x47, 0x13, 0x78);
	bg = SDL_MapRGB(screen->format, 0x57, 0x89, 0x75);

	init_sound();

	do_vmsgame(argv[1], NULL);
	
	return 0;
}

void vmputpixel(int x, int y, int p)
{
	int screen_x, screen_y;
	screen_x = (x<<1) + XOFFSET;
	screen_y = (y<<1) + YOFFSET;
	
	unsigned short color = p & 0x1 ? fg : bg;
	
	putpixel(screen_x, screen_y, color);	
	putpixel(screen_x+1, screen_y, color);	
	putpixel(screen_x, screen_y+1, color);	
	putpixel(screen_x+1, screen_y+1, color);	
}

void redrawlcd()
{
	SDL_Flip(screen);
}

/*
 0 - triangle
 1 - circle
 2 - cross
 3 - square
 4 - left trigger
 5 - right trigger
 6 - down
 7 - left
 8 - up
 9 - right
10 - select
11 - start
12 - home
13 - hold
*/

#define A		0
#define B		1
#define SQUARE	2
#define T_LEFT	3
#define T_RIGHT	4
#define DOWN	5
#define LEFT	6
#define UP	    7
#define RIGHT	8
#define SELECT	9
#define START	10


int buttons = 0;

void checkevents()
{
	//SDL_JoystickUpdate();
	
//	pspDebugScreenSetXY(0,0);
//	pspDebugScreenPrintf("%s: %d: up\n",SDL_JoystickName(0), SDL_JoystickGetButton(stick, UP));
//	pspDebugScreenPrintf("Axis: [%d]", SDL_JoystickGetAxis(stick, 0));

/*	if(SDL_JoystickGetButton(stick, UP) && !(buttons & (1 << UP)))  { keypress(0); buttons |= (1 << UP); }
	if(SDL_JoystickGetButton(stick, DOWN) && !(buttons & (1 << DOWN))) { keypress(1); buttons |= (1 << DOWN); }
	if(SDL_JoystickGetButton(stick, LEFT) && !(buttons & (1 << LEFT))) { keypress(2); buttons |= (1 << LEFT); }
	if(SDL_JoystickGetButton(stick, RIGHT) && !(buttons & (1 << RIGHT))) { keypress(3); buttons |= (1 << RIGHT); }
	if(SDL_JoystickGetButton(stick, CROSS) && !(buttons & (1 << CROSS))) { keypress(4); buttons |= (1 << CROSS); }
	if(SDL_JoystickGetButton(stick, CIRCLE) && !(buttons & (1 << CIRCLE))) { keypress(5); buttons |= (1 << CIRCLE); }
	if(SDL_JoystickGetButton(stick, SELECT) && !(buttons & (1 << SELECT))) { keypress(6); buttons |= (1 << SELECT); }
	if(SDL_JoystickGetButton(stick, START) && !(buttons & (1 << START))) { keypress(7); buttons |= (1 << START); }

	if(!SDL_JoystickGetButton(stick, UP) && (buttons & (1 << UP)))  { keyrelease(0); buttons ^= (1 << UP); }
	if(!SDL_JoystickGetButton(stick, DOWN) && (buttons & (1 << DOWN))) { keyrelease(1); buttons ^= (1 << DOWN); }
	if(!SDL_JoystickGetButton(stick, LEFT) && (buttons & (1 << LEFT))) { keyrelease(2); buttons ^= (1 << LEFT); }
	if(!SDL_JoystickGetButton(stick, RIGHT) && (buttons & (1 << RIGHT))) { keyrelease(3); buttons ^= (1 << RIGHT); }
	if(!SDL_JoystickGetButton(stick, CROSS) && (buttons & (1 << CROSS))) { keyrelease(4); buttons ^= (1 << CROSS); }
	if(!SDL_JoystickGetButton(stick, CIRCLE) && (buttons & (1 << CIRCLE))) { keyrelease(5); buttons ^= (1 << CIRCLE); }
	if(!SDL_JoystickGetButton(stick, SELECT) && (buttons & (1 << SELECT))) { keyrelease(6); buttons ^= (1 << SELECT); }
	if(!SDL_JoystickGetButton(stick, START) && (buttons & (1 << START))) { keyrelease(7); buttons ^= (1 << START); }

*/
	//scroller();
	SDL_PumpEvents();
	Uint8 *keystate = SDL_GetKeyState(NULL);
	
	if(keystate[SDLK_UP] && !(buttons & (1 << UP)))  { keypress(0); buttons |= (1 << UP); }
    if(keystate[SDLK_DOWN] && !(buttons & (1 << DOWN))) { keypress(1); buttons |= (1 << DOWN); }
    if(keystate[SDLK_LEFT] && !(buttons & (1 << LEFT))) { keypress(2); buttons |= (1 << LEFT); }
    if(keystate[SDLK_RIGHT] && !(buttons & (1 << RIGHT))) { keypress(3); buttons |= (1 << RIGHT); }
    if(keystate[SDLK_LCTRL] && !(buttons & (1 << B))) { keypress(4); buttons |= (1 << B); }
    if(keystate[SDLK_LALT] && !(buttons & (1 << A))) { keypress(5); buttons |= (1 << A); }
    if(keystate[SDLK_ESCAPE] && !(buttons & (1 << SELECT))) { keypress(6); buttons |= (1 << SELECT); }
    if(keystate[SDLK_RETURN] && !(buttons & (1 << START))) { keypress(7); buttons |= (1 << START); }

	if(keystate[SDLK_TAB]) {salir = 1;}

    if(!keystate[SDLK_UP] && (buttons & (1 << UP)))  { keyrelease(0); buttons ^= (1 << UP); }
    if(!keystate[SDLK_DOWN] && (buttons & (1 << DOWN))) { keyrelease(1); buttons ^= (1 << DOWN); }
    if(!keystate[SDLK_LEFT] && (buttons & (1 << LEFT))) { keyrelease(2); buttons ^= (1 << LEFT); }
    if(!keystate[SDLK_RIGHT] && (buttons & (1 << RIGHT))) { keyrelease(3); buttons ^= (1 << RIGHT); }
    if(!keystate[SDLK_LCTRL] && (buttons & (1 << B))) { keyrelease(4); buttons ^= (1 << B); }
    if(!keystate[SDLK_LALT] && (buttons & (1 << A))) { keyrelease(5); buttons ^= (1 << A); }
    if(!keystate[SDLK_ESCAPE] && (buttons & (1 << SELECT))) { keyrelease(6); buttons ^= (1 << SELECT); }
    if(!keystate[SDLK_RETURN] && (buttons & (1 << START))) { keyrelease(7); buttons ^= (1 << START); }
	

}

void waitforevents(struct timeval *t)
{
	SDL_WaitEvent(NULL);
}

void sound(int freq)
{
	sound_freq = freq;
}

