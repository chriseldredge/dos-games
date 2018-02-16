#define DCPP
#include "toolkit.h"

//////////////////////////////////////////////////////////////////////////////////////
// BUTTON Class implimentation                                                      //
//////////////////////////////////////////////////////////////////////////////////////

BUTTON::BUTTON(BINT16 sx, BINT16 sy, BINT16 sw, BINT16 sh, STR *sc, BINT16 nhk)
{
 // Class constructor //
 x = sx, y = sy, w = sw, h = sh, state = 0;
 strcpy((char *)caption, (const char *)sc);
 hotkey = nhk;
 keydown=0;
 state=0;
}

VBINT BUTTON::show(BINT8 son)
{
 // Displays button (son: 1=Up, 0=Down, 2=Flat)
 
 BINT16 bx, by;
 NOBINT32 c;

 on = son;
 
 switch (BytesPerPixel)
  {
   case 1: c = 252; break;
   case 2: c = 33808; break;
   case 4: c = 8421504;
  }

 SVGA_Rectangle(x,y,x+w,y+h,c,1);

 then (on!=2)
  end
   then (on)
    switch (BytesPerPixel)
     {
      case 1: c = 254; break;
      case 2: c = 50712; break;
      case 4: c = 12632256;
     }
   also
    switch (BytesPerPixel)
     {
      case 1: c = 251; break;
      case 2: c = 16904; break;
      case 4: c = 4210752;
     }


   SVGA_Line(x ,y,x,y+h,c);
   SVGA_HLine(x,x+w,y  ,c);
  
   then (!on)
    switch (BytesPerPixel)
     {
      case 1: c = 254; break;
      case 2: c = 50712; break;
      case 4: c = 12632256;
     }
   also
    switch (BytesPerPixel)
     {
      case 1: c = 251; break;
      case 2: c = 16904; break;
      case 4: c = 4210752;
     }

   SVGA_Line(x+w  ,y+1,x+w  ,y+h,c);
   SVGA_HLine(x+1,x+w,y+h  ,c);
  begin

 bx = x+w/2-TW(caption)/2;
 by = y+(h/2-TH/2);
 then (!on) bx++,by++;
 if (CurrentFont->romset) by++;

 if (BytesPerPixel==1)
  SVGA_PrintString(bx, by, 249, caption, 0);
 else
  SVGA_PrintString(bx, by, 1, caption, 0);
}

BINT16 BUTTON::inrange(VBINT)
{
 if ((MX>x)&&(MX<x+w)&&(MY>y)&&(MY<y+h))
  return(1);

 return(0);
}

BINT16 BUTTON::check(VBINT)
{
 if ((MX>x)&&(MX<x+w)&&(MY>y)&&(MY<y+h))
  {
   if (ML)
    {
     if (on!=0) show(0);
     state=1;
    }
   else if (!ML)
    {
     if (on!=1) show(1);
     if (state) {state=0;return(1);}
    }
  }
 else if (KeyFlags[hotkey])
  {
   if (on!=0) show(0);
   keydown=1;
  }
 else if (keydown)
  {
   if (on!=1) show(1);
   keydown=0;
   return(1);
  }
 else
  {
   if (on!=1) show(1);
   state = 0;
  }

 return(0);
}

BINT16 BUTTON::down(VBINT)
{
 if ((MX>x)&&(MX<x+w)&&(MY>y)&&(MY<y+h))
  {
   if (ML)
    {
     if (on!=0) show(0);
     state=1;
     return(1);
    }
   else if (!ML)
    {
     state=0;
    }
  }
 else
  {
   if (on!=1) show(1);
   state = 0;
  }
  
 return(0);
}
    
VBINT BUTTON::reset(BINT16 sx, BINT16 sy, BINT16 sw, BINT16 sh, STR *sc, BINT16 nhk)
{
 x = sx, y = sy, w = sw, h = sh, state = 0;
 if (w==SET_AT_RUN) w = 2+TW(sc);
 if (h==SET_AT_RUN) h = 2+TH;
 strcpy((char *)caption, (const char *)sc);
 hotkey = nhk;
}

BUTTONBAR::BUTTONBAR(BINT8 nb, ...)
{
 va_list v;
 BINT16  i;
 
 numbuts = nb;
 
 va_start(v, nb);
 for (i=0;i<numbuts;i++)
  but[i] = (BUTTON *)va_arg(v, BUTTON *);

 va_end(v);
}

VBINT BUTTONBAR::init(BINT16 nx, BINT16 ny, BINT16 nw, BINT16 nh)
{
 x = nx, y = ny;
 if (nw==SET_AT_RUN)
  {}
 else
  w=nw;
 if (nh==SET_AT_RUN)
  h = TH+8;
 else
  h = nh;
}

BINT16 BUTTONBAR::add(BINT8 nb, ...)
{
 va_list v;
 BINT16  i;

 va_start(v, nb);
 for (i=numbuts;i<numbuts+nb;i++)
  but[i] = (BUTTON *)va_arg(v, BUTTON *);
 va_end(v);
 numbuts+=nb;

 return(i);
} 

VBINT BUTTONBAR::space(VBINT)
{
 BINT16 totw=0, wb, cw, i;
 
 for (i=0;i<numbuts;i++)
  totw += TW(but[i]->caption)+4;

 wb = (w-totw) / (numbuts);
 cw = 0-(wb/2); 

 for (i=0;i<numbuts;i++)
  {
   but[i]->reset(x+cw+wb, y, TW(but[i]->caption)+4, TH+4, but[i]->caption,but[i]->hotkey);
   cw += wb+TW(but[i]->caption)+4;
  }
}

VBINT BUTTONBAR::show(VBINT)
{
 BINT16 i;
 
 for (i=0;i<numbuts;i++)
  {
   but[i]->show(1);
  }   
}

BINT16 BUTTONBAR::check(VBINT)
{
 BINT16 i;
 for (i=0;i<numbuts;i++)
  {
   if (but[i]->check()) return(i);
  }
 return(NONE);
}

FORM::FORM(BINT16 nx, BINT16 ny, BINT16 nw, BINT16 nh, BINT16 center, BINT16 usecap, BINT16 usebar, ...)
{
 va_list v;
 STR     *s;

 if (center)
  {
   x = MaxX / 2 - nw / 2;
   y = MaxY / 2 - nh / 2;
  }
 else
  {
   x = nx;
   y = ny;
  }
 w = nw; h = nh;
  
 fx = x+5,
 fy = y+5,
 fw = w-10,
 fh = h-10;

 va_start(v, usebar);
 
 if (usecap) 
  {
   s = (STR *)va_arg(v, STR *);
   strcpy((char *)caption, (const char *)s);
   fy += TH+5;
   fh -= (TH+5);
  }
 else
  caption[0]=NULL;
  
 if (usebar)
  {
   bar = (BUTTONBAR *)va_arg(v, BUTTONBAR *);
   bar->init(x+4, y+h-TH-6, w-8, TH+4);
   bar->space();
   fh -= (TH+4);
  }
 else
  bar=(BUTTONBAR *)NULL;

 va_end(v);
 backup.in_use = 0;
}

VBINT FORM::init(BINT16 nx, BINT16 ny, BINT16 nw, BINT16 nh, BINT16 center, BINT16 usecap, BINT16 usebar, ...)
{
 va_list v;
 STR     *s;

 if (center)
  {
   x = MaxX / 2 - nw / 2;
   y = MaxY / 2 - nh / 2;
  }
 else
  {
   x = nx;
   y = ny;
  }
 w = nw; h = nh;
  
 fx = x+5,
 fy = y+5,
 fw = w-10,
 fh = h-10;

 va_start(v, usebar);
 
 if (usecap) 
  {
   s = (STR *)va_arg(v, STR *);
   strcpy((char *)caption, (const char *)s);
   fy += TH+5;
   fh -= (TH+5);
  }
 else
  caption[0]=NULL;
  
 if (usebar)
  {
   bar = (BUTTONBAR *)va_arg(v, BUTTONBAR *);
   bar->init(x+4, y+h-TH-6, w-8, TH+4);
   bar->space();
   fh -= (TH+4);
  }
 else
  bar=(BUTTONBAR *)NULL;

 va_end(v);
 backup.in_use = 0;
}

VBINT FORM::show(VBINT)
{
 BUTTON  b(0,0,0,0,"",NONE);

 SVGA_GetImage(&backup,x,y,w,h);
 
 b.reset(x,y,w,h,"",NONE);
 b.show(1);

 if (caption[0]!=NULL) 
  {
   b.reset(x+4, y+4, w-8, TH+2, caption,NONE);
   b.show(0);
  }

 if (bar!=NULL)
  bar->show();

 b.reset(fx-1,fy-1,fw+2,fh+2,"",NONE);
 b.show(0);
}

VBINT FORM::hide(VBINT)
{
 if (backup.in_use)
  {
   SVGA_ShowImage(&backup, x,y,BLIT_NORMAL);
   SVGA_KillImage(&backup);
  }
}

MENU::MENU(BINT16 sx, BINT16 sy, BINT16 center, BINT16 usecap, BINT16 ni, ...)
{
 va_list v;
 BINT16  i;
 STR     *s;

 va_start(v, ni);
 if (usecap)
  {
   s = (STR *)va_arg(v, STR *);
   strcpy((char *)title, (const char *)s);
  }
 else
  title[0]=NULL;
  
 num_items = ni;
 
 for (i=0;i<ni;i++)
  {
   s = (STR *)va_arg(v, STR *);
   strcpy((char *)item[i], (const char *)s);
  }

 va_end(v);
 
 x=sx, y=sy;
 longstr=TW(title);
 
 for (i=0;i<num_items;i++)
  {
   if (TW(item[i])>longstr) longstr = TW(item[i]);
  }

 w = longstr+10;
 
 h = (num_items)*(TH+2)+8;
 if (title[0]!=NULL)
  h += TH+6;
 
 if (center)
  {
   x = (MaxX/2)-(w/2);
   y = (MaxY/2)-(h/2);
  }
 backup.in_use = 0;
}

VBINT MENU::init(BINT16 sx, BINT16 sy, BINT16 center, BINT16 usecap, BINT16 ni, ...)
{
 va_list v;
 BINT16  i;
 STR     *s;

 va_start(v, ni);
 if (usecap)
  {
   s = (STR *)va_arg(v, STR *);
   strcpy((char *)title, (const char *)s);
  }
 else
  title[0]=NULL;
  
 num_items = ni;
 
 for (i=0;i<ni;i++)
  {
   s = (STR *)va_arg(v, STR *);
   strcpy((char *)item[i], (const char *)s);
  }

 va_end(v);
 
 x=sx, y=sy;
 longstr=TW(title);
 
 for (i=0;i<num_items;i++)
  {
   if (TW(item[i])>longstr) longstr = TW(item[i]);
  }

 w = longstr+10;
 
 h = (num_items)*(TH+2)+8;
 if (title[0]!=NULL)
  h += TH+6;
 
 if (center)
  {
   x = (MaxX/2)-(w/2);
   y = (MaxY/2)-(h/2);
  }
 backup.in_use = 0;
}

VBINT MENU::show(VBINT)
{
 BUTTON b(0,0,0,0,"",NONE);
 BINT16 dy,i;
 
 SVGA_GetImage(&backup, x, y, w, h);

 b.reset(x,y,w,h,"",NONE);
 b.show(1);

 if (title[0]==NULL)
  {
   b.reset(x+3, y+3, w-6, h-6, "",NONE);
   b.show(0);
  }
 else
  {
   b.reset(x+3, y+TH+8, w-6, h-TH-11, "",NONE);
   b.show(0);
   b.reset(x+3, y+3,  w-6, TH+2, title,NONE);
   b.show(0);
  }

 dy = y+4;
 if (title[0]!=NULL)
  dy += TH+5;
  
 for (i=0;i<num_items;i++)
  {
   b.reset(x+4,dy,w-8,TH+2,item[i],NONE);
   b.show(2);
   dy+=TH+2;
  }
}

VBINT MENU::hide(VBINT)
{
 SVGA_ShowImage(&backup, x, y, BLIT_NORMAL);
 SVGA_KillImage(&backup);
}

VBINT MENU::setxy(BINT16 sx, BINT16 sy)
{
 x = sx, y = sy;
}

VBINT MENU::center(VBINT)
{
 BINT16 i;
 
 longstr=TW(title);
 
 for (i=0;i<num_items;i++)
  {
   if (TW(item[i])>longstr) longstr = TW(item[i]);
  }

 w = longstr+10;
 
 h = (num_items)*(TH+2)+9;
 if (title[0]!=NULL)
  h += TH+6;
 
 x = (MaxX/2)-(w/2);
 y = (MaxY/2)-(h/2);
}

BINT16 MENU::go(VBINT)
{
 BUTTON b(0,0,0,0,"",NONE);
 BINT16 tx, ty, tw, th, cs=0xFF, os=0xFF;
 BINT16 guy=0, ms=0;

 th = TH+2;
 tw = w-8;
 tx = x+4;
 ty = y+4;
 if (title[0]!=NULL)
  ty += TH+4;

 forever
  {
   if (KeyFlags[KEY_DOWN])
    cs++;
   else if (KeyFlags[KEY_UP])
    cs--;

   if ((MX>tx)&&(MX<tx+tw)&&(MY>ty)&&(MY<ty+(th*num_items))&&(ML))
    {
     ms = 1;                                    // Mouse down over menu
     cs=(MY-ty-2)/(TH+2);
     if (cs>=num_items) cs=num_items-1;
     guy=0;
    }
   else if ((MX>tx)&&
            (MX<tx+tw)&&
            (MY>ty)&&
            (MY<ty+(th*num_items))&&
            (!ML)&&
            (ms))
    {
     cs=(MY-ty-2)/(TH+2);                       // Mouse up (after being down) over menu
     if (cs>=num_items) cs=num_items-1;
     b.reset(tx,ty+(cs*(TH+2)), tw, th, item[cs],NONE);
     b.show(2);
     return(cs);
    }
   else if ((ms)&&(ML))
    {                                           // Mouse was down on menu but has left menu
     ms=0;                                      // Button still down
     guy=1;
     b.reset(tx,ty+(cs*(TH+2)), tw, th-1, item[cs],NONE);
     b.show(2);
     os=cs=0xFF;
     return(CCB);
    }   
   else if (!ML)                                // Mouse button is not down
    return(NONE);
   else if ((MX<tx)||(MX>tx+tw)||(MY<ty)||(MY>ty+(th*num_items)))
    if ((ML)&&(!guy))                 // Mouse down off the menu without already being down
     return (NONE);
    else
     {}

   if (cs<0)
    cs=num_items-1;
   else if ((cs>=num_items)&&(cs!=0xFF))
    cs=0;

   if (cs!=os)
    {
     if (os!=0xFF) 
      {
       b.reset(tx,ty+(os*(TH+2)), tw, th-1, item[os],NONE);
       b.show(2);
      }
     if (cs!=0xFF)
      {
       b.reset(tx,ty+(cs*(TH+2)), tw, th-1, item[cs],NONE);
       b.show(0);
      }
     delay(100);
    }

    os=cs;

    if (KeyFlags[KEY_ENTER])
     {
      if (os!=0xFF)
       {
        b.reset(tx,ty+(os*(TH+2)), tw, th-1, item[os],NONE);
        b.show(2);
       }
      return(cs);
     }
    else if (KeyFlags[KEY_ESC])
     {
      if (os!=0xFF)
       {
        b.reset(tx,ty+(os*(TH+2)), tw, th-1, item[os],NONE);
        b.show(2);
       }
      return(0xFF);
     }
  }
 return(0xFF); // We should not get here (the above loop is forever)
}

OPTION::OPTION(BINT16 sx, BINT16 sy, STR *cap)
{
 x = sx, y = sy;
 strcpy((char *)caption, (const char *)cap);
}

VBINT OPTION::reset(BINT16 sx, BINT16 sy, STR *cap)
{
 x = sx, y = sy;
 strcpy((char *)caption, (const char *)cap);
}

VBINT OPTION::show(BINT16 on)
{
 BUTTON b(0,0,0,0,"",NONE);

 b.reset(x,y,7,7,"",NONE);
 b.show(on);
 SVGA_PrintString(x+12, y+7/2-TH/2, 0, caption, 0);
 i = on;
}

BINT16 OPTION::check(VBINT)
{
 BUTTON b(0,0,0,0,"",NONE);
 static BINT16 cok=1;

 if ((MX>x)&&(MX<x+7)&&(MY>y)&&(MY<y+7)&&(ML))
  {
   if (cok)
    {
     i = !i;
     b.reset(x,y,7,7,"",NONE);
     b.show(i);
     cok = 0;
     return(!i);
    }
  }

 if (!ML) cok = 1;

 return(!i);
}

OPTIONGROUP::OPTIONGROUP(BINT16 sx, BINT16 sy, BINT16 md, BINT16 od, BINT16 no, ...)
{
 va_list v;
 BINT16 i;
 
 x = sx, y = sy, mode = md, nops = no;
 va_start(v, no);
 for (i=0;i<nops;i++)
  {
   op[i] = (OPTION *)va_arg(v, OPTION *);
   op[i]->reset(sx, sy+i*(TH+2), op[i]->caption);
  }
 va_end(v);
 csm=od;
}
 
VBINT OPTIONGROUP::reset(BINT16 sx, BINT16 sy, BINT16 md, BINT16 od, BINT16 no, ...)
{
 va_list v;
 BINT16 i;
 
 x = sx, y = sy, mode = md, nops = no;
 va_start(v, no);
 for (i=0;i<nops;i++)
  {
   op[i] = (OPTION *)va_arg(v, OPTION *);
   op[i]->reset(sx, sy + i*(TH+2), op[i]->caption);
  }
 va_end(v);
 csm=od;
}

VBINT OPTIONGROUP::show(VBINT)
{
 BINT16 i;

 for (i=0;i<nops;i++)
  op[i]->show(1);
 op[csm]->show(0);
}

BINT16 OPTIONGROUP::check(VBINT)
{
 BINT16 x, i;
 x = 0; 

 if (mode==SELECT_ONE)
  {
   for (i=0;i<nops;i++)
    {
     if (csm!=i)
      x = op[i]->check();

     if (x)
      {
       if ((csm!=NONE))
        op[csm]->show(1);
       csm = i;
       op[csm]->show(0);
      }
    }
   return(csm);
  }
 
 for (i=0;i<nops;i++)
  op[i]->check();
 return(NONE);
}

VBINT OPTIONGROUP::setcap(BINT16 n, STR *c)
{
 strcpy((char *)op[n]->caption, (const char *)c);
}

BINT16 msgbox(STR *caption, STR *msg, BINT16 bt, FONT_PTR f1, FONT_PTR f2)
{
 FONT_PTR       of;
 BUTTON         b1(0,0,0,0,"",NONE);
 BUTTON         b2(0,0,0,0,"",NONE);
 BUTTON         b3(0,0,0,0,"",NONE);
 BUTTONBAR      bb(0);
 FORM           f(0,0,0,0,0,0,0);
 BINT16         capwidth, lines, width, x, l, cx, cy, height, k;
 STR            word[50];
 STR            c;
 
 of = CurrentFont;
 
 if (bt==OK)
  {
   b1.reset(0,0,0,0,"Ok",KEY_O);
   bb.add(1, &b1);
  }
 else if (bt==OK_CANCEL)
  {
   b1.reset(0,0,0,0,"Ok",KEY_O);
   b2.reset(0,0,0,0,"Cancel",KEY_C);
   bb.add(2, &b1, &b2);
  }
 else if (bt==YES_NO)
  {
   b1.reset(0,0,0,0,"Yes",KEY_Y);
   b2.reset(0,0,0,0,"No",KEY_N);
   bb.add(2, &b1, &b2);
  }
 else if (bt==YES_NO_CANCEL)
  {
   b1.reset(0,0,0,0,"Yes",KEY_Y);
   b2.reset(0,0,0,0,"No",KEY_N);
   b3.reset(0,0,0,0,"Cancel",KEY_C);
   bb.add(3, &b1, &b2, &b3);
  }
 else if (bt==RETRY_CANCEL)
  {
   b1.reset(0,0,0,0,"Retry",KEY_R);
   b2.reset(0,0,0,0,"Cancel",KEY_C);
   bb.add(2, &b1, &b2);
  }

 SVGA_SetFont(f1);
 capwidth = TW(caption);

 SVGA_SetFont(f2);

 width = TW(msg);
 lines = 1;
 x=width;
 while (x > (MaxX*3/4))
  {
   lines++;
   x = width/lines;
  }
 
 for (cx=0;cx<strlen(msg);cx++)
  if (msg[cx]==13) lines++;

 height = (lines+2)*TH+10;
 SVGA_SetFont(f1);
 height += 2*TH+20;

 f.init(0,0,x+30,height,1,1,1,caption,&bb);
 f.show();
 cx = f.fx+5; cy = f.fy+5;
 l=0;
 c = '!';

 SVGA_SetFont(f2);
 while (c!=0)
  {
   memset(&word, 0, 50);

   x=l;
   c = '!';
   while ((c!=0) && (c!=' ') && (c!=13))
    c = msg[l++];
   strncpy((STR *)word, (const STR *)(msg+x), l-x);

   k = TW(word);
   if ((k+cx)<(f.fx+f.fw))
    {
     SVGA_PrintString(cx,cy,0,word,0);
     cx+=TW(word)+5;
    }
   else
    {
     cy+=TH;
     cx = f.fx+5;
     SVGA_PrintString(cx,cy,0,word,0);
     cx+=TW(word)+5;
    }
   if (c==13)
    {
     cy+=TH;
     cx = f.fx+5;
    }
  }

 x=NONE;

 SVGA_SetFont(f1);
 while (x==NONE)
  x = bb.check();

 f.hide();
 SVGA_SetFont(of);
 return(x);
}

MENUBAR::MENUBAR(BINT16 sx, BINT16 sy, BINT16 sw, BINT16 sh, BINT16 nm, ...)
{
 va_list v;
 BINT16  i, lx;
 STR     str[500];
 BUTTON  *b;
  
 b = &b1;
 for (i=0;i<10;i++)
  {
   but[i] = b;
   b++;
  }
  
 x = sx, y = sy, w = sw, h = sh, num_menu = nm;

 va_start(v, nm);
 lx = x+2;
 for (i=0;i<nm;i++)
  {
   strcpy((char *)str, (const char *)va_arg(v, const char *));
   but[i]->reset(lx, y+2, TW(str)+4, h-4, str,NONE);
   menu[i] = (MENU *)va_arg(v, MENU *);
   menu[i]->x = lx;
   menu[i]->y = y+h+1;
   lx += TW(str)+8;
  }
 va_end(v);
}

VBINT MENUBAR::reset(BINT16 sx, BINT16 sy, BINT16 sw, BINT16 sh, BINT16 nm, ...)
{
 va_list v;
 BINT16  i, lx;
 STR     str[500];
 BUTTON  *b;
  
 b = &b1;
 for (i=0;i<10;i++)
  {
   but[i] = b;
   b++;
  }
  
 x = sx, y = sy, w = sw, h = sh, num_menu = nm;

 va_start(v, nm);
 lx = x+2;
 for (i=0;i<nm;i++)
  {
   strcpy((char *)str, (const char *)va_arg(v, const char *));
   but[i]->reset(lx, y+2, TW(str)+4, h-4, str,NONE);
   menu[i] = (MENU *)va_arg(v, MENU *);
   menu[i]->x = lx;
   menu[i]->y = y+h+1;
   lx += TW(str)+8;
  }
 va_end(v);
}

VBINT MENUBAR::show(VBINT)
{
 BUTTON b(x,y,w,h,"",NONE);
 BINT16 i;
 
 b.show(1);
 
 for (i=0;i<num_menu;i++)
  but[i]->show(1);
}

BINT16 MENUBAR::go(BINT16 *sm, BINT16 *cs)
{
 BINT16 i,x,j,z;
 
 *sm=NONE;*cs=NONE;
 
   for (i=0;i<num_menu;i++)
    {
     if (ML && but[i]->inrange())
      {
       but[i]->show(0);
       menu[i]->show();

       x = NONE;
       while (ML && ((x==NONE)||(x==CCB)))
        {
         x =  menu[i]->go();

         j =  0;
         for (z=0;z<num_menu;z++)
          if (z!=i) j |= (ML && but[z]->inrange());
         if (j) break;
        }
      
       menu[i]->hide();
       but[i]->show(1);

       if ((x!=CCB)&&(x!=NONE))
        {
         *sm = i, *cs = x;
         return(1);
        }
      }
    }
 return(0);
} 

TEXTBOX::TEXTBOX(BINT16 tx, BINT16 ty, BINT16 on, STR *initstr, BINT16 mc)
{
 x=tx, y=ty, w=8*mc+2, h=8+2, hasfocus=on, max=mc;
 strcpy((char *)text, (const char *)initstr);
}

void TEXTBOX::reset(BINT16 tx, BINT16 ty, BINT16 on, STR *initstr, BINT16 mc)
{
 x=tx, y=ty, w=8*mc+2, h=8+2, hasfocus=on, max=mc;
 strcpy((char *)text, (const char *)initstr);
}

#define FST 175

void TEXTBOX::show(void)
{
 SVGA_Rectangle(x, y, x+w, y+h, 255, 1);
 SVGA_Rectangle(x, y, x+w, y+h, (hasfocus==1) ? FST : 250, 0);

 if (CurrentFont->romset)
  SVGA_PrintString(x+2,y+2,(hasfocus==1) ? FST : 250, text, 0);
 else
  {
   CurrentFont->romset = 1;
   SVGA_PrintString(x+2,y+2,(hasfocus==1) ? FST : 250, text, 0);
   CurrentFont->romset = 0;
  }
}

BINT16 TEXTBOX::checkfocus(VBINT)
{
 if ((MX>=x)&&(MY>=y)&&(MX<x+w)&&(MY<y+h)&&(ML))
  {
   hasfocus = 1;
   show();
   return(1);
  }
 else
  {
   if (hasfocus)
    {
     hasfocus = 0;
     show();
    }
   return(0);
  }
} 

BINT16 TEXTBOX::setfocus(BINT16 f)
{
 hasfocus=f;
 show();
 return(1);
}

BINT16 TEXTBOX::go(VBINT)
{
 STR            tmp, wrk[500];
 BINT16         numchars=0;
 
 while (ML) {}
 KEYBOARD_Done();
 
 numchars=strlen(text);
 strcpy(wrk, text);
   
 forever
  {
   tmp = 0;
   if (kbhit())
    tmp = getch();
   if ((tmp==8))
    {
     if (numchars)
      {
       numchars--;
       SVGA_Rectangle(x+2+8*numchars, y+2, x+9+8*numchars, y+9, 255, 1);
      }
    }
   else if ((tmp==13)||(ML))
    {
     hasfocus=0;
     wrk[numchars] = 0;
     strcpy(text, wrk);
     show();
     KEYBOARD_Init();
     delay(500);
     while (KeyFlags[KEY_ENTER]) {}
     return(1);
    }
   else if (tmp==27)
    {
     show();
     KEYBOARD_Init();
     delay(100);
     while (KeyFlags[KEY_ESC]) {}
     return(0);
    }
   else if (tmp!=0)
    {
     if (numchars==max)
      printf("\7\n");
     else
      {
       if (CurrentFont->romset)
        SVGA_PrintChar(x+2+8*numchars, y+2, FST, tmp, 0);
       else
        {
         CurrentFont->romset = 1;
         SVGA_PrintChar(x+2+8*numchars, y+2, FST, tmp, 0);
         CurrentFont->romset = 0;
        }
       wrk[numchars++] = tmp;
      }
    }
  }
  
 KEYBOARD_Init();
 delay(100);
 while (KeyFlags[KEY_ENTER]) {}
 return(0);
}


VBINT TEXTBOX::settext(STR *newstr)
{
 strcpy((char *)text, (const char *)newstr);
 show();
}

STR *TEXTBOX::gettext(VBINT)
{
 return((STR *)text);
}
