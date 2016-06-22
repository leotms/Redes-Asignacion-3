/* Archivo de cabeceras para la aplicacion servidor.
* Sahid Reyes 		10-10603
* Leonardo Martinez 11-10576
*
* Ultima Modificacion 21/06/16
*/

/* Librerias a utilizar */

#include "pdu.h"

/* Constantes */
#define MAX_PUESTOS 200

/*Imprime en pantalla un mensaje de error y finaliza la ejecucion*/
void * error(char * mensaje);

/*Registra un mensaje en la bitacora indicada*/
void * registrar(char *bitacora, PDU *pdu);

/*Funcion que verifica y retorna los argumentos de la linea de comandos*/
void *leer_args(int argc, char *argv[], int *numero_puerto,
                       char *bitacora_entrada, char *bitacora_salida);


