#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

int mils(char *aux, char* aux2)
{
  char buffer[512];
  struct stat *atributos;
  struct dirent *direntp;
  DIR *dirp;

  
    if(strcmp(aux,"-l")== 0 && strcmp(aux2,"")== 0) {
      dirp = opendir(getcwd(buffer,-1));
      while ((direntp = readdir(dirp)) != NULL) {
	printf("%d\t%d\t%d\t%s\n", direntp->d_ino, direntp->d_off, direntp->d_reclen, direntp->d_name);
	}
    }else{
		if(strcmp(aux,"-l")== 0 && strcmp(aux2,"")!= 0) {
		 dirp = opendir(aux2);
 		 while ((direntp = readdir(dirp)) != NULL) {
			printf("%d\t%d\t%d\t%s\n", direntp->d_ino, direntp->d_off, direntp->d_reclen, direntp->d_name);
		  }
		}
/*
      if(lstat(*opendir(getcwd( buffer, -1 )),&atributos)==-1){
		printf("Imposible obtener los atributos");
		return(1);
	}
	if(S_ISREG(atributos.st_mode)!=0) printf("Fichero.\n");
	if(S_ISDIR(atributos.st_mode)!=0) printf("Directorio.\n");
	if(S_ISLNK(atributos.st_mode)!=0) printf("Enlace simbolico.\n");
	if(S_ISCHR(atributos.st_mode)!=0) printf("Fichero especial de caracter.\n");
	closedir (*opendir(getcwd( buffer, -1 ));
	
    }else{  
      if(strcmp(aux,"")==0)
      {
      }
      */
    }
    return 0;
}

