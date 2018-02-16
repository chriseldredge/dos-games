#include "toolkit.h"
BINT16 weird;

VBINT MIXER_WriteVolume(BINT8 reg, BINT8 val)
{
 outp(MIXER_BaseAddr+0x04, reg);
 outp(MIXER_BaseAddr+0x05, val);
}

NOBINT8 MIXER_ReadVolume(BINT8 reg)
{
 outp(MIXER_BaseAddr+0x04, reg);
 return(inp(MIXER_BaseAddr+0x05));
}

BINT16 MIXER_Init(VBINT)
{
 if (MIXER_BaseAddr==0)
  MIXER_BaseAddr = SB_BaseAddr;
 weird = 0; 
 switch(DSP_VERMAJ)
  {
   case 1:
   case 2:
    MIXER_WriteVolume(CT1335_MASTER, 5 << 1);
    if (MIXER_ReadVolume(CT1335_MASTER)!=(5 << 1))
     {
      MIXER_BaseAddr = 0x530; weird = 1;
      MIXER_WriteVolume(CT1345_MASTER, 5 << 1);
      if (MIXER_ReadVolume(CT1345_MASTER)!=(5 << 1))
       return(0);
     }
    MIXER_WriteVolume(0,1);
    return(1);
    break;

   case 3:
    return(1);
    break;

   case 4:
    return(1);
    break;
  }
 return(0);
}

void MIXER_SetVolume(BINT8 reg, BINT8 lev)
{
 NOBINT8 l;
 BINT16 v;
 if (weird) v=4; else v=DSP_VERMAJ;

 switch (v)
  {
   case 1:
   case 2:
           {
            if (reg==MIX_RESET)
             MIXER_WriteVolume(0, 0xFF);
            else if (reg==MIX_MASTER)
             MIXER_WriteVolume(CT1335_MASTER, (lev/4)<<1);
            else if (reg==MIX_CD)
             MIXER_WriteVolume(CT1335_CD, (lev/4)<<1);
            else if (reg==MIX_VOICE)
             MIXER_WriteVolume(CT1335_VOICE, (lev/8)<<1);
            break;
           }
   case 3: {
            l = (lev/4)<<1;
            l += (lev/4)<<5;
            if (reg==MIX_RESET)
             MIXER_WriteVolume(0, 0xFF);
            else if (reg==MIX_MASTER)
             MIXER_WriteVolume(CT1345_MASTER, l);
            else if (reg==MIX_CD)
             MIXER_WriteVolume(CT1345_CD, l);
            else if (reg==MIX_VOICE)
             MIXER_WriteVolume(CT1345_VOICE, l);
            break;
           }
   case 4: {        
            l = lev;
            if (l>=32)
             l = 31;
            l <<= 3;
            if (reg==MIX_RESET)
             MIXER_WriteVolume(0, 0xFF);
            else if (reg==MIX_MASTER)
             {
              MIXER_WriteVolume(CT1745_MASTER, l);
              MIXER_WriteVolume(CT1745_MASTER+1, l);
             }
            else if (reg==MIX_CD)
             {
              MIXER_WriteVolume(CT1745_CD, l);
              MIXER_WriteVolume(CT1745_CD+1, l);
             }
            else if (reg==MIX_VOICE)
             {
              MIXER_WriteVolume(CT1745_VOICE, l);
              MIXER_WriteVolume(CT1745_VOICE+1, l);
             }
           }
  }
}
             
NOBINT8 MIXER_GetVolume(BINT8 reg)
{
 NOBINT8 l;
 BINT16 v;
 if (weird) v=4; else v=DSP_VERMAJ;

 switch (v)
  {
   case 1:
   case 2:
           {
            if (reg==MIX_MASTER)
             return((MIXER_ReadVolume(CT1335_MASTER)>>1)*4);
            else if (reg==MIX_CD)
             return((MIXER_ReadVolume(CT1335_CD)    >>1)*4);
            else if (reg==MIX_VOICE)
             return((MIXER_ReadVolume(CT1335_VOICE) >>1)*8);
            break;
           }
   case 3: {
            if (reg==MIX_MASTER)
             l = MIXER_ReadVolume(CT1345_MASTER);
            else if (reg==MIX_CD)
             l = MIXER_ReadVolume(CT1345_CD);
            else if (reg==MIX_VOICE)
             l = MIXER_ReadVolume(CT1345_VOICE);
            l >>= 5;
            return(l*4);
            break;
           }
   case 4: {        
            if (reg==MIX_MASTER)
             l = MIXER_ReadVolume(CT1745_MASTER);
            else if (reg==MIX_CD)
             l = MIXER_ReadVolume(CT1745_CD);
            else if (reg==MIX_VOICE)
             l = MIXER_ReadVolume(CT1745_VOICE);
            l >>= 3;
            return(l);
           }
  }
 return(l);
}
