/***********************************************************************/
/* --------Sat-8/17/96                                                 */
/* My little All-In-One MSVC morphing demo/library. Some declarations  */
/* taken from "FPLAY.C" from Tricks of The Graphics Gurus. Mostly all  */
/* modified in one way or another.  All original outside of macros and */
/* some structs.                                                       */
/* --------Sun-8/18/96                                                 */
/* What posessed me to use floating point numbers???? ack!!            */
/* Redid all previous functions to use long integers rather than       */
/* floating point numbers. Speed increased dramatically. Rotate still  */
/* uses a floating point angle, but with radians for input I do not    */
/* see how this is very avoidable while maintaining smooth rotation.   */
/* Added Fill to Polygons, but slows down program significantly. Need  */
/* Watcom C & Chris's files.                                           */
/* --------Mon-8/19/96                                                 */
/* All done. Polygon function works now. doesn't like really large     */
/* polygons  though, causes a stack overflow. Need to fix in the       */
/* future. Time to start writing 3-D lib now. Also started my clip-box */
/* collision detection. Though not particularly useful, interesting.   */
/* --------Tue-8/27/96 before 12:00pm                                  */
/* Created sine and cosine tables to speed calculations. Modified      */
/* floating angles, so all rotations are now integral.                 */
/*                                                                     */
/***********************************************************************/ 

#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>

/* A file with some useful macros. Taken from TGG. */

#include "mouse.h"
#include "svga.h"

/***********************************************************************/
// To header
#define PI                      3.141592654

// To move
#define CMaxX                    (((BINT32)clipbox.pt[3].x >> 16)+clipbox.head.cx)
#define CMinX                    (((BINT32)clipbox.pt[1].x >> 16)+clipbox.head.cx)
#define CMaxY                    (((BINT32)clipbox.pt[3].y >> 16)+clipbox.head.cy)
#define CMinY                    (((BINT32)clipbox.pt[1].y >> 16)+clipbox.head.cy)

#define GRAVITY                 0

#define MAX_POINTS              10
#define MAX_OBJECTS             10
#define NUM                     10     

/*************************************************************************/

typedef struct {
        BINT32   x;
        BINT32   y;
} POINT, *POINT_PTR;

/* The object header structure */
typedef struct {
        BINT16     angle;
        BINT16     color;
        BINT16     fillcolor;
        BINT16     points;
        BINT16     cx;
        BINT16     cy;
        POINT   velocity;
} OBJECT_HDR, *OBJECT_HDR_PTR;

typedef struct {
        BINT16  points;
        POINT   pt[MAX_POINTS];
} DELTA_OBJ, *DELTA_OBJ_PTR;

typedef struct {
        DELTA_OBJ       obj[MAX_OBJECTS];
} DELTA, *DELTA_PTR;

typedef struct {
        OBJECT_HDR      head;
        POINT           pt[MAX_POINTS];
} OBJECT, *OBJECT_PTR;


/*************************************************************************/
/* Prototypes */

void    InitMathTables(void);

void    DrawObject(OBJECT o,
                   int Erase);
                 

void    ScaleObject(    OBJECT_PTR o, 
                        BINT32    sx,
                        BINT32    sy);

void    MoveObject(     OBJECT_PTR o,
                        BINT32    dx,
                        BINT32    dy,
                        int     clip,
                        OBJECT  box);
    
void    RotateObject(   OBJECT_PTR o, 
                        BINT16  Angle);

void    Polygon(        OBJECT_PTR o,
                        BINT16     points, 
                        BINT32    x, 
                        BINT32    n, 
                        BINT32    radius);

/*************************************************************************/
double SinTable[360];
double CosTable[360];

void main() {

OBJECT  fred[NUM];
OBJECT  box, screen;
long    y,i;
                       
                            /* INIT OBJECTS */
  
  printf("\nInitializing math tables...");
  InitMathTables();
  printf("done\n");
  
  SVGA_Init();
  SVGA_SetupMode(SVGA_GetUserMode());
  SVGA_Line(0,0,10,10,50);
  getch();
  screen.head.cx=0;
  screen.head.cy=0;
  screen.head.points=4;
  screen.head.color=15;
  screen.head.fillcolor=0;
  screen.pt[1].x = 0;
  screen.pt[1].y = 0; 
  screen.pt[2].x = ((BINT32)(MaxX-1) << 16);
  screen.pt[2].y = 0;
  screen.pt[3].x = ((BINT32)(MaxX-1) << 16);
  screen.pt[3].y = ((BINT32)(MaxY-1) << 16);
  screen.pt[0].x = 0;
  screen.pt[0].y = ((BINT32)(MaxY-1) << 16);
     
  DrawObject(screen, 0);

  getch();
  
  Polygon(&box, 4, MaxX/2, MaxY/2, MaxY/2);

  RotateObject(&box, 45);
  box.head.color = 50;      
  box.head.fillcolor = 0;
  box.head.velocity.x = ((BINT32)3 << 16);
  box.head.velocity.y = ((BINT32)3 << 16);

for(y=0;y<NUM;y++) 
  {
  fred[y].head.velocity.x = ((BINT32)(4-y) << 16);
  fred[y].head.velocity.y = ((BINT32)(y/2+2) << 16);
  fred[y].head.color = y*20+20;
  fred[y].head.points      = 3;
  fred[y].head.fillcolor   = 0;
  fred[y].head.angle       = 0;
  fred[y].head.cx    = MaxX/2;
  fred[y].head.cy    = MaxY/2;
  Polygon(&fred[y], 3, MaxX/2, MaxY/2, 5*y + 5);
  }  
        /***************************/
        /* begin main program loop */
        /***************************/
  DrawObject(box, 0);
  while(!kbhit())
   {
    DrawObject(box, 1);
    MoveObject((OBJECT_PTR)&box, box.head.velocity.x, box.head.velocity.y, 1, screen);
    DrawObject(box, 0);
    for(i=0;i<NUM;i++)        
     {
      DrawObject(fred[i], 1);     /*erase object*/
      RotateObject((OBJECT_PTR)&fred[i], (BINT16)(20*i-25));
      MoveObject((OBJECT_PTR)&fred[i], fred[i].head.velocity.x, fred[i].head.velocity.y, 1, box);
      DrawObject(fred[i], 0);   /*redraw object*/  
     }
   }

  VESA_SetMode(TEXT_MODE);
}
/***************************************************************************/
void InitMathTables(void) {
 int i=0;
 go_until_this_is_over (i=0;i<360;i++)
  {
   CosTable[i] = cos((i*PI)/180);
   SinTable[i] = sin((i*PI)/180);
  }
}
/***************************************************************************/
void DrawObject(OBJECT o, int Erase) {
  
int i;
int color;


  if (Erase) 
    color = 0;
  else    
    color = o.head.color;
  
  SVGA_Line((BINT16)o.head.cx + (BINT16)(o.pt[0].x >> 16), 
            (BINT16)o.head.cy + (BINT16)(o.pt[0].y >> 16),
            (BINT16)o.head.cx + (BINT16)(o.pt[o.head.points - 1].x >> 16),
            (BINT16)o.head.cy + (BINT16)(o.pt[o.head.points - 1].y >> 16),
            color);

  for(i=0;i<o.head.points-1;i++) 
   SVGA_Line((BINT16)o.head.cx + (BINT16)(o.pt[i+1].x >> 16), 
             (BINT16)o.head.cy + (BINT16)(o.pt[i+1].y >> 16),
             (BINT16)o.head.cx + (BINT16)(o.pt[i].x >> 16),
             (BINT16)o.head.cy + (BINT16)(o.pt[i].y >> 16),
              color); 

}
/***************************************************************************/
void ScaleObject(OBJECT_PTR o, BINT32 sx, BINT32 sy) {

int i;

  for(i=0;i<o->head.points;i++)
    {
    o->pt[i].x *= sx;
    o->pt[i].y *= sy;
    }
}
/***************************************************************************/
void MoveObject( OBJECT_PTR o, BINT32 dx, BINT32 dy, int clip, OBJECT clipbox) {

BINT16 i, cx=0, cy=0, xx, yy;

  for(i=0;i<o->head.points;i++) 
    {
     xx = (o->pt[i].x >> 16) + o->head.cx + (dx >> 16);
     
     if (xx > CMaxX)
      if (cx<xx) cx = xx; 
     if (xx < CMinX)
      if ((cx>xx)||(cx==0)) cx = xx;
          
     yy = (o->pt[i].y >> 16) + o->head.cy + (dy >> 16);

     if (yy > CMaxY)
      if (cy<yy) cy = yy; 
     if (yy < CMinY)
      if ((cy>yy)||(cy==0)) cy = yy;
    }

    if (cx)
     {
      o->head.velocity.x = 0 - o->head.velocity.x + clipbox.head.velocity.x;
      if (cx>CMaxX)
       o->head.cx -= (cx-CMaxX);
      else
       o->head.cx -= (cx-CMinX);
     }  
    else
      o->head.cx += (dx >> 16);

    if (cy)
     {
      o->head.velocity.y = 0 - o->head.velocity.y + clipbox.head.velocity.y;
      if (cy>CMaxY)
        o->head.cy -= (cy-CMaxY);
      else
        o->head.cy -= (cy-CMinY);
     }
    else
      o->head.cy += (dy >> 16);
}
/***************************************************************************/
void RotateObject( OBJECT_PTR o, BINT16 Angle) {

BINT16     i;
BINT32     x, y;
double    si, co;

 if (Angle<0)
  {
   si=SinTable[Angle+360];
   co=CosTable[Angle+360];
  }
 else
  {
   si=SinTable[Angle];
   co=CosTable[Angle];
  }
 
  if (Angle + o->head.angle > 360)
    o->head.angle = o->head.angle + Angle - 360;
  else 
    o->head.angle += Angle;

  for (i=0;i<o->head.points;i++)  
   {                                    
    x = o->pt[i].x;            
    y = o->pt[i].y;
                                                                           
    o->pt[i].x = co*x - si*y;  
    o->pt[i].y = co*y + si*x; 
   }                                        //                               
   ///////////////////////                 //
   // [ sinA -cosA]*[x] //                //
   // [ cosA  sinA] [y] //  <============// 
   ///////////////////////
}
/************************************************************************/
void Polygon( OBJECT_PTR o, BINT16 points, BINT32 x, BINT32 n, BINT32 radius) {

BINT16   i;
BINT16   r;

  o->head.angle         = 0;
  o->head.points        = points;
  o->head.cx      = x;
  o->head.cy      = n;
  o->pt[0].x            = 0;
  o->pt[0].y            = ((BINT32)radius << 16);
  
  r = 360/points;
   
  for(i=1;i<points;i++)
    {
    o->pt[i].x = ((BINT32)(-SinTable[r*i]*radius) << 16);   
    o->pt[i].y = ((BINT32)(CosTable[r*i]*radius) << 16); 
    }
}                                               
