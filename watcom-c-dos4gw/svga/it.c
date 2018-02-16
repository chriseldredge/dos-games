/* IMAGETEST - IT.C */

#include "toolkit.h"

void main()
{
 IMAGE img[3];
 int sx=0, sy=0, dx=0, dy=0;
 char s[200];

 SVGA_SetupMode(SVGA_GetUserMode());

 SVGA_LoadPCX("IT0.PCX", &img[0]);
 SVGA_InitImage(&img[2], 150, 150);

 MOUSE_Init();
 
 SVGA_Cls(0);
 SVGA_SetAllPalette((PALETTE_PTR)&img[0].palette);
 SVGA_Rectangle(MaxX - 55, 5, MaxX - 4, 56, 50, 0);

 SVGA_LoadPCX("ITFG.PCX", &img[1]);
 SVGA_SetForeground(&img[1]);

 MOUSE_Show();
 SVGA_ClearImage(&img[2], 0);

 SVGA_Cls(35);

 SVGA_ShowImage(&img[0], 1, 1, BLIT_SPRITE);
 SVGA_MergeImage(&img[2], 0, 0, 50, 50, &img[0], img[0].width-29, 50, 0);
 SVGA_ShowImage(&img[2], MaxX-54, 6, BLIT_SPRITE);

 KEYBOARD_Init();

 while (!KeyFlags[KEY_ESC])
  {
   sprintf(s, "DX: %i DY: %i SX: %i SY: %i      ", dx, dy, sx, sy);
   SVGA_PrintString(0, MaxY-20, LightRed, s, 1);
   if (KeyFlags[KEY_LEFT])
     {
      if (KeyFlags[KEY_LALT])
        dx--;
      else
        sx--;
     }
   else if (KeyFlags[KEY_RIGHT])
     {
      if (KeyFlags[KEY_LALT])
        dx++;
      else
        sx++;
     }
   if (KeyFlags[KEY_DOWN])
     {
      if (KeyFlags[KEY_LALT])
        dy++;
      else
        sy++;
     }
   else if (KeyFlags[KEY_UP])
     {
      if (KeyFlags[KEY_LALT])
        dy--;
      else
        sy--;
     }
   SVGA_ClearImage(&img[2], 245); 
   SVGA_MergeImage(&img[2], dx, dy, 150, 150, &img[0], sx, sy, 0);
   SVGA_ShowImage(&img[2], MaxX-154, 6, BLIT_SPRITE);
  }

 MOUSE_Done();
 KEYBOARD_Done();

 VESA_SetMode(TEXT_MODE);
}
