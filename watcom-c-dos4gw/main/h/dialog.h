#ifdef __cplusplus    // Oop only! (Don't let C try to include CPP classes)

#define SET_AT_RUN              256
#define NONE                    256
#define CCB                     255
#define SELECT_ONE              0
#define SELECT_MUL              1

#define OK              0
#define OK_CANCEL       1
#define YES_NO          2
#define YES_NO_CANCEL   3
#define RETRY_CANCEL    4

#define BTN_UP          1
#define BTN_DOWN        0
#define BTN_FLAT        2

BINT16 msgbox(STR *caption, STR *msg, BINT16 bt, FONT_PTR f1, FONT_PTR f2);

class BUTTON
 {
  public:
                  BUTTON(BINT16 sx, BINT16 sy, BINT16 sw, BINT16 sh, STR *sc, BINT16 nhk);
          VBINT   show(BINT8 son);
          BINT16  check(VBINT);
          BINT16  down(VBINT);
          BINT16  inrange(VBINT);
          VBINT   reset(BINT16 sx, BINT16 sy, BINT16 sw, BINT16 sh, STR *sc, BINT16 nhk);
          STR     caption[50];
          BINT16  x, y, w, h, state, hotkey, keydown;
          BINT8   on;
 };
 
class BUTTONBAR                      // Contains multiple buttons
 {
  public:
                  BUTTONBAR(BINT8 nb, ...);
          VBINT   init(BINT16 nx, BINT16 ny, BINT16 nw, BINT16 nh);
          BINT16  add(BINT8 nb, ...);
          VBINT   space(VBINT);
          VBINT   show(VBINT);
          BINT16  check(VBINT);
  private:
          BINT8   numbuts;
          BINT16  x,y,w,h;
          BUTTON  *but[MAX_BUTS];
 };

class FORM                        // Form structure for uhh, forms!
 {
  public:
                  FORM(BINT16 nx, BINT16 ny, BINT16 nw, BINT16 nh, BINT16 center, BINT16 usecap, BINT16 usebar, ...);
          VBINT   init(BINT16 nx, BINT16 ny, BINT16 nw, BINT16 nh, BINT16 center, BINT16 usecap, BINT16 usebar, ...);
          VBINT   show(VBINT);
          VBINT   hide(VBINT);
          BINT16  fx, fy, fw, fh;      // Window intended for your graphics
          BINT16  x,y,w,h;             // Basic x,y, width and height
  private:
          IMAGE     backup;
          STR       caption[500];
          BUTTONBAR *bar;                // Set pointer to your buttonbar if you want one
 };

class MENU           
 {
  public:
                  MENU(BINT16 x, BINT16 y, BINT16 center, BINT16 usecap, BINT16 ni, ...);
          VBINT   init(BINT16 x, BINT16 y, BINT16 center, BINT16 usecap, BINT16 ni, ...);
          VBINT   show(VBINT);
          VBINT   setxy(BINT16 sx, BINT16 sy);
          VBINT   center(VBINT);
          VBINT   show(VBINT);
          VBINT   hide(VBINT);
          BINT16  go(VBINT);
          BINT16  x,y, w, h;
  private:
          IMAGE   backup;
          BINT16  num_items,longstr;
          STR     title[500];
          STR     item[25][100];
 };

class OPTION
 {
  public:
                  OPTION(BINT16 sx, BINT16 sy, STR *cap);
          VBINT   reset(BINT16 sx, BINT16 sy, STR *cap);
          VBINT   show(BINT16 on);
          BINT16  check(VBINT);
          STR     caption[500];
  private:
          BINT16  x,y;
          BINT16  i;
 };
 
class OPTIONGROUP
 {
  public:
                  OPTIONGROUP(BINT16 sx, BINT16 sy, BINT16 md, BINT16 od, BINT16 no, ...);
          VBINT   reset(BINT16 sx, BINT16 sy, BINT16 md, BINT16 od, BINT16 no, ...);
          VBINT   show(VBINT);
          BINT16  check(VBINT);
          VBINT   setcap(BINT16 n, STR *c);
  private:
          BINT16  csm;
          BINT16  x,y;
          BINT16  status;
          BINT16  mode;
          BINT16  nops;
          OPTION  *op[10];
 };

class MENUBAR
 {
  public:
                  MENUBAR(BINT16 sx, BINT16 sy, BINT16 sw, BINT16 sh, BINT16 nm, ...);
          VBINT   reset(BINT16 sx, BINT16 sy, BINT16 sw, BINT16 sh, BINT16 nm, ...);
          VBINT   show(VBINT);
          BINT16  go(BINT16 *sm, BINT16 *cs);
  private:
          BINT16  x,y,w,h, num_menu;
          BUTTON  *but[10];
          MENU    *menu[10];
 };

class TEXTBOX
 {
  public:
                  TEXTBOX(BINT16 tx, BINT16 ty, BINT16 on, STR *initstr, BINT16 mc);
          VBINT   reset(BINT16 tx, BINT16 ty, BINT16 on, STR *initstr, BINT16 mc);
          VBINT   show(VBINT);
          BINT16  checkfocus(VBINT);
          BINT16  setfocus(BINT16 f);
          BINT16  go(VBINT);
          VBINT   settext(STR *newstr);
          STR     *gettext(VBINT);
  private:
          BINT16  x,y,w,h, hasfocus, max;
          STR     text[500];
 };
 
#ifdef DCPP
  BUTTON b1(0,0,0,0,"",NONE),
        b2(0,0,0,0,"",NONE),
        b3(0,0,0,0,"",NONE),
        b4(0,0,0,0,"",NONE),
        b5(0,0,0,0,"",NONE),
        b6(0,0,0,0,"",NONE),
        b7(0,0,0,0,"",NONE),
        b8(0,0,0,0,"",NONE),
        b9(0,0,0,0,"",NONE),
        b10(0,0,0,0,"",NONE);
#endif
#endif
