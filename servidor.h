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
#define MAX_PUESTOS 200
#define MAX_PDU_LENGTH 50

typedef struct pdu_IO{

	int *serialID;
	char *operacion;	

} PDU;


/*Imprime en pantalla un mensaje de error y finaliza la ejecucion*/
void * error(char * mensaje);

/*Registra un mensaje en la bitacora indicada*/
void * registrar(char *bitacora, PDU *pdu);

/*Funcion que verifica y retorna los argumentos de la linea de comandos*/
void *leer_args(int argc, char *argv[], int *numero_puerto,
                       char *bitacora_entrada, char *bitacora_salida);


