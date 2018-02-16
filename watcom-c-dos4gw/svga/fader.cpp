#include "toolkit.h"

void fadesplice(STR *fn1, STR *fn2, BINT16 d);
void fade3Dsquare(STR *fn1, STR *fn2, BINT16 d);

void main(int argc, char *argv[])
{
 BINT16 c;
 
 if (argc<3) {printf("\nNot enough images!\n"); exit(-1);}

 SVGA_Init();
 SVGA_SetupMode(SVGA_640x480x16);

 if (argv[1][0]=='1')
  fadesplice(argv[2],argv[3],160);
 else
  fade3Dsquares(argv[2],argv[3],40);
   
 VESA_SetMode(TEXT_MODE);
}

void fadesplice(STR *fn1, STR *fn2, BINT16 d)
{
 IMAGE  imgDst;
 IMAGE  imgFade[32];
 BINT16 c;
 
 SVGA_LoadPCX(fn1,&imgFade[0]);
 SVGA_LoadPCX(fn2,&imgDst);

 SVGA_ShowImage(&imgFade[0],0,0,BLIT_NORMAL);

 for (c=1;c<32;c++)
  {
   SVGA_InitImage(&imgFade[c], 640, 480);
   SVGA_MergeImage(&imgFade[c], 0,0,640,480, &imgFade[c-1],0,0, BLIT_NORMAL);
   FadeSplice((BINT8 *)imgDst.hMem, (BINT8 *)imgFade[c].hMem, 640*480);
   SVGA_PrintChar(c*8,0,0xFFFF,'.',0);
   if (kbhit()) break;
  }

 while (!kbhit())
  {
   for (c=0;c<31;c++)
    {
     SVGA_ShowImage(&imgFade[c],0,0,BLIT_NTN);
     delay(d);
     //getch();
    }
   delay(2000);
   for (c=31;c>=0;c--)
    {
     SVGA_ShowImage(&imgFade[c],0,0,BLIT_NTN);
     delay(d);
     //getch();
    }
   delay(2000);
  }

 while (kbhit()) getch();

 for (c=0;c<32;c++)
  SVGA_KillImage(&imgFade[c]);
 SVGA_KillImage(&imgDst); 
}

void fade3Dsquare(STR *fn1, STR *fn2, BINT16 d)
{
 IMAGE  imgDst;
 IMAGE  imgSrc;
 BINT16 c;
 
 SVGA_LoadPCX(fn1,&imgSrc);
 SVGA_LoadPCX(fn2,&imgDst);

 SVGA_ShowImage(&imgSrc,0,0,BLIT_NTN);

 

