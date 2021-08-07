#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "selector.h"

extern SDL_Surface* screen;
extern char nombreRom[];
int start = 1;

char *uppercase (char * cadena) //convierte minusculas en mayusculas
{
  int i=0;
  char * cadenaout;
  cadenaout = (char *) malloc ((strlen(cadena)+1) * sizeof(char));
  while ( cadena[i] )
    {
      cadenaout[i] = toupper(cadena[i]);
      i++;
    }  
  cadenaout[i] = 0;
  return cadenaout;
}


void drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height)
{
  SDL_Rect srcRect;
  srcRect.x = srcX;
  srcRect.y = srcY;
  srcRect.w = width;
  srcRect.h = height;
  
  SDL_Rect dstRect;
  dstRect.x = dstX;
  dstRect.y = dstY;
  dstRect.w = width;
  dstRect.h = height;
  
  SDL_BlitSurface(imageSurface, &srcRect, screenSurface, &dstRect);
}

void drawText (SDL_Surface *image, SDL_Surface *screen, int x, int y, char *texto, int center) {
  int i=0;
  int lenght;
  if (center) {
    lenght = strlen(texto);
    if (lenght < 34) {
      x = (320-lenght*8)/2;
    }
    else x = 24;
  }
  while (texto[i] != 0 && i<34) { // i<34 es pa qe no se salga de la pantalla
    if (texto[i]>43 && texto[i]<60) { //si son numeros u otros caracteres
      drawSprite (image, screen, (texto[i]-44)*8+32, 0, x, y, 8, 8);
    }
    else if (texto[i]>64 && texto[i]<91) { //si es una letra mayuscula
      drawSprite (image, screen, (texto[i]-65)*8+160, 0, x, y, 8, 8);
    }
    else {
      switch (texto[i]) {
      case 32: // espacio
	break;
      case 33: // !
	drawSprite (image, screen, 376, 0, x, y, 8, 8);
	break;
      case 40: //parentesis
      case 41:
        drawSprite (image, screen, (texto[i]-40)*8+8, 0, x, y, 8, 8);
	break;
      case 60: // <
	drawSprite (image, screen, 408, 0, x, y, 8, 8);
	break;
      case 62: // >
        drawSprite (image, screen, 368, 0, x, y, 8, 8);
	break;
      case 63: // ?
        drawSprite (image, screen, 384, 0, x, y, 8, 8);
	break; // [
      case 91:
        drawSprite (image, screen, 392, 0, x, y, 8, 8);
	break; // ]
      case 93:
	drawSprite (image, screen, 400, 0, x, y, 8, 8);
	break;
      default:
	break;
      }
    }
    x += 8;
    i++;
  }
}

void deleteArray(char listaRoms[MAX_ROM_LIST][256], int numRomList){
	int i;
	for (i = 0;i<numRomList;i++){
		strcpy(listaRoms[i], "0");
	}
}

void drawRomNames(SDL_Surface *screen, SDL_Surface *font, char listaroms[MAX_ROM_LIST][256], int numRomList){
	int y = 75;
	int i;
	for(i = 0; i<numRomList; i++) {
		drawText(font, screen, 195, y,uppercase(listaroms[i]),0);
		y+=15;
	}	
}

void showSplashText(SDL_Surface *screen, SDL_Surface *font){
		drawText(font, screen, 30, 45, "DGSMS BY MISATO", CENTER);
		drawText(font, screen, 30, 55, "---------------", CENTER);
		/*drawText(font, screen, 30, 70, "Thanks to:", CENTER);
		drawText(font, screen);*/		
		SDL_Flip(screen);
}

void drawImage(SDL_Surface *screen, SDL_Surface *image, int x, int y){
	SDL_Rect dstRect;
  dstRect.x = x;
  dstRect.y = y;
  dstRect.w = image->w;
  dstRect.h = image->h;
  
  SDL_BlitSurface(image, NULL, screen, &dstRect);
}

int isRom(const struct dirent *nombre){
	int isrom = 0;
	char *extension = strrchr(nombre->d_name, '.');
	//printf("Rom: %s\n", nombre->d_name);
	if (strcmp(extension, EXTENSION_1) == 0 
		|| strcmp(extension, EXTENSION_2) == 0) {
		isrom = 1;
	}
	return isrom;
}

//void startSelector(char nombreRom[256]){
char* selector (){
	enum state {STATE_SPLASH, STATE_ROMS, STATE_ARGS, STATE_END};
	SDL_Surface *font;
	SDL_Surface *background;
	SDL_Surface *arrow;
	SDL_Surface *splash;

	background = IMG_Load("background.png");
	if (!background) printf("IMG_Load failed: %s\n", SDL_GetError());
	
	font = IMG_Load("font.png");	
	if (!font) printf("IMG_Load failed: %s\n", SDL_GetError());
	
	SDL_SetColorKey(font,SDL_SRCCOLORKEY|SDL_RLEACCEL,SDL_MapRGB(font->format, 255,0,255));
	arrow = IMG_Load("arrow.png");
	if (!arrow) printf("IMG_Load failed: %s\n", SDL_GetError());
	
	SDL_SetColorKey(arrow,SDL_SRCCOLORKEY|SDL_RLEACCEL,SDL_MapRGB(arrow->format, 255,0,255));
	splash = IMG_Load("vieju-splash.png");
	if (!splash) printf("IMG_Load failed: %s\n", SDL_GetError());
	
	drawImage(screen, background, 0,0);
	SDL_Flip(screen);
	
	int count = 0;
	int state = STATE_SPLASH;
	int numRoms = 0;
	int minRom = 0;
	int maxRom = MAX_ROM_LIST;
	struct dirent **nombres;
	char listaRoms[MAX_ROM_LIST][256];
	int arrowY = 67;
	int posArrow = 0;
	SDL_Event event;
	int isRom(); //funcion de filtrado
	int numRomList = 0;
	int lastArrowY = 0;
	int romSelected = 0;
	int initState = 1;

	while (state != STATE_END){
		switch (state){
				case STATE_SPLASH: 
					if (initState){
						if (start) {
							drawImage(screen, splash, 0,0);
							SDL_Flip(screen);
							start=0;
						}
						initState = 0;
						 }
											 
						 if (count == 9990000){
							state = STATE_ROMS;
							initState = 1;
							count = 0;
						 }
											 
						 break;

				case STATE_ROMS:
					if(initState){	
						//printf("about to scan roms\n");
				 		numRoms = scandir(ROMS_DIR,&nombres,isRom,alphasort);
				 		//printf("Scanned roms: %d\n", numRoms);
						if (numRoms < MAX_ROM_LIST) {
							maxRom = numRoms;
							numRomList = numRoms;
						}
						else numRomList = MAX_ROM_LIST;
						deleteArray(listaRoms, numRomList);
						int x = 0;
						int i;
						for (i = minRom; i<maxRom; i++){
							strcpy(listaRoms[x],  nombres[i]->d_name);
							x++;
		  			 	}
						initState = 0;
						
					 }
					 drawImage(screen, background, 0,0);
					 drawRomNames(screen, font, listaRoms, numRomList);
					 drawImage(screen, arrow, 175, arrowY);							 
					 SDL_Flip(screen);
					 
					 //compruebo los eventos
					 while (SDL_PollEvent(&event)){
						if (event.type==SDL_KEYDOWN){
							switch (event.key.keysym.sym){
								case SDLK_UP: 
									arrowY-=15;
									posArrow--;
									if (posArrow < 0) {
				 						if (minRom != 0){
											minRom--;
											maxRom--;
											deleteArray(listaRoms, numRomList);
											int x = 0;
											int i;
											for (i = minRom; i<maxRom; i++){
												strcpy(listaRoms[x],  nombres[i]->d_name);
												x++;
											}
										}
									 arrowY = 67;
									 posArrow = 0;
									}
									break;
								case SDLK_DOWN:
									lastArrowY = arrowY;
									arrowY += 15;
									if (posArrow > numRomList-2){
										if(maxRom+1 <= numRoms){
										 minRom++;
										 maxRom++;
										 deleteArray(listaRoms, numRomList);
										 int x = 0;
										 int i;
										 for (i = minRom; i<maxRom; i++){
											strcpy(listaRoms[x],  nombres[i]->d_name);
											x++;
										 }						 	
										}
										//arrowY = 172;
										arrowY = lastArrowY;
									}
									else posArrow++;
									break;
							case SDLK_LCTRL: 
								romSelected = 1;
								strcpy(nombreRom, ROMS_DIR);
								strcat(nombreRom, listaRoms[posArrow]);
								state = STATE_ARGS;
								count = 0;
								initState = 1;
								break;
							
							case SDLK_TAB: 
								romSelected = 0;
								state = STATE_END;
								break;
										    				
					}
								
				}
			}
			break;
			case STATE_ARGS:	//printf("rom seleccionada: %s\n", listaRoms[posArrow]); 
							state = STATE_END;
							break;
		}
		count++;
	}
	
	free(nombres);
	SDL_FreeSurface(background);
	SDL_FreeSurface(font);
	SDL_FreeSurface(arrow);
	
	if (romSelected)
		return nombreRom;
	else
		return NULL;
}

