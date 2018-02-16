#include "toolkit.h"

#define IOError(s) {printf("File not found: %s", s); exit(-1);}
#define HWError(s) {printf("%s",s); exit(-2);}

void go(void);
void CreatePalette(void);
void DrawBackGround(void);
void CyclePalette(void);
BINT16 copyfile(STR *source, STR *dest, STR *srcn, STR *destn);

BUTTON          bok(0,0,0,0,"Ok");
BUTTON          bcancel(0,0,0,0,"Cancel");
BUTTONBAR       bb(2, &bok, &bcancel);
FORM            f(0,0,400,400,1,1,1,"Meteor Installation", &bb);

PALETTE pal;
BINT32  status, filelen, filepos;
FONT    fnt;

void main()
{
 if (!MOUSE_Reset()) HWError("Error 1: No mouse driver. Type MOUSE and try again.\n");
 if (!SVGA_Init())   HWError("Error 2: No VESA driver. Load your VESA program and try again.\n");

 SVGA_LoadFont("8PT.MFF", &fnt);

 SVGA_SetupMode(SVGA_640x480);
 CreatePalette();
 SVGA_SetFont(&fnt);
 DrawBackGround();
 KEYBOARD_Init();
 MOUSE_Init();
 MOUSE_Show();
 KERNAL_Init();
 KERNAL_AddTask((void near *(*)(void))CyclePalette);
 KERNAL_Start();

 go();

 KERNAL_Stop();
 MOUSE_Done();
 KEYBOARD_Done();
 VESA_SetMode(TEXT_MODE);
}

void CreatePalette(void)
{
 RGB r;
 int i;

 SVGA_LoadPalette("256COLOR.PAL", (PALETTE_PTR)&pal);

 for (i=4;i<32;i++)
  SVGA_GetPalette(i, (RGB_PTR)&pal.color[i]);

 SVGA_GetPalette(0, (RGB_PTR)&pal.color[0]);
 
 for (i=32;i<96;i++)
  {
   r.red = i-32;
   r.green = i-32;
   r.blue = i-32;
   _fmemcpy((Pointer)&pal+i*3, (Pointer)&r, 3);
  }
 for (i=96;i<159;i++)
  {
   r.red = 159-i;
   r.green = 159-i;
   r.blue = 159-i;
   _fmemcpy((Pointer)&pal+i*3, (Pointer)&r, 3);
  }
 for (i=159;i<159+31;i++)
  {
   pal.color[i].red   = (i-159)*2;
   pal.color[i].green = 0;
   pal.color[i].blue  = 0;
  }
 for (i=0;i<32;i++)
  {
   pal.color[i+190].red   = 63-(i*2);
   pal.color[i+190].green = 0;
   pal.color[i+190].blue  = 0;
  }

 r.red = r.green = r.blue = 0;

 SVGA_SetAllPalette(&pal);
}

void DrawBackGround(void)
{
 int i;

 for (i=0;i<=MaxY;i++)
   SVGA_HLine(0, MaxX, i, (unsigned char)(i%127+32));

}

void CyclePalette(void)   
{
 SVGA_CyclePalette((PALETTE_PTR)&pal, 32, 158, 1);
}

void go(void)
{
 f.init(0,0,430,250,1,1,1,"METEOR Installation", &bb);
 
 TEXTBOX         tsource(f.fx+5, f.fy+50, 0, "A:\\", 50);
 TEXTBOX         tdest(f.fx+5, f.fy+100, 0, "C:\\METEOR", 50);
 BINT16          x;

 f.show();

 tsource.show();
 tdest.show();

 SVGA_PrintString(f.fx+5, f.fy+35, 250, "Copy files from source:", 0);
 SVGA_PrintString(f.fx+5, f.fy+85, 250, "Copy files to destination:", 0);

 SVGA_Rectangle(f.fx+5, f.fy+150, f.fx+5+f.fw-10, f.fy+150+50, 255, 1);
 SVGA_Rectangle(f.fx+5, f.fy+150, f.fx+5+f.fw-10, f.fy+150+50, 250, 0);

 forever
  {
   if (tsource.checkfocus()) tsource.go();
   if (tdest.checkfocus()) tdest.go();
   x=bb.check();
   if (x==0) break;
   if (x==1) break;
  }

 if (x==0)
  status=copyfile(tsource.gettext(), tdest.gettext(), "METEOR.DAT", "METEOR.ZIP");
  
 if (status==0)
  
 f.hide();
}

BINT16 copyfile(STR *source, STR *dest, STR *srcn, STR *destn)
{
 STR   d[500], s[500];
 FILE  *fr, *fw;
 BINT8 c;
 
 strcpy(d, dest);
 if (chdir(d))
  mkdir(d);
 if (chdir(d))
  return(3);

 _dos_setdrive(toupper(d[0])-'A'+1, NULL);

 if (d[strlen(d)-1] != '\\') strcat(d, "\\");

 strcpy(s, source);
 if (s[strlen(s)-1] != '\\') strcat(s, "\\");

 strcat(d, destn);
 strcat(s, srcn);

 if ((fr=fopen(s, "rb"))==NULL) return(1);
 if ((fw=fopen(d, "wb"))==NULL) return(2);

 filepos=0;
 fseek(fr, 0, SEEK_END);
 fgetpos(fr, &filelen);
 fseek(fr, 0, SEEK_SET);
 
 while (filepos<=filelen)
  {
   c=fgetc(fr);
   fputc(c, fw);
   filepos++;
   if ((filepos%100)==0)
    SVGA_Rectangle(f.fx+6, f.fy+151, f.fx+5+(filepos*(f.fw-12)/filelen), f.fy+150+49, LightBlue, 1);
  }
   
 fclose(fr);
 fclose(fw);

 return(0);
}
