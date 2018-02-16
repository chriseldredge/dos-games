{$X+}
unit vga;

interface

uses dos, crt;

const
 PaletteMask       = $3C6;
 PaletteRegister   = $3C8;
 PaletteData       = $3C9;
 fill              = 1;
 nofill            = 0;
 cycleup           = true;
 cycledown         = false;
 sprite            = true;
 normal            = false;

type
 rgb = record
  r,g,b: byte;
 end;
 palette = array[0..255] of RGB;

 image = record
  width,height : word;
  inuse        : byte;
  mem          : ^byte;
 end;

var
 videosegment : word;

procedure vga_init;
procedure vga_done;
procedure vga_pixel(x: integer; y: integer; c: byte);
procedure vga_hline(x1,x2,y1:integer;c:byte);
procedure vga_vline(y1,y2,x1:integer;c:byte);
procedure vga_line(x1,y1,x2,y2:integer; c:byte);
procedure vga_printstring(x,y:integer;str:string;col:byte);
procedure vga_printchar(x,y:integer;chr,col:byte);
procedure vga_rectangle(x1,y1,x2,y2:integer;col,fillflag:byte);
procedure vga_cls(col:byte);
procedure vga_setpalette(Index : Byte; Color : RGB);
procedure vga_getpalette(Index : Byte; Var Color : RGB);
procedure vga_setallpalette(Pal : Palette);
procedure vga_getallpalette(Var Pal : Palette);
procedure vga_cyclepalette(Start, Finish : Byte; Var Pal : Palette; CycleFlag : Boolean);

function int2str(i,p:integer) : string;

procedure vga_getimage(var img : image;x,y,w,h:integer);
procedure vga_showimage(var img : image;x,y:integer;rop:boolean);
procedure vga_mergeimage(var dst:  image;dx,dy,dw,dh:integer;var src:image;sx,sy:integer;rop:boolean);
procedure vga_killimage(var img : image);
function vga_loadcvi(fn : string;var img : image) : boolean;

procedure vga_wait;

implementation

procedure vga_init;
var
 r : REGISTERS;
begin
 r.ax := $13;
 intr($10, r);
end;

procedure vga_done;
var
 r : REGISTERS;
begin
 r.ax := $03;
 intr($10, r);
end;

procedure vga_pixel(x:integer; y:integer; c:byte);
begin
 mem[$a000:y*320+x] := c;
end;

procedure vga_hline(x1,x2,y1:integer;c:byte);
label nob;
begin
 asm
  mov ax, y1
  shl ax, 1
  shl ax, 1
  shl ax, 1
  shl ax, 1
  shl ax, 1
  shl ax, 1
  mov bx, ax
  shl ax, 1
  shl ax, 1
  add ax, bx
  add ax, x1
  mov di, ax
  mov ax, $a000
  mov es, ax
  mov cx, x2
  mov bx, x1
  sub cx, bx
  add cx, 1
  mov al, c
  mov ah, al
  mov bx, cx
  and bx, 1
  jz nob
  stosb
nob:
  shr cx, 1
  cld
  rep stosw
 end;
end;

procedure vga_vline(y1,y2,x1:integer;c:byte);
label nob,lpVl;
begin
 asm
  mov ax, y1
  shl ax, 1
  shl ax, 1
  shl ax, 1
  shl ax, 1
  shl ax, 1
  shl ax, 1
  mov bx, ax
  shl ax, 1
  shl ax, 1
  add ax, bx
  add ax, x1
  mov di, ax
  mov ax, $a000
  mov es, ax
  mov cx, y2
  mov bx, y1
  sub cx, bx
  add cx, 1
  mov al, c
  mov ah, al
  cld
lpVl:
  stosb
  add di, 319
  loop lpVl
 end;
end;

procedure vga_line(x1,y1,x2,y2:integer; c:byte);
var
 dx, dy, ax,bx,ay,by,xl,yl,xc,yc : integer;
procedure swap(var a,b : integer);
var
 c : integer;
begin
 if (a<b) then
  begin
   c := a;
   a := b;
   b := c;
  end
end;
begin
 ax := x1;
 bx := x2;
 swap(ax, bx);
 ay := y1;
 by := y2;
 swap(ay, by);
 dx := ax-bx+1;
 dy := ay-by+1;
 xl := dx div dy;
 yl := dy div dx;
 xc := ax;
 while (xc<bx) do
  begin
   for xc:=xc to xc+xl do
    vga_pixel(xc, yc, c);
   inc(yc);
   vga_pixel(xc, yc, c);
 end;
end;

procedure vga_printstring(x,y:integer;str:string;col:byte);
var xc : integer;
begin
 for xc:=0 to length(str)-1 do
  vga_printchar(x+xc*8, y, ord(str[xc+1]), col);
end;

procedure vga_printchar(x,y:integer;chr,col:byte);
var
 chraddr : array[0..2048] of byte absolute $f000:$fa6e;
 wrk     : byte;
 mask    : byte;
 result  : byte;
 cx, cy  : integer;
begin
 for cy:=0 to 7 do
  begin
   wrk := chraddr[chr*8+cy];
   mask := $80;
   for cx:=0 to 7 do
    begin
     asm
      mov al, wrk
      mov ah, mask
      and al, ah
      mov result, al
     end;
     if result<>0 then
      vga_pixel(x+cx, y+cy, col);
     mask := mask shr 1;
    end;
  end;
end;

procedure vga_rectangle(x1,y1,x2,y2:integer;col,fillflag:byte);
var
 i : integer;
begin
 if fillflag = 0 then
  begin
   vga_hline(x1,x2,y1,col);
   vga_vline(y1,y2,x1,col);
   vga_hline(x1,x2,y2,col);
   vga_vline(y1,y2,x2,col);
  end
 else
  for i:=y1 to y2 do
   vga_hline(x1,x2,i,col);
end;

procedure vga_cls(col:byte);
begin
 asm
  mov ax, $a000
  mov es, ax
  mov di, 0
  mov cx, 32000
  cld
  mov al, col
  mov ah, al
  rep stosw
 end;
end;

procedure vga_getimage(var img : image;x,y,w,h:integer);
var
 startx, starty : word;
 width, height  : word;
 leftover       : word;
 vidoffset      : word;
 destseg,destofs: word;
label Lp;
begin
 if (x+w) > 320 then
  begin
   startx := x;
   width := 320-x+1;
  end
 else
  begin
   startx := x;
   width := w;
  end;
 if (y+h) > 200 then
  begin
   starty := y;
   height := 200-y+1;
  end
 else
  begin
   starty := y;
   height := h;
  end;

 vidoffset := startx + starty*320;

 if img.inuse = 27 then
  freemem(img.mem, img.width*img.height);

 img.width := width;
 img.height := height;
 getmem(img.mem, img.width*img.height);
 img.inuse := 27;
 destseg := seg (img.mem^);
 leftover := 320-width;
 asm
  push ds
  mov bx, height
  xor ax, ax
  or  ax, $a000
  mov ds, ax
  mov si, vidoffset
  mov ax, destseg
  mov es, ax
  mov di, 0
  mov ax, leftover
  cld
Lp:
  mov cx, width
  rep movsb
  add si, ax
  dec bx
  cmp bx, 0
  jg Lp

  pop ds
 end;
end;

procedure vga_showimage(var img : image;x,y:integer;rop:boolean);
var
 leftover,srcleftover,
 startx,starty,srcstart,
 width,height,
 vidx,vidy,vidstart,
 srcseg         : word;
label lp, lp2, lpfor, noby, no, done;
begin
 if img.inuse<>27 then exit;
 if (x<0) then
  begin
   vidx := 0;
   startx := abs(x);
   width := img.width-startx;
   srcleftover := img.width - width;
  end
 else
  begin
   vidx := x;
   startx := 0;
   if (x+img.width) > 320 then
    begin
     width:= 320-x;
     srcleftover := img.width-width;
    end
   else
    begin
     width:= img.width;
     srcleftover := 0;
    end;
  end;
 if (y<0) then
  begin
   vidy := 0;
   starty := abs(y);
  end
 else
  begin
   vidy := y;
   starty := 0;
   if (y+img.height) > 200 then
    height:= 200-y
   else
    height := img.height;
  end;

 srcseg := seg (img.mem^);
 vidstart := vidy*320+vidx;
 srcstart := starty * img.width + startx;
 leftover := 320-width;
 if rop=true then
  asm
   push ds

   mov bx, height

   xor ax, ax
   or  ax, $a000
   mov es, ax
   mov di, vidstart
   mov ax, srcseg
   mov ds, ax
   mov si, srcstart
   cld
   mov dx, width
   and dx, 01h
Lp:
   mov cx, width
lpfor:
   lodsb
   cmp al, 0
   jz no
   stosb
   jmp done
no:
   inc di
done:
   loop lpfor
   add di, leftover
   add si, srcleftover
   dec bx
   cmp bx, 0
   jg Lp

   pop ds
  end
 else
  asm
   push ds

   mov bx, height

   xor ax, ax
   or  ax, $a000
   mov es, ax
   mov di, vidstart
   mov ax, srcseg
   mov ds, ax
   mov si, srcstart
   cld
   mov dx, width
   and dx, 01h
Lp2:
   mov cx, width
   shr cx, 1
   cmp dx, 1
   jne noby
   movsb
noby:
   rep movsw
   add di, leftover
   add si, srcleftover
   dec bx
   cmp bx, 0
   jg Lp2

   pop ds
  end;
end;

procedure vga_mergeimage(var dst:  image;dx,dy,dw,dh:integer;var src:image;sx,sy:integer;rop:boolean);
var
 leftover,srcleftover,
 startx,starty,srcstart,
 width,height,
 vidx,vidy,vidstart,
 srcseg,dstseg  : word;
label lp, lp2, lpfor, noby, no, done;
begin
 if (src.inuse<>27) or (dst.inuse<>27) then
  exit;

 if (dx<0) then
  begin
   vidx := 0;
   startx := abs(dx);
   width := src.width-startx;
   srcleftover := src.width - width;
  end
 else
  begin
   vidx := dx;
   startx := sx;
   if (dx+src.width) > dst.width then
    begin
     width:= dst.width-dx;
     srcleftover := src.width-width;
    end
   else
    begin
     width:= src.width;
     srcleftover := 0;
    end;
  end;
 if (width>dst.width) then
  begin
   width := dst.width;
   srcleftover := src.width - dst.width;
   leftover := 0;
  end
 else
  leftover := dst.width-width;

 if (dy<0) then
  begin
   vidy := 0;
   starty := abs(dy);
   height := src.height-starty;
   if (starty+height)>dst.height then
    height := dst.height;
  end
 else
  begin
   vidy := dy;
   starty := sy;
   if (dy+dh) > dst.height then
    height:= dst.height-dy
   else
    height := dh;
  end;

 srcseg := seg (src.mem^);
 dstseg := seg (dst.mem^);
 vidstart := vidy*dst.width+vidx;
 srcstart := starty * src.width + startx;

 if rop=true then
  asm
   push ds

   mov bx, height

   mov ax, dstseg
   mov es, ax
   mov di, vidstart
   mov ax, srcseg
   mov ds, ax
   mov si, srcstart
   cld
   mov dx, width
   and dx, 01h
Lp:
   mov cx, width
lpfor:
   lodsb
   cmp al, 0
   jz no
   stosb
   jmp done
no:
   inc di
done:
   loop lpfor
   add di, leftover
   add si, srcleftover
   dec bx
   cmp bx, 0
   jg Lp

   pop ds
  end
 else
  asm
   push ds

   mov bx, height

   mov ax, dstseg
   mov es, ax
   mov di, vidstart
   mov ax, srcseg
   mov ds, ax
   mov si, srcstart
   cld
   mov dx, width
   and dx, 01h
Lp2:
   mov cx, width
   shr cx, 1
   cmp dx, 1
   jne noby
   movsb
noby:
   rep movsw
   add di, leftover
   add si, srcleftover
   dec bx
   cmp bx, 0
   jg Lp2

   pop ds
  end;
end;

procedure vga_killimage(var img : image);
begin
 if (img.inuse = 27) then
  begin
   freemem(img.mem, img.width*img.height);
   img.inuse := 0;
  end;
end;

procedure vga_setpalette(Index : Byte; Color : RGB);
begin
 port[PaletteMask] := $FF;
 port[PaletteRegister] := Index;
 port[PaletteData] := Color.R;
 port[PaletteData] := Color.G;
 port[PaletteData] := Color.B;
end;

procedure vga_getpalette(Index : Byte; Var Color : RGB);
begin
 port[PaletteMask] := $FF;
 port[PaletteRegister] := Index;

 color.R := port[PaletteData];
 color.G := port[PaletteData];
 color.B := port[PaletteData];
end;

procedure vga_setallpalette(Pal : Palette);
var
 I : integer;
begin
 for I := 0 to 255 do
  vga_setpalette(I, pal[I]);
end;
procedure vga_getallpalette(Var Pal : Palette);
var
 I : integer;
begin
 vga_getpalette(0, pal[0]);
 for I := 1 to 255 do
  vga_getpalette(I, pal[I]);
end;

procedure vga_CyclePalette(Start, Finish : Byte; Var Pal : Palette; CycleFlag : Boolean);
var
 I     : Integer;
 Temp  : RGB;
begin
 if CycleFlag = CycleUp then
  begin
         Temp := Pal[Start];

         For I := Start To Finish - 1 Do
             Pal[I] := Pal[I+1];

         Pal[Finish] := Temp;
       End
     Else
       Begin
          Temp := Pal[Finish];

          For I := Finish DownTo Start + 1 Do
              Pal[I] := Pal[I-1];

          Pal[Start] := Temp;
       End;
{       vga_SetAllPalette(Pal)}
   End;

function vga_loadcvi(fn : string;var img : image) : boolean;
var
 fi : file;
 s  : string;
 b  : ^byte;
 numread : word;
begin
 assign(fi, fn);
 {$I-}
 reset(fi,1);
 {$I+}
 if IOResult<>0 then
  begin
   vga_loadcvi := false;
   exit;
  end;
 s[0]:=#8;
 blockread(fi,s[1],8, numread);
 if ((s = 'CVI IMG'#10) or (s = 'CVE IMG'#10)) and (numread=8) then
  begin
   if img.inuse=27 then
    freemem(img.mem, img.width*img.height);
   img.inuse:=27;
   blockread(fi,img.width,2, numread);
   blockread(fi,img.height,2, numread);
   getmem(img.mem, img.width*img.height);
   b := ptr(seg(img.mem^), 0);
   blockread(fi,b^,img.width*img.height,numread);
   vga_loadcvi := true;
  end
 else
  begin
   vga_loadcvi := false;
   exit;
  end;

 close(fi);
end;

procedure vga_wait; assembler;
label L1, L2;
asm
   mov dx, 3DAh
l1:in al, dx
   and al, 08h
   jnz l1
l2:in  al, dx
   and al, 08h
   jz  L2
end;

function int2str(i,p:integer) : string;
{ FUNCTION       : Takes an integer value and converts it to a string. }
{ ARGS           : i is an integer value which is to be placed in the string. }
{                  p is the number of place holders to fill (leading with zeros). }
{ RETS           : returns a string constant                         . }
var
 tmp   : string[6];
 length: integer;
 wrk   : integer;
 numplace,
 place,
 count : longint;
begin
 if i<0 then tmp := '-' else tmp := '';
 wrk := abs(i);
 place := 1;
 for count := 2 to p do
  place := place*10;
 for count := p downto 1 do
  begin
   tmp := tmp + chr(48 + wrk div place);
   wrk := wrk mod place;
   place := place div 10;
  end;
 int2str := tmp;
end;
begin
 videosegment := $a000;
end.
