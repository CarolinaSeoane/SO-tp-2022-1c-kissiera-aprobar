#include "../include/planificacion.h"

void crear_y_poner_proceso_en_new(int tam_proceso, void* stream, int len_instrucciones, int cliente_fd) {
	PCB pcb;
	crear_pcb(&pcb, tam_proceso, stream, len_instrucciones, config.ESTIMACION_INICIAL, cliente_fd);
	log_info(logger, "PCB creado: PDI es %d - Tamaño: %d - PC: %d - Tabla de páginas: %d - Estimación Inicial: %d", pcb.pid, pcb.tamanio_proceso, pcb.program_counter , pcb.tabla_paginas, pcb.estimacion_rafaga);
	
	// Agrego el proceso a New
	pthread_mutex_lock(&mutexNew);
	list_add(cola_new, &pcb);
	pthread_mutex_unlock(&mutexNew);

	// Signal al hilo new - ready
	log_info(logger, "Nuevo proceso agregado a NEW, cantidad de procesos en NEW: %d", cola_new->elements_count);
	sem_post(&sem_hilo_new_ready);
    //sem_post(&planificador_largo_plazo);
}

/* ********** PLANIFICADOR LARGO PLAZO ********** */

void* intentar_pasar_de_new_a_ready() {
	while(1) {
		sem_wait(&sem_hilo_new_ready);
        //sem_wait(&planificador_largo_plazo);

   		int GRADO_MULTIPROGRAMACION = config.GRADO_MULTIPROGRAMACION;
		int PROCESOS_EN_MEMORIA = cola_ready->elements_count + cola_blck->elements_count + hay_un_proceso_ejecutando;

		log_info(logger, "PROCESOS EN MEMORIA: %d", PROCESOS_EN_MEMORIA);
		if(PROCESOS_EN_MEMORIA < GRADO_MULTIPROGRAMACION) {
			pthread_mutex_lock(&mutexSuspendedReady);
			if(cola_suspended_ready->elements_count > 0) {
				pthread_mutex_unlock(&mutexSuspendedReady);
				sem_post(&sem_hilo_ready_susp_ready);
			} else {
				pthread_mutex_unlock(&mutexSuspendedReady);
				pasar_de_new_a_ready();
			}
		}
    }
}

void pasar_de_new_a_ready() {
	PCB* elem_iterado;

    if(list_size(cola_new)) {
		pthread_mutex_lock(&mutexNew);
		elem_iterado = list_remove(cola_new, 0);
		pthread_mutex_unlock(&mutexNew);

		log_info(logger, "Pidiendo pagina a memoria");
		elem_iterado->tabla_paginas = 7; //solicitar_tabla_de_paginas_a_memoria(elem_iterado, conexion_memoria);	

		pthread_mutex_lock(&mutexReady);
		list_add(cola_ready, elem_iterado);
		pthread_mutex_unlock(&mutexReady);

		log_info(logger, "Proceso %d removido de New, cantidad en New: %d", elem_iterado->pid, cola_new->elements_count);
		log_info(logger, "Cantidad en Ready: %d", cola_ready->elements_count);
		sem_post(&sem_hay_procesos_en_ready);
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
		proceso_exec.program_counter = pc;
		consola = proceso_exec.cliente_fd;
		pid_exec = proceso_exec.pid;
		pthread_mutex_unlock(&mutexExe);

		if(pid_exec == pid) {
			int codigo = 1;
			void* a_enviar = malloc(sizeof(int));
			memcpy(a_enviar, &codigo, sizeof(int));
			
			log_info(logger, "Enviando finalizacion a consola");
			send(consola, a_enviar, sizeof(int), 0);

			pthread_mutex_lock(&mutex_vg_ex);
			hay_un_proceso_ejecutando = false;
			pthread_mutex_unlock(&mutex_vg_ex);

			memset(&proceso_exec, 0, sizeof(PCB));
			
			if(!strcmp(config.ALGORITMO_PLANIFICACION, "FIFO")) {
				sem_post(&sem_planificar_FIFO);
			} else {
				// semaforos SRT
			}			
		} else {
			log_error(logger, "Error grave de planificacion");
		}
	}
}

/* ********** PLANIFICADOR MEDIANO PLAZO ********** */




void* pasar_de_bloqueado_a_susp() { //ver
	while(1) {

		sem_wait(&sem_hilo_ready_susp_ready);


	pthread_mutex_lock(&mutexSuspendedBlocked);
	
	PCB* elem_iterado;
	int TIEMPO_MAXIMO_BLOQUEADO = config.TIEMPO_MAXIMO_BLOQUEADO;
	int TIEMPO_BLOQUEADO = 0 ; //elem_iterado-> tiempo_en_blok;   despues corro un hilo para verificar el tiempo en blok 
	int pid_bloqueado;

	pthread_mutex_lock(&mutex_vg_ex);
	bool cpu_ocupada = hay_un_proceso_ejecutando;
	pthread_mutex_unlock(&mutex_vg_ex);

	while( TIEMPO_BLOQUEADO < TIEMPO_MAXIMO_BLOQUEADO ){
       
		pthread_mutex_lock(&mutexBlock);
		elem_iterado = list_remove(cola_blck, 0);
		pthread_mutex_unlock(&mutexBlock);

		log_info(logger, "Pidiendo a memoria pasarel proceso a swap ");
		solicitar_swap_in_a_memoria(elem_iterado, conexion_memoria);	
          
		pthread_mutex_lock(&mutexSuspendedBlocked);
		list_add(cola_suspended_blck, elem_iterado);
		pthread_mutex_unlock(&mutexSuspendedBlocked);

		log_info(logger, "Proceso %d removido de Block, cantidad en Block: %d", elem_iterado->pid, cola_new->elements_count);
		log_info(logger, "Cantidad en Susoendido: %d", cola_suspended_blck->elements_count);
		sem_post(&sem_hilo_new_ready);
    
	
     }
   }
}


/*		PCB* pcb = malloc(sizeof(PCB));

		pthread_mutex_lock(&mutexSuspendedReady);
		pcb = (PCB*) list_remove(cola_suspended_ready, 0);
		pthread_mutex_unlock(&mutexSuspendedReady);

		pthread_mutex_lock(&mutexReady);
		list_add(cola_ready, pcb);
		pthread_mutex_unlock(&mutexReady);

		log_info(logger, "Proceso %d removido de Suspended/Ready, cantidad en Suspended/Ready: %d", pcb->pid, cola_suspended_ready->elements_count);
		log_info(logger, "Cantidad en Ready: %d", cola_ready->elements_count);
		sem_post(&sem_hay_procesos_en_ready);
	

	/* Avisar SWAP IN a memoria */

	// sem_post(&sem_planificador_corto_pĺazo);
	


/* ********** PLANIFICADOR CORTO PLAZO ********** */

void* pasar_de_ready_a_exec_FIFO() {  // esta hecha asi nomas para probar
	while(1) {
		log_info(logger, "Ejecutando fifo");
		sem_wait(&sem_planificar_FIFO);
		sem_wait(&sem_hay_procesos_en_ready);

		PCB* pcb;
		pthread_mutex_lock(&mutexReady);
		if(list_size(cola_ready) && !hay_un_proceso_ejecutando) {	
			pcb = list_remove(cola_ready, 0);
			pthread_mutex_unlock(&mutexReady);

			pthread_mutex_lock(&mutexExe);
			proceso_exec = *pcb;
			pthread_mutex_unlock(&mutexExe);
			
			log_info(logger, "Enviando proceso para ejecutar con pid: %d", proceso_exec.pid);
			hay_un_proceso_ejecutando = true;
			send_proceso_a_cpu(pcb, (pcb->len_instrucciones)*sizeof(instruccion), conexion_dispatch);
		} else {
			pthread_mutex_unlock(&mutexReady);
		}
	}
}

void* pasar_de_ready_a_exec_SRT() {  

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
				elem_iterado -> tabla_paginas = solicitar_tabla_de_paginas_a_memoria(elem_iterado, conexion_memoria);			
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