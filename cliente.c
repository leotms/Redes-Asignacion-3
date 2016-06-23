/* Aplicacion Cliente para el Sistema de Estacionamiento Moriah (SEM).
* Sahid Reyes 		10-10603
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
                       int *numero_puerto, char *op,
                       int *id){
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

    int sockfd, numero_bytes;

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

    /* Asignamos los datos del servidor */ 
    datos_servidor.sin_family = AF_INET; 
    datos_servidor.sin_port = htons(numero_puerto); 
    datos_servidor.sin_addr = *((struct in_addr *)host->h_addr); 
    bzero(&(datos_servidor.sin_zero), 8); 

    /* PDU de salida */
    PDU *pdu_salida;
    pdu_salida = (PDU *)malloc(sizeof(PDU));

    /* Hora y Fecha del sistema */
    char fecha[18];
    time_t t = time(NULL);
    struct tm *tmp; 
    tmp = localtime(&t);
    strftime(fecha,sizeof(fecha),"%D %T",tmp);    

    int cod_ticket, m_cancelar;
    cod_ticket = 100;
    m_cancelar = 80;

    /* Datos a enviar/Recibir */
    pdu_salida-> tipo_paq = op;
    pdu_salida-> fuente = false;
    pdu_salida-> puesto = false;
    pdu_salida-> placa = id;
    pdu_salida-> fecha_hora = fecha;
    pdu_salida-> codigo = cod_ticket; 
    pdu_salida-> monto = m_cancelar; 


    /* Se envían los datos al servidor */ 
    if ((numero_bytes=sendto(sockfd,pdu_salida,MAX_PDU_LENGTH,0,(struct sockaddr *)&datos_servidor,
    sizeof(struct sockaddr))) == -1) { 
        perror("sendto"); 
        exit(2); 
    } 
    printf("enviados %d bytes hacia %s\n",numero_bytes,inet_ntoa(datos_servidor.sin_addr)); 
	printf("\nTipo de paquete: %c\n", pdu_salida-> tipo_paq);
	printf("Orígen del paquete: %d\n", pdu_salida-> fuente);
    printf("Puestos disponibles: %d\n", pdu_salida-> puesto);
    printf("Placa del vehiculo: %d\n", pdu_salida-> placa);
	printf("Hora de Entrada/Salida: %s\n", pdu_salida-> fecha_hora);
	printf("Ticket n°: %d\n", pdu_salida-> codigo);
	printf("Monto a Cancelar: %d\n", pdu_salida-> monto);
    
    /* cierro socket */ 
    close(sockfd); 
    exit (0); 
}
