typedef struct TRACK_TYP
{
 BYTE padding[2];
 BYTE startaddr[4];
 BYTE ctladr;      // Control and ADR byte.
} TRACK, *TRACK_PTR;

#define REQ char

typedef struct CDPOS_TYP
{
 BYTE dummy;
 BYTE ctladrr;  // Control and ADR byte.
 BYTE track;   // Current Track #.                      
 BYTE index;    // Point or Index Byte.                  
 BYTE min;     // Minute.                              
 BYTE sec;     // Second.  > Running time within track. 
 BYTE frame;   // Frame.                               
 BYTE zero;    // Should be a 0.                        
 BYTE tmin;    // Minute.                              
 BYTE tsec;    // Second.  > Running time on disk.      
 BYTE tframe;  // Frame.                               
} CDPOS, *CDPOS_PTR;

typedef struct REQ_HEADER_TYP
{
 NOBINT8  len;
 NOBINT8  unitcode;
 NOBINT8  command;
 NOBINT16 status;
 NOBINT8  reserved[8];
} REQ_HEADER, *REQ_HEADER_PTR;

typedef struct IOCTL_TYP
{
 REQ_HEADER req;
 NOBINT8    media_descriptor;
 NOBINT32   dev_addr;
 NOBINT16   dev_len;
 NOBINT16   num_read;
 NOBINT32   stupid;
} IOCTL_HEADER, *IOCTL_HEADER_PTR;

typedef struct AUDIO_TYP
{
 REQ_HEADER req;
 NOBINT8    addrtype;
 NOBINT32   start;
 NOBINT32   length;
 NOBINT16   num_read;
 NOBINT16   stupid;
} AUDIO, AUDIO_PTR;

typedef struct DRV_LIST_TYP
{
 BYTE unitcode;
 WORD dev_real_off, dev_real_seg;
} DRV_LIST, *DRV_LIST_PTR;

typedef struct CONTROL_BLOCK_TYP
{
 NOBINT8 command;
 NOBINT8 args[130];
} CONTROL_BLOCK, *CONTROL_BLOCK_PTR;

NOBINT16 MSCDEX_Init();
NOBINT16 MSCDEX_DriverRequest(STR letter, REQ *rh, BINT16 len);
NOBINT16 MSCDEX_GetNumDrives();
NOBINT16 MSCDEX_GetVersion();
NOBINT16 MSCDEX_GetDriveTable();
NOBINT16 MSCDEX_Close(BINT8 drv);
NOBINT16 MSCDEX_Eject(BINT8 drv);
NOBINT16 MSCDEX_Reset(BINT8 drv);
NOBINT16 MSCDEX_GetDiscInfo(BINT8 drv);
NOBINT16 MSCDEX_GetTrackInfo(BINT8 drv, BINT8 tracknum, TRACK_PTR trk);
NOBINT16 MSCDEX_GetDriveStatus(BINT8 drv);
NOBINT16 MSCDEX_GetTrackLength(BINT8 drv, BINT8 trk, NOBINT32 *length);
NOBINT16 MSCDEX_GetPosition(BINT8 drv, CDPOS_PTR cdpos);
NOBINT16 MSCDEX_PlayAudio(BINT8 drv, NOBINT32 start, NOBINT32 length);
NOBINT16 MSCDEX_StopAudio(NOBINT8 drv);
NOBINT16 MSCDEX_SetVolume(BINT8 drv, NOBINT8 volume);
BINT16   MSCDEX_CalcTrack(BINT8 drv);
#ifdef MSCDEX_C
 #define e
#else
 #define e extern
#endif

e DRV_LIST MSCDEX_DrvList[4];
e BINT16   MSCDEX_NumDrives;
e NOBINT16 MSCDEX_Version;
e BINT8    MSCDEX_FirstDrive;
e NOBINT8  MSCDEX_DrvOpen, MSCDEX_DrvLocked, MSCDEX_DrvRead,
           MSCDEX_DrvDataType, MSCDEX_DrvAudio, MSCDEX_DrvInterleave,
           MSCDEX_DrvPrefetch, MSCDEX_DrvChanMan, MSCDEX_DrvRedbook,
           MSCDEX_FirstTrack, MSCDEX_LastTrack;
e NOBINT32 MSCDEX_Leadout;
e NOBINT32 MSCDEX_TrackStart[99];

