#include "svga.h"
#include "mouse.h"
#include "keyboard.h"

typedef struct SNOW_FLAKE_TYPE
  {
   int X, Y;
   int OldX, OldY;
   int Try;
  } SNOW_FLAKE;

SNOW_FLAKE SNOW_FLAKES[6001];  
int        SNOW_GROUND;
int        SNOW_MAX_FLAKES;
FONT       SNOW_ICONS;

int  SNOW_INIT(void);
void SNOW_DONE(void);
int  SNOW_GET_VMODE(void);
void SNOW_ANIMATE(int MAX);
