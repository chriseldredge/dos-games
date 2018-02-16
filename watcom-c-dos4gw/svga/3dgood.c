#include "toolkit.h"

#define bit_shift 15
#define bit_const 32768
#define mu(a,b) mul(a,b,bit_shift)
#define di(a,b) fixed_div(a,b,bit_shift)

#define WinZ (360 << bit_shift)

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
   a = obj->center.oz+obj->point[count].z-WinZ;
   b = obj->center.ox+obj->point[count].x-eyeball.ox;
   c = mu(a,b);
   d = obj->center.oz+obj->point[count].z-eyeball.oz;
   px = (obj->center.ox+obj->point[count].x-eyeball.ox)-di(c,d);

   b = obj->center.oy+obj->point[count].y-eyeball.oy;
   c = mu(a,b);
   py = (obj->center.oy+obj->point[count].y-eyeball.oy)-di(c,d);

   px += eyeball.ox;
   py += eyeball.oy;

   if (obj->point[count].numconnect)
    {
     for (nc=0;nc<obj->point[count].numconnect;nc++)
      {
       dot = (POINT3D *)obj->point[count].connect[nc];
       a = obj->center.oz+dot->z-WinZ;
       b = obj->center.ox+dot->x-eyeball.ox;
       c = mu(a,b);
       d = obj->center.oz+dot->z-eyeball.oz;
       px2 = (obj->center.ox+dot->x-eyeball.ox)-di(c,d);
       b = obj->center.oy+dot->y-eyeball.oy;
       c = mu(a,b);
       py2 = (obj->center.oy+dot->y-eyeball.oy)-di(c,d);

       px2 += eyeball.ox;
       py2 += eyeball.oy;
       SVGA_Line(px>>bit_shift,py>>bit_shift,px2>>bit_shift,py2>>bit_shift, color);
      }
    }
   else
    SVGA_PutPixel(px>>bit_shift,py>>bit_shift, color);
  }
}

void obj3d_rotate(OBJ3D *obj, BINT16 ang)
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

void main(int argc, char *argv[])
{
 int   VideoMode;
 int   Err,count;
 STR   str[500];
 OBJ3D obj;

 str[0] = 0;

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

 obj.center.ox = (MaxX+1)/2 << bit_shift;
 obj.center.oy = (MaxY+1)/2 << bit_shift;

 obj.numpoints = 8;
 obj.point[0].x = -120 << bit_shift;
 obj.point[0].y = -120 << bit_shift;
 obj.point[0].z = -120 << bit_shift;
 obj.point[0].numconnect = 1;
 obj.point[0].connect[0] = (BINT32 *)&obj.point[1];
 obj.point[1].x = -120 << bit_shift;
 obj.point[1].y =  120 << bit_shift;
 obj.point[1].z = -120 << bit_shift;
 obj.point[1].numconnect = 1;
 obj.point[1].connect[0] = (BINT32 *)&obj.point[2];
 obj.point[2].x =  120 << bit_shift;
 obj.point[2].y =  120 << bit_shift;
 obj.point[2].z = -120 << bit_shift;
 obj.point[2].numconnect = 1;
 obj.point[2].connect[0] = (BINT32 *)&obj.point[3];
 obj.point[3].x =  120 << bit_shift;
 obj.point[3].y = -120 << bit_shift;
 obj.point[3].z = -120 << bit_shift;
 obj.point[3].numconnect = 1;
 obj.point[3].connect[0] = (BINT32 *)&obj.point[0];
 obj.point[4].x = -120 << bit_shift;
 obj.point[4].y = -120 << bit_shift;
 obj.point[4].z =  120 << bit_shift;
 obj.point[4].numconnect = 2;
 obj.point[4].connect[0] = (BINT32 *)&obj.point[5];
 obj.point[4].connect[1] = (BINT32 *)&obj.point[0];
 obj.point[5].x = -120 << bit_shift;
 obj.point[5].y =  120 << bit_shift;
 obj.point[5].z =  120 << bit_shift;
 obj.point[5].numconnect = 2;
 obj.point[5].connect[0] = (BINT32 *)&obj.point[6];
 obj.point[5].connect[1] = (BINT32 *)&obj.point[1];
 obj.point[6].x =  120 << bit_shift;
 obj.point[6].y =  120 << bit_shift;
 obj.point[6].z =  120 << bit_shift;
 obj.point[6].numconnect = 2;
 obj.point[6].connect[0] = (BINT32 *)&obj.point[7];
 obj.point[6].connect[1] = (BINT32 *)&obj.point[2];
 obj.point[7].x =  120 << bit_shift;
 obj.point[7].y = -120 << bit_shift;
 obj.point[7].z =  120 << bit_shift;
 obj.point[7].numconnect = 2;
 obj.point[7].connect[0] = (BINT32 *)&obj.point[4];
 obj.point[7].connect[1] = (BINT32 *)&obj.point[3];

 eyeball.oz = 0;
 eyeball.ox = (MaxX/2) << bit_shift;
 eyeball.oy = (MaxY/2) << bit_shift;

 while (!kbhit())
  {
   obj.center.oz = 400<<bit_shift;
   obj3d_show(&obj,LightRed);
   for (count=0;count<360;count++)
    {
     delay(20);
     obj3d_show(&obj,Black);
     obj3d_rotate(&obj,1);
     obj3d_show(&obj,LightRed);
    }
  }

 getch();

 VESA_SetMode(TEXT_MODE);
}


