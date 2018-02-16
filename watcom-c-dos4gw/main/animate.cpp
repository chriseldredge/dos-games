#include "toolkit.h"

SPRITE::SPRITE(BINT16 curx, BINT16 cury, BINT16 dstx, BINT16 dsty, BINT16 stpx, BINT16 stpy, BINT16 numframes, ...)
{
 va_list   vcarg;
 BINT16    i;

 cx = curx;
 cy = cury;
 ox = curx;
 oy = cury;
 dx = dstx;
 dy = dsty;
 sx = stpx;
 sy = stpy;
 nf = numframes;
 cf = 0;

 va_start(vcarg, nf);
 background = va_arg(vcarg, IMAGE_PTR);
 for (i=0;i<nf;i++)
  frame[i] = va_arg(vcarg, IMAGE_PTR);
 va_end(vcarg);

 SVGA_InitImage(&work[0], frame[0]->width+2*abs(sx),frame[0]->height+2*abs(sy));
 SVGA_InitImage(&work[1], frame[0]->width+2*abs(sx),frame[0]->height+2*abs(sy));
}

BINT16 SPRITE::animate()
{ 

 if ((cx==dx)&&(cy==dy)) enter(1);

 if (background!=NULL)
  {
   SVGA_MergeImage(&work[0], 0, 0, frame[0]->width+2*abs(sx), frame[0]->height+2*abs(sy), background, cx-abs(sx), cy-abs(sy), BLIT_NORMAL);
   SVGA_MergeImage(&work[0], abs(sx), abs(sy), frame[0]->width, frame[0]->height, frame[cf], 0, 0, BLIT_SPRITE);
   SVGA_ShowImage(&work[0], cx-abs(sx), cy-abs(sy), BLIT_NORMAL);
  begin
 also
  SVGA_ShowImage(frame[cf], cx, cy, BLIT_SPRITE);

 cx += sx;
 cy += sy;
 cf++;
 if (cf==nf) cf = 0;
 enter(0);
}
  
