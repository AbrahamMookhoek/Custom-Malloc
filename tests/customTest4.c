#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

int main()
{
  struct timeval begin;
  struct timeval end;
  gettimeofday( &begin, NULL );

  printf("Custom test 4: mallocs w/ big block sizes and unique frees\n");

  char * ptr_array[1000];

  int i;
  for ( i = 0; i < 1000; i++ )
  {
    ptr_array[i] = ( char * ) malloc ( 2097152 ); 
    
    ptr_array[i] = ptr_array[i];
  }

  for ( i = 0; i < 1000; i++ )
  {
    if(i % 2 == 0)
    {
      free( ptr_array[i] );
    }
  }
  for ( i = 0; i < 500; i++ )
  {
    if(i % 2 != 0)
    {
      free( ptr_array[i] );
    }
  }

  gettimeofday( &end, NULL );
  printf("Runtime: %.0lf\n",( (double)end.tv_usec - (double)begin.tv_usec ) );

  return 0;
}