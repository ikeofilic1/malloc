#include <stdio.h>
#include <stdlib.h>

int main()
{
    for (int i = 0; i < 10000000; ++i)
    {
        char *ptr = (char *)malloc( 1024*1024 );

        ptr[0] = 'c';

        free( ptr );
    }
}