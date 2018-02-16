#include "toolkit.h"

#define centerX  MaxX/2 
#define centerY  MaxY/2
#define pxc(n)   n->width/2
#define phc(n)   n->height/2
#define k        (BINT32)sqrt(centerX*centerX+centerY*centerY)+5
//#define k        100
#define adjust   0.707106781
#define n        (BINT16)((FLOTE)k/adjust)  
 
#define InX(p)    ((p<=MaxX)&&(p>=0))
#define InY(p)    ((p<=MaxY)&&(p>=0))
#define InXY(g,b) (InX(g) && InY(b))
#define InPX(p, j)   ((p<=j->width)&&(p>=0))
#define InPY(p, j)   ((p<=j->height)&&(p>=0))
#define InPXY(p,y,j) (InPX(p, j) && InPY(y, j))
#define sqrot(n)  sqrt((n))

BINT32 i,c,t,b;
IMAGE  img1, img2, img3, bk;

void SP_Line(NOBINT16 x2, NOBINT16 y2);
void thing(BINT16 w, BINT16 r, IMAGE_PTR p);
void box();

void main(int argc, char *argv[])
{
 rand();

 SVGA_Init();
 SVGA_SetupMode(VGA_320x200);
 
 SVGA_LoadPCX("WARP1.PCX", &img1);
 SVGA_LoadPCX("WARP2.PCX", &img2);
 SVGA_LoadPCX("WARP3.PCX", &img3);

while (!kbhit())
{
 SVGA_SetAllPalette(&img1.palette);
 SVGA_ShowImage(&img1, centerX-img1.width/2,centerY-img1.height/2,BLIT_NORMAL);
  
 
 for (c=0;c<k;c++) 
  {

    {
     sound(c);
     thing(c, c, &img1);
     sound(100+(rand()%50));
     delay(3);
    }
  }

 SVGA_Cls(0);
 SVGA_SetAllPalette(&img2.palette);
 SVGA_ShowImage(&img2, centerX-img2.width/2,centerY-img2.height/2,BLIT_NORMAL);

 for (i=0;i<k;i++)
  {
   sound(k-i);
   thing(k-i, i+180, &img2);
   sound(100+(rand()%50));
   delay(3);
  }
 sound(50);
 delay(500);
 SVGA_ShowImage(&img3, centerX-img3.width/2,centerY-img3.height/2,BLIT_NORMAL);
 nosound();
 delay(1000);
}
getch();
 VESA_SetMode(TEXT_MODE);
}

void thing(BINT16 w, BINT16 r, IMAGE_PTR p)
{
 BINT32 radqu,x,y,y2,x2,x0,y0,u1,u2,u3,u4,v1,v2,v3,v4,u5,u6,u7,u8,v5,v6,v7,v8, cnt;
 FLOTE  radius;

 x0=centerX;
 y0=centerY;

 radius=(FLOTE) w;

 while (radius<(FLOTE)(w+1))
  {
   radqu=(BINT32)((FLOTE)(radius*radius));
   cnt =(BINT16)((FLOTE)radius/sqrt(2));
   for (x=0;x<=cnt;x++)
    {
     y=(BINT16)((FLOTE)sqrot(radqu-(FLOTE)(x*x))); 
     x2 = CosTable[r]*x - SinTable[r]*y;
     y2 = CosTable[r]*y + SinTable[r]*x;

     u1=x0-x;  v1=y0-y;
     u2=x0+x;  v2=y0+y;
     u3=x0-y;  v3=y0-x;
     u4=x0+y;  v4=y0+x;
     u5=pxc(p)-x2; v5=phc(p)-y2;
     u6=pxc(p)+x2; v6=phc(p)+y2;
     u7=pxc(p)-y2; v7=phc(p)-x2;
     u8=pxc(p)+y2; v8=phc(p)+x2;

     if (InXY(u1, v1) && InPXY(u5, v5, p))
      SVGA_PutPixel(u1, v1, p->hMem[v5*p->width+u5]);
     if (InXY(u2, v2) && InPXY(u6, v6, p))
      SVGA_PutPixel(u2, v2, p->hMem[v6*p->width+u6]);
     if (InXY(u3, v4) && InPXY(u7, v8, p))
      SVGA_PutPixel(u3, v4, p->hMem[v8*p->width+u7]);
     if (InXY(u4, v3) && InPXY(u8, v7, p))
       SVGA_PutPixel(u4, v3, p->hMem[v7*p->width+u8]); 

     u6 = CosTable[(360-r)%360]*x - SinTable[(360-r)%360]*y;
     v5 = CosTable[(360-r)%360]*y + SinTable[(360-r)%360]*x;
     u6 += pxc(p);
     v5 = phc(p)-v5;
     if (InXY(u2, v1) && InPXY(u6, v5, p))
      SVGA_PutPixel(u2, v1, p->hMem[v5*p->width+u6]); 

     u5 = CosTable[(360-r)%360]*x - SinTable[(360-r)%360]*y;
     v6 = CosTable[(360-r)%360]*y + SinTable[(360-r)%360]*x;
     u5 = pxc(p)-u5;
     v6 += phc(p);

     if (InXY(u1, v2) && InPXY(u5, v6, p))
      SVGA_PutPixel(u1, v2, p->hMem[v6*p->width+u5]); 

     x2 = CosTable[(360-r)%360]*x - SinTable[(360-r)%360]*y;
     y2 = CosTable[(360-r)%360]*y + SinTable[(360-r)%360]*x;
     u7 = pxc(p)-y2;
     v7 = phc(p)-x2;

     if (InXY(u3, v3) && InPXY(u7, v7, p))
      SVGA_PutPixel(u3, v3, p->hMem[v7*p->width+u7]);     //c

     x2 = CosTable[(360-r)%360]*x - SinTable[(360-r)%360]*y;
     y2 = CosTable[(360-r)%360]*y + SinTable[(360-r)%360]*x;
     u8 = pxc(p)+y2;
     v8 = phc(p)+x2;

     if (InXY(u4, v4) && InPXY(u8, v8, p))
      SVGA_PutPixel(u4, v4, p->hMem[v8*p->width+u8]);        //c
    }
   radius+=adjust;
  }
}


