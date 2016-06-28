/* Archivo de cabeceras para la aplicacion servidor.
* Ultima Modificacion 27/06/16
* @author Sahid Reyes 	  	10-10603
* @author Leonardo Martinez 11-10576
*/

/* Librerias a utilizar */
#include "pdu.h"

/* Imprime en pantalla un error y finaliza la ejecucion
* @param mensaje Mensaje a imprimir.
* @return void finaliza la ejecucion del programa.
*/
void * error(char * mensaje);

/* Funcion que verifica y devuelve los argumentos de entrada.
* Si hay argumentos faltantes finaliza la ejecucion
* @param argc entero con el numero de argumentos
* @param argv arreglo con los argumentos
* @param dominio almacenara la direccion IP del servidor.
* @param numero_puerto almacenara el numero de puerto para la conexion
* @param op almacena el tipo de operacion a solicitar (entrada/salida)
* @param id numero de placa del vehiculo
* @return void los parametros quedan inicializados segun sea necesario
*/
void * leer_args(int argc, char *argv[], char *dominio,
                       int *numero_puerto, char *op,
                       int *id);


/* Genera un ID aleatorio para el identificador de peticiones del PDU
* @param placa entero que representa la placa del vechículo
* @param op caracter que posee el tipo de operación a solicitar (entrada/salida)
* @return int que posee el numero aleatorio generado para identificar el PDU.
*/
int generar_id(int placa, char op);