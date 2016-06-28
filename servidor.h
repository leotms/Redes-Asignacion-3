/* Archivo de cabeceras para la aplicacion servidor.
* Ultima Modificacion 26/06/16
* @author Sahid Reyes 	  	10-10603
* @author Leonardo Martinez 11-10576
*/

/* Importacion de las cabeceras */
#include "pdu.h"

/* Constantes */
#define MAX_PUESTOS 200
#define MAX_CACHE   20

/*Estructura para el registro de vehiculos.
* placa          : identificador univoco del vehiculo
* ticket         : ticket que se le asigno al momento de la entrada
* tiempo_entrada : tiempo en el que el vehiculo entro al estacionamiento
*/
typedef struct reg_vehiculo{
  int placa;
  int ticket;
  time_t tiempo_entrada;
} REG_VEHICULO;

/* Imprime en pantalla un error y finaliza la ejecucion
* @param mensaje Mensaje a imprimir.
* @return void finaliza la ejecucion del programa.
*/
void * error(char * mensaje);

/* Funcion que verifica y devuelve los argumentos de entrada.
* Si hay argumentos faltantes finaliza la ejecucion
* @param argc entero con el numero de argumentos
* @param numero_puerto almacenara el numero de puerto para la conexion
* @param bitacora_entrada ruta del archivo que contendra el registro de operaciones de entrada
* @param bitacora_salida ruta del archivo que contendra el registro de operaciones de salida
* @return void los parametros quedan inicializados segun sea necesario
*/
void *leer_args(int argc, char *argv[], int *numero_puerto,
                       char *bitacora_entrada, char *bitacora_salida);

/* Funcion que escrible la peticion solicitada y en la bitacora correspondiente
*  y si esta fue resuelta satisfactoriamente o no.
* @param bitacora_entrada ruta del archivo que contendra el registro de operaciones de entrada
* @param bitacora_salida ruta del archivo que contendra el registro de operaciones de salida
* @param pdu_entrante pdu que contiene la informacion de la peticion.
* @return void la bitacora correspondiente contendra la informacion de pdu_entrante
*/
void * registrar(char *bitacora_entrada, char *bitacora_salida,PDU *pdu_entrante);

/* Busca en Estacionamiento si el vehiculo con la placa solicitada existe
* @param estacionamiento arreglo con los registros de vehiculos manejados
* @param placa entero con la placa del vehiculo buscado
* @return int 1 si el vehiculo existe, 0 en caso contrario
*/
int buscar_vehiculo(REG_VEHICULO * estacionamiento[], int * placa);

/*Ingresa un vehiculo indicado  en el estacionamiento
* @param estacionamiento arreglo con los registros de vehiculos manejados
* @param vehiculo registro del vehiculo que se quiere ingresar.
* @return void el vehiculo se ingresa en el primer espacio libre encontrado
*/
void *ingresar_vehiculo(REG_VEHICULO * estacionamiento[], REG_VEHICULO * vehiculo);

/* Retira un vehiculo del estacionamiento
* @param estacionamiento arreglo con los registros de vehiculos manejados
* @param placa entero con la placa del vehiculo a retirar
* @return REG_VEHICULO * que contiene el vehiculo en cuestion, NULL si el vehiculo no existe
*/
REG_VEHICULO * retirar_vehiculo(REG_VEHICULO * estacionamiento[], int * placa);

/* Retorna el pago correspondiente del estacionamiento para un intervalo de tiempo_entrada.
* La primera hora de estacionamiento cuesta 80 Bs y la fracción (1 hora) cuesta 30 Bs.
* @param hora_entrada tiempo de entrada del estacionamiento
* @param hora_salida tiempo de salida del estacionamiento
* @return int que representa el pago en bolivares del estacionamiento
*/
int calcular_pago(time_t hora_entrada, time_t hora_salida);

/* Busca en el cache de peticiones atendidas el identificador de la peticion solicitada.
* @param cache_peticiones arreglo con el cache de las peticiones atendidas con anterioridad
* @param peticion identificador univoco de la peticion que se quiere buscar
* @return PDU * que apunta hacia la respuesta que genero la peticion anteriormente.
*/
PDU * buscar_en_cache(PDU * cache_peticiones[], int peticion);

/* Guarda en el cache de peticiones atendidas el la respuesta enviada hacia un cliente.
* @param cache_peticiones arreglo con el cache de las peticiones atendidas con anterioridad
* @param posicion_cache contador que cuenta la posicion en el arreglo cache_peticiones a sustituir por la nueva entrada.
* @param pdu_respuesta contiene la respuesta que se almacenara en el cache
* @return void la respuesta quedara almacenada en cache_peticiones[posicion_cache].
*/
void * guardar_en_cache(PDU * cache_peticiones[], int *posicion_cache, PDU * pdu_respuesta);

/* Procesa el PDU recibido y devuelve una respuesta acorde.
* @param pdu_entrante pdu que contiene la informacion de la peticion.
* @param estacionamiento arreglo con los registros de vehiculos manejados
* @param puestos_ocupados entero que representa el numero de puestos ocupados del estacionamiento
* @param numero_tickets representa el ultimo numero de ticket disponible para asignar
* @param pdu_salida pdu que contendra la informacion a enviar al cliente
* @return int que representa el resultado del procesamiento:
*     - 0 si la Solicitud fue resuelta satisfactoriamente
*     - 1 si el vehiculo queria ingresar y ya se encontraba en el estacionamiento
*     - 2 si el vehiculo queria salir y no se encontraba en el estacionamiento.
*      (En caso de 1 y 2, pdu_salida no contendra informacion relevante).
*/
int procesar_pdu(PDU * pdu_entrante, REG_VEHICULO * estacionamiento[],
                 int * puestos_ocupados, int * numero_tickets,
                 PDU * pdu_salida);
