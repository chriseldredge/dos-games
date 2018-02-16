unit robot;

interface

uses crt,vga;

const
 DELAYLEN = 0;
 EAST     = 0;
 SOUTH    = 1;
 WEST     = 2;
 NORTH    = 3;
 LEFT     =-1;
 RIGHT    = 1;
 FRONT    = 0;
 ANGLE_0     = 0;
 ANGLE_30    = 160;
 ANGLE_45    = 240;
 ANGLE_90    = 480;
 ANGLE_180   = 960;
 ANGLE_270   = 1440;
 ANGLE_360   = 1920;
 CELL_X_SIZE = 64;
 CELL_Y_SIZE = 64;
 OVERBOARD   = 28;

type
 trigtable = array[0..ANGLE_360] of real;

var
 viewang       : integer;
 rx,ry         : integer;
 facing        : integer;
 viewx, viewy  : integer;
 mazewidth     : integer;
 mazeheight    : integer;
 mazedata      : array[1..80,1..80] of byte;
 segwall       : array[0..1] of word;
 scaleheight   : integer;
 segvid        : word;
 viewimg       : image;
 aboutview     : image;
 wall          : array[0..1] of image;
 largescale    : boolean;
 nomap, noshow : boolean;
 norend        : boolean;
 tan_table     : ^trigtable;
 inv_tan_table : ^trigtable;
 cos_table     : ^trigtable;
 inv_cos_table : ^trigtable;
 inv_sin_table : ^trigtable;
 x_step,y_step : ^trigtable;

procedure makeTrigTables;

procedure rendermaze(x,y,view_angle : longint);

procedure turn(dir : integer);
procedure drop;
procedure step;

function  blocked(side : integer) : boolean;
function  onexit : boolean;

implementation

procedure makeTrigTables;
var
 ang        : integer;
 time       : word absolute 0000:$046C;
 start      : word;
 rad_angle  : real;
begin
 vga_printstring(182, 162, '.', lightred);
 getmem(tan_table, sizeof(real) * (ANGLE_360+1));
 getmem(inv_tan_table, sizeof(real) * (ANGLE_360+1));

 getmem(x_step, sizeof(real) * (ANGLE_360+1));
 getmem(y_step, sizeof(real) * (ANGLE_360+1));

 getmem(cos_table, sizeof(real) * (ANGLE_360+1));

 getmem(inv_cos_table, sizeof(real) * (ANGLE_360+1));
 getmem(inv_sin_table, sizeof(real) * (ANGLE_360+1));
 start := time; { time the process }
 for ang:=0 to 1920 do
  begin
   rad_angle := (3.272e-4) + ang * 2*3.141592654/1920;

   tan_table^[ang] := sin(rad_angle) / cos(rad_angle);
   inv_tan_table^[ang] := 1/tan_table^[ang];

   if (ang>=0) and (ang<ANGLE_180) then
    y_step^[ang] := abs(tan_table^[ang] * CELL_Y_SIZE)
   else
    y_step^[ang] := -abs(tan_table^[ang] * CELL_Y_SIZE);

   if (ang>=ANGLE_90) and (ang<ANGLE_270) then
    x_step^[ang] :=-abs(inv_tan_table^[ang] * CELL_X_SIZE)
   else
    x_step^[ang] := abs(inv_tan_table^[ang] * CELL_X_SIZE);

   inv_cos_table^[ang] := 1/cos(rad_angle);
   inv_sin_table^[ang] := 1/sin(rad_angle);
   if ang=1000 then
    vga_printstring(190, 162, '.', lightred);
  end;
 vga_printstring(198, 162, '.', lightred);
 if (time-start) > 44 then
  norend := true
 else
  norend := false;
 if (time-start) > 0 then
  begin
   largescale := false;
   scaleheight := 50;
   for ang:=-ANGLE_30 to ANGLE_30 do
    begin
     rad_angle := (3.272e-4) + ang * 2*3.141592654/ANGLE_360;

     cos_table^[ang+ANGLE_30] := 7112/cos(rad_angle);
    end;
  end
 else
  begin
   largescale := true;
   scaleheight := 100;
   for ang:=-ANGLE_30 to ANGLE_30 do
    begin
     rad_angle := (3.272e-4) + ang * 2*3.141592654/ANGLE_360;

     cos_table^[ang+ANGLE_30] := 31112/cos(rad_angle);
    end;
  end;
 vga_printstring(206, 162, 'Done!', lightred);
end; {Build_Tables}

procedure showmap;
 procedure maprect(x,y : integer; color : byte); assembler;
  asm
   mov ax, y
   mov di, ax
   shl di, 6
   shl ax, 8
   add di, ax
   add di, x
   mov ax, segvid
   mov es, ax
   mov al, color
   mov ah, al
   stosw
   stosb
   add di, 317
   stosw
   stosb
   add di, 317
   stosw
   stosb
  end;
 procedure mappix(x,y : integer; color : byte); assembler;
  asm
   mov ax, y
   mov di, ax
   shl di, 6
   shl ax, 8
   add di, ax
   add di, x
   mov ax, segvid
   mov es, ax
   mov al, color
   stosb
  end;
var
 yc,xc          : integer;
 oldseg         : word;
begin
 if norend then
  begin
   oldseg := segvid;
   segvid := $A000;
  end;
 for yc:=1 to mazeheight do
  for xc := 1 to mazewidth do
   begin
    if (mazedata[yc,xc]=219) or (mazedata[yc,xc]=111) then
     maprect(xc*3,yc*3, blue)
    else if mazedata[yc,xc]=ord('*') then
     maprect(xc*3,yc*3, lightgreen)
    else
     maprect(xc*3,yc*3, black);
   end;
 mappix(rx*3+1,ry*3+1,lightred);
 case facing of
  north: begin
          mappix(rx*3,ry*3+2,lightred);
          mappix(rx*3+2,ry*3+2,lightred);
         end;
  south: begin
          mappix(rx*3,ry*3,lightred);
          mappix(rx*3+2,ry*3,lightred);
         end;
  east: begin
          mappix(rx*3,ry*3,lightred);
          mappix(rx*3,ry*3+2,lightred);
         end;
  west: begin
          mappix(rx*3+2,ry*3,lightred);
          mappix(rx*3+2,ry*3+2,lightred);
         end;
 end;
 if norend then
  segvid := oldseg;

end;

procedure rendermaze(x,y,view_angle : longint);
label lpdat,lpdat2, grnd1,grnd2,lblfill, norendlabel;
var
     sw,
     wallframe,
     sprite_x,
     sprite_y,
     addx,
     addx2,
     xray,
     yray,
     next_y_cell,
     next_x_cell,
     cell_x,
     cell_y,
     x_bound,
     y_bound,
     xb_save,
     yb_save,
     x_delta,
     y_delta,
     ray,
     casting,
     x_hit_type,
     y_hit_type,
     color,
     top,
     bottom       : integer;

      pixelcount, pixelscale,
      xi,
      yi,
      xi_save,
      yi_save,
      dist_x,
      dist_y,
      scale        : real;
      str          : string[20];
begin
if norend then goto norendlabel;
 asm
  mov di, 159
  mov ax, segvid
  mov es, ax
  cli
  mov al, darkgray
  mov ah, al
  mov bx, 50
grnd1:
  mov cx, 80
  rep stosw
  add di, 160
  dec bx
  jnz grnd1

  mov al, 70
  mov ah, al
  mov bx, 50
grnd2:
  mov cx, 80
  rep stosw
  add di, 160
  dec bx
  jnz grnd2
 end;

 view_angle:=view_angle-ANGLE_30;
 if (view_angle < 0) then
  view_angle:=ANGLE_360 + view_angle;

 for ray:=0 to 319 do
  begin
   if (view_angle >= ANGLE_0) and (view_angle < ANGLE_180) then
    begin
     y_bound := CELL_Y_SIZE + CELL_Y_SIZE * (y div CELL_Y_SIZE);

     y_delta := CELL_Y_SIZE;

     xi := inv_tan_table^[view_angle] * (y_bound - y) + x;

     next_y_cell := 0;
    end {if upper half plane}
   else
    begin
     y_bound := CELL_Y_SIZE * (y div CELL_Y_SIZE);

     y_delta := -CELL_Y_SIZE;

     xi := inv_tan_table^[view_angle] * (y_bound - y) + x;

     next_y_cell := -1;
    end; {else lower half plane}

   if (view_angle < ANGLE_90) or (view_angle >= ANGLE_270) then
    begin
     x_bound := CELL_X_SIZE + CELL_X_SIZE * (x div CELL_X_SIZE);
     x_delta := CELL_X_SIZE;
     yi := tan_table^[view_angle] * (x_bound - x) + y;
     next_x_cell := 0;
    end {if right half plane}
   else
    begin
     x_bound := CELL_X_SIZE * (x div CELL_X_SIZE);
     x_delta := -CELL_X_SIZE;
     yi := tan_table^[view_angle] * (x_bound - x) + y;
     next_x_cell := -1;
    end; {else right half plane}

    casting:= 2;
    xray:=0;
    yray:=0;
    while(casting <> 0) do
     begin
      if (xray<>1) then
       begin
        if abs(y_step^[view_angle])=0 then
         begin
          xray := 1;
          dec(casting);
          dist_x := 1e+8;
         end; {if asymtotic ray}

        cell_x := ( (x_bound+next_x_cell) div CELL_X_SIZE);
        cell_y := (round(yi) div CELL_Y_SIZE);

        x_hit_type := mazedata[cell_y, cell_x];
        if (x_hit_type = 219) or (x_hit_type = 111) then
         begin
          dist_x := (yi - y) * inv_sin_table^[view_angle];
          yi_save := yi;
          xb_save := x_bound;

          xray := 1;
          dec(casting);
         end {if a hit}
        else
         yi := yi+y_step^[view_angle];
       end; {if x has been intercepted}

      if yray <> 1 then
       begin
        if abs(x_step^[view_angle])=0 then
         begin
          yray := 1;
          dec(casting);
          dist_y:=1e+8;
        end; {if asymtotic ray}

        cell_x := (round(xi) div CELL_X_SIZE);
        cell_y := ( (y_bound + next_y_cell) div CELL_Y_SIZE);

        y_hit_type := mazedata[cell_y,cell_x];
        if (y_hit_type = 219) or (y_hit_type = 111) then
         begin
          dist_y  := (xi - x) * inv_cos_table^[view_angle];
          xi_save := xi;
          yb_save := y_bound;
          yray := 1;
          dec(casting);
         end {if a hit}
        else
         xi := xi+x_step^[view_angle];
       end; {if y ray has intersected}

      x_bound := x_bound+x_delta;
      y_bound := y_bound+y_delta;
     end; {while casting}

    if (dist_x < dist_y) then
     begin
      scale := cos_table^[ray]/dist_x;
      bottom := round(scale);
      pixelscale := wall[0].height / bottom;
      pixelcount := 0;
      if (x_hit_type=219) then
       wallframe := 0
      else
       wallframe := 1;

      top := 50-round(scale) div 2;
      if top < 0 then
       begin
        pixelcount := pixelscale * abs(top);
        top := 0;
       end;

      if bottom > 99 then
       bottom := 99;

      if round(yi_save) mod CELL_Y_SIZE <= 1 then
       color:=15
      else
       color := round(dist_x * 40 / 1920) + 16;

     sprite_x := round(yi_save) mod wall[wallframe].width;
     if largescale then
      addx := top*320 + ray
     else
      addx := top*320 + ray div 2 + 159;
     addx2 := round(pixelcount)*wall[0].width + sprite_x;
     sw := segwall[wallframe];
     for sprite_y:=0 to bottom do
      begin
       asm
        push ds
        mov ax, segvid
        mov es, ax
        mov di, WORD PTR addx
        mov ax, sw
        mov ds, ax
        mov si, WORD PTR addx2
        movsb
        pop ds
       end;
      addx := addx+320;
      pixelcount := pixelcount+pixelscale;
      addx2 := (round(pixelcount)mod wall[0].height)*wall[0].width + sprite_x;
     end;
    end
   else
    begin
     scale := cos_table^[ray]/dist_y;
     top := 50 - round(scale) div 2;
     bottom := round(scale);
     if (bottom>1) then
      pixelscale := wall[1].height / bottom
     else
      pixelscale := 1;
     pixelcount := 0;
     if (y_hit_type=219) then
      wallframe := 0
     else if (y_hit_type=111) then
      wallframe := 1;

     if top < 0 then
      begin
       pixelcount := pixelscale * abs(top);
       top := 0;
      end;

     if bottom > 99 then
      begin
       bottom:=99;
      end;

     if Round(xi_save) mod CELL_X_SIZE <= 1 then
      color := 15
     else
      color := round(dist_y * 40 / 1920) + 16;

     sprite_x := round(xi_save) mod wall[wallframe].width;
     if largescale then
      addx := top*320 + ray
     else
      addx := top*320 + ray div 2 + 159;
     addx2 := round(pixelcount)*wall[0].width + sprite_x;
     sw := segwall[wallframe];

     for sprite_y:=0 to bottom do
      begin
       asm
        push ds
        mov ax, segvid
        mov es, ax
        mov di, WORD PTR addx
        mov ax, sw
        mov ds, ax
        mov si, WORD PTR addx2
        movsb
        pop ds
       end;
       addx := addx+320;
       pixelcount := pixelcount+pixelscale;
       addx2 := (round(pixelcount)mod wall[0].height)*wall[0].width + sprite_x;
      end;
    end; {else}

    inc(view_angle);
    inc(view_angle);
    inc(ray);
    if view_angle>=ANGLE_360 then
     view_angle:=0;
   end; {for ray}

 if not nomap then showmap;

 asm
  mov ax, segvid
  mov es, ax
  mov di, 32159
  mov al, 0
  mov ah, al
  mov bx, 16
lblfill:
  mov cx, 80
  rep stosw
  add di, 160
  dec bx
  jnz lblfill
 end;
 if noshow then
  begin
   vga_mergeimage(aboutview, 0,0,160,100, viewimg, 160,0, normal);
   vga_showimage(aboutview, 81, 19, normal);
   delay(30);
  end
 else
  begin
   asm
    push ds
    mov ax, segvid
    mov ds, ax
    mov ax, $a000
    mov es, ax
    xor si, si
    xor di, di
    mov cx, 32000
    cli
    rep movsw
    pop ds
   end;
norendlabel:
   if norend and not nomap then
    showmap;

   case facing of
    north: str := 'North';
    south: str := 'South';
    east : str := 'East';
    west : str := 'West';
   end;
   vga_rectangle(0,180,64,199,black,fill);
   vga_printstring(0,190,str,white);
  end;
end; {Ray_Caster}

procedure turn(dir : integer);
var
 count : word;
 angstp: integer;
begin
 if dir=left then
  angstp:=-ANGLE_30 div 2
 else
  angstp:=+ANGLE_30 div 2;
 for count:=0 to 2 do
  begin
   viewang := viewang + angstp;
   if viewang<0 then viewang := viewang+ANGLE_360;
   viewang := viewang mod ANGLE_360;
   rendermaze(viewx, viewy, viewang);
   delay(DELAYLEN div 4);
  end;
 facing := (facing + dir) mod 4;
 if (facing<0) then facing := 3;
 for count:=3 to 5 do
  begin
   viewang := viewang + angstp;
   if viewang<0 then viewang := viewang+ANGLE_360;
   viewang := viewang mod ANGLE_360;
   rendermaze(viewx, viewy, viewang);
   delay(DELAYLEN div 4);
  end;
 delay(DELAYLEN div 4);
end;

function blocked(side : integer) : boolean;
var
 x,y : integer;
 face : integer;
begin
 face := (facing+side) mod 4;
 if (face<0) then
 face := 3;

 if face=north then
  begin
   x := rx;
   y := ry-1;
  end
 else if face=west then
  begin
   x := rx-1;
   y := ry;
  end
 else if face=south then
  begin
   x := rx;
   y := ry+1;
  end
 else if face=east then
  begin
   x := rx+1;
   y := ry;
  end;

 if (mazedata[y,x] <> 32) and (mazedata[y,x] <> ord('*')) then
  blocked := true
 else
  blocked := false;
end;

function onexit : boolean;
begin
 if mazedata[ry,rx] = ord('*') then
  onexit := true
 else
  onexit := false;
end;

procedure drop;
begin
 repeat
  rx := random(mazewidth);
  ry := random(mazeheight);
 until mazedata[ry,rx] = 32;
end;

procedure step;
begin
 case facing of
  north: begin
          viewy := viewy - CELL_Y_SIZE div 2;
          dec(ry);
         end;
  south: begin
          viewy := viewy + CELL_Y_SIZE div 2;
          inc(ry);
         end;
  west:  begin
          viewx := viewx - CELL_X_SIZE div 2;
          dec(rx);
         end;
  east:  begin
          viewx := viewx + CELL_X_SIZE div 2;
          inc(rx);
         end;
 end;

 viewx := rx*CELL_X_SIZE + CELL_X_SIZE div 2;
 viewy := ry*CELL_Y_SIZE + CELL_Y_SIZE div 2;

 rendermaze(viewx,viewy,viewang);
 delay(DELAYLEN);
end;

end.