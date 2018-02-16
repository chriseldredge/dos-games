#include "svga.h"
#include "mouse.h"

void main()
{
 int y;

 SVGA_Init();
 SVGA_SetupMode(SVGA_640x480);
 SVGA_Cls(0);
 MOUSE_Init();
 MOUSE_Show();

 while (!kbhit()) {}

 getch();

 SVGA_PutPixel(5,5,15);
 SVGA_PutPixel(315,5,15);
 SVGA_PutPixel(314,26,15);
 SVGA_Line(6,5,314,5,14);
 SVGA_HLine(6,314,6,14);
 SVGA_Line(314,7,314,25,13);

 while (!kbhit())
   {
    if (MOUSE_BUTTON_RIGHT)
      {
       y = rand()%200;
       SVGA_HLine(0, 319, y, (unsigned char)(rand()%255));
      }
    if (MOUSE_BUTTON_LEFT) SVGA_PutPixel(rand()%432+108, rand()%340+108, (unsigned char)(rand()%255));
   }

 VESA_SetMode(TEXT_MODE);
 MOUSE_Done();
}
