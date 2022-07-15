#include "../include/dispatch.h"

void* atender_dispatch() {
	while(cliente_dispatch != -1) {
		int co_op;
		log_info(logger, "CPU esperando proceso en dispatch...");
		recv(cliente_dispatch, &co_op, sizeof(co_op), 0);

		switch(co_op) {
			case EXEC_PROCESO:
				log_info(logger, "Voy a recibir un proceso para ejecutar");
                Proceso_CPU proceso;
				recv_proceso(&proceso);
                log_info(logger, "Proceso recibido: PID es %d - PC: %d - Tabla de páginas: %d\n\n", proceso.pid, proceso.program_counter, proceso.tabla_paginas);
				ejecutar_ciclo_instruccion(&proceso);
				break;
			default:
				log_error(logger, "Operación desconocida");
				break;
		}	
	}
}
