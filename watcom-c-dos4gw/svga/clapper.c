#include "toolkit.h"

void main(int argc, char *argv[])
{
 SOUND  s;
 BINT8  key;
 BINT16 delaylen;
 
 printf("\nThe CLAPPER pitch demo by Chris Eldredge.\n");
 
 if (!SB_Init())
  {
   printf("ERROR #%i: %s!\n", SB_Status, ERRORSTRING[SB_Status]);
   exit(-2);
  }

 printf("SoundBlaster %i.%i detected at: %xh\n",
        DSP_VERMAJ, DSP_VERMIN, SB_BaseAddr);

 if (!SB_LoadWave("clapper.wav", &s))
  printf("Could not load file: CLAPPER.WAV!\n");
    
 delaylen = 75;       // 10Hz 

 key = 0;
 while (key != 27)
  {
   SB_PlaySound(&s, 0);
   delay(delaylen);
   if (kbhit())
    {
     key = getch();
     if (key==0)
      {
       key = getch();
       if ((key==72)&&(delaylen>2))
        delaylen--;
       else if (key==80)
        delaylen++;
       printf("%i\n", delaylen);
      }
    }
  }
 
 SB_Done();
}
