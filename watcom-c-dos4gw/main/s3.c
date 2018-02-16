NOBINT16 S3_ChipID;

void S3_Unlock()
{
 void asmS3unlock();
 #pragma aux asmS3unlock =      \
   "mov dx, 3d4h"               \
   "mov al, 31h"                \
   "out dx, al"                 \
   "inc dx"                     \
   "in  al, dx"                 \
   "or  al, 01"                 \
   "out dx, al"                 \
   "dec dx"                     \
   "mov al, 38h"                \
   "out dx, al"                 \
   "inc dx"                     \
   "in  al, dx"                 \
   "mov bh, al"                 \
   "mov al, 48h"                \
   "out dx, al"                 \
   "dec dx"                     \
   "mov al, 39h"                \
   "out dx, al"                 \
   "inc dx"                     \
   "in al, dx"                  \
   "mov bl, al"                 \
   "mov al, 0A0h"               \
   "out dx, al"                 \
   "mov ax, bx"                 ;

 asmS3unlock();
}

BINT16 S3_Check()
{
 NOBINT8 v1,v2;

 S3_Unlock();

 outp(0x3d4,0x47);
 outp(0x3d5,0xff); v1=inp(0x3d5);
 outp(0x3d5,0x00); v2=inp(0x3d5);

 if ((v2==0x00)&&(v1==0xff))
  return(1);
 return(0);
}

NOBINT16 S3_IdentifyChipset()
{
 NOBINT32 v;

 outp(0x3d4, 0x30);
 v = inp(0x3d5);        // Bits 7..4
 v &= 0xf0;
 printf("\nVersion Reg: %x\n", v);


 switch(v)
  {
   case 0x80: case 0x81: case 0x91:
    S3_ChipID = 911;
    return(911);
   case 0x82:
    S3_ChipID = 924;
    return(924);
   case 0x90: case 0x94: case 0x95: case 0xB0:
    S3_ChipID = 928;
    return(928);
   case 0xA0: case 0xA2: case 0xA3: case 0xA4: case 0xA5: case 0xA8:
    S3_ChipID = 800;
    return(800);
   case 0xC0:
    S3_ChipID = 864;
    return(864);
   default:
    S3_ChipID = 964;
    return(964);
  }
}

void doit()
{
 NOBINT16  v;
 NOBINT32 v32;
 v=inp(0x4ae8);
 v|=1;
 outp(0x4ae8, v);

 outp(0x3d4, 0x31);
 v=inp(0x3d5);
 v|=8;
 outp(0x3d5, v);

 while (inp(0x9ae8) & 512) {}

 outp(0x3d4, 0x58);
 v=inp(0x3d5);
 v|=16;
 v|=3;
 outp(0x3d5, v);

 outp(0x3d4, 0x00);
 v32=inpw(0x3d5);
 printf("\nindex 0: ");
 for (v=0;v<32;v++)
  printf("%i", (v32 & (1<<(31-v))) ? 1 : 0);
 printf("\nindex 0: %lu\n", v32);
 outp(0x3d4, 0x02);
 v32=inpw(0x3d5);
 printf("\nindex 2: ");
 for (v=0;v<32;v++)
  printf("%i", (v32 & (1<<(31-v))) ? 1 : 0);
 printf("\nindex 2: %lu\n", v32);
 outp(0x3d4, 0x04);
 v32=inpw(0x3d5);
 printf("\nindex 4: ");
 for (v=0;v<32;v++)
  printf("%i", (v32 & (1<<(31-v))) ? 1 : 0);
 printf("\nindex 4: %lu\n", v32);
 outp(0x3d4, 0x06);
 v32=inpw(0x3d5);
 printf("\nindex 6: ");
 for (v=0;v<32;v++)
  printf("%i", (v32 & (1<<(31-v))) ? 1 : 0);
 printf("\nindex 6: %lu\n", v32);
 outp(0x3d4, 0x08);
 v32=inpw(0x3d5);
 printf("\nindex 8: ");
 for (v=0;v<32;v++)
  printf("%i", (v32 & (1<<(31-v))) ? 1 : 0);
 printf("\nindex 8: %lu\n", v32);
 outp(0x3d4, 0x10);
 v32=inpd(0x3d5);
 printf("\nBase Address 0: ");
 for (v=0;v<32;v++)
  printf("%i", (v32 & (1<<(31-v))) ? 1 : 0);
 printf("\n");
printf("\nBase Address 0: %lu or %x\n", v32, v32);

// BytesPerBank = 800*600*4;

}
