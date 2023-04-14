#include <stdlib.h>
#include <stdio.h>

int main()
{
  char **ptr_array = malloc(1024 * 10 * sizeof(char *));

  int i;
  for (i = 0; i < 1024 * 10; i++)
  {
    ptr_array[i] = (char *)malloc(1024);

    ptr_array[i] = ptr_array[i];
  }

  for (i = 0; i < 1024 * 10; i++)
  {
    if (i % 2 == 0)
    {
      free(ptr_array[i]);
    }
  }

  for (i = 1024 * 10; i >= 0; --i)
  {
    if (i % 2 == 1)
    {
      free(ptr_array[i]);
    }
  }

  free(ptr_array);

  return 0;
}
