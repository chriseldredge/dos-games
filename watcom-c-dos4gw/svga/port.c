#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

void main()
{
 char c;
 int i, v;
 
 while (c!='Q')
  {
   printf("(R)ead or (W)rite?\n");
   c=toupper(getch());
   if (c=='R')
    {
     printf("Enter port value.\n");
     scanf("%i", &i);
     printf("Port %x is: %i\n", i, inp(i));
    }
   else if (c=='W')
    {
     printf("Enter port value.\n");
     scanf("%i", &i);
     printf("Enter value.\n");
     scanf("%i", &v);
     outp(i, v);
     printf("Port %x is: %i\n", i, inp(i));
    }
  }
}
