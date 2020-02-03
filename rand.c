#include "stdio.h"
#include "stdlib.h"
#include "stdio.h"
#define M 41
#define C 37

static unsigned int x=367;

unsigned int random1(void)
{
   unsigned int tmpX;

   tmpX = x;
   x = M*x + C ;//based on the y = m*x + c formula of random number generation, where m, c are prime numbers
   return M*tmpX+C;
}
char randomMove( void )
{
   return rand() % 12;
   //return random1() % 12;
}
#if 0
main()
{
   int i,j;
   printf("hellow world\n");

   for(j=0;j<10;j++)
   {
   for(i=0;i<10;i++)
      printf("%5d",randomMove());
   printf("\n");
   }
}
#endif
