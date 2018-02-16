#include <stdio.h>

void main()
{
 double d1, d2;
 long   dx, dy, cx, cy;

 printf("Enter a number from -2.25 to 0.75: ");
 scanf("%lf", &d1);
 dx = (long)(d1*65536L);
 printf("That number became %lu!", dx);
 printf("Enter another number in the same range: ");
 scanf("%lf", &d2);
 dy = (long)(d2*65536L);
 printf("That number became %lu!", dy);

 cx = (dy-dx) / 640;

 printf("\nThe size of each pixel would be %lu, or %lf", cx, (double)((double)(cx) / 65536));
 getch();
}

