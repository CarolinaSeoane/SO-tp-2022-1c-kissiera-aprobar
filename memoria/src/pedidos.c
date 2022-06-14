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

			case ENVIAR_TABLA_PRIMER_NIVEL: ;

				int pid_proceso;
				int index_tabla_primer_nivel;
				recv(args->cliente_fd, &pid_proceso, sizeof(int), 0);
				recv(args->cliente_fd, &index_tabla_primer_nivel, sizeof(int), 0);
				log_info(logger, "Recibi ENVIAR_TABLA_PRIMER_NIVEL");
				log_info(logger, "Index %d y pid %d", index_tabla_primer_nivel, pid_proceso);
				
				pthread_mutex_lock(&mutex_lista_primer_nivel);
				Tabla_Primer_Nivel* t_primer_nivel = list_get(lista_tablas_primer_nivel, pid_proceso);
				pthread_mutex_unlock(&mutex_lista_primer_nivel);

				Entrada_Tabla_Primer_Nivel* entrada_tabla_primer_nivel = list_get(t_primer_nivel->entradas_tabla_primer_nivel, index_tabla_primer_nivel);

				log_info(logger, "Envio index tabla segundo nivel %d\n\n", entrada_tabla_primer_nivel->index_tabla_segundo_nivel);
				send_tabla_segundo_nivel(args->cliente_fd, entrada_tabla_primer_nivel->index_tabla_segundo_nivel);
				

				break;
			
			case ENVIAR_TABLA_SEGUNDO_NIVEL: ;

				int index_tabla_segundo_nivel;
				int entrada_tabla_segundo_nivel;
				recv(args->cliente_fd, &index_tabla_segundo_nivel, sizeof(int), 0);
				recv(args->cliente_fd, &entrada_tabla_segundo_nivel, sizeof(int), 0);
				log_info(logger, "Recibi ENVIAR_TABLA_SEGUNDO_NIVEL");
				log_info(logger, "Recibi index %d y entrada %d", index_tabla_segundo_nivel, entrada_tabla_segundo_nivel);

				pthread_mutex_lock(&mutex_lista_segundo_nivel);
				Tabla_Segundo_Nivel* t_segundo_nivel = list_get(lista_tablas_segundo_nivel, index_tabla_segundo_nivel);
				pthread_mutex_unlock(&mutex_lista_segundo_nivel);

				Entrada_Tabla_Segundo_Nivel* entrada_segundo_nivel = list_get(t_segundo_nivel->entradas_tabla_segundo_nivel, entrada_tabla_segundo_nivel);
				if(entrada_segundo_nivel->bit_presencia == 1) {
					log_info(logger, "Pagina encontrada. Devolviendo frame %d\n\n", entrada_segundo_nivel->marco);
					send_marco(args->cliente_fd, entrada_segundo_nivel->marco);
				} else {

					log_info(logger, "Page Fault. Buscando página en memoria\n\n");

					// Acá hay que agregar todo el proceso de buscar pagina en swap,
					// reemplazar paginas si es necesario, etc. 
					
					send_marco(args->cliente_fd, 5);
				}

				break;

			case READ_M:;

				log_info(logger, "Recibi READ_M\n\n");
				int leer_en;
				recv(args->cliente_fd, &leer_en, sizeof(int), 0);
				//log_info(logger, "tengo que leer en la posicion %d", leer_en);

				//buscar el valor a leer
				int valor_leido = 8; // de prueba

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
