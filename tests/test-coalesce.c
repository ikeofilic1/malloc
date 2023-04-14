#include <stdlib.h>
#include <stdio.h>

int main()
{

  printf("Running test coalesce to test multiple coalesces\n");

  char * ptr1 = ( char * ) malloc ( 1200 );
  char * ptr2 = ( char * ) malloc ( 1200 );
  char * ptr3 = ( char * ) malloc ( 1200 );
  char * ptr4 = ( char * ) malloc ( 1200 );
  char * ptr5 = ( char * ) malloc ( 1200 );

  free( ptr3 );
  free( ptr5 );
  free( ptr4 );

  char * ptr6 = ( char * ) malloc ( 3000 );
  char * ptr7 = ( char * ) malloc ( 500 );

  free( ptr2 );
  free( ptr6 );

  free( ptr7 );
  free( ptr1 );

  char * ptr8 = ( char * ) malloc ( 6000 );

  free( ptr8 );

  return 0;
}
