#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

int main()
{
  struct timeval begin;
  struct timeval end;
  gettimeofday( &begin, NULL );

  printf("Custom test 2: How many splits and grows are performed?\n");

  char * ptr1 = ( char * ) malloc ( 1000 );
  char * ptr2 = ( char * ) malloc ( 1000 );
  char * ptr3 = ( char * ) malloc ( 1000 );
  char * ptr4 = ( char * ) malloc ( 1000 );
  char * ptr5 = ( char * ) malloc ( 1000 );
  char * ptr6 = ( char * ) malloc ( 1000 );

  free(ptr3);
  free(ptr1);
  free(ptr5);

  char * ptr7 = ( char * ) malloc ( 500 );
  char * ptr8 = ( char * ) malloc ( 250 );
  char * ptr9 = ( char * ) malloc ( 2000 );
  char * ptr10 = ( char * ) malloc ( 250 );
  char * ptr11 = ( char * ) malloc ( 750 );
  char * ptr12 = ( char * ) malloc ( 7500 );

  free(ptr2);
  free(ptr4);
  free(ptr6);
  free(ptr7);
  free(ptr8);
  free(ptr9);
  free(ptr10);
  free(ptr11);
  free(ptr12);

  gettimeofday( &end, NULL );
  printf("Runtime (milliseconds): %.0lf\n",(double)end.tv_usec - (double)begin.tv_usec );

  return 0;
}