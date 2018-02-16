/* program robot */

#include "toolkit.h"

#define ANGLE_0     0
#define ANGLE_30    160
#define ANGLE_45    240
#define ANGLE_90    480
#define ANGLE_180   960
#define ANGLE_270   1440
#define ANGLE_360   1920
#define CELL_X_SIZE 64
#define CELL_Y_SIZE 64
#define SIZESHIFT   6      
#define OVERBOARD   18
#define DELAYLEN    0
#define EAST        0
#define SOUTH       1
#define WEST        2
#define NORTH       3
#define LEFT       -1
#define RIGHT       1
#define FRONT       0

#define  BIT_SHIFT  14

#define  FIXMUL(a,b) (mul(a,b,BIT_SHIFT))
#define  FIXDIV(a,b) (fixed_div(a,b,BIT_SHIFT))
#define  FIXREAL(n)  ((FLOTE)(((FLOTE)n)*16384))
#define  FIX(n)      ((n)<<BIT_SHIFT)
#define  INT(n)      (n>>BIT_SHIFT)

#define  FIXED      signed long int

BINT16    mazewidth, mazeheight;
BINT16    roboframe, facing;
BINT16    rx,ry;
STR       mazedata[40][40];
IMAGE     viewimage;
IMAGE     robothead;
IMAGE     wall[2];
FIXED     pixelcount, pixelscale;
FIXED     bottom,top,ray,sprite_x;
BINT8    *wallptr;
BINT16    viewang, viewx, viewy;
FIXED     tan_table[ANGLE_360+1],
          inv_tan_table[ANGLE_360+1],
          cos_table[ANGLE_360+1],
          inv_cos_table[ANGLE_360+1],
          inv_sin_table[ANGLE_360+1],
          x_step[ANGLE_360+1],y_step[ANGLE_360+1];


void rendersliver();

BINT32 fixed_div(BINT32 what, BINT32 by_what, BINT8 bit)
{
 BINT32 asmdiv();
 #pragma aux asmdiv = \
  "cmp by_what, 0"    \
  "jz  end"           \
  "mov eax, what"     \
  "xor edx, edx"      \  
  "mov cl, bit"       \
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

void maketrigtables()
{
 BINT16 ang;
 FLOTE  rad_angle;

 for (ang=0;ang<ANGLE_360;ang++)
  {
   rad_angle = (3.272e-4) + ang * 2*3.141592654/ANGLE_360;

   tan_table[ang]     = FIXREAL((FLOTE)tan(rad_angle));
   inv_tan_table[ang] = FIXREAL((FLOTE)1/tan(rad_angle));

   if ((ang>=0) && (ang<ANGLE_180))
    y_step[ang] = abs(tan_table[ang])*CELL_Y_SIZE;
   else
    y_step[ang] = -abs(tan_table[ang])*CELL_Y_SIZE;

   if ((ang>=ANGLE_90) && (ang<ANGLE_270))
    x_step[ang] = -abs(inv_tan_table[ang])*CELL_X_SIZE;
   else
    x_step[ang] = abs(inv_tan_table[ang])*CELL_X_SIZE;

   inv_cos_table[ang] = FIXREAL((FLOTE)1/cos(rad_angle));
   inv_sin_table[ang] = FIXREAL((FLOTE)1/sin(rad_angle));
  }

 for (ang=-ANGLE_30;ang<ANGLE_30;ang++)
  {
   rad_angle = (3.272e-4) + ang * 2*3.141592654/ANGLE_360;

   cos_table[ang+ANGLE_30] = FIXREAL((FLOTE)12800/cos(rad_angle));
  }

} /*Build_Tables*/

void maprect(BINT16 x, BINT16 y, NOBINT8 color)
{
 void maprectasm();
 #pragma aux maprectasm =                       \
 "mov ax, y"                                    \
 "xor ebx, ebx"                                 \
 "mov bx, ax"                                   \
 "shl bx, 6"                                    \
 "shl ax, 8"                                    \
 "add bx, ax"                                   \
 "add bx, x"                                    \
 "mov edi, DWORD PTR [viewimage+0]"             \
 "add edi, ebx"                                 \
 "mov al, color"                                \
 "mov ah, al"                                   \
 "stosw"                                        \
 "stosb"                                        \
 "add edi, 317"                                 \
 "stosw"                                        \
 "stosb"                                        \
 "add edi, 317"                                 \
 "stosw"                                        \
 "stosb"                                        ;

 maprectasm();
}

void mappix(BINT16 x, BINT16 y, NOBINT8 color)
{
 void mappixasm();
 #pragma aux mappixasm =                        \
 "xor eax, eax"                                 \
 "mov ax, y"                                    \
 "mov bx, ax"                                   \
 "shl bx, 6"                                    \
 "shl ax, 8"                                    \
 "add ax, bx"                                   \
 "add ax, x"                                    \
 "mov edi, DWORD PTR [viewimage+0]"             \
 "add edi, eax"                                 \
 "mov al, color"                                \
 "stosb"                                        ;

 mappixasm();
}

void showmap()
{
 BINT16 yc,xc;

 for (yc=0;yc<mazeheight;yc++)
  for (xc=0;xc<mazewidth;xc++)
   {
    if ((mazedata[yc][xc]==219) || (mazedata[yc][xc]==111))
     maprect(xc*3,yc*3, Blue);
    else if (mazedata[yc][xc]=='*')
     maprect(xc*3,yc*3, LightGreen);
    else
     maprect(xc*3,yc*3, Black);
   }
 mappix(rx*3+1,ry*3+1,LightRed);
 switch (facing)
  {
   case NORTH: {
                mappix(rx*3,ry*3+2,LightRed);
                mappix(rx*3+2,ry*3+2,LightRed);
                break;
               }
   case SOUTH: {
               mappix(rx*3,ry*3,LightRed);
               mappix(rx*3+2,ry*3,LightRed);
               break;
              }
   case EAST: {
              mappix(rx*3,ry*3,LightRed);
              mappix(rx*3,ry*3+2,LightRed);
              break;
             }
   case WEST:{
             mappix(rx*3+2,ry*3,LightRed);
             mappix(rx*3+2,ry*3+2,LightRed);
             break;
            }
  }
}


void rendermaze(BINT16 x, BINT16 y, BINT16 view_angle)
{
 FIXED  wallframe,
        xray,
        yray,
        next_y_cell,
        next_x_cell,
        cell_x,
        cell_y,
        x_bound,
        y_bound,
        xb_save,
        yb_save,
        x_delta,
        y_delta,
        casting,
        x_hit_type,
        y_hit_type,
        xi_save,
        yi_save;

 FIXED  xi,
        yi,
        dist_x,
        dist_y,
        scale;
 STR    str[20];

 void paintview();
 #pragma aux paintview =                \
  "mov edi, DWORD PTR [viewimage+0]"    \      
  "mov cx, 16000"                       \
  "cli"                                 \
  "mov al, 8"                           \
  "mov ah, al"                          \
  "rep stosw"                           \
  "mov cx, 16000"                       \
  "mov al, 113"                         \
  "mov ah, al"                          \
  "rep stosw"                           ;

void flippage();
#pragma aux flippage =                  \
  "mov edi, 655360"                     \
  "mov esi, DWORD PTR [viewimage+0]"    \
  "mov cx, 16000"                       \
  "cli"                                 \
  "rep movsd"                           ;

 paintview();
 
 view_angle=view_angle-ANGLE_30;
 if (view_angle < 0) 
  view_angle=ANGLE_360 + view_angle;

 for (ray=0;ray<320;ray++)
  {
   if ((view_angle >= ANGLE_0) && (view_angle < ANGLE_180))
    {
     y_bound = CELL_Y_SIZE + (y & 0xFFC0);

     y_delta = CELL_Y_SIZE;

     xi = FIXMUL(inv_tan_table[view_angle], FIX(y_bound - y)) + FIX(x);

     next_y_cell = 0;
    } /*if upper half plane*/
   else
    {
     y_bound = y & 0xFFC0;

     y_delta = -CELL_Y_SIZE;

     xi = FIXMUL(inv_tan_table[view_angle], FIX(y_bound - y)) + FIX(x);

     next_y_cell = -1;
    } /*else lower half plane*/

   if ((view_angle < ANGLE_90) ||  (view_angle >= ANGLE_270))
    {
     x_bound = CELL_X_SIZE + (x & 0xFFC0);
     x_delta = CELL_X_SIZE;
     yi = FIXMUL(tan_table[view_angle], FIX(x_bound - x)) + FIX(y);
     next_x_cell = 0;
    } /*if right half plane*/
   else
    {
     x_bound = (x & 0xFFC0);
     x_delta = -CELL_X_SIZE;
     yi = FIXMUL(tan_table[view_angle],FIX(x_bound - x)) + FIX(y);
     next_x_cell = -1;
    } /*else right half plane*/

    casting= 2;
    xray=0;
    yray=0;
    dist_x = dist_y = x_hit_type = y_hit_type = 0;
    while (casting)
     {
      if (kbhit()) return;

      if (xray!=1)
       {
        cell_x = ((x_bound+next_x_cell) >> SIZESHIFT);
        cell_y = yi / FIX(CELL_Y_SIZE);

        if (((cell_x > mazewidth)||(cell_x<0))&&(yray))
         {
          dist_x = dist_y+1;
          break;
         }
        if (((cell_y > mazeheight)||(cell_y<0))&&(yray))
         {
          dist_x = dist_y+1;
          break;
         }
        x_hit_type = mazedata[cell_y][cell_x];
        if ((x_hit_type == 219) || (x_hit_type == 111))
         {
          dist_x = FIXMUL(yi - FIX(y),inv_sin_table[view_angle]);
          yi_save = INT(yi);
          xb_save = x_bound;

          xray = 1;
          casting--;
         } /*if a hit*/
        else
         {
          yi += y_step[view_angle];
          x_bound += x_delta;
         }
       } /*if x has been intercepted*/

      if (yray != 1)
       {
        cell_x = (xi / FIX(CELL_X_SIZE));
        cell_y = ((y_bound + next_y_cell) >> SIZESHIFT);
        if (((cell_x > mazewidth)||(cell_x<0))&&(xray))
         {
          dist_y = dist_x+1;
          break;
         }
        if (((cell_y > mazeheight)||(cell_y<0))&&(xray))
         {
          dist_y = dist_x+1;
          break;
         }
        y_hit_type = mazedata[cell_y][cell_x];
        if ((y_hit_type==219) || (y_hit_type==111))
         {
          dist_y  = FIXMUL(xi - FIX(x),inv_cos_table[view_angle]);
          xi_save = INT(xi);
          yb_save = y_bound;
          yray = 1;
          casting--;
         } /*if a hit*/
        else
         {
          xi += x_step[view_angle];
          y_bound += y_delta;
         }
       } /*if y ray has intersected*/
     } /*while casting*/

    if (dist_x < dist_y) 
     {
      scale = FIXDIV(cos_table[ray],dist_x);
      bottom = INT(scale);
      pixelscale = FIXDIV(FIX(wall[0].height),scale);
      pixelcount = 0;

      top = 100-bottom / 2;
      if (top < 0 )
       {
        pixelcount = FIXMUL(pixelscale, FIX(abs(top)));
        top = 0;
       }

      if ((bottom > 200))
       bottom = 200;

      if (x_hit_type==219) 
       wallframe = 0;
      else
       wallframe = 1;

     sprite_x = yi_save % wall[wallframe].width;
     wallptr = (BINT8 *)wall[wallframe].hMem;
     if (bottom>0) rendersliver();     
    }
   else
    {
     scale = FIXDIV(cos_table[ray],dist_y);

     bottom = INT(scale);
     top = 100 - bottom / 2;

     pixelscale = FIXDIV(FIX(wall[0].height),scale);
     pixelcount = 0;
     if (y_hit_type==219) 
      wallframe = 0;
     else if (y_hit_type==111) 
      wallframe = 1;

     if (top < 0)
      {
       pixelcount = FIXMUL(pixelscale, FIX(abs(top)));
       top = 0;
      }

     if (bottom > 200)
      bottom=200;

     sprite_x = xi_save % wall[wallframe].width;
     wallptr = (BINT8 *)wall[wallframe].hMem;
     if (bottom>0) rendersliver();
    } /*else*/

    view_angle++;
    if (view_angle>=ANGLE_360)
     view_angle=0;
   } /*for ray*/
 roboframe++;
 if (roboframe>1)  roboframe = 0;
 SVGA_MergeImage(&viewimage, 110+15*roboframe, 100+15*roboframe, robothead.width, robothead.height, &robothead, 0,0, BLIT_SPRITE);
 showmap();

 flippage();
 
 switch (facing)
  {
   case NORTH: strcpy(str,"North");     break;
   case SOUTH: strcpy(str,"South");     break;
   case EAST : strcpy(str,"East");      break;
   case WEST : strcpy(str,"West");      break;
  }
 SVGA_PrintString(0,190,White,str,0);
} /*Ray_Caster*/

BINT16 loadmaze(STR *fn)
{
 FILE *fi;
 WORD i;
 BYTE b;
 WORD dummy;

 if ((fi=fopen(fn,"rb"))==NULL)
  return(0);
  
 i = 0;
 while ((b!=10)&&(b!=13)) 
  {
   fread(&b, 1, 1, fi);
   mazedata[0][i] = b;
   i++;
  }
 fread(&b, 1, 1, fi);
/* dec(i);*/
 mazewidth = i+1;
 dummy = i+1;
 i = 1;
 while (dummy == mazewidth) 
  {
   dummy = fread(&mazedata[i][0], 1, mazewidth, fi);
   i++;
  }
 mazeheight = i-1;
 mazewidth = mazewidth-2;
 fclose(fi);
 return(1);
}

void turn(BINT16 dir)
{
 BINT16 count,angstp,frm;
 if (dir==LEFT)
  angstp=-ANGLE_30 / 2;
 else
  angstp=+ANGLE_30 / 2;
 frm = roboframe;
 for (count=0;count<3;count++)
  {
   viewang = viewang + angstp;
   if (viewang<0)  viewang += ANGLE_360;
   viewang %= ANGLE_360;
   rendermaze(viewx, viewy, viewang);
   roboframe = frm;
   delay(DELAYLEN / 4);
  }
 facing = (facing + dir) % 4;
 if (facing<0)  facing = 3;
 frm = (roboframe+1) % 2;
 for (count=3;count<6;count++)
  {
   viewang += angstp;
   if (viewang<0) viewang += ANGLE_360;
   viewang %= ANGLE_360;
   rendermaze(viewx, viewy, viewang);
   roboframe = frm;
   delay(DELAYLEN / 4);
  }
 delay(DELAYLEN / 4);
}

BINT16 blocked(BINT16 side)
{
 BINT16 x,y,face;

 face = (facing+side) % 4;
 if (face<0) 
  face = 3;

 if (face==NORTH) 
  {
   x = rx;
   y = ry-1;
  }
 else if (face==WEST) 
  {
   x = rx-1;
   y = ry;
  }
 else if (face==SOUTH) 
  {
   x = rx;
   y = ry+1;
  }
 else if (face==EAST) 
  {
   x = rx+1;
   y = ry;
  }

 if ((mazedata[y][x] != 32) && (mazedata[y][x] != '*')) 
  return(1);

 return(0);
}

BINT16 robotonexit()
{
 if (mazedata[ry][rx] == '*')
  return(1);
 else
  return(0);
}

void step()
{
 switch (facing)
  {
   case NORTH: {
                 viewy = viewy - CELL_Y_SIZE / 2;
                 ry--;
                 break;
                }
   case SOUTH: {
                viewy = viewy + CELL_Y_SIZE / 2;
                ry++;
                break;
               }
   case WEST: {
               viewx = viewx - CELL_X_SIZE / 2;
               rx--;
               break;
               }
   case EAST: {
               viewx = viewx + CELL_X_SIZE / 2;
               rx++;
              }
  }
 
 rendermaze(viewx,viewy,viewang);
 delay(DELAYLEN);

 viewx = rx*CELL_X_SIZE + CELL_X_SIZE / 2;
 viewy = ry*CELL_Y_SIZE + CELL_Y_SIZE / 2;

 rendermaze(viewx,viewy,viewang);
 delay(DELAYLEN);
}


BINT16 loadpal()
{
 PALETTE pal;
 FILE    *fi;
 
 if ((fi=fopen("maze.bip","rb"))==NULL) return(0);
 fread(&pal, 768, 1, fi);
 fclose(fi);
 SVGA_SetAllPalette(&pal);
 return(1);
}

void main(BINT16 argc, STR *argv[])
{
 BINT16 count,i,result;
 STR    key;
 BINT16 done;
 BINT16 change;
 FIXED  dx, dy;
 BINT16 x_sub_cell,
        y_sub_cell;

 if (argc < 2)
  result = loadmaze("maze1.txt");
 else
  result = loadmaze(argv[1]);

 if (!result)
  {
   printf("\nMaze file was not found.\n");
   exit(-1);
  }
  
 maketrigtables();

 done = 0;
 rx = 21;
 ry = 9;
 facing = SOUTH;

 if (argc >= 4)
  {
   sscanf(argv[2],"%i", &rx);
   sscanf(argv[3],"%i", &ry);
  }

 printf("\nStarting at: %i,%i.\n", rx, ry);
 getch();
 rx = 21;
 ry = 9;
 viewx = rx*CELL_X_SIZE+CELL_X_SIZE / 2;
 viewy = ry*CELL_Y_SIZE+CELL_Y_SIZE / 2;
 viewang = facing*ANGLE_90;

 change = 1;

 SVGA_SetupMode(VGA_320x200);

 SVGA_LoadPCX("robo.pcx",&robothead);
 SVGA_LoadPCX("wall.pcx",&wall[0]);
 SVGA_LoadPCX("wall2.pcx",&wall[1]);

 SVGA_InitImage(&viewimage,320,200);
 robothead.palette.color[0].red = 0;
 robothead.palette.color[0].green = 0;
 robothead.palette.color[0].blue = 0;
 SVGA_SetAllPalette(&robothead.palette);
 /* initialization is complete */

 turn(RIGHT); turn(RIGHT);turn(RIGHT);turn(RIGHT);
 /* ((!robotonexit()) && */
 while  ((!robotonexit()) &&(!kbhit()))
  {
   while ((!blocked(FRONT)) && (blocked(RIGHT))) step();
   if (!blocked(RIGHT))
    {
     turn(RIGHT);
     if (!blocked(FRONT)) step();
    }
   else if (blocked(FRONT) && blocked(RIGHT))
    {
     turn(LEFT);
     if (!blocked(FRONT)) step();
    }
  }
 if (robotonexit())
  {
   SVGA_PrintString(0,180,LightBlue,"Exit Found!",0);
   getch();
  }

/*
 while not done do
  {
    if keypressed  key = upcase(readkey) else key=#1;
    case key of
     #27:done = true;
     'S': {
            change = true;
            turn(left);
          }
     'W': dec(viewy);
     'Q': dec(viewx);
     'E': inc(viewx);
     #0:  {
           key = readkey;
           change = true;
           dx = 0;
           dy = 0;
           case key of
            'K': viewang = viewang-30;
            'M': viewang = viewang+30;
            'H': {
                  dx=cos(6.28*viewang/ANGLE_360)*10;
                  dy=sin(6.28*viewang/ANGLE_360)*10;
                 }
            'P': {
                  dx=-cos(6.28*viewang/ANGLE_360)*10;
                  dy=-sin(6.28*viewang/ANGLE_360)*10;
                 }
            else vga_printstring(0,180,key,random(256));
           }
          }
     #13: showmap;
     'P': {
           inc(count);
           vga_cls(count);
           vga_printstring(0,0,int2str(count,3), 15);
          }
    }
   if change 
    {

     if (viewang > ANGLE_360)
      viewang = 0
     else if (viewang < ANGLE_0)
      viewang = ANGLE_360;

     viewx = viewx + round(dx);
     viewy = viewy + round(dy);
     rx = viewx div CELL_X_SIZE;
     ry = viewy div CELL_Y_SIZE;

     x_sub_cell = viewx mod CELL_X_SIZE;
     y_sub_cell = viewy mod CELL_Y_SIZE;

     if (dx>0) 
      {
       if (mazedata[ry][rx+1] <> 32)  and (mazedata[ry][rx+1] <> ord('*')) and
          (x_sub_cell > (CELL_X_SIZE-OVERBOARD)) 
           viewx = viewx-(x_sub_cell-(CELL_X_SIZE-OVERBOARD));
      }
     else
      {
       if (mazedata[ry][rx-1]<>32) and (mazedata[ry][rx-1] <> ord('*')) and
          (x_sub_cell < (OVERBOARD)) 
            viewx = viewx+(OVERBOARD-x_sub_cell);
      }
     if (dy>0) 
      {
       if (mazedata[ry+1][rx]<>32) and (mazedata[ry+1][rx] <> ord('*')) and
          (y_sub_cell > (CELL_Y_SIZE-OVERBOARD )) 
            viewy = viewy-(y_sub_cell-(CELL_Y_SIZE-OVERBOARD));
      }
     else
      {
       if (mazedata[ry-1][rx] <> 32)  and (mazedata[ry-1][rx] <> ord('*')) and
          (y_sub_cell < (OVERBOARD)) 
            viewy = viewy+(OVERBOARD-y_sub_cell);
     }
    facing = (((viewang+ANGLE_45) div ANGLE_90)) mod 4;
    if (facing<0)  facing=facing+4;
    rendermaze(viewx,viewy,viewang);
   }
   change=false;
  }
*/
 /* close everything down */
 VESA_SetMode(TEXT_MODE);
}
