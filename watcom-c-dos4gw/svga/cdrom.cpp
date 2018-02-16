#include "toolkit.h"

void main()
{
 BINT16   i, drv;
 NOBINT16 r1, r3, nexttrk;
 TRACK    trk;
 CDPOS    cdpos;
 NOBINT32 trklen, lastsect, trkstart, trkstartred;
 STR      c, b;

 if (!MSCDEX_Init())
  printf("\nError!\n");

 printf("MSCDEX Driver Version %i.%i\n", (MSCDEX_Version & 0xFF00) >> 8, (MSCDEX_Version & 0x00FF));
 printf("MSCDEX has handles on %i device(s).\n", MSCDEX_NumDrives);

 for (i=0;i<MSCDEX_NumDrives;i++)
  {
   printf("Drive letter is %c, and unit code is %i.\n", MSCDEX_FirstDrive+i+'A', MSCDEX_DrvList[i].unitcode);
   printf("This CD-ROM has the following characteristics:\n");
   MSCDEX_GetDriveStatus(i);
   printf("  Device door is %s and %s.\n", (MSCDEX_DrvOpen) ? "open":"closed", (MSCDEX_DrvLocked) ? "locked":"unlocked");
   printf("  Device supports %s reading.\n", (MSCDEX_DrvRead) ? "both raw and cooked":"only cooked");
   printf("  Device is %s.\n", (MSCDEX_DrvDataType) ? "read/write":"read-only");
   printf("  Device reads %s.\n", (MSCDEX_DrvAudio) ? "data and plays audio/vidio tracks":"data only");
   printf("  Device %s support interleaving.\n", (MSCDEX_DrvInterleave) ? "does":"does not");
   printf("  Device %s support prefetching.\n", (MSCDEX_DrvPrefetch) ? "does":"does not");
   printf("  Device %s support audio channel manipulation.\n", (MSCDEX_DrvChanMan) ? "does":"does not");
   printf("  Device supports %s.\n", (MSCDEX_DrvRedbook) ? "both HSG and Red Book formats":"HSG format only");
  }
 
 drv = 0;

 if (!SB_Init())
  {
   printf("SoundBlaster Error! Check your BLASTER environment setting!\n");
   exit(-2);
  }
 else
  {
   printf("SB: SoundBlaster%s found at 0x%xh!\n\n", SB16 ? " 16" : "", SB_BaseAddr);
  }
 MIXER_Init();
 MIXER_SetVolume(MIX_MASTER, 31);
 MIXER_SetVolume(MIX_CD, 31);

 MSCDEX_GetDiscInfo(drv);
 lastsect = (MSCDEX_Leadout & 0xFF) + (((MSCDEX_Leadout>>8) & 0xFF)*75) +
            (((MSCDEX_Leadout>>16) & 0xFF)*60*75);

 MSCDEX_SetVolume(drv, 0xff);

 while (c!=27)
  {
   if (kbhit()) c=toupper(getch()); else c=0;
   switch (c)
    {
     case 'F' : {
                 for (i=32;i>0;i--)
                  {
                   printf("%i\n", i);
                   MIXER_SetVolume(MIX_MASTER, i);
                   delay(100);
                  }
                 for (i=0;i<33;i++)
                  {
                   printf("%i\n", i);
                   MIXER_SetVolume(MIX_MASTER, i);
                   delay(100);
                  }
                 break;
                }
     case 'D' : {
                 drv++;
                 if (drv>=MSCDEX_NumDrives)
                  drv = 0;
                 printf("Drive %c sub unit %i\n", drv+'A'+MSCDEX_FirstDrive, MSCDEX_DrvList[drv].unitcode);
                 break;
                }
     case 'E' : {
                 MSCDEX_Eject(drv);
                 break;
                }
     case 'C' : {
                 MSCDEX_Close(drv);
                 MSCDEX_GetDriveStatus(drv);
                 MSCDEX_GetDiscInfo(drv);
                 break;
                }
     case 'I' : {
                 r1=MSCDEX_GetPosition(drv, (CDPOS_PTR)&cdpos);
                 printf("Track %2i %2i:%2i %2i:%2i.\n", MSCDEX_CalcTrack(drv), cdpos.min, cdpos.sec, trklen/75/60, trklen/75%60);
                 nexttrk = cdpos.track;
                 break;
                }
     case 'V' : {
                 r1=MSCDEX_SetVolume(drv, 0xff);
                 break;
                }
     case 'S' : {
                 r1=MSCDEX_StopAudio(drv);
                 break;
                }
     case 'N' : {
                 r1=MSCDEX_GetPosition(drv, (CDPOS_PTR)&cdpos);
                 nexttrk = cdpos.track +1;
                 if (nexttrk > MSCDEX_LastTrack) break;
                 MSCDEX_StopAudio(drv);
                 r1 = MSCDEX_GetTrackInfo(drv, nexttrk, &trk);
                 trklen = lastsect-(trk.startaddr[0]+(trk.startaddr[1]*75)+(trk.startaddr[2]*60*75));
                 delay(50);
                 r3 = MSCDEX_PlayAudio(drv, trk.startaddr[0]+(trk.startaddr[1]<<8)+(trk.startaddr[2]<<16), trklen);
                 break;
                }
     case 'L' : {
                 r1=MSCDEX_GetPosition(drv, (CDPOS_PTR)&cdpos);
                 nexttrk = cdpos.track -1;
                 if (nexttrk > MSCDEX_LastTrack) break;
                 MSCDEX_StopAudio(drv);
                 r1 = MSCDEX_GetTrackInfo(drv, nexttrk, &trk);
                 trklen = lastsect-(trk.startaddr[0]+(trk.startaddr[1]*75)+(trk.startaddr[2]*60*75));
                 delay(50);
                 r3 = MSCDEX_PlayAudio(drv, trk.startaddr[0]+(trk.startaddr[1]<<8)+(trk.startaddr[2]<<16), trklen);
                 break;
                }
     case 'R' : {
                 printf("Results are %i and %i.\n", r1, r3);
                 break;
                }
     case 'Q' : {
                 r1=MSCDEX_Reset(drv);
                 r3=MSCDEX_GetDiscInfo(drv);
                 break;
                }
    }
   MSCDEX_GetDriveStatus(drv);
   MSCDEX_GetDiscInfo(drv);
   lastsect = (MSCDEX_Leadout & 0xFF) + (((MSCDEX_Leadout>>8) & 0xFF)*75) +
              (((MSCDEX_Leadout>>16) & 0xFF)*60*75);

   if ((c>='0')&&(c<='9'))
    {
     while (!kbhit()) {}
     b = getch();
     if (b!=13)
      nexttrk = (c-'1'+1)*10 + (b-'1'+1);
     else
      nexttrk = c-'1'+1;
     r1 = MSCDEX_StopAudio(drv);
     r1 = MSCDEX_GetTrackInfo(drv, nexttrk, &trk);
     trkstart = trk.startaddr[0]+(trk.startaddr[1]*75)+(trk.startaddr[2]*60*75);
     trkstartred = trk.startaddr[0]+(trk.startaddr[1]<<8)+(trk.startaddr[2]<<16);
     trklen = lastsect-trkstart;
     delay(50);
     r3 = MSCDEX_PlayAudio(drv, trkstartred, trklen);
    }
  }
 SB_Done();
}
