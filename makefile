all: mipwd.o mils.o libcontar.a shell

mipdwd.o: mipwd.c mipwd.h
	gcc -c -Wall -o mipwd.o  mipwd.c 
mils.o: mils.c mils.h
	gcc -c -Wall -o mils.o mils.c
libcontar.a:
	ar -r libcontar.a mils.o mipwd.o
shell:
	gcc -Wall -o shell parser.c -L . -l ordenes

clean:
	rm -f shell
	rm -f libordenes.a
	rm -f *.o	


