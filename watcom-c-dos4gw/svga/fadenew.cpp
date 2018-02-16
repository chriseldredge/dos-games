#include "toolkit.h"

void fade_splice(IMAGE_PTR src, IMAGE_PTR dst, BINT16 d);

void main(int argc, char *argv[])
{
 IMAGE  imgDst;
 IMAGE  imgFade[32];
 BINT16 c;
 
 if (argc<2) {printf("\nNot enough images!\n"); exit(-1);}
 SVGA_Init();
 
 BytesPerPixel = 2;

 SVGA_LoadPCX(argv[1],&imgFade[0]);
 SVGA_LoadPCX(argv[2],&imgDst);

 SVGA_SetupMode(SVGA_640x480x16);

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
     delay(160);
     //getch();
    }
   delay(2000);
   for (c=31;c>=0;c--)
    {
     SVGA_ShowImage(&imgFade[c],0,0,BLIT_NTN);
     delay(160);
     //getch();
    }
   delay(2000);
  }

 while (kbhit()) getch();

 VESA_SetMode(TEXT_MODE);
}

void fade_splice(IMAGE_PTR src, IMAGE_PTR dst, BINT16 d)
{
 IMAGE  imgDst;
 IMAGE  imgFade[32];
 BINT16 c;
 
 if (argc<2) {printf("\nNot enough images!\n"); exit(-1);}
 SVGA_Init();
 
 BytesPerPixel = 2;

 SVGA_LoadPCX(argv[1],&imgFade[0]);
 SVGA_LoadPCX(argv[2],&imgDst);

 SVGA_SetupMode(SVGA_640x480x16);

 SVGA_ShowImage(&imgFade[0],0,0,BLIT_NORMAL);

 for (percent=1;percent<32;percent++)
  {
   SVGA_InitImage(&imgFade[percent], 640, 480);
   SVGA_MergeImage(&imgFade[percent], 0,0,640,480, &imgFade[percent-1],0,0, BLIT_NORMAL);
   FadeSplice((BINT8 *)imgDst.hMem, (BINT8 *)imgFade[percent].hMem, 640*480);
  }

 while (percent!=0xFF) {}

 while (!kbhit())
  {
   for (c=0;c<31;c++)
    {
     SVGA_ShowImage(&imgFade[c],0,0,BLIT_NTN);
     delay(160);
     //getch();
    }
   delay(2000);
   for (c=31;c>=0;c--)
    {
     SVGA_ShowImage(&imgFade[c],0,0,BLIT_NTN);
     delay(160);
     //getch();
    }
   delay(2000);
  }

 while (kbhit()) getch();

}
