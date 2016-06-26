/* Archivo de cabeceras para el PDU a utilizar.
* Sahid Reyes 		10-10603
* Leonardo Martinez 11-10576
*
* Ultima Modificacion 22/06/16
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

typedef struct pdu_IO{

	char tipo_paq;			// Operación a realizar
	bool fuente;			// Origen del paquete
	bool puesto;			// Indica si hay puesto o no
	int placa;				// Placa del vehículo
	char fecha_hora[18];	// Fecha y Hora de la operación	
	int codigo;				// Inforamción de control
	int monto;				// Monto a cancelar por el vehículo
	int n_ticket;			// Número del ticket

} PDU;
