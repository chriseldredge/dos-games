#include "toolkit.h"

typedef struct SNOW_FLAKE_TYPE
  {
   int X, Y;
   int OldX, OldY;
   int speed;
   int Try;
  } SNOW_FLAKE;

SNOW_FLAKE SNOW_FLAKES[6001];  
int        SNOW_GROUND;
int        SNOW_MAX_FLAKES;
FONT       SNOW_ICONS;

int  SNOW_INIT(void);
void SNOW_DONE(void);
int  SNOW_GET_VMODE(void);
void SNOW_ANIMATE(int MAX);

void main()
{
 unsigned char string[50];

 if (!SNOW_INIT())
  {
   printf("\nGraphics initialization failure!!\n");
   return;
  }

 while (!KeyFlags[KEY_ESC])
  {
   SNOW_ANIMATE(SNOW_MAX_FLAKES);
   if ((KeyFlags[KEY_UP])&&(SNOW_MAX_FLAKES<6000))
     {
      SNOW_MAX_FLAKES++;
      SNOW_FLAKES[SNOW_MAX_FLAKES].OldX = SNOW_FLAKES[SNOW_MAX_FLAKES].OldY = SNOW_FLAKES[SNOW_MAX_FLAKES].Y = SNOW_FLAKES[SNOW_MAX_FLAKES].Try = 0;
      SNOW_FLAKES[SNOW_MAX_FLAKES].X = rand()%MaxX;
      SNOW_FLAKES[SNOW_MAX_FLAKES].speed = rand()%2+1;
      sprintf(string, "SNOW FLAKES: %i   ", SNOW_MAX_FLAKES);
      SVGA_PrintString(0,0,Yellow,string,1);
     }
   else if ((KeyFlags[KEY_DOWN])&&(SNOW_MAX_FLAKES>0))
     {
      SVGA_PutPixel(SNOW_FLAKES[SNOW_MAX_FLAKES].X, SNOW_FLAKES[SNOW_MAX_FLAKES].Y, Black);
      SNOW_MAX_FLAKES--;
      sprintf(string, "SNOW FLAKES: %i   ", SNOW_MAX_FLAKES);
      SVGA_PrintString(0,0,Yellow,string,1);
     }
  }

 SNOW_DONE();
}

int SNOW_INIT(void)
{
 int i, c;
 unsigned char string[50];

 if (!SVGA_Init())
   return(0);

// if (!SVGA_LoadFont("FONT.ASC", &SNOW_ICONS))
//  return(0);

 i = SVGA_GetUserMode();
 if (!i) return(0);

 i = SVGA_SetupMode(i);
 if (i!=0)
  {
   printf("\nSVGA Graphics Error #%i!!\n", i);
   return(0);
  }

 SNOW_MAX_FLAKES = MaxY * 2;

 KEYBOARD_Init();

 SNOW_GROUND = MaxY-100;
 SVGA_Cls(0);

// SVGA_SetFont((FONT_PTR)&SNOW_ICONS);
// SVGA_PrintString(16, MaxY-16, 0, SVGA_ConvertString("A B C D E F G H I J K "), 0);

// SVGA_SetFont((FONT_PTR)&ROM_SET);
 sprintf(string, "SNOW FLAKES: %i   ", SNOW_MAX_FLAKES);
 SVGA_PrintString(0,0,Yellow,string,1);
 
 if (!MOUSE_Init()) return(0);
 MOUSE_Show();
 MOUSE_SetSensitivity(5, 5);

 for (i=0;i<SNOW_MAX_FLAKES/2;i++)
  {
   for (c=0;c<2;c++)
    {
     SNOW_FLAKES[c+(i*2)].X = rand()%MaxX;
     SNOW_FLAKES[c+(i*2)].Y = 0;
     SNOW_FLAKES[c+(i*2)].speed = rand()%2+1;
     SNOW_FLAKES[c+(i*2)].Try = 0;
    }
   SNOW_ANIMATE(i*2+2);
  }
 return(1);
}

void SNOW_DONE(void)
{
 MOUSE_Done();
 VESA_SetMode(TEXT_MODE);
 KEYBOARD_Done();
}

void SNOW_ANIMATE(int MAX)
{           
// unsigned char string[255];
 int i,r;

 for (i=0;i<MAX;i++)
  {
//   if ((SNOW_FLAKES[i].OldX == SNOW_FLAKES[i].X)&&(SNOW_FLAKES[i].OldY == SNOW_FLAKES[i].Y)&&(SNOW_FLAKES[i].Y != 0)&&(SNOW_FLAKES[i].Try>1));
//    {
//     if (SVGA_GetPixel(SNOW_FLAKES[i].OldX, SNOW_FLAKES[i].OldY) == White)
//       SVGA_PutPixel(SNOW_FLAKES[i].OldX, SNOW_FLAKES[i].OldY, Black);
//     SNOW_FLAKES[i].OldX = SNOW_FLAKES[i].OldY = SNOW_FLAKES[i].Y = SNOW_FLAKES[i].Try = 0;
//     SNOW_FLAKES[i].X = rand()%MaxX;
//     continue;
//    }

   SNOW_FLAKES[i].OldX = SNOW_FLAKES[i].X;
   SNOW_FLAKES[i].OldY = SNOW_FLAKES[i].Y;

   if (SNOW_FLAKES[i].Try==0)
     r = rand()%3;
   else
     r = 4;

   if (r==1)
    {
     SNOW_FLAKES[i].X--;
    }
   else if (r==2)
    {
     SNOW_FLAKES[i].X++;
    }
   SNOW_FLAKES[i].Y+=SNOW_FLAKES[i].speed;
   
   if ((((SVGA_GetPixel(SNOW_FLAKES[i].X,SNOW_FLAKES[i].Y+1) != 0)&&(SVGA_GetPixel(SNOW_FLAKES[i].X,SNOW_FLAKES[i].Y+1) != 15))&&(SNOW_FLAKES[i].Y>=SNOW_GROUND))||(SNOW_FLAKES[i].Y>=MaxY))
    {
     SNOW_FLAKES[i].Try++;
     if ((SNOW_FLAKES[i].Try<4)&&(SNOW_FLAKES[i].Y<MaxY))
      {
       if ((SVGA_GetPixel(SNOW_FLAKES[i].X+1,SNOW_FLAKES[i].Y+1) == 0)&&(SNOW_FLAKES[i].X+1<MaxX))
         {
          SNOW_FLAKES[i].X++;SNOW_FLAKES[i].Y++;
          continue;
         }
       else if ((SVGA_GetPixel(SNOW_FLAKES[i].X-1,SNOW_FLAKES[i].Y+1) == 0)&&(SNOW_FLAKES[i].X-1>0))
         {
          SNOW_FLAKES[i].X--;SNOW_FLAKES[i].Y++;
          continue;
         }
      }

     if (SNOW_GROUND>SNOW_FLAKES[i].Y-1) SNOW_GROUND=SNOW_FLAKES[i].Y-1;

     SVGA_PutPixel(SNOW_FLAKES[i].X,SNOW_FLAKES[i].Y, 31);

     SNOW_FLAKES[i].OldX = SNOW_FLAKES[i].OldY = SNOW_FLAKES[i].Y = SNOW_FLAKES[i].Try = 0;
     SNOW_FLAKES[i].X = rand()%MaxX;
    }
   else
    {
     if (SVGA_GetPixel(SNOW_FLAKES[i].OldX, SNOW_FLAKES[i].OldY) == White)
       SVGA_PutPixel(SNOW_FLAKES[i].OldX, SNOW_FLAKES[i].OldY, Black);
     if (SVGA_GetPixel(SNOW_FLAKES[i].X, SNOW_FLAKES[i].Y) == Black)
       SVGA_PutPixel(SNOW_FLAKES[i].X, SNOW_FLAKES[i].Y, White);
       
    }
  }
}
