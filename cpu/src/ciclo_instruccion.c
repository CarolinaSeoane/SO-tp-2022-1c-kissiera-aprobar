#include "../include/ciclo_instruccion.h"

void ejecutar_ciclo_instruccion(Proceso_CPU* proceso, Config config, int dispatch, int conexion_mem) {
    instruccion inst;
    log_info(logger, "Ejecutando ciclo de instruccion del proceso %d", (*proceso).pid);
    fetch(proceso, &inst);
    while(inst.id_operacion != EXIT) {
        log_info(logger, "Fetch encontro la instruccion %d con PARAM1 = %d, PARAM2 = %d", inst.id_operacion, inst.operando1, inst.operando2);
        bool es_copy = decode(inst.id_operacion);
        
	    if (es_copy) { 
            // fetch_operands(&proceso); // Para que saque el segundo operando y se comunique con memoria
        }
        
        execute(proceso, inst, config, dispatch, conexion_mem);
        //check_interrupt();  */
        (*proceso).program_counter++;
        fetch(proceso, &inst);
    }

}

/* Fetch: buscar la próxima instrucción a ejecutar con el PC */
void fetch(Proceso_CPU* proceso, instruccion* inst) {
	int offset = (*proceso).program_counter * sizeof(instruccion);
	memcpy(inst, proceso->stream + offset, sizeof(instruccion));
}

/* Decode: interpretar que instrucción es la que se va a ejecutar para ver si se necesita fetch operands.*/ 
bool decode(int co_op) {
    return co_op == COPY;
}

void fetch_operands(Proceso_CPU* proceso) {
	// TO DO
}

void execute(Proceso_CPU* proceso, instruccion inst, Config config, int dispatch, int conexion_mem) {

	switch(inst.id_operacion) {

		case NO_OP:
			sleep(config.RETARDO_NOOP / 1000);
			break;
		case IO:;
			uint32_t bloqueo = inst.operando1;
			//aca solo hace falta devolverle el pid, pc y el bloqueo que lei recien. dsp en kernel hay que buscar el proceso con ese pid, actualizarle el pc y mandarlo a bloqueo

			break;
		case READ:;

			int leer = inst.operando1;

			int* codigo = malloc(sizeof(int));
			*codigo = READ_M;

			void* paquete = malloc(sizeof(int)*2);

			int offset = 0;
			memcpy(paquete, &(*codigo), sizeof(int));
			offset += sizeof(int);
			memcpy(paquete + offset, &(leer), sizeof(int));

			send(conexion_mem, paquete, sizeof(int)*2, 0);

			free(codigo);
			free(paquete);

			int leido;
			recv(conexion_mem, &leido, sizeof(int), 0);

			log_info(logger, "El valor leido es %d", leido);
			
			break;
		case WRITE:
			
			break;
		case COPY:
			
			break;
		case EXIT:
			
			break;
		default:
			log_error(logger, "Operacion desconocida");
	}
}