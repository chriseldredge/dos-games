#include "toolkit.h"

#define mu(a,b) mul(a,b,25)
#define MF 33554432.0   // keep fixed point on bit 25 (this number is 2 to the 25th power!)
#define M1 33554432

long ex1, wy1, ex2, wy2, dx, dy, cx, cy, x, y, xx, yy;
long nx, ny, xs, ys;
int  px, py, mx1, my1, mx2, my2, MOX, MOY;
NOBINT32  num_iters, num_cols, i, do_again=1;
char str[500];

IMAGE  pcx;

void SVGA_MakePalette(PALETTE_PTR p, STR *s);
void NotRect(int x1, int y1, int x2, int y2);

void main(int argc, char *argv[])
{
 int VideoMode;
 int Err;
 
 Err = SVGA_Init();
 if (!Err) 
  {
   printf("VESA compliant video card is not present. Load your VESA driver!\n");
   exit(1);
  }

 VideoMode = SVGA_GetUserMode(MODE_8_BIT);

 if (SVGA_SetupMode(VideoMode))
  {
   printf("Failure setting video mode!\n");
   exit(2);
  }

 SVGA_MakePalette(&pcx.palette, argv[1]);

 MOUSE_Init();
 MOUSE_Show();

 ex1 = (long)(-2.25*MF);
 wy1 = (long)(-1.5*MF);
 ex2 = (long)(0.75*MF);
 wy2 = (long)(1.5*MF);

 num_iters = 64;
 num_cols = 64;

 while (do_again)
  {
   dx = (ex2 - ex1) / MaxX;
   dy = (wy2 - wy1) / MaxY;

   for (px=0;(px<MaxX);px++)
    for (py=0;(py<MaxY);py++)
     {
      if (kbhit()) break;
      cx = ex1 + px * dx;
      cy = wy1 + py * dy;
      x = y = 0;
      xs = ys = 0;

      for (i=0; (i < num_iters)&&(xs+ys < (M1*4)); i++)
       {
        xs = mu(x,x);
        ys = mu(y,y);
        xx = xs - ys + cx;
        yy = (mu(x,y)<<1)+cy;
        x = xx; y = yy;
       }
      if (i==num_iters)
       i = 0;
      else
       i = i % num_cols;

      SVGA_PutPixel(px, py, i);
     }
   if (kbhit()) getch();

   SVGA_GetImage(&pcx, 0,0,MaxX, MaxY);
   sprintf(str, "Range: %lf,%lf to %lf,%lf  Dx:%lf  Dy: %lf", (double)(ex1 / MF), (double)(wy1 / MF), (double)(ex2 / MF),(double)(wy2 / MF), (double)(dx/MF), (double)(dy/MF));
   SVGA_PrintString(0,0,15, str, 1);

   while (!kbhit())
    {
      
     if (ML)
      {
       mx1 = MX; my1 = MY;
       while (ML)
        {
         if ((MOX != MX)||(MOY != MY))
          {
           _disable();
           NotRect(mx1, my1, MOX, MOY);
           NotRect(mx1, my1, MX, MY);
           MOX = MX, MOY = MY;
           _enable();
          }
        }
       mx2 = MX; my2 = MY;
       mx2 = mx1 + (((my2-my1+1)*MaxX)/MaxY);
       ex2 = ex1 + mx2 * dx;
       wy2 = wy1 + my2 * dy;
       ex1 += mx1 * dx;
       wy1 += my1 * dy;
       MOX = MOY = 0;
       do_again = 1;
       SVGA_Cls(0);
       break;
      }
    }
   if (kbhit()) do_again = 0;
  }
 
 while (kbhit()) getch();

 MOUSE_Done();
 SVGA_ShowImage(&pcx, 0,0,BLIT_NORMAL);

 while (!kbhit())
  SVGA_CyclePalette(&pcx.palette, 1, 255, 1), delay(50);

 SVGA_GetAllPalette(&pcx.palette);
 SVGA_SavePCX("MNDLBROT.PCX", &pcx);

 
 VESA_SetMode(TEXT_MODE);
}

void SVGA_MakePalette(PALETTE_PTR p, STR *s)
{
 int i;

 if (strcmp(s, "reg")==NULL)
  {
   for (i=0;i<256;i++)
    SVGA_GetPalette(i, (RGB_PTR)&p->color[i]);
   return;
  }
 else if (strcmp(s, "rb")==NULL)
  {
   for (i=0;i<64;i++)
    {
     p->color[i].red   = i;
     p->color[i].green = 0;
     p->color[i].blue  = 0;
    }
   for (i=64;i<128;i++)
    {
     p->color[i].red   = 127-i;
     p->color[i].green = 0;
     p->color[i].blue  = 0;
    }
   for (i=128;i<192;i++)
    {
     p->color[i].red   = 0;
     p->color[i].green = 0;
     p->color[i].blue  = i-128;
    }
   for (i=192;i<256;i++)
    {
     p->color[i].red   = 0;
     p->color[i].green = 0;
     p->color[i].blue  = 191-i;
    }
  }
 else 
  {
   for (i=0;i<32;i++)
    {
     p->color[i].red   = i*2;
     p->color[i].green = 0;
     p->color[i].blue  = 0;
    }
   for (i=32;i<64;i++)
    {
     p->color[i].red   = 126-(i*2);
     p->color[i].green = 0;
     p->color[i].blue  = 0;
    }
   for (i=64;i<96;i++)
    {
     p->color[i].red   = 0;
     p->color[i].green = (i-64)*2;
     p->color[i].blue  = 0;
    }
   for (i=96;i<128;i++)
    {
     p->color[i].red   = 0;
     p->color[i].green = 126-((i-64)*2);
     p->color[i].blue  = 0;
    }
   for (i=128;i<160;i++)
    {
     p->color[i].red   = 0;
     p->color[i].green = 0;
     p->color[i].blue  = (i-128)*2;
    }
   for (i=160;i<192;i++)
    {
     p->color[i].red   = 0;
     p->color[i].green = 0;
     p->color[i].blue  = 126-((i-128)*2);
    }
   for (i=192;i<224;i++)
    {
     p->color[i].red = p->color[i].green = p->color[i].blue = (i-192)*2;
    }
   for (i=224;i<256;i++)
    {
     p->color[i].red = p->color[i].green = p->color[i].blue = 126-((i-192)*2);
    }
   p->color[255].red = p->color[255].green = p->color[255].blue = 63;
  }
// SVGA_GetAllPalette(p);
 SVGA_SetAllPalette(p);
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
  SVGA_PutPixel(x, i, ((SVGA_GetPixel(x, i))^255));// printf("\n%i,%i", SVGA_GetPixel(x, i), 255^SVGA_GetPixel(x, i));
}

void NotRect(int x1, int y1, int x2, int y2)
{
 NotHline(x1, x2, y1);
 NotHline(x1, x2, y2);
 NotVline(y1, y2, x1);
 NotVline(y1, y2, x2);
}



