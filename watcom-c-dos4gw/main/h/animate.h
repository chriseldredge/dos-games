#ifdef __cplusplus

class SPRITE
 {
  public:
             SPRITE(BINT16 curx, BINT16 cury, BINT16 dstx, BINT16 dsty, BINT16 stpx, BINT16 stpy, BINT16 numframes, ...);
   BINT16    animate();
   VBINT     move(BINT16 x, BINT16 y);
  private:
   BINT16    nf, cf;
   IMAGE_PTR frame[10];
   IMAGE_PTR background;
   IMAGE     work[2];
   BINT16    cx, cy, ox, oy, dx, dy, sx, sy;
 };

#endif
