#include <stdio.h>
#include <string.h>
#include "svga.h"
#include "mouse.h"
#include "tools.h"

void NotRect(int x1, int y1, int x2, int y2);

void main(int argc, char *argv[])
{
 image img;
 FILE *fp;
 char  str[500], c;
 char  map[256]={0}, numchars=0;
 char  len, num[5];
 int   MOX, MOY, mx1, my1, mx2, my2, x, y, into, i,l,d;
 fontchar_ptr fc;
 FONT  s[2];

 if (strcmp(argv[1],"/J")==NULL)
  {
   SVGA_LoadFont(argv[2], &s[0]);
   SVGA_LoadFont(argv[3], &s[1]);
   fp = fopen(argv[4], "wb");
   fprintf(fp, "MAKEFONT");

   for (l=0;l<2;l++)
    {
     fc = (fontchar_ptr)&s[l].startchar;
     i=0;
     while (i<s[l].max_chars)
      {
       fputc(fc->width,fp);  
       fputc(fc->height,fp);
       for (d=0;d<fc->width*fc->height;d++)
        fputc(fc->data[d],fp);
       fc = (fontchar_ptr)fc->next_char;
       i++;
      }
    }
   fprintf(fp, "MAPTABLE");
   for (d=0;d<256;d++)
    {
     map[d] = s[0].map[d];
     if (!map[d])
      map[d] = s[1].map[d];
    }
   fwrite(&map, 1, 256, fp);
   fclose(fp);
   exit(0);
  }

 SVGA_SetupMode(VGA_320x200);
 SVGA_LoadPCX(argv[1], &img);
 SVGA_SetAllPalette(&img.palette);
 SVGA_ShowImage(img, 0, 0, 0);

 MOUSE_Init();
 MOUSE_Show();

 fp = fopen("FONT.MFF", "wb");
 fprintf(fp, "MAKEFONT");

 if (SVGA_LoadFont(argv[2], &s[0]))
  {
   fc = (fontchar_ptr)&s[0].startchar;
   i=0;
   while (i<s[0].max_chars)
    {
     fputc(fc->width,fp);  
     fputc(fc->height,fp);
     for (d=0;d<fc->width*fc->height;d++)
      fputc(fc->data[d],fp);
     fc = (fontchar_ptr)fc->next_char;
     i++;
    }
   numchars = s[0].max_chars;
   _fmemcpy(&map, &s[0].map, 256);
  }

 while (!kbhit())
  {
   while (!ML)
   {
    if (kbhit()) break;
    if (MR)
     {
      MOUSE_Hide();
      MX++;
      MOUSE_Show();
      while (MR) {}
     }
    }
   if (kbhit()) break;

   mx1 = MX; my1 = MY;
   MOX=mx1;  MOY = my1;
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
     if (MR)
      {
       MOUSE_Hide();
       MX++;
       MOUSE_Show();
       while (MR) {}
      }
    }
   mx2 = MX; my2 = MY;
  
   sprintf(str, "Image size is %ix%i. Keep? [Y/N]   ", mx2-mx1+1, my2-my1+1);
   SVGA_PrintString(0, MaxY-20, 15, str, 1);
   c=getch();
   NotRect(mx1, my1, MOX, MOY);
   if ((c=='y')||(c=='Y'))
    {
     x=mx2-mx1+1; y=my2-my1+1;
     fwrite(&x, 1, 1, fp);
     fwrite(&y, 1, 1, fp);
     for (y=my1;y<=my2;y++)
      for (x=mx1;x<=mx2;x++)
       {
        c=SVGA_GetPixel(x,y);
        fwrite(&c,1,1,fp);
       }
     SVGA_PrintString(0, MaxY-20, 15, "Enter the Ascii number to map:        ", 1);
     len=0;
     c=0;
     while (c!=13)
      {
       c=getch();
       num[len++] = c;
      }
     num[len-1] = 0;
     sscanf(num, "%i", &into);
     map[into] = numchars;
     numchars++;
     sprintf(str, "Enter the Ascii number to map: %3i    ", into);
     SVGA_PrintString(0, MaxY-20, 15, str, 1);
    }
   delay(250);
   SVGA_PrintString(0, MaxY-20, 15, "                                      ", 1);
  }
 fprintf(fp, "MAPTABLE");
 fwrite(&map, 1, 256, fp);
 fclose(fp);
 MOUSE_Hide();
 MOUSE_Done();
 VESA_SetMode(TEXT_MODE);
 if (kbhit()) getch();
 if (kbhit()) getch();
}



//////////////////////////////////////////////////////////////////////////////////////////////
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
