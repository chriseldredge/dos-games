#include "svga.h"
#include "mouse.h"

void Delay(int clicks);

FONT font;

void main()
{
 long i;
 int x, y, up, left;
 char c;

 char string[255];

 SVGA_SetupMode(SVGA_GetUserMode());

 MOUSE_SetRange(0, 0, MaxX, MaxY);
 MOUSE_Init();
 MOUSE_SetSensitivity(3,3);
 MOUSE_Show();

 if (!(SVGA_LoadFont("D:\\COMPILER\\PROJECTS\\SVGA\\FONT1.ASC", (FONT_PTR)&font)))
   return;

 SVGA_SetFont((FONT_PTR)&font);

 up=0;left=0;
 x=0;y=0;
 c = 0;

 while (!MOUSE_BUTTON_LEFT)
   {
    if (left)
      x--;
    else
      x++;
    if (up)
      y--;
    else
      y++;

    if (y<=0)
     up=0;
    else if (y>=MaxY-16)
     up=1;
    if (x<=0)
     left=0;
    else if (x>=MaxX-16)
     left=1;

    SVGA_PrintChar(x, y, c, 0, 0);

    if (MOUSE_BUTTON_RIGHT)
      {
       c++;
       if (c > CHAR_MAX) c=0;
       while (MOUSE_BUTTON_RIGHT) {}
      }
  }
 
 while (MOUSE_BUTTON_LEFT) {}

 SVGA_Cls(0);

 while (!MOUSE_BUTTON_LEFT)
   SVGA_PrintChar(rand()%(MaxX-16), rand()%(MaxY-16), (char)(rand()%CHAR_MAX), 0, 0);

 while (MOUSE_BUTTON_LEFT) {}

 SVGA_Cls(0);

 x=0; y=80; c=0;

 while (!MOUSE_BUTTON_LEFT)
   {
    SVGA_PrintChar(x, y, c, 0, 1);

    if (c==0)
     c=1;
    else if (c==1)
     c=0;

    x++;
    if (x>=MaxX)
      x = 0;

    i = rand()%2;
    if (i==0)
      y--;
    else
      y++;
    if (y>MaxY-16)
      y=MaxY-16;
    else if (y<0)
      y=0;
   }

 while (MOUSE_BUTTON_LEFT) {}

 SVGA_Cls(0);
 x = 0;

 while (!MOUSE_BUTTON_LEFT)
   {
    if (MOUSE_BUTTON_RIGHT)
     {
      MOUSE_SetCursor(CharSet+(c*CHAR_WIDTH*CHAR_HEIGHT));
      while (MOUSE_BUTTON_RIGHT) {SVGA_PrintChar(MOUSE_X, MOUSE_Y, c, 0, 0); }
      c++;
     }
   }

 MOUSE_Done();
 VESA_SetMode(TEXT_MODE);

 printf("Bp: %i\nCw: %i\nCh: %i\nMc: %i\nSx: %i\nSy: %i\n", font.BitsPerPixel, font.Width, font.Height, font.MaxChars, MOUSE_SENSITIVITY_X, MOUSE_SENSITIVITY_Y);
 _ffree(CharSet);
}

void Delay(int clicks)
{
unsigned int far *clock = (unsigned int far *)0x0000046CL;

unsigned int now;

now = *clock;

while(abs(*clock - now) < clicks) {}
}
