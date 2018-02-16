
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

BINT32 i,c,t,b, fr;
IMAGE  img, bk;
STR    l;
NOBINT16 start,stop, *cl;

void SP_Line(NOBINT16 x2, NOBINT16 y2);
void thing(BINT16 w, BINT16 r, IMAGE_PTR p);
void box();
void sinimg(IMAGE_PTR img, BINT16 r, BINT16 coe, BINT16 mod);

#include "p:\main\s3.c"

void main(int argc, char *argv[])
{
 char key;

 cl = (NOBINT16 *)0x046C;

 printf("\nUse VESA?\n");
 key=toupper(getch());
 if (key!='Y') no_vesa++;

 SVGA_Init();
 SVGA_SetupMode(SVGA_GetUserMode(MODE_8_BIT));
 S3_Check();
 S3_IdentifyChipset();
 

 if (argc>=2)
  SVGA_LoadPCX(argv[1], &img);
 else
  SVGA_LoadPCX("IMAGE.PCX", &img);

 SVGA_SetAllPalette(&img.palette);
 SVGA_ShowImage(&img, centerX-img.width/2,centerY-img.height/2,BLIT_NORMAL);
  
 if (argv[2][0]=='1') 
  {
   for (i=0;i<k;i++)
    {
     if (kbhit()) break;
     thing(i, i, &img);
    }
   getch();
   t = i;
   b = i;
   for (i=t;i<2*t;i++)
    {
     if (kbhit()) break;
     b--;
     thing(i, b, &img);
    }
  }
 else if (argv[2][0]=='2') 
  {
   for (i=0;i<k;i++)
    {
     if (kbhit()) break;
     thing(i, 360-i, &img);
    }
   getch();
   t = i;
   b = i;
   for (i=t;i<2*t;i++)
    {
     if (kbhit()) break;
     b--;
     thing(i, b, &img);
    }
  }
 else if (argv[2][0]=='3') 
  {
   for (i=0;i<k;i++)
    {
     for (c=0;c<k;c+=2)
      {
       if (kbhit()) break;
       thing(c, 2+(rand()%10)-1, &img);
       thing(c+1, 2+(rand()%10)-1, &img);
      }
   
  //   box();

     if (kbhit()) break;
     SVGA_KillImage(&bk);
     SVGA_GetImage(&bk, 0,0,MaxX, MaxY);
     SVGA_MergeImage(&img, img.width/2 - centerX, img.height/2 -centerY, MaxX, MaxY, &bk, 0, 0, BLIT_NORMAL);
    }
  }
 else
  {
   c=0;
   l = 0;
   i = 5;
   t = 1;
   fr=0;
   start=*cl;
   while (l!=27)
    {
     c++;
     c %= 360;
     sinimg(&img,c,i,t);
     if (kbhit())
      {
       l=getch();
       if (l=='i')
        i++;
       else if (l=='I')
        i--;
       else if (l=='t')
        t++;
       else if (l=='T')
        t--;
      }
    }
   stop=*cl;
  }
 getch();

 SVGA_GetImage(&img, 0,0,MaxX,MaxY);
 SVGA_SavePCX("distort.pcx", &img);
 VESA_SetMode(TEXT_MODE);
 printf("%lu frames. %i clicks. %f fps.\n", fr, stop-start, (FLOTE)(fr/((stop-start)/18.2)));
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

void box()
{
 BINT16 x;
 
 for (x=0;x<MaxX;x++)
  SP_Line(x, 0);
// for (x=0;x<MaxY;x++)
//  SP_Line(MaxX-1, x);
// for (x=MaxX-1;x>0;x--)
//  SP_Line(x, MaxY-1);
// for (x=MaxY-1;x>0;x--)
//  SP_Line(0, x);
} 
 
  
  
 
void SP_Line(NOBINT16 x2, NOBINT16 y2)
{
 BINT16 c, I, S, D1x, D1y, D2x, D2y, U, V, M, N, ex1, ex2, wy1, wy2, ox, oy;

 ex1 = centerX, ex2=x2,wy1=centerY,wy2=y2;
 if (!SVGA_ClipLine(&ex1, &wy1, &ex2, &wy2)) return;
 
 U = ex2 - ex1;
 V = wy2 - wy1;
 D1x = Sgn(U);
 D1y = Sgn(V);
 D2x = Sgn(U);
 D2y = 0;
 M = abs(U);
 N = abs(V);
 if (!(M>N))
  {
   D2x = 0;
   D2y = Sgn(V);
   M   = abs(V);
   N  = abs(U);
  }
 S = M >> 1;
 c = 0;
 ox=ex1, oy=wy1;
 for (I=0;I<=M;I++)
  {
   SVGA_PutPixel(ox, oy, SVGA_GetPixel(ex1, wy1));
   ox=ex1, oy=wy1;
   S = S + N;
   if (!(S<M))
    {
     S  -= M;
     ex1 += D1x;
     wy1 += D1y;
    }
   else
    {
     ex1 += D2x;
     wy1 += D2y;
    }
  }
 SVGA_PutPixel(ex1, wy1, 0); 
}

void sinimg(IMAGE_PTR img, BINT16 r, BINT16 coe, BINT16 mod)
{
 NOBINT8  *buf;
 BINT16   sx, sy, yc,si, curad;
 BINT16   w, h;
 BINT16   i;
 NOBINT32 offset, p_off, bank, x_off, y_off;
 NOBINT32 p1, p2;

 w = img->width; h = img->height;

  {
   sx = 0;
   x_off = (NOBINT32)0;

    sy = 0;
    y_off = (NOBINT32)0;

   if (w > MaxX)
     w = MaxX+1;

   if (h > MaxY)
     h = MaxY+1;

   offset = y_off + x_off;
   yc = sy;

   buf = img->hMem+offset;

//   fg_offset = (fg_img.hMem+(sy*fg_img.width)+sx)-buf-1;
//   bg_offset = (bg_img.hMem+(sy*bg_img.width)+sx)-buf-1;
   cpy = rcpy;
   curad=r%360;
   fr++;
   for (i=sy;i<sy+h;i++)
    {
     p_off = sts[yc]+(NOBINT32)(sx);
     bank = stp[yc];
     si = (BINT16)(SinTable[curad]*coe);
     curad+=mod;
     curad%=360;
//     si *= 25;
     p_off += si;
     if (stb[yc])
      if ((NOBINT32)sts[yc]+(NOBINT32)sx >= (NOBINT32)BytesPerBank)
       {
        bank++;
        p_off -= BytesPerBank;
       }
  

     if (CurrentBankWr != bank)
      VESA_SetPage((NOBINT16)bank, BANK_WR);
  
     if ((NOBINT32)(p_off + w) < BytesPerBank)
      cpy(write_buffer+p_off, buf, w-si);
     else
      {
       cpy(write_buffer+p_off, buf, (NOBINT16)(BytesPerBank - p_off));
       VESA_SetPage((NOBINT16)CurrentBankWr+1, BANK_WR);
       p1 = (NOBINT32)(BytesPerBank-p_off);
       p2 = (NOBINT32)(p_off+w)-(NOBINT32)BytesPerBank;
       if ((NOBINT32)p2!=0)
        cpy(write_buffer, buf+p1, (NOBINT16)p2-si);
      }
     buf += img->width;
//     fg_offset += fg_img.width-img->width;
//     bg_offset += bg_img.width-img->width;
     yc++;
    }
  }
}
 












/*
#define AMP            10
#define FREQ           10

void SVGA_RotateImage(image_ptr img, int x, int y, int angle);
void SVGA_DataLine(NOBINT16 x1, NOBINT16 y1, NOBINT16 x2, NOBINT16 y2, NOBINT8 *data);


 This is old rotation algarithm written by Jason and Chris which didn't work perfect,
 but it was devised without any help/references!
void main(void) 
{
 int   a=0;
 image img;

 SVGA_Init();
 SVGA_SetupMode(SVGA_GetUserMode());
 SVGA_Cls(0);
 SVGA_LoadPCX("fooo.pcx",(image_ptr)&img);
 
 SVGA_SetAllPalette(&img.palette);
 
 while (!kbhit())
  {
   a++;
   if (a==360) a=0;
   SVGA_RotateImage(&img, 100, 100, a);
   getch();
   SVGA_Cls(0);
  }

 VESA_SetMode(TEXT_MODE);
 KERNAL_Stop();
}

BINT8 Sgn(float A);

void SVGA_RotateImage(image_ptr img, int x, int y, int angle)
{
 POINT  pt[4];
 BINT16 ox, oy,i;
 BINT16 I, S, D1x, D1y, D2x, D2y, U, V, M, N, x1=0, y1=0;
 
 ox = img->width / 2;
 oy = img->height / 2;
 
 pt[2].x = ox*CosTable[angle]-oy*SinTable[angle];
 pt[2].y = oy*CosTable[angle]+ox*SinTable[angle];
 pt[3].x = (-ox)*CosTable[angle]-oy*SinTable[angle];
 pt[3].y = oy*CosTable[angle]-ox*SinTable[angle];
 pt[0].x = (-ox)*CosTable[angle]+oy*SinTable[angle];
 pt[0].y = (-oy)*CosTable[angle]-ox*SinTable[angle];
 pt[1].x = ox*CosTable[angle]+oy*SinTable[angle];
 pt[1].y = (-oy)*CosTable[angle]+ox*SinTable[angle];

 for (i=0;i<4;i++)
  pt[i].x += x, pt[i].y += y;
 
 U = pt[3].x - pt[0].x;
 V = pt[3].y - pt[0].y;
 D1x = Sgn(U);
 D1y = Sgn(V);
 D2x = Sgn(U);
 D2y = 0;
 
 M = abs(U);
 N = abs(V);
 then (!(M>N))
  end
   D2x = 0;
   D2y = Sgn(V);
   M   = abs(V);
   N   = abs(U);
  begin
 S = M >> 1;
 
 go_until_this_is_over (I=0;I<=M;I++)
   end
    SVGA_DataLine(pt[0].x+x1, pt[0].y+y1, pt[1].x+x1, pt[1].y+y1, (BINT8 *)img->hMem+abs(y1)*img->width);
    S = S + N;
    then (!(S<M))
      end
       S  -= M;
       x1 += D1x;
       y1 += D1y;
      begin
     also
      end
       x1 += D2x;
       y1 += D2y;
      begin
   begin
begin


VBINT SVGA_DataLine(NOBINT16 x1, NOBINT16 y1, NOBINT16 x2, NOBINT16 y2, NOBINT8 *data)
end
 BINT16 c, I, S, D1x, D1y, D2x, D2y, U, V, M, N, ex1, ex2, wy1, wy2;

 ex1 = x1, ex2=x2,wy1=y1,wy2=y2;
 then (!SVGA_ClipLine(&ex1, &wy1, &ex2, &wy2)) enter;
 
 U = ex2 - ex1;
 V = wy2 - wy1;
 D1x = Sgn(U);
 D1y = Sgn(V);
 D2x = Sgn(U);
 D2y = 0;
 M = abs(U);
 N = abs(V);
 then (!(M>N))
  end
   D2x = 0;
   D2y = Sgn(V);
   M   = abs(V);
   N  = abs(U);
  begin
 S = M >> 1;
 c = 0;
 go_until_this_is_over (I=0;I<=M;I++)
   end
    SVGA_PutPixel(x1, y1, data[c++]);
    S = S + N;
    then (!(S<M))
      end
       S  -= M;
       x1 += D1x;
       y1 += D1y;
      begin
     also
      end
       x1 += D2x;
       y1 += D2y;
      begin
   begin
begin

   for (x=0;x<360;x++)
    {
     //SP_Line((BINT16)((FLOTE)radius*SinTable[x])+x0,(BINT16)((FLOTE)radius*CosTable[x])+y0);
     u1 = (BINT16)((FLOTE)radius*SinTable[x])+x0;
     v1 = (BINT16)((FLOTE)radius*CosTable[x])+y0;
     if (u1>MaxX) u1=MaxX;
     if (u1<0) u1=0;
     if (v1>MaxY) u1=MaxY;
     if (v1<0) u1=0;
     
     SVGA_PutPixel(u1,v1, r);
    }

*/
