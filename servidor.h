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
  int ticket;
  time_t tiempo_entrada;
} REG_VEHICULO;

/*Imprime en pantalla un mensaje de error y finaliza la ejecucion*/
void * error(char * mensaje);

/*Registra un mensaje en la bitacora indicada*/
void * registrar(char *bitacora_entrada, char *bitacora_salida,PDU *pdu_entrante);

/*Funcion que verifica y retorna los argumentos de la linea de comandos*/
void *leer_args(int argc, char *argv[], int *numero_puerto,
                       char *bitacora_entrada, char *bitacora_salida);

/*Busca en Estacionamiento si el vehiculo existe*/
int buscar_vehiculo(REG_VEHICULO * estacionamiento[], int * placa);

/*Ingresa un vehiculo en el estacionamiento*/
void *ingresar_vehiculo(REG_VEHICULO * estacionamiento[], REG_VEHICULO * vehiculo);

/* Retira un vehiculo del estacionamiento y retorna un puntero al vehiculo en cuestion, NULL si el vehiculo no existe*/
REG_VEHICULO * retirar_vehiculo(REG_VEHICULO * estacionamiento[], int * placa);

/* Recibe las horas de entrada y salida y retorna el pago correspondiente del estacionamiento*/
int calcular_pago(time_t hora_entrada, time_t hora_salida);

/* Si hay puestos, procesa el PDU entrante*/
int procesar_pdu(PDU * pdu_entrante, REG_VEHICULO * estacionamiento[],
                 int * puestos_ocupados, int * numero_tickets,
                 PDU * pdu_salida);
