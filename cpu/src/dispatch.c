#include "../include/dispatch.h"

void mostrar_instrucciones(void* stream, int len_instrucciones){

	int offset=0;
	operacion id_operacion;
	uint32_t operando1;
	uint32_t operando2;

	for(int i=0; i<len_instrucciones; i++) {
		memcpy(&id_operacion, stream+offset, sizeof(operacion));
		offset+=sizeof(operacion);
		memcpy(&operando1, stream+offset, sizeof(uint32_t));
		offset+=sizeof(uint32_t);
		memcpy(&operando2, stream+offset, sizeof(uint32_t));
		offset+=sizeof(uint32_t);
		log_info(logger, "id_operacion: %d - operando1: %d - operando2: %d", id_operacion, operando1, operando2);
	}
	log_info(logger, "------------------ DONE ---------------\n");
} //dsp borrar


void recv_proceso(Proceso_CPU* proceso, args_dispatch* args) {

	recv(args->cliente_dispatch_fd, &(proceso->pid), sizeof(int), 0);

	recv(args->cliente_dispatch_fd, &(proceso->program_counter), sizeof(int), 0);

	recv(args->cliente_dispatch_fd, &(proceso->tabla_paginas), sizeof(int), 0);

	int tam_stream;
	recv(args->cliente_dispatch_fd, &tam_stream, sizeof(int), 0);

	proceso->stream = malloc(tam_stream);
	recv(args->cliente_dispatch_fd, proceso->stream, tam_stream, 0);
	mostrar_instrucciones(proceso->stream, tam_stream/sizeof(instruccion));

}

// Si esta haciendo recv del cliente_dispatch_fd no deberia estar en el while.
// Dispatch siempre recibe EXEC_PROCESO. No deberia recibir otra cosa
void* atender_dispatch(void* void_args) {
	args_dispatch* args = (args_dispatch*) void_args;
	while(1) {
		int co_op;
		log_info(logger, "Estoy en dispatch");
		recv(args->cliente_dispatch_fd, &co_op, sizeof(co_op), 0);
		switch(co_op) {
			case EXEC_PROCESO:
				log_info(logger, "Voy a recibir un proceso para ejecutar");
                Proceso_CPU proceso;
				recv_proceso(&proceso, args);
                log_info(logger, "Proceso recibido: PID es %d - PC: %d - Tabla de páginas: %d\n\n", proceso.pid, proceso.program_counter, proceso.tabla_paginas);
				ejecutar_ciclo_instruccion(&proceso, args);
				free(proceso.stream);
				break;
			default:
				log_error(logger, "Operación desconocida");
				break;
		}	
	}
	free(args);
}
