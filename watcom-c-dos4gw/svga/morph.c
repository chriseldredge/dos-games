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
/* --------Later                                                       */
/* Rewrote all code in Watcom C & DOS4GW compatible format. Fixed bugs */
/* In clipping and spent a lot of time doing it. Wrote morph function. */
/*                                                                     */
/*                                                                     */
/***********************************************************************/ 

#include <stdio.h>
#include <math.h>

#include "toolkit.h"

void main() {

OBJECT  fred[15];
OBJECT  box, box2, box3, box4, box5, screen;
int    NUM;
long    y;
int     oo=0,i=0;
                       
                            /* INIT OBJECTS */
  
  printf("\nEnter the number of objects: ");
  scanf("%i", &NUM);
  
  SVGA_Init();
  SVGA_SetupMode(SVGA_GetUserMode());

  screen.head.c.x = 0;
  screen.head.c.y = 0;
  screen.head.points=4;
  screen.head.color=15;
  screen.head.fillcolor=0;
  screen.head.velocity.x = 0;
  screen.head.velocity.y = 0; 
  screen.pt[1].x = 0;
  screen.pt[1].y = 0; 
  screen.pt[2].x = ((BINT32)(MaxX) << 16);
  screen.pt[2].y = 0;
  screen.pt[3].x = ((BINT32)(MaxX) << 16);
  screen.pt[3].y = ((BINT32)(MaxY) << 16);
  screen.pt[0].x = 0;
  screen.pt[0].y = ((BINT32)(MaxY) << 16);
  
  SVGA_MakeRegPoly(&box, 8, MaxX/2, MaxY/2, 100);
  SVGA_MakeRegPoly(&box2, 4, MaxX/2, MaxY/2, 50);
  SVGA_MakeRegPoly(&box3, 8, MaxX/2, MaxY/2, 50);

  SVGA_RotateObject(&box2, 45);
  box3.head.color = 50;
  box3.pt[0] = box2.pt[0];
  box3.pt[1].x = box2.pt[0].x; 
  box3.pt[1].y = (box2.pt[1].y + box2.pt[0].y)/2;
  box3.pt[2] = box2.pt[1];
  box3.pt[3].x = (box2.pt[2].x + box2.pt[1].x)/2;
  box3.pt[3].y = box2.pt[1].y;
  box3.pt[4] = box2.pt[2];
  box3.pt[5].x = box2.pt[2].x; 
  box3.pt[5].y = (box2.pt[3].y + box2.pt[2].y)/2;
  box3.pt[6] = box2.pt[3];
  box3.pt[7].x = (box2.pt[3].x + box2.pt[0].x)/2;
  box3.pt[7].y = box2.pt[3].y;

  SVGA_RotateObject(&box, 45);

  SVGA_StartMorph(&box3, &box);
  SVGA_StartMorph(&box, &box3);

  box.head.color = 50;
  
  box2 = box3;
  box4 = box;
  
  SVGA_MakeRegPoly(&box5, 4, MaxX/2, MaxY/2, MaxY/2-2);
  SVGA_RotateObject(&box5, 45);
  box5.head.color = 50;      
  box5.head.fillcolor = 0;
  box5.head.velocity.x = ((BINT32)1 << 16);
  box5.head.velocity.y = ((BINT32)1 << 16);
 
  for(y=0;y<NUM;y++) 
   {
    fred[y].head.velocity.x = ((BINT32)(4-y) << 16);
    fred[y].head.velocity.y = ((BINT32)(y/2+2) << 16);
    fred[y].head.color = y*20+20;
    fred[y].head.points      = 3;
    fred[y].head.fillcolor   = 0;
    fred[y].head.angle       = 0;
    fred[y].head.c.x    = ((BINT32)MaxX/2 << 16);
    fred[y].head.c.y    = ((BINT32)MaxY/2 << 16);
    SVGA_MakeRegPoly(&fred[y], fred[y].head.points, MaxX/2, MaxY/2, 5*y + 5);
   }

  /***************************/
  /* begin main program loop */
  /***************************/
  oo=0;
  SVGA_DrawObject(&box3, 0);
 
  while(!kbhit())
   {
    if (oo==0)
     {
      delay(50);
      SVGA_DrawObject(&box3, 1);
      if (SVGA_MorphObject(&box3, &box, 3)==0)
       {
        oo=!oo;
        box3 = box2;
       }
      else
       SVGA_DrawObject(&box3, 0);
     }
    else
     {
      delay(50);
      SVGA_DrawObject(&box, 1);
      if (SVGA_MorphObject(&box, &box3, 10)==0)
       {
        oo=!oo;
        box = box4;
       }
      else
       SVGA_DrawObject(&box, 0);
     }
  }
 getch();
 while(!kbhit())
  {
   SVGA_DrawObject(&box5, 1);
   SVGA_MoveObject((OBJECT_PTR)&box5, box5.head.velocity.x, box5.head.velocity.y, 1, screen);
   SVGA_DrawObject(&box5, 0);
   for(i=0;i<NUM;i++)        
    {
     SVGA_DrawObject(&fred[i], 1);     
     SVGA_RotateObject((OBJECT_PTR)&fred[i], (BINT16)(20*i-25));
     SVGA_MoveObject((OBJECT_PTR)&fred[i], fred[i].head.velocity.x, fred[i].head.velocity.y, 1, box5);
     SVGA_DrawObject(&fred[i], 0);   
    }
  }
 while (kbhit()) getch();
 
 VESA_SetMode(TEXT_MODE);
}

    
