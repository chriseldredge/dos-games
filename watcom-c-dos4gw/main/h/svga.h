//////////////////////////////////////////////////////////////////////
// SVGA.H - Header file for 256 color super-vga graphics found in   //
// toolkit.lib.  All code written and distributed by:               //
//  Chris Eldredge <wantaco@erols.com>                              //
//  3 Thorburn Place, Gaithersburg, MD 20878                        //
//  (301) 424-0253                                                  //
//  -AND-                                                           //
//  Jason Martin (Not currently available)                          //
//                                                                  //
//////////////////////////////////////////////////////////////////////

#ifdef SB_H
 #error (32)
#endif

#define SB_H

#include <stdlib.h>
#include <dos.h>
#include <malloc.h>
#include <stdio.h>
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Defines                                                          //
//////////////////////////////////////////////////////////////////////
#define MODE_8_BIT     8
#define MODE_16_BIT    16
#define MODE_32_BIT    32
#define MODE_ANY       0xFF

#define MAP            CurrentFont->map         // Font ascii remaping

#define VGA_320x200       0x013                 // All currently standard
#define VGA_320x200x16    0x10E
#define VGA_320x200x24    0x10F
#define SVGA_640x400      0x100                 // VESA video modes and
#define SVGA_640x480      0x101                 // also VGA 320x200
#define SVGA_640x480x16   0x111
#define SVGA_640x480x24   0x112
#define SVGA_800x600      0x103
#define SVGA_800x600x16   0x114
#define SVGA_800x600x24   0x115
#define SVGA_1024x768     0x105
#define SVGA_1024x768x16  0x117
#define SVGA_1280x1024    0x107
#define SVGA_1600x1200    0x120
#define TEXT_MODE         0x003

#define BANK_WR        0
#define BANK_RD        1

#define MODE_SUPPORTED 0x001                 // VESAMODEINFO.modeattr
#define MODE_COLOR     0x008                 // masks to get data
#define MODE_GRAPHICS  0x010

#define PALETTE_MASK        0x3C6            // the bit mask register
#define PALETTE_REGISTER_RD 0x3C7            // set read index at this I/O
#define PALETTE_REGISTER_WR 0x3C8            // set write index at this I/O
#define PALETTE_DATA        0x3C9            // the R/W data is here

#define BLIT_NORMAL     0                    // blit with foreground, normal
#define BLIT_SPRITE     1                    // blit with foreground, sprite
#define BLIT_NTSPRITE   2                    // blit with no foreground, sprite     
#define BLIT_NTN        3                    // blit with no foreground, normal
#define BLIT_AND_MASK   4                    // blit no forground, and with SVGA_Mask
#define BLIT_PAL        5

#define Black        0                       // Default palette colors 0-15
#define Blue         1
#define Green        2
#define Cyan         3
#define Red          4
#define Magenta      5
#define Brown        6
#define LightGray    7
#define DarkGray     8
#define LightBlue    9
#define LightGreen   10
#define LightCyan    11
#define LightRed     12
#define LightMagenta 13
#define Yellow       14
#define White        15

#define TW           SVGA_TextWidth          // Used to get width of 
#define TH           SVGA_TextHeight         // specific string

#define kAngle360               256
#define kAngleMask              (kAngle360 - 1)
#define kAngle180               (kAngle360 / 2)
#define kAngle90                (kAngle360 / 4)
#define kPi                     3.141592654

#define MAX_POINTS   16                      // OBJECT things
#define MAX_OBJECTS  10
#define PI           3.141592654

#define MAX_BUTS     10                      // Interface thing

//////////////////////////////////////////////////////////////////////
// Structures                                                       //
//////////////////////////////////////////////////////////////////////
typedef struct CHAR_TYP                      // Character for font files
   {
    NOBINT8     width;
    NOBINT8     height;
    NOBINT8     *data;
    NOBINT8     *next_char;
   } FONTCHAR, *FONTCHAR_PTR;

typedef struct FONT_TYP                      // Font data structure
   {
    NOBINT16    romset;
    NOBINT8     width;
    NOBINT8     height;
    NOBINT8     BitsPerPixel;
    NOBINT8     max_chars;
    FONTCHAR    startchar;
    NOBINT8     map[256];
   } FONT, *FONT_PTR;

typedef struct RGB_TYP            // Red, Green and Blue of one color
   {
    NOBINT8 red;    // red   component of color 0-63
    NOBINT8 green;  // green component of color 0-63
    NOBINT8 blue;   // blue  component of color 0-63
   } RGB, *RGB_PTR;

typedef struct PALETTE_TYP        // 256 RGB's
   {
    RGB color[256];
   } PALETTE, far *PALETTE_PTR;

typedef struct PCXHEAD_TYP        // For reading PCX files
   {
        BINT8    manufacturer;   // Always 10
        BINT8    version;        // 1, 2, 3 or 5
        BINT8    encoding;       // Always 1
        BINT8    BitsPerPixel;   // For our purposes, should be 8
        NOBINT16 x1, y1;         // Other stuff...
        NOBINT16 x2, y2;
        NOBINT16 hres;
        NOBINT16 vres;
        BINT8    palette[48];
        BINT8    reserved;
        BINT8    colour_planes;
        NOBINT16 BytesPerLine;
        NOBINT16 PaletteType;
        BINT8    filler[58];
} PCXHEAD, *PCXHEAD_PTR;
  
typedef struct IMAGE_TYP        // Used for all Image functions
  {
   NOBINT8 *hMem;
   BINT8    in_use;
   NOBINT16 bytesperpixel;
   NOBINT16 width, height;
   PALETTE  palette;
  } IMAGE, *IMAGE_PTR;
  
typedef struct VESAINFO_TYP // Structure to recieve information about
  {                           // video card
   BINT8    signature[4];     // 'VESA'
   NOBINT16 version;          // hopefully 1.2 or better
   BINT8    *oem_string;      // REAL MODE pointer to manufacturer string
   NOBINT8  capabilities[4];  // Never could find this useful!
   NOBINT16 *video_mode;      // REAL MODE pointer to 0xFF terminated
                              // string of supported video modes
   BINT16   video_mem;        // Number of banks on card
   BINT8    dummy[236];       // Reserved for future expansion
  } VESAINFO, *VESAINFO_PTR;

typedef struct VESAMODEINFO_TYP // For information specific to video mode
  {
   NOBINT16 mode_attr;          // Mode attributes
   NOBINT8  win_a_attr;         // attributes of window 1 or a
   NOBINT8  win_b_attr;         // attributes of window 2 or b
   NOBINT16 granularity;        // size of banks
   NOBINT16 win_size;           // see previous comment
   NOBINT16 win_a_segment;      // REAL SEGMENT where window 1 or a starts
   NOBINT16 win_b_segment;      // REAL SEGMENT where window 2 or b starts
   NOBINT32 page_switch_function_ptr; // REAL MODE Pointer to page switch function
   NOBINT16 bytes_per_scan_line; // X resolution

   NOBINT16 x_res;              // X resolution
   NOBINT16 y_res;              // Y resolution
   NOBINT8  x_char_size;        // text mode char width and
   NOBINT8  y_char_size;        // height
   NOBINT8  number_of_planes;   // For us, always 1
   NOBINT8  bits_per_pixel;     // For us, always 8
   NOBINT8  number_of_banks;    // Number of banks needed for this mode
   NOBINT8  memory_model;       // Don't reckon I know!
   NOBINT8  bank_size;          // Don't reckon I know!
   BINT8    dummy[227];         // Reserved for future expansion  
  } VESAMODEINFO, *VESAMODEINFO_PTR;

typedef struct POINT_TYP        // X, Y co-ordinate
 {
        BINT32   x;
        BINT32   y;
        BINT32   dx;
        BINT32   dy;
 } POINT, *POINT_PTR;

typedef struct OBJECT_HDR_TYP // Information about object
 {
        BINT16     angle;
        BINT16     color;
        BINT16     fillcolor;
        BINT16     points;     // Number of points
        BINT16     morphing;   // In morphing phase
        POINT      c;          // center x,y
        POINT      velocity;   // Speed to move when called to SVGA_ModeObject
 } OBJECT_HDR, *OBJECT_HDR_PTR;

typedef struct OBJECT_TYP     // OBJECT info
 {
        OBJECT_HDR      head;
        POINT           pt[MAX_POINTS];
 } OBJECT, *OBJECT_PTR;

//////////////////////////////////////////////////////////////////////
// Prototypes                                                       //
//////////////////////////////////////////////////////////////////////
// Low level junx                                                   //
VBINT  tset(BINT8 *to, NOBINT32 c, BINT32 w);                    //
VBINT  scpy(BINT8 *to, BINT8 *from, NOBINT32 w); //BLIT_NTSPRITE
VBINT  tcpy(BINT8 *to, BINT8 *from, NOBINT32 w); //BLIT_NORMAL/
VBINT  tscpy(BINT8 *to, BINT8 *from, NOBINT32 w);//BLIT_SPRITE/
VBINT  rcpy(BINT8 *to, BINT8 *from, NOBINT32 w);  //BLIT_NTS //
VBINT  cpyandc(BINT8 *to, BINT8 *from, NOBINT32 wi);
VBINT  cpypal(BINT8 near *to, BINT8 near *from, NOBINT32 wi);
VBINT  FadeSplice(BINT8 *to, BINT8 *from, NOBINT32 wi);

   // Fixed point math thingy. Dont worry about it!                 //
BINT32   mul                   (BINT32   i,                         //
                                BINT32   ii,                        //
                                BINT16   bit);                      //
                                                                    //
// VESA interface junx                                              //
NOBINT8  VESA_SetMode          (NOBINT16     Mode);                 //        
NOBINT8  VESA_ReturnInfo       (VESAINFO     *Buffer);              //
NOBINT8  VESA_ReturnModeInfo   (NOBINT16     Mode,                  //
                                VESAMODEINFO *Buf);                 //
VBINT    VESA_SetPage          (NOBINT16     Page,                  //
                                NOBINT16 window);                   //
                                                                    //
// Initialization junx                                              //
NOBINT8  SVGA_Init             (VBINT);                             //
BINT8    SVGA_SetupMode        (NOBINT16 VMode);                    //
NOBINT16 SVGA_GetUserMode      (BINT16 bpp);                        //
                                                                    //
// Graphics junx                                                    //
VBINT    SVGA_PutPixel         (NOBINT16 Xc,                        //
                                NOBINT16 Yc,                        //
                                NOBINT32 color);                    //
NOBINT32 SVGA_GetPixel         (NOBINT16 Xc,                        //
                                NOBINT16 Yc);                       //
VBINT    SVGA_Line             (NOBINT16 x1,                        //
                                NOBINT16 y1,                        //
                                NOBINT16 x2,                        //
                                NOBINT16 y2,                        //
                                BINT32   color);                    //
VBINT    SVGA_HLine            (NOBINT16 x1,                        //
                                NOBINT16 x2,                        //
                                NOBINT16 y1,                        //
                                NOBINT32 color);                    //
VBINT    SVGA_Rectangle        (NOBINT16 x1,                        //
                                NOBINT16 y1,                        //
                                NOBINT16 x2,                        //
                                NOBINT16 y2,                        //
                                BINT32   color,                     //
                                NOBINT8  trans);                    //
VBINT    SVGA_Cls              (NOBINT32 bc);                       //
                                                                    //
// Font junx                                                        //
VBINT    SVGA_PrintChar        (NOBINT16 xc,                        //
                                NOBINT16 yc,                        //
                                NOBINT32 color,                     //
                                NOBINT8  c,                         //
                                NOBINT32 trans_flag);               //
VBINT    SVGA_PrintString      (NOBINT16 x,                         //
                                NOBINT16 y,                         //
                                NOBINT32 color,                     //
                                STR     *string,                    //
                                NOBINT32 trans_flag);               //
NOBINT8  SVGA_LoadFont         (STR     *fn,                        //
                                FONT_PTR f);                        //
VBINT    SVGA_SetFont          (FONT_PTR f);                        //
NOBINT16 SVGA_TextWidth        (STR      *str);                     //
#define  SVGA_TextHeight        CurrentFont->height                 //
                                                                    //
// Image junx                                                       //
                                                                    //
VBINT    SVGA_SetForeground    (IMAGE_PTR img,                      //
                                BINT16    sh);                      //
VBINT    SVGA_ClearForeground  (BINT16 showbg);                     //
VBINT    SVGA_InitImage        (IMAGE_PTR img,                      //
                                NOBINT16  w,                        //
                                NOBINT16  h);                       //
NOBINT8  SVGA_LoadPCX          (STR       *fn,                      //
                                IMAGE_PTR img);                     //
VBINT    SVGA_SavePCX          (STR       *fn,                      //
                                IMAGE_PTR img);                     //
VBINT    SVGA_MirrorImage      (IMAGE_PTR img);                     //
VBINT    SVGA_ShowImage        (IMAGE_PTR img,                      //
                                BINT16    x,                        //
                                BINT16    y,                        //
                                BINT8     rop);                     //
VBINT    SVGA_GetImage         (IMAGE_PTR img,                      //
                                BINT16    x,                        //
                                BINT16    y,                        //
                                BINT16    w,                        //
                                BINT16    h);                       //
VBINT    SVGA_MergeImage       (IMAGE_PTR d,                        //
                                BINT16    dx,                       //
                                BINT16    dy,                       //
                                BINT16    tw,                       //
                                BINT16    th,                       //
                                IMAGE_PTR s,                        //
                                BINT16    x,                        //
                                BINT16    y,                        //
                                BINT16    rop);                     //
VBINT    SVGA_ClearImage       (IMAGE_PTR img,                      //
                                BINT32    c);                       //
VBINT    SVGA_KillImage        (IMAGE_PTR img);                     //
void SVGA_RSImage(IMAGE_PTR img, IMAGE_PTR dst, BINT16 x, BINT16 y, BINT16 angle, BINT32 scale);
                                                                    //
// Palette junx                                                     //
                                                                    //
VBINT _loadds _saveregs   SVGA_SetPalette       (BINT16      index,                  //
                                RGB_PTR     color);                 //
VBINT _loadds _saveregs   SVGA_GetPalette       (BINT16      index,                  //
                                RGB_PTR     color);                 //
VBINT    SVGA_SetAllPalette    (PALETTE_PTR pal);                   //
VBINT    SVGA_GetAllPalette    (PALETTE_PTR pal);                   //
VBINT    SVGA_FadeIn           (BINT16      d,                      //
                                PALETTE_PTR p);                     //
VBINT    SVGA_FadeOut          (BINT16      d);                     //
VBINT    _loadds _saveregs                                          //
         SVGA_CyclePalette     (PALETTE_PTR p,                      //
                                BINT16      s,                      //
                                BINT16      f,                      //
                                BINT16      up);                    //
NOBINT8  SVGA_LoadPalette      (STR         *filename,              //
                                PALETTE_PTR the_palette);           //
NOBINT8  SVGA_SavePalette      (STR         *filename,              //
                                PALETTE_PTR the_palette);           //
                                                                    //
VBINT    SVGA_ConvertPalette   (STR *fn);                           //
// Polygon junx                                                     //
VBINT    SVGA_DrawObject       (OBJECT_PTR o,                       //
                                BINT8      Erase);                  //
VBINT    SVGA_ScaleObject      (OBJECT_PTR o,                       //
                                BINT32     sx,                      //
                                BINT32     sy);                     //
VBINT    SVGA_MoveObject       (OBJECT_PTR o,                       //
                                BINT32     dx,                      //
                                BINT32     dy,                      //
                                BINT16     clip,                    //
                                OBJECT     box);                    //
VBINT    SVGA_RotateObject     (OBJECT_PTR o,                       //
                                BINT16     Angle);                  //
VBINT    SVGA_MakeRegPoly      (OBJECT_PTR o,                       //
                                BINT16     points,                  //
                                BINT32     x,                      //
                                BINT32     n,                     //
                                BINT32     radius);              //
VBINT    SVGA_StartMorph       (OBJECT_PTR os,                  //
                                OBJECT_PTR od);                //
BINT8    SVGA_MorphObject      (OBJECT_PTR os,                //
                                OBJECT_PTR od,               //
                                BINT8      l);              //
                                                           //
BINT8 SVGA_ClipLine(BINT16 *x1, BINT16 *y1, BINT16 *x2, BINT16 *y2);
BINT8 Sgn(float A);
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Declarations                                                     //
//////////////////////////////////////////////////////////////////////

#ifndef SVGA_C
 extern NOBINT32 WondowControl;
 extern NOBINT16 CurrentBankWr, CurrentBankRd, winattr;
 extern NOBINT32 BytesPerBank;
 extern NOBINT32 SVGA_ColConv[256];
 extern NOBINT16 BytesPerLine;
 extern NOBINT16 MaxX, MaxY;
 extern BINT16   ClipX1, ClipX2, ClipY1, ClipY2;
 extern NOBINT8  *write_buffer, *read_buffer;
 extern NOBINT16 *write_buffer16, *read_buffer16;
 extern NOBINT32 *write_buffer32, *read_buffer32, MaxC, SVGA_Mask;
 extern NOBINT8  *CharSet;
 extern NOBINT32 stx[1601];
 extern NOBINT16 sts[1601], PC;
 extern NOBINT16 do_enable;
 extern BINT8    stb[1601];
 extern BINT8    stp[1601];
 extern FONT_PTR CurrentFont;
 extern FLOTE    SinTable[360];
 extern FLOTE    CosTable[360];
 extern BINT32   SinTRS[kAngle360];
 extern void (near *cpy)(BINT8 *to, BINT8 *from, NOBINT32 w);
 extern unsigned long int fg_offset, bg_offset;
 extern IMAGE    fg_img, bg_img;
 extern BINT16   no_vesa;
 extern BINT16   BytesPerPixel;
#else
 NOBINT32 WindowControl;
 NOBINT16 CurrentBankWr, CurrentBankRd, winattr;
 NOBINT32 BytesPerBank;
 NOBINT32 SVGA_ColConv[256];
 NOBINT16 BytesPerLine;
 NOBINT16 MaxX, MaxY;
 BINT16   ClipX1, ClipX2, ClipY1, ClipY2;
 NOBINT8  *write_buffer, *read_buffer;
 NOBINT16 *write_buffer16, *read_buffer16;
 NOBINT32 *write_buffer32, *read_buffer32, MaxC, SVGA_Mask;
 NOBINT8  *CharSet;
 NOBINT32 stx[1601];
 NOBINT16 sts[1601], PC;
 NOBINT16 do_enable;
 BINT8    stb[1601];
 BINT8    stp[1601];
 FONT_PTR CurrentFont;
 FLOTE    SinTable[360];
 FLOTE    CosTable[360];
 BINT32   SinTRS[kAngle360];
 void (near *cpy)(BINT8 *to, BINT8 *from, NOBINT32 w);
 unsigned long int fg_offset, bg_offset;
 IMAGE    fg_img, bg_img;
 BINT16   no_vesa=0;
 BINT16   BytesPerPixel;
#endif
