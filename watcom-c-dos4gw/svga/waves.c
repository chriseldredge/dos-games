#include "toolkit.h"

IMAGE   wave;
BINT16  i,r,d;
BINT8   c;
BINT16  x=5, s=2, o=0;
BINT16  g;
STR     str[500];

void main()
{
 SVGA_LoadPCX("WAVE.PCX", &wave);
 SVGA_SetupMode(SVGA_GetUserMode());
 SVGA_SetAllPalette(&wave.palette);

 d=0;r=30;c=0;

 sprintf(str, "Amplitude: %i  Speed: %i", x, s);
 SVGA_PrintString(0,0,1,str,1);

 while ((c!=13)&&(c!=27))
  {
   if (kbhit()) c=getch(); else c=0;
   if (c=='W')
    {
     r=x;
     for (x=r;x<r+5;x++)
      {
       ripple();
       ripple();
      }
    }
   else if (c=='w')
    {
     r=x;
     for (x=r;x>r-5;x--)
      {
       ripple();
       ripple();
      }
    }
   else if (c=='S')
    s+=2; 
   else if (c=='s')  
    s-=2; 

   if (c!=0)
    {
     sprintf(str, "Amplitude: %i  Speed: %i                    ", x, s);
     SVGA_PrintString(0,0,1,str,1);
    }
    
   ripple();
  }

 VESA_SetMode(TEXT_MODE);
}

void ripple()
{
 o+=s;
 if (o>32000) o%=360; 
 for (g=0;g<MaxX;g+=wave.width)
  SVGA_ShowImage(&wave, g, (MaxY-wave.height)-x*SinTable[(o+g)%360], BLIT_NORMAL);
}



