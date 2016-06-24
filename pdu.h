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

	char tipo_paq;
	bool fuente;
	bool puesto;
	int placa;
	char fecha_hora[18];
	int codigo;
	int monto;

} PDU;
