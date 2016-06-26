/* Archivo de cabeceras para la aplicacion servidor.
* Sahid Reyes 		10-10603
* Leonardo Martinez 11-10576
*
* Ultima Modificacion 24/06/16
*/

/* Librerias a utilizar */

#include "pdu.h"

/* Constantes */
#define MAX_PUESTOS 200

/*Estructura para el registro de vehiculos*/
typedef struct reg_vehiculo{
  int placa;
  time_t tiempo_entrada;
} REG_VEHICULO;

/*Imprime en pantalla un mensaje de error y finaliza la ejecucion*/
void * error(char * mensaje);

/*Registra un mensaje en la bitacora indicada*/
void * registrar(char *bitacora, PDU *pdu);

/*Funcion que verifica y retorna los argumentos de la linea de comandos*/
void *leer_args(int argc, char *argv[], int *numero_puerto,
                       char *bitacora_entrada, char *bitacora_salida);

/*Ingresa un vehiculo en el estacionamiento*/
void *ingresar_vehiculo(REG_VEHICULO * estacionamiento[], REG_VEHICULO * vehiculo);

/* Retira un vehiculo del estacionamiento y retorna un puntero al vehiculo en cuestion, NULL si el vehiculo no existe*/
REG_VEHICULO * retirar_vehiculo(REG_VEHICULO * estacionamiento[], int * placa);

/*Imprime todos los vehiculos en un estacionamiento (SOLO PARA DEBUGGING)*/
void * imprimir_estacionamiento(REG_VEHICULO *estacionamiento[]);

/* Recibe las horas de entrada y salida y retorna el pago correspondiente del estacionamiento*/
int calcular_pago(time_t hora_entrada, time_t hora_salida);

/* Si hay puestos, procesa el PDU entrante*/
void * procesar_pdu(PDU* pdu_entrante, REG_VEHICULO * estacionamiento[], int * num_puestos, PDU* pdu_salida);

void * salida(PDU * pdu_salida);

void * salida1(int m);
