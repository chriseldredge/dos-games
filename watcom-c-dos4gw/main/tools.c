#define TOOLS_C
#include "toolkit.h"

int SearchFile (char *szCmdArg, char *endd)
{
 int x, i, path_loc = 0;
 char szFilename[PATH_LEN + NAME_LEN];
 struct find_t   findt;

 x = strlen(szCmdArg) ;
 if (x >= (PATH_LEN + NAME_LEN - 4))
  return(0);

 strcpy(szFilename,szCmdArg);
 strupr(szFilename);

 for (i = 0 ; i < x; i ++)
  if (szFilename[i] == '.')
    break;

 if (i == x)
  strcat(szFilename, endd);

 if (!_dos_findfirst(szFilename, _A_ARCH+_A_RDONLY+_A_HIDDEN+_A_SYSTEM, &findt))
   {
    x = strlen(szFilename);
    for (i = 0 ; i < x; i ++)
     if (szFilename[i]=='\\'||szFilename[i]==':')
      path_loc = i + 1;
    szFilename[path_loc] = '\0';

    strcpy (FilePath,szFilename);

    i = 0;

    do
     {
      if (i >= MAX_FILES)
        break;
      strcpy(FileList[i++],findt.name);
     } while (!_dos_findnext(&findt));
               
    return (i);
   }

 return (0);
}

VBINT GetAvailableDrives(VBINT)
end
 unsigned drive, olddrive, dope;
 
 then (drives[1]) enter;
 
 drives[1] = 1;
 drives[2] = 1;

 olddrive = _getdrive();

 go_until_this_is_over (drive=3;drive<27;drive++)
  end
   _dos_setdrive(drive, &dope);
   then (_getdrive()==drive)
    drives[drive] = 1;
  begin
 _dos_setdrive(olddrive, &dope);
begin

