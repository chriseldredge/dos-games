#include "toolkit.h"

#define mu(a,b) mul(a,b,16)
#define di(a,b) fixed_div(a,b,16)
#define MF 65536.0   // keep fixed point on bit 16 (this number is 2 to the 16th power!)
#define M1 65536

typedef struct point_type
{
 BINT32 x,y,z;
 BINT32 dx, dy, dz;
 BINT8  color;
} point;

BINT32 fixed_div(BINT32 what, BINT32 by_what, BINT8 bit)
{
 BINT32 asmdiv();
 #pragma aux asmdiv = \
  "cmp  by_what, 0"   \
  "jz end"            \
  "mov  eax, what"    \
  "xor  edx, edx"     \  
  "mov  cl, bit"      \
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

void main(int argc, char *argv[])
{
 int   VideoMode;
 int   Err,z;
 STR   str[500];
 BINT32 WinZ, ang, x, y,  px, py,a,b,c,d, co, si;
 FLOTE f;
 point eyeball, dot;
 
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

 f=SinTable[1]*65536;
 si = (BINT32) f;
 f=CosTable[1]*65536;
 co = (BINT32) f;

 eyeball.z = 0;
 eyeball.x = (MaxX/2) << 16;
 eyeball.y = (MaxY/2) << 16;

 WinZ = 40 << 16;

 dot.dx = MaxX/4*3 << 16;
 dot.dy = MaxY/4 << 16;
 dot.dz = 0;

 SVGA_Rectangle(0,0,100,100,15,0);
 for (z=198;z>83;z-=1)
  {
   delay(25);
//   SVGA_Cls(0);
   dot.z = z<<16;
   dot.z /= 2;
   dot.x = 40<<16;
   dot.y = 0;
   SVGA_PutPixel((dot.x+dot.dx) >> 16, (dot.y+dot.dy)>>16, LightGreen);
   SVGA_PutPixel(eyeball.x>>16, eyeball.y>>16, LightGreen);
   for (ang=0;ang<360;ang++)
    {
//     px = (dot.z-WinZ)*abs(dot.dx+dot.x-eyeball.x)/(dot.z-eyeball.z);
//     py = (dot.z-WinZ)*abs(dot.dy+dot.y-eyeball.y)/(dot.z-eyeball.z);
     a = dot.z-WinZ;
     b = dot.dx+dot.x-eyeball.x;
     c = mu(a,b);
     d = dot.z-eyeball.z;

     px = (dot.dx+dot.x-eyeball.x)-di(c,d);
     b = dot.dy+dot.y-eyeball.y;
     c = mu(a,b);
     
     py = (dot.dy+dot.y-eyeball.y)-di(c,d);

//     if (((px+eyeball.x)>>16<0)||((py+eyeball.y)>>16<0)||((px+eyeball.x)>>16>MaxX)||((py+eyeball.y)>>16>MaxY))
//      {}
//     else
      a = px+eyeball.x;
      b = py+eyeball.y;
      SVGA_PutPixel(a>>16, b>>16, z);
//     SVGA_PutPixel((px+eyeball.x)>>16, MaxY-30, (BINT8)dot.z>>16);
//     SVGA_PutPixel((BINT16)px, (BINT16)py, LightGreen);
     x = mu(co,dot.x)-mu(si,dot.y);
     y = mu(co,dot.y)+mu(si,dot.x);
     dot.x = x;
     dot.y = y;

     sprintf(str, "DZ: %i, DX: %i, EX: %i  ", dot.z>>16, (dot.x+dot.dx)>>16, eyeball.x>>16);
     SVGA_PrintString(0, 104, LightBlue, str, 16);
     sprintf(str, "DZ: %i, DY: %i, EY: %i  ", dot.z>>16, (dot.y+dot.dy)>>16, eyeball.y>>16);
     SVGA_PrintString(0, 114, LightBlue, str, 16);
     sprintf(str, "PX: %i, PY: %i          ", px>>16, py>>16);
     SVGA_PrintString(0, 124, LightBlue, str, 16);

     SVGA_Rectangle(1,1,99,99,0,1);

     // line from dot to eyeball
//     SVGA_Line((BINT16)(dot.dx+dot.x)>>16*100/MaxX, (BINT16)100-dot.z,(BINT16)(eyeball.x*100/MaxX),95,LightGreen);
     // horizontal line indicating x distance
//     SVGA_Line((BINT16)(dot.dx+dot.x)*100/MaxX, 95,(BINT16)eyeball.x*100/MaxX,95,LightGreen);
     // vertical line indicating z difference 
//     SVGA_Line((BINT16)(dot.dx+dot.x)*100/MaxX, (BINT16)100-dot.z,(BINT16)(dot.dx+dot.x)*100/MaxX,95,LightGreen);
//     SVGA_Line(1,100-WinZ,99,100-WinZ,LightBlue);

     if (kbhit()) break;
//     getch();
    }
// if (getch()=='q') getch();
   if (kbhit()) break;
  }
 getch();
 getch();

 VESA_SetMode(TEXT_MODE);
}


