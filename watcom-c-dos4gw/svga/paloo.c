#include "toolkit.h"

void main(int argc, char *argv[])
{
 IMAGE  i;
 FILE   *f;
 NOBINT8 ch;
 NOBINT32 c,d; //dummy;
 if (argc) i.width++;                   // stupid you didn't use argc error
 
 SVGA_SetupMode(SVGA_640x480);
/*
 i.palette.color[0].red = 0;
 i.palette.color[0].green = 0;
 i.palette.color[0].blue = 0;
 for (c=32;c<256;c++)
  SVGA_SetPalette(c,&i.palette.color[0]);

 SVGA_GetAllPalette(&i.palette);
 SVGA_SavePalette("paloo.pal", &i.palette);
*/

 SVGA_LoadPCX(argv[1], &i);
 SVGA_SetAllPalette(&i.palette);
 SVGA_ShowImage(&i,0,0,BLIT_NORMAL);
 getch();

 f = fopen(argv[2], "wb");
 fprintf(f, "CVE IMG\n");
 d = i.width;
 fwrite(&d, 1, 2, f);
 d = i.height;
 fwrite(&d, 1, 2, f);
 for (c=0;c<i.width*i.height;c++)
  {
   ch = i.hMem[c];
   fwrite(&ch, 1, 1, f);
  }
 fclose(f);
 f = fopen("new.bip", "wb");
 fwrite(&i.palette,768,1,f);
 fclose(f);
 VESA_SetMode(TEXT_MODE);
}
 
