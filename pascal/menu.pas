unit menu;

interface

uses crt, vga;

const
 gray1 = 16;
 gray2 = 19;
 gray3 = 22;
 gray4 = 25;
 gray5 = 28;
 gray6 = 31;
 BUTTON_UP   = 0;
 BUTTON_DOWN = 1;
 BUTTON_FLAT = 2;

type
 form = object
  public
    constructor init(nx,ny,nw,nh : integer;uc,ub:byte;ncap:string);
    procedure   show;
    procedure   backup;
    procedure   hide;

    function    fx : integer;
    function    fy : integer;
{    function    fx : integer;
    function    fx : integer;}
  private
    back    : image;
    x,y,w,h : integer;
    c       : string[34];
 end;
 button = object
  public
    constructor init(nx,ny,nw,nh : integer; ncap:string);
    procedure reset(nx,ny,nw,nh : integer; ncap:string);
    procedure show(nstat:byte);
  private
    x,y,w,h : integer;
    hichr : byte;
    cap     : string;
 end;

implementation

constructor button.init(nx,ny,nw,nh : integer; ncap:string);
begin
 button.reset(nx,ny,nw,nh,ncap);
end;

procedure button.reset(nx,ny,nw,nh : integer; ncap:string);
var
 i : integer;
begin
 x := nx;
 y := ny;
 w := nw;
 h := nh;
 hichr := 255;
 cap := '';
 for i:=1 to length(ncap) do
  if (ncap[i]='&') then
   hichr := i
  else
   cap := cap + ncap[i];
end;

procedure button.show(nstat:byte);
var
 dx : integer;
begin
 case (nstat) of
  BUTTON_UP:
            begin
             vga_vline(y,y+h,x,gray6);
             vga_hline(x,x+w,y,gray6);
             vga_vline(y,y+h,x+w,gray4);
             vga_hline(x,x+w,y+h,gray4);
            end;
  BUTTON_DOWN:
            begin
             vga_vline(y,y+h,x,gray4);
             vga_hline(x,x+w,y,gray4);
             vga_vline(y,y+h,x+w,gray6);
             vga_hline(x,x+w,y+h,gray6);
            end;
 end;
 vga_rectangle(x+1,y+1,x+w-1,y+h-1, gray5, fill);
 dx := (x+x+w) div 2 - length(cap)*4;
 if (nstat=BUTTON_UP) then
  begin
   vga_printstring(dx+1, (y+y+h) div 2 - 4, cap, 0);
   if (hichr<>255) then
    vga_printchar(dx+hichr*8-8+1, (y+y+h) div 2 - 4, ord(cap[hichr]), red)
  end
 else
  begin
   vga_printstring(dx+2, (y+y+h) div 2 - 3, cap, 0);
   if (hichr<>255) then
    vga_printchar(dx+hichr*8-8+2, (y+y+h) div 2 - 3, ord(cap[hichr]), red)
  end;

end;

constructor form.init(nx,ny,nw,nh : integer;uc,ub:byte;ncap:string);
begin
 x:=nx;
 y:=ny;
 w:=nw;
 h:=nh;
 if uc<>0 then          { center the form }
  begin
   x := 160-(w div 2);
   y := 100-(h div 2);
  end;
 c := ncap;
end;

procedure form.backup;
begin
 vga_getimage(back,x,y,w+1,h+1);
end;

procedure form.hide;
begin
 vga_showimage(back,x,y,false);
 back.inuse:=0;
 freemem(back.mem, back.width*back.height);
end;

function form.fx : integer;
begin
 fx := x + 2;
end;

function form.fy : integer;
begin
 if (length(c)<>0) then
  fy := y+12
 else
  fy := y + 2;
end;

procedure form.show;
var
 wx, wy : integer;
 btn    : button;
begin
 btn.init(x,y,w,h,'');
 btn.show(BUTTON_UP);

 if length(c) <> 0 then
  begin
   btn.reset(x+3, y+3, w-6, 10, c);
   btn.show(BUTTON_DOWN);
  end;

end;

end.