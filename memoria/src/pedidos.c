#include "../include/pedidos.h"

void* atender_pedido(void* void_args) {

	args_thread_memoria* args = (args_thread_memoria*) void_args;
	
	while(args->cliente_fd != -1) {

		int accion;
		recv(args->cliente_fd, &accion, sizeof(accion), 0);

		switch(accion) {
			case INIT_PROCESO: ;
				int pid;
				int tamanio_proceso;
				uint32_t tabla_primer_nivel;

				recv_proceso_init(&pid, &tamanio_proceso, args->cliente_fd);
				log_info(logger, "Recibi INIT_PROCESO PID: %d TAM: %d", pid, tamanio_proceso);

				tabla_primer_nivel = asignar_memoria_y_estructuras(pid, tamanio_proceso);
				send_tabla_primer_nivel(args->cliente_fd, tabla_primer_nivel);

				break;

			case EXIT_PROCESO_M:

				log_info(logger, "Recibi EXIT_PROCESO_M");

				break;

			case ENVIAR_TABLA_PRIMER_NIVEL:
	
				log_info(logger, "Recibi ENVIAR_TABLA_PRIMER_NIVEL");
				break;
			
			case ENVIAR_TABLA_SEGUNDO_NIVEL:

				log_info(logger, "Recibi ENVIAR_TABLA_SEGUNDO_NIVEL");
				break;

			case READ_M:;

				log_info(logger, "Recibi READ_M");
				int leer_en;
				recv(args->cliente_fd, &leer_en, sizeof(int), 0);
				//log_info(logger, "tengo que leer en la posicion %d", leer_en);

				//buscar el valor a leer
				int valor_leido = 6; // de prueba

				void* paquete = malloc(sizeof(int));

				memcpy(paquete, &valor_leido, sizeof(int));

				send(args->cliente_fd, paquete, sizeof(int), 0);

				free(paquete);

				break;

			case WRITE_M:
				
				log_info(logger, "Recibi WRITE_M");
				break;

			case HANDSHAKE_MEMORIA:

				log_info(logger, "Recibi HANDSHAKE_MEMORIA\n\n");
				send_cpu_handshake((void*) args);
				break;

			case SWAP_IN:

				log_info(logger, "Recibi SWAP_IN");
				break;

			case SWAP_OUT:

				log_info(logger, "Recibi SWAP_OUT");
				break;

			default:
				log_warning_sh(logger, "Operacion desconocida.\n\n");
				close(args->cliente_fd);
				break;
		}
	}
	free(args);
}
