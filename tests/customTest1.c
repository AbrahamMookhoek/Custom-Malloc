#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

int main()
{
  struct timeval begin;
  struct timeval end;
  gettimeofday( &begin, NULL );

  printf("Custom test 1: Determine how fast 100000 mallocs and frees can be performed\n");

  char * ptr_array[100000];

  int i;
  for ( i = 0; i < 100000; i++ )
  {
    ptr_array[i] = ( char * ) malloc ( 1024 ); 
    
    ptr_array[i] = ptr_array[i];
  }

  for ( i = 0; i < 100000; i++ )
  {
      free( ptr_array[i] );
  }

  gettimeofday( &end, NULL );
  printf("Runtime (seconds): %lf\n",( (double)( end.tv_sec - begin.tv_sec ) + ( ( (double)end.tv_usec - (double)begin.tv_usec )/1000000 ) ));

  return 0;
}