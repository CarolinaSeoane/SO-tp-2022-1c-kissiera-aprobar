#include "../include/planificacion.h"

/* ********** PLANIFICADOR LARGO PLAZO ********** */

void* priorizar_procesos_suspendidos_ready_sobre_new() {
   while(1){
	sem_wait(&sem_priorizar);
	sem_wait(&sem_grado_multiprogramacion);
	  	if(list_size(cola_suspended_ready)) {
        	pasar_de_ready_susp_a_ready();
        } else { 
          	pasar_de_new_a_ready();
   		}
    }
}

void pasar_de_new_a_ready() {
	pthread_mutex_lock(&mutexNew);
    if(list_size(cola_new)) {

		PCB* elem_iterado = list_remove(cola_new, 0);
		pthread_mutex_unlock(&mutexNew);

		//log_info(logger, "Pidiendo tabla de paginas a memoria");
		elem_iterado->tabla_paginas = solicitar_tabla_de_paginas_a_memoria(elem_iterado);
		log_info(logger, "Recibi tabla de paginas de index: %d", elem_iterado->tabla_paginas);

		pthread_mutex_lock(&mutexReady);
		list_add(cola_ready, elem_iterado);
		pthread_mutex_unlock(&mutexReady);
	
		log_info(logger, "EVENTO: Proceso %d removido de NEW y agregado a READY", elem_iterado->pid);
		print_colas();
		sem_post(&sem_hay_procesos_en_ready);

		if(!strcmp(config.ALGORITMO_PLANIFICACION, "SRT")) {
			pasar_de_exec_a_ready();
		}

    } else {
		pthread_mutex_unlock(&mutexNew);
	}
}

void pasar_de_exec_a_exit(int pid, int pc) { 

	int consola;
	int pid_exec;

	pthread_mutex_lock(&mutex_vg_ex);
	bool cpu_ocupada = hay_un_proceso_ejecutando;
	pthread_mutex_unlock(&mutex_vg_ex);

	if(cpu_ocupada) {
		
		pthread_mutex_lock(&mutexExe);
		proceso_exec->program_counter = pc;
		consola = proceso_exec->cliente_fd;
		pid_exec = proceso_exec->pid;
		pthread_mutex_unlock(&mutexExe);

		if(pid_exec == pid) {
			log_info(logger, "Envio pedido a memoria para que libere estructuras");
			pedir_finalizar_estructuras_y_esperar_confirmacion(pid);
			send_proceso_finalizado_a_consola(pid, consola);

			pthread_mutex_lock(&mutex_vg_ex);
			hay_un_proceso_ejecutando = false;
			pthread_mutex_unlock(&mutex_vg_ex);
			
			if(!strcmp(config.ALGORITMO_PLANIFICACION, "FIFO")) {
				sem_post(&sem_planificar_FIFO);
			} else {
				sem_post(&sem_planificar_SRT);
			}	
			sem_post(&(proceso_exec->puedo_finalizar));	
			sem_post(&sem_grado_multiprogramacion);
		} else {
			log_error(logger, "Error de planificacion");
		}
	}

}

/* ********** PLANIFICADOR MEDIANO PLAZO ********** */

void pasar_de_blocked_susp_a_ready_susp() {
	
	pthread_mutex_lock(&mutexSuspendedBlocked);
	PCB* pcb = list_remove(cola_suspended_blck, 0);
	pthread_mutex_unlock(&mutexSuspendedBlocked);

	pthread_mutex_lock(&mutexSuspendedReady);
	list_add(cola_suspended_ready, pcb);
	pthread_mutex_unlock(&mutexSuspendedReady);

	log_info(logger, "EVENTO: Proceso %d removido de SUSP/BLOCKED y agregado a SUSP/READY", pcb->pid);
	print_colas();

	//sem_post(&sem_hilo_ready_susp_ready);
	pasar_de_ready_susp_a_ready();
  
}

void pasar_de_ready_susp_a_ready() {
	
	sem_wait(&sem_grado_multiprogramacion);
	
	pthread_mutex_lock(&mutexSuspendedReady);
	if(list_size(cola_suspended_ready)) {
			
		PCB* pcb = list_remove(cola_suspended_ready, 0);

		solicitar_swap_in_a_memoria(pcb);

		pthread_mutex_lock(&mutexReady);
		list_add(cola_ready, pcb);
		pthread_mutex_unlock(&mutexReady);

		log_info(logger, "EVENTO: Proceso %d removido de SUSP/READY y agregado a READY", pcb->pid);
		print_colas();
		sem_post(&sem_hay_procesos_en_ready);

		if(!strcmp(config.ALGORITMO_PLANIFICACION, "SRT")) {
			pasar_de_exec_a_ready();
		}
	}
	pthread_mutex_unlock(&mutexSuspendedReady);
	log_info(logger, "No hay procesos para suspender");
	
}

/* ********** PLANIFICADOR CORTO PLAZO ********** */

void* pasar_de_ready_a_exec_FIFO() {
	while(1) {
		sem_wait(&sem_planificar_FIFO);
		sem_wait(&sem_hay_procesos_en_ready);

		pthread_mutex_lock(&mutex_vg_ex);
		bool cpu_ocupada = hay_un_proceso_ejecutando;
		pthread_mutex_unlock(&mutex_vg_ex);

		pthread_mutex_lock(&mutexReady);
		if(list_size(cola_ready) && !cpu_ocupada) {	
			PCB* pcb = list_remove(cola_ready, 0);
			pthread_mutex_unlock(&mutexReady);

			pthread_mutex_lock(&mutexExe);
			proceso_exec = pcb;
			pthread_mutex_unlock(&mutexExe);

			pthread_mutex_lock(&mutex_vg_ex);
			hay_un_proceso_ejecutando = true;
			pthread_mutex_unlock(&mutex_vg_ex);

			send_proceso_a_cpu(pcb, (pcb->len_instrucciones)*sizeof(instruccion));
			log_info(logger, "EVENTO: Proceso %d removido de READY y agregado a EXEC", proceso_exec->pid);
			print_colas();

		} else {
			pthread_mutex_unlock(&mutexReady);
		}
	}
}

void pasar_de_exec_a_bloqueado(int pid, int pc, int tiempo_bloqueo) {

	pthread_mutex_lock(&mutex_vg_ex);
	bool cpu_ocupada = hay_un_proceso_ejecutando;
	pthread_mutex_unlock(&mutex_vg_ex);

	pthread_mutex_lock(&mutexExe);
	if(cpu_ocupada && proceso_exec->pid == pid) {

		proceso_exec->program_counter = pc;
		proceso_exec->tiempo_bloqueo = tiempo_bloqueo;
		
		//time_t tiempo_actual = time(NULL);
		
		clock_gettime(CLOCK_REALTIME,(&proceso_exec->timestamp_blocked)); // = tiempo_actual; //guardo cuando ingreso a la cola blocked. time devuelve segundos, lo paso a milisegundos

		// Sumo a la rafaga real de cpu y calculo la nueva estimacion
		if(!strcmp(config.ALGORITMO_PLANIFICACION, "SRT")) {
			proceso_exec->ult_rafaga_real_CPU += (proceso_exec->timestamp_blocked.tv_sec - proceso_exec->timestamp_exec.tv_sec) * 1000 + (proceso_exec->timestamp_blocked.tv_nsec - proceso_exec->timestamp_exec.tv_nsec) / 1000000; // Sumo a la rafaga real de cpu		
			log_info(logger, "El proceso %d ejecuto %lld", proceso_exec->pid, proceso_exec->ult_rafaga_real_CPU);

			double alpha = config.ALFA;
			proceso_exec->estimacion_rafaga = (alpha * proceso_exec->ult_rafaga_real_CPU) + ((1 - alpha) * proceso_exec->estimacion_rafaga);
			log_info(logger, "La nueva estimacion del proceso %d es %f", proceso_exec->pid, proceso_exec->estimacion_rafaga);
			
		}

		pthread_mutex_lock(&mutexBlock);
		list_add(cola_blck, proceso_exec);
		pthread_mutex_unlock(&mutexBlock);

		pthread_mutex_unlock(&mutexExe);

		pthread_mutex_lock(&mutex_vg_ex);
		hay_un_proceso_ejecutando = false;
		pthread_mutex_unlock(&mutex_vg_ex);

		if(!strcmp(config.ALGORITMO_PLANIFICACION, "FIFO")) {
			sem_post(&sem_planificar_FIFO);
		} else {
			sem_post(&sem_planificar_SRT);
		}

		////
		//if(!strcmp(config.ALGORITMO_PLANIFICACION, "FIFO")) {
			
			sem_post(&sem_ejecutar_IO);
			log_info(logger, "EVENTO: Proceso %d removido de EXEC y agregado a BLOCKED", proceso_exec->pid);
			print_colas();
		//} else {
			// semaforos SRT
		//}

		int tiempo_en_bloqueo = calcular_tiempo_que_estara_bloqueado();
		if(tiempo_en_bloqueo >= config.TIEMPO_MAXIMO_BLOQUEADO) {
			//se que se va a suspender
			pthread_mutex_lock(&mutexBlock);
			int last_index = list_size(cola_blck);

			pthread_mutex_lock(&mutexProcesosQueSeVanASuspender);
			list_add(procesos_que_se_van_a_suspender, list_get(cola_blck, last_index - 1));
			pthread_mutex_unlock(&mutexProcesosQueSeVanASuspender);
			pthread_mutex_unlock(&mutexBlock);

			pthread_t hilo_timer;

			args_timer *args = malloc(sizeof(args_timer));
			args->tiempo = proceso_exec->timestamp_blocked.tv_sec * 1000 + (proceso_exec->timestamp_blocked.tv_nsec / 1000000);

			pthread_create(&hilo_timer, NULL, timer, (void*) args);
			pthread_detach(hilo_timer);
		}


	} else {
		pthread_mutex_unlock(&mutexExe);
		log_error(logger, "Error de planificacion\n");
	}
}

void* pasar_de_ready_a_exec_SRT() {  
	while(1) {
		sem_wait(&sem_planificar_SRT);
		sem_wait(&sem_hay_procesos_en_ready);

		pthread_mutex_lock(&mutex_vg_ex);
		bool cpu_ocupada = hay_un_proceso_ejecutando;
		pthread_mutex_unlock(&mutex_vg_ex);

		pthread_mutex_lock(&mutexReady);
		if(list_size(cola_ready) && !cpu_ocupada) {	
			PCB* pcb = list_get_max_priority(cola_ready);

			bool tienen_mismo_pid(void* elemento) {
				return pcb->pid == ((PCB*) elemento)->pid;
			}

			list_remove_by_condition(cola_ready, tienen_mismo_pid);

			pthread_mutex_unlock(&mutexReady);

			pthread_mutex_lock(&mutexExe);
			clock_gettime(CLOCK_REALTIME,(&pcb->timestamp_exec));
			proceso_exec = pcb;
			pthread_mutex_unlock(&mutexExe);

			//log_info(logger, "Timestamp de cuando proceso %d pasa a exec: %f", pcb->pid, pcb->timestamp_exec);

			pthread_mutex_lock(&mutex_vg_ex);
			hay_un_proceso_ejecutando = true;
			pthread_mutex_unlock(&mutex_vg_ex);

			send_proceso_a_cpu(pcb, (pcb->len_instrucciones)*sizeof(instruccion));
			log_info(logger, "EVENTO: Proceso %d removido de READY y agregado a EXEC", proceso_exec->pid);
			print_colas();

		} else {
			pthread_mutex_unlock(&mutexReady);
		}
	}
}

void* menor_tiempo_restante(PCB* p1, PCB* p2) {
    float tiempo_restante_p1 = p1->estimacion_rafaga - p1->ult_rafaga_real_CPU;
	float tiempo_restante_p2 = p2->estimacion_rafaga - p2->ult_rafaga_real_CPU;
	if(tiempo_restante_p1 < 0) {
		tiempo_restante_p1 = 0;
	} 
	if(tiempo_restante_p2 < 0) {
		tiempo_restante_p2 = 0;
	}
	return tiempo_restante_p1 <= tiempo_restante_p2 ? p1 : p2;
}

void *list_get_max_priority(t_list *lista) {	
	PCB* pcb = list_get_minimum(lista, (void*) menor_tiempo_restante);
	log_info(logger, "Tiene mayor prioridad el pid %d, tiene %lf", pcb->pid, (pcb->estimacion_rafaga - pcb->ult_rafaga_real_CPU));
	return pcb;
}

void pasar_de_exec_a_ready() {

	pthread_mutex_lock(&mutex_vg_ex);
	bool cpu_ocupada = hay_un_proceso_ejecutando;
	pthread_mutex_unlock(&mutex_vg_ex);

	if(cpu_ocupada) {
		// Pido a cpu que devuelva el proceso
		int* co_op = malloc(sizeof(int));
		*co_op = INTERRUPCION;
		send(conexion_interrupt, co_op, sizeof(int), 0);
		free(co_op);
		// El recv de esto se hace pos dispatch en pedidos.c
	}
}

void* timer(void* void_args) {
    uint64_t delta_tiempo = 0;
	struct timespec tiempo_medido;
	args_timer* args = (args_timer*) void_args;

    while(delta_tiempo < config.TIEMPO_MAXIMO_BLOQUEADO) {
        clock_gettime(CLOCK_REALTIME,(&tiempo_medido));
        delta_tiempo = (tiempo_medido.tv_sec * 1000 + (tiempo_medido.tv_nsec / 1000000)) - args->tiempo;
    }
	//sale del while cuando se cumple el tiempo maximo de bloqueo
	sem_post(&sem_hilo_blocked_a_blocked_susp);
	free(args);
}

void* pasar_de_bloqueado_a_bloqueado_susp() {
	while(1) {
		sem_wait(&sem_hilo_blocked_a_blocked_susp);
		log_info(logger, "ME DESBLOQUEE");

		pthread_mutex_lock(&mutexProcesosQueSeVanASuspender);
		PCB* pcb = list_remove(procesos_que_se_van_a_suspender, 0);
		pthread_mutex_unlock(&mutexProcesosQueSeVanASuspender);

		bool tienen_mismo_pid(void* elemento) {
			return pcb->pid == ((PCB*) elemento)->pid;
		}

		pthread_mutex_lock(&mutexBlock);
		list_remove_by_condition(cola_blck, tienen_mismo_pid);
		pthread_mutex_unlock(&mutexBlock);

		pthread_mutex_lock(&mutexSuspendedBlocked);
		list_add(cola_suspended_blck, pcb);
		pthread_mutex_unlock(&mutexSuspendedBlocked);

		log_info(logger, "EVENTO: Proceso %d removido de BLOCKED y agregado a BLOCKED/SUSP", pcb->pid);
		print_colas();

		solicitar_swap_out_a_memoria(pcb);

		sem_post(&sem_grado_multiprogramacion);	

	}
}

int calcular_tiempo_que_estara_bloqueado() {
	pthread_mutex_lock(&mutexBlock);
	
	t_list_iterator* iterator = list_iterator_create(cola_blck);
	PCB* elem_iterado;
	int tiempo_total = 0;

	while(list_iterator_has_next(iterator)) {
		elem_iterado = list_iterator_next(iterator);
		tiempo_total += elem_iterado->tiempo_bloqueo;
		log_info(logger, "Lei tiempo de bloqueo %d del PID %d", elem_iterado->tiempo_bloqueo, elem_iterado->pid);
	}
	
	log_info(logger, "Tiempo que estará bloqueado: %d", tiempo_total);
	list_iterator_destroy(iterator);

	pthread_mutex_unlock(&mutexBlock);

	return tiempo_total;	
}

/* ********** DISPOSITIVO IO ********** */

void* ejecutar_IO() {
	while(1) {
		sem_wait(&sem_ejecutar_IO);
		sem_wait(&IO_esta_disponible);

		pthread_mutex_lock(&mutex_vg_io);
		bool IO_esta_ocupado = IO_ocupado;
		pthread_mutex_unlock(&mutex_vg_io);

		pthread_mutex_lock(&mutexSuspendedBlocked);
		if(!IO_esta_ocupado && list_size(cola_suspended_blck) > 0) {
			pthread_mutex_lock(&mutex_vg_io);
			IO_ocupado = true;
			pthread_mutex_unlock(&mutex_vg_io);

			PCB* pcb = list_get(cola_suspended_blck, 0);
			pthread_mutex_unlock(&mutexSuspendedBlocked);

			while(pcb->tiempo_bloqueo > 0) {
				sleep(1);
				pcb->tiempo_bloqueo -= 1000;
				//log_info(logger, "DISMINUYO TIEMPO BLOQUEO A: %d", pcb->tiempo_bloqueo);			
			}
		
			pcb->ult_rafaga_real_CPU = 0;
		
			pthread_mutex_lock(&mutex_vg_io);
			IO_ocupado = false;
			pthread_mutex_unlock(&mutex_vg_io);
			
			sem_post(&IO_esta_disponible);

			esperar_que_termine_swap_out(pcb);
			//pasar_de_blocked_susp_a_ready_susp();
		} else {
			pthread_mutex_lock(&mutex_vg_io);
			IO_ocupado = true;
			pthread_mutex_unlock(&mutex_vg_io);

			pthread_mutex_unlock(&mutexSuspendedBlocked);

			pthread_mutex_lock(&mutexBlock);
			PCB* pcb = list_get(cola_blck, 0);
			pthread_mutex_unlock(&mutexBlock);

			while(pcb->tiempo_bloqueo > 0) {
				sleep(1);
				pcb->tiempo_bloqueo -= 1000;	
				//log_info(logger, "2) DISMINUYO TIEMPO BLOQUEO A: %d", pcb->tiempo_bloqueo);			
			}

			pcb->ult_rafaga_real_CPU = 0;

			pthread_mutex_lock(&mutex_vg_io);
			IO_ocupado = false;
			pthread_mutex_unlock(&mutex_vg_io);
			
			sem_post(&IO_esta_disponible);

			// Se pudo haber suspendido mientras hacia IO

			bool tienen_mismo_pid(void* elemento) {
				return pcb->pid == ((PCB*) elemento)->pid;
			}

			pthread_mutex_lock(&mutexSuspendedBlocked);
			bool se_suspendio = list_any_satisfy(cola_suspended_blck, tienen_mismo_pid);
			pthread_mutex_unlock(&mutexSuspendedBlocked);

			if(se_suspendio) {
				esperar_que_termine_swap_out(pcb);
				// pasar_de_blocked_susp_a_ready_susp();
			} else {
				pthread_mutex_lock(&mutexBlock);
				list_remove(cola_blck, 0);
				pthread_mutex_unlock(&mutexBlock);
				//log_info(logger, "Removiendo proceso %d de Blocked", pcb->pid);

				pthread_mutex_lock(&mutexReady);
				list_add(cola_ready, pcb);
				pthread_mutex_unlock(&mutexReady);
				//log_info(logger, "Proceso %d agregado a Ready", pcb->pid);
				log_info(logger, "EVENTO: Proceso %d removido de BLOCKED y agregado a READY", pcb->pid);
				print_colas();

				sem_post(&sem_hay_procesos_en_ready);
				if(!strcmp(config.ALGORITMO_PLANIFICACION, "SRT")) {
					pasar_de_exec_a_ready();
				}	
			}
		}
	}
}

void esperar_que_termine_swap_out(PCB* pcb) {
	sem_wait(&pcb->termino_operacion_swap_out);
	pasar_de_blocked_susp_a_ready_susp();
}