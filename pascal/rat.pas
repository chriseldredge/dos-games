program rat;

uses crt, vga;

procedure loadpal;
var
 f  : file;
 i  : integer;
 p  : palette;
begin
 assign(f, 'colors.bip');
 {$I-}
 reset(f,768);
 {$I+}
 blockread(f, p, 1, i);
 close(f);
 vga_setallpalette(p);
end;

var
 x,y         : array[1..50] of integer;
 xstep,ystep : array[1..50] of integer;
 numrats     : integer;
 i,c,ci,l    : integer;
 total       : array[1..50] of integer;
 tmp         : image;
 frm         : image;
 offscreen   : image;
 offscreen2  : image;
 ratfrm      : array[1..50] of image;
label lp,skippy;
begin
 randomize;
 vga_init;

 vga_loadcvi('slots.cvi', tmp);
 vga_getimage(frm, 0,0,38,38);
 vga_mergeimage(frm, 0,0,38,38, tmp, 42*2+2, 2, NORMAL);

 asm
  push ds
  les di, frm.mem
  lds si, frm.mem
  mov di,0
  mov si,0
  mov cx, 38*38
  cli
lp:
  lodsb
  cmp al, 253
  jne skippy
  mov al, 0
skippy:
  stosb
  loop lp
  pop ds
 end;
 loadpal;

 vga_cls(0);
 numrats := 50;
 for i:=1 to numrats do
  begin
   vga_showimage(frm,0,0,normal);
   vga_printstring(16,22,int2str(i,2),255);
   vga_getimage(ratfrm[i],0,0,38,38);
   x[i] := 319;
   y[i] := (i-1)*100 div numrats + 100;
   xstep[i] := -random(5)-1;
   total[i]:=0;
  end;

 vga_rectangle(0,0,320,100,lightblue,fill);
 vga_rectangle(0,100,320,200,green,fill);
 for i:=0 to 4000 do
  vga_pixel(32000+random(32000),0,lightgreen);


 vga_killimage(tmp);
 vga_getimage(offscreen, 0,0, 320, 200);
 vga_getimage(offscreen2, 0,0, 320, 200);
 c := 0;
 while (c div 320) >= 0 do
  begin
   if keypressed then break;
   delay(5);
   vga_mergeimage(offscreen2, 0,0,320,200, offscreen, 0,0, normal);
   for i:=1 to numrats do
    begin
     x[i] := x[i]+xstep[i];
     total[i] := total[i]-xstep[i];
     if (random(20)=5) then
      xstep[i] := -random(6)-1;
     if (x[i]<-36) then
      begin
       x[i] := 319;
      end;
     y[i] := y[i] + random(3)-1;
     if (y[i]<100) then y[i] := 100;
     if (y[i]>160) then y[i] := 160;

     vga_mergeimage(offscreen2, x[i],y[i],38,38, ratfrm[i], 0,0, sprite);
    end;
   c := 0;
   for i:=1 to numrats do
    begin
     if total[i] > c then
      begin
       c := total[i];
       ci := i;
      end
    end;
   vga_showimage(offscreen2, 0, 0, normal);
   if (x[ci]>0) then
    vga_printstring(x[ci]+8,y[ci]+8,'1',lightred);
   {delay(1);}
  end;
 vga_cls(darkgray);
 vga_printstring(125, 75, 'Rat    wins!',lightred);
 vga_printstring(157, 75, int2str(ci,2), lightred);
 vga_showimage(ratfrm[ci],140, 100,sprite);
 while keypressed do readkey;
 readkey;
 vga_killimage(frm);
 vga_killimage(offscreen);
 vga_killimage(offscreen2);
 for i:=1 to numrats do
  vga_killimage(ratfrm[i]);
 vga_done;
end.