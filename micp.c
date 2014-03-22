#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>


void micp(char* origen, char* destino)
{
int fd,contador,filesize,fd2;
float buffer[1000];
	fd=0;
	fd2=0;
	contador=0;
	filesize=lseek(fd,0,SEEK_END);
	if((fd=open(origen,O_RDONLY,S_IRUSR)) == -1)
	{
		perror("Error, archivo de origen no abierto");
	}else{
		if((fd2=open(destino,O_WRONLY,S_IRUSR))==-1)
		{
			perror("Error, archivo de origen no abierto");
		}else{
		      printf("%i",contador);
			while(contador <= filesize){
				contador++;
				if ((read(fd, buffer, sizeof(float))) == -1){
					perror("Error al leer el archivo");
					break;
				}	
				if ((write(fd2, buffer, sizeof(float))) == -1){
					perror("Error al leer el archivo");
					break;
				}
				  printf("%i",contador);
			}
		}
	}
	close(fd);
	close(fd2);	
}
