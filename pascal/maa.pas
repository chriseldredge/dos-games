program maa;

uses crt, vga;

var
 m : array[0..2] of image;
 f : file;
 p : palette;
begin
 assign(f, 'maze.bip');
 reset(f,1);
 blockread(f,p,768);

 vga_init;
 vga_setallpalette(p);
 vga_loadcvi('m1a.cvi', m[0]);
 vga_loadcvi('m1b.cvi', m[1]);
 vga_loadcvi('m1c.cvi', m[2]);
 vga_showimage(m[0],0,0,normal);
 vga_showimage(m[1],27,12,normal);
 vga_showimage(m[2],56,50,normal);
 readkey;
 vga_done;
end.