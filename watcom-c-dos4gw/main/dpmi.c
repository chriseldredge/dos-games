#include "toolkit.h"

short DPMI_AllocCM(short NumPars, short *selector, short *segment)
{
 void asmacm();
 BINT16 cf,sel, segm;
 #pragma aux asmacm = \
  "mov ax, 0x0100"    \
  "mov bx, NumPars"   \
  "int 0x31"          \
  "jc  fail"          \
  "mov segm, ax"      \
  "mov sel, dx"       \
  "mov cf, 1"         \
  "jmp don"           \
  "fail: mov cf, 0"   \
  "don:"              ;

 cf = 0;
 sel = 0; segm=0;
 asmacm();
 *selector = sel;
 *segment = segm;
 return(cf);
}

int DPMI_FreeCM(int selector)
{
 void asmfcm();
 BINT16 sel;
 #pragma aux asmfcm = \
  "mov ax, 0x0101"    \
  "mov dx, sel"       \
  "int 0x31"          ;

 sel = selector;
 asmfcm();

 return(1);
}

int DPMI_LockRegion (void *address, unsigned length)
{
    union REGS regs;
    unsigned linear;

    /* Thanks to DOS/4GW's zero-based flat memory model, converting
       a pointer of any type to a linear address is trivial.
    */
    linear = (unsigned) address;

    regs.w.ax = 0x600;                           /* DPMI Lock Linear Region */
    regs.w.bx = (unsigned short) (linear >> 16); /* Linear address in BX:CX */
    regs.w.cx = (unsigned short) (linear & 0xFFFF);
    regs.w.si = (unsigned short) (length >> 16); /* Length in SI:DI */
    regs.w.di = (unsigned short) (length & 0xFFFF);
    int386 (0x31, &regs, &regs);
    return (! regs.w.cflag);                    /* Return 0 if can't lock */
}

void (__interrupt __far *DPMI_GetRealVect(int whichvect))()
{
 union REGS r;

 r.w.ax = 0x0200;
 r.h.bl = whichvect;
 int386(0x31, &r, &r);
 return(MK_FP(r.w.cx, r.w.dx));
}

int DPMI_SetRealVect(int whichvect, void (__interrupt __far *__handler)())
{
 union REGS r;
 void (far *function_ptr)();

 r.w.ax = 0x0201;
 r.h.bl = whichvect;
 function_ptr = ( void(*)(void) ) __handler;
 r.w.cx = FP_SEG(function_ptr);
 r.w.dx = FP_OFF(function_ptr);
 int386(0x31, &r, &r);
 return(!r.w.cflag);
}

int DPMI_SetProtVect(int whichvect, void (__interrupt __far *__handler)())
{
 union REGS r;
 void (far *function_ptr)();

 r.w.ax = 0x0205;
 r.h.bl = whichvect;
 function_ptr = ( void(*)(void) ) __handler;
 r.w.cx = FP_SEG(function_ptr);
 r.w.dx = FP_OFF(function_ptr);
 int386(0x31, &r, &r);
 return(!r.w.cflag);
}

void DPMI_GetFreeMem(BINT16 s, BINT32 o)
{
 void asmgetmem(void);
 #pragma aux asmgetmem = \
  "push es"              \
  "push edi"             \
  "mov ax, s"            \
  "mov es, ax"           \
  "mov eax, o"           \
  "mov edi, eax"         \
  "mov eax, 0x00000500"  \
  "int 0x31h"            \
  "pop edi"              \
  "pop es"               ;

 asmgetmem();
}


