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

int main(int argc, char** argv) {

	logger = log_create("consola.log", "Consola", 1, LOG_LEVEL_DEBUG);
	
	log_info(logger, "-------- COMIENZO --------");
    log_info(logger, "Inicia el módulo Consola");

    if(argc != 3) {
    	log_error(logger, "Está iniciando mal este proceso. Tiene %d parámetros cuando deberia tener 2\n", argc-1);
    	return EXIT_FAILURE;
    }

    log_info(logger, "El proceso tiene tamanio %s", argv[2]);
    log_info(logger, "Sus instrucciones estan en %s", argv[1]);

    log_info(logger, "------ PASAJE DE INSTRUCCIONES -------");
    char* contenido = malloc(sizeof(char*));
    int len = 0;
    int cantidad_de_instrucciones = devolver_cantidad_de_instrucciones(argv[1]);
    log_info(logger, "------ CANTIDAD DE INSTRUCCIONES EN ARCHIVO %d-------", cantidad_de_instrucciones);

    log_info(logger, "----- APERTURA DE ARCHIVO ------");
    FILE* archivo = fopen(argv[1], "r");

    if(archivo == NULL) {
    	log_error(logger, "El archivo no es correcto");
    	return EXIT_FAILURE;
    }
    log_info(logger, "------ ARMANDO STREAM CON TODAS LAS INSTRUCCIONES -------");

    void* stream = malloc(cantidad_de_instrucciones*sizeof(instruccion));
    int offset=0;
    char* aux = malloc(sizeof(char*));
    while (getline(&contenido, &len, archivo) != -1)
        {
    		strcpy(aux, contenido);
    		instruccion* instr = armar_y_devolver_instruccion(aux);

    		if(instr->id_operacion == 0){

    			char** tokens_string = string_split(contenido, " ");
    		    log_info(logger, "------ INICIO DE REPETICIONES ------- %d", atoi(tokens_string[1]));

    		    for (int i = 0; i < atoi(tokens_string[1]); i++)
    		    {
    				instr->operando1 = 0;
    				instr->operando2 = 0;
    				log_info(logger, "id_operacion: %d - operando1: %d - operando2: %d", instr->id_operacion, instr->operando1, instr->operando2 );
    		    	memcpy(stream+offset, &(instr->id_operacion), sizeof(operacion));
    				offset += sizeof(operacion);
    				memcpy(stream+offset, &(instr->operando1), sizeof(uint32_t));
    				offset += sizeof(uint32_t);
    				memcpy(stream+offset, &(instr->operando2), sizeof(uint32_t));
    				offset += sizeof(uint32_t);
    			}

    		    log_info(logger, "------ INICIO DE OPERACIONES NORMALES -------");
    		    string_array_destroy(tokens_string);
    		}else
    		{

    			log_info(logger, "id_operacion: %d - operando1: %d - operando2: %d", instr->id_operacion, instr->operando1, instr->operando2 );
    			memcpy(stream+offset, &(instr->id_operacion), sizeof(operacion));
				offset += sizeof(operacion);
				memcpy(stream+offset, &(instr->operando1), sizeof(uint32_t));
				offset += sizeof(uint32_t);
				memcpy(stream+offset, &(instr->operando2), sizeof(uint32_t));
				offset += sizeof(uint32_t);
    		}
        }
    fclose(archivo);
    log_info(logger, "------ ARMANDO PAQUETE PARA USAR EN VOID* A_ENVIAR -------");

    t_paquete_instrucciones* paquete = malloc(sizeof(accion)+sizeof(int)*2+sizeof(stream));
    paquete->id_accion = ENVIAR_INSTRUCCIONES;
    paquete->length_instrucciones = cantidad_de_instrucciones;
    paquete->tamanio_proceso = atoi(argv[2]);
    paquete->stream = stream;

    log_info(logger, "------ ARMANDO VOID* A_ENVIAR -------");

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

    log_info(logger, "------ ABRO CONFIG ---------");
	Config config;
	cargarConfig("consola.config", &config);

	log_info(logger, "------ CONECTO A KERNEL Y ENVIO POR SOCKET -----");

	int conexion_kernel = crear_conexion(config.IP_KERNEL, config.PUERTO_KERNEL, logger);
	send(conexion_kernel, a_enviar, sizeof(accion)+sizeof(int)+cantidad_de_instrucciones*sizeof(instruccion), 0);

	//libero punteros
	free(contenido);
	free(stream);
	free(aux);
	free(paquete);

	log_info(logger, "------COMPROBACION DEL tamanio +  INSTRUCCIONES EN EL VOID* A ENVIAR--------");

	offset = sizeof(accion)+sizeof(int);
	int tamanio_proceso;
	operacion id_operacion;
	uint32_t operando1;
	uint32_t operando2;
	// copio el tamanio primero
	memcpy(&tamanio_proceso, a_enviar+offset, sizeof(int));
	offset+=sizeof(int);
	log_info(logger, "TAMANIO DEL PROCESO: %d", tamanio_proceso);

	for(int i=0; i<cantidad_de_instrucciones; i++) {
		memcpy(&id_operacion, a_enviar+offset, sizeof(operacion));
		offset+=sizeof(operacion);
		memcpy(&operando1, a_enviar+offset, sizeof(uint32_t));
		offset+=sizeof(uint32_t);
		memcpy(&operando2, a_enviar+offset, sizeof(uint32_t));
		offset+=sizeof(uint32_t);
		log_info(logger, "id_operacion: %d - operando1: %d - operando2: %d", id_operacion, operando1, operando2 );
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

	log_info(logger, "------------------ ESCUCHANDO CON RECV POR LA FINALIZACION DEL PROCESO ---------------\n");
	int proceso_finalizado;
	recv(conexion_kernel, &proceso_finalizado, sizeof(int), MSG_WAITALL);
	if(proceso_finalizado == 1) {
		log_info(logger, "Proceso finalizado con codigo %d", proceso_finalizado);
		return EXIT_SUCCESS;
	} else {
		log_error(logger, "Proceso finalizado con codigo de error %d", proceso_finalizado);
		return -1;
	}
}
