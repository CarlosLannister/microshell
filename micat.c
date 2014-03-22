#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>




int micat(char* archivo)
{
	int fd,filesize,i;
	char* map;

	fd=open(archivo,O_RDONLY,S_IRUSR);
	filesize=lseek(fd,0,SEEK_END);
	map = mmap(0, filesize, PROT_READ , MAP_SHARED, fd, 0);	
	if (map == MAP_FAILED) {
		close(fd);
		perror("Error al proyectar en memoria");
		return 0;
    	}

        for (i = 0; i <filesize; i++) {
		printf("%c",map[i]);		 
        }

        if (munmap(map, filesize) == -1) {
		perror("Error al liberar las direcciones de memoria");
        }

	return 0;
}
