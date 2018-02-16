#include "toolkit.h"

void show_how_easy_it_is_to_make_a_form(void);
void show_how_easy_it_is_to_make_a_menu(void);
void show_how_easy_it_is_to_make_an_option_group(void);

void main()
{
 PALETTE pal;
 FONT    fnt;

 SVGA_SetupMode(SVGA_640x480);

 SVGA_LoadPalette("256COLOR.PAL", (PALETTE_PTR)&pal);
 SVGA_SetAllPalette(&pal);
 SVGA_LoadFont("TNR.MFF", &fnt);
 SVGA_SetFont(&fnt);

 MOUSE_Init();
 MOUSE_Show();

 show_how_easy_it_is_to_make_a_form();

 MOUSE_Done();
 VESA_SetMode(TEXT_MODE);
}

void show_how_easy_it_is_to_make_a_form(void)
{
 BUTTON         b1(0,0,0,0,"Hi Guys");          // Declare vars
 BUTTON         b2(0,0,0,0,"Hey man");
 BUTTON         b3(0,0,0,0,"exit");
 BUTTONBAR      bb(3, &b1, &b2, &b3);
 FORM           f(0,0,500,300,1,1,1,"This is a test",&bb);
 BINT16         i;
 
 f.show();                                     // show the form
 
 while (((i=bb.check())!=2)&&(!kbhit())) if (i==1) show_how_easy_it_is_to_make_a_menu();        // get input

 f.hide();                                     // hide the form
}

void show_how_easy_it_is_to_make_a_menu(void)
{
 MENU   m(50,50,0,1,5, "This is the title", "Menu1", "How Are Your Face?", "Options", "Snow...", "Exit");
 BINT16 x=NONE;
 
 m.show();

 while(x!=4)
  {
   x=m.go();
   if (x==2) show_how_easy_it_is_to_make_an_option_group();
  } 

 m.hide();
}

void show_how_easy_it_is_to_make_an_option_group(void)
{
 OPTION         o1(0,0,"dont read this");
 OPTION         o2(0,0,"Oww my neck");
 OPTION         o3(0,0,"Push to quit");
 OPTION         o4(0,0,"Hey...");
 OPTION         o5(0,0,"Secret snake");
 OPTIONGROUP    og(400,200,SELECT_MUL,5,&o1,&o2,&o3,&o4,&o5);
 BUTTON         b(400, 150, TW("Run that thayng...")+2, TH+2, "Run that thayng...");

 b.show(1);
 og.show();

 forever
  {
   while (!b.check()) {if (kbhit()) break; og.check(); }
   if (o3.check()) break;
   if (kbhit()) break;
  }
}
