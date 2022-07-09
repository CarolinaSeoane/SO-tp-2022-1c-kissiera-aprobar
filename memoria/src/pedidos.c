#include "../include/pedidos.h"

void* atender_pedido(void* void_args) {

	args_thread_memoria* args = (args_thread_memoria*) void_args;
	
	while(args->cliente_fd != -1) {

		usleep(config.RETARDO_MEMORIA * 1000);

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

			case EXIT_PROCESO_M: ;
				int proceso;
				recv(args->cliente_fd, &proceso, sizeof(int), 0);
				log_info(logger, "Recibi EXIT_PROCESO_M del proceso %d",proceso);
				finalizar_estructuras_del_proceso_y_avisar_a_kernel(proceso, args->cliente_fd);
				log_info(logger, "La confirmación a Kernel fue enviada\n");
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

				pthread_mutex_lock(&mutex_lista_segundo_nivel);
				Tabla_Segundo_Nivel* t_segundo_nivel = list_get(lista_tablas_segundo_nivel, index_tabla_segundo_nivel);
				Entrada_Tabla_Segundo_Nivel* entrada_segundo_nivel = list_get(t_segundo_nivel->entradas_tabla_segundo_nivel, entrada_tabla_segundo_nivel);
				pthread_mutex_unlock(&mutex_lista_segundo_nivel);
				if(entrada_segundo_nivel->bit_presencia == 1) {
					log_info(logger, "Pagina encontrada. Devolviendo frame %d\n\n", entrada_segundo_nivel->marco);
					send_marco(args->cliente_fd, entrada_segundo_nivel->marco);
				} else {
					log_info(logger, "Page Fault. Buscando página en disco\n\n");

					//Aca en realidad buscas la cantidad de paginas cargadas en memoria (cantidad de marcos asignados a paginas con bit de presencia en 1)
					//Como es paginacion bajo demanda, los primeros N marcos que se carguen (los permitidos por el config) van a pasar por el chequeo.
					//Despues todo lo demas que se necesite va a ser bajar una pagina a swap y cargar otra
					

					int paginas_ocupadas = paginas_con_marco_cargado_presente(proceso_pid);
					int marco;

					log_info(logger, "El proceso %d tiene %d paginas cargadas en memoria", proceso_pid, paginas_ocupadas);

					if (paginas_ocupadas == config.MARCOS_POR_PROCESO){

						//Consigo la lista de paginas cargadas
						//Luego se lo paso al algoritmo que se necesite ejecutar para loopear sobre la misma
						generar_lista_de_paginas_cargadas(proceso_pid);

						//Ejecuto algoritmo de sustitucion para elegir la pagina victima a liberar
						if(!strcmp(config.ALGORITMO_REEMPLAZO, "CLOCK")) {
							
							log_info(logger, "Aplico algoritmo Clock");
							marco = aplicar_algoritmo_de_sustitucion_clock();
							
						} else {
							log_info(logger, "Aplico algoritmo Clock Modificado");
							marco = aplicar_algoritmo_de_sustitucion_clock_modificado();
						}

						list_clean(lista_paginas_cargadas);

						int numero_pagina = buscar_numero_de_pagina(marco, proceso_pid);
						if(fue_modificada) {
							solicitar_swap_out_a_swap(proceso_pid, numero_pagina, marco);
							fue_modificada = false;
						} else {
							actualizar_tabla_de_paginas(index_tabla_segundo_nivel, numero_pagina, -1); //con marco = -1 se muestra - - -
						}
						
						solicitar_pagina_a_swap(proceso_pid, numero_pagina);
						log_info(logger, "Actualizando tabla de paginas luego del SWAP IN...");						
						actualizar_tabla_de_paginas(index_tabla_segundo_nivel, entrada_tabla_segundo_nivel, marco);						
						

					} else {
						
						marco = solicitar_pagina_a_swap(proceso_pid, entrada_tabla_segundo_nivel);
						
						actualizar_tabla_de_paginas(index_tabla_segundo_nivel, entrada_tabla_segundo_nivel, marco);
						
					}
					
					send_marco(args->cliente_fd, marco);
					log_info(logger, "Se envio el marco %d a CPU", marco);
				}

				break;

			case READ_M:;

				log_info(logger, "Recibi READ_M\n\n");
				int direccion_fisica;
				recv(args->cliente_fd, &direccion_fisica, sizeof(int), 0);

				uint32_t valor_leido; 
				pthread_mutex_lock(&mutex_memoria);
				memcpy(&valor_leido, memoria_principal+direccion_fisica, sizeof(uint32_t)); 
				pthread_mutex_unlock(&mutex_memoria);

				void* paquete = malloc(sizeof(uint32_t));
				memcpy(paquete, &valor_leido, sizeof(uint32_t));
				send(args->cliente_fd, paquete, sizeof(uint32_t), 0);

				free(paquete);
				log_info(logger, "Se leyo en la posicion %d de memoria el valor %d", direccion_fisica, valor_leido);				

				break;

			case WRITE_M:
				
				log_info(logger, "Recibi WRITE_M");

				int pid_write;
				recv(args->cliente_fd, &pid_write, sizeof(int), 0);

				int dir_fisica;
				recv(args->cliente_fd, &dir_fisica, sizeof(int), 0);

				uint32_t valor_a_escribir;
				recv(args->cliente_fd, &valor_a_escribir, sizeof(uint32_t), 0);

				pthread_mutex_lock(&mutex_memoria);
				memcpy(memoria_principal+dir_fisica, &valor_a_escribir, sizeof(uint32_t)); 
				pthread_mutex_unlock(&mutex_memoria);
				
				log_info(logger, "Se escribio en la posicion %d de memoria el valor %d", dir_fisica, valor_a_escribir);

				uint32_t chequear_valor;

				memcpy(&chequear_valor, memoria_principal+dir_fisica, sizeof(uint32_t));

				uint32_t operacion_exitosa;
				log_info(logger, "Chequeando valor escrito en memoria");				
				if(chequear_valor == valor_a_escribir) {					
					operacion_exitosa = 1;
				} else {					
					operacion_exitosa = 0;					
				} //leo en la posicion que acabo de escribir y el valor de ahi deberia ser el mismo que escribi

				actualizar_bit_modificado(pid_write, floor((double) dir_fisica/config.TAM_PAGINA));

				void* a_enviar = malloc(sizeof(int));
				memcpy(a_enviar, &operacion_exitosa, sizeof(uint32_t));
				send(args->cliente_fd, a_enviar, sizeof(uint32_t), 0);
				free(a_enviar);

				break;

			case HANDSHAKE_MEMORIA:

				log_info(logger, "Recibi HANDSHAKE_MEMORIA\n\n");
				send_cpu_handshake((void*) args);
				break;

			case SWAP_IN: ;

				int pid_proceso_swap_in;
				recv(args->cliente_fd, &pid_proceso_swap_in, sizeof(int), 0);
				log_info(logger, "Recibi SWAP_IN PARA PID %d", pid_proceso_swap_in);

				// solamente asignarle MARCOS_POR_PROCESO. Swap no se involucra

				// avisar al proceso que finalizo la operacion de swap. Ver como hacer para que los planificadores no se pisen al
				// hacer recv de mensajes de memoria

				break;

			case SWAP_OUT: ;

				int pid_proceso_swap_out;
				recv(args->cliente_fd, &pid_proceso_swap_out, sizeof(int), 0);
				log_info(logger, "Recibi SWAP_OUT PARA PID %d", pid_proceso_swap_out);

				// Buscar paginas modificadas
				// Si las hay, mandar a swap para escribirlas
				escribir_paginas_modificadas(pid);

				actualizar_bit_presencia(pid_proceso_swap_out);

				int swap_out_exitoso = 1;
				void* paquete_swap_out = malloc(sizeof(int));
				memcpy(paquete_swap_out, &swap_out_exitoso, sizeof(int));
				send(args->cliente_fd, paquete_swap_out, sizeof(int), 0);
				free(paquete_swap_out);
				
				break;

			default:
				log_warning_sh(logger, "Operacion desconocida.\n\n");
				close(args->cliente_fd);
				break;
		}
	}
	free(args);
}
