#include "../include/dispatch.h"

void recv_proceso() {

}

void atender_dispatch(void* void_args) {
	args_dispatch* args = (args_dispatch*) void_args;
	int co_op;
	while(1) {		
		recv(args->cliente_fd, &co_op, sizeof(co_op), 1);
		switch(co_op) {
			case EXEC_PROCESO:
				log_info(logger, "Voy a recibir un proceso para ejecutar");
                Proceso_CPU proceso;
				recv_proceso(&proceso);
                printf("Proceso recibido: PDI es %d - PC: %d - Tabla de páginas: %d\n\n", proceso.pid, proceso.program_counter, proceso.tabla_paginas);
				// ejecutar ciclo de instruccion
				break;
			case BLOCK_PROCESO:
				break;
			default:
				break;
		}	
	}
	free(args);
}
