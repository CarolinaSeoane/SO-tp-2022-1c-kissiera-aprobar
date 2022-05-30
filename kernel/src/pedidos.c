#include "../include/pedidos.h"


void* atender_pedidos_consolas(void* void_args) {
    args_thread* args = (args_thread*) void_args;

	int accion;
	recv(args->cliente_fd, &accion, sizeof(accion), 0);
	log_info(logger, "Acción: %d", accion);

	switch(accion) {
		case ENVIAR_INSTRUCCIONES: ;
			int len_instrucciones;
			recv(args->cliente_fd, &len_instrucciones, sizeof(int), 0);

			int tamanio_proceso;
			recv(args->cliente_fd, &tamanio_proceso, sizeof(int), 0);

			void* stream = malloc(len_instrucciones*sizeof(instruccion));
			recv(args->cliente_fd, stream, len_instrucciones*sizeof(instruccion), 0);

			PCB pcb;
			crear_pcb(&pcb, tamanio_proceso, stream, len_instrucciones, config.ESTIMACION_INICIAL, args->cliente_fd);
			log_info(logger, "PCB creado: PDI es %d - Tamaño: %d - PC: %d - Tabla de páginas: %d - Estimación Inicial: %d", pcb.pid, pcb.tamanio_proceso, pcb.program_counter , pcb.tabla_paginas, pcb.estimacion_rafaga);
			
			// Agrego el proceso a New
			pthread_mutex_lock(&mutexNew);
			list_add(cola_new, &pcb);
			pthread_mutex_unlock(&mutexNew);

			// Signal al hilo new - ready
			log_info(logger, "Nuevo proceso agregado a NEW, cantidad de procesos en NEW: %d", cola_new->elements_count);
			sem_post(&sem_hilo_new_ready);

			sem_wait(&finalizar);

			//free(stream);
			break;
		default:
			log_warning_sh(logger, "Operacion desconocida.");
			close(args->cliente_fd);
			break;
	}
	free(args);
}

void* atender_pedidos_dispatch() {
	while(conexion_dispatch != -1) {
		int accion;
		recv(conexion_dispatch, &accion, sizeof(accion), 0);
		log_info(logger, "Acción: %d", accion);

		switch(accion) {
			case EXIT_PROCESO: ;
				int pid_a_finalizar;
				recv(conexion_dispatch, &pid_a_finalizar, sizeof(int), 0);
				int program_counter;
				recv(conexion_dispatch, &program_counter, sizeof(int), 0);
				pasar_de_exec_a_exit(pid_a_finalizar, program_counter);
				//avisar_a_memoria_proceso_finalizado(pid_a_finalizar);	//Avisar a memoria que elimine las estructuras(mandar operacion + pid)
				//Mover el proceso de running a exit con mutex
				//Mandar signal de mutex_popular_cola_ready ya que el grado de multiprogramacion decrementa

				break;
			case BLOCK_PROCESO: ;
				break;
			default:
				log_error(logger, "Operacion desconocida.");
				break;
		}
	}  
}