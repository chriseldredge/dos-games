#include <stdio.h>
#include <stdlib.h>
#include "svga.h"
#include "mouse.h"

void main()
{
 image pcx;
 FILE *fp;
 int i;

 fp = fopen("font.asc", "wb");

 SVGA_Init();
 SVGA_SetupMode(SVGA_640x480);
 
 MOUSE_Init();

 SVGA_LoadPCX("P:\\F.PCX", &pcx);
 SVGA_SetAllPalette((Palette_ptr)&pcx.palette);
 SVGA_ShowImage(pcx, 0, 0, 0);

 MOUSE_X = 0;
 MOUSE_Y = 0;

 fwrite("MAKEFONT\n", 9, 1, fp);
 i = 8;
 fwrite(&i, 1, 1, fp);
 i = 16;
 fwrite(&i,1, 1, fp);
 fwrite(&i,1, 1, fp);
 i = 80;
 fwrite(&i,1, 1, fp);

 for (MOUSE_X=0;MOUSE_X<16*27;MOUSE_X+=16)
  {
   MOUSE_Backup();
   fwrite(&BackupSprite, 256, 1, fp);
  }

 MOUSE_Y = 16;
                           
 for (MOUSE_X=0;MOUSE_X<16*27;MOUSE_X+=16)
  {
   MOUSE_Backup();
   fwrite(&BackupSprite, 256, 1, fp);
  }

 MOUSE_Y = 32;
                           
 for (MOUSE_X=0;MOUSE_X<16*27;MOUSE_X+=16)
  {
   MOUSE_Backup();
   fwrite(&BackupSprite, 256, 1, fp);
  }

 getch();

 fclose(fp);

 MOUSE_Done();
 VESA_SetMode(TEXT_MODE);
}
