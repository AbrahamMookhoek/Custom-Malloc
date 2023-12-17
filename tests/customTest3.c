#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

int main()
{
  struct timeval begin;
  struct timeval end;
  gettimeofday( &begin, NULL );

  printf("Custom test 3: A bunch of realloc and calloc\n");

  char * ptr1 = ( char * ) malloc ( 3000 );
  char * ptr2 = ( char * ) calloc ( 512, 1000 );
  char * ptr3 = ( char * ) malloc ( 1000 );
  char * ptr4 = ( char * ) realloc ( ptr3, 1000 );
  char * ptr5 = ( char * ) malloc ( 1000 );
  char * ptr6 = ( char * ) calloc ( 256, 2000 );
  char * ptr7 = ( char * ) realloc ( ptr6, 5000 );
  char * ptr8 = ( char * ) realloc ( ptr1, 2000 );
  char * ptr9 = ( char * ) calloc ( 1024, 1000 );
  char * ptr10 = ( char * ) malloc ( 1000 );
  char * ptr11 = ( char * ) realloc ( ptr5, 100 );
  char * ptr12 = ( char * ) calloc ( 128, 5000 );
  char * ptr13 = ( char * ) realloc ( ptr2, 4 );

  gettimeofday( &end, NULL );
  printf("Runtime (milliseconds): %.0lf\n", (double)end.tv_usec - (double)begin.tv_usec);

  return 0;
}