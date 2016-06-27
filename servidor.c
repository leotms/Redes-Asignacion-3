/* Aplicacion Servidor para el Sistema de Estacionamiento Moriah (SEM).
* Ultima Modificacion 26/06/16
* @author Sahid Reyes 	  	10-10603
* @author Leonardo Martinez 11-10576
*/

/* Archivo de cabecera*/
#include "servidor.h"

void * error(char * mensaje){
	printf("%s\n", mensaje);
	exit(-1);
}

void * registrar(char *bitacora_entrada, char *bitacora_salida ,PDU *pdu_entrante){

	FILE *archivo_in;
	if ((archivo_in = fopen(bitacora_entrada,"a")) == NULL) {
		error("No se pudo abrir el archivo de bitacora");
	}

	FILE *archivo_out;
	if ((archivo_out = fopen(bitacora_salida,"a")) == NULL) {
		error("No se pudo abrir el archivo de bitacora");
	}

	char fecha[18];
	time_t t1 = time(NULL);
	struct tm *tmp;
	tmp = localtime(&t1);
	strftime(fecha,sizeof(fecha),"%D %T",tmp);

	switch(pdu_entrante->tipo_paq) {

		case 'e':

			fprintf(archivo_in,"Tipo de Operación: %c\n",pdu_entrante->tipo_paq);
			fprintf(archivo_in,"Fecha y hora %s\n",pdu_entrante->fecha_hora);
			fprintf(archivo_in,"Placa: %d\n",pdu_entrante-> placa);
			fprintf(archivo_in,"N° Ticket: %d\n",pdu_entrante-> n_ticket);
			fprintf(archivo_in,"%s\n","Solicitud atendida satisfactoriamente");
			fprintf(archivo_in,"%s\n","--------------------------------------");
			fclose(archivo_in);
			break;

		case 's':

			fprintf(archivo_out,"Tipo de Operación: %c\n",pdu_entrante->tipo_paq);
			fprintf(archivo_out,"Fecha y hora %s\n",pdu_entrante->fecha_hora);
			fprintf(archivo_out,"Placa: %d\n",pdu_entrante-> placa);
			fprintf(archivo_out,"N° Ticket: %d\n",pdu_entrante-> n_ticket);
			fprintf(archivo_out,"Monto a cancelar: %d Bs\n",pdu_entrante->monto);
			fprintf(archivo_out,"%s\n","Solicitud atendida satisfactoriamente");
			fprintf(archivo_out,"%s\n","--------------------------------------");
			fclose(archivo_out);
			break;

		case 'o':
			switch (pdu_entrante->codigo) {

				case 0: // estacionamiento sin puestos
					fprintf(archivo_in,"%s\n","Tipo de Operación: e");
					fprintf(archivo_in,"Fecha y hora %s\n",fecha);
					fprintf(archivo_in,"%s\n","Solicitud no atendida: Estacionamiento lleno");
					fprintf(archivo_in,"%s\n","--------------------------------------");
					fclose(archivo_in);
					break;

				case 1: // vehiculo exitente
					fprintf(archivo_in,"%s\n","Tipo de Operación: e");
					fprintf(archivo_in,"Fecha y hora %s\n",fecha);
					fprintf(archivo_in,"%s\n","Solicitud no atendida: Vehículo ya estacionado");
					fprintf(archivo_in,"%s\n","--------------------------------------");
					fclose(archivo_in);
					break;

				case 2: // vehiculo no esta en el estacionamiento
					fprintf(archivo_out,"%s\n","Tipo de Operación: s");
					fprintf(archivo_out,"Fecha y hora %s\n",fecha);
					fprintf(archivo_out,"%s\n","Solicitud no atendida: Vehículo inexistente");
					fprintf(archivo_out,"%s\n","--------------------------------------");
					fclose(archivo_out);
					break;
			}
		break;

	}
}

int buscar_vehiculo(REG_VEHICULO * estacionamiento[], int * placa){
	int i;
	int existe = 0;
	for (i = 0; i < MAX_PUESTOS; i++){
		if (estacionamiento[i] != NULL && estacionamiento[i]->placa == *placa){
			existe = 1;
			break;
		}
	}
	return existe;
}

void * ingresar_vehiculo(REG_VEHICULO * estacionamiento[], REG_VEHICULO * vehiculo){
  int i;
  for (i = 0; i < MAX_PUESTOS; i++){
    if (estacionamiento[i] == NULL){
      estacionamiento[i] = vehiculo;
      break;
    }
  }
}

REG_VEHICULO * retirar_vehiculo(REG_VEHICULO * estacionamiento[], int * placa) {
  int i;
  REG_VEHICULO * vehiculo = NULL;
  for (i = 0; i < MAX_PUESTOS; i++){
    if (estacionamiento[i] != NULL && estacionamiento[i]->placa == *placa){
      vehiculo = estacionamiento[i];
      estacionamiento[i] = NULL;
      break;
    }
  }
  return vehiculo;
}

int calcular_pago(time_t hora_entrada, time_t hora_salida){
	int pago;
	double tiempo_total = difftime(hora_salida,hora_entrada);
	if (tiempo_total <= 3600.0){
		pago = 80;
	} else {
		pago = 80;
		tiempo_total = tiempo_total - 3600.0;
		while (tiempo_total > 0.0) {
			pago = pago + 30;
			tiempo_total = tiempo_total - 3600.0;
		}
	}

	printf("Se debe pagar %d Bs.\n",pago);
	return pago;

}

void * leer_args(int argc, char *argv[], int *numero_puerto,
               char *bitacora_entrada, char *bitacora_entrante){
	if (argc < 7) {
		error("Error en el numero de argumentos.");
	}

	/* Identificadores de los argumentos */
	char *portID = "-l";
	char *bitEnt = "-i";
	char *bitSal = "-o";

	int i;
	for (i = 1; i < argc; ++i){

		/*Identificamos cada argumento de entrada y lo asignamos donde corresponda*/
		if ((strcmp(argv[i], portID)) == 0){
			char *endpt;
			*numero_puerto = strtol(argv[i+1],&endpt,10);
		} else if ((strcmp(argv[i], bitEnt)) == 0) {
			strcpy(bitacora_entrada,argv[i+1]);
		} else if ((strcmp(argv[i], bitSal)) == 0) {
			strcpy(bitacora_entrante, argv[i+1]);
		}
	}
}

int procesar_pdu(PDU * pdu_entrante, REG_VEHICULO * estacionamiento[],
                    int * puestos_ocupados, int * numero_tickets,
                    PDU * pdu_salida){

	/*Apuntador al vehiculo a ingresar o sacar del estacionamiento*/
	REG_VEHICULO * vehiculo;
	int res = 0;

	/*Este sera el tiempo de entrada o salida del vehiculo segun corresponda*/
	time_t t = time(NULL);

	switch(pdu_entrante->tipo_paq) {

		/* Corresponde a una solicitud de entrada*/
		case 'e':


			if (buscar_vehiculo(estacionamiento,&pdu_entrante->placa)) {
				res = 1; // Vehiculo ya esta en el estacionamiento.
			} else {
					/*Como la operacion es de entrada, reservamos la memoria para el nuevo
					 * vehiculo y le asignamos los valores correspondientes.*/
					vehiculo = (REG_VEHICULO *) calloc(1,sizeof(REG_VEHICULO));
					memcpy(&vehiculo->placa, &pdu_entrante->placa, sizeof(int));
					memcpy(&vehiculo->ticket,numero_tickets,sizeof(int));
					memcpy(&vehiculo->tiempo_entrada,&t,sizeof(time_t));

					// se le hace strftime a tiempo_entrada para el PDU
					struct tm tiempo_entrada;
					localtime_r(&t,&tiempo_entrada);

					/*Ingresamos el vehiculo al estacionamiento*/
					ingresar_vehiculo(estacionamiento,vehiculo);

					/* Aqui le enviamos al cliente el mensaje correspondiente*/

					pdu_salida-> tipo_paq = 'e';
					pdu_salida-> fuente = true;
					pdu_salida-> placa = pdu_entrante->placa;
					pdu_salida-> n_ticket = *numero_tickets;

					if (*puestos_ocupados < MAX_PUESTOS - 1){

					    char fecha[18];

					    time_t t1 = time(NULL);
					    struct tm *tmp;
					    tmp = localtime(&t1);

					    strftime(fecha,sizeof(fecha),"%D %T",tmp);
					    pdu_salida-> puesto = true;
					    strcpy(pdu_salida->fecha_hora,fecha);

					} else {
						/*Significa que el vehiculo tomo el ultimo puesto disponible. */
					    pdu_salida-> puesto = false;
					}

					/*Incrementamos el numero de puestos y el numero de los tickets*/
					*puestos_ocupados = *puestos_ocupados + 1;
					*numero_tickets   = *numero_tickets + 1;

			}
			break;
		case 's' :

			/*En este caso, vehiculo corresponde el vehiculo que sacaremos del estacionamiento*/
			vehiculo = retirar_vehiculo(estacionamiento, &pdu_entrante->placa);

			// Aqui se calcula el tiempo en el que el carro salio del estacionamiento
		  // se le hace strftime a tiempo_salida para el PDU
			struct tm tiempo_salida;
			localtime_r(&t,&tiempo_salida);

			/*Si vehiculo retorna como NULL, entonces no exitia o hubo un error.*/
			if (vehiculo == NULL) {
				res = 2;  // El vehiculo buscado no se encuentra en el estacionamiento.
			} else {

					//Se calcula el tipo de pago.
					int monto_a_pagar = calcular_pago(vehiculo->tiempo_entrada,t);

					/* Aqui le enviamos al cliente el mensaje correspondiente*/

					pdu_salida-> tipo_paq = 's';
					pdu_salida-> fuente   = true;
					pdu_salida-> placa    = pdu_entrante->placa;

					char fecha[18];

					time_t t1 = time(NULL);
					struct tm *tmp;
					tmp = localtime(&t1);

					strftime(fecha,sizeof(fecha),"%D %T",tmp);
					pdu_salida-> puesto = true;
					strcpy(pdu_salida->fecha_hora,fecha);
					pdu_salida->n_ticket = vehiculo->ticket;
					pdu_salida->monto   = monto_a_pagar;

					/*Liberamos la memoria del vehiculo*/
					free(vehiculo);
					/*Decrementamos el numero de puestos*/
					*puestos_ocupados = *puestos_ocupados - 1;

			}

			break;
		}

		return res;
}

/* Programa Principal*/
void main(int argc, char *argv[]) {

	/* Inicializamos el estacionamiento con punteros a NULL.*/
	REG_VEHICULO * estacionamiento[MAX_PUESTOS] = {NULL};

	/*Numero de pustos ocupados en el estacionamiento*/
	int puestos_ocupados = 0;

	/* Numero de los tickets */
	int numero_tickets   = 0;

	/*Almacentaran los argumentos recibidos de la linea de comandos*/
	int numero_puerto;
	char *bitacora_entrada = (char *) malloc(512*sizeof(char));
	char *bitacora_salida  = (char *) malloc(512*sizeof(char));

	/*Leemos los argumentos y los asignamos a las variables respectivas.*/
	leer_args(argc,argv,&numero_puerto, bitacora_entrada, bitacora_salida);

	/*guardaran las los datos del cliente y servidor*/
	struct sockaddr_in datos_servidor, datos_cliente;

	/* socketfd: descriptor del socket.
	* tam_direccion: tama#o de la estructura sockaddr_in
	* numero_bytes: himero de bytes recibidos.
	*/
	int socketfd, tam_direccion, numero_bytes;

	/* Contendra el PDU entrante/saliente */

	PDU *pdu_entrante;
	pdu_entrante =    (PDU *) calloc(1,sizeof(PDU));

	PDU *pdu_salida;
	pdu_salida =      (PDU *) calloc(1,sizeof(PDU));

	PDU *pdu_informacion;
	pdu_informacion = (PDU *) calloc(1,sizeof(PDU));

	/* pdu_informacion se envia solo cuando no hay puestos o cuando ocurre un error */
	/* En cualquier otro caso, se envia pdu_salida*/
	pdu_informacion -> tipo_paq = 'o';
	pdu_informacion -> fuente = true;
	pdu_informacion -> puesto = false;


	/*creamos el socket*/
	printf("Iniciando Computador Central del SEM.\n");
	if ((socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		error("No se pudo crear el Socket para la conexion del SEM.");
	}

	/* Inicializamos los buffers de las direcciones del cliente y del servidor*/
	memset((void *) &datos_servidor, 0, sizeof(datos_servidor));
	memset((void *) &datos_cliente, 0, sizeof(datos_cliente));
  tam_direccion = sizeof(datos_cliente);

	/* Asignamos los datos del servidor*/
	datos_servidor.sin_family = AF_INET;
	datos_servidor.sin_port = htons(numero_puerto);
	datos_servidor.sin_addr.s_addr = INADDR_ANY; // Para que escuche de cualquier IP.
	bzero(&(datos_servidor.sin_zero),8);

	/*Unimos el socket con los datos del servidor*/

	if (bind(socketfd, (struct sockaddr*)&datos_servidor, sizeof(struct sockaddr)) == -1) {
		error("Error uniendo el Socket con la direccion IP y Puerto para el CC de SEM.");
	}

	printf("Computador Central del SEM activo y escuchando peticiones en el puerto %d\n",numero_puerto);

	while(1){

		printf("\nEsperando solicitudes de los clientes...\n");

		if ((numero_bytes = recvfrom(socketfd, pdu_entrante, sizeof(PDU), 0,
									(struct sockaddr*) &datos_cliente,
									(socklen_t *) &tam_direccion)) == -1){
			print("Error recibiendo datos del cliente.");
		} else {

			printf("\nSolicitud de cliente recibida desde la IP %s. ",inet_ntoa(datos_cliente.sin_addr));
			printf("Atendiendo solicitud... ");

			/* Es un apuntador al PDU que efectivamente se enviara*/
			PDU * pdu_respuesta;

			if (puestos_ocupados == MAX_PUESTOS && pdu_entrante -> tipo_paq == 'e') {
				pdu_informacion->codigo = 0;
				pdu_respuesta = pdu_informacion;

			} else {

				int resultado = procesar_pdu(pdu_entrante,estacionamiento,
										 								 &puestos_ocupados, &numero_tickets, pdu_salida);

				switch (resultado) {
					case 0: // Operacion procesada satisfactoriamente
						pdu_respuesta = pdu_salida;
						break;
					case 1: // El vehiculo ya existia en el estacionamiento      (para la entrada)
					  pdu_informacion->codigo = 1;
						pdu_respuesta = pdu_informacion;
						break;
					case 2: // El Vehiculo no se encuentra en el estacionamiento (para la salida)
						pdu_informacion->codigo = 2;
						pdu_respuesta = pdu_informacion;
						break;
				}
			}

			printf("solicitud atendida. \n");
			printf("Enviando respuesta al cliente... ");

			// Se envia la respuesta del servidor
			if (numero_bytes = sendto(socketfd,pdu_respuesta,sizeof(PDU),0,
									(struct sockaddr*) &datos_cliente,
									(socklen_t) tam_direccion) == -1){
				perror("No se pudo enviar respuesta de solicitud al cliente. Error: ");
			} else {
				printf("respuesta enviada satisfactoriamente.\n");
			}
			// Registramos la operacion en la bitacora
			registrar(bitacora_entrada,bitacora_salida,pdu_respuesta);
		}

	}

	close(socketfd);
	exit(0);

}
