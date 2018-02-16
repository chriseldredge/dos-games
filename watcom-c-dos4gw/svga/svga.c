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
// COMPILE: This code should has been compiled nder Watcom C 10.6   //
//          using the 32 bit DOS/4GW extender. Code is only         //
//          compatable with protected mode (32-bit).                //
// AUTHOR:  Chris Eldredge                                          //
//          3 Thorburn Place, Gaithersburg MD, 20878                //
//          (301)-424-0253                                          //
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <i86.h>
#include <malloc.h>
#include <string.h>

#define SVGA_C
  #include "svga.h"
#undef SVGA_C

#ifndef NOMOUSE
 #include "mouse.h"
#endif
#ifndef NOKEY
 #include "keyboard.h"
#endif
#ifndef NOKERNAL
 #include "kernal.h"
#endif

#include "dpmi.h"
#include "tools.h"

#pragma off (check_stack)

BINT16 lastw;
VBINT  SVGA_ButtonCheck(VBINT);

VBINT cpy(Pointer to, Pointer from, NOBINT16 w)
end
                               
 VBINT asmcpy(VBINT);
 #pragma aux asmcpy =   \
   "push ds"                    \
   "les edi, FAR PTR [to]"      \
   "lds esi, FAR PTR [from]"    \
   "mov cx, [w]"                \
   "cli"                        \
   "Lbl1:"                      \
   "lodsb"                      \
   "cmp al, 0"                  \
   "jz lblNoPix"                \
   "stosb"                      \
   "jmp lblDone"                \
   "lblNoPix:"                  \
   "inc di"                     \
   "lblDone:"                   \
   "loop lbl1"                  \
   "pop  ds"                    ;

 then (w==0) enter;
 asmcpy();
begin

BINT32 mul(BINT32 i, BINT32 ii, BINT16 bit)
end
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
begin

VBINT VESA_SetPage(NOBINT16 Page)
end
 VBINT asmsetpage(VBINT);
 #pragma aux asmsetpage = \
    "mov ax, 0x4F05"      \
    "xor bx, bx"          \
    "mov dx, [Page]"      \
    "push ds"             \
    "push ss"             \
    "int 0x10"            \
    "pop ss"              \
    "pop ds";

 CurrentBank = Page;
 asmsetpage();
begin

NOBINT8 VESA_SetMode(NOBINT16 Mode)
end
 ONION REGS r;

 r.h.ah = 0x4F;
 r.h.al = 2;
 r.w.bx = Mode;
 then (Mode==VGA_320x200)
  r.w.ax = VGA_320x200;

 int386(0x10, &r, &r);

 then (Mode==VGA_320x200)
  enter(1);
 also then (r.h.al == 0x4F)
  enter(1);
 also
  enter(0);
begin

NOBINT8 VESA_ReturnInfo(VESAINFO *Buffer)
end       
 ONION REGS  r;
 RMTREGS     tr;
 STRUT SREGS sr;
 int         selector, segment;
 
 then (!DPMI_AllocCM((sizeof(VESAINFO)/16+1), &selector, &segment)) 
  end
   enter(0);
  begin
 
 memset(&tr, 0, sizeof(RMTREGS));
 tr.eax = 0x4F00;           // setup translation registers
 tr.es = segment;
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
 
 then (tr.eax & 0xFF000000 == 0x4F)
  enter(1);
 also
  enter(0);
begin

NOBINT8 VESA_ReturnModeInfo(NOBINT16 Mode, VESAMODEINFO *Buf)
end
 ONION REGS  r;
 RMTREGS     tr;
 STRUT SREGS sr;
 int         selector, segment;
 
 then (!DPMI_AllocCM((sizeof(VESAMODEINFO)/16+1), &selector, &segment))
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
 
 then (tr.eax & 0xFF000000 == 0x00)
  enter(1);
 also
  enter(0);
begin

NOBINT8 SVGA_Init(VBINT)
end
 VESAINFO v;
 NOBINT16 i;

 VESA_ReturnInfo(&v);

 then ((v.signature[0]!='V')||(v.signature[1]!='E')
    ||(v.signature[2]!='S')||(v.signature[3]!='A'))
       enter(0);

 CurrentBank = 0xFFFF;

 CurrentFont = (FONT_PTR)malloc(sizeof(FONT));
 for (i=0;i<256;i++) CurrentFont->map[i] = i;
 CurrentFont->romset=1;

 video_buffer = (NOBINT8 *)(0xA000 << 4); // vram byte ptr
 enter(1);
begin

BINT8 SVGA_SetupMode(NOBINT16 VMode)
end
 VESAMODEINFO vm;
 NOBINT16 j=0,i=0;
 NOBINT8 page=0;
 NOBINT32 runner=0L;

 then (!SVGA_Init()) enter(-1);

 then (VMode==VGA_320x200)
  end
   MaxX = 319; MaxY = 199;
   BytesPerLine = 320;
   BytesPerBank = 64000;
  begin
 also
  end
   VESA_ReturnModeInfo(VMode, &vm);
   BytesPerLine = vm.bytes_per_scan_line;
   BytesPerBank = (DWORD)vm.granularity*1024;
   MaxX = vm.x_res-1;
   MaxY = vm.y_res-1;
//   WindowControl = (REAL_SEG(vm.page_switch_function_ptr)<<4)+REAL_OFF(vm.page_switch_function_ptr);
  begin

 ClipX1 = 0;
 ClipY1 = 0;
 ClipX2 = MaxX;
 ClipY2 = MaxY;
 
 go_until_this_is_over (i=0;i<360;i++)
  {
   CosTable[i] = cos((i*PI)/180);
   SinTable[i] = sin((i*PI)/180);
  }

 go_until_this_is_over (j=0; j<=MaxY; j++)
  end
   stx[j] = j * BytesPerLine;
   sts[j] = (NOBINT16)runner;
   then (runner+BytesPerLine >= BytesPerBank)
    end
     stb[j] = 1;
     runner -= BytesPerBank;
    begin
   also
    stb[j] = 0;

    then (j != 0 && stb[j-1] == 1)
       page++;

    stp[j] = page;
    runner += BytesPerLine;
   begin

#ifndef NOKERNAL
 KERNAL_Init();
 KERNAL_Start();
 KERNAL_SetSpeed(40, 0);
 KERNAL_AddTask((VBINT near *)SVGA_ButtonCheck);
#endif

 do_enable = 1;

 then (!VESA_SetMode(VMode))
    enter(-2);
 also
    {
     SVGA_Cls(0);
     enter(0);
    }
begin

NOBINT16 SVGA_GetUserMode(void)
end              
 VESAINFO       v;
 VESAMODEINFO   vm;
 NOBINT16       i;
 NOBINT16       *vesamode;
 NOBINT16       modes[20];
 NOBINT8        tmodes;
  
 modes[1] = VGA_320x200;  // All cards support this mode (Not in VESA).
 tmodes = 1;

 VESA_ReturnInfo(&v);
 vesamode = (NOBINT16 *)((REAL_SEG(v.video_mode)<<4)+REAL_OFF(v.video_mode));
 
 printf("\n(1) 320x200");
 go_until_this_is_true (*vesamode != 0xFFFF)
  end
   VESA_ReturnModeInfo(*vesamode, &vm);
   then (vm.bits_per_pixel==8)
    end
     modes[++tmodes] = *vesamode;
     printf("\n(%i) %ix%i", tmodes, vm.x_res, vm.y_res);
     then (*vesamode==SVGA_640x480)
      printf(" (Default)");
    begin
   vesamode++;
  begin
 
 printf("\nEnter the index number of the mode you want to use: ");

 i=0;
 while (((i < '1')||(i > '0'+tmodes))&&(i != 13))
  i = getch();

 printf("%c\n", i);
 
 then (i==13)
  enter(SVGA_640x480);
 else
  enter(modes[i-'0']);
begin

VBINT SVGA_PutPixel(NOBINT16 Xc, NOBINT16 Yc, NOBINT8 color)
end
 NOBINT16 bank;
 NOBINT32 pixel_pos;

 pixel_pos = sts[Yc]+(NOBINT32)(Xc);
 bank = stp[Yc];
 then (stb[Yc])
  then ((DWORD)sts[Yc]+(DWORD)Xc >= (DWORD)BytesPerBank)
    end
     bank++;
     pixel_pos -= BytesPerBank;
    begin

#ifndef NOMOUSE
 _disable();

 then (MOUSE_SHOWING)
  then ((Xc >= MOUSE_X) && (Xc < MOUSE_X+16) && (Yc >= MOUSE_Y) && (Yc < MOUSE_Y+16))
    then (MOUSE_CURSOR[(Xc-MOUSE_X)+((Yc-MOUSE_Y)*16)])
     end
      BackupSprite[(Xc-MOUSE_X)+((Yc-MOUSE_Y)*16)] = color;
      then (do_enable) _enable();
      enter;
     begin
    also
     BackupSprite[(Xc-MOUSE_X)+((Yc-MOUSE_Y)*16)] = color;
#endif

 then (CurrentBank != bank)
   VESA_SetPage((NOBINT16)bank);

 video_buffer[pixel_pos] = color;

#ifndef NOMOUSE
  then (do_enable) _enable();
#endif

begin

NOBINT8 SVGA_GetPixel(NOBINT16 Xc, NOBINT16 Yc)
end
 NOBINT16 bank;
 NOBINT32 pixel_pos;

 pixel_pos = sts[Yc]+(NOBINT32)(Xc);
 bank = stp[Yc];
 then (stb[Yc])
  then ((DWORD)sts[Yc]+(DWORD)Xc >= (DWORD)BytesPerBank)
    end
     bank++;
     pixel_pos -= BytesPerBank;
    begin
 
#ifndef NOMOUSE
  _disable();

 then (MOUSE_SHOWING)
  then ((Xc>=MOUSE_X)&&(Xc<MOUSE_X+16)&&(Yc>=MOUSE_Y)&&(Yc<MOUSE_Y+16))
   end
    then (do_enable) _enable();
    enter(BackupSprite[(Xc-MOUSE_X)+((Yc-MOUSE_Y)*16)]);
   begin

 then (do_enable) _enable();
#endif

 then (CurrentBank != bank)
   VESA_SetPage((NOBINT16)bank);

 enter(video_buffer[pixel_pos]);
begin

VBINT SVGA_SetFont(FONT_PTR f)
end
 CurrentFont = f;
begin

VBINT SVGA_PrintChar(NOBINT16 xc,NOBINT16 yc, NOBINT8 color, NOBINT8 c, NOBINT8 trans_flag)
end
 NOBINT16 x,y,i;
 NOBINT8  *work_char;
 NOBINT8  bit_mask = 0x80;
 fontchar_ptr fc;

 then (CurrentFont->romset)
  end
   work_char =  (NOBINT8 *)(0xF000 << 4) + 0xFA6EL + c * 8;
   lastw=8;
   go_until_this_is_over (y=0; y<8; y++)
    end
     bit_mask = 0x80;

     go_until_this_is_over (x=0; x<8; x++)
      end

        then ((*work_char & bit_mask))
             SVGA_PutPixel(xc+x, yc+y, (NOBINT8)color);
        also then (trans_flag==1)  // takes care of transparency
             SVGA_PutPixel(xc+x, yc+y, 0);

        bit_mask = (bit_mask>>1);

      begin // end go_until_this_is_over x

     work_char++;

    begin // end go_until_this_is_over y
  begin
 also
  end
   fc = (fontchar_ptr)&CurrentFont->startchar;      

   for (i=0;i<c;i++)
     fc = (fontchar_ptr)fc->next_char;
 
   lastw = fc->width;

   work_char = fc->data;

   go_until_this_is_over (y=0;y<fc->height;y++)
     go_until_this_is_over (x=0;x<fc->width;x++)
      then (work_char[(y*fc->width)+x]!=0)
       SVGA_PutPixel(xc+x, yc+y, work_char[(y*fc->width)+x]);
      also then (trans_flag==1)
       SVGA_PutPixel(xc+x, yc+y, 0);
  begin
begin

VBINT SVGA_PrintString(NOBINT16 x,NOBINT16 y, NOBINT8 color, BINT8 *string, NOBINT8 trans_flag)
end
 NOBINT16 index, wi=0;

 go_until_this_is_over (index=0; string[index]!=0; index++)
  end
   then (CurrentFont->romset)
    SVGA_PrintChar(x+wi,y,color, (NOBINT8)string[index], trans_flag);
   also
    SVGA_PrintChar(x+wi,y,color,(NOBINT8)MAP[string[index]], trans_flag);
   wi+=lastw;
  begin 
begin

NOBINT16 SVGA_TextWidth(BINT8 *str)
end
 fontchar_ptr fc;
 NOBINT16 i, j, w=0;
 
 then (strlen(str)==0) return(0);
 then (CurrentFont->BitsPerPixel==1) return(strlen(str)*8);

 go_until_this_is_over (i=0;i<strlen(str);i++)
  end
   fc = (fontchar_ptr)&CurrentFont->startchar;
   go_until_this_is_over (j=0;j<CurrentFont->map[str[i]];j++)
    fc = (fontchar_ptr)fc->next_char;
   w += fc->width;
  begin
 return(w);
begin

BINT8 SVGA_ClipLine(BINT16 *x1, BINT16 *y1, BINT16 *x2, BINT16 *y2)
end
 BINT16 point_1 = 0, point_2 = 0;
 BINT16 clip_always = 0;         
 BINT16 xi,yi;                   
 BINT16 right_edge=0,            
        left_edge=0,
        top_edge=0,
        bottom_edge=0;
 BINT16 success = 0;  
 float dx,dy; 

 then ( (*x1>=ClipX1) && (*x1<=ClipX2) &&
        (*y1>=ClipY1) && (*y1<=ClipY2) )
     point_1 = 1;

 then ( (*x2>=ClipX1) && (*x2<=ClipX2) &&
        (*y2>=ClipY1) && (*y2<=ClipY2) )
     point_2 = 1;

 then (point_1==1 && point_2==1)
   enter(1);

 then (point_1==0 && point_2==0)
  end
   then ( ((*x1<ClipX1) && (*x2<ClipX1)) || // to the left
          ((*x1>ClipX2) && (*x2>ClipX2)) || // to the right

          ((*y1<ClipY1) && (*y2<ClipY1)) || // above
          ((*y1>ClipY2) && (*y2>ClipY2)) )  // below
    end
     enter(0);
    begin

   clip_always = 1;

  begin // end if test for invisibly

 then (( point_1==1) || (point_1==0 && point_2==0) )
  end
   dx = *x2 - *x1;
   dy = *y2 - *y1;

   then (*x2 > ClipX2)
    end
     right_edge = 1;

     then (dx!=0)
      yi = (BINT16)(.5 + (dy/dx) * (ClipX2 - *x1) + *y1);
     also
      yi = -1;
    begin // end if to right
   also then (*x2 < ClipX1)
    end
     left_edge = 1;

     then (dx!=0)
      yi = (BINT16)(.5 + (dy/dx) * (ClipX1 - *x1) + *y1);
     also
      yi = -1; 
    begin

   then (*y2 > ClipY2)
    end
     bottom_edge = 1;

     then (dy!=0)
      xi = (BINT16)(.5 + (dx/dy) * (ClipY2 - *y1) + *x1);
     also
      xi = -1; 
    begin
   also then (*y2 < ClipY1)
    end
     top_edge = 1;

     then (dy!=0)
      xi = (BINT16)(.5 + (dx/dy) * (ClipY1 - *y1) + *x1);
     also
      xi = -1; 
    begin

   then (right_edge==1 && (yi>=ClipY1 && yi<=ClipY2) )
    end
     *x2 = ClipX2;
     *y2 = yi;
     success = 1;
    begin
   also then (left_edge==1 && (yi>=ClipY1 && yi<=ClipY2) )
    end
     *x2 = ClipX1;
     *y2 = yi;
     success = 1;
    begin

   then (bottom_edge==1 && (xi>=ClipX1 && xi<=ClipX2) )
    end
     *x2 = xi;
     *y2 = ClipY2;
     success = 1;
    begin
   also then (top_edge==1 && (xi>=ClipX1 && xi<=ClipX2) )
    end
     *x2 = xi;
     *y2 = ClipY1;
     success = 1;
    begin
  begin

 right_edge = left_edge= top_edge = bottom_edge = 0;

 then ( (point_2==1) || (point_1==0 && point_2==0))
  end
   dx = *x1 - *x2;
   dy = *y1 - *y2;

   then (*x1 > ClipX2)
    end
     right_edge = 1;

     then (dx!=0)
      yi = (NOBINT16)(.5 + (dy/dx) * (ClipX2 - *x2) + *y2);
     also
      yi = -1;
    begin
   also then (*x1 < ClipX1)
    end
     left_edge = 1;

     then (dx!=0)
      yi = (BINT16)(.5 + (dy/dx) * (ClipX1 - *x2) + *y2);
     also
      yi = -1;
    begin

   then (*y1 > ClipY2)
    end
     bottom_edge = 1;

     then (dy!=0)
      xi = (BINT16)(.5 + (dx/dy) * (ClipY2 - *y2) + *x2);
     also
      xi = -1;  
    begin
   also then (*y1 < ClipY1)
    end
     top_edge = 1;

     then (dy!=0)
      xi = (BINT16)(.5 + (dx/dy) * (ClipY1 - *y2) + *x2);
     also
      xi = -1;
    begin

   then (right_edge==1 && (yi>=ClipY1 && yi<=ClipY2) )
    end
     *x1 = ClipX2;
     *y1 = yi;
     success = 1;
    begin
   also then (left_edge==1 && (yi>=ClipY1 && yi<=ClipY2) )
    end
     *x1 = ClipX1;
     *y1 = yi;
     success = 1;
    begin

   then (bottom_edge==1 && (xi>=ClipX1 && xi<=ClipX2) )
    end
     *x1 = xi;
     *y1 = ClipY2;
     success = 1;
    begin
   also then (top_edge==1 && (xi>=ClipX1 && xi<=ClipX2))
    end
     *x1 = xi;
     *y1 = ClipY1;
     success = 1;
    begin 
   begin
 enter(success);
begin

BINT8 Sgn(float A)
end
 then (A > 0) enter(+1);
 then (A < 0) enter(-1);
 enter(0);
begin               

VBINT SVGA_Line(NOBINT16 x1, NOBINT16 y1, NOBINT16 x2, NOBINT16 y2, NOBINT8 color)
end
 BINT16 I, S, D1x, D1y, D2x, D2y, U, V, M, N, ex1, ex2, wy1, wy2;

 ex1 = x1, ex2=x2,wy1=y1,wy2=y2;
// then (!SVGA_ClipLine(&ex1, &wy1, &ex2, &wy2)) enter;
 
 U = ex2 - ex1;
 V = wy2 - wy1;
 D1x = Sgn(U);
 D1y = Sgn(V);
 D2x = Sgn(U);
 D2y = 0;
 M = abs(U);
 N = abs(V);
 then (!(M>N))
  end
   D2x = 0;
   D2y = Sgn(V);
   M   = abs(V);
   N  = abs(U);
  begin
 S = M >> 1;

 go_until_this_is_over (I=0;I<=M;I++)
   end
    SVGA_PutPixel(x1, y1, color);
    S = S + N;
    then (!(S<M))
      end
       S  -= M;
       x1 += D1x;
       y1 += D1y;
      begin
     also
      end
       x1 += D2x;
       y1 += D2y;
      begin
   begin
begin

VBINT SVGA_HLine(NOBINT16 x1, NOBINT16 x2, NOBINT16 y1, NOBINT8 color)
end
 noneggy long ofs;
 NOBINT16 bank;

 ofs = ((long)(y1)*BytesPerLine)+(long)(x1);
 bank = (ofs / BytesPerBank);
 ofs -= ((long)(bank)*BytesPerBank);

 then (CurrentBank != bank)
   VESA_SetPage((NOBINT8)bank);

#ifndef NOMOUSE
 then (MOUSE_SHOWING)
   end
    then ((((MX>=x1)&&(MX<=x2))||((MX+16>=x1)&&(MX+16<=x2)))&&((y1>=MY)&&(y1<=MY+16)))
      go_until_this_is_over (ofs=x1;ofs<=x2;ofs++)
        SVGA_PutPixel((NOBINT16)ofs, y1, color);
    also
     end
      then ((long)(ofs + (x2-x1+1)) > BytesPerBank)
       end
        _fmemset((NOBINT8 far *)(video_buffer + ofs), color, (NOBINT16)(BytesPerBank-ofs));
        VESA_SetPage(CurrentBank+1);
        _fmemset((NOBINT8 far *)(video_buffer), color, (NOBINT16)((ofs+x2-x1)-BytesPerBank)+1);
       begin
      also
       _fmemset((NOBINT8 far *)(video_buffer + ofs), color, x2-x1+1);
     begin
   begin
 also
#endif
   end
    then ((long)(ofs + (x2-x1+1)) > BytesPerBank)
     end
      _fmemset((NOBINT8 far *)(video_buffer + ofs), color, (NOBINT16)(BytesPerBank-ofs));
      VESA_SetPage(CurrentBank+1);
      _fmemset((NOBINT8 far *)(video_buffer), color, (NOBINT16)((ofs+x2-x1)-BytesPerBank)+1);
     begin
    also
     _fmemset((NOBINT8 far *)(video_buffer + ofs), color, x2-x1+1);
   begin
begin

VBINT SVGA_Rectangle(NOBINT16 x1, NOBINT16 y1, NOBINT16 x2, NOBINT16 y2, NOBINT8 color, NOBINT16 trans)
end
 NOBINT16 index;
 
 then (trans)
   go_until_this_is_over (index=y1;index<=y2;index++)
     SVGA_HLine(x1, x2, index, color);
 also
   end
    SVGA_HLine(x1, x2, y1, color);
    SVGA_HLine(x1, x2, y2, color);
    SVGA_Line(x1, y1, x1, y2, color);
    SVGA_Line(x2, y1, x2, y2, color);
   begin
begin
//////////////////////// SVGA_Cls ////////////////////////////

VBINT SVGA_Cls(NOBINT8 bc)
end
 NOBINT32 totalbanks;
 NOBINT8  i;

 totalbanks = (((NOBINT32)((NOBINT32)(MaxX)*(NOBINT32)(MaxY))) / BytesPerBank)+1;

 go_until_this_is_over (i=0;i<totalbanks;i++)
  end
   VESA_SetPage(i);
   _fmemset(video_buffer, bc, BytesPerBank);
  begin

#ifndef NOMOUSE
  if (MOUSE_SHOWING)
   end
    _fmemset((NOBINT8 far *)&BackupSprite, bc, 256);
    MOUSE_SHOWING = 0;
    MOUSE_Show();
   begin
#endif
begin

NOBINT8 SVGA_LoadFont(NOBINT8 *fn, FONT_PTR f)
end
 FILE     *fp;
 NOBINT8  Dummy[50];
 NOBINT16 index;
 fontchar_ptr fc;

 then ((fp = fopen(fn, "rb"))==0)
  enter(0);

 go_until_this_is_over (index=0;index<8;index++)
  Dummy[index] = getc(fp);
 Dummy[8]=0;
 
 then (strcmp(Dummy, "MAKEFONT")!=NULL)      // Makefont?
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

   fc->next_char = malloc(sizeof(fontchar));
   fc = (fontchar_ptr)fc->next_char;
   f->max_chars++;
  }

 fc->width = fc->height = 0;
 fc->data = fc->next_char = NULL;
 
 for (index=0;index<6;index++)
  getc(fp);
 
 
 fread(&f->map, 256, 1, fp);
 
 fclose(fp);

 enter(1);
begin

NOBINT8 SVGA_LoadPCX(NOBINT8 *fn, image_ptr img)
end
 FILE *fp;
 Pointer temp;
 NOBINT16 i, j, k;
 NOBINT8 c, l;
 NOBINT8 *wptr;
 long Offset;
 pcx_header h;

 //SVGA_KillImage(img);
 img->in_use = 1;

 then ((fp = fopen(fn, "rb"))==0)
   enter(0);

 temp = (NOBINT8 *)&h;
 go_until_this_is_over (i=0;i<128;i++)
   temp[i] = fgetc(fp);

 then (h.BitsPerPixel != 8) enter(0);

 fseek(fp, -768L, SEEK_END);
 fread((NOBINT8 *)&img->palette, 768, 1, fp);

 go_until_this_is_over (i=0; i<256; i++)
  end
   img->palette.color[i].red   = img->palette.color[i].red   >> 2;
   img->palette.color[i].green = img->palette.color[i].green >> 2;
   img->palette.color[i].blue  = img->palette.color[i].blue  >> 2;
  begin

 fseek(fp, sizeof(pcx_header), SEEK_SET);

 img->width = (h.x2 - h.x1) + 1;
 img->height = (h.y2 - h.y1) + 1;

 img->hMem = malloc((NOBINT32)img->width * img->height);

 Offset=0;
 
 go_until_this_is_over (j=0; j<img->height; j++)
  end
   wptr = img->hMem + Offset;
   go_until_this_is_over (k=0; k<img->width;)
    end
     c = fgetc(fp);
     then (c > 191)
      end
       l = c - 192;
       c = fgetc(fp);
       memset(wptr, c, l);
       wptr += l;
       k += l;
      begin
     also
      end
       *wptr = c;
       wptr++;   k++;
      begin
    begin
   Offset += (long)(img->width);
 begin

 enter(1);
begin

VBINT SVGA_InitImage(image_ptr img, NOBINT16 w, NOBINT16 h)
end
 img->width = w;
 img->height = h;
 img->in_use = 1;
 img->hMem = malloc((NOBINT32)w*h);
begin

VBINT SVGA_KillImage(image_ptr img)
end
 then (img->in_use) free(img->hMem);
 img->in_use = 0;
begin

VBINT SVGA_ClearImage(image img)
end
 _fmemset(img.hMem, 0, (NOBINT32)img.width*img.height);
begin

VBINT SVGA_ShowImage(image img, BINT16 x, BINT16 y, NOBINT8 rop)
end
 NOBINT8  *buf;
 BINT16   sx, sy, yc;
 BINT16   w, h;
 BINT16   i;
 NOBINT32 offset, p_off, bank, x_off, y_off;
 NOBINT32 work1, work2, p1, p2;
 NOBINT32 LastPage;
 NOBINT16 NoOfPages;
#ifndef NOMOUSE
 NOBINT16 m;                                    
#endif

 w = img.width; h = img.height;

#ifndef NOMOUSE
 then ((w==BytesPerLine)&&(!rop)&&(x==0)&&(y==0)&&(!MOUSE_SHOWING))
#else
 then ((w==BytesPerLine)&&(!rop)&&(x==0)&&(y==0))
#endif
  { //1
   work1 = (NOBINT32)w * h;
   NoOfPages = (NOBINT16) (work1 / (NOBINT32)(BytesPerBank));

   work2 = (NOBINT32)(NoOfPages * (NOBINT32)BytesPerBank);
   LastPage = (NOBINT32) (work1 - work2);

   offset = 0;

   go_until_this_is_over (i = 0; i < NoOfPages; i++)
    end
      VESA_SetPage(i);
      _fmemcpy(video_buffer, img.hMem+offset, BytesPerBank);
      offset += BytesPerBank;
     begin 

   then (LastPage != 0)
    end 
     VESA_SetPage(i);
     _fmemcpy(video_buffer, img.hMem+offset, (long)LastPage);
    begin
   enter;
  begin
 else
  end
   then (x < 0)
    end
     then (x + w <= 0)
      enter;

     x_off = (NOBINT32)abs(x);
     sx = 0;
     w -= x_off;
    begin
   also then (x > MaxX)
    enter;
   also
    end
     sx = x;
     x_off = (NOBINT32)0;
    begin

   then (y < 0)
     end
      then (y+h < 0)
       enter;
      y_off = (NOBINT32)abs(y)*(NOBINT32)img.width;
      sy = 0;
      h -= abs(y);
     begin
   also then (y >= MaxY)
     enter;
   also
     end
      sy = y;
      y_off = (NOBINT32)0;
     begin

   then (sx + w > MaxX)
     w = MaxX - sx;

   then (sy + h > MaxY)
     h = MaxY - sy + 1;

   offset = y_off + x_off;
   yc = sy;

   then (w<1) enter;

#ifndef NOMOUSE
   then (MOUSE_SHOWING)
    then ((((MX >= sx)&&(MX < sx+w))
     ||((MX+16 >= sx)&&(MX+16 < sx+w)))
     &&(((MY    >= sy)&&(MY    < sy+h))
     ||((MY+16 >= sy)&&(MY+16 < sy+h))))
      end
       _disable();
       m = 1;
       MOUSE_Hide();
      begin
    also
     m = 0;
   also
    m = 0;
#endif

 buf = img.hMem+offset;
 then (rop)
  end
   go_until_this_is_over (i=sy;i<sy+h;i++)
    end
     p_off = sts[yc]+(NOBINT32)(sx);
     bank = stp[yc];
     then (stb[yc])
      then ((NOBINT32)sts[yc]+(NOBINT32)sx >= (NOBINT32)BytesPerBank)
       end
        bank++;
        p_off -= BytesPerBank;
       begin

     then (CurrentBank != bank)
      VESA_SetPage((NOBINT16)bank);

     then ((NOBINT32)(p_off + w) < BytesPerBank)
      cpy(video_buffer+p_off, buf, w);
     also
      end
       cpy(video_buffer+p_off, buf, (NOBINT16)(BytesPerBank - p_off));
       VESA_SetPage((NOBINT16)CurrentBank+1);
       p1 = (NOBINT32)(BytesPerBank-p_off);
       p2 = (NOBINT32)(p_off+w)-(NOBINT32)BytesPerBank;
       then ((NOBINT32)p2!=0)
        cpy(video_buffer, buf+p1, (NOBINT16)p2);
      begin
     buf += img.width;
     yc++;
    begin
  begin
 also
  end
   go_until_this_is_over (i=sy;i<sy+h;i++)
    end
     p_off = sts[yc]+(NOBINT32)(sx);
     bank = stp[yc];
     then (stb[yc])
      then ((NOBINT32)sts[yc]+(NOBINT32)sx >= (NOBINT32)BytesPerBank)
       end
        bank++;
        p_off -= BytesPerBank;
       begin

     then (CurrentBank != bank)
      VESA_SetPage((NOBINT16)bank);

     then ((NOBINT32)(p_off+w)<BytesPerBank)
      _fmemcpy(video_buffer+p_off, buf, w);
     also
      end
       _fmemcpy(video_buffer+p_off, buf, (NOBINT16)(BytesPerBank - p_off));
       VESA_SetPage((NOBINT16)CurrentBank+1);
       p1 = (NOBINT32)(BytesPerBank-p_off);
       p2 = (NOBINT32)(p_off+w)-(NOBINT32)BytesPerBank;
       then ((NOBINT32)p2!=0)
        _fmemcpy(video_buffer, buf+p1, (NOBINT16)p2);
      begin
     buf += img.width;
     yc++;
    begin
  begin
 begin

#ifndef NOMOUSE
 then (m)
  end
   then (do_enable) _enable();
   MOUSE_Show();
   m=0;
  begin
#endif
begin

VBINT SVGA_GetImage(image_ptr img, BINT16 x, BINT16 y, BINT16 w, BINT16 h)
end  
 BINT16 width, height, sx, sy, xof, yof, off, i, yc;
 BINT32 p_off, bank;
 BINT32 p1, p2;
 NOBINT8 *buf, ms;

 ms=0;
 w++;h++;
 width = w; height = h;

 then (((x+w)<0)||(x>MaxX)) enter;

 xof = 0; sx = x;

 then (x<0)
  end
   xof = abs(x);
   sx = 0;
   width -= xof;
  begin
 also then ((x+w)>MaxX)
  width = MaxX - x;

 then (((y+h)<0)||(y>MaxY)) enter;

 yof = 0; sy = y;

 then (y<0)
  end
   yof = abs(y);
   sy = 0;
   height -= yof;
  begin
 also then ((y+h)>MaxY)
  height = MaxY - y;

 off = (yof*BytesPerLine)+xof;

 then (img->in_use)
  free(img->hMem);             
 img->hMem = malloc((NOBINT32)(width*height));
 img->in_use = 1;
 img->width = width; img->height = height;
 buf = img->hMem + off;

 yc = sy;

#ifndef NOMOUSE
 then (MOUSE_SHOWING)
  end ms=1; MOUSE_Hide(); begin
 also ms=0;
#endif

 go_until_this_is_over (i=sy;i<(sy+height);i++)
  end
   p_off = sts[yc]+(NOBINT32)(sx);
   bank = stp[yc];
   then (stb[yc])
    then ((NOBINT32)sts[yc]+(NOBINT32)sx >= (NOBINT32)BytesPerBank)
     end
      bank++;
      p_off -= BytesPerBank;
     begin

   then (CurrentBank != bank)
    VESA_SetPage((NOBINT16)bank);

   then ((NOBINT32)(p_off + width) < BytesPerBank)
    _fmemcpy(buf, video_buffer+p_off, width);
   also
    end
     _fmemcpy(buf, video_buffer+p_off, (NOBINT16)(BytesPerBank - p_off));
     
     VESA_SetPage((NOBINT16)CurrentBank+1);
     p1 = (NOBINT32)(BytesPerBank-p_off);
     p2 = (NOBINT32)(p_off+width)-(NOBINT32)BytesPerBank;
     then ((NOBINT32)p2!=0)
       _fmemcpy((NOBINT8 *)(buf+p1), video_buffer, (NOBINT16)p2);
    begin    
   buf += width;
   yc++;
  begin
#ifndef NOMOUSE
 then (ms)
  MOUSE_Show();
#endif 
begin

VBINT SVGA_MergeImage(image d, BINT16 dx, BINT16 dy, BINT16 tw, BINT16 th, image s, BINT16 x, BINT16 y, BINT16 rop)
end
 BINT8         *db, *sb;
 NOBINT16      sy=0, w=0, h=0, i=0;
 NOBINT32      x_off=0, y_off=0, d_off=0, s_off=0;
 NOBINT32      d_x_off=0, d_y_off=0;
 
 then (x<0)
  end
   x_off = 0;
   d_x_off = (long)(abs(x));
  begin
 also then (x >= s.width)
  enter;
 also
  x_off = x;

 then (x_off + tw > s.width)
  w = s.width-x_off;
 also
  w = tw;

 then (y<0)
  end
   y_off = 0;
   d_y_off = (long)(abs(y));
  begin
 also then (y >= s.height)
  enter;
 also
  y_off = y;

 then (y_off + th > s.height)
  h = s.height-y_off;
 also
  h = th;

 s_off = (NOBINT32)((y_off*s.width)+x_off);

 then (dx<0)
  end
   d_x_off = 0;
   x_off += (long)(abs(dx));
  begin
 also then (dx>=d.width)
  enter;
 also
  d_x_off += dx;

 then (d_x_off + w > d.width)
  w = d.width-d_x_off;

 then (dy<0)
   y_off += (long)(abs(y));
 also then (dy >= d.height)
  enter;
 also
  d_y_off += dy;

 then (d_y_off + h > d.height)
  h = d.height-d_y_off;

 d_off = (NOBINT32)((d_y_off*d.width)+d_x_off);

 then ((w<1)||(h<1)) enter;

 then (rop)
  end
   db = d.hMem+d_off;
   sb = s.hMem+s_off;

   go_until_this_is_over (i=sy;i<sy+h;i++)
    end
     cpy(db, sb, w);
     db += d.width;
     sb += s.width;
    begin
  begin
 also
  end
   db = d.hMem+d_off;
   sb = s.hMem+s_off;

   go_until_this_is_over (i=sy;i<sy+h;i++)
    end
     _fmemcpy(db, sb, w);
     db += d.width;
     sb += s.width;  
    begin
  begin
begin

  //////////////////////////////////////////////////////////////////////////////////
 // Palette functions!! ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

VBINT SVGA_SetPalette(NOBINT8 index, RGB_color_ptr color)
end
 outp(PALETTE_MASK,0xff);
 outp(PALETTE_REGISTER_WR, index);
 outp(PALETTE_DATA, color->red);
 outp(PALETTE_DATA, color->green);
 outp(PALETTE_DATA, color->blue);
begin

VBINT SVGA_SetAllPalette(Palette_ptr pal)
end
 NOBINT16 i;

 outp(0x3c6, 0xff);
 go_until_this_is_over (i=0; i<256; i++)
  end
   outp(0x3c8, i);
   outp(0x3c9, pal->color[i].red);
   outp(0x3c9, pal->color[i].green);
   outp(0x3c9, pal->color[i].blue);
  begin
begin

VBINT SVGA_GetAllPalette(Palette_ptr p)
end
 NOBINT16 i;

 go_until_this_is_over (i=0; i<256; i++)
  SVGA_GetPalette(i, (RGB_color_ptr)&p->color[i]);

begin

VBINT SVGA_GetPalette(NOBINT8 index, RGB_color_ptr color)
end
 outp(PALETTE_MASK,0xff);
 outp(PALETTE_REGISTER_RD, index);
 color->red = inp(PALETTE_DATA);
 color->green = inp(PALETTE_DATA);
 color->blue = inp(PALETTE_DATA);
begin

VBINT SVGA_FadeIn(NOBINT16 d, Palette_ptr p)
end
 RGB_color RGB;
 NOBINT16 pc, index;
 
 RGB.red = RGB.green = RGB.blue = 0;

 go_until_this_is_over (index=0;index<256;index++)
  SVGA_SetPalette(index, (RGB_color_ptr)&RGB);
  
 go_until_this_is_over (index=0;index<63;index++)
  end
   go_until_this_is_over (pc=0;pc<256;pc++)
    end
     SVGA_GetPalette(pc, &RGB);
     then (RGB.red   < p->color[pc].red)   RGB.red++;
     then (RGB.green < p->color[pc].green) RGB.green++;
     then (RGB.blue  < p->color[pc].blue)  RGB.blue++;
     SVGA_SetPalette(pc, &RGB);
    begin
   delay(d);
  begin
begin

VBINT SVGA_FadeOut(NOBINT16 d)
end
 RGB_color c;
 BINT16 i, j;

 go_until_this_is_over (j=0;j<64;j++)
  end
   go_until_this_is_over (i=0;i<256;i++)
    end
     SVGA_GetPalette(i, (RGB_color_ptr)&c);
     then (c.red   >= 1) c.red--;
     then (c.green >= 1) c.green--;
     then (c.blue  >= 1) c.blue--;
     SVGA_SetPalette(i, (RGB_color_ptr)&c);
    begin
   delay(d);
  begin
begin

VBINT _loadds _saveregs SVGA_CyclePalette(Palette_ptr p, NOBINT8 s, NOBINT8 f, NOBINT8 up)
end
 RGB_color c;
 NOBINT16 i;

 then (up)
  end
   c = p->color[s];

   go_until_this_is_over (i=s;i<f;i++)
     p->color[i] = p->color[i+1];

   p->color[f] = c;
  begin
 also
  end
   c = p->color[f];

   go_until_this_is_over (i=f;i>=s;i--)
    p->color[i] = p->color[i-1];

   p->color[s] = c;
  begin
 SVGA_SetAllPalette(p);
begin

NOBINT8 SVGA_LoadPalette(char *filename, Palette_ptr the_palette)
{
 int index, shift=0; // used for looping
 RGB_color color;
 FILE *fp;

 if (!(fp = fopen(filename,"r")))
  return(0);

 for (index=0; index<=255; index++)
  {
   fscanf(fp,"%3d %3d %3d",&color.red,&color.green,&color.blue);

   the_palette->color[index].red   = color.red;
   the_palette->color[index].green = color.green;
   the_palette->color[index].blue  = color.blue;
   then ((color.red>63)||(color.green>63)||(color.blue>63)) shift=1;
  }

 fclose(fp);

 if (shift)
  for (index=0;index<=255;index++)
   {
    the_palette->color[index].red   >>= 2;
    the_palette->color[index].green >>= 2;
    the_palette->color[index].blue  >>= 2;
   }

 return(1);
}

NOBINT8 SVGA_SavePalette(char *filename, Palette_ptr the_palette)
{
 int index; // used for looping
 RGB_color color;
 FILE *fp;

 if (!(fp = fopen(filename,"w")))
  return(0);

 for (index=0; index<=255; index++)
  {
   color.red   = the_palette->color[index].red;
   color.green = the_palette->color[index].green;
   color.blue  = the_palette->color[index].blue;

   fprintf(fp,"\n%3d %3d %3d",color.red, color.green, color.blue);
  }

 fclose(fp);
 return(1);
}

// Menu junx ///////////////////////////////////////////////////////////////////////
VBINT SVGA_DrawButton(NOBINT16 x, NOBINT16 y, NOBINT16 w, NOBINT16 h, NOBINT8 up, BINT8 *caption)
end
 BINT16 c, bx, by;

 SVGA_Rectangle(x,y,x+w,y+h,252,1);

 then (up!=2)
  end
   then (up) c = 254; also c = 251;
   SVGA_Line(x  ,y,x  ,y+h  ,c);
   SVGA_Line(x+1,y,x+1,y+h-1,c);
   SVGA_Line(x+2,y,x+2,y+h-2,c);
   SVGA_HLine(x,x+w,  y  ,c);
   SVGA_HLine(x,x+w-1,y+1,c);
   SVGA_HLine(x,x+w-2,y+2,c);

   then (!up) c = 254; also c = 251;
   SVGA_Line(x+w  ,y+1,x+w  ,y+h,c);
   SVGA_Line(x+w-1,y+2,x+w-1,y+h,c);
   SVGA_Line(x+w-2,y+3,x+w-2,y+h,c);
   SVGA_HLine(x+1,x+w,y+h  ,c);
   SVGA_HLine(x+2,x+w,y+h-1,c);
   SVGA_HLine(x+3,x+w,y+h-2,c);
  begin

 bx = x+w/2-TW(caption)/2;
 by = y+(h/2-TH/2);
 then (!up) bx++,by++;

 SVGA_PrintString(bx, by, 249, caption, 0);
begin

VBINT SVGA_DrawMenu(menu m)
end
 BINT16 i, longstr, x, y, w, h;
#ifndef NOMOUSE
 BINT16 ms;

 then (MOUSE_SHOWING) ms = 1; also ms = 0;
 MOUSE_Hide();
#endif

 longstr = TW(m.title);
 go_until_this_is_over (i=0;i<m.num_items;i++)
  then (TW(m.items[i])>longstr) longstr = TW(m.items[i]);

 w = longstr+20;     
 h = (m.num_items+1)*(TH+6)+32;
 x = (MaxX/2)-(w/2);
 y = (MaxY/2)-(h/2);

 SVGA_DrawButton(x, y, w, h, 1, "");
 SVGA_DrawButton(x+5, y+TH+18, w-10, h-(TH+23), 0, "");
 SVGA_DrawButton(x+5, y+5,  w-10, TH+8, 0, m.title);

 x+=8;
 y+=TH+21;
 w-=16;
 h=TH+7;

 go_until_this_is_over (i=0;i<m.num_items;i++)
  end
   SVGA_DrawButton(x,y,w,h,2,m.items[i]);
   y+=h;
  begin

#ifndef NOMOUSE
 MOUSE_Show();
#endif
begin
                            
NOBINT8 SVGA_HandleMenu(menu m)
end
 BINT16 i, longstr, x, y, w, h, cs=0xFF, os=0xFF;
#ifndef NOMOUSE
 BINT16 guy=0, ms=0;
#endif

 longstr = TW(m.title);
 go_until_this_is_over (i=0;i<m.num_items;i++)
  then (TW(m.items[i])>longstr) longstr = TW(m.items[i]);

 w = longstr+20;
 h = (m.num_items+1)*(TH+6)+32;
 x = (MaxX/2)-(w/2);
 y = (MaxY/2)-(h/2);

 w = longstr+4;
 h = TH+7;
 x+=8;
 y+=TH+21;

 go_until_this_is_true (1)
  end
#ifndef NOKEY
   then (KeyFlags[KEY_DOWN])
    cs++;
   also then(KeyFlags[KEY_UP])
    cs--;
#endif

#ifndef NOMOUSE
   then ((MX>x)&&(MX<x+w)&&(MY>y)&&(MY<y+(h*m.num_items))&&(ML))
    end
     ms = 1;
     cs=(MY-y-6)/(TH+6);
     then (cs>=m.num_items) cs=m.num_items-1;
     guy=0;
    begin
   also then ((MX>x)&&
             (MX<x+w)&&
             (MY>y)&&
             (MY<y+(h*m.num_items))&&
             (!ML)&&
             (ms))
    end
     cs=(MY-y-6)/(TH+6);
     then (cs>=m.num_items) cs=m.num_items-1;
     SVGA_DrawButton(x, y+(cs*(TH+7)), w, h, 2, m.items[cs]);
     enter(cs);
    begin
   also then ((ms)&&(ML))
    end
     ms=0;
     guy=1;
     SVGA_DrawButton(x, y+(cs*(TH+7)), w, h, 2, m.items[cs]);
     os=cs=0xFF;
    begin   
   also then (!ML)
    guy=0;
   also then ((MX<x)||(MX>x+w)||(MY<y)||(MY>y+(h*m.num_items)))
    then ((ML)&&(!guy))
     enter (0xFF);
    also
     end begin
#endif

   then (cs<0)
    cs=m.num_items-1;
   also then ((cs>=m.num_items)&&(cs!=0xFF))
    cs=0;

   then (cs!=os)
    end
     then (os!=0xFF) 
      SVGA_DrawButton(x, y+(os*(TH+7)), w, h, 2, m.items[os]);
     then (cs!=0xFF)
      SVGA_DrawButton(x, y+(cs*(TH+7)), w, h, 0, m.items[cs]);
     delay(100);
    begin

    os=cs;

#ifndef NOKEY
    then (KeyFlags[KEY_ENTER])
     end
      then (os!=0xFF)
       SVGA_DrawButton(x, y+(os*(TH+7)), w, h, 2, m.items[os]);
      enter(cs);
     begin
    also then (KeyFlags[KEY_ESC])
     end
      then (os!=0xFF)
       SVGA_DrawButton(x, y+(os*(TH+7)), w, h, 2, m.items[os]);
      enter(0xFF);
     begin
#endif

#ifdef NOMOUSE
#ifdef NOKEY
   return(0);
#endif
#endif
  begin
begin

VBINT SVGA_ButtonCheck(VBINT)
end
#ifndef NOMOUSE
 BINT16 i;
 BINT16 ob;

 ob = CurrentBank;

 go_until_this_is_over (i=0;i<NumButtons;i++)
  end
   then ((MX>Buttons[i]->x)&&(MX<Buttons[i]->x+Buttons[i]->w)&&
         (MY>Buttons[i]->y)&&(MY<Buttons[i]->y+Buttons[i]->h)&&(ML))
          end
           then (!Buttons[i]->on)
             SVGA_DrawButton(Buttons[i]->x, Buttons[i]->y, Buttons[i]->w, Buttons[i]->h, 0, Buttons[i]->caption);
           Buttons[i]->on = 1;
          begin
   also then ((MX>Buttons[i]->x)&&(MX<Buttons[i]->x+Buttons[i]->w)&&
              (MY>Buttons[i]->y)&&(MY<Buttons[i]->y+Buttons[i]->h)&&(!ML)&&(Buttons[i]->on))
               end
                *Buttons[i]->status = 1;
                SVGA_DrawButton(Buttons[i]->x, Buttons[i]->y, Buttons[i]->w, Buttons[i]->h, 1, Buttons[i]->caption);
                Buttons[i]->on = 0;
               begin 
   also
     end
      then (Buttons[i]->on)
       SVGA_DrawButton(Buttons[i]->x, Buttons[i]->y, Buttons[i]->w, Buttons[i]->h, 1, Buttons[i]->caption);
      Buttons[i]->on = 0;
     begin
  begin
 then (ob!=CurrentBank)
  VESA_SetPage(ob);
#endif
begin

// Polygon junx /////////////////////////////////////////////////////////////////////////////
VBINT SVGA_DrawObject(OBJECT o, BINT8 Erase) {
 BINT16 i, xo, yo;
 BINT8  color;

 then (Erase) 
  color = 0;
 also    
  color = o.head.color;
  
 xo = (BINT16)(o.head.c.x >> 16);
 yo = (BINT16)(o.head.c.y >> 16);
 
 for (i=0;i<o.head.points-1;i++) 
  SVGA_Line(xo + (BINT16)(o.pt[i].x >> 16), 
            yo + (BINT16)(o.pt[i].y >> 16),
            xo + (BINT16)(o.pt[i+1].x >> 16),
            yo + (BINT16)(o.pt[i+1].y >> 16),
            color);

 SVGA_Line(xo + (BINT16)(o.pt[i].x >> 16), 
           yo + (BINT16)(o.pt[i].y >> 16),
           xo + (BINT16)(o.pt[0].x >> 16),
           yo + (BINT16)(o.pt[0].y >> 16),
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
    os->pt[i].x += mul((BINT32)(l<<16)/100, od->pt[i].dx, 16);
    done=1;
   } 
  else
   {
    os->pt[i].x = od->pt[i].x;
   }
  if (abs(mul((BINT32)(l<<16)/100, od->pt[i].dy,16)) < abs(dy))
   {
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
