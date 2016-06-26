/* Aplicacion Servidor para el Sistema de Estacionamiento Moriah (SEM).
* Sahid Reyes 		10-10603
* Leonardo Martinez 11-10576
*
* Ultima Modificacion 22/06/16
*/

/* Archivo de cabecera*/
#include "servidor.h"

/* Imprime en pantalla un error y finaliza la ejecucion*/
void * error(char * mensaje){
	printf("%s\n", mensaje);
	exit(-1);
}

/*Registra un mensaje en la bitacora indicada*/
void * registrar(char *bitacora, PDU *pdu){
	FILE *archivo;
	if ((archivo = fopen(bitacora,"a")) == NULL) {
		error("No se pudo abrir el archivo de bitacora");
	}

	//fputc(pdu-> placa,archivo);
	// fputs(op,archivo);
	fclose(archivo);

}

/*Ingresa un vehiculo en el estacionamiento*/
void * ingresar_vehiculo(REG_VEHICULO * estacionamiento[], REG_VEHICULO * vehiculo){
  int i;
  for (i = 0; i < MAX_PUESTOS; i++){
    if (estacionamiento[i] == NULL){
      estacionamiento[i] = vehiculo;
      break;
    }
  }
}

/*Imprime todos los vehiculos en un estacionamiento (SOLO PARA DEBUGGING)*/
void * imprimir_estacionamiento(REG_VEHICULO *estacionamiento[]){
  int i;
  REG_VEHICULO * vehiculo;
  char fecha[18];

  printf("IMPRIMIENDO ESTACIONAMIENTO: \n");
  for (i = 0; i < MAX_PUESTOS; i++){
    if (estacionamiento[i] != NULL){
      vehiculo = estacionamiento[i];
			struct tm tiempo_local;
			localtime_r(&vehiculo->tiempo_entrada,&tiempo_local);
      strftime(fecha,sizeof(fecha),"%D %T", &tiempo_local);
      printf("Vehiculo en puesto %d: \n",i);
      printf(" --Placa: %d\n", vehiculo->placa );
      printf(" --Hora Entrada: %s\n", fecha);
      //free(vehiculo);
      }
  }
}

/* Retira un vehiculo del estacionamiento y retorna un puntero al vehiculo en cuestion NULL si el vehiculo no existe*/
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

/* Recibe las horas de entrada y salida y retorna el pago correspondiente del estacionamiento*/
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

/* Funcion que verifica y devuelve los argumentos de entrada*/
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


/* Si hay puestos, procesa el PDU entrante*/
void * procesar_pdu(PDU * pdu_entrante, REG_VEHICULO * estacionamiento[],
                    int * puestos_ocupados, int * numero_tickets,
                    PDU * pdu_salida){

	/*Apuntador al vehiculo a ingresar o sacar del estacionamiento*/
	REG_VEHICULO * vehiculo;

	/*Este sera el tiempo de entrada o salida del vehiculo segun corresponda*/
	time_t t = time(NULL);

	switch(pdu_entrante->tipo_paq) {

		/* Corresponde a una solicitud de entrada*/
		case 'e':

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

			if (*puestos_ocupados < 3) {

			    char fecha[18];

			    time_t t1 = time(NULL);
			    struct tm *tmp;
			    tmp = localtime(&t1);

			    strftime(fecha,sizeof(fecha),"%D %T",tmp);
			    pdu_salida-> puesto = true;
			    strcpy(pdu_salida->fecha_hora,fecha);
			    pdu_salida-> codigo = *numero_tickets;

			} else {
			    pdu_salida-> puesto = false;
			}

			/*Incrementamos el numero de puestos y el numero de los tickets*/
			*puestos_ocupados = *puestos_ocupados + 1;
			*numero_tickets   = *numero_tickets + 1;

			/*Imprimimos el vehiculo solo para verificar*/
			imprimir_estacionamiento(estacionamiento);
			printf("Numero de puestos ocupados : %d\n", *puestos_ocupados);

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
				printf("El vehiculo con placa %d no existe o hubo un error.\n", pdu_entrante->placa);
				/* Aqui le enviamos al cliente el mensaje correspondiente*/

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
					pdu_salida-> codigo = vehiculo->ticket;
					pdu_salida->monto   = monto_a_pagar;

					printf("El vehiculo retirado es:  %d\n", vehiculo->placa);
					/*Liberamos la memoria del vehiculo*/
					free(vehiculo);
					/*Imprimimos el estacionamiento solo para verificar*/
					imprimir_estacionamiento(estacionamiento);
					/*Decrementamos el numero de puestos*/
					*puestos_ocupados = *puestos_ocupados - 1;
					printf("Numero de puestos ocupados : %d\n", *puestos_ocupados);
			}

			break;
		}
}

void * salida(PDU * pdu_salida){

	pdu_salida-> fuente = true;
	//pdu_salida-> placa = pdu_entrante->placa;
	char fecha[18];

	time_t t1 = time(NULL);
	struct tm *tmp;
	tmp = localtime(&t1);

	strftime(fecha,sizeof(fecha),"%D %T",tmp);
	pdu_salida-> puesto = true;
	strcpy(pdu_salida->fecha_hora,fecha);
	pdu_salida-> codigo = 1;

}

void * salida1 (int m){
	m = 1000;
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
	char *bitacora_entrante  = (char *) malloc(512*sizeof(char));

	/*Leemos los argumentos y los asignamos a las variables respectivas.*/
	leer_args(argc,argv,&numero_puerto, bitacora_entrada, bitacora_entrante);
	printf("Este es el archivo de entrada (afuera): %s\n", bitacora_entrada);
	printf("Este es el archivo de salida (afuera): %s\n", bitacora_entrante);
	printf("Este es el numero de puerto (afuera): %d\n", numero_puerto );

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

	/* PDU Informacion se envia solo cuando no hay puestos o cuando ocurre un error */
	pdu_informacion -> tipo_paq = 'o';
	pdu_informacion -> fuente = true;
	pdu_informacion -> puesto = false;


	/*creamos el socket*/
	printf("Creando el socket.\n");
	if ((socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		error("No se pudo crear el socket.");
	}

	memset((char *) &datos_servidor, 0, sizeof(datos_servidor));
	memset((char *) &datos_cliente, 0, sizeof(datos_cliente));

	/* Asignamos los datos del servidor*/
	datos_servidor.sin_family = AF_INET;
	datos_servidor.sin_port = htons(numero_puerto);
	datos_servidor.sin_addr.s_addr = INADDR_ANY; // Para que escuche de cualquier IP.
	bzero(&(datos_servidor.sin_zero),8);

	/*Unimos el socket con los datos del servidor*/

	printf("Uniendo el socket con la direccion.\n");
	if (bind(socketfd, (struct sockaddr*)&datos_servidor, sizeof(struct sockaddr)) == -1) {
		error("Error uniendo el socket con los datos del servidor.");
	}

	while(1){

		printf("\nEsperando datos del cliente...\n");

		if ((numero_bytes = recvfrom(socketfd, pdu_entrante, sizeof(PDU), 0,
									(struct sockaddr*) &datos_cliente,
									(socklen_t *) &tam_direccion)) == -1){
			error("Error recibiendo datos del cliente.");

		} else {

			printf("\nMensaje recibido de %s\n",inet_ntoa(datos_cliente.sin_addr));
			printf("Longitud del PDU en bytes %d\n",numero_bytes);
			printf("\nTipo de paquete: %c\n", pdu_entrante-> tipo_paq);
			printf("Orígen del paquete: %d\n", pdu_entrante-> fuente);
			printf("Placa del vehiculo: %d\n", pdu_entrante-> placa);

			/* Es un apuntador al PDU que efectivamente se enviara*/
			PDU * pdu_respuesta;

			if (puestos_ocupados == MAX_PUESTOS && pdu_entrante -> tipo_paq == 'e') {
				pdu_respuesta = pdu_informacion;
			} else {
				procesar_pdu(pdu_entrante,estacionamiento,
										 &puestos_ocupados, &numero_tickets, pdu_salida);
				pdu_respuesta = pdu_salida;
			}

//			salida(pdu_salida);

			int m;
			m = 50;
			salida1(m);

			// Se envia la respusta del servidor
			if (numero_bytes = sendto(socketfd,pdu_respuesta,sizeof(PDU),0,
									(struct sockaddr*) &datos_cliente,
									sizeof(struct sockaddr)) == -1){
				error("Error enviando datos al cliente.");
			}

		}

	}

	close(socketfd);
	exit(0);

}
