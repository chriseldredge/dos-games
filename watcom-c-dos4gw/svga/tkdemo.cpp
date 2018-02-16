///////////////////////////////////////////////////////////////////////////////////////////
// FILE: TKDEMO.C                                                                        //
// PURPOSE: To demonstrate the abilities of the ToolKit, ranging from VESA graphics to   //
//         mouse and keyboard input to PIT interface (and then some).                    //
// AUTHOR:    Chris Eldredge <wantaco@erols.com>                                         //
//            Three Thorburn Place                                                       //
//            Gaithersburg MD, 20878                                                     //
//            (301) 424-0253                                                             //
// CO-AUTHOR: Jason Martin <jmartin@binx.mbhs.edu>                                       //
//            Addx not available                                                         //
//            (205) 835-6519                                                             //
///////////////////////////////////////////////////////////////////////////////////////////
  
#include "toolkit.h"                  /* Include this header, and you most likely will not
                                         have to include any other standard headers! */

MENU DEMO_Menu(0,0,0,0,0); 
          
int  DEMO_Init(void);                   
void DEMO_Run(void);
void DEMO_Done(void);
void DEMO_CreatePalette(void);          
void DEMO_DrawBackGround(void);
void DEMO_CyclePalette(void);

void DEMO_ChangeMode(void);
void DEMO_VESAInfo(void);
void DEMO_Mandelbrot(void);
void DEMO_Sound(void);

PALETTE  DEMO_Pal;
FONT     DEMO_Font8pt, DEMO_FontTNR;
BINT16   CurDemo;
  
void main(void)
{
 if (!DEMO_Init()) exit(-1);

 DEMO_Run();

 DEMO_Done();
}

int DEMO_Init(void)
{
 KERNAL_Init();
 
 if (!SVGA_Init())
  {
   printf("An error occured while trying to initialize SVGA graphics libraries.\n"
          " o VESA compatable video card not found. You need a Super VGA graphics card to\n"
          "   use the libraries.  This video card must comply with the VESA standards for\n"
          "   Super VGA graphics cards.  Make sure you have loaded your VESA driver if you\n"
          "   need one. \n");
   return(0);
  }

 if (SVGA_SetupMode(SVGA_640x480))
  {
   printf("An error occurred while trying to setup SVGA video mode 640x480x256!!\n"
          "Error #2: Grahpics mode not supported. Sorry! This really shouldn't have happened!\n");
   return(0);
  }

 DEMO_CreatePalette();
 
 DEMO_DrawBackGround();

 SVGA_LoadFont("8pt.MFF", &DEMO_Font8pt);
 SVGA_LoadFont("TNR.MFF", &DEMO_FontTNR); 
 SVGA_SetFont(&DEMO_FontTNR);

 DEMO_Menu.init(0,0,1,1,10, 
               "Toolkit Demonstration",
               "Change Video Mode",
               "VESA Information Viewer",
               "Pixel Plotting with the Mandelbrot Set",
               "Polygon Drawing, Scaling, Rotating and Morphing",
               "Image Animation",
               "PCX File Viewer",
               "Sound Loading and Mixing",
               "MIDI File Player",
               "Change Font",
               "Exit");

 DEMO_Menu.show();
 
 SVGA_FadeIn(40, (PALETTE_PTR)&DEMO_Pal);

 MOUSE_Init();
 MOUSE_Show();

 KERNAL_Start();
 KERNAL_SetSpeed(50,0);
 KERNAL_AddTask((void near *(*)(void))DEMO_CyclePalette);

 KEYBOARD_Init();
 return(1);
}

void DEMO_Run(void)
{
 CurDemo=0;
 while (CurDemo!=9)
  {
   CurDemo=DEMO_Menu.go();
   switch(CurDemo)
    {
     case 0: DEMO_ChangeMode(); break;
     case 2: DEMO_Mandelbrot(); break;
     case 6: DEMO_Sound();      break;
    }
  }
}

void DEMO_Done(void)
{
 MOUSE_Hide();
 MOUSE_Done();
 KEYBOARD_Done();

 DEMO_Menu.hide();
 KERNAL_Stop();
 delay(500);

 SVGA_FadeOut(40);
 VESA_SetMode(TEXT_MODE);
}

void DEMO_CreatePalette(void)
{
 RGB r;
 int i;

 SVGA_LoadPalette("256COLOR.PAL", (PALETTE_PTR)&DEMO_Pal);

 for (i=4;i<32;i++)
  SVGA_GetPalette(i, (RGB_PTR)&DEMO_Pal.color[i]);

 SVGA_GetPalette(0, (RGB_PTR)&DEMO_Pal.color[0]);
 
 for (i=32;i<96;i++)
  {
   r.red = i-32;
   r.green = i-32;
   r.blue = i-32;
   _fmemcpy((Pointer)&DEMO_Pal+i*3, (Pointer)&r, 3);
  }
 for (i=96;i<159;i++)
  {
   r.red = 159-i;
   r.green = 159-i;
   r.blue = 159-i;
   _fmemcpy((Pointer)&DEMO_Pal+i*3, (Pointer)&r, 3);
  }
 for (i=159;i<159+31;i++)
  {
   DEMO_Pal.color[i].red   = (i-159)*2;
   DEMO_Pal.color[i].green = 0;
   DEMO_Pal.color[i].blue  = 0;
  }
 for (i=0;i<32;i++)
  {
   DEMO_Pal.color[i+190].red   = 63-(i*2);
   DEMO_Pal.color[i+190].green = 0;
   DEMO_Pal.color[i+190].blue  = 0;
  }

 r.red = r.green = r.blue = 0;

 for (i=0;i<=255;i++)
  SVGA_SetPalette(i, (RGB_PTR)&r);
}

void DEMO_DrawBackGround(void)
{
 int i;

 for (i=0;i<=MaxY;i++)
   SVGA_HLine(0, MaxX, i, (unsigned char)(i%127+32));

}

void DEMO_CyclePalette(void)   
{
 static char i;
 
 SVGA_CyclePalette((PALETTE_PTR)&DEMO_Pal, 32, 158, 1);

 if (i++>=3) i=0;
 if (i==0)
  SVGA_CyclePalette((PALETTE_PTR)&DEMO_Pal, 160, 159+64, 1);
}
/*
VBINT SVGA_DrawFileList(VBINT)
end
 BINT16 x, y, w, h, c=0;

 if (CurrentFont->romset) c=1;
 CurrentFont->romset=1;
 
 w = 50*8+19;
 h = 21*8+22;
 x = MaxX/2-w/2;
 y = MaxY/2-h/2;

 SVGA_DrawButton(x,y,w,h,1,"");

 SVGA_DrawButton(x+3, y+3, w-6, 14, 1, "");
 SVGA_Rectangle(x+5, y+5, x+w-5, y+15, 255, 1);
 SVGA_Rectangle(x+5, y+5, x+w-5, y+15, 249, 0);

 SVGA_DrawButton(x+3, y+20, 13*8+15, 20*8+6, 1, "");
 SVGA_Rectangle(x+5, y+22, x+13*8, y+22+20*8+2, 255, 1);
 SVGA_Rectangle(x+5, y+22, x+13*8, y+22+20*8+2, 249, 0);
 SVGA_Rectangle(x+5, y+22, x+13*8+16, y+22+20*8+2, 249, 0);
   
 SVGA_DrawButton(x+1+13*8, y+23, 14, 14, 1, "");
 SVGA_PrintChar(x+5+13*8, y+27, 0, 24, 0);
 
 SVGA_DrawButton(x+1+13*8, y+22+20*8-13, 14, 14, 1, "");
 SVGA_PrintChar(x+5+13*8, y+22+20*8-9, 0, 25, 0); 
 
 SVGA_DrawButton(x+15*8+20, y+20, 13*8+15, 20*8+6, 1, "");
 SVGA_Rectangle(x+15*8+22, y+22, x+26*8+20, y+22+20*8+2, 255, 1);
 SVGA_Rectangle(x+15*8+22, y+22, x+16*8+20, y+22+20*8+2, 249, 0);
 SVGA_Rectangle(x+15*8+22, y+22, x+16*8+36, y+22+20*8+2, 249, 0);
   
 SVGA_DrawButton(x+21+26*8, y+23, 14, 14, 1, "");
 SVGA_PrintChar(x+5+13*8, y+27, 0, 24, 0);
 
 SVGA_DrawButton(x+1+13*8, y+22+20*8-13, 14, 14, 1, "");
 SVGA_PrintChar(x+5+13*8, y+22+20*8-9, 0, 25, 0); 

 then (!c) CurrentFont->romset=0; 
begin

BINT8 *SVGA_HandleFileList(BINT8 *dir, BINT8 *file);
*/

// Change Video Mode Junx ////////////////////////////////////////////////////////////////////

void DEMO_ChangeMode(void)
{
 VESAINFO         vesa;
 VESAMODEINFO     mode;
 STR              str[500];
 NOBINT16         *mode_num, yof=7, tm=1, i, csm, bs=0xFF;
 BINT16           mby[10];
 NOBINT16         mbm[10]; 
 static NOBINT16  curmode=SVGA_640x480;

 VESA_ReturnInfo(&vesa);                 // Get regular VESA information.

 mode_num = vesa.video_mode;

 while (*mode_num != 0xFFFF)             // Scan for all the supported 8bit modes.
  {
   mode_num++;
   VESA_ReturnModeInfo(*mode_num, &mode);  
   if (mode.bits_per_pixel==8)
    tm++;
  }

 SVGA_SetFont(&DEMO_Font8pt);

 BUTTON           ok(0,0,0,0,"Ok"),
                  cancel(0,0,0,0,"Cancel"),
                  b(0,0,0,0,"");
 BUTTONBAR        bb(2,&ok, &cancel);
 FORM             f(0,0,TW("Mode Number   Width   Height")+40,
                        (TH+3)*(tm+1)+82, 1, 1, 1,"Change Video Mode", &bb);

 SVGA_SetFont(&DEMO_FontTNR);
 f.show();

 SVGA_SetFont(&DEMO_Font8pt);

 SVGA_PrintString(f.fx+15, f.fy+7, 0, "Mode Number   Width   Height", 0);
 
 mode_num = vesa.video_mode;
 i=0;
 while (*mode_num != 0xFFFF)
  {
   VESA_ReturnModeInfo(*mode_num, &mode);
   if (mode.bits_per_pixel==8)
    {
     yof+=TH+3;

     b.reset(f.fx+20, f.fy+yof, 7, 7, "");
     b.show((curmode!=*mode_num));
     
     sprintf(str, "%x", *mode_num);
     SVGA_PrintString(f.fx+45, f.fy+yof, 0, str, 0);
     sprintf(str, "%i", mode.x_res);
     SVGA_PrintString(f.fx+115, f.fy+yof, 0, str, 0);
     sprintf(str, "%i", mode.y_res);
     SVGA_PrintString(f.fx+160, f.fy+yof, 0, str, 0);
     mby[i] = f.fy+yof;
     mbm[i] = *mode_num;
     if (curmode==*mode_num) csm = i;
     i++;
    }
   mode_num++;
  }

 SVGA_SetFont(&DEMO_FontTNR);
                             
 bs = NONE;

 while (bs==NONE)
  {
   bs = bb.check();

   if (KeyFlags[KEY_UP])
    {
    }
     
   for (i=0;i<tm;i++)
    {
     if ((MX>f.fx+20)&&(MY>mby[i])&&(MX<f.fx+27)&&(MY<mby[i]+7)&&(ML))
      {
       b.reset(f.fx+20, mby[csm], 7, 7, "");
       b.show(1);
       csm = i;
       b.reset(f.fx+20, mby[csm], 7, 7, "");
       b.show(0);
       while (ML) {}
      }
    }
  }

 f.hide();
 if (bs==1)
  {
   return;
  }

 curmode = mbm[csm];

 MOUSE_Done();

 VESA_SetMode(TEXT_MODE);
 printf("Setting mode: %xh\n", mbm[csm]);
 delay(3000);
 SVGA_SetupMode(curmode);
 delay(500);

 MOUSE_Init();
 MOUSE_Show();

 DEMO_DrawBackGround();
 SVGA_SetFont(&DEMO_FontTNR);

 DEMO_Menu.init(0,0,1,1,10,
               "Toolkit Demonstration",
               "Change Video Mode",
               "VESA Information Viewer",
               "Pixel Plotting with the Mandelbrot Set",
               "Polygon Drawing, Scaling, Rotating and Morphing",
               "Image Animation",
               "PCX File Viewer",
               "Sound Loading and Mixing",
               "MIDI File Player",
               "Change Font",
               "Exit");
 DEMO_Menu.show();
}
 
// Mandelbrot Junx ///////////////////////////////////////////////////////////////////////////

#define mu(a,b) mul(a,b,16)
#define MF 65536.0
#define M1 65536

void NotRect(int x1, int y1, int x2, int y2);

void DEMO_Mandelbrot(void)
{
 BINT32         ex1, wy1, ex2, wy2, dx, dy, cx, cy, x, y, xx, yy, xs, ys;
 BINT16         px, py, mx1, my1, mx2, my2, MOX, MOY;
 BINT16         num_iters, num_cols, i, r=0, bs;
 STR            str[500];
 BUTTON         start(0,0,0,0,"Start"),
                restart(0,0,0,0,"Restart"),
                stop(0,0,0,0,"Stop"),
                zoom(0,0,0,0,"Zoom Out"),
                save(0,0,0,0,"Save"),
                quit(0,0,0,0,"Quit");
 BUTTONBAR      bb(6,&start, &restart, &stop, &zoom, &save, &quit);
 FORM           f(0,0,MaxX-150,MaxY-100,1,1,1,"Pixel Plotting with the Mandelbrot Set", &bb);

 msgbox("Mandelbrot Set", "This is a graphics demonstration which utilizes SVGA_PutPixel to draw the formula of the MandelBrot set. The basic formula to compute the Mandelbrot set is: \n z = z*z+c \n After several un-simplifications, this becomes: \n x = x*x-y*y and \n y = 2*x*y + cy \n Ok, just watch the pretty picture. You can use the left mouse button to select a region to zoom in on, the press start!\nEnjoy...", OK, &DEMO_FontTNR, &DEMO_Font8pt);
 f.show();

 ex1 = (long)(-2.25*MF);
 wy1 = (long)(-1.5*MF);
 ex2 = (long)(.75*MF);
 wy2 = (long)(1.5*MF);
 num_iters = num_cols = 64;

 bs = NONE;
 while ((!KeyFlags[KEY_ESC])&&(bs!=5))
  {
   dx = (ex2 - ex1) / f.fw;
   dy = (wy2 - wy1) / f.fh;

   for (px=0;(px<f.fw);px++)
    {
     for (py=0;(py<f.fh);py++)
      {
       cx = ex1 + px * dx;
       cy = wy1 + py * dy;
       x = y = 0;
       xs = ys = 0;

       for (i=0; (i < num_iters)&&(xs+ys < (M1*4)); i++)
        {
         xs = mu(x,x);
         ys = mu(y,y);
         xx = xs - ys + cx;
         yy = (mu(x,y)<<1)+cy;
         x = xx; y = yy;
        }
       if (i==num_iters)
        i = 0;
       else
        i = i % num_cols;
    
       SVGA_PutPixel(f.fx+px, f.fy+py, 159+i);
       bs = bb.check();
       if ((bs==2)||(bs==5)||(KeyFlags[KEY_ESC])) break;
      }
     if ((bs==2)||(bs==5)||(KeyFlags[KEY_ESC])) break;
    }
   r = 0;

   sprintf(str, "Range: %f,%f to %f,%f", (double)(ex1 / MF), (double)(wy1 / MF), (double)(ex2 / MF),(double)(wy2 / MF));

   if (CurrentFont->romset)
    SVGA_PrintString(f.fx,f.fy,15, str, 0);
   else
    {
     CurrentFont->romset = 1;
     SVGA_PrintString(f.fx,f.fy,15, str, 0);
     CurrentFont->romset = 0;
    }
    
   while ((!KeyFlags[KEY_ESC])&&(bs!=5))
    {
     bs = bb.check();
     if (bs==0)
      if (r)
       {
        ex2 = ex1 + (mx2-f.fx) * dx;
        wy2 = wy1 + (my2-f.fy) * dy;
        ex1 += (mx1-f.fx) * dx;
        wy1 += (my1-f.fy) * dy;
        break;
       }
      else
       {
        printf("\7\n");
       }

     if (bs==1) break;
     if (bs==3)
      {
       ex1 = (long)(-2.25*MF);
       wy1 = (long)(-1.5*MF);
       ex2 = (long)(.75*MF);
       wy2 = (long)(1.5*MF);
       break;
      }
     if (bs==4)
      {
      }      
 
     if ((ML)&&(MX>f.fx)&&(MX<f.fx+f.fw)
             &&(MY>f.fy)&&(MY<f.fy+f.fh))
      
      {
       if (r) NotRect(mx1, my1, MOX, MOY);
       MOX = MX, MOY = MY;
       mx1 = MX; my1 = MY;
       while (ML)
        {
         _disable();
         if ((MOX != MX)||(MOY != MY))
          if ((MX>f.fx)&&(MX<f.fx+f.fw)
             &&(MY>f.fy)&&(MY<f.fy+f.fh))
            {
             do_enable = 0;
             NotRect(mx1, my1, MOX, MOY);
             NotRect(mx1, my1, MX, MY);
             MOX = MX, MOY = MY;
             do_enable = 1;
            }
         _enable();
        }
       mx2 = MX; my2 = MY;
       r = 1;
      }
    }
  }

 f.hide();
}

void NotHline(int x1, int x2, int y)
{
 int i;

 for (i=x1;i<x2;i++)
  SVGA_PutPixel(i, y, (SVGA_GetPixel(i, y))^255);
}

void NotVline(int y1, int y2, int x)
{
 int i;

 for (i=y1;i<y2;i++)
  SVGA_PutPixel(x, i, ((SVGA_GetPixel(x, i))^255));
}

void NotRect(int x1, int y1, int x2, int y2)
{
 if (x1<=x2)
  {
   NotHline(x1, x2, y1);
   NotHline(x1, x2, y2);
  }
 else
  {
   NotHline(x2, x1, y1);
   NotHline(x2, x1, y2);
  }

 if (y1<=y2)
  {
   NotVline(y1, y2, x1);
   NotVline(y1, y2, x2);
  }
 else
  {
   NotVline(y2, y1, x1);
   NotVline(y2, y1, x2);
  }
}

void DEMO_Sound(void)
{
 BINT16    i;
 BUTTON    bplay1(0,0,0,0,"Play"),
           bplay2(0,0,0,0,"Play"),
           bplay3(0,0,0,0,"Play"),
           bplay4(0,0,0,0,"Play"),
           bload1(0,0,0,0,"Load"),
           bload2(0,0,0,0,"Load"),
           bload3(0,0,0,0,"Load"),
           bload4(0,0,0,0,"Load"),
           quit(0,0,0,0,"Quit");
 BUTTONBAR bb(1, &quit);
 FORM      f(0,0,300,200,1,1,1,"Sound Loading and Mixing",&bb);
  
 f.show();

 bload1.reset(f.fx+20,f.fy + (TH+4)*(1), TW("Load") + 2, TH+2, "Load");
 bload2.reset(f.fx+20,f.fy + (TH+4)*(2), TW("Load") + 2, TH+2, "Load");
 bload3.reset(f.fx+20,f.fy + (TH+4)*(3), TW("Load") + 2, TH+2, "Load");
 bload4.reset(f.fx+20,f.fy + (TH+4)*(4), TW("Load") + 2, TH+2, "Load");
     
 bplay1.reset(f.fx+20+TW("Load")+7,f.fy + (TH+4)*(1), TW("Play") + 2, TH+2, "Play");
 bplay2.reset(f.fx+20+TW("Load")+7,f.fy + (TH+4)*(2), TW("Play") + 2, TH+2, "Play");
 bplay3.reset(f.fx+20+TW("Load")+7,f.fy + (TH+4)*(3), TW("Play") + 2, TH+2, "Play");
 bplay4.reset(f.fx+20+TW("Load")+7,f.fy + (TH+4)*(4), TW("Play") + 2, TH+2, "Play");

 bb.add(8, &bload1, &bload2, &bload3, &bload4, &bplay1,  &bplay2, &bplay3, &bplay4);
 bb.show();

 for (i=0;i<4;i++)
  {
   SVGA_PrintChar(f.fx+5, f.fy+(TH+4)*(i+1)+1, 0, MAP['1'+i], 0);
   SVGA_PrintString(f.fx+25+TW("Play"), f.fy+(TH+4)*(i+1)+1, 0, "Not in use", 0);
  }

 while (bb.check()!=0) {}

 f.hide();
}
