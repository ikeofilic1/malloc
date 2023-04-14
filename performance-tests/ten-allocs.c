#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main()
{
    char *ptr1 = malloc( INT_MAX );
    char *ptr2 = malloc( INT_MAX );
    char *ptr3 = malloc( INT_MAX );
    char *ptr4 = malloc( INT_MAX );
    char *ptr5 = malloc( INT_MAX );
    char *ptr6 = malloc( INT_MAX );
    char *ptr7 = malloc( INT_MAX );
    char *ptr8 = malloc( INT_MAX );
    char *ptr9 = malloc( INT_MAX );
    char *ptr10 = malloc( INT_MAX );

    free ( ptr1 );
    free ( ptr2 );
    free ( ptr3 );
    free ( ptr4 );
    free ( ptr5 );
    free ( ptr6 );
    free ( ptr7 );
    free ( ptr8 );
    free ( ptr9 );
    free ( ptr10 );    
}