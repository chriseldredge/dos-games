#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <math.h>

#include "cursors.h"

#define TRUE  1
#define FALSE 0

void _loadds far MouseHandler(int max, int mbx, int mcx, int mdx,
                              int msi, int mdi);

int  MOUSE_Reset(void);
int  MOUSE_Init(void);
void MOUSE_Done(void);
void MOUSE_Hide(void);
void MOUSE_Show(void);
void MOUSE_SetRange(int x1, int y1, int x2, int y2);
void MOUSE_Replace(void);
void MOUSE_Backup(void);
void MOUSE_DrawCursor(void);
void MOUSE_SetCursor(unsigned char *c, unsigned int nw, unsigned int nh);
void MOUSE_SetSensitivity(int x, int y);

#ifdef MOUSE_C
short int volatile
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_RIGHT,
    MOUSE_X, MOUSE_Y,
    MOUSE_OLD_X, MOUSE_OLD_Y,
    MOUSE_LIMIT_X1, MOUSE_LIMIT_X2,
    MOUSE_LIMIT_Y1, MOUSE_LIMIT_Y2,
    MOUSE_SENSITIVITY_X, MOUSE_SENSITIVITY_Y,
    MOUSE_SHOWING,
    MOUSE_CURSOR_WIDTH, MOUSE_CURSOR_HEIGHT;

unsigned char MOUSE_CURSOR[6400];
unsigned char BackupSprite[6400];
unsigned char BackupSprite_fg[6400];
int   hey;
#else
extern short int volatile
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_RIGHT,
    MOUSE_X, MOUSE_Y,
    MOUSE_OLD_X, MOUSE_OLD_Y,
    MOUSE_LIMIT_X1, MOUSE_LIMIT_X2,
    MOUSE_LIMIT_Y1, MOUSE_LIMIT_Y2,
    MOUSE_SENSITIVITY_X, MOUSE_SENSITIVITY_Y,
    MOUSE_SHOWING,
    MOUSE_CURSOR_WIDTH, MOUSE_CURSOR_HEIGHT;

extern unsigned char MOUSE_CURSOR[6400];
extern unsigned char BackupSprite[6400];
extern unsigned char BackupSprite_fg[6400];
extern int hey;
#endif

