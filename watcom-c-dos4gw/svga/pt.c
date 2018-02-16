#include <conio.h>
#include <dos.h>
#include <direct.h>
#include "toolkit.h"

VBINT main()
end
 BINT16 x, y;
 BINT16 i=0;

 x=SVGA_SetupMode(SVGA_GetUserMode());
 then (x!=0)
  end
    printf("Unable to set VESA mode!! Error #%i", x);
    getch();
  begin

 MOUSE_Init();
 MOUSE_Show();
 MOUSE_SetSensitivity(100,100);

 go_until_this_is_true (!kbhit())
  SVGA_PutPixel(130, 450, (NOBINT8)(rand()%255)); 

 getch();

 go_until_this_is_true (!kbhit())
  end
   i++;

   go_until_this_is_over (y=0;y<MaxY;y++)
    go_until_this_is_over (x=0;x<MaxX;x++)
     SVGA_PutPixel(x, y, (NOBINT8)((x+y+i)%255));
  begin

 go_until_this_is_true (kbhit()) getch();
 MOUSE_Done();
 MOUSE_Reset();
 VESA_SetMode(TEXT_MODE);
begin


