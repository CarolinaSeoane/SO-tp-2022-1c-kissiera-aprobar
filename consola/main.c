#include "include/main.h"

/*

----- COSAS QUE DEBERIA HACER EL MODULO CONSOLA -------

	Validar que recibe dos parametros: el tamanio y el path ---> Hecho
	Abrir el archivo que me indica el path                  ---> Hecho
	Empezar a leer las instrucciones y validar la sintaxis  ---> Hecho
	A medida que leo, agregar la instruccion a una lista    ---> Hecho
	Conectarse a Kernel 									---> Hecho
	Enviar Lista y tamaño									---> Hecho
	Esperar respuesta										---> Falta
	Finalizar												---> Falta

---- PROBLEMAS A SOLUCIONAR -----
- Evitar repeticion de codigo
*/

instruccion* armar_y_devolver_instruccion (char* linea_leida){

	instruccion* nuevo = malloc(sizeof(instruccion));

    char* codigo = strtok(linea_leida, " ");
    nuevo->id_operacion = traer_identificador(codigo);
    if(!strcmp("NO_OP", codigo) || !strcmp("I/O", codigo) || !strcmp("READ", codigo)) {

    	nuevo->operando1 = atoi(strtok(NULL, "\n"));

    } else if (!strcmp("COPY", codigo) || !strcmp("WRITE", codigo)) {

    	nuevo->operando1 = atoi(strtok(NULL, " "));
    	nuevo->operando2 = atoi(strtok(NULL, "\n"));

    }
    return nuevo;
}

int main(int argc, char** argv) {

	printf("-------- COMIENZO --------\n");
    printf("Inicia el módulo Consola \n");


    if(argc != 3) {
    	printf("Está iniciando mal este proceso. Tiene %d parámetros cuando deberia tener 2\n", argc-1);
    	return EXIT_FAILURE;
    }

    printf("El proceso tiene tamanio %s\n", argv[2]);
    printf("Sus instrucciones estan en %s\n", argv[1]);

    printf("------ PASAJE DE INSTRUCCIONES -------\n");
    char* contenido = malloc(sizeof(char*));
    int len = 0;
    int cantidad_de_instrucciones = devolver_cantidad_de_instrucciones(argv[1]);
    printf("------ CANTIDAD DE INSTRUCCIONES EN ARCHIVO %d-------\n", cantidad_de_instrucciones);

    printf("----- APERTURA DE ARCHIVO ------\n");
    FILE* archivo = fopen(argv[1], "r");

    if(archivo == NULL) {
    	printf("El archivo no es correcto\n");
    	return EXIT_FAILURE;
    }
    printf("------ ARMANDO STREAM CON TODAS LAS INSTRUCCIONES -------\n");

    void* stream = malloc(cantidad_de_instrucciones*sizeof(instruccion));
    int offset=0;
    char* aux = malloc(sizeof(char*));
    while (getline(&contenido, &len, archivo) != -1)
        {
    		strcpy(aux, contenido);
    		instruccion* instr = armar_y_devolver_instruccion(aux);

    		if(instr->id_operacion == 0){
    			char* oper_string = malloc(sizeof(char*));
    			oper_string = strtok(contenido, " ");
    			int limit = atoi(strtok(NULL, "\n"));
    		    printf("------ INICIO DE REPETICIONES ------- %d\n", limit);

    		    for (int i = 0; i < limit; i++)
    		    {
    				instr->operando1 = 0;
    				instr->operando2 = 0;
    				printf("id_operacion: %d - operando1: %d - operando2: %d\n", instr->id_operacion, instr->operando1, instr->operando2 );
    		    	memcpy(stream+offset, &(instr->id_operacion), sizeof(operacion));
    				offset += sizeof(operacion);
    				memcpy(stream+offset, &(instr->operando1), sizeof(uint32_t));
    				offset += sizeof(uint32_t);
    				memcpy(stream+offset, &(instr->operando2), sizeof(uint32_t));
    				offset += sizeof(uint32_t);
    			}


    		    printf("------ INICIO DE OPERACIONES NORMALES ------- \n");
    		}else
    		{

    			printf("id_operacion: %d - operando1: %d - operando2: %d\n", instr->id_operacion, instr->operando1, instr->operando2 );
    			memcpy(stream+offset, &(instr->id_operacion), sizeof(operacion));
				offset += sizeof(operacion);
				memcpy(stream+offset, &(instr->operando1), sizeof(uint32_t));
				offset += sizeof(uint32_t);
				memcpy(stream+offset, &(instr->operando2), sizeof(uint32_t));
				offset += sizeof(uint32_t);
    		}
        }
    fclose(archivo);
    printf("------ ARMANDO PAQUETE PARA USAR EN VOID* A_ENVIAR -------\n");

    t_paquete_instrucciones* paquete = malloc(sizeof(accion)+sizeof(int)*2+sizeof(stream));
    paquete->id_accion = ENVIAR_INSTRUCCIONES;
    paquete->length_instrucciones = cantidad_de_instrucciones;
    paquete->tamanio_proceso = atoi(argv[2]);
    paquete->stream = stream;

    printf("------ ARMANDO VOID* A_ENVIAR -------\n");


    offset = 0;
    void* a_enviar = malloc(sizeof(accion)+sizeof(int)*2+cantidad_de_instrucciones*sizeof(instruccion));
    memcpy(a_enviar, &(paquete->id_accion), sizeof(accion));
	offset += sizeof(accion);
	memcpy(a_enviar+offset, &(paquete->length_instrucciones), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar+offset, &(paquete->tamanio_proceso), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar+offset, stream, cantidad_de_instrucciones*sizeof(instruccion));

	//int primera_operacion;
	//memcpy(&primera_operacion, paquete->stream, sizeof(Identificador));
	//printf("identificador: %d", primera_operacion);



    printf("------ ABRO CONFIG ---------\n");
	logger = log_create("consola.log", "Consola", 1, LOG_LEVEL_DEBUG);
	Config config;
	cargarConfig("consola.config", &config);

	printf("------ CONECTO A KERNEL Y ENVIO POR SOCKET -----\n");

	int conexion_kernel = crear_conexion(config.IP_KERNEL, config.PUERTO_KERNEL, logger);
	send(conexion_kernel, a_enviar, sizeof(accion)+sizeof(int)+cantidad_de_instrucciones*sizeof(instruccion), 0);


	//libero punteros
	//free(a_enviar);
	free(contenido);
	free(stream);
	free(aux);
	free(paquete);

	printf("------COMPROBACION DEL tamanio +  INSTRUCCIONES EN EL VOID* A ENVIAR--------\n");

	offset = sizeof(accion)+sizeof(int);
	int tamanio_proceso;
	operacion id_operacion;
	uint32_t operando1;
	uint32_t operando2;
	// copio el tamanio primero
	memcpy(&tamanio_proceso, a_enviar+offset, sizeof(int));
	offset+=sizeof(int);
	printf("TAMANIO DEL PROCESO: %d\n", tamanio_proceso);

	for(int i=0; i<cantidad_de_instrucciones; i++) {
		memcpy(&id_operacion, a_enviar+offset, sizeof(operacion));
		offset+=sizeof(operacion);
		memcpy(&operando1, a_enviar+offset, sizeof(uint32_t));
		offset+=sizeof(uint32_t);
		memcpy(&operando2, a_enviar+offset, sizeof(uint32_t));
		offset+=sizeof(uint32_t);
		printf("id_operacion: %d - operando1: %d - operando2: %d\n", id_operacion, operando1, operando2 );
	}

	//printf("------------------ DONE ---------------\n\n");

	//printf("------COMPROBACION DEL LAS INSTRUCCIONES EN EL STREAM A ENVIAR--------\n");

	offset = 0;
	for(int i=0; i<cantidad_de_instrucciones; i++) {
		memcpy(&id_operacion, stream+offset, sizeof(operacion));
		offset+=sizeof(operacion);
		memcpy(&operando1, stream+offset, sizeof(uint32_t));
		offset+=sizeof(uint32_t);
		memcpy(&operando2, stream+offset, sizeof(uint32_t));
		offset+=sizeof(uint32_t);
		//printf("id_operacion: %d - operando1: %d - operando2: %d\n", id_operacion, operando1, operando2 );
	}
	free(a_enviar);

	printf("------------------ ESCUCHANDO CON RECV POR LA FINALIZACION DEL PROCESO ---------------\n\n");
	int proceso_finalizado;
	recv(conexion_kernel, &proceso_finalizado, sizeof(int), MSG_WAITALL);
	// Acá se podría loguear que el proceso con las instrucciones a,b,c ...etc terminó
	return EXIT_SUCCESS;

}


int devolver_cantidad_de_instrucciones(char* path){
	FILE* archivo = fopen(path, "r");
	char* contenido = malloc(sizeof(char*));
	char* codigo_operacion = malloc(sizeof(char*));
	int len = 0;
	int lines=0;
	int adic;
	while (getline(&contenido, &len, archivo) != -1)
	{
		adic=0;
		codigo_operacion = strtok(contenido, " ");
		if (!strcmp("NO_OP", codigo_operacion)){
			adic = atoi(strtok(NULL, "\n"));
			lines = lines + adic;

		}else{
			lines++;
		}

	}
	fclose(archivo);
	return lines;

}

int es_una_instruccion_valida(char* instruccion) {
	char* codigo_operacion = malloc(sizeof(char*));
	codigo_operacion = strtok(instruccion, " ");

	if(!strcmp("NO_OP", codigo_operacion) || !strcmp("I/O", codigo_operacion) || !strcmp("READ", codigo_operacion)) {

		char* operando = malloc(sizeof(char*));
		operando = strtok(NULL, "\n");

		for(int i=0; i<strlen(operando); i++) {
			if(!isdigit(operando[i])){
				return 0;
			}
		}
		return 1;

	} else if(!strcmp("COPY", codigo_operacion) || !strcmp("WRITE", codigo_operacion)) {

		char* operando1 = malloc(sizeof(char*));
		operando1 = strtok(NULL, " ");

		if(operando1 == NULL) {
			return 0;
		}

		char* operando2 = malloc(sizeof(char*));
		operando2 = strtok(NULL, "\n");

		if(operando2 == NULL) {
			return 0;
		}

		for(int i=0; i<strlen(operando1); i++) {
			if(!isdigit(operando1[i])){
				return 0;
			}
		}

		for(int i=0; i<strlen(operando2); i++) {
			if(!isdigit(operando2[i])){
				return 0;
			}
		}
		return 1;

	} else {

		return !strcmp("EXIT\n", instruccion);

	}
}


int traer_identificador(char* codigo) {
	if(!strcmp("NO_OP", codigo)){
		return 0;
	} else if(!strcmp("I/O", codigo)) {
		return 1;
	} else if(!strcmp("READ", codigo)) {
		return 2;
	} else if(!strcmp("WRITE", codigo)) {
		return 3;
	} else if(!strcmp("COPY", codigo)) {
		return 4;
	} else {
		return 5;
	}
}
