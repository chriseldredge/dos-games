{$A+,B-,D+,E+,F+,G+,I+,L+,N-,O-,P-,Q-,R-,S+,T-,V+,X+}

program slots;

uses dos, crt, vga, menu, pit;

var
 usesound         : boolean;  { tells program whether or not to use sound. }
 soundsrun        : boolean;  { tells program to turn on speaker. }
 timerrun         : boolean;
 numq             : integer;  { how many quarters to start with. }
 winratio         : integer;  { percentage of how often you win. }
 delaylen         : integer;
 minrolls         : integer;
 maxrolls,
 numslots         : integer;
 leastfree,
 mostfree         : longint;
 clickcount,
 clicktotal       : integer;  { number of clicks to wait to make sound. }
 point            : image;    { arrow in main menu. }
 p                : palette;  { used to do palette animation. }
 mach1,mach2,mach3: image;    { the acutal machine. }
 slotimg          : image;    { image containing the neat-o torpedo pictures.}
 slot             : image;    { image work area for each slot. }
 name             : string[8];
 scrname          : array[0..9] of string[8];
 diff,
 score            : array[0..9] of integer;
 numscores,
 hiscore          : integer;
 nextslot,
 rolls,
 state,
 lastslot : array[0..2] of integer;

const
 B = 28;
 C = 32;
 D = 33;
 E = 34;
 F = 35;
 G = 36;
 H = 37;
 I = 38;
 J = 39;
 K = 40;
 L = 41;
 M = 42;              { pascal doesn't acutally use segments, so you have
                        to trick it into finding the address... }
 dummy    : array[0..13] of byte = (0,0,0,0,0,0,0,0,0,0,0,0,0,0);
 pointdat : array[0..15,0..15] of byte =
                ((B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B),
                 (0,0,0,0,0,B,B,B,B,B,B,B,B,B,B,B),
                 (B,0,C,D,E,0,0,B,B,B,B,B,B,B,B,B),
                 (B,B,0,C,D,E,F,0,0,B,B,B,B,B,B,B),
                 (B,B,0,C,D,E,F,G,H,0,0,B,B,B,B,B),
                 (B,B,B,0,C,D,E,F,G,H,I,0,0,B,B,B),
                 (B,B,B,B,0,C,D,E,F,G,H,I,K,0,0,B),
                 (B,B,B,B,0,C,D,E,F,G,H,I,J,L,M,0),
                 (B,B,B,B,0,C,D,E,F,G,H,I,K,0,0,B),
                 (B,B,B,0,C,D,E,F,G,H,I,0,0,B,B,B),
                 (B,B,0,C,D,E,F,G,H,0,0,B,B,B,B,B),
                 (B,B,0,C,D,E,F,0,0,B,B,B,B,B,B,B),
                 (B,0,C,D,E,0,0,B,B,B,B,B,B,B,B,B),
                 (0,0,0,0,0,B,B,B,B,B,B,B,B,B,B,B),
                 (B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B),
                 (B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B));
 SW       = 42; { Slot width }
 SH       = 43; { Guess...   }
 SY       = 55;
 SX       : array[0..2] of integer = (66,111,157);
 PICKLE   = 0;

procedure cycle_int; interrupt;
begin
  vga_cyclepalette(32,42,p,false);
  vga_cyclepalette(50,112,p,false);
  vga_wait;
  vga_setallpalette(p);
  if (memavail < leastfree) then
   leastfree := memavail;
  if (soundsrun) and (usesound) and (clickcount=clicktotal) then
   begin
    sound(500);
    delay(1);
    clickcount := 0;
    nosound;
   end;

  if soundsrun then inc(clickcount);

  clock_ticks := clock_ticks + counter;

  if clock_ticks >= $10000 then
    begin
      clock_ticks := clock_ticks - $10000;
      asm pushf end;
      BIOSTimerHandler;
   end
  else
    Port[$20] := $20;
end;

procedure loadpal;
var
 f  : file;
 i  : integer;
begin
 assign(f, 'colors.bip');
 {$I-}
 reset(f,768);
 {$I+}
 blockread(f, p, 1, i);
 close(f);
end;

procedure configure;
const
 BTN_OK = 0;
 BTN_SOUND = 1;
 BTN_TIME = 2;
var
 frm_cfg : form;
 btn     : array[0..3] of button;
 i       : integer;
 key     : char;
 okhit,
 shift   : boolean;
 str     : string;
begin
 maxrolls := maxrolls*3;
 winratio := winratio div 2;
 frm_cfg.init(0,0,300,150,1,0,'Configuration Options');
 if memavail > 45000 then
  frm_cfg.backup
 else
  begin
   sound(500);
   delay(25);
   nosound;
  end;
 frm_cfg.show;
 btn[BTN_OK].init(frm_cfg.fx + 125, frm_cfg.fy + 125, 40, 10, '&Ok');
 if (usesound) then
  btn[BTN_SOUND].init(frm_cfg.fx + 280, frm_cfg.fy + 5, 10, 10,'&Y')
 else
  btn[BTN_SOUND].init(frm_cfg.fx + 280, frm_cfg.fy + 5, 10, 10,'N');
 if (timerrun) then
  begin
   btn[BTN_TIME].init(frm_cfg.fx + 280, frm_cfg.fy+80, 10, 10, '&Y');
   btn[BTN_TIME].show(BUTTON_DOWN);
  end
 else
  begin
   btn[BTN_TIME].init(frm_cfg.fx + 280, frm_cfg.fy+80, 10, 10, 'N');
   btn[BTN_TIME].show(BUTTON_UP);
  end;

 if timerrun then
  begin
   vga_printstring(frm_cfg.fx + 4, frm_cfg.fy + 8, 'Enable Sound.....................', 0);
   vga_printchar(frm_cfg.fx + 4 + 7*8, frm_cfg.fy + 8, ord('S'), red);
  end
 else
  vga_printstring(frm_cfg.fx + 4, frm_cfg.fy + 8, 'Enable Sound.....................', 26);
 vga_printstring(frm_cfg.fx + 4, frm_cfg.fy + 22, 'Number of quarters at start......', 0);
 vga_printchar(frm_cfg.fx + 84, frm_cfg.fy + 22, ord('q'), red);
 vga_printstring(frm_cfg.fx + 4, frm_cfg.fy + 34, 'Chance of getting a pickle (%)...', 0);
 vga_printchar(frm_cfg.fx + 4+160 , frm_cfg.fy + 34, ord('p'), red);
 vga_printstring(frm_cfg.fx + 4, frm_cfg.fy + 46, 'Minimum number of spins..........', 0);
 vga_printchar(frm_cfg.fx + 4, frm_cfg.fy + 46, ord('M'), red);
 vga_printstring(frm_cfg.fx + 4, frm_cfg.fy + 58, 'Maximum number of spins..........', 0);
 vga_printchar(frm_cfg.fx + 20, frm_cfg.fy + 58, ord('x'), red);
 vga_printstring(frm_cfg.fx + 4, frm_cfg.fy + 70, 'Number of slot types.............', 0);
 vga_printchar(frm_cfg.fx + 124, frm_cfg.fy + 70, ord('t'), red);
 vga_printstring(frm_cfg.fx+ 4, frm_cfg.fy+ 80, 'Enable color animation...........', 0);
 vga_printchar(frm_cfg.fx+60, frm_cfg.fy+80, ord('c'), red);
 vga_printstring(frm_cfg.fx + 30, frm_cfg.fy + 110, '*Hold shift to decrease values*', lightblue);
 { draw the quarters box. }
 vga_rectangle(frm_cfg.fx + 270, frm_cfg.fy + 19, frm_cfg.fx + 290, frm_cfg.fy + 30, black, nofill);
 vga_rectangle(frm_cfg.fx + 271, frm_cfg.fy + 20, frm_cfg.fx + 289, frm_cfg.fy + 29, white, fill);
 str[0] := #2;
 str[1] := chr(48 + numq div 10);
 str[2] := chr(48 + numq mod 10);
 vga_printstring(frm_cfg.fx + 272, frm_cfg.fy + 21, str, black);
 { draw the win ratio box. }
 vga_rectangle(frm_cfg.fx + 270, frm_cfg.fy + 32, frm_cfg.fx + 290, frm_cfg.fy + 42, black, nofill);
 vga_rectangle(frm_cfg.fx + 271, frm_cfg.fy + 33, frm_cfg.fx + 289, frm_cfg.fy + 41, white, fill);
 str[0] := #2;
 str[1] := chr(48 + winratio div 10);
 str[2] := chr(48 + winratio mod 10);
 vga_printstring(frm_cfg.fx + 272, frm_cfg.fy + 34, str, black);
 {draw the spins box}
 vga_rectangle(frm_cfg.fx + 270, frm_cfg.fy + 44, frm_cfg.fx + 290, frm_cfg.fy + 54, black, nofill);
 vga_rectangle(frm_cfg.fx + 271, frm_cfg.fy + 45, frm_cfg.fx + 289, frm_cfg.fy + 53, white, fill);
 str[0] := #2;
 str[1] := chr(48 + minrolls div 10);
 str[2] := chr(48 + minrolls mod 10);
 vga_printstring(frm_cfg.fx + 272, frm_cfg.fy + 46, str, black);
 {draw the maxspins box}
 vga_rectangle(frm_cfg.fx + 270, frm_cfg.fy + 56, frm_cfg.fx + 290, frm_cfg.fy + 66, black, nofill);
 vga_rectangle(frm_cfg.fx + 271, frm_cfg.fy + 57, frm_cfg.fx + 289, frm_cfg.fy + 65, white, fill);
 str[0] := #2;
 str[1] := chr(48 + maxrolls div 10);
 str[2] := chr(48 + maxrolls mod 10);
 vga_printstring(frm_cfg.fx + 272, frm_cfg.fy + 58, str, black);
 {draw the slots box}
 vga_rectangle(frm_cfg.fx + 270, frm_cfg.fy + 68, frm_cfg.fx + 290, frm_cfg.fy + 78, black, nofill);
 vga_rectangle(frm_cfg.fx + 271, frm_cfg.fy + 69, frm_cfg.fx + 289, frm_cfg.fy + 77, white, fill);
 str[0] := #2;
 str[1] := chr(48 + numslots div 10);
 str[2] := chr(48 + numslots mod 10);
 vga_printstring(frm_cfg.fx + 272, frm_cfg.fy + 70, str, black);

 if (usesound) then
  btn[BTN_SOUND].show(BUTTON_DOWN)
 else
  btn[BTN_SOUND].show(BUTTON_UP);

 btn[BTN_OK].show(BUTTON_UP);

 okhit := false;
 while not okhit do
  begin
   key := readkey;
   if upcase(key) = key then
    shift := true
   else
    shift := false;
   key := upcase(key);

   case key of
    #13,'O': begin
          btn[BTN_OK].show(BUTTON_DOWN);
          delay(400);
          btn[BTN_OK].show(BUTTON_UP);
          okhit := true;
         end;
    'S': begin
          if not timerrun then break;
          usesound := not usesound;
          if (usesound) then
           begin
            btn[BTN_SOUND].init(frm_cfg.fx + 280, frm_cfg.fy + 5, 10, 10,'&Y');
            btn[BTN_SOUND].show(BUTTON_DOWN);
           end
          else
           begin
            btn[BTN_SOUND].init(frm_cfg.fx + 280, frm_cfg.fy + 5, 10, 10,'N');
            btn[BTN_SOUND].show(BUTTON_UP);
           end;
         end;
    'Q': begin
          if shift then
           begin
            dec(numq);
            if (numq<=0) then numq := 40;
           end
          else
           begin
            inc(numq);
            if (numq>40) then numq := 1;
           end;
          vga_rectangle(frm_cfg.fx + 271, frm_cfg.fy + 20, frm_cfg.fx + 289, frm_cfg.fy + 29, white, fill);
          str[0] := #2;
          str[1] := chr(48 + numq div 10);
          str[2] := chr(48 + numq mod 10);
          vga_printstring(frm_cfg.fx + 272, frm_cfg.fy + 21, str, black);
         end;
    'P': begin
          if shift then
           begin
            dec(winratio);
            if (winratio<0) then winratio := 50;
           end
          else
           begin
            inc(winratio);
            if (winratio>50) then winratio := 0;
           end;
          vga_rectangle(frm_cfg.fx + 271, frm_cfg.fy + 33, frm_cfg.fx + 289, frm_cfg.fy + 41, white, fill);
          str[0] := #2;
          str[1] := chr(48 + winratio div 10);
          str[2] := chr(48 + winratio mod 10);
          vga_printstring(frm_cfg.fx + 272, frm_cfg.fy + 34, str, black);
         end;
    'M': begin
          if shift then
           begin
            dec(minrolls);
            if (minrolls<3) then minrolls := maxrolls-1;
           end
          else
           begin
            inc(minrolls);
            if (minrolls>=maxrolls) then minrolls := 3;
           end;
          vga_rectangle(frm_cfg.fx + 271, frm_cfg.fy + 45, frm_cfg.fx + 289, frm_cfg.fy + 53, white, fill);
          str[0] := #2;
          str[1] := chr(48 + minrolls div 10);
          str[2] := chr(48 + minrolls mod 10);
          vga_printstring(frm_cfg.fx + 272, frm_cfg.fy + 46, str, black);
         end;
    'X': begin
          if shift then
           begin
            dec(maxrolls);
            if (maxrolls<=minrolls) then maxrolls := 99;
           end
          else
           begin
            inc(maxrolls);
            if (maxrolls>99) then maxrolls := minrolls+1;
           end;
           vga_rectangle(frm_cfg.fx + 271, frm_cfg.fy + 57, frm_cfg.fx + 289, frm_cfg.fy + 65, white, fill);
           str[0] := #2;
           str[1] := chr(48 + maxrolls div 10);
           str[2] := chr(48 + maxrolls mod 10);
           vga_printstring(frm_cfg.fx + 272, frm_cfg.fy + 58, str, black);
         end;
    'T': begin
          if shift then
           begin
            dec(numslots);
            if (numslots<4) then numslots := 12;
           end
          else
           begin
            inc(numslots);
            if (numslots>12) then numslots := 4;
           end;
           vga_rectangle(frm_cfg.fx + 271, frm_cfg.fy + 69, frm_cfg.fx + 289, frm_cfg.fy + 77, white, fill);
           str[0] := #2;
           str[1] := chr(48 + numslots div 10);
           str[2] := chr(48 + numslots mod 10);
           vga_printstring(frm_cfg.fx + 272, frm_cfg.fy + 70, str, black);
         end;
    'C': begin
          timerrun := not timerrun;
          if (timerrun) then
           begin
            SetTimer(addr(cycle_int),20);
            if usesound then
             begin
              btn[BTN_SOUND].init(frm_cfg.fx + 280, frm_cfg.fy + 5, 10, 10,'&Y');
              btn[BTN_SOUND].show(BUTTON_DOWN);
             end
            else
             begin
              btn[BTN_SOUND].init(frm_cfg.fx + 280, frm_cfg.fy + 5, 10, 10,'N');
              btn[BTN_SOUND].show(BUTTON_UP);
             end;
            vga_printstring(frm_cfg.fx + 4, frm_cfg.fy + 8, 'Enable Sound.....................', 0);
            vga_printchar(frm_cfg.fx + 4 + 7*8, frm_cfg.fy + 8, ord('S'), red);
            btn[BTN_TIME].init(frm_cfg.fx + 280, frm_cfg.fy+80, 10, 10, '&Y');
            btn[BTN_TIME].show(BUTTON_DOWN);
           end
          else
           begin
            CloseTimer;
            btn[BTN_SOUND].init(frm_cfg.fx + 280, frm_cfg.fy + 5, 10, 10,'N');
            btn[BTN_SOUND].show(BUTTON_UP);
            btn[BTN_TIME].init(frm_cfg.fx + 280, frm_cfg.fy+80, 10, 10, 'N');
            btn[BTN_TIME].show(BUTTON_UP);
            vga_printstring(frm_cfg.fx + 4, frm_cfg.fy + 8, 'Enable Sound.....................', 26);
           end;
         end;
   end;
  end;
 frm_cfg.hide;
 maxrolls := maxrolls div 3;
 winratio := winratio * 2;
 { read out the staus of vars here... }
end;

function newslot(os : integer) : integer;
var
 i : integer;
begin
 i := random(100);
 if (i<=winratio) and (os<>PICKLE) then
  i := PICKLE
 else
  begin
   i := os;
   while (i=os) or (i=PICKLE) do i := random(numslots);
  end;
 newslot := i;
end;

procedure gameover;
var
 frm : form;
 str : string;
 ok  : button;
 key : char;
 done: boolean;
begin
 frm.init(0,0,200,61,1,1,'Game Over');
 ok.init(frm.fx+90, frm.fy+33, 20, 12, '&Ok');
 frm.backup;
 frm.show;
 ok.show(BUTTON_UP);
 str := 'Sorry, ' + name + ', but';
 vga_printstring(frm.fx+2, frm.fy+8, str, 0);
 str := 'you are bankrupt...';
 vga_printstring(frm.fx+2, frm.fy+18, str, 0);
 done := false;
 while keypressed do readkey;
 while not done do
  begin
   key := readkey;
   if (key=#13) or (key=#27) or (key='O') or (key='o') then
    begin
     done := true;
     ok.show(BUTTON_DOWN);
     delay(200);
     ok.show(BUTTON_UP);
    end;
  end;
 frm.hide;
end;

procedure loadscores;
var
 f    : file;
 i,r  : word;
begin
 assign(f, 'slotscrs.dat');
 {$I-}
 reset(f, 1);
 {$I+}
 if IOResult <> 0 then
  begin
   numscores := 0;
   name := 'Mr Gates';
   hiscore := 0;
  end
 else
  begin
   blockread(f, numscores, sizeof(numscores), r);
   hiscore := 0;
   for i:=0 to numscores do
    begin
     blockread(f, score[i], sizeof(score[i]), r);
     if hiscore<score[i] then
      hiscore := score[i];
     blockread(f, diff[i], sizeof(diff[i]), r);
     blockread(f, scrname[i], sizeof(scrname[i]), r);
    end;
   name := scrname[numscores-1];
   close(f);
  end;
end;

procedure savescores;
var
 f   : file;
 i,w : word;
begin
 assign(f, 'slotscrs.dat');
 rewrite(f,1);

 blockwrite(f, numscores, sizeof(numscores), w);
 for i:=0 to numscores do
  begin
   blockwrite(f, score[i], sizeof(score[i]), w);
   blockwrite(f, diff[i], sizeof(diff[i]), w);
   blockwrite(f, scrname[i], sizeof(scrname[i]), w);
  end;
 close(f);
end;

function addscore(dif,scr:integer;ow:boolean) : boolean;
var
 i     : integer;
 found : boolean;
 index : integer;
begin
 found := false;
 for i:=0 to numscores do
  if scrname[i] = name then
   begin
    index := i;
    found := true;
   end;

 if found and not ow then
  begin
   addscore := true;
   exit;
  end;
 if not found then
  begin
   index := numscores;
   inc(numscores);
  end;

 diff[index] := dif;
 score[index] := scr;
 scrname[index] := name;
end;

procedure showscores;
var
 frm   : form;
 btn   : button;
 i     : integer;
 str   : string;
begin
 frm.init(0,0,200,165,1,1,'Slot Machine Scores');
 frm.backup;
 frm.show;
 btn.init(frm.fx+80, frm.fy+135, 40, 12, '&Ok');
 btn.show(BUTTON_UP);

 for i:=0 to numscores-1 do
  begin
   vga_printstring(frm.fx+5, frm.fy+15+i*10, scrname[i], lightblue);
   str[0] := #3;
   str[1] := chr(48 + score[i] div 100);
   str[2] := chr(48 + score[i] mod 100 div 10);
   str[3] := chr(48 + score[i] mod 10);
   vga_printstring(frm.fx+120, frm.fy+15+i*10, str, lightgreen);
   str[0] := #4;
   str[1] := chr(48 + diff[i] div 100);
   str[2] := chr(48 + diff[i] mod 100 div 10);
   str[3] := chr(48 + diff[i] mod 10);
   str[4] := '%';
   vga_printstring(frm.fx+160, frm.fy+15+i*10, str, lightred);
  end;
 vga_printstring(frm.fx+5, frm.fy+115, 'Red:   Difficulty level', Black);
 vga_printstring(frm.fx+5, frm.fy+115, 'Red:', LightRed);
 vga_printstring(frm.fx+5, frm.fy+125, 'Green: Score (0-999)', Black);
 vga_printstring(frm.fx+5, frm.fy+125, 'Green:', LightGreen);

 readkey;
 btn.show(BUTTON_DOWN);
 delay(100);
 btn.show(BUTTON_UP);
 frm.hide;
end;

function processscore(a,b,c:word) : word;
 function total(flag:word) : word;
 var
  i : word;
 begin
  if a=flag then i:=1 else i:=0;
  if b=flag then inc(i);
  if c=flag then inc(i);
  total:=i;
 end;
var
 count, int : integer;
begin
 if total(PICKLE)=2 then
  begin
   processscore := 10;           { user got 2 pickles, give them 10 quarters }
   exit;
  end
 else if total(PICKLE)=3 then
  begin
   processscore := 40;           { user got 3 pickles, give them 40 quarters }
   exit;
  end
 else
  begin
   for count:=1 to numslots do
    begin
     int := total(count);
     if int=2 then
      begin
       processscore := 5;
       exit;
      end
     else if int=3 then
      begin
       processscore := 20;
       exit;
      end;
    end;
  end;
 processscore := 0;
end;

function getname : string;
var
 frm : form;
 str : string;
 key : char;
 done: boolean;
 len : integer;
 cx  : integer;
procedure erase;
begin
 vga_rectangle(frm.fx+1, frm.fy+2, frm.fx+140, frm.fy+15, 28, fill);
 cx := 75-len*4;
 vga_printstring(frm.fx+cx, frm.fy+7, str, LightGreen);
end;
begin
 frm.init(0,0,150,30,1,1,'Enter your name');
 frm.backup;
 frm.show;
 str := name;
 key := #0;
 done := false;
 len := length(str);
 erase;
 while not done do
  begin
   key := readkey;
   if key=#8 then
    begin
     if len>0 then
      begin
       str[0] := chr(len-1);
       dec(len);
       erase;
      end
     else
      begin
       sound(1000);
       delay(100);
       nosound;
      end;
    end
   else if key=#13 then
    begin
     done := true;
     getname := str;
    end
   else if key=#27 then
    begin
     done := true;
     getname := name;
    end
   else
    begin
     if (len+1<9) then
      begin
       inc(len);
       str := str+key;
       erase;
      end
     else
      begin
       sound(1000);
       delay(100);
       nosound;
      end;
    end;
  end;
 frm.hide;
end;

procedure playgame;
const
 BTN_OK = 0;
 BTN_PULL = 1;
var
 frm_play : form;
 btn      : array[0..1] of button;
 key      : char;
 done,
 slotsrun : boolean;
 curs,i   : integer;
 int,qs   : integer;
 scr,dif  : word;
 nextscr  : word;
 tmp      : image;
 str      : string;
procedure hidecurs;
begin
 vga_rectangle(frm_play.fx+2, 154+curs*18,frm_play.fx+17, 170+curs*18, 28, fill);
end;
procedure showcurs;
begin
 vga_showimage(point, frm_play.fx+2, 154+curs*18,false);
end;
begin
 frm_play.init(245, 10, 73, 180, 0,0, name);
 btn[BTN_PULL].init(265, 154, 50, 16, '&Pull');
 btn[BTN_OK].init(265, 172, 50, 16, '&Quit');

 frm_play.backup;
 frm_play.show;
 btn[0].show(BUTTON_UP);
 btn[1].show(BUTTON_UP);
 done := false;
 curs := 0;
 showcurs;
 qs := numq;
 vga_printstring(frm_play.fx+2, frm_play.fy+10, 'Quarters', LightBlue);
 vga_printstring(frm_play.fx+14, frm_play.fy+30, 'Score', LightBlue);
 str[0] := #2;
 str[1] := chr(48 + qs div 10);
 str[2] := chr(48 + qs mod 10);
 vga_printstring(frm_play.fx+25, frm_play.fy+20, str, LightRed);
 scr := 0;
 str[0] := #3;
 str[1] := chr(48 + scr div 100);
 str[2] := chr(48 + scr mod 100 div 10);
 str[3] := chr(48 + scr mod 10);
 vga_printstring(frm_play.fx+21, frm_play.fy+40, str, LightRed);

 while not done do
  begin
   key := readkey;
   if key=#0 then       { arrow key }
    begin
     key := readkey;
     hidecurs;
     case key of
      'P': if curs = 0 then curs := 1 else curs := 0;
      'H': if curs = 0 then curs := 1 else curs := 0;
     end;
     showcurs;
    end
   else
    begin
     if (key=#13) then
      case curs of
       0: key := 'P';
       1: key := 'Q';
      end
     else
      key := upcase(key);

     case key of
      'Q': begin
            if curs=0 then
             begin
              hidecurs;
              curs:=1;
              showcurs;
             end;
            btn[BTN_OK].show(BUTTON_DOWN);
            delay(200);
            btn[BTN_OK].show(BUTTON_UP);
            done := true;
           end;
      'P': begin
            if curs=1 then
             begin
              hidecurs;
              curs:=0;
              showcurs;
             end;
            btn[BTN_PULL].show(BUTTON_DOWN);
            delay(200);
            btn[BTN_PULL].show(BUTTON_UP);

            vga_rectangle(frm_play.fx+25, frm_play.fy+20, frm_play.fx+41, frm_play.fy+28, 28, fill);
            dec(qs);
            str[0] := #2;
            str[1] := chr(48 + qs div 10);
            str[2] := chr(48 + qs mod 10);
            vga_printstring(frm_play.fx+25, frm_play.fy+20, str, LightRed);

            vga_showimage(mach2, 0,0,NORMAL);

            lastslot[0] := nextslot[0];
            nextslot[0] := newslot(lastslot[0]);
            state[0] := 1;
            vga_mergeimage(slot, 0,0,SW,SH, slotimg, 0,0, NORMAL);
            soundsrun := true;
            for int:=0 to 16 do
             begin
              vga_mergeimage(slot, 0,2,SW,state[0], slotimg, SW*nextslot[0], SH-state[0]-2, NORMAL);
              vga_mergeimage(slot, 0,state[0]+2,SW, SH-state[0]-4, slotimg, SW*lastslot[0], 2, NORMAL);
              inc(state[0],1);
              vga_showimage(slot, SX[0], SY, NORMAL);
             end;
            delay(50);
            vga_getimage(tmp, sx[0], sy, sx[2]+sw-sx[0], sh);
            vga_mergeimage(mach3, sx[0], sy, tmp.width, tmp.height, tmp, 0,0,normal);
            vga_showimage(mach3, 0,0,NORMAL);
            lastslot[1] := nextslot[1];
            nextslot[1] := newslot(lastslot[1]);
            state[1] := 1;
            for int:=0 to 16 do
             begin
              for i:= 0 to 1 do
               begin
                vga_mergeimage(slot, 0,2,SW,state[i], slotimg, SW*nextslot[i], SH-state[i]-2, NORMAL);
                vga_mergeimage(slot, 0,state[i]+2,SW, SH-state[i]-4, slotimg, SW*lastslot[i], 2, NORMAL);
                inc(state[i],1);
                vga_showimage(slot, SX[i], SY, NORMAL);
               end;
             end;
            delay(50);
            vga_getimage(tmp, sx[0], sy, sx[2]+sw-sx[0], sh);
            vga_mergeimage(mach1, sx[0], sy, tmp.width, tmp.height, tmp, 0,0,normal);
            vga_mergeimage(mach2, sx[0], sy, tmp.width, tmp.height, tmp, 0,0,normal);

            vga_showimage(mach2, 0,0,NORMAL);
            delay(50);
            vga_showimage(mach1, 0,0,NORMAL);

            slotsrun := true;

            state[2] := 0;
            delaylen := 1;
            rolls[0] := random(maxrolls)+minrolls;
            rolls[1] := random(maxrolls)+rolls[0]+1;
            rolls[2] := random(maxrolls)+rolls[1]+1;
            soundsrun := true;
            clickcount := 0;
            clicktotal := 1;
            while slotsrun do
             begin
{              if (keypressed) then slotsrun := false;}
              if rolls[2]=0 then slotsrun := false;
              for int:= 0 to 2 do
               begin
                if rolls[int]=0 then continue;
                if (state[int]<>0) then
                 begin
                  vga_mergeimage(slot, 0,2,SW,state[int], slotimg, SW*nextslot[int], SH-state[int]-2, NORMAL);
                  vga_mergeimage(slot, 0,state[int]+2,SW, SH-state[int]-4, slotimg, SW*lastslot[int], 2, NORMAL);
                  inc(state[int],1);
                  if (state[int]>=SH-4) then state[int] := 0;
                 end
                else
                 begin
                  vga_mergeimage(slot, 0,0,SW,SH, slotimg, SW*nextslot[int], 0, NORMAL);
                  state[int] := 1;
                  dec(rolls[int]);
                  if (rolls[int]=0) then continue;
                  lastslot[int] := nextslot[int];
{                  while nextslot[int]=lastslot[int] do}
                   nextslot[int] := newslot(lastslot[int]);
                  if (int=2) and (rolls[2] mod 20 = 0) then inc(delaylen);
                  if (int=2) and (rolls[2] mod 5 = 0) then inc(clicktotal);
                 end;
                vga_showimage(slot, SX[int], SY, NORMAL);
               end;
              delay(delaylen);
             end;
            vga_getimage(tmp, sx[0], sy, sx[2]+sw-sx[0], sh);
            vga_mergeimage(mach1, sx[0], sy, tmp.width, tmp.height, tmp, 0,0,normal);
            vga_mergeimage(mach2, sx[0], sy, tmp.width, tmp.height, tmp, 0,0,normal);
            vga_mergeimage(mach3, sx[0], sy, tmp.width, tmp.height, tmp, 0,0,normal);
            soundsrun := false;
            nextscr := processscore(nextslot[0], nextslot[1], nextslot[2]);
            if (nextscr > 0) and (usesound) and (timerrun) then
             begin
              if nextscr = 5 then
               begin
                sound(500);
                delay(50);
                sound(750);
                delay(150);
                nosound;
               end
              else if nextscr = 10 then
               begin
                sound(400);
                delay(100);
                sound(420);
                delay(100);
                sound(440);
                delay(100);
                sound(500);
                delay(200);
                nosound;
                delay(25);
                sound(500);
                delay(200);
                nosound;
               end
              else if nextscr = 20 then
               begin

               end
              else
               begin
               end   {big money}
             end;
            scr := scr+nextscr;
            qs := qs+nextscr;
            str[0] := #3;
            str[1] := chr(48 + scr div 100);
            str[2] := chr(48 + scr mod 100 div 10);
            str[3] := chr(48 + scr mod 10);
            vga_rectangle(frm_play.fx+21, frm_play.fy+40, frm_play.fx+45, frm_play.fy+50, 28, fill);
            vga_printstring(frm_play.fx+21, frm_play.fy+40, str, LightRed);
            vga_rectangle(frm_play.fx+25, frm_play.fy+20, frm_play.fx+41, frm_play.fy+28, 28, fill);
            str[0] := #2;
            str[1] := chr(48 + qs div 10);
            str[2] := chr(48 + qs mod 10);
            vga_printstring(frm_play.fx+25, frm_play.fy+20, str, LightRed);

            if (qs=0) then
             begin
              delay(1000);
              gameover;                 { let them know that they lose. }
              done := true;
             end;
            vga_killimage(tmp);

           end;
     end;
   end;
  end;
 dif := 100-((numq + (11-numslots) + winratio)*100 div 146);
 addscore(dif,scr,true);
 frm_play.hide;
end;

procedure about;
var
 f    : form;
 b    : button;
 done : boolean;
 i    : image;
 key  : char;
begin
 f.init(0,0,275,130,1,1,'About Slot Machine');
 b.init(f.fx+5, f.fy+100, 24, 12, '&Ok');
 f.backup;
 f.show;
 b.show(BUTTON_UP);
 vga_getimage(i, 0,0,SW,SH);
 vga_mergeimage(i, 0,0,SW,SH, slotimg,0,0, normal);
 vga_showimage(i, f.fx+5, f.fy+5, normal);
 vga_killimage(i);

 vga_printstring(f.fx+50, f.fy+5, 'Sweet Pickle was written by:', LightBlue);
 vga_printstring(f.fx+75, f.fy+15, 'Chris Eldredge', LightRed);
 vga_printstring(f.fx+75, f.fy+25, '3 Thorburn Place', LightRed);
 vga_printstring(f.fx+75, f.fy+35, 'Gaithersburg, MD 20878', LightRed);
 vga_printstring(f.fx+75, f.fy+50, '<wantaco@erols.com>', LightGreen);
 vga_printstring(f.fx+75, f.fy+65, 'PHONE: (301) 424-1360', Cyan);
 vga_printstring(f.fx+75, f.fy+75, 'FAX: (301) 424-0253', Cyan);
 vga_printstring(f.fx+75, f.fy+90, 'Copyright (C) 1997', Magenta);

 done := false;
 while not done do
  begin
   key := readkey;
   if (key=#13) or (key='o') or (key='O') then
    begin
     b.show(BUTTON_DOWN);
     delay(150);
     b.show(BUTTON_UP);
     done := true;
    end;
  end;
 f.hide;
end;

procedure showcalcs;
var
 f    : form;
 b    : button;
 done : boolean;
 key  : char;
begin
 f.init(0,0,275,130,1,1,'How Calculations Are Made');
 b.init(f.fx+240, f.fy+5, 24, 12, '&Ok');
 f.backup;
 f.show;
 b.show(BUTTON_UP);

 vga_printstring(f.fx+5, f.fy+5, 'Difficulty is calculated with', LightBlue);
 vga_printstring(f.fx+5, f.fy+15, 'the following equation:', LightBlue);
 vga_printstring(f.fx+5, f.fy+25, '100-((number of quarters', LightGreen);
 vga_printstring(f.fx+5, f.fy+35, '+(11-number of slot types)', LightGreen);
 vga_printstring(f.fx+5, f.fy+45, '+ chance of pickle)*100 / 146)', LightGreen);

 vga_printstring(f.fx+5, f.fy+60, 'Winning combinations include:', LightBlue);
 vga_printstring(f.fx+5, f.fy+70, '3 Pickles = 40 Quarters', LightRed);
 vga_printstring(f.fx+5, f.fy+80, '2 Pickles = 10 Quarters', LightRed);
 vga_printstring(f.fx+5, f.fy+90, '3 of any other kind = 20 Quarters', LightRed);
 vga_printstring(f.fx+5, f.fy+100, '2 of any other kind = 5 Quarters', LightRed);

 done := false;
 while not done do
  begin
   key := upcase(readkey);
   if (key=#13) or (key='O') then
    begin
     b.show(BUTTON_DOWN);
     delay(150);
     b.show(BUTTON_UP);
     done := true;
    end;
  end;
 f.hide;
end;

const
 QUIT_BTN = 5;
 ABOUT_BTN = 4;
 PNTS_BTN = 3;
 OPTS_BTN = 2;
 SCOR_BTN = 1;
 PLAY_BTN = 0;
var
 frm_main : form;
 btn      : array[0..5] of button;
 key,key2 : char;
 quit     : boolean;
 count,int: integer;
 im       : image;
 pos      : pointer;
 temp     : image;
 curs     : integer;  { current select. }
 fi       : file;
procedure hidecurs;
begin
 vga_rectangle(frm_main.fx+2, frm_main.fy+4+curs*20,frm_main.fx+18, frm_main.fy+20+curs*20, 28, fill);
end;
procedure showcurs;
begin
 vga_showimage(point, frm_main.fx+2, frm_main.fy+4+curs*20,false);
end;
begin
 { initialize variables. }
 mostfree := memavail;
 usesound := true;
 numq := 4;
 winratio := 25;
 minrolls := 3;
 maxrolls := 3;
 numslots:= 7;
 timerrun := true;
 nextslot[0] := 3;
 nextslot[1] := 3;
 nextslot[2] := 3;
 randomize;

 point.width := 16;
 point.height := 16;
 point.mem := ptr(seg(pointdat)+1, ofs(pointdat));
 point.inuse := 27;
 curs := 0;

 vga_init;

 loadpal;
 loadscores;


 for int:=50 to 82 do
  begin
   p[int].r := 0;
   p[int].g := (int-50)*2;
   p[int].b := 0;
  end;
 for int:=82 to 112 do
  begin
   p[int].r := 0;
   p[int].g := (31-(int-82))*2;
   p[int].b := 0;
  end;

 vga_setallpalette(p);

 for int:=0 to 199 do
  for count:=0 to 319 do
   vga_pixel(count,int,50+int mod 62);

 vga_loadcvi('mach1.cvi', mach1);
 vga_loadcvi('mach2.cvi', mach2);
 vga_loadcvi('mach3.cvi', mach3);
 vga_loadcvi('slots.cvi', slotimg);
 vga_getimage(slot, 0,0,SW,SH);                     { Init work img. }
 vga_getimage(temp, 0,0,mach1.width,mach1.height);
 vga_mergeimage(temp, 0,0,mach1.width, mach1.height, mach1, 0,0, SPRITE);
 vga_mergeimage(mach1, 0,0,mach1.width, mach1.height, temp, 0,0, NORMAL);
 vga_getimage(temp, 0,0,mach1.width,mach1.height);
 vga_mergeimage(temp, 0,0,mach1.width, mach1.height, mach2, 0,0, SPRITE);
 vga_mergeimage(mach2, 0,0,mach1.width, mach1.height, temp, 0,0, NORMAL);
 vga_getimage(temp, 0,0,mach1.width,mach1.height);
 vga_mergeimage(temp, 0,0,mach1.width, mach1.height, mach3, 0,0, SPRITE);
 vga_mergeimage(mach3, 0,0,mach1.width, mach1.height, temp, 0,0, NORMAL);

 vga_showimage(mach1, 0,0,normal);
 vga_killimage(temp);

 SetTimer(Addr(cycle_int), 30);

 frm_main.init(195,60,120,135,0,0,'Slot Machine');
 frm_main.backup;
 frm_main.show;
 btn[PLAY_BTN].init(frm_main.fx+20, frm_main.fy+4, 96, 16, '&Play');
 btn[SCOR_BTN].init(frm_main.fx+20, frm_main.fy+24, 96, 16, 'View &Scores');
 btn[OPTS_BTN].init(frm_main.fx+20, frm_main.fy+44, 96, 16, '&Options');
 btn[PNTS_BTN].init(frm_main.fx+20, frm_main.fy+64, 96, 16, '&Calculations');
 btn[ABOUT_BTN].init(frm_main.fx+20, frm_main.fy+84, 96, 16, '&About Slots');
 btn[QUIT_BTN].init(frm_main.fx+20, frm_main.fy+104, 96, 16, 'E&xit');

 showcurs;

 for count:=0 to 5 do
  btn[count].show(BUTTON_UP);

 {******************************}
 quit := false;

 while not quit do
  begin
   key := readkey;
   if (key=#0) then            { an arrow was pressed! }
    begin
     key := readkey;
     hidecurs;
     case key of
      'H': begin              { up! }
            dec(curs);
            if curs<0 then curs:=5;
           end;
      'P': begin              { down! }
            inc(curs);
            if curs>5 then curs:=0;
           end;
     end;
     showcurs;
    end
   else
    begin
     key := upcase(key);
     if (key=#13) then
      begin
       case curs of
        0: key := 'P';
        1: key := 'S';
        2: key := 'O';
        3: key := 'C';
        4: key := 'A';
        5: key := 'X';
       end;
      end;
     case key of
      'X', #27: begin
            if (curs<>5) then
             begin
              hidecurs;
              curs := 5;
              showcurs;
             end;
            btn[QUIT_BTN].show(BUTTON_DOWN);
            delay(400);
            btn[QUIT_BTN].show(BUTTON_UP);
            quit := true;
           end;
      'P': begin
            if (curs<>0) then
             begin
              hidecurs;
              curs := 0;
              showcurs;
             end;
            btn[PLAY_BTN].show(BUTTON_DOWN);
            delay(400);
            btn[PLAY_BTN].show(BUTTON_UP);
            frm_main.hide;
            name := getname;
            playgame;
            frm_main.backup;
            frm_main.show;

            showcurs;

            for count:=0 to 5 do
             btn[count].show(BUTTON_UP);
           end;
      'O': begin
            if (curs<>2) then
             begin
              hidecurs;
              curs := 2;
              showcurs;
             end;
            btn[OPTS_BTN].show(BUTTON_DOWN);
            delay(400);
            btn[OPTS_BTN].show(BUTTON_UP);
            configure;
           end;
      'S': begin
            if (curs<>1) then
             begin
              hidecurs;
              curs := 1;
              showcurs;
             end;
            btn[SCOR_BTN].show(BUTTON_DOWN);
            delay(400);
            btn[SCOR_BTN].show(BUTTON_UP);
            showscores;
           end;
      'C': begin
            if (curs<>3) then
             begin
              hidecurs;
              curs := 3;
              showcurs;
             end;
            btn[PNTS_BTN].show(BUTTON_DOWN);
            delay(400);
            btn[PNTS_BTN].show(BUTTON_UP);
            showcalcs;
           end;
      'A': begin
            if (curs<>4) then
             begin
              hidecurs;
              curs := 4;
              showcurs;
             end;
            btn[ABOUT_BTN].show(BUTTON_DOWN);
            delay(400);
            btn[ABOUT_BTN].show(BUTTON_UP);
            about;
           end;
     end;
    end;
  end;
 frm_main.hide;
 vga_killimage(mach1);
 vga_killimage(mach2);
 vga_killimage(mach3);
 vga_killimage(slot);
 vga_killimage(slotimg);

 if (timerrun) then closetimer;

 vga_done;
{ writeln('Total memory used: ', mostfree-leastfree,' bytes.');
 writeln('Total at start: ', mostfree, ' and total now: ', memavail,'.');
}
 savescores;
end.
