/* Archivo de cabeceras para el PDU a utilizar.
* Ultima Modificacion 26/06/16
* @author Sahid Reyes 	  	10-10603
* @author Leonardo Martinez 11-10576
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
#include <stdbool.h>
#include <time.h>

/* Estructura para el PDU del protocolo SEM
*  Permite manejar de manera estructurada los servicios del Sistema SEM.
* tipo_paq   : identifica el tipo de mensaje que contiene el paquete.
* fuente     : identifica si el paquete viene del servidor o de un cliente.
* puesto	   : indica si quedan puestos en el estacionamiento.
* placa		   : placa del vehiclo que quiere ingresar/salir
* fecha_hora : fecha y hora en string de la operacion de entrada/salida
* n_ticket   : numero de ticket asignado
* monto		   : monto a pagar
* codigo	   : codigo utilizado en caso de ser un mensaje de informacion
* chk_sum	   : Suma de comprobación de los campos del PDU.
*/

typedef struct pdu_IO{
	char tipo_paq;
	int  codigo;
	bool fuente;
	bool puesto;
	int  placa;
	char fecha_hora[18];
	int  n_ticket;
	int  monto;
	int  chk_sum;
} PDU;

/* Funcion que calcula la suma de comprobación del PDU
* @param pdu será el pdu al que se le calculará el checksum
* @return void se almacenará la suma de comprobación en el respectivo campo del PDU
*/
void * calc_checksum(PDU *pdu);

/* Funcion que comprueba la llegada de un PDU por medio de la suma de comprobación.
* @param pdu será el pdu a comprobar mediante su checksum
* @return void en caso de ser fallido el checksum, éste imprimirá mensaje de error.
*/
void * comp_checksum(PDU *pdu);