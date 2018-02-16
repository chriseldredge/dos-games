// to do: ask really want to quit really want new game
//        ask for use of polka
//        add memory checker
//        fix ctrl direction bug

#include "toolkit.h"

#define IOError(s) {printf("File not found: %s", s); exit(-1);}
#define HWError(s) {printf("%s",s); exit(-2);}

#define LFTMRGN         200
#define RTMRGN          400
#define STEP            30
#define EXPXOFFSET      44
#define EXPY            88
#define BIOSCLOCKADDX   0x046C
#define FYOFF           160
#define NL              20

void intro();
void controls();
void howto();
void mainthing();
void GetSoundSettings(void);
void getgamesettings(void);
void timerproc();
void processbackdrop(IMAGE_PTR to, IMAGE_PTR from, BINT16 xoff, BINT16 yoff, BINT16 blt);
BINT16 loadscores();
void savescores();
void showscores();
void drawhelm();
void hidehelm();
void umbup();
void umbdn();
void getname();
void helpfulhints();

BUTTON          bokabout(0,0,0,0,"Ok",KEY_ENTER);
BUTTONBAR       bbabout(1, &bokabout);
FORM            fabout(0,0,200,400,1,1,1,"Meteor", &bbabout);
IMAGE           imgabout;
IMAGE           imghowto;
IMAGE           imgcontrols;
IMAGE           imglose;

BINT32  bx;             // Background X
BINT32  fx;             // Foreground X
BINT32  sx;             // Sprite X
BINT32  wx;             // Work X
BINT16  usesound=0, speaking=0, skid=0, helmet, useumbr;
BINT16  stpx;           // Step X
BINT16  cf, playing=0, autodeath=0;    // Current Frame
BINT16  metx, mety, mstp = 11, md=150;
BINT16  flag, saveimage=0;
BINT16  *time = (BINT16 *)BIOSCLOCKADDX;
BINT16  stime, etime, fdts, mins, secs, et, mt, count=20, sps=1;   // Start time, End time, Frames Drawn To Screen
BINT16  scores[26], numscores, curscore, musicid, rockid, i;
STR     names[26][NL];
IMAGE   background;
IMAGE   foreground;
IMAGE   andrew[12];
IMAGE   meteor[2];
IMAGE   explosion[12];
IMAGE   helm[7];
IMAGE   umbr[6];
IMAGE   clean[2];
IMAGE   work;
IMAGE   work2, work3, work4, menubar, explosionback, explosionfront;
IMAGE   save;
PALETTE palette;
FONT    font, digits;
SOUND   music, sndrock, sndexplode;
STR     str[500], oldstr[500];

void main()
{
 printf("Loading\n");
 BytesPerPixel = 1;
 if (!SVGA_LoadPCX("BACKGRND.PCX", &background)) IOError("BACKGRND.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("FOREGRND.PCX", &foreground)) IOError("FOREGRND.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("ANDREW1.PCX", &andrew[0])) IOError("ANDREW1.PCX");    printf(".\n");
 if (!SVGA_LoadPCX("ANDREW2.PCX", &andrew[1])) IOError("ANDREW2.PCX");    printf(".\n");
 if (!SVGA_LoadPCX("ANDREW3.PCX", &andrew[2])) IOError("ANDREW3.PCX");    printf(".\n");
 if (!SVGA_LoadPCX("ANDREW1.PCX", &andrew[3])) IOError("ANDREW1.PCX");    printf(".\n");
 if (!SVGA_LoadPCX("ANDREW4.PCX", &andrew[4])) IOError("ANDREW4.PCX");    printf(".\n");
 if (!SVGA_LoadPCX("ANDREW5.PCX", &andrew[5])) IOError("ANDREW5.PCX");    printf(".\n");
 if (!SVGA_LoadPCX("ANDREW6.PCX", &andrew[6])) IOError("ANDREW6.PCX");    printf(".\n");
 if (!SVGA_LoadPCX("ANDREW7.PCX", &andrew[7])) IOError("ANDREW7.PCX");    printf(".\n");
 if (!SVGA_LoadPCX("ANDREW8.PCX", &andrew[8])) IOError("ANDREW8.PCX");    printf(".\n");
 if (!SVGA_LoadPCX("ANDREW6.PCX", &andrew[9])) IOError("ANDREW6.PCX");    printf(".\n");
 if (!SVGA_LoadPCX("ANDREW9.PCX", &andrew[10])) IOError("ANDREW9.PCX");   printf(".\n");
 if (!SVGA_LoadPCX("ANDREW10.PCX", &andrew[11])) IOError("ANDREW10.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("METEOR1.PCX", &meteor[0])) IOError("METEOR1.PCX");    printf(".\n");
 if (!SVGA_LoadPCX("METEOR2.PCX", &meteor[1])) IOError("METEOR2.PCX");    printf(".\n");
 if (!SVGA_LoadPCX("EXPLOD1.PCX", &explosion[0])) IOError("EXPLOD1.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("EXPLOD2.PCX", &explosion[1])) IOError("EXPLOD2.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("EXPLOD3.PCX", &explosion[2])) IOError("EXPLOD3.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("EXPLOD4.PCX", &explosion[3])) IOError("EXPLOD4.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("EXPLOD5.PCX", &explosion[4])) IOError("EXPLOD5.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("EXPLOD6.PCX", &explosion[5])) IOError("EXPLOD6.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("EXPLOD7.PCX", &explosion[6])) IOError("EXPLOD7.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("EXPLOD8.PCX", &explosion[7])) IOError("EXPLOD8.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("EXPLOD9.PCX", &explosion[8])) IOError("EXPLOD9.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("EXPLOD10.PCX", &explosion[9])) IOError("EXPLOD10.PCX");  printf(".\n");
 if (!SVGA_LoadPCX("EXPLOD11.PCX", &explosion[10])) IOError("EXPLOD11.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("EXPLOD12.PCX", &explosion[11])) IOError("EXPLOD12.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("HELM1.PCX", &helm[0])) IOError("HELM1.PCX"); printf("50%...\n");
 if (!SVGA_LoadPCX("HELM2.PCX", &helm[1])) IOError("HELM2.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("HELM3.PCX", &helm[2])) IOError("HELM3.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("HELM4.PCX", &helm[3])) IOError("HELM4.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("HELM5.PCX", &helm[4])) IOError("HELM5.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("HELM6.PCX", &helm[5])) IOError("HELM6.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("HELM7.PCX", &helm[6])) IOError("HELM7.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("UMB1.PCX", &umbr[0])) IOError("UMB1.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("UMB2.PCX", &umbr[1])) IOError("UMB2.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("UMB3.PCX", &umbr[2])) IOError("UMB3.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("UMB4.PCX", &umbr[3])) IOError("UMB4.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("UMB5.PCX", &umbr[4])) IOError("UMB5.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("UMB6.PCX", &umbr[5])) IOError("UMB6.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("CLEAN1.PCX", &clean[0])) IOError("CLEAN1.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("CLEAN2.PCX", &clean[1])) IOError("CLEAN2.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("ABOUT.PCX", &imgabout)) IOError("ABOUT.PCX");         printf(".\n");
 if (!SVGA_LoadPCX("HOWTO.PCX", &imghowto)) IOError("HOWTO.PCX");         printf(".\n");
 if (!SVGA_LoadPCX("CONTROLS.PCX", &imgcontrols)) IOError("CONTROLS.PCX"); printf(".\n");
 if (!SVGA_LoadPCX("LOSER.PCX", &imglose)) IOError("LOSER.PCX");           printf(".\n");
 if (!SVGA_LoadPalette("METEOR.PAL", &palette)) IOError("METEOR.PAL");     printf(".\n");
 if (!SVGA_LoadFont("8PT.MFF", &font)) IOError("8PT.MFF");                 printf(".\n");
 if (!SVGA_LoadFont("DIGITAL.MFF", &digits)) IOError("DIGITAL.MFF");       printf(".\n");
 if (!loadscores()) IOError("SCRAPPLE.DAT");                               printf(".\ndone\n");
 
 if (!MOUSE_Reset())  HWError("Microsoft compatible mouse driver could not be located.")

 if ((flag = SVGA_SetupMode(SVGA_640x480))!=0)
  {
   printf("Graphics error: #%i. Aborting...\n", abs(flag));
   exit(flag);
  }

 SVGA_SetAllPalette(&palette);
 SVGA_InitImage(&work, BytesPerLine, 480);
 SVGA_InitImage(&work2, BytesPerLine, 480);
 SVGA_InitImage(&work3, meteor[0].width+(4*mstp), meteor[0].height+(2*mstp));
 SVGA_InitImage(&work4, explosion[0].width, explosion[0].height);
 SVGA_InitImage(&explosionback, explosion[0].width, explosion[0].height);
 SVGA_InitImage(&explosionfront, explosion[0].width, explosion[0].height);
 SVGA_InitImage(&save, BytesPerLine, 450);

 SVGA_ClearImage(&save, 0);
 SVGA_SetFont(&font);

 KERNAL_Init();
 KERNAL_AddTask((void near *(*)(void))timerproc);
 KERNAL_Start();

 MOUSE_Init();
 MX=320, MY=240;
 MOUSE_Show();
 
 KEYBOARD_Init();

 if (!SB_Init()) GetSoundSettings(); else usesound=1;
 if (usesound) SB_LoadWave("REPEATME.WAV", &music);
 if (usesound) SB_LoadWave("METEOR.WAV", &sndrock);
 if (usesound) SB_LoadWave("EXPLODE.WAV", &sndexplode);
 intro();

 sx = -1;
 fx=bx=0;
 stpx=10;
 stime = *time;
 fdts = 0;

 mainthing();

 KERNAL_Stop();

 if (saveimage)
  {
   SVGA_GetAllPalette(&save.palette);
   SVGA_SavePCX("METEOR.PCX", &save);
  }
  
 savescores();
 
 if (usesound) SB_Done();
 KEYBOARD_Done();
 MOUSE_Done();
 VESA_SetMode(TEXT_MODE);
}
 
void intro()
{
 fabout.init(0,0,311,448+TH,1,0,1,&bbabout);
 fabout.show();
 SVGA_ShowImage(&imgabout, fabout.fx+1, fabout.fy+1, BLIT_NORMAL);

 while (!bokabout.check()) {}
 
 fabout.hide();
}

void mainthing()
{
 MENU    moptions(0,0,0,0,2,"New Game","Exit");
 MENU    msettings(0,0,0,0,2,"Game Settings", "SoundBlaster Settings");
 MENU    mhelp(0,0,0,0,4,"How to play","Controls","Helpful Hints", "High Scores","About");
 MENUBAR menu(0,0,MaxX,28,3, "Options", &moptions, "Settings", &msettings, "Help", &mhelp);
 BUTTON  btimer(MaxX-70, 2, 68, 23, "", NONE);
 BINT16  sm, smi;                           // Status Menu options, status of menu help;

 menu.show();
 SVGA_SetFont(&digits);
 btimer.show(0);
 SVGA_GetImage(&menubar, 0, 0, MaxX, 30);
 SVGA_SetFont(&font);
 et = *time;
 playing = 0;

 forever
  {
   mins = (count-(BINT16)((FLOTE)(*time-stime)/18.2))/60;
   secs = (count-(BINT16)((FLOTE)(*time-stime)/18.2))%60;
   sprintf(str,"%c%2i:%2i", (secs<0) ? '-':'+', abs(mins), abs(secs));

   if (strcmp((const char *)str, (const char *)oldstr))
    {
     strcpy((char *)oldstr, (const char *)str);
     btimer.reset(MaxX-70, 2, 68, 23, str, NONE);
     SVGA_SetFont(&digits);
     btimer.show(0);
     SVGA_SetFont(&font);
    }
   
   if ((mins<=0)&&(secs<=-2)&&((skid)||(cf==0)||(cf==3)))
    {
     if ((!autodeath)&&(usesound)) { SB_SOUND_INUSE[musicid] = 0; SB_SOUNDS[musicid].loop = 0; }
     
     mety = -meteor[0].height-40;
     metx = sx-(2*(meteor[0].height+201));
     SVGA_MergeImage(&work2, 0, 0, work2.width, work2.height, &work, 0, 0, BLIT_NORMAL);

     SVGA_ClearImage(&work, 0);
     processbackdrop(&work, &foreground, fx, FYOFF+30, BLIT_SPRITE);
     SVGA_SetForeground(&work, 1);

     if (usesound) rockid=SB_PlaySound(&sndrock, 0);
     delay(450);
     i = 0;
     while ((metx+80)<sx)
      {
       if (usesound && (SB_SOUND_INUSE[rockid])) while (SB_SOUNDS[rockid].curpos < i*(SB_SOUNDS[rockid].length/100)) {}
//       if (!usesound) sound(abs(1000-i));
       i++;
       metx+= 2*mstp;
       mety+= mstp;
       if (metx+80>sx)
        {
         mety -= (metx+80-sx)/2;
         metx = sx-80;
        }
        
       SVGA_MergeImage(&work3, 0, 0, work3.width, work3.height, &work2, metx-(2*mstp), mety-mstp, BLIT_NORMAL);
       SVGA_MergeImage(&work3, 2*mstp, mstp, meteor[0].width, meteor[0].height, &meteor[(mety)&1], 0, 0, BLIT_SPRITE);
       if ((mety-mstp+30)<30)
        SVGA_MergeImage(&work3, 0, 0, work3.width, 30, &menubar, metx-2*mstp, (mety-mstp+30), BLIT_NORMAL);
       SVGA_ShowImage(&work3, metx-2*mstp, mety-mstp+30, BLIT_NORMAL);
      }
     processbackdrop(&work, &background, bx, 0, BLIT_NORMAL);
     SVGA_MergeImage(&work, sx-EXPXOFFSET, EXPY, explosion[0].width, explosion[0].height, &explosion[0], 0, 0, BLIT_SPRITE);
     processbackdrop(&work, &foreground, fx, FYOFF, BLIT_SPRITE);
     SVGA_ShowImage(&work, 0, 30, BLIT_NORMAL);
     processbackdrop(&work, &background, bx, 0, BLIT_NORMAL);
     SVGA_MergeImage(&explosionback, 0,0, explosion[0].width, explosion[0].height, &work, sx-EXPXOFFSET, EXPY, BLIT_NORMAL);
     SVGA_ClearImage(&work, 0);
     processbackdrop(&work, &foreground, fx, FYOFF, BLIT_SPRITE);
     SVGA_MergeImage(&explosionfront, 0,0, explosion[0].width, explosion[0].height, &work, sx-EXPXOFFSET, EXPY, BLIT_NORMAL);     

     if (usesound)
      {
       SB_SOUND_INUSE[rockid] = 0;
       SB_PlaySound(&sndexplode, 0);
      }
//     else
//      nosound();

     SVGA_ClearForeground(0);

     for (sm=0;sm<12;sm++)
      {
       SVGA_MergeImage(&work4, 0, 0, work4.width, work4.height, &explosionback, 0, 0, BLIT_NORMAL);
       SVGA_MergeImage(&work4, 0, 0, explosion[0].width, explosion[0].height, &explosion[sm], 0, 0, BLIT_SPRITE);
       SVGA_MergeImage(&work4, 0, 0, work4.width, work4.height, &explosionfront, 0, 0, BLIT_SPRITE);
       SVGA_ShowImage(&work4, sx-EXPXOFFSET, EXPY+30, BLIT_NORMAL);
       delay(md);
      }
     playing = 0;
     if (!autodeath) { delay(2000); showscores();}
    }

   forever
    {
     mt = *time;
     menu.go(&sm, &smi);
     if ((sm==0)&&(smi==1)) return;
     if ((sm==0)&&(smi==0))
      {
       getname();
       if (usesound && (!playing)) musicid=SB_PlaySound(&music, 1);
       playing = 1;
       stime = *time-9;
       et = *time;
       fx=bx=0;
       sx=-1;
       stpx=10;
       break;
      }
     else if ((autodeath)&&(playing==0))
      {
       playing = 1;
       stime = *time-9;
       et = *time;
       fx=bx=0;
       sx=-1;
       stpx=10;
      }
     if ((sm==1)&&(smi==0)) getgamesettings();
     if ((sm==1)&&(smi==1)) GetSoundSettings();
     if ((sm==2)&&(smi==0)) howto();
     if ((sm==2)&&(smi==1)) controls();
     if ((sm==2)&&(smi==2)) helpfulhints();
     if ((sm==2)&&(smi==3)) showscores();
     if ((sm==2)&&(smi==4)) intro();
     stime += (*time-mt);
     if (playing) break;
    }

   if ((skid)||((KeyFlags[KEY_LEFT]||KeyFlags[KEY_RIGHT]||(sx==-1)||((cf!=0)&&(cf!=3)))&&((*time-et)>sps)))
    {
     et = *time;
     if (sx==-1) sx=300;
  
     if ((!skid)||(skid&&(KeyFlags[KEY_LEFT]||KeyFlags[KEY_RIGHT])))
      {
       if (cf==5)
        cf=0;
       else
        cf++;
      }

     if (KeyFlags[KEY_LEFT])
      {
       stpx = -STEP;
       if (sx>RTMRGN) sx=RTMRGN-1;
       }
     else if (KeyFlags[KEY_RIGHT])
      {
       stpx = STEP;
       if (sx<LFTMRGN) sx=LFTMRGN+1;
      }

     if ((!skid)&&(KeyFlags[KEY_LCTRL]||KeyFlags[KEY_RCTRL])&&(KeyFlags[KEY_LEFT]||KeyFlags[KEY_RIGHT])) stpx *= 2;

     if (stpx<0) cf += 6;

     if ((sx<LFTMRGN)||(sx>RTMRGN))
      {
       bx += stpx;
       fx += stpx;
      }
     else
      sx += stpx;

     fdts++;
 
     processbackdrop(&work, &background, bx, 0, BLIT_NORMAL);
     SVGA_MergeImage(&work, sx, 201, andrew[0].width, andrew[0].height, &andrew[cf], 0, 0, BLIT_SPRITE);
     processbackdrop(&work, &foreground, fx, FYOFF, BLIT_SPRITE);

     SVGA_ShowImage(&work, 0,30,BLIT_NORMAL);
     if (cf>5) cf -= 6;
    }
  }      /* End Program Loop */
}

void GetSoundSettings(void)
{
 BUTTON         ok(0,0,0,0,"Ok", KEY_ENTER);
 BUTTON         cancel(0,0,0,0,"Cancel", KEY_ESC);
 BUTTONBAR      bb(2, &ok, &cancel);
 FORM           f(0,0,370,305,1,1,1,"Configure Sound Settings", &bb);

 OPTION         a1(0,0,"220h (default)"),
                a2(0,0,"240h"),
                a3(0,0,"260h"),
                a4(0,0,"280h");
 OPTIONGROUP    oa(f.fx+5, f.fy+10+TH, SELECT_ONE, 0, 4, &a1, &a2, &a3, &a4);

 OPTION         l1(0,0,"Channel 0"),
                l2(0,0,"Channel 1 (default)"),
                l3(0,0,"Channel 3");
 OPTIONGROUP    ol(f.fx+5, f.fy+80+TH, SELECT_ONE, 1, 3, &l1, &l2, &l3);

 OPTION         h1(0,0,"Channel 4"),
                h2(0,0,"Channel 5 (default)"),
                h3(0,0,"Channel 6"),
                h4(0,0,"Channel 7"),
                h5(0,0,"Use same as low");
 OPTIONGROUP    oh(f.fx+5, f.fy+133+TH, SELECT_ONE, 1, 5, &h1, &h2, &h3, &h4, &h5); 

 OPTION         i1(0,0,"Irq 2"),
                i2(0,0,"Irq 5 (default)"),
                i3(0,0,"Irq 7"),
                i4(0,0,"Irq 10");
 OPTIONGROUP    oi(f.fx+5, f.fy+212+TH, SELECT_ONE, 1, 4, &i1, &i2, &i3, &i4);
 BINT16         bs; // No not bullshit, button status!!
 STR            newenv[500];

 if (usesound) SB_Done();
 
 f.show();
 oa.show();
 ol.show();
 oh.show();
 oi.show();

 SVGA_PrintString(f.fx+5, f.fy+5, 0, "SoundBlaster base I/O address:", 0);
 SVGA_PrintString(f.fx+5, f.fy+75, 0, "Low DMA Channel:", 0);
 SVGA_PrintString(f.fx+5, f.fy+130, 0, "High DMA Channel:", 0);
 SVGA_PrintString(f.fx+5, f.fy+206, 0, "Interrupt request query (IRQ) number:", 0);

 while ((bs=bb.check())==NONE)
  {
   oa.check();
   ol.check();
   oh.check();
   oi.check();
  }

 if (bs==1)
  { 
   f.hide();
   usesound = 0;
   return;   // User cancelled
  }
 if (a1.check()) SB_BaseAddr = 0x220;
 if (a2.check()) SB_BaseAddr = 0x240;
 if (a3.check()) SB_BaseAddr = 0x260;
 if (a4.check()) SB_BaseAddr = 0x280;
 if (l1.check()) SB_LowDma = 0;
 if (l2.check()) SB_LowDma = 1;
 if (l3.check()) SB_LowDma = 3;
 if (h1.check()) SB_HighDma = 4;
 if (h2.check()) SB_HighDma = 5; 
 if (h3.check()) SB_HighDma = 6;
 if (h4.check()) SB_HighDma = 7;
 if (h5.check()) SB_HighDma = SB_LowDma;
 if (i1.check()) SB_IrqNum = 2;
 if (i2.check()) SB_IrqNum = 5;
 if (i3.check()) SB_IrqNum = 7;
 if (i4.check()) SB_IrqNum = 10;
 sprintf(newenv, "BLASTER=A%x D%i H%i I%i", SB_BaseAddr, SB_LowDma, SB_HighDma, SB_IrqNum);
 putenv(newenv);

 f.hide();

 if (!SB_Init()) 
  {
   msgbox("SoundBlaster Error", "The settings you just entered did not work. This may be due to a low level driver error, or you entered incorrect data. The program will continue without sound.", OK, &font, &font);
   usesound=0;
  }
 else
  usesound=1;
 if (playing) musicid=SB_PlaySound(&music, 1);
}

void howto()
{
 fabout.show();
 SVGA_ShowImage(&imghowto, fabout.fx+1, fabout.fy+1, BLIT_NORMAL);

 while (!bokabout.check()) {}
 fabout.hide();
}

void controls()
{
 fabout.show();
 SVGA_ShowImage(&imgcontrols, fabout.fx+1, fabout.fy+1, BLIT_NORMAL);

 while (!bokabout.check()) {}
 fabout.hide();
}

void timerproc()
{
 static BINT16 c=50;
 BINT32 w;

 if ((KeyFlags[KEY_LCTRL]||KeyFlags[KEY_RCTRL])&&(KeyFlags[KEY_P])&&(c>36))
  {
   c=0;

   if (saveimage)
    for (w=0;w<(NOBINT32)save.width*save.height;w++)
     save.hMem[w] = 0;

   SVGA_MergeImage(&save, 0, 0, save.width, save.height, &work, 0, 0, BLIT_SPRITE);
   saveimage=1;
  }
 c++;
}

void getgamesettings(void)
{
 BUTTON         ok(0,0,0,0,"Ok", KEY_ENTER);
 BUTTON         cancel(0,0,0,0,"Cancel", KEY_ESC);
 BUTTONBAR      bb(2, &ok, &cancel);
 FORM           f(0,0,470,255,1,1,1,"Configure Game Settings", &bb);
 TEXTBOX        t1(f.fx+5, f.fy+5, 1, "20", 3);
 TEXTBOX        t2(f.fx+5, f.fy+20, 1, "2", 2);
 TEXTBOX        t3(f.fx+5, f.fy+35, 1, "15", 4);
 TEXTBOX        t4(f.fx+5, f.fy+50, 1, names[curscore], NL);
 TEXTBOX        t5(f.fx+5, f.fy+65, 0, "0", 4);
 OPTION         o1(0,0,"Sound Enabled"),
                o3(0,0,"Skiding Mode Enabled"),
                o2(0,0,"Umbrella Mode Enabled"),
                o5(0,0,"Helmet Mode Enabled"),
                o4(0,0,"Auto-Death Mode Enabled");
 OPTIONGROUP    oo(f.fx+5, f.fy+110, SELECT_MUL, 0, 5, &o1, &o3, &o2, &o5, &o4);

 BINT16         i,x;
 

 f.show();

 if (numscores==25) t4.settext(names[4]);
 if (numscores==25) sprintf(str, "%i", scores[4]); else sprintf(str, "%i", scores[curscore]);
 t5.settext(str);
 
 SVGA_PrintString(f.fx+45, f.fy+5, 0, "Number of seconds before METEOR comes (max:600)", 0);
 SVGA_PrintString(f.fx+45, f.fy+20, 0, "Number of frames Andrew moves per second (max:18)", 0);
 SVGA_PrintString(f.fx+45, f.fy+35, 0, "Number of milliseconds per frame of Andrew exploding", 0);
 SVGA_PrintString(f.fx+170, f.fy+50, 0, "Name of current user", 0);
 SVGA_PrintString(f.fx+170, f.fy+65, 0, "Score of current user", 0); 

 sprintf(str, "%i", count);
 t1.settext(str);
 sprintf(str, "%i", sps*18);
 t2.settext(str);
 sprintf(str, "%i", md);
 t3.settext(str);
 
 t1.show();
 t2.show();
 t3.show();
 t4.show();
 t5.show();
 o1.show(!usesound);
 o2.show(!useumbr);
 o3.show(!skid);
 o4.show(!autodeath);
 o5.show(!helmet);

 i=NONE;
 while ((i=bb.check())==NONE)
  {
   if (t1.checkfocus()) t1.go();
   if (t2.checkfocus()) t2.go();
   if (t3.checkfocus()) t3.go();
   if (t4.checkfocus()) t4.go();
//   if (t5.checkfocus()) t5.go();
   oo.check();
  }

 if (i==0)
  {
   strcpy(str, t1.gettext());
   sscanf(str, "%i", &count);
   strcpy(str, t2.gettext());
   sscanf(str, "%i", &i);
   sps = 18/i;
   strcpy(str, t3.gettext());
   sscanf(str, "%i", &i);
   md = i;
   strcpy(names[curscore], t4.gettext());
//   strcpy(str, t5.gettext());
//   sscanf(str, "%i", &i);
//   scores[curscore] = i;

   if (o1.check()) usesound=1; else usesound=0;
//   if (o2.check()) speaking=1; else speaking=0;
   if (o3.check()) skid=1; else skid=0;
   i = autodeath;
   if (o4.check()) autodeath=1; else autodeath=0;
   if (helmet) i=1; else i=0;
   x=useumbr;
   if (o5.check()) helmet=1; else helmet=0;
   if (o2.check()) useumbr=1; else useumbr=0;
   if (useumbr) helmet=0;
   if (autodeath && (!i) && (!playing) && (usesound))  musicid=SB_PlaySound(&music, 1);
   f.hide();
   if ((!playing)&&(!autodeath)) return;
   if (!helmet && (i)) hidehelm();
   if ((!useumbr) && x) umbdn();
   if (helmet && (!i)) drawhelm();
   if (useumbr && (!x)) umbup();
  }
 else
  f.hide();
}

void processbackdrop(IMAGE_PTR to, IMAGE_PTR from, BINT16 xoff, BINT16 yoff, BINT16 blt)
{
 
 if (xoff>=0) wx = xoff%from->width; else wx = from->width-(abs(xoff)%from->width);
 if ((wx+work.width)>background.width)
  {
   if (yoff)
    {
     SVGA_MergeImage(to, 0,yoff,from->width-wx, from->height, from, wx,0, blt);
     SVGA_MergeImage(to, from->width-wx,yoff,(wx+to->width)-from->width, from->height, from, 0,0, blt);
    }
   else
    {
     SVGA_MergeImage(to, 0,0,from->width-wx, from->height-30, from, wx,30, blt);
     SVGA_MergeImage(to, from->width-wx,0,(wx+to->width)-from->width, from->height-30, from, 0,30, blt);
    }
  }
 else 
  {
   if (yoff)
    SVGA_MergeImage(to, 0,yoff, from->width,from->height, from, wx, 0, blt);
   else
    SVGA_MergeImage(to, 0,0, from->width,from->height-30, from, wx,30, blt);
  }
}

BINT16 loadscores()
{
 FILE *f;
 BINT16 i,x;

 if ((f=fopen("SCRAPPLE.DAT", "rb"))==NULL) return(1);

 fread(&i, 1, 2, f);

 for (x=0;x<i;x++)
  {
   fread(&names[x][0], NL, 1, f);
   fread(&scores[x], 1, 2, f);
  }
 curscore=i-1; numscores=i;
 fclose(f);
 return(1);
}
  
void savescores()
{
 FILE *f;
 BINT16 i,x;

 if ((f=fopen("SCRAPPLE.DAT", "wb"))==NULL) return;

 i=numscores;
 fwrite(&i, 1, 2, f);

 for (x=0;x<i;x++)
  {
   fwrite(&names[x][0], NL, 1, f);
   fwrite(&scores[x], 1, 2, f);
  }
 fclose(f);
}

void showscores()
{
 BINT16 x, now;
 fabout.init(0,0,311,448+TH,1,1,1,"High Scores in METEOR",&bbabout);
 fabout.show();

 SVGA_ShowImage(&imglose, fabout.fx, fabout.fy+fabout.fh-imglose.height-6, BLIT_NORMAL);
 
 CurrentFont->romset=1;
 
 for (x=0;x<numscores;x++)
  {
   SVGA_PrintString(fabout.fx+5, fabout.fy+5+(x*(TH+2)), 2, names[x], 0);
   sprintf(str, "%i", scores[x]);
   SVGA_PrintString(fabout.fx+175, fabout.fy+5+(x*(TH+2)), 2, str, 0);
  }

 CurrentFont->romset=0;

 sprintf(str, "%i", scores[curscore]);
 
 now = *time;
 while (!bokabout.check())
  {
   if (*time-now>=6)
    {
     now = *time;
     _disable();
     CurrentFont->romset=1;
     SVGA_PrintString(fabout.fx+175, fabout.fy+5+(curscore*(TH+2)), (((x++)%2)==1) ? 175 : 2, str, 0);
     CurrentFont->romset=0;
     _enable();
    }
  }

 fabout.hide();
 fabout.init(0,0,311,448+TH,1,0,1,&bbabout);
}

void getname()
{
 FORM    f(0,0,180, 53, 1, 1, 1, "Enter your name", &bbabout);
 TEXTBOX t(f.fx+3, f.fy+3, 1, names[curscore], NL);
 BINT16  x;
 
 if (numscores==25) t.settext(names[4]);
 
 f.show();
 t.show();

 t.setfocus(1);
 t.go();

 while ((!bokabout.check())&&(!KeyFlags[KEY_ENTER]))
  if (t.checkfocus()) t.go();

 for (x=4;x<numscores+1;x++)
  if ((strcmp(names[x], t.gettext()))==NULL)
   {
    curscore = x;
    f.hide();
    fabout.init(0,0,311,448+TH,1,0,1,&bbabout);
    return;
   }

 if (numscores==25)
  {
   for (x=24;x>4;x--)
    {
     strcpy(names[x+1], names[x]);
     scores[x+1] = scores[x];
    }
   strcpy(names[5], t.gettext());
  }
 else
  {
   curscore=numscores;numscores++;
   strcpy(names[curscore], t.gettext());
  }

 f.hide();
 fabout.init(0,0,311,448+TH,1,0,1,&bbabout);
}

void drawhelm()
{
 BINT16 j;

 for (i=0;i<5;i++)
  {
   processbackdrop(&work, &background, bx, 0, BLIT_NORMAL);

   if ((i==3)||(i==4))
    SVGA_MergeImage(&work, sx, 151, helm[0].width, helm[i].height, &helm[i], 0, 0, BLIT_SPRITE);
   else
    SVGA_MergeImage(&work, sx, 201, helm[0].width, helm[0].height, &helm[i], 0, 0, BLIT_SPRITE);
   processbackdrop(&work, &foreground, fx, FYOFF, BLIT_SPRITE);

   SVGA_ShowImage(&work, 0, 30, BLIT_NORMAL);

   delay(350);
  }

 for (j=0;j<10;j++)
  {
   for (i=3;i<5;i++)
    {
     processbackdrop(&work, &background, bx, 0, BLIT_NORMAL);

     if ((i==3)||(i==4))
      SVGA_MergeImage(&work, sx, 151, helm[0].width, helm[i].height, &helm[i], 0, 0, BLIT_SPRITE);
     else
      SVGA_MergeImage(&work, sx, 201, helm[0].width, helm[0].height, &helm[i], 0, 0, BLIT_SPRITE);
     processbackdrop(&work, &foreground, fx, FYOFF, BLIT_SPRITE);

     SVGA_ShowImage(&work, 0, 30, BLIT_NORMAL);

     delay((j>3) ? 25 : 250);
    }
  }
 for (i=0;i<6;i++)
  {
   SVGA_MergeImage(&andrew[i], 0,0,helm[5].width, helm[5].height, &helm[5], 0, 0, BLIT_SPRITE);
   SVGA_MergeImage(&andrew[i+6], 0,0,helm[6].width, helm[6].height, &helm[6], 0, 0, BLIT_SPRITE);
  }

 processbackdrop(&work, &background, bx, 0, BLIT_NORMAL);
 SVGA_MergeImage(&work, sx, 201, andrew[0].width, andrew[0].height, &andrew[cf=0], 0, 0, BLIT_SPRITE);
 processbackdrop(&work, &foreground, fx, FYOFF, BLIT_SPRITE);

 SVGA_ShowImage(&work, 0, 30, BLIT_NORMAL);
}

void hidehelm()
{
 BINT16 j;

 for (j=0;j<10;j++)
  {
   for (i=4;i>2;i--)
    {
     processbackdrop(&work, &background, bx, 0, BLIT_NORMAL);

     if ((i==3)||(i==4))
      SVGA_MergeImage(&work, sx, 151, helm[0].width, helm[i].height, &helm[i], 0, 0, BLIT_SPRITE);
     else
      SVGA_MergeImage(&work, sx, 201, helm[0].width, helm[0].height, &helm[i], 0, 0, BLIT_SPRITE);
     processbackdrop(&work, &foreground, fx, FYOFF, BLIT_SPRITE);

     SVGA_ShowImage(&work, 0, 30, BLIT_NORMAL);

     delay((j>3) ? 25 : 250);
    }
  }

 for (i=2;i>=0;i--)
  {
   processbackdrop(&work, &background, bx, 0, BLIT_NORMAL);

   if ((i==3)||(i==4))
    SVGA_MergeImage(&work, sx, 151, helm[0].width, helm[i].height, &helm[i], 0, 0, BLIT_SPRITE);
   else
    SVGA_MergeImage(&work, sx, 201, helm[0].width, helm[0].height, &helm[i], 0, 0, BLIT_SPRITE);
   processbackdrop(&work, &foreground, fx, FYOFF, BLIT_SPRITE);

   SVGA_ShowImage(&work, 0, 30, BLIT_NORMAL);

   delay(350);
  }

 for (i=0;i<6;i++)
  {
   SVGA_MergeImage(&andrew[i], 0,0,clean[0].width, clean[0].height, &clean[0], 0, 0, BLIT_NORMAL);
   SVGA_MergeImage(&andrew[i+6], 0,0,clean[1].width, clean[1].height, &clean[1], 0, 0, BLIT_NORMAL);
  }

 processbackdrop(&work, &background, bx, 0, BLIT_NORMAL);
 SVGA_MergeImage(&work, sx, 201, andrew[0].width, andrew[0].height, &andrew[cf=0], 0, 0, BLIT_SPRITE);
 processbackdrop(&work, &foreground, fx, FYOFF, BLIT_SPRITE);

 SVGA_ShowImage(&work, 0, 30, BLIT_NORMAL);
}

#define NUMHINTS 66

void helpfulhints()
{
 BUTTON    bprv(0,0,0,0,"Previous Hint", KEY_P);
 BUTTON    bclose(0,0,0,0,"Close", KEY_ESC);
 BUTTON    bnext(0,0,0,0,"Next Hint", KEY_N);
 BUTTONBAR bb(3, &bprv, &bclose, &bnext);
 FORM      f(0,0,638,110,1,1,1,"Helpful Hints for METEOR", &bb);
 static BINT16 ch=0, x;
 STR       hints[NUMHINTS][80] =
  {"Try contacting the METEOR Help Hotline (8576 309)",
   "Try contacting the TACO BELL Hotline (1 800 TACOBELL)",
   "Make sure you bought the EXECUTIVE version.",
   "Are your eyes open? Are your glasses on? How about the monitor?",
   "Have you tried putting on the footbal helmet?",
   "Have you tried putting up the umbrella?",
   "Have you tried running really fast? (Hold CONTROL)",
   "Have you tried hiding in the fall out shelter?",
   "Have you tried waiting until the meteor gets real close then running away?",
   "Maybe you're being too pessimistic.",
   "Take a deep breath and check if you're a minor.",
   "Poor a tall cool glass of your favorite golden beverage.",
   "Have you tried running three screens to the left...",
   ", to the right?",
   "How about five?",
   "Are you sure you really want to save Andrew?",
   "Do you have any compassion at all?",
   "Try getting really angry (not around the people who sold you this game).",
   "Have you played this game much?",
   "Have you tried entering your name as Demosthenes?",
   "Have you tried pressing the space bar really fast?",
   "Have you tried NOT getting hit by the meteor (you fool)?",
   "I don't think you really tried.",
   "Well try harder.",
   "Did you try reading the instructions?",
   "I bet you're not pressing the correct buttons.",
   "Have you tried getting a haircut?",
   "And you're sure you didn't slobber on the disk or something?",
   "Are you or have you ever been part of a cult?",
   "What junior college do you attend?",
   "Have you tried buying a large number of METEOR games?",
   "Have you spoken with that O'Texan guy?",
   "Do you think maybe YOU should figure this out?",
   "Did you consider taking a training course on PUSHING THE LEFT AND RIGHT ARROWS?",
   "Would you consider putting the gun down?",
   "What would happen if you played 66 consecutive games?",
   "What would happen if your parents caught you playing 66 games?",
   "Are you some kind of a wuss?",
   "You should probably by the REGISTERED version of METEOR.",
   "It would be helpful if you read the hints section.",
   "Have you played THIS game much?",
   "Have you written your local legislators to complain?",
   "It might be helpful to read the ENTIRE hints section.",
   "-End of hints section-",
   "Sit down and relax. Feel the game. Put yourself in Andrew's shoes.",
   "Type the secret password as your name.",
   "Whats that? You don't know the password?",
   "Try skidding in the left direction on your right foot.",
   "Try setting the time to 600 seconds. Now you have 10 more minutes!",
   "Try hiding in the lake.",
   "Push the button that sais Push To Win",
   "That didn't do much -eh?",
   "Try going into the McDonald's",
   "Are you sure you bought the EXECUTIVE version?",
   "Let your dad play. See how he does.",
   "Try running to the end of the screen.",
   "You didn't find the end? Keep going.",
   "Farther.",
   "Still farther.",
   "We were lying, there is no end.",
   "Play the game on your birthday.",
   "Play the game on Andrew's birthday.",
   "Try dropping the controlled substance abuse.",
   "Get a support group.",
   "Buy yourself a clean shirt.",
   "-End of hints section-"
  };

 f.show();
 x=0;
 ch++;
 while (x!=1)
  {
   if (x==0)
    if (ch==0)
     printf("\7\n");
    else
     {
      SVGA_Rectangle(f.fx, f.fy, f.fx+f.fw, f.fy+f.fh, 252, 1);
      CurrentFont->romset=1;
      SVGA_PrintString(f.fx+2, f.fy+20, 250, hints[--ch], 0);
      CurrentFont->romset=0;
     }
   else if (x==2)
    if (ch==NUMHINTS-1)
     printf("\7\n");
    else
     {
      SVGA_Rectangle(f.fx, f.fy, f.fx+f.fw, f.fy+f.fh, 252, 1);
      CurrentFont->romset=1;
      SVGA_PrintString(f.fx+2, f.fy+20, 250, hints[++ch], 0);
      CurrentFont->romset=0;
     }
   x=bb.check();
  }
 f.hide();
}

void umbup()
{
 for (i=0;i<3;i++)
  {
   processbackdrop(&work, &background, bx, 0, BLIT_NORMAL);

   SVGA_MergeImage(&work, sx, 201, umbr[0].width, umbr[i].height, &umbr[i], 0, 0, BLIT_SPRITE);

   processbackdrop(&work, &foreground, fx, FYOFF, BLIT_SPRITE);

   SVGA_ShowImage(&work, 0, 30, BLIT_NORMAL);

   delay(250);
  }

 for (i=0;i<6;i++)
  {
   SVGA_MergeImage(&andrew[i], 0,0,umbr[4].width, umbr[4].height, &umbr[4], 0, 0, BLIT_NORMAL);
   SVGA_MergeImage(&andrew[i+6], 0,0,umbr[5].width, umbr[5].height, &umbr[5], 0, 0, BLIT_NORMAL);
  }

 processbackdrop(&work, &background, bx, 0, BLIT_NORMAL);
 SVGA_MergeImage(&work, sx, 201, andrew[0].width, andrew[0].height, &andrew[cf=0], 0, 0, BLIT_SPRITE);
 processbackdrop(&work, &foreground, fx, FYOFF, BLIT_SPRITE);

 SVGA_ShowImage(&work, 0, 30, BLIT_NORMAL);
}   

void umbdn()
{
 for (i=3;i>=0;i--)
  {
   processbackdrop(&work, &background, bx, 0, BLIT_NORMAL);

   SVGA_MergeImage(&work, sx, 201, umbr[0].width, umbr[i].height, &umbr[i], 0, 0, BLIT_SPRITE);

   processbackdrop(&work, &foreground, fx, FYOFF, BLIT_SPRITE);

   SVGA_ShowImage(&work, 0, 30, BLIT_NORMAL);

   delay(250);
  }

 for (i=0;i<12;i++)
  SVGA_KillImage(&andrew[i]);

 SVGA_LoadPCX("ANDREW1.PCX", &andrew[0]);
 SVGA_LoadPCX("ANDREW2.PCX", &andrew[1]);
 SVGA_LoadPCX("ANDREW3.PCX", &andrew[2]);
 SVGA_LoadPCX("ANDREW1.PCX", &andrew[3]);
 SVGA_LoadPCX("ANDREW4.PCX", &andrew[4]);
 SVGA_LoadPCX("ANDREW5.PCX", &andrew[5]);
 SVGA_LoadPCX("ANDREW6.PCX", &andrew[6]);
 SVGA_LoadPCX("ANDREW7.PCX", &andrew[7]);
 SVGA_LoadPCX("ANDREW8.PCX", &andrew[8]);
 SVGA_LoadPCX("ANDREW6.PCX", &andrew[9]);
 SVGA_LoadPCX("ANDREW9.PCX", &andrew[10]);
 SVGA_LoadPCX("ANDREW10.PCX", &andrew[11]);
 
 processbackdrop(&work, &background, bx, 0, BLIT_NORMAL);
 SVGA_MergeImage(&work, sx, 201, andrew[0].width, andrew[0].height, &andrew[cf=0], 0, 0, BLIT_SPRITE);
 processbackdrop(&work, &foreground, fx, FYOFF, BLIT_SPRITE);

 SVGA_ShowImage(&work, 0, 30, BLIT_NORMAL);
}   
