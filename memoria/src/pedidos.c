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

				int proceso_pid;
				int index_tabla_segundo_nivel;
				int entrada_tabla_segundo_nivel;
				recv(args->cliente_fd, &proceso_pid, sizeof(int), 0);
				recv(args->cliente_fd, &index_tabla_segundo_nivel, sizeof(int), 0);
				recv(args->cliente_fd, &entrada_tabla_segundo_nivel, sizeof(int), 0);
				log_info(logger, "Recibi ENVIAR_TABLA_SEGUNDO_NIVEL");
				log_info(logger, "Recibi pid %d, index %d y entrada %d", proceso_pid,  index_tabla_segundo_nivel, entrada_tabla_segundo_nivel);

				//No estoy seguro si necesitamos mutext para acceder a la lista de las tablas de 2do nivel del proceso 
				//Supuestamente nunca tendrias accesos simultaneos a una lista de tablas de 2do nivel
				pthread_mutex_lock(&mutex_lista_segundo_nivel);
				Tabla_Segundo_Nivel* t_segundo_nivel = list_get(lista_tablas_segundo_nivel, index_tabla_segundo_nivel);
				Entrada_Tabla_Segundo_Nivel* entrada_segundo_nivel = list_get(t_segundo_nivel->entradas_tabla_segundo_nivel, entrada_tabla_segundo_nivel);
				pthread_mutex_unlock(&mutex_lista_segundo_nivel);
				if(entrada_segundo_nivel->bit_presencia == 1) {
					log_info(logger, "Pagina encontrada. Devolviendo frame %d\n\n", entrada_segundo_nivel->marco);
					send_marco(args->cliente_fd, entrada_segundo_nivel->marco);
				} else {
					log_info(logger, "Page Fault. Buscando página en memoria\n\n");

					//Aca en realidad buscas la cantidad de paginas cargadas en memoria (cantidad de marcos asignados a paginas con bit de presencia en 1)
					//Como es paginacion bajo demanda, los primeros N marcos que se carguen (los permitidos por el config) van a pasar por el chequeo.
					//Despues todo lo demas que se necesite va a ser bajar una pagina a swap y cargar otra
					
					int paginas_ocupadas = paginas_con_marco_cargado_presente(proceso_pid);
					int marco;


					log_info(logger, "Este proceso tiene %d paginas ocupadas", paginas_ocupadas);
					if (paginas_ocupadas == config.MARCOS_POR_PROCESO){

						if(!strcmp(config.ALGORITMO_REEMPLAZO, "CLOCK")) {
							//CLOCK
							//Ejecuto algoritmo de sustitucion para elegir la pagina victima a liberar
						} else {
							//CLOCK-M
						}
						
						//Libero y obtengo la pagina libre 
						//log_info(logger, "Pagina #%d liberada por algoritmo de sustitucion %s\n\n", pagina_libre, config.ALGORITMO_REEMPLAZO);
					} else {

						solicitar_pagina_a_swap(proceso_pid, entrada_tabla_segundo_nivel);	
						sem_wait(&swap_respondio);

						pthread_mutex_lock(&mutex_pagina_en_intercambio);
						marco = cargar_pagina_en_memoria(proceso_pid, pagina_en_intercambio);
						pthread_mutex_unlock(&mutex_pagina_en_intercambio);
						
						actualizar_tabla_de_paginas(index_tabla_segundo_nivel, entrada_tabla_segundo_nivel, marco);
						verificar_memoria();
						
					}
					
					send_marco(args->cliente_fd, marco);
					log_info(logger, "Se envio el marco %d a CPU", marco);
				}

				break;

			case READ_M:;

				log_info(logger, "Recibi READ_M\n\n");
				int direccion_fisica;
				recv(args->cliente_fd, &direccion_fisica, sizeof(int), 0);
				//log_info(logger, "tengo que leer en la posicion %d", direccion_fisica);

				//buscar el valor a leer
				int valor_leido = 8; // de prueba

				void* paquete = malloc(sizeof(int));

				memcpy(paquete, &valor_leido, sizeof(int));

				send(args->cliente_fd, paquete, sizeof(int), 0);

				free(paquete);

				break;

			case WRITE_M:
				
				log_info(logger, "Recibi WRITE_M");

				int dir_fisica;
				recv(args->cliente_fd, &dir_fisica, sizeof(int), 0);

				int valor_a_escribir;
				recv(args->cliente_fd, &valor_a_escribir, sizeof(int), 0);

				//escribir el valor en la direccion
				
				int operacion_exitosa; //si no fue exitoso, del lado de cpu se cierra el programa. no deberia ocurrir.

				void* a_enviar = malloc(sizeof(int));

				memcpy(a_enviar, &operacion_exitosa, sizeof(int));

				send(args->cliente_fd, a_enviar, sizeof(int), 0);

				free(a_enviar);

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
