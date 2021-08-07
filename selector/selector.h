#ifndef _SELECTOR_H_
#define _SELECTOR_H_

#define CENTER 1
#define ROMS_DIR "./roms/"
#define EXTENSION_1 ".vms"
#define EXTENSION_2 ".VMS"
#define MAX_ROM_LIST 9
#define EMU_NAME "./vmudingoo.dge"

char *uppercase (char * cadena);
void drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height);
void drawText (SDL_Surface *image, SDL_Surface *screen, int x, int y, char *texto, int center);
void deleteArray(char listaRoms[MAX_ROM_LIST][256], int numRomList);
void drawRomNames(SDL_Surface *screen, SDL_Surface *font, char listaroms[MAX_ROM_LIST][256], int numRomList);
void showSplashText(SDL_Surface *screen, SDL_Surface *font);
void drawArrow(SDL_Surface *screen, SDL_Surface *arrow, int x, int y);
void startSelector(char nombreRom[256]);
int isRom(const struct dirent *nombre); //funcion de filtrado
char* selector(void);

#endif
