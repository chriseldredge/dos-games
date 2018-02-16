(*******************************************)
(* FILE:    maze.pas                       *)
(* PURPOSE: maze/robot simulation          *)
(* AUTHOR:  Chris Eldredge                 *)
(*******************************************)
{$F+ $M 32768, 6553600, 393216}
program maze;

uses crt,                               { for keyboard interface   }
     vga,                               { VGA 320x200x256 graphics }
     menu,                              { GUI interface            }
     robot;                             { Robot control interface  }

var
 done : boolean;                        { signals program to quit  }

{ loads a maze from text.  Autodetects width and height. True = success }
function loadmaze(fn:string) : boolean;
var
 fi    : file;
 i     : word;
 b     : byte;
 dummy : word;
begin
 assign(fi,fn);
 {$I-}
 reset(fi,1);
 {$I+}
 if IOResult <> 0 then
  begin
   loadmaze := false;
   exit;
  end;
 i := 1;
 repeat
  blockread(fi, b, 1, dummy);
  mazedata[1,i] := b;
  inc(i);
 until (b=10) or  (b=13);
 blockread(fi, b, 1, dummy);
{ dec(i);}
 mazewidth := i;
 dummy := i;
 i := 1;
 while dummy = mazewidth do
  begin
   inc(i);
   blockread(fi, mazedata[i,1], mazewidth, dummy);
  end;
 mazeheight := i-1;
 mazewidth := mazewidth-2;
 close(fi);
 loadmaze := true;
end;


{ loads program files and initializes variables.  Displays program info.
  NOTE: Call only once. }
procedure aboutbox;
var
 fi    : file;
 pal   : palette;
 ok    : button;
 about : form;
 key   : char;
 count : word;
begin
 vga_cls(0);
 vga_getimage(viewimg, 0,0,320,200);
 vga_getimage(aboutview, 0,0,160,90);
 about.init(0,0,175,175,1,0,'');

 ok.init(about.x+about.w div 2 - 75 div 2, about.y+about.h-16, 75, 12, '&Ok');
 about.backup;
 about.show;
 ok.show(BUTTON_UP);
 vga_rectangle(80, about.fy + 3, 240, about.fy+94, black, nofill);

 vga_printstring(about.fx+3, about.fy+100, 'Programming by', blue);
 vga_printstring(about.fx+13, about.fy+110, 'Chris Eldredge', lightblue);
 vga_printstring(about.fx+3, about.fy+120, 'Images drawn by', blue);
 vga_printstring(about.fx+13, about.fy+130, 'Andrew McLaughlin', lightblue);

 vga_printstring(about.fx+3, about.fy+147, 'Loading files', lightred);
 assign(fi, 'maze.bip');
 {$I-}
 reset(fi,1);
 {$I+}
 if IOResult<>0 then
  begin
   {file error}
  end;
 blockread(fi, pal,768,count);
 pal[0].r := 0;
 pal[0].g := 0;
 pal[0].b := 0;
 close(fi);
 vga_setallpalette(pal);
 delay(500);
 vga_printstring(about.fx+107, about.fy+147, '.', lightred);
 if paramcount < 1 then
  loadmaze('maze2.txt')
 else
  loadmaze(paramstr(1));
 vga_printstring(about.fx+115, about.fy+147, '.', lightred);
 delay(250);
 vga_loadcvi('wall.cvi',wall[0]);
 vga_printstring(about.fx+123, about.fy+147, '.', lightred);
 delay(250);
 vga_loadcvi('wall2.cvi',wall[1]);
 vga_printstring(about.fx+131, about.fy+147, 'Done!', lightred);
 delay(500);
 vga_rectangle(about.fx+3, about.fy+147,about.fx+about.w-6, about.fy+155, 28, fill);
 vga_printstring(about.fx+3, about.fy+147, 'Making Tables', lightred);
 makeTrigTables;
 delay(150);
 vga_rectangle(about.fx+3, about.fy+147,about.fx+about.w-6, about.fy+155, 28, fill);
 if norend then
  vga_printstring(about.fx+3, about.fy+147, 'CPU is pathetic!', lightred)
 else if not largescale then
  vga_printstring(about.fx+3, about.fy+147, 'CPU is decent!', lightblue)
 else
  vga_printstring(about.fx+3, about.fy+147, 'CPU is fast!', lightgreen);
 delay(1200);

 vga_rectangle(about.fx+3, about.fy+147,about.fx+about.w-6, about.fy+155, 28, fill);
 vga_printstring(about.fx+3, about.fy+147, 'Ready to Play...', lightgreen);

 segvid := seg(viewimg.mem^);
 segwall[0] := seg(wall[0].mem^);
 segwall[1] := seg(wall[1].mem^);

 drop;

 facing := random(4);
 if paramcount >= 3 then
  begin
  val(paramstr(2),rx,count);
  val(paramstr(3),ry,count);
  end;

 viewx := rx*CELL_X_SIZE+CELL_X_SIZE div 2;
 viewy := ry*CELL_Y_SIZE+CELL_Y_SIZE div 2;
 viewang := facing*ANGLE_90;

 nomap := true;
 noshow := true;
 norend := false;
 key := #0;
 while (key <> #13) and (key <> 'O') do
  begin
   while (not blocked(front)) and blocked(right) and (not keypressed) do
    step;
   if not blocked(right) then
    begin
     turn(right);
     if not blocked(front) then step;
    end
   else if blocked(front) and blocked(right) then
    begin
     turn(left);
     if not blocked(front) then step;
    end;
   if keypressed then
    key := upcase(readkey);
  end;
 norend := false;
 vga_killimage(aboutview);
 noshow := false;
 nomap := false;
 ok.show(BUTTON_DOWN);
 delay(250);
 ok.show(BUTTON_DOWN);
 delay(100);
 about.hide;
end;

function checkexit : boolean;
var
 f:form;
 b:array[0..1] of button;
 key:char;
begin
 f.init(0,0,153,75,1,1,'Warning!');
 b[0].init(f.fx+13,f.fy+45,50,14,'&Ok');
 b[1].init(f.fx+85,f.fy+45,50,14,'&Cancel');
 f.backup;
 f.show;
 b[0].show(BUTTON_UP);
 b[1].show(BUTTON_UP);
 vga_printstring(f.fx+3, f.fy+6, '  Exiting game may', black);
 vga_printstring(f.fx+3, f.fy+16, 'result in desired', black);
 vga_printstring(f.fx+3, f.fy+26, 'effect.', black);
 key:=upcase(readkey);
 if (key='O') or (key=#13) then
  begin
   checkexit:=true;
   b[0].show(BUTTON_DOWN);
   delay(200);
   b[0].show(BUTTON_UP);
  end
 else
  begin
   checkexit := false;
   b[1].show(BUTTON_DOWN);
   delay(200);
   b[1].show(BUTTON_UP);
  end;
 f.hide;
end;

procedure options;
var
 f       : form;
 bok     : button;
 byn     : array[0..1] of button;
 key     : char;
 menudone: boolean;
begin
 f.init(0,0,200,150,1,1,'Maze Options');
 bok.init(140,160,40,12,'&Ok');
 menudone := false;
 f.backup;
 f.show;
 if norend then
  begin
   byn[0].init(f.fx+f.w-16,f.fy+4,10,10,'N');
   byn[0].show(BUTTON_UP);
  end
 else
  begin
   byn[0].init(f.fx+f.w-16,f.fy+4,10,10,'Y');
   byn[0].show(BUTTON_DOWN);
  end;
 if nomap then
  begin
   byn[1].init(f.fx+f.w-16,f.fy+26,10,10,'N');
   byn[1].show(BUTTON_UP);
  end
 else
  begin
   byn[1].init(f.fx+f.w-16,f.fy+26,10,10,'Y');
   byn[1].show(BUTTON_DOWN);
  end;
 bok.show(BUTTON_UP);
 vga_printstring(f.fx+3, f.fy+5, 'Show 3D picture.........', black);
 vga_printstring(f.fx+43, f.fy+5, '3', red);
 vga_printstring(f.fx+11, f.fy+17, 'Window size large small', black);
 vga_printstring(f.fx+83, f.fy+17, 'z', red);
 vga_printstring(f.fx+3, f.fy+27, 'Show maze map..........', black);
 vga_printstring(f.fx+83, f.fy+27, 'm', red);
 while not menudone do
  begin
   key := upcase(readkey);
   case key of
    'O',#13: menudone:=true;
    '3':     begin
              norend := not norend;
              if norend then
               begin
                byn[0].init(f.fx+f.w-16,f.fy+4,10,10,'N');
                byn[0].show(BUTTON_UP);
               end
              else
               begin
                byn[0].init(f.fx+f.w-16,f.fy+4,10,10,'Y');
                byn[0].show(BUTTON_DOWN);
               end;
             end;
    'M':     begin
              nomap := not nomap;
              if nomap then
               begin
                byn[1].init(f.fx+f.w-16,f.fy+26,10,10,'N');
                byn[1].show(BUTTON_UP);
               end
              else
               begin
                byn[1].init(f.fx+f.w-16,f.fy+26,10,10,'Y');
                byn[1].show(BUTTON_DOWN);
               end;
             end;
   end;
  end;
 bok.show(BUTTON_DOWN);
 delay(200);
 bok.show(BUTTON_UP);
 f.hide;
end;
{ Main GUI interface }
procedure mainmenu;
var
 b        : array[0..2] of button;
 f        : form;
 key      : char;
 i        : integer;
 curs     : integer;
 menudone : boolean;
const
 bload  = 0;
 bopt   = 1;
 bexit  = 2;
 procedure hidecurs;
 begin
  vga_rectangle(f.fx+2,f.fy+8+20*curs,f.fx+10, f.fy+16+20*curs, 28, fill);
 end;
 procedure showcurs;
 begin
  vga_printchar(f.fx+2, f.fy+8+20*curs, ord('>'), black);
 end;
begin
 f.init(200, 100, 119, 74, 0,0, 'Options');
 f.backup;
 f.show;
 b[bload].init(f.fx+12, f.fy+4, 100, 16, '&Load Maze');
 b[bopt].init(f.fx+12, f.fy+24, 100, 16, 'Game &Options');
 b[bexit].init(f.fx+12, f.fy+44, 100, 16, 'E&xit Game');
 for i:=0 to 2 do
  b[i].show(BUTTON_UP);
 menudone:=false;
 curs := 0;
 showcurs;
 while not menudone do
  begin
   key := readkey;
   if (key=#0) then            { an arrow was pressed! }
    begin
     key := readkey;
     hidecurs;
     case key of
      'H': begin              { up! }
            dec(curs);
            if curs<0 then curs:=2;
           end;
      'P': begin              { down! }
            inc(curs);
            if curs>2 then curs:=0;
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
        0: key := 'L';
        1: key := 'O';
        2: key := 'X';
       end;
      end;
     case key of
      #27: menudone := true;
      'X': begin
              if (curs<>2) then
               begin
                hidecurs;
                curs := 2;
                showcurs;
               end;
              b[bexit].show(BUTTON_DOWN);
              delay(200);
              b[bexit].show(BUTTON_UP);
              if checkexit=true then
               begin
                done := true;
                menudone := true;
               end;
             end;
      'O'  : begin
              if (curs<>1) then
               begin
                hidecurs;
                curs := 1;
                showcurs;
               end;
              b[bopt].show(BUTTON_DOWN);
              delay(200);
              b[bopt].show(BUTTON_UP);
              options;
             end;
      'L'  : begin
              if (curs<>0) then
               begin
                hidecurs;
                curs := 0;
                showcurs;
               end;
              b[bload].show(BUTTON_DOWN);
              delay(200);
              b[bload].show(BUTTON_UP);
              { load maze }
             end;
     end;
    end;
  end;

 f.hide;
end;

var
 count,i           : integer;
 key               : char;
 change            : boolean;
 dx, dy            : real;
 x_sub_cell,
 y_sub_cell        : integer;
begin
 randomize;

 vga_init;              { initialize video }
 aboutbox;

 change := true;
 done := false;
 count := 0;

 { initialization is complete }

 while not done do
  begin
    if onexit then
     vga_printstring(0,180,'Exit Found!',magenta);
    if keypressed then key := upcase(readkey) else key:=#1;
    case key of
     #27,#13,'M': mainmenu;
     #0:  begin
           key := readkey;
           change := true;
           dx := 0;
           dy := 0;
           case key of
            'K': viewang := viewang-ANGLE_30;
            'M': viewang := viewang+ANGLE_30;
            'H': begin
                  dx:=cos(6.28*viewang/ANGLE_360)*20;
                  dy:=sin(6.28*viewang/ANGLE_360)*20;
                 end;
            'P': begin
                  dx:=-cos(6.28*viewang/ANGLE_360)*20;
                  dy:=-sin(6.28*viewang/ANGLE_360)*20;
                 end;
           end;
          end;
    end;
   if change then
    begin
     if viewang > ANGLE_360 then
      viewang := 0
     else if viewang < ANGLE_0 then
      viewang := ANGLE_360;

     viewx := viewx + round(dx);
     viewy := viewy + round(dy);
     rx := viewx div CELL_X_SIZE;
     ry := viewy div CELL_Y_SIZE;

     x_sub_cell := viewx mod CELL_X_SIZE;
     y_sub_cell := viewy mod CELL_Y_SIZE;

     if (dx>0) then
      begin
       if (mazedata[ry][rx+1] <> 32)  and (mazedata[ry][rx+1] <> ord('*')) and
          (x_sub_cell > (CELL_X_SIZE-OVERBOARD)) then
           viewx := viewx-(x_sub_cell-(CELL_X_SIZE-OVERBOARD));
      end
     else
      begin
       if (mazedata[ry][rx-1]<>32) and (mazedata[ry][rx-1] <> ord('*')) and
          (x_sub_cell < (OVERBOARD)) then
            viewx := viewx+(OVERBOARD-x_sub_cell);
      end;
     if (dy>0) then
      begin
       if (mazedata[ry+1][rx]<>32) and (mazedata[ry+1][rx] <> ord('*')) and
          (y_sub_cell > (CELL_Y_SIZE-OVERBOARD )) then
            viewy := viewy-(y_sub_cell-(CELL_Y_SIZE-OVERBOARD));
      end
     else
      begin
       if (mazedata[ry-1][rx] <> 32)  and (mazedata[ry-1][rx] <> ord('*')) and
          (y_sub_cell < (OVERBOARD)) then
            viewy := viewy+(OVERBOARD-y_sub_cell);
     end;
    facing := (((viewang+ANGLE_45) div ANGLE_90)) mod 4;
    if (facing<0) then facing:=facing+4;
    rendermaze(viewx,viewy,viewang);
   end;
   change:=false;
  end;

 { close everything down }
 vga_killimage(wall[0]);
 vga_killimage(wall[1]);
 vga_killimage(viewimg);
 vga_done;
end.
