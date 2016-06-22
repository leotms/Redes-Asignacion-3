/* Archivo de cabeceras para la aplicacion servidor.
* Sahid Reyes 		10-10603
* Leonardo Martinez 11-10576
*
* Ultima Modificacion 21/06/16
*/

/* Librerias a utilizar */
#include "pdu.h"

/* Imprime en pantalla un error y finaliza la ejecucion*/
void * error(char * mensaje);

/* Funcion que verifica y devuelve los argumentos de entrada*/
void * leer_args(int argc, char *argv[], char *dominio,
                       int *numero_puerto, char *op,
                       int *id);

