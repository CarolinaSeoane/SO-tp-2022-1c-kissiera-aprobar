#include "../include/dispatch.h"

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
