#include "toolkit.h"

void main(int argc, char *argv[])
{
 IMAGE   pcx;

 SVGA_Init();

 if (strcmp(argv[3], "gp")==NULL)
  {
   printf("\nGetting palette out of pcx...\n");
   SVGA_LoadPCX(argv[1], &pcx);

   SVGA_SavePalette(argv[2], &pcx.palette);
  }
 else
  {
   printf("\nPutting palette into pcx...\n");
   SVGA_LoadPCX(argv[1], &pcx);

   SVGA_LoadPalette(argv[2], &pcx.palette);

   SVGA_SavePCX(argv[1], &pcx);
  }

} 
