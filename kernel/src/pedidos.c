#include "../include/pedidos.h"

void* atender_pedidos_consolas(void* void_args) {
    args_thread* args = (args_thread*) void_args;

	int accion;
	recv(args->cliente_fd, &accion, sizeof(accion), 0);

	switch(accion) {
		case ENVIAR_INSTRUCCIONES: ;
			int len_instrucciones;
			recv(args->cliente_fd, &len_instrucciones, sizeof(int), 0);

			int tamanio_proceso;
			recv(args->cliente_fd, &tamanio_proceso, sizeof(int), 0);

			void* stream = malloc(len_instrucciones*sizeof(instruccion));
			recv(args->cliente_fd, stream, len_instrucciones*sizeof(instruccion), 0);

			PCB pcb;
			crear_pcb(&pcb, tamanio_proceso, stream, len_instrucciones, args->cliente_fd);
			log_info(logger, "PCB creado: PDI es %d - Tamaño: %d - PC: %d - Tabla de páginas: %d - Estimación Inicial: %f", pcb.pid, pcb.tamanio_proceso, pcb.program_counter , pcb.tabla_paginas, pcb.estimacion_rafaga);
			
			// Agrego el proceso a New
			pthread_mutex_lock(&mutexNew);
			list_add(cola_new, &pcb);
			pthread_mutex_unlock(&mutexNew);

			// Signal a la funcion priorizar, para priorizar los suspendidos sobre los new a ready
			log_info(logger, "EVENTO: Proceso %d agregado a NEW", pcb.pid);
			print_colas();
			sem_post(&sem_priorizar);

			// Espera que el planificador de largo plazo le diga que puede finalizar
			sem_wait(&pcb.puedo_finalizar);

			sem_destroy(&pcb.puedo_finalizar);
			free(stream);
			close(args->cliente_fd);
			//log_info(logger, "Termine");
			break;
		default:
			log_warning_sh(logger, "Operacion desconocida. Recibi de una Consola: %d", accion);
			close(args->cliente_fd);
			break;
	}
	free(args);
}

void* atender_pedidos_dispatch() {
	while(conexion_dispatch != -1) {
		int accion;
		recv(conexion_dispatch, &accion, sizeof(accion), 0);

		switch(accion) {
			case EXIT_PROCESO: ;

				int pid_a_finalizar;
				int program_counter;

				recv_proceso_cpu(&pid_a_finalizar, &program_counter);

				pasar_de_exec_a_exit(pid_a_finalizar, program_counter);
				log_info(logger, "EVENTO: Proceso %d removido de EXEC y FINALIZA", pid_a_finalizar);
				print_colas();
				
				//avisar_a_memoria_proceso_finalizado(pid_a_finalizar);	//Avisar a memoria que elimine las estructuras(mandar operacion + pid)
				//Mover el proceso de running a exit con mutex
				//Mandar signal de mutex_popular_cola_ready ya que el grado de multiprogramacion decrementa

				break;

			case BLOCK_PROCESO: ;
				int pid;
				int pc;
				int tiempo_bloqueo;
				recv_proceso_bloqueado(&pid, &pc, &tiempo_bloqueo);

				log_info(logger, "Proceso %d bloqueado, pc %d, tiempo de bloqueo %d", pid, pc, tiempo_bloqueo);

				pasar_de_exec_a_bloqueado(pid, pc, tiempo_bloqueo);
				//mover el proceso de exec a blocked y mandar otro a ejecutar segun algoritmo de planificacion
				break;

			case DESALOJO_PROCESO: ;
				int pid_a_desalojar;
				int pc_desalojado;
				recv_proceso_cpu(&pid_a_desalojar, &pc_desalojado);
			
				proceso_exec->program_counter = pc_desalojado;

				log_info(logger, "Voy a desalojar al proceso %d", pid_a_desalojar);
				
				pthread_mutex_lock(&mutexExe);	
				time_t tiempo_actual = time(NULL);
				proceso_exec->ult_rafaga_real_CPU += difftime(tiempo_actual, proceso_exec->timestamp_exec) * 1000; // Sumo a la rafaga real de cpu

				log_info(logger, "El proceso %d ejecuto %lf", proceso_exec->pid, proceso_exec->ult_rafaga_real_CPU);
				pthread_mutex_lock(&mutexReady);
				list_add(cola_ready, proceso_exec);
				pthread_mutex_unlock(&mutexReady);
				pthread_mutex_unlock(&mutexExe);

				pthread_mutex_lock(&mutex_vg_ex);
				hay_un_proceso_ejecutando = false;
				pthread_mutex_unlock(&mutex_vg_ex);
			
				sem_post(&sem_planificar_SRT);
				sem_post(&sem_hay_procesos_en_ready);

				break;

			default:
				log_error(logger, "Operacion desconocida. Recibi de CPU: %d", accion);
				break;
		}
	}  
}