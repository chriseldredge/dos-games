/*STI's new plasma routine*/

/*This program is based on a plasma routine I found on the front of
  PC-Format.  I have optimised it quite a bit though.  It now runs
  at about 10fps on my DX33 (With an ISA card!!) so it should be
  quite impressive on DX2's and upwards.  It is compiled using Borland
  C++ Version 4 in the TINY memory model

  For those of you who don't know how to do a plasma, imagine two complex
  sine waves, one on the x-axis and one on the y-axis.  Then, for each
  pixel, add the corresponding values for the two sine-waves, and plot
  that colour to the screen.  It's simpler than it sounds.  Honest!

  Simon Holt. (100753.3575@compuserve.com)
*/
  
  
#pragma off (check_stack)

#include <math.h>
#include <dos.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <bios.h>

#define NOMOUSE
#define NOKEY
#define NOKERNAL
#include "svga.c"

#include "behave.c"

image offscr;

void main(void)
{
 SVGA_SetupMode(SVGA_GetUserMode());
 colours();
 SVGA_InitImage(&offscr, MaxX+1, MaxY+1);

 while(!kbhit())    /*While no key is pressed...*/
  {
   plasma();
  }

 VESA_SetMode(TEXT_MODE);

 getch();
 exit(EXIT_SUCCESS);
}

void plasma(void)
{   
   void vblank(void);

   char xbuffer[1600], ybuffer[1600], value;
   char *work;
   unsigned char buffer[3];  
   static float angle1=PHASE_1;
   static float angle2=PHASE_2;
   static float angle3=PHASE_3;
   static float angle4=PHASE_4;
   static float angle5=PHASE_5;
   float ang1b, ang2b, ang3b, ang4b, ang5b;
   int x, y;
   
   ang1b=angle1+SPD_1;
   ang2b=angle2+SPD_2;  
   ang3b=angle3+SPD_3;
   ang4b=angle4+SPD_4;
   ang5b=angle5+SPD_5;
   
/*Work out the complex sine waves and put them in the buffers*/
   for(x=0; x<(MaxX+1)/2; x++)
   {
      xbuffer[x]=AMP_1*sin(angle1) + AMP_2*sin(angle2) + AMP_3*sin(angle3);
      ybuffer[x]=AMP_4*sin(angle4) + AMP_5*cos(angle5)+AMP_5*cos(1.3*angle5);
      angle1+=FREQ_1;
      angle2+=FREQ_2;
      angle3+=FREQ_3;
      angle4+=FREQ_4;
      angle5+=FREQ_5;
   }
   for(x=(MaxX+1)/2; x<MaxY; x++)
   {
      ybuffer[x]=AMP_4*sin(angle4) + AMP_5*cos(angle5)+AMP_5*cos(1.3*angle5);
      angle4+=FREQ_4;
      angle5+=FREQ_5;
   }      

   angle1=ang1b;
   angle2=ang2b;
   angle3=ang3b;
   angle4=ang4b;
   angle5=ang5b;
   
   work = offscr.hMem;
   
   for(y=0; y<MaxY+1; y++)
    {
     for(x=0; x<(MaxX+1)/2; x++)
      {
       value=xbuffer[x]+ybuffer[y];
       *(work++) = value;
       *(work++) = value;  
      }
    }

 SVGA_ShowImage(offscr, 0,0,0);
 
/*Rotate the palette to make it all a bit less predictable...*/

   for (x=1; x<256; x++)
  {  
      getcolour(x, buffer);
      setcolour(x-1, buffer);
   }
  getcolour(0, buffer);
  setcolour(255, buffer);
}


void colours(void)
{
   unsigned char palette[3];
   int counter;

   for(counter=0; counter<64; counter++)
   {
      palette[0]=counter;
      palette[1]=0;
      palette[2]=0;
      setcolour(counter, palette);
   }
   for(counter=64; counter<128; counter++)
   {
      palette[0]=63;
      palette[1]=counter-64;
      palette[2]=0;
      setcolour(counter, palette);
   }
   for(counter=128; counter<192; counter++)
   {
      palette[0]=191-counter;
      palette[1]=63;
      palette[2]=0;
      setcolour(counter, palette);
   }
   for(counter=192; counter<256; counter++)
   {
      palette[0]=0;
      palette[1]=255-counter;
      palette[2]=0;
      setcolour(counter, palette);
   }
}
