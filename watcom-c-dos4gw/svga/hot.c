#include "toolkit.h"

void NotRect(int x1, int y1, int x2, int y2);

void main(int argc, char *argv[])
{
 IMAGE img;
 BINT16 x, sx=0, y, sy=0, MOX, MOY, x2, y2, d, i;
 FILE   *f, *fw;

 SVGA_SetupMode(SVGA_640x480x24);
 SVGA_LoadPCX(argv[1], &img);

 KEYBOARD_Init();
 MOUSE_Init();
 MOUSE_Show();

 f = fopen(argv[2], "wb");
 if ((fw = fopen(argv[3], "rb"))!=NULL)
  {
   fscanf(fw, "%2i\n", &d);
   for (i=0;i<d;i++)
    {
     fscanf(fw, "%4i, %4i, %4i, %4i, %i\n", &x, &y, &x2, &y2, &sx);
     fprintf(f, "%4i, %4i, %4i, %4i, %i\n", x, y, x2, y2, sx);
    }
   fclose(fw);
  }
 sx=0;

 SVGA_ShowImage(&img, sx, sy, BLIT_NORMAL);
 while (!KeyFlags[KEY_ESC])
  {
   if (KeyFlags[KEY_LEFT])
    {
     sx += MaxX/2;
     SVGA_Cls(0);
     SVGA_ShowImage(&img, sx, sy, BLIT_NORMAL);
     while(KeyFlags[KEY_LEFT]) {}
    }
   if (KeyFlags[KEY_RIGHT])
    {
     sx -= MaxX/2;
     SVGA_Cls(0);
     SVGA_ShowImage(&img, sx, sy, BLIT_NORMAL);
     while(KeyFlags[KEY_RIGHT]) {}
    }
   if (KeyFlags[KEY_UP])
    {
     sy -= MaxY/2;
     SVGA_Cls(0);
     SVGA_ShowImage(&img, sx, sy, BLIT_NORMAL);
     while(KeyFlags[KEY_UP]) {}
    }
   if (KeyFlags[KEY_DOWN])
    {
     sy += MaxY/2;
     SVGA_Cls(0);
     SVGA_ShowImage(&img, sx, sy, BLIT_NORMAL);
     while(KeyFlags[KEY_DOWN]) {}
    }
   if (ML)
    {
     x = MX; y = MY;
     MOX=x, MOY=y;
     while (ML)
      {
       if ((MOX != MX)||(MOY != MY))
        {
         _disable();
         NotRect(x, y, MOX, MOY);
         NotRect(x, y, MX, MY);
         MOX = MX, MOY = MY;
         _enable();
        }
      }
     x2 = MX, y2 = MY;
     if (x2<x)
      {
       d = x;
       x=x2;
       x2=d;
      }
     if (y2<y)
      {
       d=y;
       y = y2;
       y2=d;
      }
     forever
      {
       if (KeyFlags[KEY_Y])
        {
         fprintf(f, "%4i, %4i, %4i, %4i, %i\n", x+abs(sx), y+abs(sy), x2+abs(sx), y2+abs(sy), 1);
         break;
        }
       if (KeyFlags[KEY_N])
        break;
      }
     NotRect(x, y, x2, y2);
    }
  }
 fclose(f);
 KEYBOARD_Done();
 MOUSE_Done();
 VESA_SetMode(TEXT_MODE);
}

void NotHline(int x1, int x2, int y)
{
 int i;

 for (i=x1;i<x2;i++)
  SVGA_PutPixel(i, y, 255^SVGA_GetPixel(i, y));
}

void NotVline(int y1, int y2, int x)
{
 int i;

 for (i=y1;i<y2;i++)
  SVGA_PutPixel(x, i, ((SVGA_GetPixel(x, i))^255));
}

void NotRect(int x1, int y1, int x2, int y2)
{
 NotHline(x1, x2, y1);
 NotHline(x1, x2, y2);
 NotVline(y1, y2, x1);
 NotVline(y1, y2, x2);
}

