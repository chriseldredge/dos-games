//////////////////////////////////////////////////////////////////////
// FILE:    SVGA.C                                                  //
// PURPOSE: Source code to utilize svga graphics through VESA calls //
//          and implimented routines.                               //
// REMARKS:                                                         //
//          - Any resolutions are supported, and all VESA standard  //
//            modes are defined with the syntax: [S]VGA_Xres x Yres //
//                                                                  //
//              For example: VGA_320x200                            //
//                           SVGA_640x480                           //
//                                                                  //
//          - Any resolution is available, but only in color depth  //
//            of 8-Bits (256 colors).                               //
//          - If a VESA driver is not detected, functions will fail.//
//            It is recomended that you test for errors and abort if//
//            the program cannot detect a driver.                   //
//          - The library will work fully with the mouse.  You do   //
//            not need to hide the mouse when calling routines.     //
// COMPILE: This code has been compiled under Watcom C 10.6         //
//          using the 32 bit DOS/4GW extender. Code is only         //
//          compatable with protected mode (32-bit).                //
// AUTHOR:  Chris Eldredge                                          //
//          3 Thorburn Place, Gaithersburg MD, 20878                //
//          (301)-424-0253 FAX / TELEPHONE                          //
//          <wantaco@erols.com>                                     //
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <i86.h>
#include <malloc.h>
#include <string.h>
#include <graph.h>

#define SVGA_C
#include "toolkit.h"

#pragma off (check_stack)

BINT16 lastw;
VBINT  SVGA_ButtonCheck(VBINT);

VBINT tset(BINT8 far *to, NOBINT32 c, BINT32 w)
{
 VBINT asmtset8(VBINT);
 #pragma aux asmtset8 =         \
   "les edi, far ptr [to]"      \
   "mov ecx, [w]"               \   
   "mov eax, [c]"               \   
   "mov ebx, [bg_offset]"       \   
   "mov edx, [fg_offset]"       \   
   "cld"                        \   
   "lblLoop:"                   \
   "mov [edi+ebx], al"          \
   "cmp [edi+edx], 0"           \
   "jnz lblNoPix"               \
   "stosb"                      \
   "jmp lblDone"                \
   "lblNoPix:"                  \
   "inc edi"                    \   
   "lblDone:"                   \
   "loop lblLoop"               ;

 VBINT asmtset16(VBINT);
 #pragma aux asmtset16 =        \
   "les edi, far ptr [to]"      \
   "mov ecx, [w]"               \   
   "mov eax, [c]"                \   
   "mov ebx, [bg_offset]"       \   
   "mov edx, [fg_offset]"       \   
   "cld"                        \   
   "lblLoop:"                   \
   "mov [edi+ebx], ax"          \
   "cmp WORD PTR [edi+edx], 0"  \
   "jnz lblNoPix"               \
   "stosw"                      \
   "jmp lblDone"                \
   "lblNoPix:"                  \
   "inc edi"                    \
   "inc edi"                    \
   "lblDone:"                   \
   "loop lblLoop"               ;
 
 VBINT asmtset32(VBINT);
 #pragma aux asmtset32 =        \
   "les edi, far ptr [to]"      \
   "mov ecx, [w]"               \   
   "mov eax, [c]"               \   
   "mov ebx, [bg_offset]"       \   
   "mov edx, [fg_offset]"       \   
   "cld"                        \   
   "lblLoop:"                   \
   "mov [edi+ebx], eax"         \
   "cmp DWORD PTR [edi+edx], 0" \
   "jnz lblNoPix"               \
   "stosd"                      \
   "jmp lblDone"                \
   "lblNoPix:"                  \
   "inc edi"                    \
   "inc edi"                    \
   "inc edi"                    \
   "inc edi"                    \
   "lblDone:"                   \
   "loop lblLoop"               ;

 if (w==0) enter;
 switch (BytesPerPixel)
  {
   case 1: asmtset8(); break;
   case 2: asmtset16(); break;
   case 4: asmtset32(); break;
  }
}

VBINT scpy(BINT8 far *to, BINT8 far *from, NOBINT32 w)
{
                               
 VBINT asmscpy8(VBINT);
 #pragma aux asmscpy8 =         \
   "les edi, far ptr [to]"      \
   "lds esi, far ptr [from]"    \
   "mov ecx, [w]"               \
   "cld"                        \
   "Lbl1:"                      \
   "lodsb"                      \
   "cmp al, 0"                  \
   "jz lblNoPix"                \
   "stosb"                      \
   "jmp lblDone"                \
   "lblNoPix:"                  \
   "inc edi"                    \
   "lblDone:"                   \
   "loop lbl1"                  ;
 VBINT asmscpy16(VBINT);
 #pragma aux asmscpy16 =        \
   "les edi, far ptr [to]"      \
   "lds esi, far ptr [from]"    \
   "mov ecx, [w]"               \
   "cld"                        \
   "Lbl1:"                      \
   "lodsw"                      \
   "cmp ax, 0"                  \
   "jz lblNoPix"                \
   "stosw"                      \
   "jmp lblDone"                \
   "lblNoPix:"                  \
   "inc edi"                    \
   "inc edi"                    \
   "lblDone:"                   \
   "loop lbl1"                  ;
 VBINT asmscpy32(VBINT);
 #pragma aux asmscpy32 =        \
   "les edi, far ptr [to]"      \
   "lds esi, far ptr [from]"    \
   "mov ecx, [w]"               \
   "cld"                        \
   "Lbl1:"                      \
   "lodsd"                      \
   "cmp ax, 0"                  \
   "jz lblNoPix"                \
   "stosd"                      \
   "jmp lblDone"                \
   "lblNoPix:"                  \
   "inc edi"                    \
   "inc edi"                    \
   "inc edi"                    \
   "inc edi"                    \
   "lblDone:"                   \
   "loop lbl1"                  ;

 if (w==0) enter;
 switch (BytesPerPixel)
  {
   case 1: asmscpy8(); break;
   case 2: asmscpy16(); break;
   case 4: asmscpy32(); break;
  }
}

VBINT tcpy(BINT8 far *to, BINT8 far *from, NOBINT32 w)
{
                               
 VBINT asmtcpy8(VBINT);
 #pragma aux asmtcpy8 =         \
   "les edi, far ptr [to]"      \
   "lds esi, far ptr [from]"    \
   "mov ecx, [w]"               \
   "mov ebx, [fg_offset]"       \
   "mov edx, [bg_offset]"       \
   "cld"                        \
   "Lbl1:"                      \
   "lodsb"                      \
   "mov ds:[esi+edx], al"       \
   "cmp ds:[esi+ebx], 0"        \
   "jnz lblNoPix"               \
   "stosb"                      \
   "jmp lblDone"                \
   "lblNoPix:"                  \
   "inc edi"                    \
   "lblDone:"                   \
   "loop lbl1"                  ;
 VBINT asmtcpy16(VBINT);
 #pragma aux asmtcpy16 =        \
   "les edi, far ptr [to]"      \
   "lds esi, far ptr [from]"    \
   "mov ecx, [w]"               \
   "mov ebx, [fg_offset]"       \
   "mov edx, [bg_offset]"       \
   "cld"                        \
   "Lbl1:"                      \
   "lodsw"                      \
   "mov ds:[esi+edx], ax"       \
   "cmp WORD PTR ds:[esi+ebx], 0"\
   "jnz lblNoPix"               \
   "stosw"                      \
   "jmp lblDone"                \
   "lblNoPix:"                  \
   "inc edi"                    \
   "inc edi"                    \
   "lblDone:"                   \
   "loop lbl1"                  ;

 VBINT asmtcpy32(VBINT);
 #pragma aux asmtcpy32 =        \
   "les edi, far ptr [to]"      \
   "lds esi, far ptr [from]"    \
   "mov ecx, [w]"               \
   "mov ebx, [fg_offset]"       \
   "mov edx, [bg_offset]"       \
   "cld"                        \
   "Lbl1:"                      \
   "lodsd"                      \
   "mov ds:[esi+edx], eax"      \
   "cmp DWORD PTR ds:[esi+ebx], 0"\
   "jnz lblNoPix"               \
   "stosd"                      \
   "jmp lblDone"                \
   "lblNoPix:"                  \
   "inc edi"                    \
   "inc edi"                    \
   "inc edi"                    \
   "inc edi"                    \
   "lblDone:"                   \
   "loop lbl1"                  ;


 if (w==0) enter;
 switch (BytesPerPixel)
  {
   case 1: asmtcpy8(); break;
   case 2: asmtcpy16(); break;
   case 4: asmtcpy32(); break;
  }
}
   

VBINT tscpy(BINT8 far *to, BINT8 far *from, NOBINT32 w)
{
                               
 VBINT asmtscpy8(VBINT);
 #pragma aux asmtscpy8 =         \
   "les edi, far ptr [to]"      \
   "lds esi, far ptr [from]"    \
   "mov ebx, [fg_offset]"       \
   "mov edx, [bg_offset]"       \
   "mov ecx, [w]"               \
   "cld"                        \
   "Lbl1:"                      \
   "lodsb"                      \
   "cmp al, 0"                  \
   "jz lblNoPix"                \
   "mov ds:[esi+edx], al"       \
   "cmp ds:[esi+ebx], 0"        \
   "jnz lblNoPix"               \
   "stosb"                      \
   "jmp lblDone"                \
   "lblNoPix:"                  \
   "inc edi"                    \
   "lblDone:"                   \
   "loop lbl1"                  ;
 VBINT asmtscpy16(VBINT);
 #pragma aux asmtscpy16 =       \
   "les edi, far ptr [to]"      \
   "lds esi, far ptr [from]"    \
   "mov ebx, [fg_offset]"       \
   "mov edx, [bg_offset]"       \
   "mov ecx, [w]"               \
   "cld"                        \
   "Lbl1:"                      \
   "lodsw"                      \
   "cmp ax, 0"                  \
   "jz lblNoPix"                \
   "mov ds:[esi+edx], ax"       \
   "cmp WORD PTR ds:[esi+ebx], 0"\
   "jnz lblNoPix"               \
   "stosw"                      \
   "jmp lblDone"                \
   "lblNoPix:"                  \
   "inc edi"                    \
   "inc edi"                    \
   "lblDone:"                   \
   "loop lbl1"                  ;
 VBINT asmtscpy32(VBINT);
 #pragma aux asmtscpy32 =       \
   "les edi, far ptr [to]"      \
   "lds esi, far ptr [from]"    \
   "mov ebx, [fg_offset]"       \
   "mov edx, [bg_offset]"       \
   "mov ecx, [w]"               \
   "cld"                        \
   "Lbl1:"                      \
   "lodsd"                      \
   "cmp eax, 0"                 \
   "jz lblNoPix"                \
   "mov DWORD PTR ds:[esi+edx], eax"       \
   "cmp DWORD PTR ds:[esi+ebx], 0"\
   "jnz lblNoPix"               \
   "stosd"                      \
   "jmp lblDone"                \
   "lblNoPix:"                  \
   "inc edi"                    \
   "inc edi"                    \
   "inc edi"                    \
   "inc edi"                    \
   "lblDone:"                   \
   "loop lbl1"                  ;

 if (w==0) enter;
 switch (BytesPerPixel)
  {
   case 1: asmtscpy8(); break;
   case 2: asmtscpy16(); break;
   case 4: asmtscpy32(); break;
  }
}

VBINT rcpy(BINT8 far *to, BINT8 far *from, NOBINT32 wi)
{
 NOBINT32 w;                              
 VBINT asmcpy(VBINT);
 #pragma aux asmcpy =           \
   "les edi, far ptr [to]"      \
   "lds esi, far ptr [from]"    \
   "mov ecx, [w]"               \
   "mov ebx, ecx"               \
   "and ebx, 1"                 \
   "cmp ebx, 1"                 \
   "jnz lblnb"                  \
   "movsb"                      \
   "lblnb:"                     \
   "mov ebx, ecx"               \
   "and ebx, 2"                 \
   "cmp ebx, 2"                 \
   "jnz lblnw"                  \
   "movsw"                      \
   "lblnw:"                     \
   "shr ecx, 2"                 \
   "cld"                        \
   "rep movsd"                  ;

 if (wi==0) enter;
 w = wi*BytesPerPixel;
 asmcpy();
}

VBINT cpyandc(BINT8 far *to, BINT8 far *from, NOBINT32 wi)
{
 NOBINT32 w, m;

 VBINT asmandcpy(VBINT);
 #pragma aux asmandcpy =        \
   "les edi, far ptr [to]"      \
   "lds esi, far ptr [from]"    \
   "mov ecx, [w]"               \
   "mov ebx, ecx"               \
   "and ebx, 1"                 \
   "mov edx, [m]"               \
   "cmp ebx, 1"                 \
   "jnz lblnb"                  \
   "lodsb"                      \
   "and al, dl"                 \
   "stosb"                      \
   "lblnb:"                     \
   "mov ebx, ecx"               \
   "and ebx, 2"                 \
   "cmp ebx, 2"                 \
   "jnz lblnw"                  \
   "lodsw"                      \
   "and ax, dx"                 \
   "stosw"                      \
   "lblnw:"                     \
   "shr ecx, 2"                 \
   "cld"                        \
   "lblLpC:"                    \
   "lodsd"                      \
   "and eax, edx"               \
   "stosd"                      \
   "loop lblLpC"                ;

 if (wi==0) enter;
 w = wi*BytesPerPixel;
 switch (BytesPerPixel)
  {
   case 1:
     m = SVGA_Mask + SVGA_Mask<<8 + SVGA_Mask<<16 + SVGA_Mask<<24;
     break;
   case 2:
     m = (SVGA_Mask & 0xFFFF) + (SVGA_Mask<<16);
  }
 asmandcpy();
}


BINT32 mul(BINT32 i, BINT32 ii, BINT16 bit)
{
 BINT32 asm_mul(VBINT);
 #pragma aux asm_mul =          \
   "mov     eax, i"             \
   "imul    dword ptr [ii]"     \
   "mov     cx, bit"            \
   "cmp     cx, 0020H"          \
   "jb      L1"                 \
   "mov     eax,edx"            \
   "mov     edx, 00000000H"     \
   "sub     cx, 0020H"          \
   "L1:"                        \
   "shrd    eax,edx,cl"         \
   "js      L2"                 \
   "sar     edx,cl"             \
   "jne     L15"                \
   "shld    edx,eax,10H"        \
   "jmp     L16"                \
   "L2:"                        \
   "sar     edx,cl"             \
   "inc     edx"                \
   "jne     L15"                \
   "shld    edx,eax,10H"        \
   "jmp     L16"                \
   "L15:"                       \
   "mov     ax,0ffffH"          \
   "mov     dx,7fffH"           \
   "L16:"                       ;

 enter(asm_mul());
}

VBINT VESA_SetPage(NOBINT16 Page, NOBINT8 window)
{
/* VBINT asmsetpage(VBINT);
 #pragma aux asmsetpage = \
    "mov eax, 0x4F05"     \
    "xor ebx, ebx"        \
    "xor edx, edx"        \
    "xor ecx, ecx"        \
    "mov bl, [window]"    \
    "mov dx, [Page]"      \
    "int 0x10"            ;

 if (winattr==1)
  CurrentBankWr = CurrentBankRd = Page;
 else if (window==0)
  CurrentBankWr = Page;
 else
  CurrentBankRd = Page;
 asmsetpage();*/

 VBINT asmsetpage_s3(VBINT);
 #pragma aux asmsetpage_s3 =    \
    "mov dx, 3d4h"              \
    "mov al, 35h"               \
    "out dx, al"                \
    "inc dx"                    \
    "in  al, dx"                \
    "and al, 0f0h"              \
    "mov bx, [Page]"            \
    "mov bh, bl"                \
    "and bl, 0fh"               \
    "or  al, bl"                \
    "out dx, al"                \
    "in  al, dx"                \
/*
    "mov al, 67h"               \
    "out dx, al"                \
    "inc dx"                    \
    "in  al, dx"                \
    "cmp bh, 15"                \
    "jle rsy"                   \
    "or  al, 8"                 \
    "jmp wrt"                   \
    "rsy:"                      \
    "or  al, 8"                 \
    "xor al, 8"                 \
    "wrt:"                      ;



    "dec dx"                    \
    "mov al, 31h"               \
    "out dx, al"                \
    "inc dx"                    \
    "in  al, dx"                \
    "cmp bh, 15"                \
    "jle rsy"                   \
    "or  al, 1"                 \
    "jmp wrt"                   \
    "rsy:"                      \
    "or  al, 1"                 \
    "xor al, 1"                 \
    "wrt:"                      \
    "out dx, al"                ;
*/



  CurrentBankWr = CurrentBankRd = Page;
  asmsetpage_s3();
}

NOBINT8 VESA_SetMode(NOBINT16 Mode)
{
 ONION REGS r;

 r.h.ah = 0x4F;
 r.h.al = 2;
 r.w.bx = Mode;
 if (Mode==VGA_320x200)
  r.w.ax = VGA_320x200;

 int386(0x10, &r, &r);

 if (Mode==VGA_320x200)
  enter(1);
 else if (r.h.al == 0x4F)
  enter(1);
 else
  enter(0);
}

NOBINT8 VESA_ReturnInfo(VESAINFO *Buffer)
{       
 ONION REGS   r;
 RMTREGS      tr;
 STRUT SREGS  sr;
 BINT16       selector, segment;
 NOBINT16     *p;
 NOBINT32     l1,l2;

 if (!DPMI_AllocCM((sizeof(VESAINFO)/16+10), &selector, &segment)) 
  {
   enter(0);
  }
 
 memset(&tr, 0, sizeof(RMTREGS));

 tr.eax = 0x4F00;           // setup translation registers
 tr.es  = segment;
 tr.edi = 0;

 segread(&sr);
 r.w.ax = 0x0300;
 r.h.bl = 0x10;
 r.h.bh = 0;
 r.w.cx = 0;
 sr.es = FP_SEG(&tr);
 r.x.edi = FP_OFF(&tr);

 int386x(0x31, &r, &r, &sr);

 _fmemcpy(Buffer, MK_FP(selector,0), sizeof(VESAINFO));

 DPMI_FreeCM(selector);
 
 // Translate OEM string to flat model
 Buffer->oem_string = (BINT8 *)((REAL_SEG(Buffer->oem_string)<<4)+REAL_OFF(Buffer->oem_string));

 // Translate the pointer to flat model
 p = (NOBINT16 *)((REAL_SEG(Buffer->video_mode)<<4)+(REAL_OFF(Buffer->video_mode)));

 l1 = (NOBINT32) (p);
 l2 = (NOBINT32) (segment<<4);

 if ((l1>l2)&&(l1-l2<sizeof(VESAINFO)))
  {                                    // They are pointing to the old buffer!
   p = (NOBINT16 *)(Buffer);           // Pointer to new buffer!
   p += (l1-l2)/sizeof(NOBINT16);
  }

 Buffer->video_mode=p;

 enter(1);
}

NOBINT8 VESA_ReturnModeInfo(NOBINT16 Mode, VESAMODEINFO *Buf)
{
 ONION REGS  r;
 RMTREGS     tr;
 STRUT SREGS sr;
 BINT16      selector, segment;
 
 if (!DPMI_AllocCM((sizeof(VESAMODEINFO)/16+10), &selector, &segment))
  enter(0);
  
 memset(&tr, 0, sizeof(RMTREGS));

 tr.eax = 0x4F01;           // setup translation registers
 tr.ecx = Mode;
 tr.es = segment;
 tr.edi = 0;

 r.x.eax = 0x0300;
 r.x.ebx = 0x10;
 r.x.ecx = 0;
 segread(&sr);
 sr.es = FP_SEG(&tr);
 r.x.edi = FP_OFF(&tr);
 
 int386x(0x31, &r, &r, &sr);
 
 _fmemcpy(Buf, MK_FP(selector,0), sizeof(VESAMODEINFO));

 DPMI_FreeCM(selector);
 
 if (tr.eax & 0xFF000000 == 0x00)
  enter(1);
 else
  enter(0);
}

NOBINT8 SVGA_Init(VBINT)
{
 VESAINFO v;
 NOBINT16 i;

 if (!no_vesa)
  {
   VESA_ReturnInfo(&v);
   if (strncmp(v.signature, "VESA", 4)!=NULL)
    enter(0);
  }

 CurrentBankWr = CurrentBankRd = 0xFFFF;

 CurrentFont = (FONT_PTR)malloc(sizeof(FONT));
 for (i=0;i<256;i++)
  CurrentFont->map[i] = i;
 CurrentFont->romset=1;
 CurrentFont->height = 8;
 CurrentFont->max_chars = 255;
 CurrentFont->BitsPerPixel = 1;
  
 enter(1);
}

void InitSinTable(void);

BINT8 SVGA_SetupMode(NOBINT16 VMode)
{
 VESAMODEINFO vm;
 NOBINT16 j=0,i=0;
 NOBINT8 page=0;
 NOBINT32 runner=0L;

 if (!SVGA_Init()) enter(-1);

 if (VMode==VGA_320x200)
  {
   MaxX = 319; MaxY = 199;
   BytesPerLine = 320;
   BytesPerBank = 65535;
   write_buffer = (NOBINT8 *)(0xA000 << 4);
   read_buffer =  (NOBINT8 *)(0xA000 << 4);
   winattr=1;
  }
 else
  {
   VESA_ReturnModeInfo(VMode, &vm);
   BytesPerLine = vm.bytes_per_scan_line;
   BytesPerBank = (DWORD)vm.granularity*1024;
   BytesPerPixel = vm.bits_per_pixel / 8;
   if (vm.bits_per_pixel==15)
    BytesPerPixel=2;
   MaxX = vm.x_res-1;
   MaxY = vm.y_res-1;
   WindowControl = (REAL_SEG(vm.page_switch_function_ptr)<<4)+REAL_OFF(vm.page_switch_function_ptr);

   write_buffer = (NOBINT8 *)(0xA000 << 4);
   if (vm.win_a_attr & 4)
    {
     read_buffer =  (NOBINT8 *)(0xA000 << 4);
     winattr = 1;
    }
  if (vm.win_b_attr & 4)
    {
     read_buffer =  (NOBINT8 *)(0xAF000 << 4);
     winattr=0;
    }
//   else
//   printf("\7\n");        // Shouldnt happen... 
  }

 write_buffer16 = (NOBINT16 *)write_buffer;
 write_buffer32 = (NOBINT32 *)write_buffer;

 if (BytesPerPixel==4)
  MaxC = 1<<24;
 else
  MaxC = 1<<(BytesPerPixel*8);

 ClipX1 = 0;
 ClipY1 = 0;
 ClipX2 = MaxX;
 ClipY2 = MaxY;
 
 go_until_this_is_over (i=0;i<360;i++)
  {
   CosTable[i] = cos((i*PI)/180);
   SinTable[i] = sin((i*PI)/180);
  }
 InitSinTable();

 go_until_this_is_over (j=0; j<=MaxY; j++)
  {
   stx[j] = j * BytesPerLine;
   sts[j] = (NOBINT16)runner;
   if (runner+BytesPerLine >= BytesPerBank)
    {
     stb[j] = 1;
     runner -= BytesPerBank;
    }
   else
    stb[j] = 0;

    if (j != 0 && stb[j-1] == 1)
       page++;

    stp[j] = page;
    runner += BytesPerLine;
   }

 if (!VESA_SetMode(VMode))
    enter(-2);
 else
    {
     SVGA_InitImage(&bg_img, MaxX+1, MaxY+1);
     SVGA_ClearImage(&bg_img, 0);
     SVGA_InitImage(&fg_img, MaxX+1, MaxY+1);
     SVGA_ClearImage(&fg_img, 0);
     SVGA_Cls(0);
     enter(0);
    }
}

NOBINT16 SVGA_GetUserMode(BINT16 bpp)
{              
 VESAINFO       v;
 VESAMODEINFO   vm;
 NOBINT16       i;
 NOBINT16       *vesamode;
 NOBINT16       modes[20];
 NOBINT8        tmodes, tdn;
   
 if (no_vesa) return(VGA_320x200);
 tmodes = 0;
 if (bpp==1)
  {
   modes[1] = VGA_320x200;  // All cards support this mode (Not in VESA).
   tmodes = 1;
   printf("(1) 320x200\n");
  }
 VESA_ReturnInfo(&v);
 vesamode = v.video_mode;

 if (bpp & 8)
  while (*vesamode != 0xFFFF)
   {
    VESA_ReturnModeInfo(*vesamode, &vm);
    if (vm.bits_per_pixel==8)
     {
      modes[++tmodes] = *vesamode;
      if (*vesamode==SVGA_640x480)
       {
        printf("(%i) 640x480 (Default)\n", tmodes);
        tdn = tmodes;
       }
      else
       printf("(%i) %ix%ix%i\n", tmodes, vm.x_res, vm.y_res, vm.bits_per_pixel);
     }
    vesamode++;
   }
 vesamode = v.video_mode;

 if (bpp & 16)
  while (*vesamode != 0xFFFF)
   {
    VESA_ReturnModeInfo(*vesamode, &vm);
    if (vm.bits_per_pixel==16)
     {
      modes[++tmodes] = *vesamode;
      printf("(%i) %ix%ix%i\n", tmodes, vm.x_res, vm.y_res, vm.bits_per_pixel);
     }
    vesamode++;
   }

 vesamode = v.video_mode;

 if (bpp & 32)
  while (*vesamode != 0xFFFF)
   {
    VESA_ReturnModeInfo(*vesamode, &vm);
    if (vm.bits_per_pixel==32)
     {
      modes[++tmodes] = *vesamode;
      printf("(%i) %ix%ix%i\n", tmodes, vm.x_res, vm.y_res, vm.bits_per_pixel);
     }
    vesamode++;
   }
 
 _outtext("\nEnter the index number of the mode you want to use: ");

 i=0;
 while (((i < '1')||(i > '0'+tmodes))&&(i != 13))
  i = getch();

 if (i==13) i = '0'+tdn;
 printf("%c\n", i);
 
 delay(500);
 
 if (i==13)
  enter(SVGA_640x480);
 else
  enter(modes[i-'0']);
}

VBINT SVGA_PutPixel(NOBINT16 Xc, NOBINT16 Yc, NOBINT32 color)
{
 NOBINT16 bank;
 NOBINT32 pixel_pos;
 NOBINT16 *vid16;
 NOBINT32 *vid32;

 pixel_pos = sts[Yc]+(NOBINT32)(Xc*BytesPerPixel);
 bank = stp[Yc];
 if (stb[Yc])
  if ((DWORD)sts[Yc]+(DWORD)Xc*BytesPerPixel >= (DWORD)BytesPerBank)
    {
     bank++;
     pixel_pos -= BytesPerBank;
    }

 if (CurrentBankWr != bank)
   VESA_SetPage((NOBINT16)bank, BANK_WR);

 switch (BytesPerPixel)
  {
   case 1:
    bg_img.hMem[stx[Yc]+Xc] = color;
    if (!fg_img.hMem[stx[Yc]+Xc])
     write_buffer[pixel_pos] = lo(color);
    break;
   case 2:
    vid16 = (NOBINT16 *)bg_img.hMem;
    vid16[stx[Yc]/2+Xc] = color;
    vid16 = (NOBINT16 *)fg_img.hMem;
    if (!vid16[stx[Yc]/2+Xc])
     write_buffer16[pixel_pos/2] = color;
    break;
   case 4:
    vid32 = (NOBINT32 *)bg_img.hMem;
    vid32[stx[Yc]/4+Xc] = color;
    vid32 = (NOBINT32 *)fg_img.hMem;
    if (!vid32[stx[Yc]/4+Xc])
     write_buffer32[pixel_pos/4] = color;
    break;
  }
}

NOBINT32 SVGA_GetPixel(NOBINT16 Xc, NOBINT16 Yc)
{
 NOBINT16 *vid16;
 NOBINT32 *vid32;

 switch (BytesPerPixel)
  {
   case 1: return(bg_img.hMem[stx[Yc]+Xc]);
   case 2: vid16 = (NOBINT16 *)bg_img.hMem;
           return(vid16[stx[Yc]/2+Xc]);
   case 4: vid32 = (NOBINT32 *)bg_img.hMem;
           return(vid32[stx[Yc]/4+Xc]);
  }
 return(bg_img.hMem[stx[Yc]+Xc]);               // No return for Get Pixel. Bla Bla Bla.
}

VBINT SVGA_SetFont(FONT_PTR f)
{
 CurrentFont = f;
}

VBINT SVGA_PrintChar(NOBINT16 xc,NOBINT16 yc, NOBINT32 color, STR c, NOBINT32 trans_flag)
{
 NOBINT16     x,y,i;
 NOBINT8      *work_char;
 NOBINT8      bit_mask = 0x80;
 FONTCHAR_PTR fc;

 if (CurrentFont->romset||CurrentFont->BitsPerPixel!=8)
  {
   work_char =  (NOBINT8 *)(0xF000 << 4) + 0xFA6EL + c * 8;
   lastw=8;
   go_until_this_is_over (y=0; y<8; y++)
    {
     bit_mask = 0x80;

     go_until_this_is_over (x=0; x<8; x++)
      {

        if ((*work_char & bit_mask))
             SVGA_PutPixel(xc+x, yc+y, (NOBINT8)color);
        else if (trans_flag!=0)  // takes care of transparency
             SVGA_PutPixel(xc+x, yc+y, trans_flag);

        bit_mask = (bit_mask>>1);

      } // { go_until_this_is_over x

     work_char++;

    } // { go_until_this_is_over y
  }
 else
  {
   fc = (FONTCHAR_PTR)&CurrentFont->startchar;      

   for (i=0;i<c;i++)
     fc = (FONTCHAR_PTR)fc->next_char;
 
   lastw = fc->width+1;

   work_char = fc->data;

   go_until_this_is_over (y=0;y<fc->height;y++)
     go_until_this_is_over (x=0;x<fc->width;x++)
      if (work_char[(y*fc->width)+x]!=0)
       SVGA_PutPixel(xc+x, yc+y, SVGA_ColConv[work_char[(y*fc->width)+x]]);
      else if (trans_flag)
       SVGA_PutPixel(xc+x, yc+y, trans_flag);
  }
}

VBINT SVGA_PrintString(NOBINT16 x,NOBINT16 y, NOBINT32 color, BINT8 *string, NOBINT32 trans_flag)
{
 NOBINT16 index, wi=0;

 go_until_this_is_over (index=0; string[index]!=0; index++)
  {
   if (CurrentFont->romset)
    SVGA_PrintChar(x+wi,y,color, (NOBINT8)string[index], trans_flag);
   else
    SVGA_PrintChar(x+wi,y,color,(NOBINT8)MAP[string[index]], trans_flag);
   wi+=lastw;
  } 
}

NOBINT16 SVGA_TextWidth(BINT8 *str)
{
 FONTCHAR_PTR fc;
 NOBINT16     i, j, w=0;
 
 if (strlen(str)==0) return(0);
 if (CurrentFont->romset==1||CurrentFont->BitsPerPixel!=8) return(strlen(str)*8);

 go_until_this_is_over (i=0;i<strlen(str);i++)
  {
   fc = (FONTCHAR_PTR)&CurrentFont->startchar;
   go_until_this_is_over (j=0;j<CurrentFont->map[str[i]];j++)
    fc = (FONTCHAR_PTR)fc->next_char;
   w += fc->width+1;
  }
 return(w);
}


BINT8 Sgn(float A)
{
 if (A > 0) enter(+1);
 if (A < 0) enter(-1);
 enter(0);
}               

VBINT SVGA_Line(NOBINT16 x1, NOBINT16 y1, NOBINT16 x2, NOBINT16 y2, NOBINT32 color)
{
 BINT16 I, S, D1x, D1y, D2x, D2y, U, V, M, N, ex1, ex2, wy1, wy2;

 ex1 = x1, ex2=x2,wy1=y1,wy2=y2;
// if (!SVGA_ClipLine(&ex1, &wy1, &ex2, &wy2)) enter;
 
 U = ex2 - ex1;
 V = wy2 - wy1;
 D1x = Sgn(U);
 D1y = Sgn(V);
 D2x = Sgn(U);
 D2y = 0;
 M = abs(U);
 N = abs(V);
 if (!(M>N))
  {
   D2x = 0;
   D2y = Sgn(V);
   M   = abs(V);
   N  = abs(U);
  }
 S = M >> 1;

 go_until_this_is_over (I=0;I<=M;I++)
   {
    SVGA_PutPixel(x1, y1, color);
    S = S + N;
    if (!(S<M))
      {
       S  -= M;
       x1 += D1x;
       y1 += D1y;
      }
     else
      {
       x1 += D2x;
       y1 += D2y;
      }
   }
}

VBINT SVGA_HLine(NOBINT16 x1, NOBINT16 x2, NOBINT16 y1, NOBINT32 color)
{
 noneggy long ofs, width;
 NOBINT16 bank;
  
 ofs = ((long)(y1)*BytesPerLine)+(long)(x1*BytesPerPixel);
 bank = (ofs / BytesPerBank);
 ofs -= ((long)(bank)*BytesPerBank);

 fg_offset = (fg_img.hMem + stx[y1] + x1*BytesPerPixel) - (write_buffer+ofs);
 bg_offset = (bg_img.hMem + stx[y1] + x1*BytesPerPixel) - (write_buffer+ofs);
 
 if (CurrentBankWr != bank)
   VESA_SetPage((NOBINT8)bank, BANK_WR);

 width = (x2-x1)+1;
 if ((long)(ofs + width*BytesPerPixel) > BytesPerBank)
  {
   tset(write_buffer + ofs, color, (NOBINT16)(BytesPerBank-ofs)/BytesPerPixel);
   VESA_SetPage(CurrentBankWr+1, BANK_WR);
   fg_offset = (fg_img.hMem + CurrentBankWr*BytesPerBank) - (write_buffer);
   bg_offset = (bg_img.hMem + CurrentBankWr*BytesPerBank) - (write_buffer);
   tset(write_buffer, color, (NOBINT16)((ofs+width*BytesPerPixel)-BytesPerBank)/BytesPerPixel);
  }
 else
  tset(write_buffer + ofs, color,  width);
}

VBINT SVGA_Rectangle(NOBINT16 x1, NOBINT16 y1, NOBINT16 x2, NOBINT16 y2, NOBINT32 color, NOBINT16 trans)
{
 NOBINT16 index;
 
 if (trans)
   go_until_this_is_over (index=y1;index<=y2;index++)
     SVGA_HLine(x1, x2, index, color);
 else
   {
    SVGA_HLine(x1, x2, y1, color);
    SVGA_HLine(x1, x2, y2, color);
    SVGA_Line(x1, y1, x1, y2, color);
    SVGA_Line(x2, y1, x2, y2, color);
   }
}

VBINT SVGA_Cls(NOBINT32 bc)
{
 NOBINT16  i;

 _disable();
 for (i=0;i<MaxY+1;i++)
  SVGA_HLine(0, MaxX, i, bc);
 _enable();
}

NOBINT8 SVGA_LoadFont(NOBINT8 *fn, FONT_PTR f)
{
 FILE         *fp;
 NOBINT8      Dummy[50];
 NOBINT16     index;
 FONTCHAR_PTR fc;

 if ((fp = fopen(fn, "rb"))==0)
  enter(0);

 go_until_this_is_over (index=0;index<8;index++)
  Dummy[index] = getc(fp);
 Dummy[8]=0;
 
 if (strcmp(Dummy, "MAKEFONT")!=NULL)      // Makefont?
  enter(0);
 
 f->max_chars = 0;
 f->width = 0; f->height = 0;
 f->BitsPerPixel = 8;
 fc = &f->startchar;

 while (1)
  {
   fc->width = getc(fp);
   fc->height = getc(fp);

   if ((fc->width=='M')&&(fc->height=='A')) break;

   fc->data = malloc(fc->width*fc->height);
   fread(fc->data, fc->width*fc->height, 1, fp);

   if (fc->width>f->width) f->width = fc->width;
   if (fc->height>f->height) f->height = fc->height;

   fc->next_char = malloc(sizeof(FONTCHAR));
   fc = (FONTCHAR_PTR)fc->next_char;
   f->max_chars++;
  }

 fc->width = fc->height = 0;
 fc->data = fc->next_char = NULL;
 
 for (index=0;index<6;index++)
  getc(fp);
 
 
 fread(&f->map, 256, 1, fp);
 
 fclose(fp);

 enter(1);
}

NOBINT8 SVGA_LoadPCX(NOBINT8 *fn, IMAGE_PTR img)
{
 FILE     *fp;
 STR      fitolo[100];
 Pointer  temp;
 NOBINT32 colconv[256];
 NOBINT16 i, j, k, idt=0, w, plane, count, wrk;
 NOBINT8  c, l;
 NOBINT8  *wptr;
 BINT32   Offset;
 PCXHEAD  h;

// if (img->in_use==1) SVGA_KillImage(img);
 img->in_use = 1;

 strcpy(fitolo, fn);
 if ((fp = fopen(fitolo, "rb"))==NULL)
  {
   strcat(fitolo, ".PCX");
   if ((fp = fopen(fitolo, "rb"))==NULL)
    enter(0);
  }

 temp = (NOBINT8 *)&h;
 go_until_this_is_over (i=0;i<128;i++) temp[i] = fgetc(fp);

 if ((BytesPerPixel==1)&&(h.BitsPerPixel*h.colour_planes/8!=1))
  return(0);
 img->bytesperpixel = BytesPerPixel;

 if (h.colour_planes==1)
  {
   fseek(fp, -768L, SEEK_END);
   fread((NOBINT8 *)&img->palette, 768, 1, fp);

   for (i=0; i<256; i++)
    {
     if (img->bytesperpixel==4)
      {
       colconv[i] = ((NOBINT32)img->palette.color[i].blue) +
                    ((NOBINT32)img->palette.color[i].green)<<8 +
                    ((NOBINT32)img->palette.color[i].red)<<16;
      }
     else if (img->bytesperpixel==2)
      {
       colconv[i] = ((NOBINT16)img->palette.color[i].red)>>3 +
                    ((NOBINT16)img->palette.color[i].green>>2)<<5 +
                    ((NOBINT16)img->palette.color[i].blue>>3)<<11;
      }
     else
      {
       img->palette.color[i].red   = img->palette.color[i].red   >> 2;
       img->palette.color[i].green = img->palette.color[i].green >> 2;
       img->palette.color[i].blue  = img->palette.color[i].blue  >> 2;
      }
    }
  }

 fseek(fp, sizeof(PCXHEAD), SEEK_SET);

 img->width = (h.x2 - h.x1) + 1;
 img->height = (h.y2 - h.y1) + 1;
 w = h.BytesPerLine;
 if (img->width!=w) idt=1;

 if (idt)
  img->hMem = malloc((NOBINT32)w * (img->height+5) * BytesPerPixel);
 else
  img->hMem = malloc((NOBINT32)img->width * img->height * BytesPerPixel);  

 Offset=0;
 j=0;

 while (Offset<w*img->height)
  {
   if (kbhit()) break;
   for (plane=0;plane<h.colour_planes;plane++)
     {
      wptr = img->hMem + Offset*img->bytesperpixel;
      for (k=0; k<w;)
       {
//        if (k+Offset>w*img->height) break;
        c = fgetc(fp);
        if (c > 191)
         {
          l = c - 192;
          c = fgetc(fp);
     
          for (count=0;count<l;count++)
           {
            if (h.colour_planes==1)
             {
              if (img->bytesperpixel==1)
               *wptr=c;
              else if (img->bytesperpixel==4)
               {
                wptr[0]=img->palette.color[c].blue;
                wptr[1]=img->palette.color[c].green;
                wptr[2]=img->palette.color[c].red;
                wptr[3]=0;
               }
              else
               {
                wptr[1] = (img->palette.color[c].red>>3)<<3; 
                wrk = img->palette.color[c].green;
                wrk >>= 2;
                wrk <<= 5;
                wptr[0] = lo(wrk);
                wptr[1] += hi(wrk);  
                wptr[0] += (img->palette.color[c].blue>>3);     
               }
             }
            else if (img->bytesperpixel==4)
             {
              switch (plane)
               {
                case 2: *wptr = c; break;
                case 1: wptr[1] = c; break;
                case 0: wptr[2] = c; break;
               }
              wptr[3] = 0;
             }
            else
             {
              switch (plane)
               {
                case 2: wptr[0] += (c>>3);     break;
                case 1: {
                         wrk = c;
                         wrk >>= 2;
                         wrk <<= 5;
                         wptr[0] = lo(wrk);
                         wptr[1] += hi(wrk);  break;
                        }
                case 0: wptr[1] = (c>>3)<<3; break;
               }
             }

            wptr+=BytesPerPixel;
            k++;
           }
         }
        else
         {
          if (h.colour_planes==1)
           {
            if (img->bytesperpixel==1)
             *wptr=c;
            else if (img->bytesperpixel==4)
             {
              wptr[0]=img->palette.color[c].blue;
              wptr[1]=img->palette.color[c].green;
              wptr[2]=img->palette.color[c].red;
              wptr[3]=0;
             }
            else
             {
              wptr[1] = (img->palette.color[c].red>>3)<<3; 
              wrk = img->palette.color[c].green;
              wrk >>= 2;
              wrk <<= 5;
              wptr[0] = lo(wrk);
              wptr[1] += hi(wrk);  
              wptr[0] += (img->palette.color[c].blue>>3);     
             }
           }
          else if (img->bytesperpixel==4)
           {
            switch (plane)
              {
                case 2: *wptr = c; break;
                case 1: wptr[1] = c; break;
                case 0: wptr[2] = c; break;
              }
              wptr[3] = 0;
             }
            else if (img->bytesperpixel==2)
             {
              switch (plane)
               {
                case 2: wptr[0] += (c>>3);     break;
                case 1: {
                         wrk = c;
                         wrk >>= 2;
                         wrk <<= 5;
                         wptr[0] = lo(wrk);
                         wptr[1] += hi(wrk);  break;
                        }
                case 0: wptr[1] = (c>>3)<<3; break;
               }
             }
            else
             *wptr=c;

          wptr+=BytesPerPixel;
          k++;
         }
       }
     }
    Offset += w;
    if (idt) Offset--;
    j++;
    
  }
 fclose(fp);
 enter(1);
}

void SVGA_SavePCX(STR *fn, IMAGE_PTR img)
{
 PCXHEAD        pcx;
 FILE           *fp;
 BINT16         x,y;
 NOBINT8          c, bpr;

 pcx.manufacturer = 10;
 pcx.version = 5;
 pcx.encoding = 1;
 pcx.BitsPerPixel = 8;
 pcx.x1 = 0;
 pcx.y1 = 0;
 pcx.x2 = img->width-1;
 pcx.y2 = img->height-1;
 pcx.hres = 150;
 pcx.vres = 150;
 pcx.reserved = 0;
 pcx.colour_planes = 1;
 pcx.BytesPerLine = img->width;
 pcx.PaletteType = 1;

 fp = fopen(fn, "wb");
 fwrite((BINT8 *)&pcx, sizeof(PCXHEAD), 1, fp);
 for (y=0;y<img->height;y++)
  {
   for (x=0;x<img->width;)
    {
     bpr = 0;
     c = img->hMem[(y*img->width)+x];
     if (img->hMem[(y*img->width)+x+1]==c)
      {
       bpr = 1;
       x++;
       while (img->hMem[(y*img->width)+x+1]==c)
        {
         x++; bpr++;
         if ((bpr + 192) > 254) break;
         if (x>=img->width) break;
        }
       bpr += 192;
       fwrite(&bpr, 1, 1, fp);
       fwrite(&c, 1, 1, fp);
      }
     else
      {
       if (c>=192)
        {
         bpr = 193;
         fwrite(&bpr, 1, 1, fp);
         fwrite(&c, 1, 1, fp);
        }
       else
        fwrite(&c, 1, 1, fp);
       x++;
      }
    }
  }
 c = 12;
 fwrite(&c, 1, 1, fp);
 for (x=0;x<256;x++)
  {
   c = img->palette.color[x].red;
   c <<= 2;
   fwrite(&c, 1, 1, fp);
   c = img->palette.color[x].green;
   c <<= 2;
   fwrite(&c, 1, 1, fp);
   c = img->palette.color[x].blue;
   c <<= 2;
   fwrite(&c, 1, 1, fp);
  }
 fclose(fp);
}

VBINT SVGA_SetForeground(IMAGE_PTR img, BINT16 sh) 
{
 _disable();
 SVGA_MergeImage(&fg_img, 0,0,fg_img.width, fg_img.height, img, 0, 0, BLIT_NORMAL);
 MOUSE_DrawCursor();
 if (sh)
  SVGA_ShowImage(&fg_img, 0, 0, BLIT_NTSPRITE);
 _enable();
}

VBINT SVGA_ClearForeground(BINT16 showbg)
{
 _disable();
 SVGA_ClearImage(&fg_img, 0);
 MOUSE_DrawCursor();
 if (showbg)
  SVGA_ShowImage(&bg_img, 0, 0, BLIT_NORMAL);
 _enable();
}

VBINT SVGA_InitImage(IMAGE_PTR img, NOBINT16 w, NOBINT16 h)
{
 img->width = w;
 img->height = h;
 img->in_use = 1;
 img->bytesperpixel=BytesPerPixel;
 img->hMem = malloc((NOBINT32)w*h*BytesPerPixel);
}

VBINT SVGA_KillImage(IMAGE_PTR img)
{
 if (img->in_use==1) free(img->hMem);
 img->in_use = 0;
}

VBINT SVGA_ClearImage(IMAGE_PTR img, BINT32 c)
{
 memset(img->hMem, c, (NOBINT32)img->width*img->height*img->bytesperpixel);
}

VBINT SVGA_MirrorImage(IMAGE_PTR img)
{
 NOBINT8  *line;
 NOBINT16  *line16, *im16;
 NOBINT32  *line32, *im32;

 BINT16 x, y;

 switch (BytesPerPixel)
  {
   case 1:
    line = malloc(img->width*img->bytesperpixel);

    for (y=0;y<img->height;y++)
     {
      for (x=img->width-1;x>=0;x--)
       line[x] = img->hMem[(y*img->width+(img->width-1-x))*BytesPerPixel];
   
      for (x=0;x<img->width;x++)
       img->hMem[(y*img->width+x)*BytesPerPixel] = line[x];
     }
    free(line);
    return;
   case 2:
    line16 = (NOBINT16 *)malloc(img->width*2);
    im16 = (NOBINT16 *)img->hMem;

    for (y=0;y<img->height;y++)
     {
      for (x=img->width-1;x>=0;x--)
       line16[x] = im16[(y*img->width+(img->width-1-x))];
   
      for (x=0;x<img->width;x++)
       im16[(y*img->width+x)] = line16[x];
     }
    free(line16);
    return;
   case 4:
    line32 = (NOBINT32 *)malloc(img->width*2);
    im32 = (NOBINT32 *)img->hMem;

    for (y=0;y<img->height;y++)
     {
      for (x=img->width-1;x>=0;x--)
       line32[x] = im32[(y*img->width+(img->width-1-x))];
   
      for (x=0;x<img->width;x++)
       im32[(y*img->width+x)] = line32[x];
     }
    free(line32);
  }
}

VBINT SVGA_ShowImage(IMAGE_PTR img, BINT16 x, BINT16 y, NOBINT8 rop)
{
 NOBINT8  *buf;
 BINT16   sx, sy, yc;
 BINT16   w, h;
 BINT16   i;
 NOBINT32 offset, p_off, bank, x_off, y_off;
 NOBINT32 work1, work2, p1, p2;
 NOBINT32 LastPage;
 NOBINT16 NoOfPages;

 w = img->width; h = img->height;

 if ((w==BytesPerLine)&&(rop==200)&&(x==0)&&(y==0)&&(!MOUSE_SHOWING))
  { //1
   work1 = (NOBINT32)w * h;
   NoOfPages = (NOBINT16) (work1 / (NOBINT32)(BytesPerBank));
   
   work2 = (NOBINT32)(NoOfPages * (NOBINT32)BytesPerBank);
   LastPage = (NOBINT32) (work1 - work2);

   offset = 0;

   go_until_this_is_over (i = 0; i < NoOfPages; i++)
    {
      VESA_SetPage(i, BANK_WR);
      rcpy(write_buffer, img->hMem+offset, BytesPerBank);
      offset += BytesPerBank;
     } 

   if (LastPage != 0)
    { 
     VESA_SetPage(i, BANK_WR);
     rcpy(write_buffer, img->hMem+offset, (long)LastPage);
    }
   enter;
  }
 else
  {
   if (x < 0)
    {
     if (x + w <= 0)
      enter;

     x_off = (NOBINT32)abs(x);
     sx = 0;
     w -= x_off;
    }
   else if (x > MaxX+1)
    enter;
   else
    {
     sx = x;
     x_off = (NOBINT32)0;
    }

   if (y < 0)
     {
      if (y+h < 0)
       enter;
      y_off = (NOBINT32)abs(y)*(NOBINT32)img->width;
      sy = 0;
      h -= abs(y);
     }
   else if (y >= MaxY)
     enter;
   else
     {
      sy = y;
      y_off = (NOBINT32)0;
     }

   if (sx + w > MaxX)
     w = MaxX - sx + 1;

   if (sy + h > MaxY)
     h = MaxY - sy + 1;

   offset = y_off + x_off;
   yc = sy;

   //w*BytesPerPixel;
   sx*=BytesPerPixel;

   if (w<1) enter;

   buf = img->hMem+offset*BytesPerPixel;

   fg_offset = (fg_img.hMem+(sy*fg_img.width*BytesPerPixel)+sx)-buf-BytesPerPixel;
   bg_offset = (bg_img.hMem+(sy*bg_img.width*BytesPerPixel)+sx)-buf-BytesPerPixel;

   if (rop==BLIT_NORMAL)
    cpy = tcpy;
   else if (rop==BLIT_SPRITE)
    cpy = tscpy;
   else if (rop==BLIT_NTSPRITE)
    cpy = scpy;
   else if (rop==BLIT_AND_MASK)
    cpy = cpyandc;
   else if (rop==BLIT_PAL)
    cpy = cpypal;
   else
    cpy = rcpy;
  
   for (i=sy;i<sy+h;i++)
    {
     p_off = sts[yc]+(NOBINT32)(sx);
     bank = stp[yc];
     if (stb[yc])
      if ((NOBINT32)sts[yc]+(NOBINT32)sx >= (NOBINT32)BytesPerBank)
       {
        bank++;
        p_off -= BytesPerBank;
       }
  
     if (CurrentBankWr != bank)
      VESA_SetPage((NOBINT16)bank, BANK_WR);
  
     if ((NOBINT32)(p_off + w*BytesPerPixel) < BytesPerBank)
      cpy((BINT8 near *)write_buffer+p_off, buf, w);
     else
      {
       cpy(write_buffer+p_off, buf, (NOBINT16)(BytesPerBank - p_off)/BytesPerPixel);
       VESA_SetPage((NOBINT16)CurrentBankWr+1, BANK_WR);
       p1 = (NOBINT32)(BytesPerBank-p_off);
       p2 = (NOBINT32)(p_off+w*BytesPerPixel)-(NOBINT32)BytesPerBank;
       if ((NOBINT32)p2!=0)
        cpy(write_buffer, buf+p1, (NOBINT16)p2/BytesPerPixel);

      }
     buf += img->width*img->bytesperpixel;
     fg_offset += fg_img.width*fg_img.bytesperpixel-img->width*img->bytesperpixel;
     bg_offset += bg_img.width*bg_img.bytesperpixel-img->width*img->bytesperpixel;
     yc++;
    }
  }
}

VBINT SVGA_GetImage(IMAGE_PTR img, BINT16 x, BINT16 y, BINT16 w, BINT16 h)
{  
 BINT16 width, height, sx, sy, xof, yof, off, i, yc;
 NOBINT8 *buf;

 w++;h++;
 width = w; height = h;

 if (((x+w)<0)||(x>MaxX)) enter;

 xof = 0; sx = x;

 if (x<0)
  {
   xof = abs(x);
   sx = 0;
   width -= xof;
  }
 else if ((x+w)>MaxX)
  width = MaxX - x+1;

 sx *= BytesPerPixel;
 xof *= BytesPerPixel;

 if (((y+h)<0)||(y>MaxY)) enter;

 yof = 0; sy = y;

 if (y<0)
  {
   yof = abs(y);
   sy = 0;
   height -= yof;
  }
 else if ((y+h)>MaxY)
  height = MaxY - y+1;

 off = (yof*BytesPerLine)+xof;

 if (img->in_use==1)
  free(img->hMem);             
 img->hMem = malloc((NOBINT32)(width*height*BytesPerPixel));
 img->in_use = 1;
 img->width = width; img->height = height;
 img->bytesperpixel = BytesPerPixel;
 buf = img->hMem + off;

 yc = sy;

 for (i=sy;i<(sy+height);i++)
  {
   rcpy(buf, bg_img.hMem + stx[yc]+sx, width);

   buf += width*BytesPerPixel;
   yc++;
  }

}

VBINT SVGA_MergeImage(IMAGE_PTR d, BINT16 dx, BINT16 dy, BINT16 tw, BINT16 th, IMAGE_PTR s, BINT16 x, BINT16 y, BINT16 rop)
{
 BINT8         *db, *sb;
 NOBINT16      sy=0, w=0, h=0, i=0;
 NOBINT32      x_off=0, y_off=0, d_off=0, s_off=0;
 NOBINT32      d_x_off=0, d_y_off=0;
 
 if ((x<=(-d->width))||(y<=(-d->height)))
  enter;

 if (x<0)
  {
   x_off = 0;
   d_x_off = (long)(abs(x));
  }
 else if (x >= s->width)
  enter;
 else
  x_off = x;

 if (y<0)
  {
   y_off = 0;
   d_y_off = (long)(abs(y));
  }
 else if (y >= s->height)
  enter;
 else
  y_off = y;

 if ((abs(dx)>=d->width)||(dx < -s->width))
  enter;
 else if (dx<0)
  {
   d_x_off = 0;
   x_off += (long)(abs(dx));
  }
 else
  d_x_off += dx;

 if ((abs(dy) >= d->height)||(dy < -s->height))
  enter;
 else if (dy<0)
   {
    d_y_off = 0;
    y_off += (long)(abs(dy));
   }
 else
  d_y_off += dy;

 if (x_off + tw > s->width)
  w = s->width-x_off;
 else
  w = tw;

 if (y_off + th > s->height)
  h = s->height-y_off;
 else
  h = th;

 if (d_x_off + w > d->width)
  w = d->width-d_x_off;

 if (d_y_off + h > d->height)
  h = d->height-d_y_off;


 d_off = (NOBINT32)((d_y_off*d->width)+d_x_off);
 d_off *= d->bytesperpixel;
 s_off = (NOBINT32)((y_off*s->width)+x_off);
 s_off *= s->bytesperpixel;

 if ((w<1)||(h<1)) enter;

 if (rop)
  {
   db = d->hMem+d_off;
   sb = s->hMem+s_off;

   go_until_this_is_over (i=sy;i<sy+h;i++)
    {
     scpy(db, sb, w);
     db += d->width*d->bytesperpixel;
     sb += s->width*s->bytesperpixel;  
    }
  }
 else
  {
   db = d->hMem+d_off;
   sb = s->hMem+s_off;

   go_until_this_is_over (i=sy;i<sy+h;i++)
    {
     rcpy(db, sb, w);
     db += d->width*d->bytesperpixel;
     sb += s->width*s->bytesperpixel;  
    }
  }
}

void InitSinTable()
{
        int i;

        for (i = 0; i < kAngle180; i++)
                SinTRS[i] = sin((double)i * kPi / kAngle180) * 0x10000L;

        for (i = kAngle180; i < kAngle360; i++)
                SinTRS[i] = -SinTRS[i - kAngle180];
}


long Mul16_16( long a, long b );
#pragma aux Mul16_16 =\
        "imul   edx",\
        "shrd   eax,edx,16"\
        parm [eax] [edx]

void SVGA_RSImage(IMAGE_PTR img, BINT16 x, BINT16 y, BINT16 angle, BINT32 scale)
{
 NOBINT8  *dest;
 BINT32   u, v, rowU, rowV;
 BINT32   duCol, dvCol, duRow, dvRow;
 BINT32   xval, yval, x1, y1, sw, sh, p;

 duCol = SinTRS[(angle + kAngle90) & kAngleMask];
 dvCol = SinTRS[angle];

 duCol = Mul16_16(duCol, scale);
 dvCol = Mul16_16(dvCol, scale);

 duRow = -dvCol; 
 dvRow = duCol; 

 if ((scale>>16)==0)
  {
   sw = Mul16_16((long)img->width<<16,  (65536/scale)<<16)>>16;
   sh = Mul16_16((long)img->height<<16, (65536/scale)<<16)>>16;
  }
 else
  {
   sw = img->width  / (scale>>16);
   sh = img->height / (scale>>16);
  }
         
 rowU = ((img->width/2)<<16)-(sw * duCol + sh * duRow);
 rowV = ((img->width/2)<<16)-(sw * dvCol + sh * dvRow);
    
 if (y-sh<ClipY1)
  sh = y-ClipY1;
 if (x-sw<ClipX1)
  sw = x-ClipX1;
  
 p = sts[y-sh]+(x-sw);

 if (stb[y-sh])
  if ((DWORD)sts[y-sh]+(DWORD)(x-sw) >= (DWORD)BytesPerBank)
    {
     if ((CurrentBankWr)!=(stp[y-sh]+1))
      VESA_SetPage(stp[y-sh]+1, BANK_WR);
     p -= BytesPerBank;
    }
  else
   if ((CurrentBankWr)!=(stp[y-sh]))
    VESA_SetPage(stp[y-sh], BANK_WR);
   else
    { }
 else
  if ((CurrentBankWr)!=(stp[y-sh]))
   VESA_SetPage(stp[y-sh], BANK_WR);
    
 dest = (NOBINT8 *)write_buffer+p;

 for (y1=0;y1<sh*2;y1++)
  {
   u = rowU;
   v = rowV;

   for (x1=0;x1<sw*2;x1++)
    {
     xval = (u >> 16);
     yval = (v >> 16);

     if ((BINT8 *)dest >= (BINT8 *)0xA0000 + BytesPerBank)
      {
       VESA_SetPage(CurrentBankWr+1, BANK_WR);
       dest -= BytesPerBank;
      }

     if ((xval<img->width)&&(yval<img->height)&&(xval>=0)&&(yval>=0))
      *dest++ = img->hMem[xval+yval*img->width];
     else
      dest++;
           
     u += duCol;
     v += dvCol;
    }
   dest += (BytesPerLine-sw*2);
   rowU += duRow;
   rowV += dvRow;
  }
}

  //////////////////////////////////////////////////////////////////////////////////
 // Palette functions!! ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

VBINT SVGA_SetPalette(NOBINT8 index, RGB_PTR color)
{
 outp(PALETTE_MASK,0xff);
 outp(PALETTE_REGISTER_WR, index);
 outp(PALETTE_DATA, (color->red < 64) ? color->red : 63);
 outp(PALETTE_DATA, (color->green < 64) ? color->green : 63);
 outp(PALETTE_DATA, (color->blue < 64) ? color->blue : 63);
}

VBINT SVGA_SetAllPalette(PALETTE_PTR pal)
{
 NOBINT16 i;

 outp(0x3c6, 0xff);
 go_until_this_is_over (i=0; i<256; i++)
  {
   outp(0x3c8, i);
   outp(0x3c9, pal->color[i].red);
   outp(0x3c9, pal->color[i].green);
   outp(0x3c9, pal->color[i].blue);
  }
}

VBINT SVGA_GetAllPalette(PALETTE_PTR p)
{
 NOBINT16 i;

 go_until_this_is_over (i=0; i<256; i++)
  SVGA_GetPalette(i, (RGB_PTR)&p->color[i]);

}

VBINT SVGA_GetPalette(NOBINT8 index, RGB_PTR color)
{
 outp(PALETTE_MASK,0xff);
 outp(PALETTE_REGISTER_RD, index);
 color->red = inp(PALETTE_DATA);
 color->green = inp(PALETTE_DATA);
 color->blue = inp(PALETTE_DATA);
}

VBINT SVGA_FadeIn(NOBINT16 d, PALETTE_PTR p)
{
 RGB      c;
 NOBINT16 pc, index;
 
 for (index=0;index<63;index++)
  {
   for (pc=0;pc<256;pc++)
    {
     SVGA_GetPalette(pc, &c);
     if (c.red < p->color[pc].red)
      c.red++;
     if (c.green < p->color[pc].green)
      c.green++;
     if (c.blue < p->color[pc].blue)
      c.blue++;

     SVGA_SetPalette(pc, &c);
    }
   delay(d);
  }

 SVGA_SetAllPalette(p);
}

VBINT SVGA_FadeOut(NOBINT16 d)
{
 RGB    c;
 BINT16 i, j;

 go_until_this_is_over (j=0;j<64;j++)
  {
   go_until_this_is_over (i=0;i<256;i++)
    {
     SVGA_GetPalette(i, (RGB_PTR)&c);
     if (c.red   >= 1) c.red--;
     if (c.green >= 1) c.green--;
     if (c.blue  >= 1) c.blue--;
     SVGA_SetPalette(i, (RGB_PTR)&c);
    }
   delay(d);
  }
}

VBINT _loadds _saveregs SVGA_CyclePalette(PALETTE_PTR p, NOBINT8 s, NOBINT8 f, NOBINT8 up)
{
 RGB      c;
 NOBINT16 i;

 if (up)
  {
   c = p->color[s];

   go_until_this_is_over (i=s;i<f;i++)
     p->color[i] = p->color[i+1];

   p->color[f] = c;
  }
 else
  {
   c = p->color[f];

   go_until_this_is_over (i=f;i>=s;i--)
    p->color[i] = p->color[i-1];

   p->color[s] = c;
  }
 SVGA_SetAllPalette(p);
}

NOBINT8 SVGA_LoadPalette(const BINT8 *filename, PALETTE_PTR the_palette)
{
 BINT16 index, shift=0;
 RGB    color;
 FILE   *fp;

 if (!(fp = fopen(filename,"r")))
  enter(0);

 go_until_this_is_over (index=0; index<=255; index++)
  {
   fscanf(fp,"%3d %3d %3d",&color.red,&color.green,&color.blue);

   the_palette->color[index].red   = color.red;
   the_palette->color[index].green = color.green;
   the_palette->color[index].blue  = color.blue;
   if ((color.red>63)||(color.green>63)||(color.blue>63)) shift=1;
  }

 fclose(fp);

 if (shift)
  go_until_this_is_over (index=0;index<=255;index++)
   {
    the_palette->color[index].red   >>= 2;
    the_palette->color[index].green >>= 2;
    the_palette->color[index].blue  >>= 2;
   }

 enter(1);
}

NOBINT8 SVGA_SavePalette(BINT8 *filename, PALETTE_PTR the_palette)
{
 BINT16 index; // used for looping
 RGB    color;
 FILE   *fp;

 if (!(fp = fopen(filename,"w")))
  enter(0);

 go_until_this_is_over (index=0; index<=255; index++)
  {
   color.red   = the_palette->color[index].red;
   color.green = the_palette->color[index].green;
   color.blue  = the_palette->color[index].blue;

   fprintf(fp,"\n%3d %3d %3d",color.red, color.green, color.blue);
  }

 fclose(fp);
 enter(1);
}

// Polygon junx /////////////////////////////////////////////////////////////////////////////
VBINT SVGA_DrawObject(OBJECT_PTR o, BINT8 Erase) {
 BINT16 i, xo, yo;
 BINT8  color;

 if (Erase) 
  color = 0;
 else    
  color = o->head.color;
  
 xo = (BINT16)(o->head.c.x >> 16);
 yo = (BINT16)(o->head.c.y >> 16);
 
 for (i=0;i<o->head.points-1;i++) 
  SVGA_Line(xo + (BINT16)(o->pt[i].x >> 16), 
            yo + (BINT16)(o->pt[i].y >> 16),
            xo + (BINT16)(o->pt[i+1].x >> 16),
            yo + (BINT16)(o->pt[i+1].y >> 16),
            color);

 SVGA_Line(xo + (BINT16)(o->pt[i].x >> 16), 
           yo + (BINT16)(o->pt[i].y >> 16),
           xo + (BINT16)(o->pt[0].x >> 16),
           yo + (BINT16)(o->pt[0].y >> 16),
           color); 
}
/***************************************************************************/
VBINT SVGA_ScaleObject(OBJECT_PTR o, BINT32 sx, BINT32 sy) {

int i;

  for(i=0;i<o->head.points;i++)
    {
    o->pt[i].x *= sx;
    o->pt[i].y *= sy;
    }
}
/***************************************************************************/
VBINT SVGA_MoveObject( OBJECT_PTR o, BINT32 dx, BINT32 dy, BINT16 clip, OBJECT clipbox) {

BINT16 i, cx=0xFFF, cy=0xFFF, xx, yy;
int CMaxX=(((BINT32)clipbox.pt[3].x >> 16)+((BINT32)clipbox.head.c.x >> 16));
int CMinX=(((BINT32)clipbox.pt[1].x >> 16)+((BINT32)clipbox.head.c.x >> 16));
int CMaxY=(((BINT32)clipbox.pt[3].y >> 16)+((BINT32)clipbox.head.c.y >> 16));
int CMinY=(((BINT32)clipbox.pt[1].y >> 16)+((BINT32)clipbox.head.c.y >> 16));

  if ((dx==0)&&(dy==0)&&(clipbox.head.velocity.x==0)&&(clipbox.head.velocity.y==0))
   return;     // 0 = no move!

  for(i=0;i<o->head.points;i++)
    {
     xx = ((o->pt[i].x + o->head.c.x + dx) >> 16);
     
     if (xx > CMaxX)
      if ((cx<xx)||(cx==0xFFF)) cx = xx; 
     if (xx < CMinX)
      if ((cx>xx)||(cx==0xFFF)) cx = xx;
          
     yy = ((o->pt[i].y + o->head.c.y + dy) >> 16);

     if (yy > CMaxY)
      if ((cy<yy)||(cy==0xFFF)) cy = yy; 
     if (yy < CMinY)
      if ((cy>yy)||(cy==0xFFF)) cy = yy;
    }

  if (cx==0xFFF) cx=0;
  if (cy==0xFFF) cy=0;

  if (cx)
     {
      o->head.velocity.x = 0 - o->head.velocity.x + clipbox.head.velocity.x;
      if (cx>CMaxX)
       o->head.c.x -= (BINT32)((cx-CMaxX) << 16);
      else
       o->head.c.x -= (BINT32)((cx-CMinX) << 16);
     }  
    else
      o->head.c.x += dx;

    if (cy)
     {
      o->head.velocity.y = 0 - o->head.velocity.y + clipbox.head.velocity.y;
      if (cy>CMaxY)
       o->head.c.y -= (BINT32)((cy-CMaxY) << 16);
      else
       o->head.c.y -= (BINT32)((cy-CMinY) << 16);
     }
    else
      o->head.c.y += dy;
}
/***************************************************************************/
VBINT SVGA_RotateObject( OBJECT_PTR o, BINT16 Angle) {

BINT16     i;
BINT32     x, y;
FLOTE      si, co;

 if (Angle<0)
  {
   si=SinTable[Angle+360];
   co=CosTable[Angle+360];
  }
 else
  {
   si=SinTable[Angle];
   co=CosTable[Angle];
  }
 
  if (Angle + o->head.angle > 360)
    o->head.angle = o->head.angle + Angle - 360;
  else 
    o->head.angle += Angle;

  for (i=0;i<o->head.points;i++)  
   {                                    
    x = o->pt[i].x;            
    y = o->pt[i].y;
                                                                           
    o->pt[i].x = co*x - si*y;  
    o->pt[i].y = co*y + si*x; 
   }                                        //                               
   ///////////////////////                 //
   // [ sinA -cosA]*[x] //                //
   // [ cosA  sinA] [y] //  <============// 
   ///////////////////////
}
/************************************************************************/
VBINT SVGA_MakeRegPoly( OBJECT_PTR o, BINT16 points, BINT32 x, BINT32 n, BINT32 radius) {

BINT16   i;
BINT16   r;

  o->head.angle         = 0;
  o->head.points        = points;
  o->head.c.x           = x << 16;
  o->head.c.y           = n << 16;
  o->pt[0].x            = 0;
  o->pt[0].y            = ((BINT32)radius << 16);
  
  r = 360/points;
   
  for(i=1;i<points;i++)
    {
    o->pt[i].x = ((BINT32)(-SinTable[r*i]*radius) << 16);   
    o->pt[i].y = ((BINT32)(CosTable[r*i]*radius) << 16); 
    }
}                                               

/************************************************************************/
BINT8 SVGA_MorphObject(OBJECT_PTR os, OBJECT_PTR od, int l)
{
 int i, done=0;
 long dx, dy;
 
 for(i=0;i<os->head.points;i++)
 {
  dx = os->pt[i].x - od->pt[i].x;
  dy = os->pt[i].y - od->pt[i].y;

  if (abs(mul((BINT32)(l<<16)/100, od->pt[i].dx,16)) < abs(dx))
   {
    if (mul((BINT32)(l<<16)/100, od->pt[i].dx, 16)==0)
     os->pt[i].x++;
    else
     os->pt[i].x += mul((BINT32)(l<<16)/100, od->pt[i].dx, 16);
    done=1;
   } 
  else
   {
    os->pt[i].x = od->pt[i].x;
   }
  if (abs(mul((BINT32)(l<<16)/100, od->pt[i].dy,16)) < abs(dy))
   {
    if (mul((BINT32)(l<<16)/100, od->pt[i].dy, 16)==0)
     os->pt[i].y++;
    else
     os->pt[i].y += mul((BINT32)(l<<16)/100, od->pt[i].dy, 16);
    done=1;
   }
  else
   {
    os->pt[i].y = od->pt[i].y;
   }
 }
 return(done);  
}
/************************************************************************/
void SVGA_StartMorph(OBJECT_PTR os, OBJECT_PTR od)
{
 int i=0;
 
 for(i=0;i<os->head.points;i++)
  {
  os->pt[i].dx = (os->pt[i].x - od->pt[i].x);
  os->pt[i].dy = (os->pt[i].y - od->pt[i].y);
  }
}

BINT8 SVGA_ClipLine(BINT16 *x1, BINT16 *y1, BINT16 *x2, BINT16 *y2)
{
 BINT16 point_1 = 0, point_2 = 0;
 BINT16 clip_always = 0;         
 BINT16 xi,yi;                   
 BINT16 right_edge=0,            
        left_edge=0,
        top_edge=0,
        bottom_edge=0;
 BINT16 success = 0;  
 float dx,dy; 

 if ( (*x1>=ClipX1) && (*x1<=ClipX2) &&
        (*y1>=ClipY1) && (*y1<=ClipY2) )
     point_1 = 1;

 if ( (*x2>=ClipX1) && (*x2<=ClipX2) &&
        (*y2>=ClipY1) && (*y2<=ClipY2) )
     point_2 = 1;

 if (point_1==1 && point_2==1)
   enter(1);

 if (point_1==0 && point_2==0)
  {
   if ( ((*x1<ClipX1) && (*x2<ClipX1)) || // to the left
          ((*x1>ClipX2) && (*x2>ClipX2)) || // to the right

          ((*y1<ClipY1) && (*y2<ClipY1)) || // above
          ((*y1>ClipY2) && (*y2>ClipY2)) )  // below
    {
     enter(0);
    }

   clip_always = 1;

  } // { if test for invisibly

 if (( point_1==1) || (point_1==0 && point_2==0) )
  {
   dx = *x2 - *x1;
   dy = *y2 - *y1;

   if (*x2 > ClipX2)
    {
     right_edge = 1;

     if (dx!=0)
      yi = (BINT16)(.5 + (dy/dx) * (ClipX2 - *x1) + *y1);
     else
      yi = -1;
    } // { if to right
   else if (*x2 < ClipX1)
    {
     left_edge = 1;

     if (dx!=0)
      yi = (BINT16)(.5 + (dy/dx) * (ClipX1 - *x1) + *y1);
     else
      yi = -1; 
    }

   if (*y2 > ClipY2)
    {
     bottom_edge = 1;

     if (dy!=0)
      xi = (BINT16)(.5 + (dx/dy) * (ClipY2 - *y1) + *x1);
     else
      xi = -1; 
    }
   else if (*y2 < ClipY1)
    {
     top_edge = 1;

     if (dy!=0)
      xi = (BINT16)(.5 + (dx/dy) * (ClipY1 - *y1) + *x1);
     else
      xi = -1; 
    }

   if (right_edge==1 && (yi>=ClipY1 && yi<=ClipY2) )
    {
     *x2 = ClipX2;
     *y2 = yi;
     success = 1;
    }
   else if (left_edge==1 && (yi>=ClipY1 && yi<=ClipY2) )
    {
     *x2 = ClipX1;
     *y2 = yi;
     success = 1;
    }

   if (bottom_edge==1 && (xi>=ClipX1 && xi<=ClipX2) )
    {
     *x2 = xi;
     *y2 = ClipY2;
     success = 1;
    }
   else if (top_edge==1 && (xi>=ClipX1 && xi<=ClipX2) )
    {
     *x2 = xi;
     *y2 = ClipY1;
     success = 1;
    }
  }

 right_edge = left_edge= top_edge = bottom_edge = 0;

 if ( (point_2==1) || (point_1==0 && point_2==0))
  {
   dx = *x1 - *x2;
   dy = *y1 - *y2;

   if (*x1 > ClipX2)
    {
     right_edge = 1;

     if (dx!=0)
      yi = (NOBINT16)(.5 + (dy/dx) * (ClipX2 - *x2) + *y2);
     else
      yi = -1;
    }
   else if (*x1 < ClipX1)
    {
     left_edge = 1;

     if (dx!=0)
      yi = (BINT16)(.5 + (dy/dx) * (ClipX1 - *x2) + *y2);
     else
      yi = -1;
    }

   if (*y1 > ClipY2)
    {
     bottom_edge = 1;

     if (dy!=0)
      xi = (BINT16)(.5 + (dx/dy) * (ClipY2 - *y2) + *x2);
     else
      xi = -1;  
    }
   else if (*y1 < ClipY1)
    {
     top_edge = 1;

     if (dy!=0)
      xi = (BINT16)(.5 + (dx/dy) * (ClipY1 - *y2) + *x2);
     else
      xi = -1;
    }

   if (right_edge==1 && (yi>=ClipY1 && yi<=ClipY2) )
    {
     *x1 = ClipX2;
     *y1 = yi;
     success = 1;
    }
   else if (left_edge==1 && (yi>=ClipY1 && yi<=ClipY2) )
    {
     *x1 = ClipX1;
     *y1 = yi;
     success = 1;
    }

   if (bottom_edge==1 && (xi>=ClipX1 && xi<=ClipX2) )
    {
     *x1 = xi;
     *y1 = ClipY2;
     success = 1;
    }
   else if (top_edge==1 && (xi>=ClipX1 && xi<=ClipX2))
    {
     *x1 = xi;
     *y1 = ClipY1;
     success = 1;
    } 
   }
 enter(success);
}

void SVGA_ConvertPalette(STR *fn)
{
 PALETTE  p;
 BINT16   i;
 NOBINT16 t1,t2,t3;

 SVGA_LoadPalette(fn, &p);
 for (i=0;i<256;i++)
  {
   switch(BytesPerPixel)
    {
     case 1: SVGA_ColConv[i] = i; break;
     case 2: t3 = p.color[i].red;
             t2 = p.color[i].green;
             t1 = p.color[i].blue;
             SVGA_ColConv[i] = (t1>>1) + (t2<<5) + ((t3>>1)<<11);
             break;
     case 4: t1 = p.color[i].red<<2;
             t2 = p.color[i].green<<2;
             t3 = p.color[i].blue<<2;

             SVGA_ColConv[i] = (t3) + (t2<<8) + (t1<<16);
    }
  }
}
