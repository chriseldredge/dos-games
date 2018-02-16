#include <stdio.h>
#include "svga.h"
#include "keyboard.h"

#define Table_X1 187
#define Table_X2 443
#define Table_Y1 17
#define Table_Y2 274

void DrawTable(int CharactorWidth, int CharactorHeight);
void DrawCommands(void);
void SelectCell(int x, int y, int eFlag);
void FillCell(int x, int y, unsigned char Color);
unsigned char GetCellColor(int x, int y);
void DumpTable(unsigned char far *DumpHandle);
void DrawChar(unsigned char far *c);
void MakeBuffer(void);
void KillBuffer(void);
void UnpackTable(int theindex);
void FillAllCells(unsigned char Fill);
void ChangeSize(void);

char string[255];
char OldX, OldY;
char OldIndexX, OldIndexY;
char GenX, GenY;
unsigned char CharIndex;
unsigned char far *Buffer;
int CharWidth, CharHeight;
int EditFlag;
int Scale;

void main()
{
 char CurChar[1024];
 
 EditFlag = 0;

 if (!SVGA_SetupMode(SVGA_640x480))
   {
    printf("\nFailure setting video mode!\n");
    return;
   }

 SVGA_Cls(0);

 DrawTable(8, 8);
 DrawCommands();
 SelectCell(1,1,0);
 EditFlag = !EditFlag;
 SelectCell(1,1,0);

 MakeBuffer();
 SetKb();

 while (Key!=KEY_F5)
  {
   if (Key==KEY_ARROW_DOWN)
        SelectCell(GenX, GenY+1, 1);
   else if (Key==KEY_ARROW_UP)
        SelectCell(GenX, GenY-1, 1);
   else if (Key==KEY_ARROW_RIGHT)
        SelectCell(GenX+1, GenY, 1);
   else if (Key==KEY_ARROW_LEFT)
        SelectCell(GenX-1, GenY, 1);
   else if (Key==KEY_PAGE_UP)
        SelectCell(GenX+1, GenY-1, 1);
   else if (Key==KEY_PAGE_DOWN)
        SelectCell(GenX+1, GenY+1, 1);
   else if (Key==KEY_HOME)
        SelectCell(GenX-1, GenY-1, 1);
   else if (Key==KEY_END)
        SelectCell(GenX-1, GenY+1, 1);
   else if ((Key==KEY_F7)||(Key==KEY_F8))
        FillAllCells(Key);
   else if (Key==KEY_F9)
        ChangeSize();
   else if (Key==KEY_CENTER)
     {
      if (GetCellColor(GenX, GenY)!=4)
        if (EditFlag)
          FillCell(OldX, OldY, 4);
        else
          {}
      else
        if (EditFlag)
          FillCell(OldX, OldY, 0);
     }
   else if (Key==KEY_ENTER)
     {
      if (EditFlag)
        {
         GenX = OldIndexX; GenY = OldIndexY;
        }
      else
        {
         GenX = OldX; GenY = OldY;
        }
      EditFlag = !EditFlag;
     }
   DumpTable((unsigned char far *)&CurChar);
   DrawChar((unsigned char far *)&CurChar);

   Delay(3);
  }
  
 ResetKb();
 KillBuffer();
 VESA_SetMode(TEXT_MODE);
}

void DrawTable(int CharactorWidth, int CharactorHeight)
{
 int x, y;
 
 CharWidth = CharactorWidth;
 CharHeight = CharactorHeight;

 if (CharactorWidth == 8)
   Scale = 32;
 else if (CharactorWidth == 16)
   Scale = 16;
 else if (CharactorWidth == 32)
   Scale = 8;

 for (x=Table_X1;x<Table_X2+1;x+=Scale)
   SVGA_Line(x, Table_Y1, x, Table_Y2, White);
 for (y=Table_Y1;y<Table_Y2;y+=Scale)
   SVGA_HLine(Table_X1, Table_X2, y, White);

 for (x=455;x<625;x+=9)
  SVGA_Line(x, 143, x, 303, Magenta);
 for (y=143;y<304;y+=9)
  SVGA_HLine(455, 615, y, Magenta);
}

void DrawCommands(void)
{
 int x;

 for (x=13;x<627;x+=100)
   SVGA_Line(x, 450, x, 470, LightRed);

 SVGA_HLine(13, 613, 449, LightRed);
 SVGA_HLine(13, 613, 459, LightRed);
 SVGA_HLine(13, 613, 470, LightRed);
 SVGA_PrintString(015, 451, LightGreen, "F1-Help", 0);
 SVGA_PrintString(115, 451, LightGreen, "F2-Save", 0);
 SVGA_PrintString(215, 451, LightGreen, "F3-Save As", 0);
 SVGA_PrintString(315, 451, LightGreen, "F4-Open", 0);
 SVGA_PrintString(415, 451, LightGreen, "F5-Exit", 0);
 SVGA_PrintString(515, 451, LightGreen, "F6-Copy ROM", 0);

 SVGA_PrintString(015, 461, LightGreen, "F7-Fill", 0);
 SVGA_PrintString(115, 461, LightGreen, "F8-Clear", 0);
 SVGA_PrintString(215, 461, LightGreen, "F9-Change Size", 0);
 SVGA_PrintString(515, 461, LightGreen, "F10-", 0);

 SVGA_PrintString(256, 005, White,      "M a k e   F o n t", 0);
 SVGA_PrintString(005, Table_Y1, White, "File:"            , 0);
 SVGA_PrintString(053, Table_Y1, White, "Untitled.asc"     , 1);
}

void SelectCell(int x, int y, int eFlag)
{
 int StartX, StartY;

 if (!EditFlag)
   {
    if (x<1)
     x = 16;
    else if (x>16)
     x = 1;

    if (y<1)
     y = 16;
    else if (y>16)
     y = 1;

    if (eFlag)
     {
      StartX = 455+(OldIndexX*10)-10;
      StartY = 143+(OldIndexY*10)-10;
      SVGA_Rectangle(StartX, StartY, StartX+10, StartY+10, 6, 0);
     }         
 
    StartX = 455+(x*10)-10;
    StartY = 143+(y*10)-10;

    SVGA_Rectangle(StartX, StartY, StartX+10, StartY+10, 7, 0);
    CharIndex = ((y*16)+x)-17;
    OldIndexX = x;
    OldIndexY = y;
   }
 else
   {
    if (x<1)
     x = CharWidth;
    else if (x>CharWidth)
     x = 1;

    if (y<1)
     y = CharHeight;
    else if (y>CharHeight)
     y = 1;

    if (eFlag)
     {
      StartX = Table_X1+(OldX*Scale)-Scale;
      StartY = Table_Y1+(OldY*Scale)-Scale;
      SVGA_Rectangle(StartX, StartY, StartX+Scale, StartY+Scale, White, 0);
     }         
 
    StartX = Table_X1+(x*Scale)-Scale;
    StartY = Table_Y1+(y*Scale)-Scale;

    SVGA_Rectangle(StartX, StartY, StartX+Scale, StartY+Scale, LightGreen, 0);
    OldX = x;
    OldY = y;
   }
 GenX = x;
 GenY = y;
}

void FillCell(int x, int y, unsigned char Color)
{
 int index;
 int StartX, StartY;

 StartX = Table_X1+(x*Scale)-(Scale-1);
 StartY = Table_Y1+(y*Scale)-(Scale-1);

 SVGA_Rectangle(StartX, StartY, StartX+Scale-2, StartY+Scale-1, Color, 1);
}

unsigned char GetCellColor(int x, int y)
{
 int StartX, StartY;

 StartX = Table_X1+(x*Scale)-(Scale-1);
 StartY = Table_Y1+(y*Scale)-(Scale-1);
 return(SVGA_GetPixel(StartX, StartY));
}

void DumpTable(unsigned char far *DumpHandle)
{
 int index;
 int x, y;
 unsigned char mask;
 
 for (y=0;y<CharHeight;y++)
   {
    DumpHandle[y] = 0;
    mask = 0x80;
    for (x=0;x<CharWidth;x++)
      {
       if (GetCellColor(x+1, y+1)!=0)
         DumpHandle[y] |= mask;
       mask = (mask >> 1);
      }
   }
 for (x=0;x<CharWidth;x++);
   Buffer[(CharIndex*CharWidth)+x] = (DumpHandle[x]);
}

void DrawChar(unsigned char far *c)
{
 unsigned char far *old_rom_set;

 SVGA_PrintString(005, Table_Y1+10, White, "Size:", 0);
 sprintf(string, "%2ix%2i", CharWidth, CharHeight);
 SVGA_PrintString(053, Table_Y1+10, White, string, 1);

 sprintf(string, "Current Index: %3i", CharIndex);
 SVGA_PrintString(5, Table_Y1+20, White, string, 1);
 if (EditFlag)
   SVGA_PrintString(5, Table_Y1+30, White, "Current Mode: Edit  ", 1);
 else
   SVGA_PrintString(5, Table_Y1+30, White, "Current Mode: Select", 1);

 sprintf(string, "X=%2i; Y=%2i", OldX, OldY);
 SVGA_PrintString(5, Table_Y1+40, White, string, 1);
 SVGA_PrintString(5, Table_Y1+50, White, "Current Charactor:", 1);
 SVGA_Rectangle(150, Table_Y1+50, 150+CharWidth+1, Table_Y1+50+CharHeight+1, LightRed, 0);

 old_rom_set = rom_char_set;
 rom_char_set = c;
 SVGA_PrintChar(151, Table_Y1+51, 0, Yellow, 1);
 if (EditFlag)
   SVGA_PrintChar(456+((CharIndex%16)*10), 144+((CharIndex/16)*10), 0, LightBlue, 1);
 else
   UnpackTable(CharIndex);
 rom_char_set = old_rom_set;
}
 
void MakeBuffer(void)
{
 char far *old_rom_set;
 int ind;

 if (!(Buffer = (char far *)_fmalloc(2048)))
   SVGA_PrintString(0,0,5, "Error", 1);
    
 for (ind=0;ind<2048;ind++)
   Buffer[ind] = 1+4+16+64;

 old_rom_set = rom_char_set;
 rom_char_set = (char far *)Buffer;

 for (ind=0;ind<255;ind++)
   SVGA_PrintChar(456+((ind%16)*9), 144+((ind/16)*9), (ind), 3, 1);

 rom_char_set = old_rom_set;
}

void KillBuffer(void)
{
 _ffree(Buffer);
}

void UnpackTable(int theindex)
{
 int x, y;
 unsigned char mask;
 char far *work_char;

 work_char = Buffer + theindex * 8;

 for (y=0;y<8;y++)
   {
    for (x=0;x<8;x++)
      {
//       if (GetCellColor(x+1,y+1)!=0)
  //       FillCell(x+1, y+1, 4);
    //   else
//         FillCell(x+1, y+1, 0);

       mask = (mask>>1);
      }
    work_char++;
   }
}
void FillAllCells(unsigned char Fill)
{
 int x, y;
 unsigned char color;

 if (Fill==KEY_F7)
   color = 4;
 else
   color = 0;

 for (y=1;y<CharHeight+1;y++)
   for (x=1;x<CharWidth+1;x++)
     FillCell(x, y, color);
}

void ChangeSize(void)
{
 int NewX, NewY;

 SVGA_Rectangle(220, 145, 420, 245, Blue, 1);
 SVGA_Rectangle(225, 150, 415, 240, LightRed, 1);
 SVGA_PrintString(256, 151, White, "Change Font Size", 0);
 SVGA_PrintString(246, 176, White, "1 = 08x08 (default)", 0);
 SVGA_PrintString(246, 186, White, "2 = 16x16", 0);
 SVGA_PrintString(246, 196, White, "3 = 32x32", 0);

 NewX = 8; NewY = 8;
 while (Key != KEY_ENTER)
   {
    if ((Key==KEY_1)||(Key==KEY_END))
      {NewX=8;NewY=8;
       SVGA_Rectangle(226, 226, 226+(22*8), 236, LightRed, 1);
      }
    else if ((Key==KEY_2)||(Key==KEY_ARROW_DOWN))
      {NewX=16;NewY=16;
       SVGA_Rectangle(226, 226, 226+(22*8), 236, LightRed, 1);
      }
    else if ((Key==KEY_3)||(Key==KEY_PAGE_DOWN))
      {NewX=32;NewY=32;
       SVGA_Rectangle(226, 226, 226+(22*8), 236, LightRed, 1);
      }
    else if (Key==KEY_ESC)
      {SVGA_Rectangle(220, 145, 420, 245, Black, 1);
       DrawTable(CharWidth, CharHeight);
       return;
      }
    sprintf(string, "Current Choice: %2ix%2i", NewX, NewY);
    SVGA_PrintString(226, 226, Magenta, string, 0);
   }

 SVGA_Rectangle(220, 145, 420, 245, Black, 1);
 SVGA_Rectangle(150, Table_Y1+50, 150+CharWidth+1, Table_Y1+50+CharHeight+2, Black, 1);
 SVGA_Rectangle(Table_X1, Table_Y1, Table_X2, Table_Y2, Black, 1);

 DrawTable(NewX, NewY);
 KillBuffer();
 MakeBuffer();
}
