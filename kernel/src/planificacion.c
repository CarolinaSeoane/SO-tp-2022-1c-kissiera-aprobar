#include "../include/planificacion.h"

/* ********** PLANIFICADOR LARGO PLAZO ********** */

void* intentar_pasar_de_new_a_ready() {
	while(1) {
		sem_wait(&sem_hilo_new_ready);
		sem_wait(&sem_grado_multiprogramacion);	//FALTA ARREGLAR LA PRIORIDAD DE READY SUSP. NO SE ME OCURRE COMO HACERLO
		pasar_de_new_a_ready();
    }
}

void pasar_de_new_a_ready() {
	pthread_mutex_lock(&mutexNew);
    if(list_size(cola_new)) {

		PCB* elem_iterado = list_remove(cola_new, 0);
		pthread_mutex_unlock(&mutexNew);

		log_info(logger, "Pidiendo tabla de paginas a memoria");
		elem_iterado->tabla_paginas = solicitar_tabla_de_paginas_a_memoria(elem_iterado);
		log_info(logger, "Recibi tabla de paginas: %d", elem_iterado->tabla_paginas);

		pthread_mutex_lock(&mutexReady);
		list_add(cola_ready, elem_iterado);
		pthread_mutex_unlock(&mutexReady);

		log_info(logger, "Proceso %d removido de New, cantidad en New: %d", elem_iterado->pid, cola_new->elements_count);
		log_info(logger, "Cantidad en Ready: %d", cola_ready->elements_count);
		sem_post(&sem_hay_procesos_en_ready);
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
		log_info(logger, "HAY UN PROCESO EJECUTANDO");
		pthread_mutex_lock(&mutexExe);
		proceso_exec->program_counter = pc;
		consola = proceso_exec->cliente_fd;
		pid_exec = proceso_exec->pid;
		pthread_mutex_unlock(&mutexExe);

		if(pid_exec == pid) {
			send_proceso_finalizado_a_consola(pid, consola);

			pthread_mutex_lock(&mutex_vg_ex);
			hay_un_proceso_ejecutando = false;
			pthread_mutex_unlock(&mutex_vg_ex);
			
			if(!strcmp(config.ALGORITMO_PLANIFICACION, "FIFO")) {
				sem_post(&sem_planificar_FIFO);
			} else {
				// semaforos SRT
			}	
			sem_post(&(proceso_exec->puedo_finalizar));	
			sem_post(&sem_grado_multiprogramacion);
		} else {
			log_error(logger, "Error grave de planificacion");
		}
	}
}

/* ********** PLANIFICADOR MEDIANO PLAZO ********** */
<<<<<<< HEAD
/*
void* pasar_de_bloqueado_a_bloqueado_susp() { //ver
=======

void* pasar_de_bloqueado_a_bloqueado_susp() { 
>>>>>>> 563773e771a05ffedc6d24e3562d3e17af8fffc4
	while(1) {

		sem_wait(&sem_hilo_bloqueado_a_bloqueado_susp);
	
	PCB* proceso_block;
	int TIEMPO_MAXIMO_BLOQUEADO = config.TIEMPO_MAXIMO_BLOQUEADO;
	int TIEMPO_BLOQUEADO = proceso_block->tiempo_bloqueo ; //nose si funciona tan simple, o hay que hacer un hilo que verifique el tiempo bloqueado
	//int pid_bloqueado;

    pthread_mutex_lock(&mutexBlock);

	if( TIEMPO_BLOQUEADO >= TIEMPO_MAXIMO_BLOQUEADO ){
       
		pthread_mutex_lock(&mutexBlock);
		proceso_block = list_remove(cola_blck, 0);
		pthread_mutex_unlock(&mutexBlock);

		log_info(logger, "Pidiendo a memoria pasar el proceso a swap ");
		solicitar_swap_out_a_memoria(proceso_block);	
          
		pthread_mutex_lock(&mutexSuspendedBlocked);
		list_add(cola_suspended_blck, proceso_block);
		pthread_mutex_unlock(&mutexSuspendedBlocked);

		log_info(logger, "Proceso %d removido de Block, cantidad en Block: %d", proceso_block->pid, cola_new->elements_count);
		log_info(logger, "Cantidad en Susoendido: %d", cola_suspended_blck->elements_count);
		sem_post(&sem_hilo_bloqueado_susp_a_susp_ready); 	
		sem_post(&sem_grado_multiprogramacion); //recordar hacer este
     }
	 else{
	 pthread_mutex_unlock(&mutexBlock);
	 }
   }
}*/

void* pasar_de_bloqueado_susp_a_susp_ready(){
while(1) {
sem_wait(&sem_hilo_bloqueado_susp_a_susp_ready);

PCB* proceso_susp;

pthread_mutex_lock(&mutexSuspendedBlocked);
proceso_block = list_remove(cola_suspended_blck, 0);
pthread_mutex_lock(&mutexSuspendedBlocked);

pthread_mutex_lock(&mutexSuspendedBlocked);
list_add(cola_suspended_ready, proceso_block);
pthread_mutex_unlock(&mutexSuspendedBlocked);

sem_post(&sem_hilo_ready_susp_ready);

    }
}

void* pasar_de_ready_susp_a_ready() {
	while(1) {
		sem_wait(&sem_hilo_ready_susp_ready); // hay que hacerle signal cuando pasa de block/susp a ready/susp
		sem_wait(&sem_grado_multiprogramacion);

		pthread_mutex_lock(&mutexSuspendedReady);
		if(list_size(cola_suspended_ready)) {
			
			PCB* elem_iterado = list_remove(cola_suspended_ready, 0);

			pthread_mutex_lock(&mutexReady);
			list_add(cola_ready, elem_iterado);
			pthread_mutex_unlock(&mutexReady);

			log_info(logger, "Proceso %d removido de Susp/Ready, cantidad en Susp/Ready: %d", elem_iterado->pid, cola_suspended_ready->elements_count);
			log_info(logger, "Cantidad en Ready: %d", cola_ready->elements_count);
			sem_post(&sem_hay_procesos_en_ready);
		
			/*if(list_size(cola_suspended_ready)) {
				sem_post(&es_el_turno_de_new);
			}*/
		}
		pthread_mutex_unlock(&mutexSuspendedReady);
		log_info(logger, "No hay procesos para suspender");
	}
}

void pasar_de_blocked_susp_a_ready_susp() {

}

/* ********** PLANIFICADOR CORTO PLAZO ********** */

void* pasar_de_ready_a_exec_FIFO() {  // esta hecha asi nomas para probar
	while(1) {
		log_info(logger, "Ejecutando fifo");
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
	
			log_info(logger, "Enviando proceso para ejecutar con pid: %d", proceso_exec->pid);

			pthread_mutex_lock(&mutex_vg_ex);
			hay_un_proceso_ejecutando = true;
			pthread_mutex_unlock(&mutex_vg_ex);

			send_proceso_a_cpu(pcb, (pcb->len_instrucciones)*sizeof(instruccion));
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
		proceso_exec->timestamp = time(NULL) * 1000; //guardo cuando ingreso a la cola blocked. time devuelve segundos, lo paso a milisegundos

		pthread_mutex_lock(&mutexBlock);
		list_add(cola_blck, proceso_exec);
		pthread_mutex_unlock(&mutexBlock);

		pthread_mutex_unlock(&mutexExe);

		pthread_mutex_lock(&mutex_vg_ex);
		hay_un_proceso_ejecutando = false;
		pthread_mutex_unlock(&mutex_vg_ex);

		log_info(logger, "Proceso %d removido de Running. Replanificando...", proceso_exec->pid);

		if(!strcmp(config.ALGORITMO_PLANIFICACION, "FIFO")) {
			sem_post(&sem_planificar_FIFO);
			sem_post(&sem_ejecutar_IO);
		} else {
			// semaforos SRT
		}	

		int tiempo_en_bloqueo = calcular_tiempo_que_estara_bloqueado();
		if(tiempo_en_bloqueo >= config.TIEMPO_MAXIMO_BLOQUEADO) {
			//se que se va a suspender
			pthread_mutex_lock(&mutexProcesosQueSeVanASuspender);
			list_add(procesos_que_se_van_a_suspender, proceso_exec);
			pthread_mutex_unlock(&mutexProcesosQueSeVanASuspender);

			timer(proceso_exec->timestamp);
		}

	} else {
		pthread_mutex_unlock(&mutexExe);
		log_error(logger, "Error grave de planificacion");
	}
}

void* pasar_de_ready_a_exec_SRT() {  

}
/*
void* pasar_de_bloqueado_a_bloqueado_susp() {
	int limite = config.TIEMPO_MAXIMO_BLOQUEADO;
	while(1) {
		time_t tiempo_actual = time(NULL) * 1000;

		t_list_iterator* iterator = list_iterator_create(cola_blck);
		PCB* elem_iterado;

		while(list_iterator_has_next(iterator)) {
			elem_iterado = list_iterator_next(iterator);
			if(tiempo_actual - elem_iterado->timestamp >= limite) {

				pthread_mutex_lock(&mutexSuspendedBlocked);
				list_add(cola_suspended_blck, elem_iterado);
				pthread_mutex_unlock(&mutexSuspendedBlocked);
				log_info(logger, "Pidiendo a memoria pasar el proceso a swap");
				solicitar_swap_out_a_memoria(elem_iterado);	
			}
		}
		list_iterator_destroy(iterator);
	}
}*/

void timer(int tiempo) {
    int tiempo_medido, delta_tiempo = 0;

    while(delta_tiempo < config.TIEMPO_MAXIMO_BLOQUEADO) {
        tiempo_medido = time(NULL) * 1000;
        delta_tiempo = tiempo_medido - tiempo;
    }
	//sale del while cuando se cumple el tiempo maximo de bloqueo
	sem_post(&sem_hilo_blocked_a_blocked_susp);
}

void* pasar_de_bloqueado_a_bloqueado_susp() {
	while(1) {
		sem_wait(&sem_hilo_blocked_a_blocked_susp);

		pthread_mutex_lock(&mutexProcesosQueSeVanASuspender);
		PCB* pcb = list_remove(procesos_que_se_van_a_suspender, 0);
		pthread_mutex_unlock(&mutexProcesosQueSeVanASuspender);

		bool tienen_mismo_pid(void* elemento) {
			if(pcb->pid == ((PCB*) elemento)->pid)
				return true;
			else
				return false;
		}

		pthread_mutex_lock(&mutexBlock);
		list_remove_by_condition(cola_blck, tienen_mismo_pid);
		pthread_mutex_unlock(&mutexBlock);

		pthread_mutex_lock(&mutexSuspendedBlocked);
		list_add(cola_suspended_blck, pcb);
		pthread_mutex_unlock(&mutexSuspendedBlocked);

		log_info(logger, "Pidiendo a memoria pasar el proceso a swap. PROCESO %d SUSPENDIDO", pcb->pid);
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
	}
	
	log_info(logger, "Tiempo que estará bloqueado: %d", tiempo_total);
	list_iterator_destroy(iterator);

	pthread_mutex_unlock(&mutexBlock);

	return tiempo_total;	
}

/* ********** DISPOSITIVO IO ********** */

void* ejecutar_IO() {
	while(1) {
		log_info(logger, "Dispositivo de IO listo");
		sem_wait(&sem_ejecutar_IO);
		sem_wait(&IO_esta_disponible);

		pthread_mutex_lock(&mutex_vg_io);
		bool IO_esta_ocupado = IO_ocupado;
		pthread_mutex_unlock(&mutex_vg_io);

		pthread_mutex_lock(&mutexSuspendedBlocked);
		if(!IO_esta_ocupado && list_size(cola_suspended_blck) > 0) {
			PCB* pcb = list_get(cola_suspended_blck, 0);
			pthread_mutex_unlock(&mutexSuspendedBlocked);
			int rafaga_io = pcb->tiempo_bloqueo;

			sleep(rafaga_io / 1000);

			pthread_mutex_lock(&mutex_vg_io);
			IO_esta_ocupado = false;
			pthread_mutex_unlock(&mutex_vg_io);
			
			sem_post(&IO_esta_disponible);

			pthread_mutex_lock(&mutexSuspendedBlocked);
			list_remove(cola_suspended_blck, 0);
			pthread_mutex_unlock(&mutexSuspendedBlocked);

			pasar_de_blocked_susp_a_ready_susp();
			log_info(logger, "Pasando proceso %d a Ready/Susp", pcb->pid);
		} else {
			pthread_mutex_unlock(&mutexSuspendedBlocked);

			pthread_mutex_lock(&mutexBlock);
			PCB* pcb = list_get(cola_blck, 0);
			pthread_mutex_unlock(&mutexBlock);

			int rafaga_io = pcb->tiempo_bloqueo;

			sleep(rafaga_io / 1000);

			pthread_mutex_lock(&mutex_vg_io);
			IO_esta_ocupado = false;
			pthread_mutex_unlock(&mutex_vg_io);
			
			sem_post(&IO_esta_disponible);

			pthread_mutex_lock(&mutexBlock);
			list_remove(cola_blck, 0);
			pthread_mutex_unlock(&mutexBlock);
			log_info(logger, "Removiendo proceso %d de Blocked", pcb->pid);

			pthread_mutex_lock(&mutexReady);
			list_add(cola_ready, pcb);
			pthread_mutex_unlock(&mutexReady);
			log_info(logger, "Proceso %d agregado a Ready", pcb->pid);

			sem_post(&sem_hay_procesos_en_ready);	
		}
	}
}

/*
	while(1) {
		log_info(logger, "Ejecutando fifo");
		sem_wait(&sem_planificar_FIFO);
		sem_wait(&sem_hay_procesos_en_ready);

		pthread_mutex_lock(&mutex_vg_ex);
		bool cpu_ocupada = hay_un_proceso_ejecutando;
		pthread_mutex_unlock(&mutex_vg_ex);

		PCB* pcb;
		pthread_mutex_lock(&mutexReady);
		if(list_size(cola_ready) && !cpu_ocupada) {	
			pcb = list_remove(cola_ready, 0);
			pthread_mutex_unlock(&mutexReady);

			pthread_mutex_lock(&mutexExe);
			proceso_exec = *pcb;
			pthread_mutex_unlock(&mutexExe);
			
			log_info(logger, "Enviando proceso para ejecutar con pid: %d", proceso_exec.pid);

			pthread_mutex_lock(&mutex_vg_ex);
			hay_un_proceso_ejecutando = true;
			pthread_mutex_unlock(&mutex_vg_ex);

			send_proceso_a_cpu(pcb, (pcb->len_instrucciones)*sizeof(instruccion));
		} else {
			pthread_mutex_unlock(&mutexReady);
		}
	}

/*
void* pasar_de_exec_a_exit() {

	while(1)
	{
		sem_wait(&sem_hilo_exec_exit);

		pthread_mutex_lock(&mutex_popular_cola_ready);
		PCB* elem_iterado;

		int GRADO_MULTIPROGRAMACION = config.GRADO_MULTIPROGRAMACION;
		int PROCESOS_EN_MEMORIA = cola_ready->elements_count + cola_blck->elements_count + cola_exec->elements_count;
		int count=0;

		t_list* cola_a_revisar = cola_suspended_ready;
		pthread_mutex_t *mutex = &mutexSuspendedReady;
		char* cola_para_revisar = "Suspended/Ready";
		if(cola_suspended_ready->elements_count == 0){
			cola_a_revisar = cola_new;
			mutex = &mutexNew;
			cola_para_revisar = "New";
		}
		
		while( count<(cola_a_revisar->elements_count) && (PROCESOS_EN_MEMORIA < GRADO_MULTIPROGRAMACION) ){
			pthread_mutex_lock(mutex);
			t_list_iterator* iterator = list_iterator_create(cola_a_revisar);

			if(list_iterator_has_next(iterator)){
				elem_iterado = list_remove(cola_a_revisar, count);
				elem_iterado -> tabla_paginas = solicitar_tabla_de_paginas_a_memoria(elem_iterado);			
				log_info(logger, "Proceso removido, Cantidad en cola %s: %d", cola_para_revisar, cola_a_revisar->elements_count);
				pthread_mutex_lock(&mutexReady);
				list_add(cola_ready, elem_iterado);
				log_info(logger, "Cantidad en Ready: %d", cola_ready->elements_count);
				pthread_mutex_unlock(&mutexReady);
			}
			pthread_mutex_unlock(mutex);
			count++;
		}

	}
}

void* planificador_mediano_plazo(){
	while(1)
	{
	}
}

void* planificador_corto_plazo(){
	while(1){
	}
}

	//Planificador Mediano Plazo
		//Acciones:
			//- Mover procesos de Blocked a Suspensded-Blocked
			//- Mover procesos de Suspended-Blocked a Suspended-Ready (aunque podria estar en otro lado porque esto ocurre cuando termina la E/S). Podria ser que se reciba un mensaje de que termina la entrada salida, entonces ahi haces esto.
			//- Mover procesos de Suspended-Ready a Ready. Esto tiene mas prioridad que los que se mueven de New a Ready (Planificador Largo Plazo)

		// Eventos para hacer signal
			//En otro hilo formar la estructura lista de struct de elementos proceso y timestamp, y que  alguno alcance el limite establecido en config. Esto seria en un while 1 que empieza cuando al menos hay 1 bloqueado y sale cuando no hay bloqueados.
			//- Se recibe por ej un mensaje diciendo que finalizo la E/S entonces lo pasas de estado.
			//- Cuando cambie el grado de multiprogramacion.
	

		//Planificador Largo Plazo
		//Acciones:
			//- Mover procesos de New a Ready - Decirle a memoria que de de alta sus estructuras. Esto se hace si no hay procesos en suspended-ready (porque tiene mas prioridad los de suspended ready) y si el grado de multiprogramacion lo permite
			//- Finalizar un proceso - Dar de baja sus estructuras, sacarlo de la cola Exit y responder a la consola.
			
		// Eventos para hacer signal aca
			// Cuando un proceso entra en la cola de new
			// Cuando recibis un exit de un proceso
			// Cuando se modifique (incremente) el grado de multi desde otro lado

/*void mostrar_instrucciones(void* stream, int len_instrucciones){

	int offset=0;
	operacion id_operacion;
	uint32_t operando1;
	uint32_t operando2;

	for(int i=0; i<len_instrucciones; i++) {
		memcpy(&id_operacion, stream+offset, sizeof(operacion));
		offset+=sizeof(operacion);
		memcpy(&operando1, stream+offset, sizeof(uint32_t));
		offset+=sizeof(uint32_t);
		memcpy(&operando2, stream+offset, sizeof(uint32_t));
		offset+=sizeof(uint32_t);
		log_info(logger, "id_operacion: %d - operando1: %d - operando2: %d", id_operacion, operando1, operando2 );
	}
	log_info(logger, "------------------ DONE ---------------");
}*/