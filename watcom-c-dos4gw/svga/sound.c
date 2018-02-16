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
      printf("Press %c to play %s   \t%s %s %uHz\n", 'A'+c, argv[c+1], (s[c].nc==1) ? "MONO" : "STEREO", (s[c].b16) ? "16-Bit" : "8-Bit", s[c].rate);
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
      printf("Press %c to play %s   \t%s %s %uHz\n", 'A'+c, FileList[c], (s[c].nc==1) ? "MONO" : "STEREO", (s[c].b16) ? "16-Bit" : "8-Bit", s[c].rate);
    }
  }
    
 while (1)
  {
   c=0;
   if (kbhit()) c=getch();
   if (c==27) break;
   if ((c>='A')&&(c<='A'+tf)) SB_PlaySound(&s[c-'A'],0);
   if (c=='!') SB_PlaySound(&s[0],1);
  }
 
 SB_Done();
}
