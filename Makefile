# Archivo Makefile
# Sahid Reyes 		10-10603
# Leonardo Martinez 11-10576
#
# Ultima Modificacion 19/06/16

all: sem_svr sem_cli

sem_svr: servidor.o
	gcc servidor.o -o sem_svr
	
servidor.o: servidor.c pdu.h servidor.h
	gcc -c -g servidor.c

sem_cli: cliente.o
	gcc cliente.o -o sem_cli

cliente.o: cliente.c pdu.h cliente.h
	gcc -c -g cliente.c
	
clean: 
	rm -f *.o sem_svr
	rm -f *.o sem_cli
