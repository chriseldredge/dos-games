#include "toolkit.h"

#define bit_shift 16
#define bit_const 65536
#define mu(a,b) mul(a,b,bit_shift)
#define di(a,b) fixed_div(a,b,bit_shift)

#define WinZ (240 << bit_shift)

typedef struct point_type
{
 BINT32 x,y,z;
 BINT32 ox, oy, oz;
 BINT16 numconnect;
 BINT32 *connect[3];
 BINT8  color;
} POINT3D;

typedef struct obj3d_type
{
 BINT16   numpoints;
 POINT3D  center;
 POINT3D  point[10];
} OBJ3D;

VBINT obj3d_line(NOBINT16 x1, NOBINT16 y1, NOBINT16 x2, NOBINT16 y2, BINT32 z1, BINT32 z2, BINT8 colf);

BINT32 fixed_div(BINT32 what, BINT32 by_what, BINT8 bit)
{
 BINT32 asmdiv();
 #pragma aux asmdiv = \
  "cmp by_what, 0"   \
  "jz  end"            \
  "mov eax, what"    \
  "xor edx, edx"     \  
  "mov cl, bit"      \
  "cmp eax, 0"        \
  "jns nofix"         \
  "cdq"               \
  "nofix:"            \
  "shld edx, eax, cl" \
  "shl  eax, cl"      \  
  "idiv by_what"      \
  "end:"              ;

 return(asmdiv());
}

POINT3D eyeball;

void obj3d_show(OBJ3D *obj, NOBINT32 color)
{
 BINT16  count,nc;
 BINT32  px,py,px2,py2,a,b,c,d;
 POINT3D *dot;

 for (count=0;count<obj->numpoints;count++)
  {
/*
  A
  |\         AD     DE           
  | \        --  =  -- therefore AD*BC/AB = DE or AD/AB*BC = DE
  |  \       AB     BC
 D|---\E
  |    \
  |-----\
  B      C
*/
   a = obj->center.oz+obj->point[count].z-WinZ;
   b = obj->center.ox+obj->point[count].x-eyeball.ox;
   d = obj->center.oz+obj->point[count].z-eyeball.oz;
   c = di(a,d);
   c = mu(b,c);
   px = obj->center.ox+obj->point[count].x-c;

   b = obj->center.oy+obj->point[count].y-eyeball.oy;
   c = di(a,d);
   c = mu(b,c);
   py = obj->center.oy+obj->point[count].y-c;

   if (obj->point[count].numconnect)
    {
     for (nc=0;nc<obj->point[count].numconnect;nc++)
      {
       dot = (POINT3D *)obj->point[count].connect[nc];
       a = obj->center.oz+dot->z-WinZ;
       b = obj->center.ox+dot->x-eyeball.ox;
       d = obj->center.oz+dot->z-eyeball.oz;
       c = di(a,d);
       c = mu(b,c);
       px2 = (obj->center.ox+dot->x)-c;
       b = obj->center.oy+dot->y-eyeball.oy;
       c = di(a,d);
       c = mu(b,c);
       py2 = obj->center.oy+dot->y-c;

       obj3d_line(px>>bit_shift,py>>bit_shift,px2>>bit_shift,py2>>bit_shift,obj->point[count].z+obj->center.oz, dot->z+obj->center.oz, !color);
      }
    }
   else
    SVGA_PutPixel(px>>bit_shift,py>>bit_shift, !color);
  }
}

void obj3d_rotateXZ(OBJ3D *obj, BINT16 ang)
{
 BINT32 x,y,z,si,co;
 BINT16 count;
 
 si = SinTable[abs(ang)%360] * bit_const;
 co = CosTable[abs(ang)%360] * bit_const;
 y = 0;
 for (count=0;count<obj->numpoints;count++)
  {
   x = mu(co,obj->point[count].x)-mu(si,obj->point[count].z);
   z = mu(co,obj->point[count].z)+mu(si,obj->point[count].x);
   obj->point[count].x = x;
   obj->point[count].z = z;
  }
}
void obj3d_rotateXY(OBJ3D *obj, BINT16 ang)
{
 BINT32 x,y,z,si,co;
 BINT16 count;
 
 si = SinTable[abs(ang)%360] * bit_const;
 co = CosTable[abs(ang)%360] * bit_const;
 y = 0;
 for (count=0;count<obj->numpoints;count++)
  {
   x = mu(co,obj->point[count].x)-mu(si,obj->point[count].y);
   y = mu(co,obj->point[count].y)+mu(si,obj->point[count].x);
   obj->point[count].x = x;
   obj->point[count].y = y;
  }
}
void obj3d_rotateYZ(OBJ3D *obj, BINT16 ang)
{
 BINT32 x,y,z,si,co;
 BINT16 count;
 
 si = SinTable[abs(ang)%360] * bit_const;
 co = CosTable[abs(ang)%360] * bit_const;
 y = 0;
 for (count=0;count<obj->numpoints;count++)
  {
   y = mu(co,obj->point[count].y)-mu(si,obj->point[count].z);
   z = mu(co,obj->point[count].z)+mu(si,obj->point[count].y);
   obj->point[count].y = y;
   obj->point[count].z = z;
  }
}

void main(int argc, char *argv[])
{
 RGB   c;
 int   VideoMode;
 int Err,count;
 BINT32 step;
 STR   str[500];
 OBJ3D obj;
 IMAGE img1,img2;
 
 str[0] = 0;

 Err = SVGA_Init();
 if (!Err) 
  {
   printf("VESA compliant video card is not present. Load your VESA driver!\n");
   exit(1);
  }

// VideoMode = SVGA_GetUserMode(MODE_32_BIT);

 if (SVGA_SetupMode(0x101))
  {
   printf("Failure setting video mode!\n");
   exit(2);
  }
 for (count=0;count<64;count++)
  {
   c.red = count;
   c.green = count;
   c.blue = count;
   SVGA_SetPalette(count,&c);
  }

 obj.center.ox = (MaxX+1)/2 << bit_shift;
 obj.center.oy = (MaxY+1)/2 << bit_shift;

 obj.numpoints = 8;
 obj.point[0].x = -40 << bit_shift;
 obj.point[0].y = -40 << bit_shift;
 obj.point[0].z = -40 << bit_shift;
 obj.point[0].numconnect = 1;
 obj.point[0].connect[0] = (BINT32 *)&obj.point[1];
 obj.point[1].x = -40 << bit_shift;
 obj.point[1].y =  40 << bit_shift;
 obj.point[1].z = -40 << bit_shift;
 obj.point[1].numconnect = 1;
 obj.point[1].connect[0] = (BINT32 *)&obj.point[2];
 obj.point[2].x =  40 << bit_shift;
 obj.point[2].y =  40 << bit_shift;
 obj.point[2].z = -40 << bit_shift;
 obj.point[2].numconnect = 1;
 obj.point[2].connect[0] = (BINT32 *)&obj.point[3];
 obj.point[3].x =  40 << bit_shift;
 obj.point[3].y = -40 << bit_shift;
 obj.point[3].z = -40 << bit_shift;
 obj.point[3].numconnect = 1;
 obj.point[3].connect[0] = (BINT32 *)&obj.point[0];
 obj.point[4].x = -40 << bit_shift;
 obj.point[4].y = -40 << bit_shift;
 obj.point[4].z =  40 << bit_shift;
 obj.point[4].numconnect = 2;
 obj.point[4].connect[0] = (BINT32 *)&obj.point[5];
 obj.point[4].connect[1] = (BINT32 *)&obj.point[0];
 obj.point[5].x = -40 << bit_shift;
 obj.point[5].y =  40 << bit_shift;
 obj.point[5].z =  40 << bit_shift;
 obj.point[5].numconnect = 2;
 obj.point[5].connect[0] = (BINT32 *)&obj.point[6];
 obj.point[5].connect[1] = (BINT32 *)&obj.point[1];
 obj.point[6].x =  40 << bit_shift;
 obj.point[6].y =  40 << bit_shift;
 obj.point[6].z =  40 << bit_shift;
 obj.point[6].numconnect = 2;
 obj.point[6].connect[0] = (BINT32 *)&obj.point[7];
 obj.point[6].connect[1] = (BINT32 *)&obj.point[2];
 obj.point[7].x =  40 << bit_shift;
 obj.point[7].y = -40 << bit_shift;
 obj.point[7].z =  40 << bit_shift;
 obj.point[7].numconnect = 2;
 obj.point[7].connect[0] = (BINT32 *)&obj.point[4];
 obj.point[7].connect[1] = (BINT32 *)&obj.point[3];

 eyeball.oz = 0;
 eyeball.ox = (MaxX/2) << bit_shift;
 eyeball.oy = (MaxY/2) << bit_shift;

 step = -2 << bit_shift;

 obj.center.oz = 297<<bit_shift;
 obj.center.ox = 320<<bit_shift;
 obj.center.oy = 240<<bit_shift;

 SVGA_LoadPCX("fade1.pcx",&img1);
 SVGA_LoadPCX("fade2.pcx",&img2);
 
 for (step=0;step<360;step+=2)
  {
   obj3d_show(&obj,LightRed);
  }
 
// obj.center.ox = 590<<bit_shift;
// obj.center.oy = 430<<bit_shift;

// obj3d_show(&obj,LightRed);

 getch();
 
 
/* for (count=0;count<360;count+=2)
  {
   delay(15);   
   obj3d_show(&obj,Black);
   obj3d_rotateXY(&obj,2);
   obj3d_show(&obj,LightRed);
  } */  

 obj.center.oz = 400<<bit_shift;
 obj.center.ox = (MaxX+160)/2 << bit_shift;
 obj.center.oy = (MaxY+160)/2 << bit_shift;

 step = 2<<bit_shift;

 while (!kbhit())
  {
   for (count=0;count<360;count++)
    {
     delay(15);
     obj3d_show(&obj,Black);
     //obj.center.oz += step;
     if ((obj.center.oz>2000<<bit_shift)||(obj.center.oz<580<<bit_shift))
      step = -step;
     obj3d_rotateXY(&obj,1);
//     obj3d_rotateXZ(&obj,1);
     obj3d_rotateYZ(&obj,1);
     obj3d_show(&obj,LightRed);
    }
  }

 getch();

 VESA_SetMode(TEXT_MODE);
}

NOBINT32 gray(BINT8 b)
{
 NOBINT32 asmgray();
 #pragma aux asmgray=\
  "xor eax, eax"     \
  "mov ah, b"        \
  "mov al, ah"       \
  "shl eax, 8"       \
  "mov al, ah"       ;

 //return(asmgray());
 return(b);
}

VBINT obj3d_line(NOBINT16 x1, NOBINT16 y1, NOBINT16 x2, NOBINT16 y2, BINT32 z1, BINT32 z2, BINT8 colf)
{
 BINT16 I, S, D1x, D1y, D2x, D2y, U, V, M, N, ex1, ex2, wy1, wy2;
 FLOTE ack;
 BINT32 zd, sz, cz, nz;
 ex1 = x1, ex2=x2,wy1=y1,wy2=y2;

// if (!SVGA_ClipLine(&ex1, &wy1, &ex2, &wy2)) enter;
 if (z1==z2)
  zd = 0;
 else
  zd = (z1-z2);
  
 sz = z1;
 ack = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1))*bit_const;
 cz = ack;
 if (zd==0)
  cz = 0;
 else
  cz = di(zd, cz);

 nz = sz;

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

 go_until_this_is_over (I=0;I<=M;I++)
   {
    sz = (63<<bit_shift)-di(nz-WinZ, 31.25 * bit_const);
    if (colf)
     SVGA_PutPixel(x1, y1, 0);
    else
     SVGA_PutPixel(x1, y1, gray(sz>>bit_shift));
    nz -= cz;
    S = S + N;
    if (!(S<M))
      {
       S  -= M;
       x1 += D1x;
       y1 += D1y;
      }
     else
      {
       x1 += D2x;
       y1 += D2y;
      }
   }
}

