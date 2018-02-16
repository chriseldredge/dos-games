#include "svga.h"
#include "keyboard.h"

typedef struct SNOW_FLAKE_TYPE
  {
   int X, Y;
   int OldX, OldY;
   int Try;
  } SNOW_FLAKE;

#define SNOW_VMODE      SVGA_640x480
#define SNOW_MAX_FLAKES 1000

SNOW_FLAKE SNOW_FLAKES[SNOW_MAX_FLAKES];       // Lots of snow!!
int SNOW_GROUND;

int  SNOW_INIT(void);
void SNOW_DONE(void);
void SNOW_MOVE(int MAX);
void SNOW_DRAW(int MAX);
void SNOW_ERASE(int MAX);

void main()
{
 if (!SNOW_INIT())
  {
   printf("\nGraphics initialization failure!!\n");
   return;
  }

 while (!FlagTable[KEY_ESC])
  {
   SNOW_ERASE(SNOW_MAX_FLAKES);
   SNOW_MOVE(SNOW_MAX_FLAKES);
   SNOW_DRAW(SNOW_MAX_FLAKES);
  }

 SNOW_DONE();
}

int SNOW_INIT(void)
{
 int i, c, r;

 if (!SVGA_Init())
   return(0);
 SVGA_SetupMode(SNOW_VMODE);
 SetKb();

 SNOW_GROUND = MaxY-1;
 SVGA_HLine(0, MaxX, MaxY-1, White);
 for (i=0;i<SNOW_MAX_FLAKES/2;i++)
  {
   for (c=0;c<2;c++)
    {
     SNOW_FLAKES[c+(i*2)].X = rand()%MaxX;
     SNOW_FLAKES[c+(i*2)].Y = 0;
     SNOW_FLAKES[c+(i*2)].Try = 0;
    }
   SNOW_ERASE(i*2+2);
   SNOW_MOVE(i*2+2);
   SNOW_DRAW(i*2+2);
  }
 return(1);
}

void SNOW_DONE(void)
{
 VESA_SetMode(TEXT_MODE);
 ResetKb();
}

void SNOW_MOVE(int MAX)
{
 unsigned char string[255];
 int i,r;

 for (i=0;i<MAX;i++)
  {
   SNOW_FLAKES[i].OldX = SNOW_FLAKES[i].X;
   SNOW_FLAKES[i].OldY = SNOW_FLAKES[i].Y;

   if (SNOW_FLAKES[i].Try==0)
     r = rand()%3;
   else
     r = 0;

   if (r==0)
    {
     SNOW_FLAKES[i].Y++;
    }
   else if (r==1)
    {
     SNOW_FLAKES[i].Y++;
     SNOW_FLAKES[i].X--;
    }
   else if (r==2)
    {
     SNOW_FLAKES[i].Y++;
     SNOW_FLAKES[i].X++;
    }

   if (((SVGA_GetPixel(SNOW_FLAKES[i].X,SNOW_FLAKES[i].Y))&&(SNOW_FLAKES[i].Y>=SNOW_GROUND))||(SNOW_FLAKES[i].Y>=MaxY))
    {
     SNOW_FLAKES[i].Try++;
     if ((SNOW_FLAKES[i].Try==1)&&(!(SNOW_FLAKES[i].Y>=MaxY)))
      {
       if (!SVGA_GetPixel(SNOW_FLAKES[i].X+1,SNOW_FLAKES[i].Y+1))
         {
          SNOW_FLAKES[i].X++;SNOW_FLAKES[i].Y++;
          SNOW_FLAKES[i].Try--;
          continue;
         }
       else if (!SVGA_GetPixel(SNOW_FLAKES[i].X-1,SNOW_FLAKES[i].Y+1))
         {
          SNOW_FLAKES[i].X--;SNOW_FLAKES[i].Y++;
          SNOW_FLAKES[i].Try--;
          continue;
         }
      }

     if (SNOW_GROUND>SNOW_FLAKES[i].Y-1) SNOW_GROUND=SNOW_FLAKES[i].Y-1;

     SVGA_PutPixel(SNOW_FLAKES[i].X,SNOW_FLAKES[i].Y, White);

     SNOW_FLAKES[i].OldX = SNOW_FLAKES[i].OldY = SNOW_FLAKES[i].Y = SNOW_FLAKES[i].Try = 0;
     SNOW_FLAKES[i].X = rand()%MaxX;
    }
  }
}

void SNOW_DRAW(int MAX)
{
 int i;

 for (i=0;i<MAX;i++)
  SVGA_PutPixel(SNOW_FLAKES[i].X, SNOW_FLAKES[i].Y, White);
}

void SNOW_ERASE(int MAX)
{
 int i;

 for (i=0;i<MAX;i++)
  if (!SNOW_FLAKES[i].Try) SVGA_PutPixel(SNOW_FLAKES[i].OldX, SNOW_FLAKES[i].OldY, Black);
}

