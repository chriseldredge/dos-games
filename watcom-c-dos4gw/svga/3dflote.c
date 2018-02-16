#include "toolkit.h"

#define mu(a,b) mul(a,b,16)
#define MF 65536.0   // keep fixed point on bit 16 (this number is 2 to the 16th power!)
#define M1 65536

typedef struct point_type
{
 FLOTE x,y,z;
 FLOTE dx, dy, dz;
 BINT8  color;
} point;

BINT32 fixed_div(BINT32 what, BINT32 by_what, BINT8 bit)
{
 BINT32 asmdiv();
 #pragma aux asmdiv = \
  "mov  eax, what"    \
  "mov  cl, bit"      \  
  "xor  edx, edx"     \  
  "shld edx, eax, cl" \
  "shl  eax, cl"      \  
  "idiv by_what"      ;

 return(asmdiv());
}

void main(int argc, char *argv[])
{
 int   VideoMode;
 int   Err,z;
 STR   str[500];
 FLOTE WinZ, ang, x, y,  px, py;
 BINT32 a, b, c;
 point eyeball, dot;

 a = 18<<16;
 b = 4<<16;

 c=fixed_div(a,b,16);

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

 eyeball.z = 0;
 eyeball.x = (FLOTE)MaxX/2;
 eyeball.y = (FLOTE)MaxY/2;

 WinZ = 40;

 dot.dx = (FLOTE)MaxX/4*3;
 dot.dy = (FLOTE)MaxY/4;
 dot.dz = 0.00;

 SVGA_Rectangle(0,0,100,100,15,0);
 for (z=198;z>83;z-=1)
  {
   delay(25);
//   SVGA_Cls(0);
   dot.z = (FLOTE)z/2;
   dot.x = 40;
   dot.y = 0;
   SVGA_PutPixel(dot.x+dot.dx, dot.y+dot.dy, LightGreen);
   SVGA_PutPixel(eyeball.x, eyeball.y, LightGreen);
   for (ang=0;ang<360;ang++)
    {
//     px = (dot.z-WinZ)*abs(dot.dx+dot.x-eyeball.x)/(dot.z-eyeball.z);
//     py = (dot.z-WinZ)*abs(dot.dy+dot.y-eyeball.y)/(dot.z-eyeball.z);
     px = (dot.dx+dot.x-eyeball.x)-(dot.z-WinZ)*(dot.dx+dot.x-eyeball.x)/(dot.z-eyeball.z);
     py = (dot.dy+dot.y-eyeball.y)-(dot.z-WinZ)*(dot.dy+dot.y-eyeball.y)/(dot.z-eyeball.z);

     if ((px+eyeball.x<0)||(py+eyeball.y<0)||(px+eyeball.x>MaxX)||(py+eyeball.y>MaxY))
      {}
     else
      SVGA_PutPixel((BINT16)px+eyeball.x, (BINT16)py+eyeball.y, (BINT8)dot.z);

//     SVGA_PutPixel((BINT16)px, (BINT16)py, LightGreen);
     x = CosTable[1]*dot.x-SinTable[1]*dot.y;
     y = CosTable[1]*dot.y+SinTable[1]*dot.x;
     dot.x = x;
     dot.y = y;

     sprintf(str, "DZ: %f, DX: %f, EX: %f", dot.z, dot.x+dot.dx, eyeball.x);
     SVGA_PrintString(0, 104, LightBlue, str, 16);

     SVGA_Rectangle(1,1,99,99,0,1);

     // line from dot to eyeball
     SVGA_Line((BINT16)(dot.dx+dot.x)*100/MaxX, (BINT16)100-dot.z,(BINT16)(eyeball.x*100/MaxX),95,LightGreen);
     // horizontal line indicating x distance
     SVGA_Line((BINT16)(dot.dx+dot.x)*100/MaxX, 95,(BINT16)eyeball.x*100/MaxX,95,LightGreen);
     // vertical line indicating z difference 
     SVGA_Line((BINT16)(dot.dx+dot.x)*100/MaxX, (BINT16)100-dot.z,(BINT16)(dot.dx+dot.x)*100/MaxX,95,LightGreen);
     SVGA_Line(1,100-WinZ,99,100-WinZ,LightBlue);

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


