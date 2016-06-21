#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <time.h>
void main(){

	char fecha[18];
	int tam;

	time_t t = time(NULL);
	struct tm *tmp; 
	tmp = localtime(&t);
	
	strftime(fecha,sizeof(fecha),"%D %T",tmp);
	printf("%s\n", fecha);
	int size;
	size = sizeof(fecha);
	printf("%d\n",size );
}