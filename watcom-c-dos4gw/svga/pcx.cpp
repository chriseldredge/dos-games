#include "toolkit.h"

void PCX_Run(STR *arg);


class LISTBOXF
{
 public:
         LISTBOX(BINT16 nx, BINT16 ny, BINT16 nw, BINT16 nh, BINT16 ni, ...);
  VBINT  update(BINT16 i);
  VBINT  show(BINT16 flag);
  VBINT  clearlist(VBINT);
  VBINT  additem(STR *ni);
  VBINT  move(BINT16 stp);
  BINT16 check();
   
  BINT16    x,y,w,h,offset;
  BINT16    numitems, curitem, olditem;
  STR       items[150][50];
};

class SCROLLBAR
{
 public:
  VBINT     reset(BINT16 nx,BINT16 ny,BINT16 l,BINT16 d, BINT16 m);
  VBINT     show(BINT16 h);
  BINT16    check();
  VBINT     setfunction(BINT16 ft, BINT16 *ptr);
  BINT16    functype;
  LISTBOXF  *listfunc;
  BINT16    x,y,dir, len, max, cur;
};

class LISTBOX
{
 public:
         LISTBOX(BINT16 nx, BINT16 ny, BINT16 nw, BINT16 nh, BINT16 ni, ...);
  VBINT  update(BINT16 i);
  VBINT  show(BINT16 flag);
  VBINT  clearlist(VBINT);
  VBINT  additem(STR *ni);
  VBINT  move(BINT16 stp);
  BINT16 check();

  BINT16    x,y,w,h,offset;
  BINT16    numitems, curitem, olditem;
  STR       items[150][50];
 SCROLLBAR scrolly;
};

VBINT SCROLLBAR::reset(BINT16 nx,BINT16 ny,BINT16 l,BINT16 d, BINT16 m)
{
 x=nx, y=ny, len=l, max=m, dir=d;
}

VBINT SCROLLBAR::show(BINT16 h)
{
 BUTTON b(x+1,y+1,10,10, "");
 BINT16 ors;

 ors=CurrentFont->romset;
 CurrentFont->romset = 1;

 if (dir)
  {
   switch (BytesPerPixel)
    {
     case 1: SVGA_Rectangle(x,y+12,x+12,y+len+12,252,1); break;
     case 2: SVGA_Rectangle(x,y+12,x+12,y+len+12,33808,1); break;
     case 4: SVGA_Rectangle(x,y+12,x+12,y+len+12,8421504,1);
    }                                        
   if (BytesPerPixel==1)
    SVGA_Rectangle(x,y,x+12,y+len+24,249,0);
   else
    SVGA_Rectangle(x,y,x+12,y+len+24,1,0);
   if (h==1)
    b.show(0);
   else if (h==0)
    b.show(1);

   b.reset(x+1, y+len+13, 10, 10, "");
   if (h==2)
    b.show(0);
   else if (h==0)
    b.show(1);

   if ((cur*len/max+12)>len)
    {
     b.reset(x+1, y+len+2, 10, 10, "");
    }
   else
    b.reset(x+1, y+cur*len/max+12, 10, 10, "");
   
   b.show(1);
  }
 CurrentFont->romset = ors;
}

BINT16 SCROLLBAR::check()
{
 BUTTON b(x+1,y+1,10,10, "");         //
 BINT16 ors;

 ors=CurrentFont->romset;
 CurrentFont->romset = 1;

 if (ML)
  {
   if (b.down())
    {
     while (ML)
      {
       if (cur>0)
        {
         cur--; show(1);
         CurrentFont->romset = ors;
         switch (functype)
          {
           case 1: listfunc->update(cur);
          }
         CurrentFont->romset = 1;
        }
      }
     show(0);
    }
   b.reset(x+1,y+len+13,10,10, "");   
   if (b.down())
    {
     while (ML)
      {
       if (cur<max)
        {
         cur++;
         show(2);
         CurrentFont->romset = ors;
         switch (functype)
          {
           case 1: listfunc->update(cur);
          }
         CurrentFont->romset = 1;
        }
      }
     show(0);
    }
  }
 CurrentFont->romset = ors;
 return(cur);
}
VBINT SCROLLBAR::setfunction(BINT16 ft, BINT16 *ptr)
{
 functype = ft;
 switch (ft)
  {
   case 1: listfunc = (LISTBOXF *)ptr; break;
  }
}


LISTBOX::LISTBOX (BINT16 nx, BINT16 ny, BINT16 nw, BINT16 nh, BINT16 ni, ...)
{
 va_list v;
 BINT16  i;

 numitems = ni; x = nx, y=ny, w=nw-14, h=nh; offset = 0;

 va_start(v, ni);
 for (i=0;i<ni;i++)
  strcpy(items[i], (const char *)va_arg(v, const char));
 scrolly.reset(x+w, y, h-24, 1, numitems);

 va_end(v);
}

VBINT LISTBOXF::update(BINT16 i)
{
 offset = i;
 if (curitem<offset)
  {
   olditem=offset;
   curitem=offset;
  }
 else if (curitem>=(offset+((h-2)/TH)))
  {
   curitem=offset+((h-2)/TH)-1;
   olditem=offset+((h-2)/TH)-1;
  }
 show(24);
 delay(150);
}

VBINT LISTBOX::move(BINT16 stp)
{
 if (((curitem+stp)>=0)&&((curitem+stp)<numitems))
  {
   olditem=curitem;
   curitem += stp;

   if (curitem>=(offset+(h-2)/TH))
    {
     offset=curitem-((h-2)/TH)+1;
     scrolly.cur = offset;
     show(1);
    }
   else if (curitem<offset)
    {
     offset=curitem;
     scrolly.cur = offset;
     show(1);
    }
   else
    {
     show(0);
    }
   delay(100);
  }
}

VBINT LISTBOXF::show(BINT16 flag)
{
 BINT16 i;

 if (flag)
  {
    for (i=0; i<(h-2)/TH; i++)
     {
      SVGA_Rectangle(x+1,y+i*TH+1,x+w-1,y+(i+1)*TH,16777215,1);
      if (BytesPerPixel==1)
       SVGA_PrintString(x+1, y+1+(i*TH), 0, items[i+offset], 0);
      else
       SVGA_PrintString(x+1, y+1+(i*TH), 1, items[i+offset], 0);
      
      if (i>=numitems) break;
     }
   SVGA_Rectangle(x+1,y+i*TH+1,x+w-1,y+h,16777215,1);
   switch(BytesPerPixel)
    {
     case 1:
      SVGA_Rectangle(x,y+((curitem-offset)*TH),x+w-1,y+((curitem-offset)*TH)+TH,LightRed,1);
      SVGA_PrintString(x+1, y+1+(curitem-offset)*TH, 0, items[curitem], 0);
      SVGA_Rectangle(x,y,x+w+12,y+h,249,0);
      break;
     case 2:
      SVGA_Rectangle(x,y+((curitem-offset)*TH),x+w-1,y+((curitem-offset)*TH)+TH,63488,1);
      SVGA_PrintString(x+1, y+1+(curitem-offset)*TH, 0, items[curitem], 0);
      SVGA_Rectangle(x,y,x+w+12,y+h,1,0);
      break;
     case 4:
      SVGA_Rectangle(x,y+((curitem-offset)*TH),x+w-1,y+((curitem-offset)*TH)+TH,16711680,1);
      SVGA_PrintString(x+1, y+1+(curitem-offset)*TH, 0, items[curitem], 0);
      SVGA_Rectangle(x,y,x+w+12,y+h,1,0);
    }
  }
 else
  {
   switch (BytesPerPixel)
    {
     case 1:
      SVGA_Rectangle(x+1,y+((olditem-offset)*TH),x+w-1,y+((olditem-offset)*TH)+TH,255,1);
      SVGA_Rectangle(x+1,y+((curitem-offset)*TH),x+w-1,y+((curitem-offset)*TH)+TH,LightRed,1);
      SVGA_PrintString(x+1, y+1+(curitem-offset)*TH, 0, items[curitem], 0);
      SVGA_PrintString(x+1, y+1+(olditem-offset)*TH, 0, items[olditem], 0);
      break;
     case 2:
      SVGA_Rectangle(x+1,y+((olditem-offset)*TH),x+w-1,y+((olditem-offset)*TH)+TH,65535,1);
      SVGA_Rectangle(x+1,y+((curitem-offset)*TH),x+w-1,y+((curitem-offset)*TH)+TH,63488,1);
      SVGA_PrintString(x+1, y+1+(curitem-offset)*TH, 1, items[curitem], 0);
      SVGA_PrintString(x+1, y+1+(olditem-offset)*TH, 1, items[olditem], 0);
      break;
     case 4:                                                                      
      SVGA_Rectangle(x+1,y+((olditem-offset)*TH),x+w-1,y+((olditem-offset)*TH)+TH,16777215,1);
      SVGA_Rectangle(x+1,y+((curitem-offset)*TH),x+w-1,y+((curitem-offset)*TH)+TH,16711680,1);
      SVGA_PrintString(x+1, y+1+(curitem-offset)*TH, 1, items[curitem], 0);
      SVGA_PrintString(x+1, y+1+(olditem-offset)*TH, 1, items[olditem], 0);
    }
  }
}

VBINT LISTBOX::clearlist(VBINT)
{
 offset=0;
 numitems = 0;
 curitem = 0;
 show(1);
 scrolly.reset(x+w, y, h-20, 1, 1);
}

VBINT LISTBOX::additem(STR *ni)
{
 strcpy(items[numitems], (const char *)ni);
 numitems++;
 scrolly.reset(x+w, y, h-24, 1, numitems-((h-2)/TH));
}

VBINT LISTBOX::show(BINT16 flag)
{
 BINT16 i;

 if (flag)
  {
    for (i=0; i<(h-2)/TH; i++)
     {
      SVGA_Rectangle(x+1,y+i*TH+1,x+w-1,y+(i+1)*TH,16777215,1);
      if (BytesPerPixel==1)
       SVGA_PrintString(x+1, y+1+(i*TH), 0, items[i+offset], 0);
      else
       SVGA_PrintString(x+1, y+1+(i*TH), 1, items[i+offset], 0);
  
      if (i>=numitems) break;
     }
   SVGA_Rectangle(x+1,y+i*TH+1,x+w-1,y+h,16777215,1);
   switch(BytesPerPixel)
    {
     case 1:
      SVGA_Rectangle(x,y+((curitem-offset)*TH),x+w-1,y+((curitem-offset)*TH)+TH,LightRed,1);
      SVGA_PrintString(x+1, y+1+(curitem-offset)*TH, 0, items[curitem], 0);
      SVGA_Rectangle(x,y,x+w+12,y+h,249,0);
      break;
     case 2:
      SVGA_Rectangle(x,y+((curitem-offset)*TH),x+w-1,y+((curitem-offset)*TH)+TH,63488,1);
      SVGA_PrintString(x+1, y+1+(curitem-offset)*TH, 0, items[curitem], 0);
      SVGA_Rectangle(x,y,x+w+12,y+h,1,0);
      break;
     case 4:
      SVGA_Rectangle(x,y+((curitem-offset)*TH),x+w-1,y+((curitem-offset)*TH)+TH,16711680,1);
      SVGA_PrintString(x+1, y+1+(curitem-offset)*TH, 0, items[curitem], 0);
      SVGA_Rectangle(x,y,x+w+12,y+h,1,0);
    }

   if (flag!=24)
    scrolly.show(0);
  }
 else
  {
   switch (BytesPerPixel)
    {
     case 1:
      SVGA_Rectangle(x+1,y+((olditem-offset)*TH),x+w-1,y+((olditem-offset)*TH)+TH,255,1);
      SVGA_Rectangle(x+1,y+((curitem-offset)*TH),x+w-1,y+((curitem-offset)*TH)+TH,LightRed,1);
      SVGA_PrintString(x+1, y+1+(curitem-offset)*TH, 0, items[curitem], 0);
      SVGA_PrintString(x+1, y+1+(olditem-offset)*TH, 0, items[olditem], 0);
      break;
     case 2:
      SVGA_Rectangle(x+1,y+((olditem-offset)*TH),x+w-1,y+((olditem-offset)*TH)+TH,65535,1);
      SVGA_Rectangle(x+1,y+((curitem-offset)*TH),x+w-1,y+((curitem-offset)*TH)+TH,63488,1);
      SVGA_PrintString(x+1, y+1+(curitem-offset)*TH, 1, items[curitem], 0);
      SVGA_PrintString(x+1, y+1+(olditem-offset)*TH, 1, items[olditem], 0);
      break;
     case 4:                                                                      
      SVGA_Rectangle(x+1,y+((olditem-offset)*TH),x+w-1,y+((olditem-offset)*TH)+TH,16777215,1);
      SVGA_Rectangle(x+1,y+((curitem-offset)*TH),x+w-1,y+((curitem-offset)*TH)+TH,16711680,1);
      SVGA_PrintString(x+1, y+1+(curitem-offset)*TH, 1, items[curitem], 0);
      SVGA_PrintString(x+1, y+1+(olditem-offset)*TH, 1, items[olditem], 0);
    }
  }
}

BINT16 LISTBOX::check()
{
 if (ML)
  {
   if ((MX>x)&&(MX<x+w)&&(MY>y)&&(MY<y+((h-2)/TH)*TH))
    {
     olditem=curitem;
     curitem = (MY-y+1)/TH + offset;
     if (curitem>=numitems)
      curitem=numitems-1;
     
     while (ML) {}
     show(0);
    }
   else scrolly.check();
  }
 return(curitem);
}


FONT tnr, small;


void main(int argc, char **argv)
{
 SVGA_Init();

 if (SVGA_SetupMode(SVGA_GetUserMode(MODE_16_BIT|MODE_32_BIT)))
  {
   printf("\nThat mode is not supported by your video card!!\n");
   return;
  }

 KEYBOARD_Init();

 MOUSE_Init();
 MOUSE_Show();

 SVGA_ConvertPalette("256color.pal");
 SVGA_LoadFont("tnr.mff", &tnr);
 SVGA_LoadFont("8pt.mff", &small);
 SVGA_SetFont(&tnr);

 if (argc<2)
  PCX_Run("*.PCX");
 else
  PCX_Run(argv[1]);

 KEYBOARD_Done();

 MOUSE_Done();
 VESA_SetMode(TEXT_MODE);
}

void PCX_Run(STR *arg)
{
 BUTTON    buttonshow(0,0,0,0,"Show");
 BUTTON    buttonexit(0,0,0,0,"Exit");
 BUTTONBAR buttonbar(2, &buttonshow, &buttonexit);
 FORM      form(MaxX-240,MaxY/2-1,240,MaxY/2,0,1,1,"PCX Viewer", &buttonbar);
 LISTBOX   list(form.fx+4,form.fy+4,220,form.fh-6,0,"");
 IMAGE     pcx, foreground;
 BINT16    totalfiles, count, buttonpush, first;
 STR       filename[120];

 totalfiles = SearchFile(arg, ".PCX");

 SVGA_SetFont(&small);
 for (count=0;count<totalfiles;count++)
  {
   strcpy(filename,(const char *)FileList[count]);
   list.additem(filename);
  }
 list.scrolly.setfunction(1, (BINT16 *)&list);
 SVGA_SetFont(&tnr);

 form.show();
 SVGA_SetFont(&small);
 list.show(1);
 SVGA_SetFont(&tnr);
 SVGA_GetImage(&foreground, 0,0,MaxX, MaxY);

 first = 1;
 buttonpush = NONE;

 while (buttonpush!=1)
  {
   buttonpush = buttonbar.check();
   SVGA_SetFont(&small);
   list.check();

   if (KeyFlags[KEY_ENTER])
    buttonpush = 0;
   else if (KeyFlags[KEY_ESC])
    buttonpush=1;
   else if (KeyFlags[KEY_UP])
    list.move(-1);
   else if (KeyFlags[KEY_DOWN])
    list.move(1);
   else if (KeyFlags[KEY_SPACE])
    {
     while (KeyFlags[KEY_SPACE]) {}
     form.hide();
     while (!KeyFlags[KEY_SPACE]) {}
     SVGA_SetFont(&tnr);
     form.show();
     SVGA_SetFont(&small);
     list.show(1);
    }

   if (buttonpush == 0)
    {
     if (!first)
      {
       SVGA_KillImage(&pcx);
       first = 0;
      }
     list.check();
    
     strcpy(filename,(const char *)FilePath);
     strcat(filename,(const char *)FileList[list.curitem]);
     SVGA_LoadPCX(filename, &pcx);

     SVGA_SetForeground(&foreground, 0);
     SVGA_KillImage(&foreground);

     form.hide();                               // kill memory
     SVGA_Cls(0);
     SVGA_SetFont(&tnr);
     form.show();
     SVGA_SetFont(&small);
     list.show(1);
     SVGA_ClearForeground(1);
     SVGA_GetImage(&foreground, 0,0,MaxX, MaxY);
     SVGA_SetForeground(&foreground, 1);
     form.hide();                               // kill memory
     if (BytesPerPixel==1)
      SVGA_SetAllPalette(&pcx.palette);
     SVGA_ShowImage(&pcx, 0, 0, BLIT_NORMAL);
     SVGA_SetFont(&tnr);
     form.show();                               // put back on screen
     SVGA_SetFont(&small);
     list.show(1);
     SVGA_ClearForeground(1);
    }
   SVGA_SetFont(&tnr);
  }
 form.hide();
}

