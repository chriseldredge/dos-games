#include "toolkit.h"

IMAGE  frame[45];
BINT16 i, step, delaytime;
BINT8  c;
STR    fn[50];

void main()
{
 for (i=0;i<45;i++)
  {
   if ((i+1)<10)
    strcpy((char *)fn, (const char *) "MNDLBRT");
   else
    strcpy((char *)fn, (const char *) "MNDLBT");
   sprintf(fn, "%s%i.PCX", fn, i+1);
   if (!SVGA_LoadPCX(fn, &frame[i])) printf("\7\n");
  }

 SVGA_SetupMode(SVGA_GetUserMode());

 step = 1;
 i = 0;
 c = 0;
 delaytime = 750;
 while (c!=27)
  {
   c=0;
   SVGA_ShowImage(&frame[i], 0, 0, BLIT_NORMAL);
   i += step;
   if (i==45)
    step = -1;
   else if (i==0)
    step = 1;
   if (kbhit()) c = getch();
   if (c=='t')
    delaytime+=25;
   else if (c=='g')
    delaytime-=25;
   if (delaytime<=0)
    delaytime=0;
   delay(delaytime);
  }
 VESA_SetMode(TEXT_MODE);   
}
