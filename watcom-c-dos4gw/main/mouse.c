#include <i86.h>
#define  MOUSE_C
#include "toolkit.h"

static signed short int osi, odi;
short int resetok;

void _loadds far MouseHandler(int max, int mbx, int mcx, int mdx,
                              int msi, int mdi)
{
 #pragma aux MouseHandler parm [EAX] [EBX] [ECX] [EDX] [ESI] [EDI]
 void asmgetmotion(void);

 #pragma aux asmgetmotion = \
   "mov ax, ds" \
   "mov es, ax" \
   "mov ax, 11" \
   "int 0x33  " \
   "mov osi, cx"\
   "mov odi, dx";

 unsigned short int OldBank;
 
 if (!MOUSE_SHOWING)
  {
   asmgetmotion(); 
   return;
  }
  
   asmgetmotion();        /* Can't use INT386() because its non-reenterant */
 
   /* fixed point math - faster than floating point and neccesary for
     sensitivity by percentage!! */
   MOUSE_X += mul((BINT32)(osi<<16),(BINT32)(MOUSE_SENSITIVITY_X<<16)/100,16)>>16;
   MOUSE_Y += mul((BINT32)(odi<<16),(BINT32)(MOUSE_SENSITIVITY_Y<<16)/100,16)>>16;

   if (MOUSE_X > MOUSE_LIMIT_X2)
    MOUSE_X = MOUSE_LIMIT_X2-1;

   if (MOUSE_Y >= MOUSE_LIMIT_Y2)
    MOUSE_Y = MOUSE_LIMIT_Y2;

   if (MOUSE_X <= MOUSE_LIMIT_X1)
     MOUSE_X = MOUSE_LIMIT_X1;

   if (MOUSE_Y <= MOUSE_LIMIT_Y1)
     MOUSE_Y = MOUSE_LIMIT_Y1;
  

 if (MOUSE_SHOWING && ((MOUSE_X != MOUSE_OLD_X)||(MOUSE_Y != MOUSE_OLD_Y)))
  {
   OldBank = CurrentBankWr;
   CurrentBankWr = 0xffff;
   MOUSE_Replace();
   MOUSE_Backup();
   CurrentBankWr = 0xffff;                      // force page change (?)
   MOUSE_DrawCursor();

   VESA_SetPage((short int)OldBank, BANK_WR);

   MOUSE_OLD_X = MOUSE_X; MOUSE_OLD_Y = MOUSE_Y;
  }
 MOUSE_BUTTON_LEFT  = mbx & 1;    /* LMB currently down */
 MOUSE_BUTTON_RIGHT = mbx & 2;    /* RMB currently down */
 return;
}

void Handler_end(void)
{
}
#pragma on (check_stack)

int MOUSE_Reset(void)
{
 union REGS r;

 r.w.ax = 0;          
 int386(0x33, &r, &r);
 resetok = r.w.ax;
 return(r.w.ax);
}

int MOUSE_Init(void)
{
 union REGS r;
 struct SREGS sr;
 void (far *function_ptr)();

 if (!MOUSE_Reset()) return(0);
 
 MOUSE_BUTTON_LEFT = MOUSE_BUTTON_RIGHT = FALSE;
 MOUSE_X = MaxX/2-8; MOUSE_Y = MaxY/2-8;
 MOUSE_SetRange(0, 0, MaxX, MaxY);
 MOUSE_SHOWING = 0;

 switch (BytesPerPixel)
  {
   case 1: _fmemcpy((char *)&MOUSE_CURSOR, (char *)&DEFAULT_CURSOR, 256);   break;
   case 2: _fmemcpy((char *)&MOUSE_CURSOR, (int *)&DEFAULT_CURSOR16, 512); break;
   case 4: _fmemcpy((char *)&MOUSE_CURSOR, (long int *)&DEFAULT_CURSOR32, 1024);
  }
 MOUSE_CURSOR_WIDTH = 16;
 MOUSE_CURSOR_HEIGHT = 16;
 MOUSE_SetSensitivity(100,100);

 if (! DPMI_LockRegion ((void near *) MouseHandler, (char *) Handler_end - (char near *) MouseHandler))
  return(0);

 segread(&sr);

 r.x.eax  = 0xC;
 r.x.ecx  = 0x1 | 0x2 | 0x4 | 0x8 | 0x10;
 function_ptr = ( void(*)(void) ) MouseHandler;
 r.x.edx = FP_OFF (function_ptr);
 sr.es   = FP_SEG (function_ptr);

 int386x (0x33, &r, &r, &sr);
 return(1);
}

void MOUSE_Done(void)
{
 if (!resetok) return;
 MOUSE_Replace();
 MOUSE_Reset();
}

void MOUSE_Hide(void)
{
 if (!resetok) return;
 
 if (!MOUSE_SHOWING)
   return;

 _disable();

 MOUSE_SHOWING = 0;

 MOUSE_Replace();
 MOUSE_OLD_X = MOUSE_X, MOUSE_OLD_Y = MOUSE_Y;
 _enable();
}

void MOUSE_Show(void)
{
 if (!resetok) return;

 _disable();
 MOUSE_SHOWING = 1;
 hey = 1;
 MouseHandler(0, ML|MR, 0, 0, 0, 0);
 _enable();
}

void MOUSE_SetRange(int x1, int y1, int x2, int y2)
{
 _disable();
 MOUSE_LIMIT_X1 = x1;
 MOUSE_LIMIT_Y1 = y1;
 MOUSE_LIMIT_X2 = x2;
 MOUSE_LIMIT_Y2 = y2;
 _enable();
}

void MOUSE_DrawCursor(void)
{
 WORD dy, w;
 DWORD page, p_off, p1; 
 BINT32 p2;

 w = MOUSE_CURSOR_WIDTH;

 if (MOUSE_X+w>MOUSE_LIMIT_X2)
  w = MOUSE_LIMIT_X2-MOUSE_X+1;

 for (dy=0;dy<MOUSE_CURSOR_HEIGHT;dy++)
  {
   p_off = sts[MOUSE_Y+dy]+MOUSE_X*BytesPerPixel;
   page = stp[MOUSE_Y+dy];
   if (stb[MOUSE_Y+dy])
    if ((DWORD)sts[MOUSE_Y+dy]+(DWORD)MOUSE_X*BytesPerPixel >= (DWORD)BytesPerBank)
     {
      page++;
      p_off -= BytesPerBank;
     }

   if (CurrentBankWr != (short int)page)
     VESA_SetPage((short int)page, BANK_WR);

   if (dy+MY>MOUSE_LIMIT_Y2)
    break;

   scpy(fg_img.hMem+stx[MOUSE_Y+dy]+MOUSE_X*BytesPerPixel, (BINT8 *)&MOUSE_CURSOR+(dy*MOUSE_CURSOR_WIDTH*BytesPerPixel), w);

   if ((DWORD)(w*BytesPerPixel + p_off)<BytesPerBank)
     scpy(write_buffer+p_off, (BINT8 *)&MOUSE_CURSOR+(dy*MOUSE_CURSOR_WIDTH*BytesPerPixel), w);
   else
     {
      scpy(write_buffer+p_off, (BINT8 *)&MOUSE_CURSOR+(dy*MOUSE_CURSOR_WIDTH*BytesPerPixel), (WORD)(BytesPerBank - p_off)/BytesPerPixel);
      VESA_SetPage((short int)CurrentBankWr+1, BANK_WR);
      p1 = (DWORD)(dy*MOUSE_CURSOR_WIDTH*BytesPerPixel)+(BytesPerBank-p_off);
      p2 = (DWORD)(p_off+w*BytesPerPixel)-(DWORD)BytesPerBank;
      if ((int)p2>0)
        scpy(write_buffer, (BINT8 *)&MOUSE_CURSOR+p1, (WORD)p2/BytesPerPixel);
     }
  } 
}

void MOUSE_Backup(void)
{
 int dy, w, h;
 DWORD page, p_off;

 if (MX+MOUSE_CURSOR_WIDTH>MOUSE_LIMIT_X2)
  w = MOUSE_LIMIT_X2-MX+1;
 else
  w = MOUSE_CURSOR_WIDTH;
 
 if (MY+MOUSE_CURSOR_HEIGHT>MOUSE_LIMIT_Y2)
  h = MOUSE_LIMIT_Y2-MY+1;
 else
  h = MOUSE_CURSOR_HEIGHT;
 
 for (dy=0;dy<h;dy++)
  {
   p_off = sts[MOUSE_Y+dy]+MOUSE_X*BytesPerPixel;
   page = stp[MOUSE_Y+dy];
   if (stb[MOUSE_Y+dy])
    if ((DWORD)sts[MOUSE_Y+dy]+(DWORD)MOUSE_X*BytesPerPixel >= (DWORD)BytesPerBank)
     {
      page++;
      p_off -= BytesPerBank;
     }

   rcpy((BINT8 *)&BackupSprite_fg+(dy*MOUSE_CURSOR_WIDTH*BytesPerPixel), fg_img.hMem+stx[MOUSE_Y+dy]+MOUSE_X*BytesPerPixel, w);
  } 
}

void MOUSE_Replace(void)
{
 int  dy,w,h, obo, ofo;
 DWORD p_off, page;
 BINT32 p1, p2;

 if (MOUSE_OLD_X+MOUSE_CURSOR_WIDTH>MOUSE_LIMIT_X2)
  w = MOUSE_LIMIT_X2-MOUSE_OLD_X+1;
 else
  w = MOUSE_CURSOR_WIDTH;

 if (MOUSE_OLD_Y+MOUSE_CURSOR_HEIGHT>MOUSE_LIMIT_Y2)
  h = MOUSE_LIMIT_Y2-MOUSE_OLD_Y+1;
 else
  h = MOUSE_CURSOR_HEIGHT;

 obo=bg_offset; ofo=fg_offset;
 fg_offset = (fg_img.hMem+(MOUSE_OLD_Y*fg_img.width)+MOUSE_OLD_X)-(bg_img.hMem+MOUSE_OLD_Y*bg_img.width+MOUSE_OLD_X);
 bg_offset = 0; //(bg_img.hMem+(MOUSE_OLD_Y*fg_img.width)+MOUSE_OLD_X)-(bg_img.hMem+stx[MOUSE_OLD_Y+dy]+MOUSE_OLD_X)-1;

 for (dy=0;dy<h;dy++)
  {
   p_off = sts[MOUSE_OLD_Y+dy]+MOUSE_OLD_X*BytesPerPixel;
   page = stp[MOUSE_OLD_Y+dy];
   if (stb[MOUSE_OLD_Y+dy])
    if ((DWORD)sts[MOUSE_OLD_Y+dy]+(DWORD)MOUSE_OLD_X*BytesPerPixel >= (DWORD)BytesPerBank)
     {
      page++;
      p_off -= BytesPerBank;
     }

   if (CurrentBankWr != (short int)page)
     VESA_SetPage((short int)page, BANK_WR);
   
   rcpy(fg_img.hMem+stx[MOUSE_OLD_Y+dy]+MOUSE_OLD_X*BytesPerPixel, BackupSprite_fg+(dy*MOUSE_CURSOR_WIDTH*BytesPerPixel), w);
   scpy(bg_img.hMem+stx[MOUSE_OLD_Y+dy]+MOUSE_OLD_X*BytesPerPixel, fg_img.hMem+stx[MOUSE_OLD_Y+dy]+MOUSE_OLD_X*BytesPerPixel, w);

   if ((DWORD)(MOUSE_CURSOR_WIDTH + p_off)<BytesPerBank)
     rcpy(write_buffer+p_off, bg_img.hMem+stx[MOUSE_OLD_Y+dy]+MOUSE_OLD_X*BytesPerPixel, w);
   else
     {
      rcpy(write_buffer+p_off, bg_img.hMem+stx[MOUSE_OLD_Y+dy]+MOUSE_OLD_X*BytesPerPixel, (WORD)(BytesPerBank - p_off)/BytesPerPixel);
      VESA_SetPage((short int)CurrentBankWr+1, BANK_WR);
      p1 = (DWORD)(BytesPerBank-p_off);
      p2 = (DWORD)(p_off+w*BytesPerPixel)-(DWORD)BytesPerBank;
      if (p2>0)
       rcpy(write_buffer, bg_img.hMem+stx[MOUSE_OLD_Y+dy]+MOUSE_OLD_X*BytesPerPixel+p1, p2/BytesPerPixel);
     }
  }
 bg_offset=obo; fg_offset=ofo;
}

void MOUSE_SetCursor(unsigned char *c, unsigned int nw, unsigned int nh)
{
 NOBINT16 i;
 NOBINT16 *MC16;
 NOBINT32 *MC32;

 MC16 = (NOBINT16 *)&MOUSE_CURSOR;
 MC32 = (NOBINT32 *)&MOUSE_CURSOR;

 if (!resetok) return;
 _disable();
 MOUSE_Replace();

 MOUSE_CURSOR_WIDTH = nw;
 MOUSE_CURSOR_HEIGHT = nh;

 for (i=0;i<nh*nw;i++)
  {
   switch (BytesPerPixel)
    {
     case 1: MOUSE_CURSOR[i] = c[i]; break;
     case 2: MC16[i] = SVGA_ColConv[c[i]]; break;
     case 4: MC32[i] = SVGA_ColConv[c[i]]; 
    }
  }
 MOUSE_DrawCursor();
 _enable();
}

void MOUSE_SetSensitivity(int x, int y)
{
 _disable();
 if (x) MOUSE_SENSITIVITY_X = x;     /* No division by zero run time errors!! */
 if (y) MOUSE_SENSITIVITY_Y = y;
 _enable();
}

// doctor tues 1145
