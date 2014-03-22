#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


int mimkdir(char* directorio)
{
	if((mkdir(directorio,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))==-1)
	{
	   printf("Error al crear el direcctorio");
	   return 1;
	}

	return 0;

}
