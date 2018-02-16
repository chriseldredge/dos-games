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
// COMPILE: This code should be compiled with LARGE model.          //
// AUTHOR:  Chris Eldredge                                          //
//          3 Thorburn Place, Gaithersburg MD, 20878                //
//          (301)-424-0253                                          //
//////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <dos.h>
#include <malloc.h>
#include <stdio.h>
#include "svga.h"
#include "xms.h"
#include "mouse.h"

//////////////////////// SVGA_Init ///////////////////////////////////
int SVGA_Init(void)
{
 VESAINFO vesablock;
 VESAMODEINFO vesamodeblock;
 
 if (!XMS_Init())                    // Graphics cannot operate without
   return(0);                        // extended memory.
   
 VESA_ReturnInfo((char far *)&vesablock);

 // Does the signature say "VESA" ?
 if ((!vesablock.signature[0] && "V") || (!vesablock.signature[1] && "E") ||
     (!vesablock.signature[2] && "S") || (!vesablock.signature[3] && "A"))
      return(0);
 
 /* We want the bank to change with the first call.  To force it to 
    change, we can set CurrentBank to 65536. It is not likely that
    the user will want to access this bank, so it will not be equal
    and therefore, will force a bank change on the first pixel routine. */
    
 CurrentBank = 0xFFFF;


 ROM_SET.Width = 8;
 ROM_SET.Height = 8;
 ROM_SET.BitsPerPixel = 1;
 ROM_SET.MaxChars = 255;
 ROM_SET.CharSet = (char far *)0xF000FA6EL; // rom characters 8x8
 SVGA_SetFont((FONT_PTR)&ROM_SET);
 video_buffer = (char far *)0xA0000000L; // vram byte ptr
 video_buffer_w= (int far *)0xA0000000L; // vram word ptr
 
 return(1);
}
////////////////////////// SVGA_SetupMode ////////////////////////////
int SVGA_SetupMode(int VMode)
{
 VESAMODEINFO vesamodeblock;
 unsigned int j;
 char page=0;
 long runner=0L;
 
 VESA_ReturnModeInfo(VMode, (char far *)&vesamodeblock);

 if (!SVGA_Init())
   return(0);

 // Does the video card support this resolution?
 if (!(vesamodeblock.mode_attr & MODE_SUPPORTED))
   return(0);
 
 // Is the mode 8-bit?
 if (!(vesamodeblock.bits_per_pixel == 8))
   return(0);
   
 WindowControl = vesamodeblock.page_switch_function_ptr;
 BytesPerLine = vesamodeblock.x_res;
 BytesPerBank = (DWORD)((DWORD)vesamodeblock.granularity*1024);
 MaxX = vesamodeblock.x_res; MaxY = vesamodeblock.y_res;
 
 for (j=0; j<MaxY; j++)
  {
   stx[j] = j * BytesPerLine;
   sts[j] = (short int)runner;
   if (runner+MaxX+1 >= BytesPerBank)
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

 VESA_SetMode(VMode);
 return(1);
}

////////////////////////// SVGA_PutPixel /////////////////////////////

void SVGA_PutPixel(unsigned int Xc, unsigned int Yc, unsigned char color)
{
 int bank;
 int pixel_pos;

 pixel_pos = sts[Yc]+(long)(Xc);
 bank = stp[Yc];
 if (stb[Yc])
  if ((DWORD)sts[Yc]+(DWORD)Xc >= (DWORD)BytesPerBank)
    bank++;

 _disable();

 if (MOUSE_SHOWING)
  if ((Xc >= MOUSE_X) && (Xc < MOUSE_X+16) && (Yc >= MOUSE_Y) && (Yc < MOUSE_Y+16))
    if (MOUSE_CURSOR[(Xc-MOUSE_X)+((Yc-MOUSE_Y)*16)] != 0)
     {
      BackupSprite[(Xc-MOUSE_X)+((Yc-MOUSE_Y)*16)] = color;
      _enable();
      return;
     }
    else
     BackupSprite[(Xc-MOUSE_X)+((Yc-MOUSE_Y)*16)] = color;

 if (CurrentBank != bank)
   VESA_SetPage((unsigned int)bank, 1);

 _asm
   {
    mov ax, 0xA000
    mov es, ax
    mov di, word ptr [pixel_pos]
    mov al, [color]
    stosb
   }
 _enable();
}
////////////////////////// SVGA_GetPixel /////////////////////////////
unsigned char SVGA_GetPixel(unsigned int Xc, unsigned int Yc)
{
 long bank, pixel_pos;

 pixel_pos = ((long)(Yc)*BytesPerLine)+(long)(Xc);
 bank = (pixel_pos / BytesPerBank);
 pixel_pos = pixel_pos-(bank*BytesPerBank);

 if (CurrentBank != bank)
   VESA_SetPage((unsigned int)bank, 1);

 if (MOUSE_SHOWING)
  {
   _disable();
   if ((Xc>=MOUSE_X)&&(Xc<MOUSE_X+16)&&(Yc>=MOUSE_Y)&&(Yc<MOUSE_Y+16))
     return(BackupSprite[(Xc-MOUSE_X)+((Yc-MOUSE_Y)*16)]);
   _enable();
  }

 _asm
   {
    mov ax, 0xA000
    mov es, ax
    mov ax, word ptr [pixel_pos]
    mov di, ax
    mov al, es:[di]
   }
}

//////////////////////// SVGA_PrintChar //////////////////////////////
void SVGA_PrintChar(int xc,int yc,char c,int color,int trans_flag)
{
int x,y;
char far *work_char;
unsigned char bit_mask = 0x80;

 if (CHAR_BITS_PER_PIXEL == 1)
  {
   work_char = CharSet + c * CHAR_HEIGHT;

   for (y=0; y<CHAR_HEIGHT; y++)
    {
     bit_mask = 0x80;

     for (x=0; x<CHAR_WIDTH; x++)
      {
        
        if ((*work_char & bit_mask))
             SVGA_PutPixel(xc+x, yc+y, (unsigned char)color);
        else if (trans_flag==1)  // takes care of transparency
             SVGA_PutPixel(xc+x, yc+y, 0);
                
        bit_mask = (bit_mask>>1);

      } // end for x

     work_char++;

    } // end for y
  }
 else
  {
   work_char = CharSet + (c * CHAR_HEIGHT*CHAR_WIDTH);

   for (y=0;y<CHAR_HEIGHT;y++)
     for (x=0;x<CHAR_WIDTH;x++)
       if (work_char[(y*CHAR_WIDTH)+x])
         SVGA_PutPixel(xc+x, yc+y, work_char[(y*CHAR_WIDTH)+x]);
       else if (trans_flag==1)
         SVGA_PutPixel(xc+x, yc+y, 0);         
  }
}

//////////////////////// SVGA_PrintString ////////////////////////////

void SVGA_PrintString(int x,int y,int color, char *string,int trans_flag)
{
int index;

for (index=0; string[index]!=0; index++)
     {

     SVGA_PrintChar(x+(index*CHAR_WIDTH),y,string[index],color,trans_flag);

     } /* end while */
}

//////////////////////// SVGA_Line ////////////////////////////
int Sgn(float A)
{
 if (A > 0) return(+1);
 if (A < 0) return(-1);
 if (A = 0) return(0);
}

void SVGA_Line(int x1, int y1, int x2, int y2, char color)
{
 int I, S, D1x, D1y, D2x, D2y, U, V, M, N;
 
 U = x2 - x1;
 V = y2 - y1;
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

 for (I=0;I<=M;I++)
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
//////////////////////// SVGA_HLine //////////////////////////
void SVGA_HLine(int x1, int x2, int y1, unsigned char color)
{
 unsigned long ofs;
 int bank;

 ofs = ((long)(y1)*BytesPerLine)+(long)(x1);
 bank = (ofs / BytesPerBank);
 ofs -= ((long)(bank)*BytesPerBank);

 if (CurrentBank != bank)
   VESA_SetPage((unsigned char)bank, 1);

 if (!MOUSE_SHOWING)
   {
    if ((long)(ofs + (x2-x1+1)) > BytesPerBank)
     {
      _fmemset((char far *)(video_buffer + ofs), color, (unsigned int)(BytesPerBank-ofs));
      VESA_SetPage(CurrentBank+1, 1);
      _fmemset((char far *)(video_buffer), color, (unsigned int)((ofs+x2-x1)-BytesPerBank)+1);
     }
    else
     _fmemset((char far *)(video_buffer + ofs), color, x2-x1+1);
   }
 else
   {
    if ((((MX>=x1)&&(MX<=x2))||((MX+16>=x1)&&(MX+16<=x2)))&&((y1>=MY)&&(y1<=MY+16)))
      for (ofs=x1;ofs<=x2;ofs++)
        SVGA_PutPixel((int)ofs, y1, color);
    else
     {
      if ((long)(ofs + (x2-x1+1)) > BytesPerBank)
       {
        _fmemset((char far *)(video_buffer + ofs), color, (unsigned int)(BytesPerBank-ofs));
        VESA_SetPage(CurrentBank+1, 1);
        _fmemset((char far *)(video_buffer), color, (unsigned int)((ofs+x2-x1)-BytesPerBank)+1);
       }
      else
       _fmemset((char far *)(video_buffer + ofs), color, x2-x1+1);
     }
   }
}
//////////////////////// SVGA_Rectangle //////////////////////
void SVGA_Rectangle(int x1, int y1, int x2, int y2, char color, int trans)
{
 int index;
 int m;

 if (trans)
//   m = MOUSE_SHOWING;
  // MOUSE_Hide();
   for (index=y1;index<y2;index++)
     SVGA_HLine(x1, x2, index, color);
  // if (m) MOUSE_Show();
 else
   {
    SVGA_HLine(x1, x2, y1, color);
    SVGA_HLine(x1, x2, y2, color);
    SVGA_Line(x1, y1, x1, y2, color);
    SVGA_Line(x2, y1, x2, y2, color);
   }
}
//////////////////////// SVGA_Cls ////////////////////////////

void SVGA_Cls(unsigned char bc)
{
 long totalbanks;
 char i;

 totalbanks = (((long)((long)(MaxX)*(long)(MaxY))) / BytesPerBank)+1;

 for (i=0;i<totalbanks;i++)
  {
   VESA_SetPage(i, 1);
   _asm
     {
      mov  ax, 0xA000
      mov  es, ax
      mov  cx, 32768
      mov  al, [bc]
      mov  ah, al
     
      rep  stosw
     }
   }
  _fmemset((unsigned char far *)&BackupSprite, bc, 256);
}

int  SVGA_LoadFont(char *fn, FONT_PTR f)
{
 FILE *fp;
 char Dummy[50];
 int index;

 if ((fp = fopen(fn, "rb"))==0)
   return(0);

 for (index=0;index<9;index++)
   Dummy[index] = getc(fp);

 if (Dummy[0] != 'M')
   return(0);

 for (index=0;index<4;index++)
   Dummy[index] = getc(fp);

 f->BitsPerPixel = (unsigned char)Dummy[0];
 f->Width = (unsigned char)Dummy[1];
 f->Height = (unsigned char)Dummy[2];
 f->MaxChars = (unsigned char)Dummy[3];

 if (!(f->CharSet = (unsigned char far *)malloc(f->Width*f->Height*f->MaxChars)))
   return(0);

 fread((unsigned char far *)f->CharSet, f->Width*f->Height*f->MaxChars, 1, fp);
 return(1);
}

int SVGA_LoadPCX(char *fn, image_ptr img)
{
 FILE *fp;
 Pointer temp;
 int i, j, k;
 long count;
 unsigned char c, numbytes, l;
 char *wbuf, *wptr;
 long Offset; 
 pcx_header h;

 SVGA_KillImage(img);
 img->in_use = 1;

 if ((fp = fopen(fn, "rb"))==0)
   return(0);

 temp = (Pointer)&h;
 for (i=0;i<128;i++)
   temp[i] = fgetc(fp);
 
 if (h.BitsPerPixel != 8) return(0);

 fseek(fp, -768L, SEEK_END);
 fread((Pointer)&img->palette, 768, 1, fp);
 
 for (i=0; i<256; i++)
  {
   img->palette.color[i].red   = img->palette.color[i].red   >> 2;
   img->palette.color[i].green = img->palette.color[i].green >> 2;
   img->palette.color[i].blue  = img->palette.color[i].blue  >> 2;
  }

 fseek(fp, sizeof(pcx_header), SEEK_SET);

 img->width = (h.x2 - h.x1) + 1;
 img->height = (h.y2 - h.y1) + 1;

 img->hXms = XMS_Alloc((int)((((long)img->width * img->height) / 1024)+1));
 wbuf = malloc(img->width+1);

 Offset=0;

 for (j=0; j<img->height; j++)
  {
   wptr = wbuf;
   for (k=0; k<img->width;)
    {
     c = fgetc(fp);
     if (c > 191)
      {
       l = c - 192;
       c = fgetc(fp);
       memset(wptr, c, l);
       wptr += l;
       k += l;
      }
     else
      {
       *wptr = c;
       wptr++;   k++;
      }
    }
   XMS_CopyToXMS(wbuf, img->hXms, (long)img->width, Offset);
   Offset += (long)(img->width);
 }

 _ffree(wbuf);
 return(1);
}      

void SVGA_SetFont(FONT_PTR f)
{
 CHAR_MAX = f->MaxChars;
 CHAR_HEIGHT = f->Height;
 CHAR_WIDTH = f->Width;
 CHAR_BITS_PER_PIXEL = f->BitsPerPixel;
 CharSet = f->CharSet;
}

void SVGA_ShowImage(image img, int x, int y, int rop)
{
 Pointer buf;
 Pointer nb;
 int sx, sy, yc;
 int w, h;
 int i, d;
 int bufseg, bufoff;
 long offset, p_off, bank, x_off, y_off;
 long work1, work2;
 long LastPage;
 int NoOfPages;
 int m, p, nw, nbo, nbs;

 w = img.width; h = img.height;

 if ((w==BytesPerLine)&&(!rop)&&(x==0)&&(y==0))
  { //1
   work1 = (long)w * h;
   NoOfPages = (int) (work1 / (DWORD)(BytesPerBank));

   work2 = (long)(NoOfPages * (DWORD)BytesPerBank);
   LastPage = (long) (work1 - work2);

   offset = 0;

   for (i = 0; i < NoOfPages; i++)
     { //2
      VESA_SetPage(i, 1);
      XMS_CopyFromXMS(MK_FP(0xA000, 0), img.hXms, BytesPerBank, offset);
      offset += BytesPerBank;
     } //2

   if (LastPage != 0)
    { //2
     VESA_SetPage(i, 1);
     XMS_CopyFromXMS(MK_FP(0xA000, 0), img.hXms, (long)LastPage, offset);
    } //2
  } //1
 else
  { //1
   if (x < 0)
     { //2
      if (x + w <= 0)
        return;

      x_off = (long)abs(x);
      sx = 0;
      w -= x_off;
     } //2
   else if (x >= MaxX)
     return;
   else
     { //2
      sx = x;
      x_off = (long)0;
     } //2

   if (y < 0)
     { //2
      if (y+h < 0)
        return;
      y_off = (long)abs(y)*(long)img.width;
      sy = 0;
      h -= abs(y);
     } //2
   else if (y >= MaxY)
     return;
   else
     { //2
      sy = y;
      y_off = (long)0;
     } //2

   if (sx + w > MaxX)
     w = MaxX - sx;

   if (sy + h > MaxY)
     h = MaxY - sy + 1;

   offset = y_off + x_off;
   yc = sy;

   if (w<1) return;


 if (MOUSE_SHOWING)
  if (
      (
       ((MOUSE_X    >= sx)&&(MOUSE_X    < sx+w))
     ||((MOUSE_X+16 >= sx)&&(MOUSE_X+16 < sx+w))
      )
     &&
      (
       ((MOUSE_Y    >= sy)&&(MOUSE_Y    < sy+h))
     ||((MOUSE_Y+16 >= sy)&&(MOUSE_Y+16 < sy+h))
      ) 
     )
   {
    _disable();
    m = 1;
    MOUSE_Hide();
   }
  else
   m = 0;
 else
  m = 0;

 if (rop)
  { //2
   buf = malloc(w);
   bufseg = FP_SEG(buf);
   bufoff = FP_OFF(buf);

   for (i=sy;i<sy+h;i++)
     { //3
      XMS_CopyFromXMS(buf, img.hXms, w, offset);
      p_off = ((long)(yc) * BytesPerLine) +(long)(sx);
      bank = (p_off / BytesPerBank);
      p_off = p_off-(bank*BytesPerBank);

      if (CurrentBank != bank)
        VESA_SetPage((unsigned int)bank, 1);


      if ((long)(p_off + w) < BytesPerBank)
        { //4
         _asm
          { //5
           cld
           push   ds
           push   si

           mov    ax, 0xA000
           mov    es, ax
           mov    di, word ptr [p_off]
           mov    ax, bufseg
           mov    ds, ax
           mov    si, bufoff
           mov    cx, [w]

LoopStart:
           lodsb
           cmp    al, 0
           jnz    DoPixel
       
           inc    di
           jmp    Finish
DoPixel:
           stosb
Finish:
           loop LoopStart

           pop    si
           pop    ds
          } //5
        } //4
      else
       {  //4
        for (d=0;d<w;d++)
          if (buf[d]) SVGA_PutPixel(sx+d, i, buf[d]);
        } //4
      offset += img.width;
      yc++;
     }  //3
   free(buf);
  } //2
 else
  { //2
   for (i=sy;i<sy+h;i++)
    { //3
     p_off = ((long)(i) * BytesPerLine) +(long)(sx);
     bank = (p_off / BytesPerBank);
     p_off = p_off-(bank*BytesPerBank);

     if (CurrentBank != bank)
       VESA_SetPage((int)bank, 1);

     buf = MK_FP(0xA000, p_off);

     if ((long)(p_off+w)<BytesPerBank)
      { //4
       XMS_CopyFromXMS(buf, img.hXms, w, offset);
       offset += img.width;
      } //4
     else
      { //4
       XMS_CopyFromXMS(buf, img.hXms, (long)(BytesPerBank-p_off), offset);
       VESA_SetPage(CurrentBank+1, 1);
       buf = MK_FP(0xA000, 0000);
       XMS_CopyFromXMS(buf, img.hXms, (long)(w-(BytesPerBank-p_off)), offset+(BytesPerBank-p_off));
       offset += (long)(img.width);
      } //4
    } //3
  } //2
 } //1
 if (m)
  {
   _enable();
   MOUSE_Show();
   m=0;
  }

} // ENDP

void SVGA_GetImage(image_ptr img, int x, int y, int width, int height)
{
 Pointer buf=NULL;
 int bufseg, bufoff;
 int sx, sy, w, h, yc;
 int i;
 unsigned long p_off, bank, o;

 SVGA_KillImage(img);
 img->in_use = 1;

 w = width;
 h = height;
 sx = x;
 sy = y;

 if (x+width>MaxX)
  w = (MaxX-x);

 img->width = w;
 img->height = h;
 img->hXms = XMS_Alloc(((int)(((long)w*h)/1024)+1));

 if ((buf = malloc(w))==NULL)
  {
   printf("GetImage Error\n");
  }

 bufseg = FP_SEG(buf);
 bufoff = FP_OFF(buf);
 yc = sy;
 o = 0;

 for (i=sy;i<sy+h;i++)
  {
   p_off = ((long)(yc) * BytesPerLine) +(long)(sx);
   bank = (p_off / BytesPerBank);
   p_off = p_off-(bank*BytesPerBank);

   if (CurrentBank != bank) VESA_SetPage((unsigned int) bank , 1);

   if ((long)(w + p_off)<BytesPerBank)
     _fmemcpy(buf, video_buffer+p_off, w);
   else
     {
      _fmemcpy(buf, video_buffer+p_off, (WORD)(BytesPerBank - p_off));
      VESA_SetPage(CurrentBank+1, 1);
      _fmemcpy(buf+BytesPerBank-w, video_buffer, (WORD)((p_off+w)-BytesPerBank));
     }
   XMS_CopyToXMS((Pointer)buf, img->hXms, w, o);
   o += w;
   yc++;
  }
 free(buf);
}

char *SVGA_ConvertString(char *str)
{
 int i;
 char string[1000];
 char c;

 sprintf(string, "");

 for (i=0;i<strlen(str);i++)
   {
    c = str[i];

    if ((c>='A')&&(c<='Z'))
      c -= 64;
    else if ((c>='a')&&(c<='z'))
      c -= 70;
    else if (c==' ')
      c = 53;
    else if (c==',')
      c = 54;
    else if (c=='.')
      c = 55;
    else if (c=='?')
      c = 56;
    else if (c=='!')
      c = 57;
    else if ((c>='0')&&(c<='9'))
      c += 10;

    sprintf(string, "%s%c", string, c);
   }
 return(string);
}

void SVGA_KillImage(image_ptr img)
{
 if (img->in_use) XMS_Free(img->hXms);
 img->in_use = 0;
}

int  SVGA_GetUserMode(void)
{
 int i;

 printf("\n(1) 320x200"
        "\n(2) 640x400"
        "\n(3) 640x480 (default)"
        "\n(4) 800x600"
        "\n(5) 1024x768"
        "\n(6) 1280x1024"
        "\nEnter the index of the 256 Color SVGA mode you would like to use: ");
 i=0;
 while (((i < '1')||(i > '7'))&&(i != 13))
  i = getch();

 printf("%c\n", i);

 switch (i)
  {
   case '1': return(VGA_320x200);
   case '2': return(SVGA_640x400);
   case '3': return(SVGA_640x480);
   case '4': return(SVGA_800x600);
   case '5': return(SVGA_1024x768);
   case '6': return(SVGA_1280x1024);
   case '7': return(0);
   case  13: return(SVGA_640x480);
  }
}

void SVGA_MergeImage(image d, int dx, int dy, int tw, int th, image s, int x, int y, int rop)
{
 Pointer  db;
 Pointer  sb;
 int      dbs, dbo;
 int      sbs, sbo;
 int      sx=0, sy=0, w=0, h=0, i=0;
 long     x_off=0, y_off=0, d_off=0, s_off=0;
 long     d_x_off=0, d_y_off=0;
 XMSBLOCK xb;

 if (x<0)
   {
    x_off = 0;
    d_x_off = (long)(abs(x));
   }
 else if (x >= s.width)
  return;
 else
   x_off = x;

 if (x_off + tw > s.width)
  w = s.width-x_off;
 else
  w = tw;

 if (y<0)
   {
    y_off = 0;
    d_y_off = (long)(abs(y));
   }
 else if (y >= s.height)
  return;
 else
   y_off = y;

 if (y_off + th > s.height)
   h = s.height-y_off;
 else
   h = th;

 s_off = (long)((y_off*s.width)+x_off);

 if (dx<0)
   {
    d_x_off = 0;
    x_off += (long)(abs(dx));
   }
 else if (dx>=d.width)
   return;
 else
   d_x_off += dx;

 if (d_x_off + w > d.width)
  w = d.width-d_x_off;

 if (dy<0)
   y_off += (long)(abs(y));
 else if (dy >= d.height)
  return;
 else
   d_y_off += dy;

 if (d_y_off + h > d.height)
   h = d.height-d_y_off;

 d_off = (long)((d_y_off*d.width)+d_x_off);

 if ((w<1)||(h<1)) return;

 if (rop)
  {
   db = malloc(w);
   dbs = FP_SEG(db);
   dbo = FP_OFF(db);
   sb = malloc(w);
   sbs = FP_SEG(sb);
   sbo = FP_OFF(sb);

   for (i=sy;i<sy+h;i++)
    {
     XMS_CopyFromXMS(sb, s.hXms, w, s_off);
     XMS_CopyFromXMS(db, d.hXms, w, d_off);
     _asm
      {
       push      ds
       push      si
       
       mov       ax, sbs
       mov       bx, dbs
       mov       ds, ax
       mov       es, bx
       mov       di, dbo
       mov       si, sbo
       mov       cx, w

cLoop:
       lodsb                         // Read the pixel
       cmp       al, 0               // Is the pixel "transparent"?
       jnz       cDoPixel            // No so draw it!!

       inc       di                  // Yes, point to next pixel and jump
       jmp       cFinish             // to the end of the loop!
cDoPixel:
       stosb                         // Write the pixel
cFinish:
       loop      cLoop               // Loop w times

       pop       si
       pop       ds
      }
     XMS_CopyToXMS(db, d.hXms, w, d_off);
     d_off += (long)(d.width);
     s_off += (long)(s.width);
    }  
   free(db);
   free(sb);
  }
 else
  {
   xb.length = w;
   xb.source_handle = s.hXms;
   xb.source_offset = s_off;
   xb.dest_handle = d.hXms;
   xb.dest_offset = d_off;
   for (i=0;i<h;i++)
    {
     if (w&1)
      {
       xb.length = 2;
       XMS_Copy((Pointer)&xb);
       xb.length = w-1;
       xb.source_offset++;
       xb.dest_offset++;
       XMS_Copy((Pointer)&xb);
       xb.source_offset--;
       xb.dest_offset--;
      }
     else
      XMS_Copy((Pointer)&xb);

     xb.dest_offset+=d.width;
     xb.source_offset+=s.width;
    }
  }
}

void SVGA_InitImage(image_ptr img, int w, int h)
{
 if (img->in_use) SVGA_KillImage(img);
 img->in_use = 1;
 img->width = w;
 img->height = h;
 img->hXms = XMS_Alloc(((int)(((long)w*h)/1024)+1));
}

//////////////////////// Palette Routines /////////////////////
void SVGA_SetPalette(int index, RGB_color_ptr color)
{
_outp(PALETTE_MASK,0xff);
_outp(PALETTE_REGISTER_WR, index);
_outp(PALETTE_DATA, color->red);
_outp(PALETTE_DATA, color->green);
_outp(PALETTE_DATA, color->blue);
} // end Set_Palette_Color

void SVGA_SetAllPalette(Palette_ptr pal)
{
 short int i;

 outp(0x3c6, 0xff);
 for (i=0; i<256; i++)
  {
   outp(0x3c8, i);
   outp(0x3c9, pal->color[i].red);
   outp(0x3c9, pal->color[i].green);
   outp(0x3c9, pal->color[i].blue);
  }
}

void SVGA_GetPalette(int index, RGB_color_ptr color)
{
 _outp(PALETTE_MASK,0xff);
 _outp(PALETTE_REGISTER_RD, index);
 color->red = _inp(PALETTE_DATA);
 color->green = _inp(PALETTE_DATA);
 color->blue = _inp(PALETTE_DATA);
} 

void SVGA_FadeIn(int d, Palette_ptr p)
{
 RGB_color RGB;
 char str[500];
 int pc, index;
 unsigned int far *clock = (unsigned int far *)0x0000046CL;
 unsigned int now;

 RGB.red = RGB.green = RGB.blue = 0;

 for (index=0;index<256;index++)
  SVGA_SetPalette(index, (RGB_color_ptr)&RGB);
  
 for (index=0;index<64;index++)
  {
   now = *clock;
   for (pc=0;pc<256;pc++)
    {
     SVGA_GetPalette(pc, &RGB);
     if (RGB.red   < p->color[pc].red)   RGB.red++;
     if (RGB.green < p->color[pc].green) RGB.green++;
     if (RGB.blue  < p->color[pc].blue)  RGB.blue++;
     SVGA_SetPalette(pc, &RGB);
    }
   now = *clock;
   while(abs(*clock - now) < d) {}
  }
}

void SVGA_FadeOut(int d)
{
 RGB_color c;
 int i, j;
 unsigned int far *clock = (unsigned int far *)0x0000046CL;
 unsigned int now;


 for (j=0;j<64;j++)
  {
   now = *clock;
   for (i=0;i<256;i++)
    {
     SVGA_GetPalette(i, (RGB_color_ptr)&c);
     if (c.red   >= 1) c.red--;
     if (c.green >= 1) c.green--;
     if (c.blue  >= 1) c.blue--;
     SVGA_SetPalette(i, (RGB_color_ptr)&c);
    }
   while(abs(*clock - now) < d) {}
  }
}

void SVGA_CyclePalette(Palette_ptr p, int s, int f, int up)
{
 RGB_color c;
 int i;
 char str[500];

 if (up)
  {
   c = p->color[s];

   c.red = 25;

   for (i=s;i<f;i++)
     p->color[i] = p->color[i+1];

   p->color[f] = c;
  }
 else
  {
   c = p->color[f];

   for (i=f;i>s+1;i--)
    p->color[i] = p->color[i-1];

   p->color[s] = c;
  }
 SVGA_SetAllPalette(p);
}

int SVGA_LoadPalette(char *filename, Palette_ptr the_palette)
{
 int index; // used for looping
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
 }

 fclose(fp);
 return(1);
}

int SVGA_SavePalette(char *filename, Palette_ptr the_palette)
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



