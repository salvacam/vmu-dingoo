#include <stdlib.h>
#include <string.h>

//#include <pspkernel.h>
//#include <pspdebug.h>

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

//#ifdef __DINGOO
#include <stdlib.h>
#include <stdio.h> 

#include <sys/mman.h>

#include <fcntl.h>


/*
// Define this to the CPU frequency 

#define CPU_FREQ 336000000    // CPU clock: 336 MHz

#define CFG_EXTAL 12000000    // EXT clock: 12 Mhz 



// SDRAM Timings, unit: ns
#define SDRAM_TRAS		45	// RAS# Active Time 
#define SDRAM_RCD		20	// RAS# to CAS# Delay 
#define SDRAM_TPC		20	// RAS# Precharge Time 
#define SDRAM_TRWL		7	// Write Latency Time 
#define SDRAM_TREF	        15625	// Refresh period: 4096 refresh cycles/64ms 

//#define SDRAM_TREF      7812  // Refresh period: 8192 refresh cycles/64ms



#include "jz4740.h"



static unsigned long jz_dev=0;

static volatile unsigned long  *jz_cpmregl, *jz_emcregl;

volatile unsigned short *jz_emcregs; 



inline int sdram_convert(unsigned int pllin,unsigned int *sdram_freq)
{
	register unsigned int ns, tmp;
 
	ns = 1000000000 / pllin;
	// Set refresh registers 
	tmp = SDRAM_TREF/ns;
	tmp = tmp/64 + 1;
	if (tmp > 0xff) tmp = 0xff;
        *sdram_freq = tmp; 

	return 0;

}
 

void pll_init(unsigned int clock)

{

	register unsigned int cfcr, plcr1;
	unsigned int sdramclock = 0;
	int n2FR[33] = {
		0, 0, 1, 2, 3, 0, 4, 0, 5, 0, 0, 0, 6, 0, 0, 0,
		7, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0,
		9
	};
	//int div[5] = {1, 4, 4, 4, 4}; // divisors of I:S:P:L:M 
  	int div[5] = {1, 3, 3, 3, 3}; // divisors of I:S:P:L:M 
	int nf, pllout2;

	cfcr = CPM_CPCCR_CLKOEN |
		(n2FR[div[0]] << CPM_CPCCR_CDIV_BIT) | 
		(n2FR[div[1]] << CPM_CPCCR_HDIV_BIT) | 
		(n2FR[div[2]] << CPM_CPCCR_PDIV_BIT) |
		(n2FR[div[3]] << CPM_CPCCR_MDIV_BIT) |
		(n2FR[div[4]] << CPM_CPCCR_LDIV_BIT);

	pllout2 = (cfcr & CPM_CPCCR_PCS) ? clock : (clock / 2);

	// Init UHC clock 
//	REG_CPM_UHCCDR = pllout2 / 48000000 - 1;

    	jz_cpmregl[0x6C>>2] = pllout2 / 48000000 - 1;


	nf = clock * 2 / CFG_EXTAL;
	plcr1 = ((nf - 2) << CPM_CPPCR_PLLM_BIT) | // FD 
		(0 << CPM_CPPCR_PLLN_BIT) |	// RD=0, NR=2 
		(0 << CPM_CPPCR_PLLOD_BIT) |    // OD=0, NO=1 
		(0x20 << CPM_CPPCR_PLLST_BIT) | // PLL stable time 
		CPM_CPPCR_PLLEN;                // enable PLL           

	// init PLL 
//	REG_CPM_CPCCR = cfcr;
//	REG_CPM_CPPCR = plcr1;

      	jz_cpmregl[0] = cfcr;

    	jz_cpmregl[0x10>>2] = plcr1;

	
  	sdram_convert(clock,&sdramclock);
  	if(sdramclock > 0)
  	{
//	REG_EMC_RTCOR = sdramclock;
//	REG_EMC_RTCNT = sdramclock;	  

      	jz_emcregs[0x8C>>1] = sdramclock;

    	jz_emcregs[0x88>>1] = sdramclock;	


  	}else
  	{
  	printf("sdram init fail!\n");
  	while(1);
  	} 

	

}
//#endif

*/
extern int psp_vms(char *argv[]);
extern int selector();

SDL_Surface *screen;
char nombreRom[256];
int salir;

int main(int argc, char *argv[])
{

/*#ifdef __DINGOO
	int clockspeed=250;

	if(!jz_dev)  jz_dev = open("/dev/mem", O_RDWR);  

	jz_cpmregl=(unsigned long  *)mmap(0, 0x80, PROT_READ|PROT_WRITE, MAP_SHARED, jz_dev, 0x10000000);
	jz_emcregl=(unsigned long  *)mmap(0, 0x90, PROT_READ|PROT_WRITE, MAP_SHARED, jz_dev, 0x13010000);
	jz_emcregs=(unsigned short *)jz_emcregl;
	pll_init(clockspeed*1000000);
	munmap((void *)jz_cpmregl, 0x80); 
	munmap((void *)jz_emcregl, 0x90); 	
	close(jz_dev);
#endif*/

	SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);
	
	screen = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE|SDL_ANYFORMAT);
	SDL_ShowCursor(SDL_DISABLE);

	SDL_Event event;
	while (1) {
		while(SDL_PollEvent(&event));
		argv[1] = (char*)selector();
		if (argv[1] == NULL) {
				SDL_Quit();
				exit(0);
		}
		dingoo_vms(argv);
	}

	return 0;	
}

void putpixel(int x, int y, int pixel)
{
	if(x > 479 || y > 271) return;
    int bpp = screen->format->BytesPerPixel;
    // Here p is the address to the pixel we want to set 
    Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}


