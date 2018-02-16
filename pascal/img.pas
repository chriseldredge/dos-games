program ass;

uses crt, vga;

var
 c : integer;
 i : image;
begin
 randomize;

 vga_init;
 vga_cls(0);

 for c:= 0 to 200 do
  vga_hline(0,320,c,c);

 readkey;
 vga_getimage(i, 20,5,200,190);
 vga_cls(random(255));
 readkey;
 while not keypressed do
  begin
   vga_showimage(i, random(340)-20, random(220)-20);
  end;

 readkey;
 vga_done;
end.