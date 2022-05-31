#include "../include/interrupt.h"

void* atender_interrupt(void* void_args) {
	args_interrupt* args = (args_interrupt*) void_args;

	while(1) {
		uint32_t codigo;
		recv(args->cliente_interrupt_fd, &codigo, sizeof(codigo), 0);
		switch(codigo) {
			case INTERRUPCION:
				log_info(logger, "Se recibió una interrupción");
				pthread_mutex_lock(&mutex_flag_interrupcion);
				flag_interrupcion = 1;
				pthread_mutex_unlock(&mutex_flag_interrupcion);
				break;
			default:
				log_error(logger, "Operación desconocida");
				break;
		}
	}
}