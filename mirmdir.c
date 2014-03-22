#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


int mirmdir(char* directorio)
{
	if(rmdir(directorio)==-1)
	{
	   printf("Error al borrar el direcctorio");
	   return 1;
	}

	return 0;

}
