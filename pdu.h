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

#define MAX_PDU_LENGTH 200

typedef struct pdu_IO{

	char tipo_paq;
	bool fuente;
	bool puesto;
	int placa;
	time_t fecha_hora;
	int codigo;
	int monto;

} PDU;
