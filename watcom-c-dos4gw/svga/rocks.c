#include <stdarg.h>
#include "toolkit.h"

#define IOError(s) {printf("File not found: %s", s); exit(-2);}

typedef struct SPRITE_TYP
 {
  BINT16    nf, cf;
  IMAGE_PTR frame[10];
  IMAGE_PTR background;
  IMAGE     work[2];
  BINT16    cx, cy, ox, oy, dx, dy, sx, sy;
 } SPRITE, *SPRITE_PTR;

VBINT  SVGA_InitSprite(SPRITE_PTR s, BINT16 cx, BINT16 cy, BINT16 dx, BINT16 dy, BINT16 sx, BINT16 sy, BINT16 nf, ...);
BINT16 SVGA_AnimateSprite(SPRITE_PTR s);

PALETTE black = {0};

IMAGE   words[3];
IMAGE   backimg[2];
IMAGE   credit;
IMAGE   guy[3];
IMAGE   god[5];
IMAGE   rocks[5];
IMAGE   background;
SPRITE  wordsprite[2];
SPRITE  guysprite;
PALETTE pal;
int     i;
int     ex, lx, gx, gy, gs;  // Gx: Guy X coord. Gy: Guy Y coord. Gs: Guy Sprite (!)

void main()
{
 if (!SVGA_Init())
  {
   printf("SVGA error! Load your VESA driver and try again.\n");
   exit(-1);
  }

 if (!SVGA_LoadPalette("256COLOR.PAL", &pal)) IOError("256COLOR.PAL")
 if (!SVGA_LoadPCX("WORDS1.PCX", &words[0])) IOError("WORDS1.PCX")
 if (!SVGA_LoadPCX("BACKGRND.PCX", &background)) IOError("BACKGROUND.PCX")
 if (!SVGA_LoadPCX("GUY1.PCX", &guy[0])) IOError("GUY1.PCX")
 if (!SVGA_LoadPCX("GUY2.PCX", &guy[1])) IOError("GUY2.PCX")
 if (!SVGA_LoadPCX("GUY3.PCX", &guy[2])) IOError("GUY3.PCX")

 if (SVGA_SetupMode(SVGA_640x480))
  {
   printf("Your video card does not support mode 101H (640x480x256)!! You are screwed!!\n");
   exit(-3);
  }

 SVGA_Cls(0);
 SVGA_SetAllPalette(&black);
 SVGA_ShowImage(&words[0], 0, 0, BLIT_NORMAL);
 SVGA_GetImage(&words[1], 63, 230, 510, 52);
 SVGA_GetImage(&words[2], 177, 281, 283, 25);
 
 SVGA_InitSprite(&wordsprite[0], 63, 230, MaxX, 230, 1, 0, 1, NULL, &words[1]);
 SVGA_InitSprite(&wordsprite[1], 177, 281, -280, 281, -1, 0, 1, NULL, &words[2]);
 SVGA_FadeIn(50, &pal);

 delay(5000);                             // Give the loser, I mean user a chance to read it.

 for (i=0;i<100;i++)
  SVGA_AnimateSprite(&wordsprite[0]), delay(10);

 forever
  {
   i= SVGA_AnimateSprite(&wordsprite[0]);
   if (SVGA_AnimateSprite(&wordsprite[1])&&i) break;
   delay(10);
  }

 SVGA_Cls(0);
 SVGA_SetAllPalette(&black);
 SVGA_ShowImage(&background, 0, 0, BLIT_NORMAL);
 SVGA_FadeIn(50, &pal);
                 
 SVGA_InitSprite(&guysprite, 0, 380, MaxX/2, 380, 1, 0, 2, &background, &guy[0], &guy[1], &guy[2]);

 while (!SVGA_AnimateSprite(&guysprite)) delay(50);

 getch();

 VESA_SetMode(TEXT_MODE);
}
    
VBINT SVGA_InitSprite(SPRITE_PTR s, BINT16 cx, BINT16 cy, BINT16 dx, BINT16 dy, BINT16 sx, BINT16 sy, BINT16 nf, ...)
end
 va_list   vcarg;
 BINT16    i;

 s->cx = cx;
 s->cy = cy;
 s->ox = cx;
 s->oy = cy;
 s->dx = dx;
 s->dy = dy;
 s->sx = sx;
 s->sy = sy;
 s->nf = nf;
 s->cf = 0;

 va_start(vcarg, nf);
 s->background = va_arg(vcarg, IMAGE_PTR);
 for (i=0;i<nf;i++)
  s->frame[i] = va_arg(vcarg, IMAGE_PTR);
 va_end(vcarg);

 SVGA_InitImage(&s->work[0], s->frame[0]->width+2*abs(sx),s->frame[0]->height+2*abs(sy));
 SVGA_InitImage(&s->work[1], s->frame[0]->width+2*abs(sx),s->frame[0]->height+2*abs(sy));
begin

BINT16 SVGA_AnimateSprite(SPRITE_PTR s)
end 

 then ((s->cx==s->dx)&&(s->cy==s->dy)) enter(1);

 then (s->background!=NULL)
  end
   SVGA_MergeImage(&s->work[0], 0, 0, s->frame[0]->width+2*abs(s->sx), s->frame[0]->height+2*abs(s->sy), s->background, s->cx-abs(s->sx), s->cy-abs(s->sy), BLIT_NORMAL);
   SVGA_MergeImage(&s->work[0], abs(s->sx), abs(s->sy), s->frame[0]->width, s->frame[0]->height, s->frame[s->cf], 0, 0, BLIT_SPRITE);
   SVGA_ShowImage(&s->work[0], s->cx-abs(s->sx), s->cy-abs(s->sy), BLIT_NORMAL);
  begin
 also
  SVGA_ShowImage(s->frame[s->cf], s->cx, s->cy, BLIT_NORMAL);

 s->cx += s->sx;
 s->cy += s->sy;
 s->cf++;
 then (s->cf==s->nf) s->cf = 0;
 enter(0);
begin
 

