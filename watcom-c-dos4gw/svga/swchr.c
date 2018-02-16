#include "toolkit.h"

FILE *fr, *fw;
BINT32 l;
char c;

void main(int argc, char *argv[])
{
 fr = fopen(argv[1], "rb");
 fw = fopen(argv[2], "wb");
 l=0;
 while (!feof(fr))
  {
   l++;
   c = fgetc(fr);
   if (c=='I') c = 'E';
   fputc(c,fw);
  }
 fclose(fw);
 fclose(fr);
}   
