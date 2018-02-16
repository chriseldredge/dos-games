#include "toolkit.h"

BINT16 sortpalette(PALETTE_PTR dst);

IMAGE    bitmap;
PALETTE  pal;
NOBINT32 i, ci;

void main(int argc, char *argv[])
{

 
 if (argc<5)
  {
   printf("More args");
   exit(1);
  }
 
 SVGA_LoadPCX(argv[1], &bitmap);
 SVGA_LoadPalette(argv[2], &pal);

 printf("Sorting...\n");
 if (sortpalette(&pal)) printf("FAILED!!\n");
 printf("Done.\nPlease be sure to use PSP to fix the new image...");
 
 for (i=0;i<256;i++)
  {
   bitmap.palette.color[i].red = pal.color[i].red;
   bitmap.palette.color[i].green = pal.color[i].green;
   bitmap.palette.color[i].blue = pal.color[i].blue;
  }
 
 SVGA_SavePCX(argv[3], &bitmap);
 SVGA_SavePalette(argv[4], &bitmap.palette);
}

BINT16 sortpalette(PALETTE_PTR dst)
/* Integrates the image palette to the 40 color system palette
   ARGS:
        dst - The palette to conform to
   RETS:
        0  - Success
        1  - An overflow of some sort occured */
{
 NOBINT32       c,i,np;
 NOBINT16       write_ok[256], fixed[256];
 NOBINT16       need_change[256];
 IMAGE          img;

 SVGA_InitImage(&img, bitmap.width, bitmap.height);
 SVGA_MergeImage(&img,0,0,bitmap.width,bitmap.height,&bitmap,0,0,BLIT_NORMAL);
 np = bitmap.width*bitmap.height;
 
 for (i=0;i<256;i++)                    // Search for un-used colors
  {
   if ((dst->color[i].red==0)&&
       (dst->color[i].green==0)&&
       (dst->color[i].blue==0))
         write_ok[i]=1;
    else
         write_ok[i]=0;
   fixed[i] = 0;
   need_change[i]=0;
  }
 write_ok[0]=0;
 write_ok[249]=0;

 for (i=0;i<np;i++)
  {
   need_change[bitmap.hMem[i]]=1;
  }
 need_change[0] = 0;                           // black=black

 if ((bitmap.palette.color[0].red==0)&&
     (bitmap.palette.color[0].green==0)&&
     (bitmap.palette.color[0].blue==0))
       need_change[0]=0;

 // Now we must just move the old into overwritable index's...

 for (ci=0;ci<256;ci++)
  {
   if (need_change[ci] && !fixed[ci])          // If the color needs fixing, fix it
    {
     for (c=0;c<255;c++)
      {
       if (write_ok[c]) break;            // Take first slot
      }

     if (c>=255) return(1);             // No slots left- return bad status!

     for (i=0;i<np;i++)                 // Change old ci's to new c's
      {
       if (img.hMem[i] == (NOBINT8)ci)
        {
         bitmap.hMem[i] = (NOBINT8)c;
        }
      }
     _fmemcpy(&dst->color[c], &bitmap.palette.color[ci], 3); // Set the new color
     fixed[ci] = 1;                                           // fix the color
     write_ok[c] = 0;                                        // dont overwrite the color
    }
  }
 return(0);
}

