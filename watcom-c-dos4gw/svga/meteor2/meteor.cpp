//////////////////////////////////////////////////////////////////////////////////////
// METEOR.CPP (Ver.2)                                                               /
// Chris Eldredge (Andrew McLuaghlin Art.)                                         /
// Log:                                                                           /
//  4-27 Started work.                                                           /
//  4-30 Made walking scroll correctly w/ 2 players (complex).                  /
//  5-1  Added punching frames for Chris. Weird KB bugs...                     /
//  5-6  Working on intro.  Punch/ouch/walk frames for Andrew                 /
//       have been completed.                                                /
//  5-9  Re-writing entire input handling........ blah.                     /
//  5-11 Making user input work.  I have an idea for ai now.               /
//       NOTES -need intermediate standup frame (duck dont work)          /
//             -fix jumping so one can punch (kick?) and move            /
/////////////////////////////////////////////////////////////////////////
// AI NOTES                                                            /
//  make proc get_keys() which gets user input or generates           /
//  fake input for computers.                                        /
/////////////////////////////////////////////////////////////////////

#include "toolkit.h"

//////////////////////////////////////////////////////////////////
// MACROS                                                       /
////////////////////////////////////////////////////////////////

#define IOError(s) {printf("File not found: %s", s); exit(-1);}
#define HWError(s) {printf("%s",s); exit(-2);}
#define keys(n)    KeyFlags[playerkeys[player][n]]
#define time(n)    (*clock-n)
#define ACTION     action[player]
#define ACTION_TIME time(action_timer[player])
#define ACTION_TIMER action_timer[player]
#define FRAME      cf[player]

////////////////////////////////////////////////////////////////
// CONSTANT MACROS                                            /
//////////////////////////////////////////////////////////////

#define ANDREW          0
#define CHRIS           1
#define JASON           2

#define LFTMRGN         50
#define RTMRGN          530
#define ZTOP            200
#define ZBOT            400
#define LFT            -1
#define RTE             1
#define A               0
#define C               1
#define CLOCKADDX       0x046C
#define NAME_LENGTH     20

#define KB_LFT   0
#define KB_RTE   1
#define KB_UP    2
#define KB_DOWN  3
#define KB_RUN   4
#define KB_PUNCH 5
#define KB_JUMP  6

#define NUM_FRAMES  13
#define NUM_SOUNDS  2
#define MAX_SPRITES 2

#define IN          1
#define OUT         2

#define FRM_PUNCH       5
#define FRM_ARMSUP      4
#define FRM_OUCH        6
#define FRM_DUCK        7
#define FRM_JUMP        8
#define FRM_MEGALOAD    9
#define FRM_MEGAPUNCH   10
#define FRM_FALLING     11
#define FRM_DOWN        12

#define HIT_BLOCKING            1
#define JP_LAND                 2
#define JP_UP                   3
#define JP_DOWN                 4
#define HIT_PUNCHING            5
#define HIT_MEGALOAD            6
#define HIT_MEGAPUNCH           7
#define OUCH_DUCK               8
#define OUCH_OUCH               9
#define OUCH_FALLINGUP          11
#define OUCH_FALLING            12
#define OUCH_DOWN               13
#define OUCH_STANDUP            14

////////////////////////////////////////////////////////////////
// PROTOTYPES                                                 /
//////////////////////////////////////////////////////////////
void SVGA_ClearPalette();

void intro();
void _loadds timerproc();
void getsoundsettings(void);
void getgamesettings(void);
void mainthing();
void getname();
void helpfulhints();
void processbackdrop(IMAGE_PTR to, IMAGE_PTR from, BINT16 xoff, BINT16 yoff, BINT16 blt);
void mirror_image(IMAGE_PTR i);
void select_players();
BINT16 init_man(BINT16 p, BINT16 m);

void fadeout();
void fadein(PALETTE_PTR p);

////////////////////////////////////////////////////////////////////
// GLOBALS                                                        /
//////////////////////////////////////////////////////////////////

BUTTON          st_bok(0,0,0,0,"Ok",KEY_ENTER);          // Standard Ok btn
BUTTONBAR       st_bb(1, &st_bok);                       // Standard btn bar
FORM            st_frm(0,0,200,400,1,1,1,"Meteor", &st_bb); // Standard form

PALETTE fade_pal;
PALETTE fade_counter;
volatile BINT16  fade_state;
volatile BINT16  fade_level;
volatile BINT16  *clock = (BINT16 *)CLOCKADDX;

BINT16  PLAYERSTEP[MAX_SPRITES];
BINT16  PL[MAX_SPRITES];                // punch length
BINT32  bx;             // Background X
BINT32  fx;             // Foreground X
BINT32  sx[MAX_SPRITES];          // Sprite X
BINT16  sxol[MAX_SPRITES][NUM_FRAMES];      // sprite x offset left
BINT16  sxor[MAX_SPRITES][NUM_FRAMES];      // sprite x offset right
BINT16  syob[MAX_SPRITES][NUM_FRAMES];      // sprite y offset both
BINT16  action[MAX_SPRITES];                // What's this guy doing?
BINT16  action_timer[MAX_SPRITES];          // How long has this guy been doing it?
BINT16  hit_count[MAX_SPRITES];             // How many punches has this guy taken?
BINT16  punchkey[MAX_SPRITES];              // don't let the user just hold the punch key down.
SOUND   sfx[MAX_SPRITES][NUM_SOUNDS];
BINT16  step[MAX_SPRITES];        // Step X
BINT16  feet[MAX_SPRITES];        //
BINT16  sz[MAX_SPRITES];          // vertical (depth) co-ordinate.
BINT16  cf[MAX_SPRITES];          // Current Frame
BINT16  facing[MAX_SPRITES];      // direction currently facing
BINT16  hitlist[MAX_SPRITES];     // who did this guy hit?
BINT32  wx;                       // offset background work temp
BINT16  usesound=0;
BINT16  musicid, rockid;
STR     names[26][NAME_LEN];
IMAGE   background;
IMAGE   foreground;
IMAGE   men[MAX_SPRITES][NUM_FRAMES];
IMAGE   work;
PALETTE palette;
FONT    font, digits;
SOUND   music, sndrock, sndexplode;
STR     str[500];
BINT16  playerkeys[2][10];
BINT16  numplayers;

void main()
{
 BINT16 err;

 printf("Loading\n");
 BytesPerPixel = 1;

 if (!SVGA_LoadPCX("BACKGRND.PCX", &background)) IOError("BACKGRND.PCX");
 if (!SVGA_LoadPalette("METEOR.PAL", &palette)) IOError("METEOR.PAL");
// if (!SVGA_LoadFont("8PT.MFF", &font)) IOError("8PT.MFF");
 if (!SVGA_LoadFont("FONT.MFF", &font)) IOError("FONT.MFF");
 if (!SVGA_LoadFont("DIGITAL.MFF", &digits)) IOError("DIGITAL.MFF");
 
 if (!MOUSE_Reset())  HWError("Mouse driver could not be located.")

 if ((err = SVGA_SetupMode(SVGA_640x480))!=0)
  {
   printf("Graphics error: #%i. Aborting...\n", abs(err));
   exit(err);
  }

 SVGA_SetAllPalette(&palette);
 SVGA_InitImage(&work, BytesPerLine, 480);
 SVGA_ClearImage(&work,0);

 SVGA_SetFont(&font);

 fade_state = NONE;
 fade_level = 0;
 KERNAL_Init();
 KERNAL_AddTask((void near *(*)(void))timerproc);
 KERNAL_Start();

 KEYBOARD_Init();
// while (kbhit()) getch();
  
 intro();
 
 MOUSE_Init();
 MX=320, MY=240;
 MOUSE_Show();

 playerkeys[A][KB_LFT] = KEY_LEFT;
 playerkeys[A][KB_RTE] = KEY_RIGHT;
 playerkeys[A][KB_UP] = KEY_UP;
 playerkeys[A][KB_DOWN] = KEY_DOWN;
 playerkeys[A][KB_RUN] = KEY_RSHIFT;
 playerkeys[A][KB_PUNCH] = KEY_RALT;
 playerkeys[A][KB_JUMP] = KEY_RCTRL;
 playerkeys[C][KB_LFT] = KEY_F;
 playerkeys[C][KB_RTE] = KEY_H;
 playerkeys[C][KB_UP] = KEY_T;
 playerkeys[C][KB_DOWN] = KEY_G;
 playerkeys[C][KB_RUN] = KEY_LSHIFT;
 playerkeys[C][KB_PUNCH] = KEY_LALT;
 playerkeys[C][KB_JUMP] = KEY_LCTRL;

 SVGA_Cls(0);
 SVGA_SetAllPalette(&palette);

 select_players();
// if (!SB_Init()) GetSoundSettings(); else usesound=1;

 SB_Init();

 sx[A] = -1;
 sx[C] = -1;
 fx=bx=0;

 mainthing();

 KERNAL_Stop();

 SB_Done();
 KEYBOARD_Done();
 MOUSE_Done();
 VESA_SetMode(TEXT_MODE);
}

void select_players()
{
 FORM      f(0,0,620,460,1,0,0);
 BUTTON    p(f.fx+4,f.fy+4,(f.fw-12)/2,226,"",NONE);
 BUTTON    bContinue(f.fx+f.fw-128,f.fy+238,120,20,"Continue Code...",KEY_C);
 BUTTON    bVs(f.fx+f.fw-128,f.fy+262,120,20,"Vs. Mode",KEY_V);
 BUTTON    bStart(f.fx+f.fw-128,f.fy+286,120,20,"Start Game",KEY_S);
 BUTTONBAR bb(3,&bContinue,&bVs,&bStart);
 IMAGE     chris[3], andrew[3], window, bg;
 BINT16    frame=1, stp=1, timer, w, bgx=0,bbr=NONE;

 SVGA_LoadPCX("CWLF.PCX",&chris[0]);
 SVGA_LoadPCX("CSTAND.PCX",&chris[1]);
 SVGA_LoadPCX("CWRF.PCX",&chris[2]);
 SVGA_LoadPCX("AWLF.PCX",&andrew[0]);
 SVGA_LoadPCX("ASTAND.PCX",&andrew[1]);
 SVGA_LoadPCX("AWRF.PCX",&andrew[2]);
 SVGA_LoadPCX("SPBG.PCX",&bg);

 for (w=0;w<3;w++)
  {
   SVGA_MirrorImage(&chris[w]);
   SVGA_MirrorImage(&andrew[w]);
  }
 f.show();
 w = (p.w-12)/2;
 SVGA_InitImage(&window,w-8,180);

 p.show(BTN_UP);
 p.reset(f.fx+8,f.fy+8,w,188,"",NONE);
 p.show(BTN_UP);
 p.reset(f.fx+12+w,f.fy+8,w,188,"",NONE);
 p.show(BTN_UP);
 p.reset(f.fx+8,f.fy+200,(f.fw-12)/2-9,26,"The Clumps",NONE);
 p.show(BTN_UP);
 p.reset(f.fx+(f.fw-12)/2+8,f.fy+4,(f.fw-12)/2,226,"",NONE);
 p.show(BTN_UP);
 p.reset(f.fx+(f.fw-12)/2+12,f.fy+8,w,188,"",NONE);
 p.show(BTN_UP);
 p.reset(f.fx+(f.fw-12)/2+w+16,f.fy+8,w,188,"",NONE);
 p.show(BTN_UP);
 p.reset(f.fx+(f.fw-12)/2+12,f.fy+200,(f.fw-12)/2-9,26,"Lord Trumpet",NONE);
 p.show(BTN_UP);

 timer = *clock-5;       // force first step;
 p.reset(f.fx+f.fw-132,f.fy+234,128,bStart.y-210,"",NONE);
 p.show(BTN_UP);
 bb.show();

 while (!(bbr==2))
  {
   bbr = bb.check();
   if (time(timer)>4)
    {
     timer = *clock;
     if (bgx+window.width > bg.width)
      {
       SVGA_MergeImage(&window,0,0,bg.width-bgx,window.height, &bg, bgx,0, BLIT_NORMAL);
       SVGA_MergeImage(&window,bg.width-bgx,0,(bgx+window.width)-bg.width,window.height, &bg, 0,0, BLIT_NORMAL);
      }
     else
      SVGA_MergeImage(&window,0,0,window.width,window.height, &bg, bgx,0, BLIT_NORMAL);
     SVGA_MergeImage(&window,w/2-andrew[0].width/2,14,andrew[0].width,andrew[0].height,&andrew[frame],0,0, BLIT_SPRITE);
     SVGA_ShowImage(&window,f.fx+12,26,BLIT_NORMAL);

     bgx += window.width;
     bgx %= bg.width;
     if (bgx+window.width > bg.width)
      {
       SVGA_MergeImage(&window,0,0,bg.width-bgx,window.height, &bg, bgx,0, BLIT_NORMAL);
       SVGA_MergeImage(&window,bg.width-bgx,0,(bgx+window.width)-bg.width,window.height, &bg, 0,0, BLIT_NORMAL);
      }
     else
      SVGA_MergeImage(&window,0,0,window.width,window.height, &bg, bgx,0, BLIT_NORMAL);
     SVGA_MergeImage(&window,w/2-chris[0].width/2,10,chris[0].width,chris[0].height,&chris[frame],0,0, BLIT_SPRITE);
     SVGA_ShowImage(&window,f.fx+315,26,BLIT_NORMAL);

     bgx -= window.width;
     if (bgx<0) bgx+=bg.width;

     bgx += 15;
     bgx %= bg.width;
     frame += stp;
     if ((frame==2)||(frame==0))
      {
       stp = -stp;
      }
    }
  }

 SVGA_KillImage(&chris[0]);
 SVGA_KillImage(&chris[1]);
 SVGA_KillImage(&chris[2]);
 SVGA_KillImage(&andrew[0]);
 SVGA_KillImage(&andrew[1]);
 SVGA_KillImage(&andrew[2]);
 SVGA_KillImage(&window);
 SVGA_KillImage(&bg);
 f.hide();
 init_man(0,CHRIS);
 init_man(1,ANDREW);
 numplayers = 2;
 MOUSE_Hide();
}

void fadeout()
{
 SVGA_GetAllPalette(&fade_counter);
 fade_level = 0;
 fade_state = OUT;
}
void fadein(PALETTE_PTR p)
{
 BINT16 i;
 for (i=0;i<256;i++)
  {
   fade_pal.color[i].red = p->color[i].red;
   fade_pal.color[i].green = p->color[i].green;
   fade_pal.color[i].blue = p->color[i].blue;
  }
 fade_level = 0;
 fade_state = IN;
}

void reverse_man(BINT16 m)
{
 BINT16 i;

 for (i=0;i<NUM_FRAMES;i++)
  SVGA_MirrorImage(&men[m][i]);
}

#define Fx (fMain.fx+fMain.fw)
#define bw (120)
#define bh (20)

void mainthing()
{
 BINT16    player,btn,quit=0,i,hit;
 FORM      fMain(0,0,450,350,1,0,0);
 BUTTON    bPanel(Fx-bw-12,fMain.fy+4,bw+8,124,"",NONE);
 BUTTON    bHide((Fx+fMain.fx)/2-60,fMain.fy+fMain.fh-24,bw,bh,"Hide",KEY_H);
 BUTTON    bVolume(Fx-bw-8,fMain.fy+8,bw,bh,"Volume Control",KEY_V);
 BUTTON    bRestart1(Fx-bw-8,fMain.fy+32,bw,bh,"Restart Stage",KEY_S);
 BUTTON    bRestart2(Fx-bw-8,fMain.fy+56,bw,bh,"Restart Game",KEY_G);
 BUTTON    bAddCode(Fx-bw-8,fMain.fy+80,bw,bh,"Continue",KEY_C);
 BUTTON    bExit(Fx-bw-8,fMain.fy+104,bw,bh,"Quit",KEY_Q);
 BUTTONBAR bbMain(6, &bHide, &bRestart1,&bRestart2,&bAddCode,&bExit,&bVolume);

 sx[A] = 300;
 sx[C] = 100;
 cf[0]= cf[1] = 0;
 facing[A] = RTE;
 reverse_man(A);
 facing[C] = LFT;
 while (!(quit))
  {
   for (player=0;player<numplayers;player++)
    {
     switch(ACTION)
      {
       case JP_UP: {
         FRAME = FRM_JUMP;
         if (feet[player]-15>sz[player]-100)
          feet[player]-=15;
         else
          ACTION=JP_DOWN;
         break;
        }
       case JP_DOWN: {
         FRAME = FRM_JUMP;
         if (feet[player]+20<sz[player])
          feet[player]+=20;
         else
          {
           feet[player]=sz[player];
           ACTION=JP_LAND;
           ACTION_TIMER = *clock;
           FRAME = FRM_DUCK;
          }
         break;
        }
       case JP_LAND: {
         step[player]=0;
         if (ACTION_TIME>5)
          {
           ACTION = 0;
           FRAME=0;
          }
         else
          FRAME=FRM_DUCK;
         break;
        }
       case OUCH_OUCH: {
         FRAME=FRM_OUCH;
         step[player] = 0;
         if (ACTION_TIME>5)
          {
           ACTION = 0;
           FRAME=0;
          }
         break;
        }
       case OUCH_DUCK: {
         FRAME=FRM_DUCK;
         step[player]=0;
         if (ACTION_TIME>27)
          {
           hit_count[player]--;
           ACTION=0;
           FRAME=0;
          }
         break;
        }
       case OUCH_FALLINGUP: {
         FRAME = FRM_FALLING;
         if (!((sx[player]+step[player]>LFTMRGN)&&(sx[player]+step[player]<RTMRGN)))
          step[player]=0;
         if (feet[player]>sz[player]-60)
          feet[player]-=20;
         else
          {
           ACTION = OUCH_FALLING;
           ACTION_TIMER = *clock;
          }
         break;
        }
       case OUCH_FALLING: {
         FRAME = FRM_FALLING;
         if (!((sx[player]+step[player]>LFTMRGN)&&(sx[player]+step[player]<RTMRGN)))
          step[player]=0;
         if (feet[player]-30<sz[player])
          feet[player]+=20;
         else
          {
           feet[player]=sz[player];
           ACTION = OUCH_DOWN;
           ACTION_TIMER = *clock;
           FRAME=FRM_DOWN;
          }
         break;
        }
       case OUCH_DOWN: {
         FRAME = FRM_DOWN;
         feet[player]=sz[player];
         step[player]=0;
         if (ACTION_TIME>18)
          {
           ACTION = OUCH_STANDUP;
           ACTION_TIMER = *clock;
           FRAME=FRM_DUCK;
          }
         break;
        }
       case OUCH_STANDUP: {
         FRAME = FRM_DUCK;
         if (ACTION_TIME>6)
          {
           ACTION = 0;
           FRAME=0;
          }
         break;
        }
       case HIT_BLOCKING: {
         if (ACTION_TIME>54)
          {
           ACTION=0;
           FRAME=0;
          }
         break;
        }
       case HIT_PUNCHING: {
         if (ACTION_TIME<3) break;
         FRAME = FRM_ARMSUP;
         ACTION = HIT_BLOCKING; 
         ACTION_TIMER = *clock;
         break;
        }
       case HIT_MEGALOAD: {
         if (ACTION_TIME<10) break;
         hit_count[hitlist[player]]=0;
         FRAME=FRM_MEGAPUNCH;
         if (facing[player]==LFT)
          {
           if (sx[player]-10>LFTMRGN) sx[player]-=10;
           if (facing[hitlist[player]]==LFT)
            {
             facing[hitlist[player]]=RTE;
             reverse_man(hitlist[player]);
            }
           step[hitlist[player]] = -8;
          }
         else
          {
           if (sx[player]+10<RTMRGN) sx[player]+=10;
           if (facing[hitlist[player]]==RTE)
            {
             facing[hitlist[player]]=LFT;
             reverse_man(hitlist[player]);
            }
           step[hitlist[player]] = 8;
          }
         ACTION_TIMER = *clock;
         ACTION = HIT_MEGAPUNCH;
         SB_PlaySound(&sfx[player][0],0);
         action[hitlist[player]]=OUCH_FALLINGUP;  
         cf[hitlist[player]]=FRM_FALLING;
         action_timer[hitlist[player]] = *clock;
         break;
        }
       case HIT_MEGAPUNCH: {
         if (ACTION_TIME<4) break;
         FRAME=FRM_ARMSUP;
         ACTION_TIMER = *clock;
         ACTION = HIT_BLOCKING;
         break;
        }
      }       // end if actions are running.

     if (ACTION>2) continue;

     if ((keys(KB_JUMP))&&(ACTION<2))
      {
       ACTION=JP_UP;
       ACTION_TIMER=*clock;
       FRAME=FRM_JUMP;
//       continue;
      }
     if (keys(KB_PUNCH))
      {
       if ((!punchkey[player])&&(ACTION!=HIT_PUNCHING)&&(ACTION!=HIT_MEGALOAD)&&(ACTION!=HIT_MEGAPUNCH))
        {
         FRAME = FRM_PUNCH;
         ACTION = HIT_PUNCHING;
         ACTION_TIMER = *clock;

         for (i=0;i<numplayers;i++)
          {
           if (facing[player]==LFT)
            {
             if (((sx[player]+sxol[player][FRAME]+4<sx[i]+men[i][0].width)&&(sx[player]+sxol[player][cf[player]]+4>sx[i]))||((sx[player]+sxol[player][cf[player]]+PL[player]+4<sx[i]+men[i][0].width)&&(sx[player]+sxol[player][cf[player]]+PL[player]+4>sx[i])))
              hit = 1;
             else hit = 0;
            }
           else
            {
             if (((sx[player]+sxor[player][cf[player]]+men[player][cf[player]].width-4<sx[i]+men[i][0].width)&&(sx[player]+sxor[player][cf[player]]+men[player][cf[player]].width-4>sx[i]))||((sx[player]+sxol[player][cf[player]]+men[player][cf[player]].width-PL[player]-4<sx[i]+men[i][0].width)&&(sx[player]+sxor[player][cf[player]]+men[player][cf[player]].width-PL[player]-4>sx[i])))
              hit = 1;
             else hit = 0;
            }
           if (hit)
            {
             if ((!(player==i))&&(action[i]<OUCH_OUCH))
              {
               hit_count[i]++;
//if jumping?
               if (hit_count[i]>=5)
                { // mega-hit!
                 FRAME=FRM_MEGALOAD;
                 ACTION_TIMER = *clock;
                 ACTION = HIT_MEGALOAD;
                 hitlist[player] = i;
                 SB_PlaySound(&sfx[player][1],0);
                }
               else
                {
                 if ((action[i]==JP_UP)||(action[i]==JP_DOWN))
                  {
                   cf[i] = FRM_FALLING;
                   action[i] = OUCH_FALLING;
                   action_timer[i] = *clock;
                  }
                 else if (hit_count[i]==4)
                  {
                   cf[i] = FRM_DUCK;
                   action[i] = OUCH_DUCK;
                   action_timer[i] = *clock;
                  }
                 else
                  {
                   action[i]=OUCH_OUCH; 
                   action_timer[i] = *clock;
                   cf[i] = FRM_OUCH;
                   step[i] = 0;
                  }
                 SB_PlaySound(&sfx[player][0],0);
                }
              }
//             break;             // get outta the loop if he hit someone.
            }
          } // end for num players (punch)
         punchkey[player]=1;
        }
      } // end if kb_punch
     else
      punchkey[player]=0;

     if (keys(KB_LFT)||keys(KB_RTE))
      {
       if (cf[player]>=3)
        cf[player]=0;
       else
        cf[player]++;
      }
     if (keys(KB_LFT))
      {
       if (!(facing[player]==LFT))
        {
         reverse_man(player);
         facing[player] = LFT;
        }
       step[player] = -PLAYERSTEP[player];
       if (sx[player]>RTMRGN) sx[player]=RTMRGN-1;
       }
     else if (keys(KB_RTE))
      {
       if (!(facing[player]==RTE))
        {
         reverse_man(player);
         facing[player] = RTE;
        }
       step[player] = PLAYERSTEP[player];
       if (sx[player]<LFTMRGN) sx[player]=LFTMRGN+1;
      }
     if (keys(KB_RUN)) step[player] *= 2;
     if ((!keys(KB_LFT))&&(!keys(KB_RTE)))
      step[player] = 0;
    } /* FOR PLAYER */

   if (numplayers==2) /* IF TWO PLAYER */
    {
     if ((sx[C]<LFTMRGN)&&(playerkeys[C][KB_LFT])) // LEFT (CHRIS)
      {
       if ((sx[A]<LFTMRGN)&&(step[A]<step[C]))
        {
         bx += step[A];
         fx += step[A];
         sx[C] += step[C]-step[A];
        }
       else if ((step[C]>=step[A])||(sx[A]<RTMRGN-2))
        {
         bx += step[C];
         fx += step[C];
         if ((sx[A]>LFTMRGN)||(step[A]>step[C]))
          sx[A] += step[A]-step[C];
        }
       else if ((step[C]<step[A])&&(step[A]<0))
        {
         bx += step[A];
         fx += step[A];
        } 
      }
     else if ((sx[A]<LFTMRGN)&&(playerkeys[A][KB_LFT])) // LEFT (ANDREW)
      {
       if ((step[A]>=step[C])||(sx[C]<RTMRGN-2))
        {
         bx += step[A];
         fx += step[A];
         if ((sx[C]>LFTMRGN)||(step[C]>step[A]))
          sx[C] += step[C]-step[A];
        }
       else if ((step[A]<step[C])&&(step[C]<=0))
        {
         bx += step[C];
         fx += step[C];
        } 
      }
     else if ((sx[C]>RTMRGN)&&(playerkeys[C][KB_RTE])) // RIGHT (CHRIS)
      {
       if ((sx[A]>RTMRGN)&&(step[A]>step[C]))
        {
         bx += step[A];
         fx += step[A];
         sx[C] += step[C]-step[A];
        }
       else if ((step[C]<=step[A])||(sx[A]>LFTMRGN+4))
        {
         bx += step[C];
         fx += step[C];
         if ((sx[A]<RTMRGN)||(step[A]<step[C]))
          sx[A] += step[A]-step[C];
        }
       else if ((step[C]>step[A])&&(step[A]>0))
        {
         bx += step[A];
         fx += step[A];
         //sx[C] += step[C]-step[A];
        } 
      }
     else if ((sx[A]>RTMRGN)&&(playerkeys[A][KB_RTE])) // RIGHT (ANDREW)
      {
       if ((step[A]<=step[C])||(sx[C]>LFTMRGN+2))
        {
         bx += step[A];
         fx += step[A];
         if ((sx[C]<RTMRGN)||(step[C]>step[A]))
          sx[C] += step[C]-step[A];
        }
       else if ((step[A]>step[C])&&(step[C]>=0))
        {
         bx += step[C];
         fx += step[C];
        } 
      }
     else
      {
       sx[C] += step[C];
       sx[A] += step[A];
      }
    }
   else /* ELSE IF ONE PLAYER */
    {
     if ((sx[A]<LFTMRGN)||(sx[A]>RTMRGN))
      {
       bx += step[A];
       fx += step[A];
      }
     else
      sx[A] += step[A];
    }

   processbackdrop(&work, &background, bx, 0, BLIT_NORMAL);
   for (player=0;player<numplayers;player++)
    {
     if (!keys(KB_PUNCH)) punchkey[player]=0; 
     if (facing[player]==LFT)
      SVGA_MergeImage(&work, sxol[player][cf[player]]+sx[player],feet[player]+syob[player][cf[player]], men[player][cf[player]].width,men[player][cf[player]].height, &men[player][cf[player]], 0,0, BLIT_SPRITE);
     else
      SVGA_MergeImage(&work, sxor[player][cf[player]]+sx[player],feet[player]+syob[player][cf[player]], men[player][cf[player]].width,men[player][cf[player]].height, &men[player][cf[player]], 0,0, BLIT_SPRITE);
    }
//   processbackdrop(&work, &foreground, fx, 160, BLIT_SPRITE);

   SVGA_ShowImage(&work, 0,30,BLIT_NTN);

   if (KeyFlags[KEY_SPACE]||KeyFlags[KEY_ESC]||KeyFlags[KEY_ENTER])
    {
     // SVGA_ClearForeground();
     SVGA_ShowImage(&work, 0,30,BLIT_NORMAL);
     MOUSE_Show();
     while (KeyFlags[KEY_SPACE]||KeyFlags[KEY_ESC]||KeyFlags[KEY_ENTER]) {}
    
     fMain.show();
     bPanel.show(BTN_DOWN);
     bbMain.show();
     btn = NONE;
     while ((btn!=0)&&(!(KeyFlags[KEY_SPACE]||KeyFlags[KEY_ESC]||KeyFlags[KEY_ENTER])))
      {
       btn = bbMain.check();
       if (btn==4)
        {
         if (msgbox("METEOR","Are you absolutely sure that you have had enough fun playing METEOR2? Remember that at any time while reading this message, you may press the NO button to continue playing.  Remember METEOR has no side effects and is great fun for the whole family.  You web toed vegetarian commie bastards?",YES_NO,&font,&font)==0)
          {
           quit=1;
           btn=0; // hide the form and exit
          }
        }
       if ((MX<fMain.x)||(MY<fMain.y)||(MX>fMain.x+fMain.w)||(MY>fMain.y+fMain.h))
        {
         if (ML) break;
        }
      }
     while (ML) {}
     fMain.hide();
     MOUSE_Hide();
    }
  }      /* End Program Loop */
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

BINT16 init_man(BINT16 p,BINT16 m)
{
 BINT16 i;

 switch (m)
  {
   case CHRIS: {
                if (!SVGA_LoadPCX("CSTAND.PCX", &men[p][0])) return(0);
                if (!SVGA_LoadPCX("CWLF.PCX", &men[p][1]))   return(0);
                if (!SVGA_LoadPCX("CWRF.PCX", &men[p][3]))   return(0);
                if (!SVGA_LoadPCX("CBLOCK.PCX", &men[p][4])) return(0);
                if (!SVGA_LoadPCX("CPUNCH.PCX", &men[p][5])) return(0);
                if (!SVGA_LoadPCX("COUCH.PCX", &men[p][6])) return(0);
                if (!SVGA_LoadPCX("CDUCK.PCX", &men[p][7])) return(0);
                if (!SVGA_LoadPCX("CJUMP.PCX", &men[p][8])) return(0);
                if (!SVGA_LoadPCX("CSWING.PCX", &men[p][FRM_MEGALOAD])) return(0);
                if (!SVGA_LoadPCX("CUPCUT.PCX", &men[p][FRM_MEGAPUNCH])) return(0);
                if (!SVGA_LoadPCX("CFALL.PCX", &men[p][FRM_FALLING])) return(0);
                if (!SVGA_LoadPCX("CDOWN.PCX", &men[p][FRM_DOWN])) return(0);
                if (!SB_LoadWave("CPUNCH.WAV", &sfx[p][0])) return(0);
                if (!SB_LoadWave("METEOR.WAV", &sfx[p][1])) return(0);
                SVGA_InitImage(&men[p][2],men[p][0].width,men[p][0].height);
                SVGA_MergeImage(&men[p][2],0,0,men[p][2].width,men[p][2].height,&men[p][0],0,0,BLIT_NORMAL);
                PLAYERSTEP[p] = 17;
                hit_count[p] = 0;
                feet[p] = sz[p] = 310;
                cf[p] = 0;
                PL[p] = 12;
                facing[p] = LFT;
                for (i=0;i<NUM_FRAMES;i++)
                 {
                  sxol[p][i] = 0;
                  sxor[p][i] = 0;
                  syob[p][i] = -164;
                 }
                sxol[p][5] = -15;
                break;
               }
   case ANDREW:{
                if (!SVGA_LoadPCX("ASTAND.PCX", &men[p][0])) return(0);
                if (!SVGA_LoadPCX("AWLF.PCX", &men[p][1]))   return(0);
                if (!SVGA_LoadPCX("AWRF.PCX", &men[p][3]))   return(0);

                if (!SVGA_LoadPCX("ABLOCK.PCX", &men[p][FRM_ARMSUP])) return(0);
                if (!SVGA_LoadPCX("APUNCH.PCX", &men[p][FRM_PUNCH])) return(0);
                if (!SVGA_LoadPCX("AOUCH.PCX", &men[p][FRM_OUCH])) return(0);
                if (!SVGA_LoadPCX("ADUCK.PCX", &men[p][FRM_DUCK])) return(0);
                if (!SVGA_LoadPCX("AJUMP.PCX", &men[p][FRM_JUMP])) return(0);
                if (!SVGA_LoadPCX("ASWING.PCX", &men[p][FRM_MEGALOAD])) return(0);
                if (!SVGA_LoadPCX("AUPCUT.PCX", &men[p][FRM_MEGAPUNCH])) return(0);
                if (!SVGA_LoadPCX("AFALL.PCX", &men[p][FRM_FALLING])) return(0);
                if (!SVGA_LoadPCX("ADOWN.PCX", &men[p][FRM_DOWN])) return(0);

                if (!SB_LoadWave("APUNCH.WAV", &sfx[p][0])) return(0);
                if (!SB_LoadWave("ATALK.WAV", &sfx[p][1])) return(0);
                SVGA_InitImage(&men[p][2],men[p][0].width,men[p][0].height);
                SVGA_MergeImage(&men[p][2],0,0,men[p][2].width,men[p][2].height,&men[p][0],0,0,BLIT_NORMAL);
                PLAYERSTEP[p] = 15;
                hit_count[p] = 0;
                cf[p] = 0;
                PL[p] = 12;
                feet[p] = sz[p] =  310;
                facing[p] = LFT;
                for (i=0;i<NUM_FRAMES;i++)
                 {
                  sxol[p][i] = 0;
                  sxor[p][i] = 0;
                  syob[p][i] = -160;
                 }
                sxol[p][5] = -15;
                break;
               }
   case JASON: {

                break;
               }
  }
 return(1);
}
void SVGA_ClearPalette()
{
 BINT16 i;
 RGB    c;
 c.red = c.green = c.blue = 0;
 for (i=0;i<255;i++)
  {
   SVGA_SetPalette(i,&c);
   fade_counter.color[i] = c;
  }
}

void intro()
{
 BINT32 scale,x,fx,fxs;
 BINT16 zx,zy,zf,i;
 BINT16 numbers[4] = {0,1,2,1};
 IMAGE meteor;
 IMAGE chrs[2];
 IMAGE jason[14];
 IMAGE z[3];
 IMAGE frames[10];
 IMAGE bg;
 IMAGE img;
 RGB   c;
 STR   fn[14];

 SVGA_Cls(0);

 for (i=0;i<10;i++)
  {
   sprintf(fn,"m%i.pcx",i);
   SVGA_LoadPCX(fn,&frames[i]);
  }
 SVGA_SetAllPalette(&frames[0].palette);
 
 x=0;

 for (i=0;i<7;i++)
  {
   x = *clock;
   while(time(x)<30)
    {
     zf++;
     zf%=4;
     SVGA_ShowImage(&frames[numbers[zf]+i],MaxX/2-frames[0].width/2,MaxY/2-frames[0].height/2,BLIT_NTN);
     delay(100);
    }
   if (KeyFlags[KEY_SPACE]) break;
  }
 
 fadeout();

 for (i=0;i<10;i++)
  SVGA_KillImage(&frames[i]);
 while (fade_state==OUT) {}
 if (KeyFlags[KEY_SPACE]) return;

 SVGA_LoadPCX("chrsdark.pcx",&bg);
 SVGA_LoadPCX("chrsdrk0.pcx",&chrs[0]);
 SVGA_LoadPCX("chrsdrk1.pcx",&chrs[1]);
 SVGA_LoadPCX("chrsz0.pcx",&z[0]);
 SVGA_LoadPCX("chrsz1.pcx",&z[1]);
 SVGA_LoadPCX("chrsz2.pcx",&z[2]);

 SVGA_ClearPalette();
 SVGA_ShowImage(&bg,0,0,BLIT_NTN);

 fadein(&bg.palette);
 i=0;
 x=*clock;
 zx = 525;
 zy = 140;
 while ((time(x)<280)&&(!KeyFlags[KEY_SPACE]))
  {
   i = !i;
   SVGA_ShowImage(&chrs[i],225,150,BLIT_NTN);
   delay(100);

   if (fade_state==NONE)
    {
     fade_state=NONE;             // Weird bug fix... disregard.
     zf++;
     zf %= 3;
     zx+=2;
     zy-=2;
     SVGA_ShowImage(&z[zf],zx,zy,BLIT_NTN);
     SVGA_GetPalette(114,&c);
     if (c.red > 1)
      {
       c.red-=2;
       c.blue-=2;
       c.green-=2;
      }
     else
      {
       SVGA_Rectangle(zx,zy,zx+z[0].width,zy+z[0].height,0,1);
       zx = 525;
       zy = 140;
       c.red=c.blue=c.green=63;
      }
     SVGA_SetPalette(114,&c);
    }
  }
 fadeout();
 x = *clock;
 while ((fade_state==OUT)&&(!KeyFlags[KEY_SPACE]))
  {
   i = !i;
   SVGA_ShowImage(&chrs[i],225,150,BLIT_NTN);
   delay(100);
   zf++;
   zf %= 3;
   zx+=2;
   zy-=2;
   SVGA_ShowImage(&z[zf],zx,zy,BLIT_NTN);
   if (zx>640)
    {
     zx = 524;
     zy = 140;
    }
  }
 SVGA_KillImage(&chrs[0]);
 SVGA_KillImage(&chrs[1]);
 SVGA_KillImage(&bg);
 SVGA_KillImage(&z[0]);
 SVGA_KillImage(&z[1]);
 SVGA_KillImage(&z[2]);

 if (KeyFlags[KEY_SPACE])
  {
   while (fade_state==OUT) {}
   return;
  }

 SVGA_LoadPCX("intrmet.pcx",&meteor);
 SVGA_LoadPCX("space.pcx",&bg);
 for (i=0;i<14;i++)
  {
   sprintf(fn,"introj%i.pcx",i);
   SVGA_LoadPCX(fn,&jason[i]);
  }
 SVGA_InitImage(&img,jason[9].width,jason[9].height);
 SVGA_Cls(0);
 SVGA_ClearPalette();
 SVGA_MergeImage(&bg,314,190,jason[0].width,jason[0].height, &jason[0], 0,0, BLIT_SPRITE);
 SVGA_ShowImage(&bg,0,0,BLIT_NORMAL);
 c.red = c.green = c.blue = 10;
 bg.palette.color[255] = c;
 fadein(&bg.palette);
 while (fade_state==IN) {}
 delay(2000);
 for (i=10;i<64;i++)  // fade jason's frame in
  {
   c.red = c.green = c.blue = i;
   SVGA_SetPalette(255,&c);
   delay(85);
  }

 delay(1000);

 for (i=0;i<2;i++)   // jason flicks his lighter
  {
   SVGA_MergeImage(&work,0,0,work.width,work.height, &bg, 0,0, BLIT_NORMAL);
   SVGA_MergeImage(&work,243,141,jason[2].width,jason[2].height, &jason[2],0,0, BLIT_SPRITE);
   SVGA_ShowImage(&work,0,0,BLIT_NTN);
   delay(300);
   SVGA_MergeImage(&work,0,0,work.width,work.height, &bg, 0,0, BLIT_NORMAL);
   SVGA_MergeImage(&work,314,190,jason[1].width,jason[1].height, &jason[1],0,0, BLIT_SPRITE);
   SVGA_ShowImage(&work,0,0,BLIT_NTN);
   delay(500);
  }

 i = *clock;
 x = 1;
 while (time(i)<80) // spark starts
  {
   SVGA_MergeImage(&work,0,0,work.width,work.height, &bg, 0,0, BLIT_NORMAL);
   SVGA_MergeImage(&work,243,141,jason[2].width,jason[2].height, &jason[x+3],0,0, BLIT_SPRITE);
   SVGA_ShowImage(&work,0,0,BLIT_NTN);
   delay(150);
   x = !x;
  }

 for (i=5;i<9;i++) // flame grows
  {
   SVGA_MergeImage(&work,0,0,work.width,work.height, &bg, 0,0, BLIT_NORMAL);
   SVGA_MergeImage(&work,243,141,jason[i].width,jason[i].height, &jason[i],0,0, BLIT_SPRITE);
   SVGA_ShowImage(&work,0,0,BLIT_NTN);
   delay(150);
  }
  
 i = *clock;
 while (time(i)>180) // flame flickers at max
  {
   x = !x;
   SVGA_MergeImage(&img,0,0,img.width,img.height, &bg, 243,141, BLIT_NORMAL);
   SVGA_MergeImage(&img,0,0,jason[x+9].width,jason[x+9].height, &jason[x+9],0,0, BLIT_SPRITE);
   SVGA_ShowImage(&img,243,141,BLIT_NORMAL);
   delay(150);
  }

 i = 63;

 while (i>10)
  {
   x = !x;
   SVGA_MergeImage(&img,0,0,img.width,img.height, &bg, 243,141, BLIT_NORMAL);
   SVGA_MergeImage(&img,0,0,jason[x+9].width,jason[x+9].height, &jason[x+9],0,0, BLIT_SPRITE);
   SVGA_ShowImage(&img,243,141,BLIT_NORMAL);
   delay(150);
   i--;
   c.red = c.green = c.blue = i;
   SVGA_SetPalette(255,&c);
  }

 for (i=10;i<14;i++)
  {
   SVGA_MergeImage(&img,0,0,img.width,img.height, &bg, 243,141, BLIT_NORMAL);
   SVGA_MergeImage(&img,0,0,jason[i].width,jason[i].height, &jason[i],0,0, BLIT_SPRITE);
   SVGA_ShowImage(&img,243,141,BLIT_NORMAL);
   delay(250);
  }

 for (scale=16;scale>1;scale--) // meteor cometh!
  {
   SVGA_MergeImage(&work,0,0,work.width,work.height, &bg, 0,0, BLIT_NORMAL);
   SVGA_RSImage(&meteor,&work,320,240,0,scale<<16);
   SVGA_ShowImage(&work,0,0,BLIT_NTN);
   if (KeyFlags[KEY_SPACE]) break;
   delay(300);
  }
 for (scale=13;scale>8;scale--)
  {
   SVGA_MergeImage(&work,0,0,work.width,work.height, &bg, 0,0, BLIT_NORMAL);
   SVGA_RSImage(&meteor,&work,320,240,0,16<<scale);
   SVGA_ShowImage(&work,0,0,BLIT_NTN);
   if (KeyFlags[KEY_SPACE]) break;
   delay(300);
  }

 fadeout();
 while (fade_state==OUT) {}

 SVGA_KillImage(&bg);
 SVGA_KillImage(&meteor);
 for (i=0;i<14;i++)
  SVGA_KillImage(&jason[i]);
 SVGA_KillImage(&img);
 if (KeyFlags[KEY_SPACE]) return;
 
 SVGA_ClearPalette();
 SVGA_MergeImage(&work,0,0,work.width,work.height, &bg, 0,0, BLIT_NORMAL);
 SVGA_RSImage(&meteor,&work,320,240,0,16384);
 SVGA_ShowImage(&work,0,0,BLIT_NTN);

 SVGA_LoadPCX("earth.pcx",&bg);
 SVGA_LoadPCX("intrmet1.pcx",&meteor);
 fadein(&bg.palette);

 for (i=0;i<340;i+=2)
  {
   SVGA_MergeImage(&work,0,0,work.width,work.height, &bg, i,0, BLIT_NORMAL);
   SVGA_RSImage(&meteor,&work,320,240,(i*2)%256,16384);
   SVGA_ShowImage(&work,0,0,BLIT_NTN);
   if (KeyFlags[KEY_SPACE]) break;
  }
 scale = 16384;
 x=320;
 fx = 320<<16;
 fxs = 215883;
 
 for (i=240;i>70;i-=2)    //     (i-70)    sc
  {                       //     ------  = --
   fx+=fxs;              //      170      16
   x=(fx>>16);
   SVGA_MergeImage(&work,0,0,work.width,work.height, &bg, 340,0, BLIT_NORMAL);
   scale += scale/64;
   SVGA_RSImage(&meteor,&work,x,i,((240-i+84)*2)%256,scale);
   SVGA_ShowImage(&work,0,0,BLIT_NTN);
   if (i==220)
    fadeout();
   if (KeyFlags[KEY_SPACE]) break;
  }
 if (i<220)
  fadeout();
 SVGA_KillImage(&meteor);
 SVGA_KillImage(&bg);
 while (fade_state==OUT) {}

}

void timerproc()
{
 NOBINT16 pc;
 
 if (fade_state==IN)
  {
   for (pc=0;pc<256;pc++)
    {
     if (fade_level >= (63-fade_pal.color[pc].red))
      fade_counter.color[pc].red++;
     if (fade_level >= (63-fade_pal.color[pc].green))
      fade_counter.color[pc].green++;
     if (fade_level >= (63-fade_pal.color[pc].blue))
      fade_counter.color[pc].blue++;
     SVGA_SetPalette(pc, &fade_counter.color[pc]);
    }
   fade_level++;
   if (fade_level > 63)
    {
     fade_state = NONE;
     fade_level = 0;
    }
  }
 else if (fade_state==OUT)
  {
   for (pc=0;pc<256;pc++)
    {
     if (fade_counter.color[pc].red)
      fade_counter.color[pc].red--;
     if (fade_counter.color[pc].green)
      fade_counter.color[pc].green--;
     if (fade_counter.color[pc].blue)
      fade_counter.color[pc].blue--;
     SVGA_SetPalette(pc, &fade_counter.color[pc]);
    }
   fade_level++;
   if (fade_level > 63)
    {
     fade_state = NONE;
     fade_level = 0;
    }
  }
}


void mirror_image(IMAGE_PTR i)
{
 NOBINT8 *to, *from;
 NOBINT32 w,h;
 void asm_reverse();
 #pragma aux asm_reverse =               \
   "mov edi, near ptr [to]"             \
   "mov esi, near ptr [from]"           \
   "std"                                \
   "mov edx, h"                         \
   "mov ebx, w"                         \
   "add esi, ecx"                       \
   "lblstart:"                          \
   "mov ecx, ebx"                       \
   "lbllp:"                             \
   "movsb"                              \
   "add edi, 2"                         \
   "loop lbllp"                         \
   "add esi, ebx"                       \
   "add esi, ebx"                       \
   "dec edx"                            \
   "cmp edx, 0"                         \
   "jnz lblstart"                       ;

 
 from = (NOBINT8 *)i->hMem;
 to = (NOBINT8 *)malloc(i->width*i->height);
 i->hMem = (NOBINT8 *)to;
 w = i->width;
 h = i->height;

 asm_reverse();

 free(from);
}
/*
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
 
 now = *clock;
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
*/
