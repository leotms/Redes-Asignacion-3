/* Archivo de cabeceras para la aplicacion servidor.
* Sahid Reyes 		10-10603
* Leonardo Martinez 11-10576
*
* Ultima Modificacion 19/06/16
*/

/* Librerias a utilizar */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>


/* Imprime en pantalla un error y finaliza la ejecucion*/
void * error(char * mensaje);

/* Funcion que verifica y devuelve los argumentos de entrada*/
void * leer_args(int argc, char *argv[], char *dominio,
                       int *numero_puerto, char *op,
                       int *id);


