#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int mipwd(void)
{
  char buffer[512];

  printf( "El directorio actual es: %s\n",
           getcwd( buffer, -1 ) );

   return 1;
}

