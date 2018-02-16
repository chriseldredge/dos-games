#include "toolkit.h"

void main()
{
 IMAGE Blobs[2];
 IMAGE Background, WorkSprite;
 int   Blob;
 int   X, Y;

 if (!SVGA_Init())
  {
   printf("\nFailure initializing graphics!!"
          "\nWhat about a VESA compliant "
          "\ndriver for your video card? Try again, please.");
   return;
  }
 
 if (!SVGA_LoadPCX("BLOB1.PCX", &Blobs[0]))
  {
   printf("\nFailure locating or loading file: BLOB1.PCX.  Aborting...");
   return;
  }

 if (!SVGA_LoadPCX("BLOB2.PCX", &Blobs[1]))
  {
   printf("\nFailure locating or loading file: BLOB2.PCX.  Aborting...");
   SVGA_KillImage(&Blobs[0]);
   return;
  }

 if (!SVGA_LoadPCX("MAP1.PCX", &Background))
  {
   printf("\nFailure locating or loading file: MAP1.PCX.  Aborting...");
   SVGA_KillImage(&Blobs[0]);
   SVGA_KillImage(&Blobs[1]);
   return;
  }

 SVGA_SetupMode(SVGA_640x480);
 SVGA_SetAllPalette(&Background.palette);

 Blob = 0;
 X = 0; Y = 50;

 KEYBOARD_Init();

 SVGA_InitImage (&WorkSprite, Blobs[0].width, Blobs[0].height);
 SVGA_ShowImage (&Background, 0, 0, 0);
 SVGA_MergeImage(&WorkSprite, 0, 0, Blobs[0].width, Blobs[0].height, &Background, X, Y, 0);
 SVGA_MergeImage(&WorkSprite, 0, 0, Blobs[0].width, Blobs[0].height, &Blobs[Blob], 0, 0, 1);
 SVGA_ShowImage (&WorkSprite, X, Y, 0);

 while (!KeyFlags[KEY_ENTER])
  {
   if (KeyFlags[KEY_LEFT])
    {
     X-=10;
     Blob = !Blob;
    }
   else if (KeyFlags[KEY_RIGHT])
    {
     X+=10;
     Blob = !Blob;
    }
   else if (KeyFlags[KEY_UP])
    {
     Y-=10;
     Blob = !Blob;
    }
   else if (KeyFlags[KEY_DOWN])
    {
     Y+=10;
     Blob = !Blob;
    }
   
   SVGA_ClearImage(&WorkSprite, 0);
   SVGA_MergeImage(&WorkSprite, 0, 0, Blobs[0].width, Blobs[0].height, &Background, X, Y, BLIT_NORMAL);
   SVGA_MergeImage(&WorkSprite, 0, 0, Blobs[0].width, Blobs[0].height, &Blobs[Blob], 0, 0, BLIT_SPRITE);
   SVGA_ShowImage (&WorkSprite, X, Y, BLIT_NORMAL);
   delay(250);
  }

 KEYBOARD_Done();
 VESA_SetMode(TEXT_MODE);
}
