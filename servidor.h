/* Archivo de cabeceras para la aplicacion servidor.
* Sahid Reyes 10-10603
* Leonardo Martinez 11-10576
*
* Ultima Modificacion 18/06/16
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

/* Constantes */
#define PUERTO_SERVIDOR 4321
#define MAX_PDU_LENGTH 50
