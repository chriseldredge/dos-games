/*  VESAINFO.C   -  Find out about your card!  */

#include <stdio.h>
#include <i86.h>


typedef struct {
   char                    Signature[4];
   unsigned short int      Version;
   char  far               *OEMName;  
   unsigned char           Capabilities[4];
   unsigned short int far  *VideoMode;
   short int               VideoMem;
   char                    dummy[238];    /*Pad to 256 bytes */
   } VESAINFO;

typedef struct {
   unsigned short int		ModeAttr;
   unsigned char			WinAAttr;
   unsigned char			WinBAttr;
   unsigned short int		Granularity;
   unsigned short int		WinSize;
   unsigned short int		WinASegment;
   unsigned short int		WinBSegment;
   unsigned short int		(*PageSwitchFunction)();
   unsigned short int		BytesPerScanLine;

        /*  Not all VESA cards provide the following optional information  */
   unsigned int				XResolution;
   unsigned int				YResolution;
   unsigned char			XCharSize;
   unsigned char			YCharSize;
   unsigned char			NumberOfPlanes;
   unsigned char			BitsPerPixel;
   unsigned char			NumberOfBanks;
   unsigned char			MemoryModel;
   unsigned char			BankSize;
   char						dummy[227];                         /*Pad to 256 bytes */
   } VESAMODEINFO;

short int VESA_GetModeInfo( int Mode, VESAMODEINFO far *Buffer );
short int VESA_GetInfo(VESAINFO far *Buffer);

main()
{

int j;
short int far *p;

VESAINFO VESAInfo;
VESAMODEINFO VESAModeInfo;

VESA_GetInfo( &VESAInfo );

p = (short int far *)&VESAInfo;
for (j=0;j<256/sizeof(short int);j++)
 printf("%4x ", p[j]);
getch();

printf("Signature:  ");
for (j=0; j<4; j++)
	printf("%c", VESAInfo.Signature[j]);
	
printf("\nVESA Version:  %4x\n", VESAInfo.Version);
printf("Manufacturer:  %s\n", VESAInfo.OEMName);
printf("Mode ptr: %lx\n", VESAInfo.OEMName);

printf("Capabilities:  ");
for (j=0; j<4; j++)
	printf("%c", VESAInfo.Capabilities[j]);

printf("\nVideo Memory:  %4dK", VESAInfo.VideoMem * 64);

printf("\nSupported Modes:\n");

while (*VESAInfo.VideoMode != 0xFFFF)
	{
	printf("%4x     ", *VESAInfo.VideoMode);
	VESA_GetModeInfo( *VESAInfo.VideoMode++, &VESAModeInfo);
	printf("Gran: %2d    ", VESAModeInfo.Granularity);
	printf("Size: %3d    ", VESAModeInfo.WinSize);
	printf("Bytes:  %4d\n", VESAModeInfo.BytesPerScanLine);
	}

return(0);
}


short int VESA_GetInfo(VESAINFO far *Buffer)
{

union REGS r;
struct SREGS sr;

r.x.ax = 0x4F00;
sr.es = FP_SEG(Buffer);
r.x.di = FP_OFF(Buffer);

int86x(0x10, &r, &r, &sr);

if (r.h.al == 0x4F)
	return(0);
else
	return(-1);
	
}


short int VESA_GetModeInfo( int Mode, VESAMODEINFO far *Buffer )
{

union REGS r;
struct SREGS sr;

r.x.ax = 0x4F01;
r.x.cx = Mode;
sr.es = FP_SEG(Buffer);
r.x.di = FP_OFF(Buffer);

int86x(0x10, &r, &r, &sr);

if (r.h.al == 0x4F)
	return(0);
else
	return(1);

}
