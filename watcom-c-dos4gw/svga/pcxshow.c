#include "toolkit.h"

RGB c;

void main(int argc, char **argv)
{
 IMAGE pcx, img;
 int i, tf;
 int x, y;
 char Filename [PATH_LEN+NAME_LEN] ;

 c.red = c.green = c.blue = 0;

 if (argc<2)
  {
   printf("Specify a PCX to show!!");
   return;
  }

 SVGA_Init();

 tf = SearchFile(argv[1], ".PCX");
 if (!tf)
  {
   printf("File(s) not found.\n");
   return;
  }

 x = SVGA_GetUserMode();
 if (SVGA_SetupMode(x))
  {
   printf("\nThat mode is not supported by your video card!!\n");
   return;
  }

 KEYBOARD_Init();

 for (i=0;i<tf;)
   {
    strcpy(Filename,FilePath);
    strcat(Filename,FileList[i]);
    
    SVGA_Cls(0);
    if (!SVGA_LoadPCX(Filename, &pcx)) continue;
 
    SVGA_SetAllPalette(&pcx.palette);
    x = y = 0;
    SVGA_ShowImage(&pcx, x, y, 0);
    SVGA_GetImage(&img, x, y, pcx.width, pcx.height);
    getch();
    SVGA_Cls(0);
    getch();
    SVGA_ShowImage(&img, x, y, 0);
    while ((!KeyFlags[KEY_ENTER])&&(!KeyFlags[KEY_PAGEDN]))
     {
      if ((KeyFlags[KEY_LEFT]))
       {
        x--;
        SVGA_ShowImage(&pcx, x, y, 1);
       }
      else if ((KeyFlags[KEY_RIGHT]))
       {
        x++;
        SVGA_ShowImage(&pcx, x, y, 0);
       }
      if ((KeyFlags[KEY_UP]))
       {
        y--;
        SVGA_ShowImage(&pcx, x, y, 0);
       }
      else if ((KeyFlags[KEY_DOWN]))
       {
        y++;
        SVGA_ShowImage(&pcx, x, y, 0);
       }
      else if ((KeyFlags[KEY_PAGEUP])&&(i>=1))
       {
        while (KeyFlags[KEY_PAGEUP]) {}
        i-=2;
        break;
       }
      if (KeyFlags[KEY_ESC]) break;
     }
    while ((KeyFlags[KEY_ENTER])||(KeyFlags[KEY_PAGEDN])) {}
    SVGA_KillImage(&pcx);
    if (KeyFlags[KEY_ESC]) break;
    i++;
   }

 KEYBOARD_Done();
 VESA_SetMode(TEXT_MODE);
}
