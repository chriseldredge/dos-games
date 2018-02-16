#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void main()
{
// unsigned long l;
 unsigned int i;

 for (i=0;i<32;i++)
  printf("\n2 to the %i power is %lu", i, (unsigned long)1<<i);
}
