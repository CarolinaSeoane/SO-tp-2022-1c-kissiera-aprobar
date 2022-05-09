#include "../include/ciclo_instruccion.h"

void ejecutar_ciclo_instruccion(Proceso_CPU* proceso, void* void_args) {
    instruccion inst;
    log_info(logger, "Ejecutando ciclo de instruccion del proceso %d", (*proceso).pid);
    fetch(proceso, &inst);
    while(!flag_interrupcion) {
        log_info(logger, "Fetch encontro la instruccion %d con PARAM1 = %d, PARAM2 = %d", inst.id_operacion, inst.operando1, inst.operando2);
        bool es_copy = decode(inst.id_operacion);
        
	    if (es_copy) { 
            // fetch_operands(&proceso); // Para que saque el segundo operando y se comunique con memoria
        }
        
        (*proceso).program_counter++;
        execute(proceso, inst, void_args);
        check_interrupt(void_args); 
        fetch(proceso, &inst);
    }
	// atender_interrupcion();

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

void execute(Proceso_CPU* proceso, instruccion inst, void* void_args) {
	args_dispatch* args = (args_dispatch*) void_args;

	switch(inst.id_operacion) {

		case NO_OP:
			log_info(logger, "Proceso %d ejecuta NO_OP", (*proceso).pid);
			sleep(args->config.RETARDO_NOOP / 1000);
			break;
		case IO:;
			log_info(logger, "Proceso %d ejecuta IO", (*proceso).pid);
			flag_interrupcion = 1;
			send_proceso_bloqueado(proceso, inst.operando1, args);
			//uint32_t bloqueo = inst.operando1;
			//aca solo hace falta devolverle el pid, pc y el bloqueo que lei recien. dsp en kernel hay que buscar el proceso con ese pid, actualizarle el pc y mandarlo a bloqueo
			break;
		case READ:;
			log_info(logger, "Proceso %d ejecuta READ", (*proceso).pid);
			int leer = inst.operando1;

			int* codigo = malloc(sizeof(int));
			*codigo = READ_M;

			void* paquete = malloc(sizeof(int)*2);

			int offset = 0;
			memcpy(paquete, &(*codigo), sizeof(int));
			offset += sizeof(int);
			memcpy(paquete + offset, &(leer), sizeof(int));

			send(args->con_memoria, paquete, sizeof(int)*2, 0);

			free(codigo);
			free(paquete);

			int leido;
			recv(args->con_memoria, &leido, sizeof(int), 0);

			log_info(logger, "El valor leido es %d", leido);
			break;
		case WRITE:
			log_info(logger, "Proceso %d ejecuta WRITE", (*proceso).pid);
			break;
		case COPY:
			log_info(logger, "Proceso %d ejecuta COPY", (*proceso).pid);
			break;
		case EXIT:
			log_info(logger, "Proceso %d ejecuta EXIT", (*proceso).pid);
			flag_interrupcion = 1;
			send_proceso_finalizado(proceso, args);
			break;
		default:
			log_error(logger, "Operacion desconocida");
	}
}


void check_interrupt(void* void_args) {
	args_dispatch* args = (args_dispatch*) void_args;

	/* 
	No se puede poner un recv aca porque seria bloqueante. 
	Habria que usar una funcion que consulte si hay algo o 
	tener el recv en otro hilo y que vaya pasando las interrupciones 
	a una lista
	*/

}