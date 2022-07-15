#include "include/main.h"

int main(int argc, char** argv) {

	logger = log_create("consola.log", "Consola", 1, LOG_LEVEL_DEBUG);
	
    log_info(logger, "Inicia una nueva instancia de Consola");

    if(argc != 3) {
    	log_error(logger, "Está iniciando mal este proceso. Tiene %d parámetros cuando deberia tener 2\n", argc-1);
    	return EXIT_FAILURE;
    }

    log_info(logger, "El proceso tiene tamanio %s", argv[2]);
    log_info(logger, "Sus instrucciones estan en %s", argv[1]);

    char* contenido = malloc(sizeof(char));
    int len = 0;
    int cantidad_de_instrucciones = devolver_cantidad_de_instrucciones(argv[1]);
    log_info(logger, "Cantidad de instrucciones: %d", cantidad_de_instrucciones);

    FILE* archivo = fopen(argv[1], "r");

    if(archivo == NULL) {
    	log_error(logger, "El archivo no es correcto");
    	return EXIT_FAILURE;
    }

    void* stream = malloc(cantidad_de_instrucciones*sizeof(instruccion));
    int offset=0;
    char* aux = malloc(sizeof(char) * 20); //feo pero resuelve sf
    while (getline(&contenido, &len, archivo) != -1)
        {
    		strcpy(aux, contenido);
    		instruccion* instr = armar_y_devolver_instruccion(aux);

    		if(instr->id_operacion == 0){

    			char** tokens_string = string_split(contenido, " ");

    		    for (int i = 0; i < atoi(tokens_string[1]); i++)
    		    {
    				instr->operando1 = 0;
    				instr->operando2 = 0;

    		    	memcpy(stream+offset, &(instr->id_operacion), sizeof(operacion));
    				offset += sizeof(operacion);
    				memcpy(stream+offset, &(instr->operando1), sizeof(uint32_t));
    				offset += sizeof(uint32_t);
    				memcpy(stream+offset, &(instr->operando2), sizeof(uint32_t));
    				offset += sizeof(uint32_t);
    			}

    		    string_array_destroy(tokens_string);
    		}else
    		{

    			memcpy(stream+offset, &(instr->id_operacion), sizeof(operacion));
				offset += sizeof(operacion);
				memcpy(stream+offset, &(instr->operando1), sizeof(uint32_t));
				offset += sizeof(uint32_t);
				memcpy(stream+offset, &(instr->operando2), sizeof(uint32_t));
				offset += sizeof(uint32_t);
    		}
        }
    fclose(archivo);

    t_paquete_instrucciones* paquete = malloc(sizeof(accion)+sizeof(int)*2+sizeof(stream));
    paquete->id_accion = ENVIAR_INSTRUCCIONES;
    paquete->length_instrucciones = cantidad_de_instrucciones;
    paquete->tamanio_proceso = atoi(argv[2]);
    paquete->stream = stream;


    offset = 0;
    void* a_enviar = malloc(sizeof(accion)+sizeof(int)*2+cantidad_de_instrucciones*sizeof(instruccion));
    memcpy(a_enviar, &(paquete->id_accion), sizeof(accion));
	offset += sizeof(accion);
	memcpy(a_enviar+offset, &(paquete->length_instrucciones), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar+offset, &(paquete->tamanio_proceso), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar+offset, stream, cantidad_de_instrucciones*sizeof(instruccion));

	Config config;
	cargarConfig("consola.config", &config);

	log_info(logger, "Conectando a Kernel y enviando instrucciones...");
	int conexion_kernel = crear_conexion(config.IP_KERNEL, config.PUERTO_KERNEL, logger);
	send(conexion_kernel, a_enviar, sizeof(accion)+sizeof(int)*2+cantidad_de_instrucciones*sizeof(instruccion), 0);

	free(contenido);
	free(stream);
	free(aux);
	free(paquete);
	free(a_enviar);

	log_info(logger, "Esperando finalización del proceso...");
	int proceso_finalizado;
	int pid;
	recv(conexion_kernel, &proceso_finalizado, sizeof(int), MSG_WAITALL);
	recv(conexion_kernel, &pid, sizeof(int), MSG_WAITALL);
	if(proceso_finalizado == 1) {
		log_info(logger, "Proceso %d finalizado con codigo %d. Finaliza correctamente.", pid, proceso_finalizado);
		return EXIT_SUCCESS;
	} else {
		log_error(logger, "Proceso %d finalizado con codigo de error %d", pid, proceso_finalizado);
		return -1;
	}
}
