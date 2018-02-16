#include "toolkit.h"

#define BIOSCLOCKADDX   0x046C

void beater1();
void beater2();
                    //204 , 240;  
#define LEFT_SAMPLE (BINT16)(255+0x80) //204  // 10kHz
#define RGHT_SAMPLE (BINT16)(255+0x80)  // 5kHz

void main(int argc, char *argv[])
{
 BINT32 speed, sd1, sd2, x, c, c1, c2; // Number of clickes per second, sub divide 1, and 2, counters 1 and 2
 FLOTE  td1, td2;
 SOUND  snd;

 if ((!SB_Init())||(!SB16))
  {
   printf("You don't have your SB16 compatible configured good enough.\n");
   exit(0);
  }

 if (argc<4)
  { 
   printf("\nPlease try giving me variables.\n\n");
   printf("Metron Bps Sd1 Sd2\n");
   printf("Bps: Units of time where 60=1 second and 120=2 seconds\n");
   printf("Sd1: First number to SubDivide beat by  (Left Channel, Low Pitch).\n");
   printf("Sd2: Second number to SubDivide beat by (Right Chennl, High Pitch).\n\n");

   exit(0);
  }
 sscanf(argv[3], "%i", &sd2);
 sscanf(argv[2], "%i", &sd1);
 sscanf(argv[1], "%i", &speed);

 speed = speed*200;

 snd.rate = 6000;
 snd.nc = 2;
 snd.b16 = 0;
 snd.loop=1;
 snd.length = speed;
 snd.curpos=0;
 snd.bytesleft = snd.length;
 snd.hSnd8 = (BINT8 *)malloc(speed);

 td1=(FLOTE)speed / (FLOTE)sd1 / 2;
 td2=(FLOTE)speed / (FLOTE)sd2 / 2;

 c1=0;
 c2=0;
 c=0;

 for (x=0;x<snd.length;x++)
  snd.hSnd8[x] = 0;

 for (x=0;x<150;x+=3)
  {
   snd.hSnd8[x] = (BINT8) -(1+2+4+8+16+32+64);
   snd.hSnd8[x+1] = (BINT8) -(1+2+4+8+16+32+64);
  }

 while (c1<snd.length)
  {
   for (x=0;x<150;x+=8) snd.hSnd8[(c1+x)*2] = (BINT8) -(1+2+4+8+16+32+64);
   c1 += (BINT16)td1;
   c++;
   if (c==sd1) break;
  }

 c=0;

 while (c2<(snd.length-(BINT16)td1))
  {
   for (x=0;x<150;x+=5)
    snd.hSnd8[(c2+x)*2+1] = (BINT8) -(1+2+4+8+16+32+64);
   c2 += (BINT16)td2;
   c++;
   if (c==sd2) break;
  }

 SB_PlaySound(&snd, 1);

 while (!kbhit())
  {
  }

 SB_Done();
}
