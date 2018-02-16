#include "tools.h"
#include "dpmi.h"
#include "kernal.h"
#include "keyboard.h"
#include "mouse.h"
#include "svga.h"

void main()
{
 OBJECT   line1, line2;
 BINT16   x;
 
 SVGA_Init();
 SVGA_SetupMode(SVGA_GetUserMode());

 line1.head.points = 2;
 line1.head.c.x    = 0;
 line1.head.c.y    = 0;

 line2.head.points = 2;
 line2.head.c.x    = 0;
 line2.head.c.y    = 0;

 while (!kbhit())
  {
   line1.head.color  = rand()%255;
   line1.pt[0].x     = rand()%(MaxX);
   line1.pt[0].y     = rand()%(MaxY);
   line1.pt[1].x     = rand()%(MaxX);
   line1.pt[1].y     = rand()%(MaxY);
 

   line2.pt[0].x     = rand()%(MaxX);
   line2.pt[0].y     = rand()%(MaxY);
   line2.pt[1].x     = rand()%(MaxX);
   line2.pt[1].y     = rand()%(MaxY);
 
   SVGA_StartMorph(&line1, &line2);
   SVGA_StartMorph(&line2, &line1);
   
   x=1;
   while (x)
    {
//     SVGA_Line(line1.pt[0].x,line1.pt[0].y,line1.pt[1].x,line1.pt[1].y, 0);
     x=SVGA_MorphObject(&line1, &line2, 5);
     SVGA_Line(line1.pt[0].x,line1.pt[0].y,line1.pt[1].x,line1.pt[1].y, line1.head.color);
     delay(100);
    }
//   SVGA_Line(line1.pt[0].x,line1.pt[0].y,line1.pt[1].x,line1.pt[1].y, 0);
  }  
 getch();
 VESA_SetMode(TEXT_MODE);
}
