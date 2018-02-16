#define MSCDEX_C
#include "toolkit.h"

NOBINT16 MSCDEX_GetVersion()
{
 NOBINT16 v;

 void asmgetversion(void);
 
 #pragma aux asmgetversion = \
  "mov ax, 150Ch"            \
  "int 2fh"                  \
  "mov [v], bx"              ;

 v=0;
 asmgetversion();
 return(v);
} 

NOBINT16 MSCDEX_GetNumDrives()
{
 BINT16 v;
 
  void asmgetnumd();
 
 #pragma aux asmgetnumd = \
  "mov  AX,1500H"            \
  "xor bx, bx"               \
  "int 2fh"                  \
  "mov v, bx"                \
  "mov MSCDEX_FirstDrive, cl";
  v=0;
  asmgetnumd();
  return(v);
}

NOBINT16 MSCDEX_GetDriveTable()
{
 ONION RINGS  r;
 RMTREGS      tr;
 STRUT SREGS  sr;
 BINT16       segment, selector;

 if (!DPMI_AllocCM(sizeof(MSCDEX_DrvList), &selector, &segment)) 
  {
   enter(0);
  }
 
 memset(&tr, 0, sizeof(RMTREGS));

 tr.eax = 0x1501;           // setup translation registers
 tr.es  = segment;
 tr.ebx = 0;

 segread(&sr);
 r.w.ax = 0x0300;
 r.h.bl = 0x2f;
 r.h.bh = 0;
 r.w.cx = 0;
 sr.es = FP_SEG(&tr);
 r.x.edi = FP_OFF(&tr);

 int386x(0x31, &r, &r, &sr);

 _fmemcpy(&MSCDEX_DrvList, MK_FP(selector,0), sizeof(MSCDEX_DrvList));

 DPMI_FreeCM(selector);

 return(1);
}

NOBINT16 MSCDEX_DriverRequest(STR letter, REQ *rh, BINT16 len)
{
 ONION RINGS  r;
 RMTREGS      tr;
 STRUT SREGS  sr;
 BINT16       segment, selector;

 if (!DPMI_AllocCM(len/16+10, &selector, &segment)) 
  {
   enter(0);
  }
 
 _fmemcpy(MK_FP(selector,0), rh, len);

 memset(&tr, 0, sizeof(RMTREGS));

 tr.eax = 0x1510;           // setup translation registers
 tr.ecx = letter;
 tr.es  = segment;
 tr.ebx = 0;

 segread(&sr);
 r.w.ax = 0x0300;
 r.h.bl = 0x2f;
 r.h.bh = 0;
 r.w.cx = 0;
 sr.es = FP_SEG(&tr);
 r.x.edi = FP_OFF(&tr);

 int386x(0x31, &r, &r, &sr);

 _fmemcpy(rh, MK_FP(selector,0), len);

 DPMI_FreeCM(selector);

 return(1);
}

NOBINT16 MSCDEX_Reset(BINT8 drv)
{
 IOCTL_HEADER rh;
 CONTROL_BLOCK_PTR cb;
 BINT16            segment, selector;

 if (!DPMI_AllocCM(sizeof(cb)/16+10, &selector, &segment)) 
  {
   enter(0);
  }

 cb = (CONTROL_BLOCK_PTR)selector;

 cb->command = 3;

 rh.req.len = sizeof(rh);
 rh.req.unitcode = MSCDEX_DrvList[drv].unitcode;
 rh.req.command = 12;
 rh.req.status = 0;
 rh.dev_len = 1;
 rh.dev_addr = FP_OFF(cb);

 MSCDEX_DriverRequest(MSCDEX_FirstDrive+drv, (REQ *)&rh, sizeof(rh));
 DPMI_FreeCM(selector);
 return(rh.req.status);
}

NOBINT16 MSCDEX_Eject(BINT8 drv)
{
 IOCTL_HEADER rh;
 CONTROL_BLOCK_PTR cb;
 BINT16            segment, selector;

 if (!DPMI_AllocCM(sizeof(cb)/16+10, &selector, &segment)) 
  {
   enter(0);
  }

 cb = (CONTROL_BLOCK_PTR)selector;

 cb->command = 0;

 rh.req.len = sizeof(rh);
 rh.req.unitcode = MSCDEX_DrvList[drv].unitcode;
 rh.req.command = 12;
 rh.req.status = 0;
 rh.dev_len = 1;
 rh.dev_addr = FP_OFF(cb);

 MSCDEX_DriverRequest(MSCDEX_FirstDrive+drv, (REQ *)&rh, sizeof(rh));
 DPMI_FreeCM(selector);
 return(rh.req.status);
}

NOBINT16 MSCDEX_SetVolume(BINT8 drv, NOBINT8 volume)
{
 IOCTL_HEADER rh;
 CONTROL_BLOCK_PTR cb;
 BINT16 segment, selector;

 if (!DPMI_AllocCM(sizeof(cb)/16+10, &selector, &segment)) 
  {
   enter(0);
  }

 cb = (CONTROL_BLOCK_PTR)selector;

 cb->command = 3;

 rh.req.len = sizeof(rh);
 rh.req.unitcode = MSCDEX_DrvList[drv].unitcode;
 rh.req.command = 12;
 rh.req.status = 0;
 rh.dev_len = 9;
 rh.dev_addr = FP_OFF(cb);

 cb->args[0] = 0; cb->args[2] = 1; cb->args[4] = 2; cb->args[6] = 3;
 cb->args[1] = volume; cb->args[3] = volume;
 cb->args[5] = volume; cb->args[7] = volume;
 MSCDEX_DriverRequest(MSCDEX_FirstDrive+drv, (REQ *)&rh, sizeof(rh));
 DPMI_FreeCM(selector);
 return(rh.req.status);
}

NOBINT16 MSCDEX_Close(BINT8 drv)
{
 IOCTL_HEADER rh;
 CONTROL_BLOCK_PTR cb;
 BINT16            segment, selector;

 if (!DPMI_AllocCM(sizeof(cb)/16+10, &selector, &segment)) 
  {
   enter(0);
  }

 cb = (CONTROL_BLOCK_PTR)selector;

 cb->command = 5;

 rh.req.len = sizeof(rh);
 rh.req.unitcode = MSCDEX_DrvList[drv].unitcode;
 rh.req.command = 12;
 rh.req.status = 0;
 rh.dev_len = 1;
 rh.dev_addr = FP_OFF(cb);

 MSCDEX_DriverRequest(MSCDEX_FirstDrive+drv, (REQ *)&rh, sizeof(rh));
 DPMI_FreeCM(selector);
 return(rh.req.status);
}

NOBINT16 MSCDEX_GetDriveStatus(BINT8 drv)
{
 IOCTL_HEADER        rh;
 CONTROL_BLOCK_PTR cb;
 BINT16            segment, selector;

 if (!DPMI_AllocCM(sizeof(cb)/16+10, &selector, &segment)) 
  {
   enter(0);
  }

 cb = (CONTROL_BLOCK_PTR)selector;

 cb->command = 6;

 rh.req.len = sizeof(rh);
 rh.req.unitcode = MSCDEX_DrvList[drv].unitcode;
 rh.req.command = 3;
 rh.req.status = 0;
 rh.dev_len = 5;
 rh.dev_addr = FP_OFF(cb);

 cb->args[0] = 0;
 cb->args[1] = 0;
 cb->args[2] = 0;
 cb->args[3] = 0;

 MSCDEX_DriverRequest(MSCDEX_FirstDrive+drv, (REQ *)&rh, sizeof(rh));

 MSCDEX_DrvOpen = cb->args[0]&1;
 MSCDEX_DrvLocked = !(cb->args[0]&2);
 MSCDEX_DrvRead = cb->args[0]&4;
 MSCDEX_DrvDataType = cb->args[0]&8;
 MSCDEX_DrvAudio = cb->args[0]&16;
 MSCDEX_DrvInterleave = cb->args[0]&32;
 MSCDEX_DrvPrefetch = cb->args[0]&128;
 MSCDEX_DrvChanMan = cb->args[1]&1;
 MSCDEX_DrvRedbook = cb->args[1]&2;

 DPMI_FreeCM(selector);

 return(rh.req.status);
}

NOBINT16 MSCDEX_GetDiscInfo(BINT8 drv)
{
 IOCTL_HEADER      rh;
 CONTROL_BLOCK_PTR cb;
 TRACK             trk;
 BINT16            segment, selector, i;

 if (!DPMI_AllocCM(sizeof(cb)/16+10, &selector, &segment)) 
  {
   enter(0);
  }

 cb = (CONTROL_BLOCK_PTR)selector;

 cb->command = 10;

 rh.req.len = sizeof(rh);
 rh.req.unitcode = MSCDEX_DrvList[drv].unitcode;
 rh.req.command = 3;                                // IOCTL read
 rh.req.status = 0;
 rh.dev_len = 7;
 rh.dev_addr = FP_OFF(cb);

 cb->args[0] = 0;
 cb->args[1] = 0;
 cb->args[2] = 0;
 cb->args[3] = 0;

 MSCDEX_DriverRequest(MSCDEX_FirstDrive+drv, (REQ *)&rh, sizeof(rh));

 MSCDEX_FirstTrack = cb->args[0];
 MSCDEX_LastTrack = cb->args[1];
 MSCDEX_Leadout = cb->args[2]+(cb->args[3]<<8)+(cb->args[4]<<16)+(cb->args[5]<<32);

 DPMI_FreeCM(selector);

 if ((rh.req.status&23768) == 0)
  {
   for (i=MSCDEX_FirstTrack;i<=MSCDEX_LastTrack;i++)
    {
     MSCDEX_GetTrackInfo(drv, i, &trk);
     MSCDEX_TrackStart[i] = trk.startaddr[0]+trk.startaddr[1]*75+trk.startaddr[2]*75*60;
    }
   MSCDEX_TrackStart[MSCDEX_LastTrack+1] = (MSCDEX_Leadout & 0xFF) + ((MSCDEX_Leadout>>8) & 0xFF)*75+
                                           ((MSCDEX_Leadout>>16) & 0xFF)*60*75;
  }
 return(rh.req.status);
}

NOBINT16 MSCDEX_GetTrackInfo(BINT8 drv, BINT8 tracknum, TRACK_PTR trk)
{
 IOCTL_HEADER        rh;
 CONTROL_BLOCK_PTR cb;
 BINT16            segment, selector;

 if (!DPMI_AllocCM(sizeof(cb)/16+10, &selector, &segment)) 
  {
   enter(0);
  }

 cb = (CONTROL_BLOCK_PTR)selector;

 cb->command = 11;
 cb->args[0] = tracknum;

 rh.req.len = sizeof(rh);
 rh.req.unitcode = MSCDEX_DrvList[drv].unitcode;
 rh.req.command = 3;                                // IOCTL read
 rh.req.status = 0;
 rh.dev_len = 7;
 rh.dev_addr = FP_OFF(cb);

 MSCDEX_DriverRequest(MSCDEX_FirstDrive+drv, (REQ *)&rh, sizeof(rh));

 _fmemcpy(trk, (void *)selector, 7);
 DPMI_FreeCM(selector);

 return(rh.req.status);
}

NOBINT16 MSCDEX_GetTrackLength(BINT8 drv, BINT8 trk, NOBINT32 *length)
{
 NOBINT16 result;
 TRACK    trkinfo;
 NOBINT32 l, t;

 result = MSCDEX_GetTrackInfo(drv, trk, &trkinfo);
 t = trkinfo.startaddr[0] + trkinfo.startaddr[1]*75 + trkinfo.startaddr[2]*60*75;

 if (trk == MSCDEX_LastTrack)
  {
   l =  (MSCDEX_Leadout & 0xFF) + ((MSCDEX_Leadout>>8) & 0xFF)*75+
        ((MSCDEX_Leadout>>16) & 0xFF)*60*75;
   l -= t;
  }
 else
  {
   result |= MSCDEX_GetTrackInfo(drv, trk+1, &trkinfo);
   l = trkinfo.startaddr[0] + trkinfo.startaddr[1]*75 + trkinfo.startaddr[2]*60*75;
   l -= t;
  }
 *length = l;

 return(result);
}

NOBINT16 MSCDEX_GetPosition(BINT8 drv, CDPOS_PTR cdpos)
{
 IOCTL_HEADER      rh;
 CONTROL_BLOCK_PTR cb;
 BINT16            segment, selector;

 if (!DPMI_AllocCM(sizeof(cb)/16+10, &selector, &segment)) 
  {
   enter(0);
  }

 cb = (CONTROL_BLOCK_PTR)selector;

 cb->command = 12;

 rh.req.len = sizeof(rh);
 rh.req.unitcode = MSCDEX_DrvList[drv].unitcode;
 rh.req.command = 3;                                // IOCTL read
 rh.req.status = 0;
 rh.dev_len = 11;
 rh.dev_addr = FP_OFF(cb);

 MSCDEX_DriverRequest(MSCDEX_FirstDrive+drv, (REQ *)&rh, sizeof(rh));

 _fmemcpy(cdpos, (void *)selector, 11);
 DPMI_FreeCM(selector);

 return(rh.req.status);
}


NOBINT16 MSCDEX_PlayAudio(BINT8 drv, NOBINT32 start, NOBINT32 length)
{
 AUDIO        rh;

 rh.req.len = sizeof(rh);
 rh.req.unitcode = MSCDEX_DrvList[drv].unitcode;
 rh.req.command = 132;                               //play audio
 rh.req.status = 0;
 rh.addrtype = 1;
 rh.start = start;
 rh.length = length;

 MSCDEX_DriverRequest(MSCDEX_FirstDrive+drv, (REQ *)&rh, sizeof(rh));

 return(rh.req.status);
}

NOBINT16 MSCDEX_StopAudio(NOBINT8 drv)
{
 AUDIO        rh;

 rh.req.len = sizeof(rh);
 rh.req.unitcode = MSCDEX_DrvList[drv].unitcode;
 rh.req.command = 133;                               //play audio
 rh.req.status = 0;

 MSCDEX_DriverRequest(MSCDEX_FirstDrive+drv, (REQ *)&rh, sizeof(rh));

 return(rh.req.status);
}
NOBINT16 MSCDEX_Init()
{
 NOBINT16 ver;

 MSCDEX_NumDrives = MSCDEX_GetNumDrives();

 if (MSCDEX_NumDrives==0)
  return(0);

 MSCDEX_Version = ver = MSCDEX_GetVersion();
 MSCDEX_GetDriveTable();

 return(1);
}

BINT16 MSCDEX_CalcTrack(BINT8 drv)
{
 CDPOS    cdpos;
 NOBINT32 trkstart, trkend, curpos;
 NOBINT16 r, i;
 
 r = MSCDEX_GetPosition(drv, (CDPOS_PTR)&cdpos);
 curpos = cdpos.tframe + cdpos.tmin*60*75 + cdpos.tsec*75;
 
 for (i=MSCDEX_FirstTrack;i<=MSCDEX_LastTrack;i++)
  {
   trkstart = MSCDEX_TrackStart[i];
   if (i>=MSCDEX_LastTrack)
    trkend =  (MSCDEX_Leadout & 0xFF) + ((MSCDEX_Leadout>>8) & 0xFF)*75+
              ((MSCDEX_Leadout>>16) & 0xFF)*60*75;
   else
    trkend = MSCDEX_TrackStart[i+1]-1;

   if ((curpos>=trkstart)&&(curpos<=trkend))
    return(i);
  }
 return(0);
}

