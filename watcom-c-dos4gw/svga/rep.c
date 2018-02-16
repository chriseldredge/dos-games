#include "toolkit.h"

void main(int argc, char *argv[])
{
 SOUND s[23];
 BINT8 c, tf;
 BINT8 FileName[PATH_LEN+NAME_LEN];
 
 printf("\nMIX 1.1 wave mixer developed by Chris Eldredge C1996.\n");
 
 if (!SB_Init())
  {
   printf("ERROR #%i: %s!\n", SB_Status, ERRORSTRING[SB_Status]);
   exit(-2);
  }

 MIXER_Init();

 if (argc<2)
  {
   printf("\nUSAGE: mix <WAVEFILE> [<WAVEFILES>]\n");
   exit(-1);
  }
  
 printf("SoundBlaster %i.%i detected at: %xh Low DMA: %i High DMA: %i IRQ: %i\n\n",
        DSP_VERMAJ, DSP_VERMIN, SB_BaseAddr, SB_LowDma, SB_HighDma, SB_IrqNum);

 if (argc>2)
  {
   tf = argc-1;
   for (c=0;c<tf;c++)
    {
     if (!SB_LoadWave(argv[c+1], &s[c]))
      printf("Could not load file: %s!\n", argv[c+1]);
     else
      printf("%s   \t%s %s %uHz\n", argv[c+1], (s[c].nc==1) ? "MONO" : "STEREO", (s[c].b16) ? "16-Bit" : "8-Bit", s[c].rate);
     s[c].nc = 2;
    }
  }
 else
  {
   tf = SearchFile(argv[1], ".WAV");
   if (tf>23) tf = 23;
   for (c=0;c<tf;c++)
    {
     strcpy(FileName, FilePath);
     strcat(FileName, FileList[c]);
     
     if (!SB_LoadWave(FileName, &s[c]))
      printf("Could not load file: %s!\n", FileName);
     else
      printf("%s   \t%s %s %uHz\n", FileList[c], (s[c].nc==1) ? "MONO" : "STEREO", (s[c].b16) ? "16-Bit" : "8-Bit", s[c].rate);
     s[c].nc = 2;
    }
  }
    

 while (1)
  {
   c=0;
   if (kbhit()) c=getch();
   if (c==27) break;
   if (c=='!')
    {
     c=toupper(getch());
     SB_PlaySound(&s[c-'A'], 1);
    }
   c = rand()%tf;
   SB_PlaySound(&s[c], 0);
   delay(1000*(rand()%4));
  }
 
 SB_Done();
}
