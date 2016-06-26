/* Aplicacion Cliente para el Sistema de Estacionamiento Moriah (SEM).
* Sahid Reyes       10-10603
* Leonardo Martinez 11-10576
*
* Ultima Modificacion 22/06/16
*/

/* Archivo de cabecera*/
#include "cliente.h"

/* Imprime en pantalla un error y finaliza la ejecucion*/
void * error(char * mensaje){
    printf("%s\n", mensaje);
    exit(-1);
}

/* Funcion que verifica y devuelve los argumentos de entrada*/
void * leer_args(int argc, char *argv[], char *dominio,
                int *numero_puerto, char *op, int *id){
    if (argc < 9) {
        error("Error en el numero de argumentos.");
    }

    /* Identificadores de los argumentos */
    char *dom_IP = "-d";
    char *portID = "-p";
    char *operIO = "-c";
    char *vehiID = "-i";

    int i;
    for (i = 1; i < argc; ++i){

        /*Identificamos cada argumento de entrada y lo asignamos donde corresponda*/
        if ((strcmp(argv[i], dom_IP)) == 0){
            strcpy(dominio, argv[i+1]);
        } else if ((strcmp(argv[i], portID)) == 0) {
            char *endpt;
            *numero_puerto = strtol(argv[i+1],&endpt,10);
        } else if ((strcmp(argv[i], operIO)) == 0) {
            strcpy(op, argv[i+1]);
        } else if ((strcmp(argv[i], vehiID)) == 0) {
            char *endid;
            *id = strtol(argv[i+1],&endid,10);
        }
    }
}



/* Programa Principal*/
int main(int argc, char *argv[]) {

    /*Almacentaran los argumentos recibidos de la linea de comandos*/
    char *dominio = (char *) malloc(512*sizeof(char));
    int numero_puerto;
    char op;
    int id;

    /*Leemos los argumentos y los asignamos a las variables respectivas.*/
    leer_args(argc,argv,dominio,&numero_puerto,&op,&id);


    /* sockfd: descriptor del socket.
    * numero_bytes: numero de bytes recibidos.
    */

    int sockfd, numero_bytes, tam_direccion;

    /* Guarda la direccion IP y numero de puerto del servidor */
    struct sockaddr_in datos_servidor;

    /* Obtiene nombre del host */
    struct hostent *host;


    /* Se convierte el hostname a su direccion IP */
    if ((host=gethostbyname(dominio)) == NULL) {
        perror("No se pudo obtener la direccion del host");
        exit(1);
    }

    /* Creamos el socket */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(2);
    }

    memset((char *) &datos_servidor, 0, sizeof(datos_servidor));

    /* Asignamos los datos del servidor */
    datos_servidor.sin_family = AF_INET;
    datos_servidor.sin_port = htons(numero_puerto);
    datos_servidor.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(datos_servidor.sin_zero), 8);

    /* PDU de entrada */
    PDU *pdu_entrante;
    pdu_entrante = (PDU *) calloc(1,sizeof(PDU));

    /* PDU de salida */
    PDU *pdu_salida;
    pdu_salida =   (PDU *) calloc(1,sizeof(PDU));


    /* Datos a enviar */
    pdu_salida-> tipo_paq = op;
    pdu_salida-> fuente = false;
    pdu_salida-> placa = id;


    /* Tiempo de Espera de respuesta del Servidor */
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
        perror("Error");
    }

    /* Se envían los datos al servidor la primera vez */
    if ((numero_bytes = sendto(sockfd,pdu_salida,sizeof(PDU),0,(struct sockaddr *)&datos_servidor,
                    sizeof(struct sockaddr))) == -1) {
        perror("sendto");
        exit(2);
    }

    memset(pdu_entrante,'\0', sizeof(PDU));


    int cont = 0;
    int numero_bytes_recibidos;

    /* Mientras no exista respuesta del servidor, intentamos hasta el maximo de intentos*/

    while ((numero_bytes_recibidos = recvfrom(sockfd, pdu_entrante, sizeof(PDU), 0,
        (struct sockaddr*) &datos_servidor, (socklen_t *) &tam_direccion)) == -1){

        if (cont == 2) {
          printf("Tiempo maximo de espera para respuesta terminado.\n");
          exit(0);
        }

        if ((numero_bytes = sendto(sockfd,pdu_salida,sizeof(PDU),0,(struct sockaddr *)&datos_servidor,
                    sizeof(struct sockaddr))) == -1) {
            perror("sendto");
            exit(2);
        }

        memset(pdu_entrante,'\0', sizeof(PDU));

      	if (numero_bytes_recibidos == -1){
            printf("Sin respuesta del servidor. Reenviando...\n");
        }


        cont++;
    }

    printf("OPERACION: %c\n",pdu_entrante->tipo_paq);


   switch(pdu_entrante->tipo_paq) {

   case 'o':

      switch (pdu_entrante->codigo) {
        case 0: // estacionamiento sin puestos
          printf("--------------------------------------");
          printf("\n|  *** ESTACIONAMIENTO LLENO ***   ");
          printf("\n|*** NO HAY PUESTOS DISPONIBLES ***\n");
          printf("|    DISCULPE LOS INCONVENIENTES    \n");
          printf("--------------------------------------\n");
          break;
        case 1: // vehiculo exitente
          printf("------------------------------------------");
          printf("\n|  *** VEHICULO YA EN EL ESTACIONAMIENTO ***   ");
          printf("\n|          CONTACTE AL ADMINISTRADOR \n");
          printf("|         DISCULPE LOS INCONVENIENTES    \n");
          printf("------------------------------------------\n");
          break;
        case 2: // vehiculo no esta en el estacionamiento
          printf("------------------------------------------");
          printf("\n|  *** VEHICULO NO ESTA EN EL ESTACIONAMIENTO ***   ");
          printf("\n|          CONTACTE AL ADMINISTRADOR \n");
          printf("|         DISCULPE LOS INCONVENIENTES    \n");
          printf("------------------------------------------\n");
          break;
      }
      break;
   case 'e':

      printf("--------------------------------------\n");
      printf("|          Ticket n°: %d              \n", pdu_entrante-> codigo);
      printf("| Hora de Entrada: %s \n", pdu_entrante->fecha_hora);
      printf("|    Placa del vehiculo: %d        \n", pdu_entrante-> placa);
      printf("--------------------------------------\n");

      if (pdu_entrante->puesto < 3){
        printf("\n*** HAY PUESTOS DISPONIBLES ***\n\n");
      } else {
        printf("\n*** NO HAY PUESTOS DISPONIBLES ***\n\n");
      }

      break;

   case 's':

      printf("--------------------------------------\n");
      printf("|          Ticket n°: %d              \n", pdu_entrante-> codigo);
      printf("| Hora de Salida: %s \n", pdu_entrante->fecha_hora);
      printf("|    Placa del vehiculo: %d        \n", pdu_entrante-> placa);
      printf("| *** Monto a Cancelar: %d ***\n",pdu_entrante->monto);
      printf("--------------------------------------\n");
      printf("\n*** HAY PUESTOS DISPONIBLES ***\n\n");
      break;

   default :

      printf("Error en el mensaje del Servidor\n");

    }


    /* cierro socket */
    close(sockfd);
    exit (0);
}
