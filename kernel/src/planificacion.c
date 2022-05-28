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
}

void* pasar_de_new_a_ready() {
	while(1) {
		sem_wait(&sem_hilo_new_ready);
        // sem_wait(&planificador_largo_plazo); //ver
        
        int GRADO_MULTIPROGRAMACION = config.GRADO_MULTIPROGRAMACION;
		int PROCESOS_EN_MEMORIA = cola_ready->elements_count + cola_blck->elements_count + cola_exec->elements_count;

/* Aca creo que si hay procesos en susp ready tenemos que avisar al planificador de mediano plazo y si no ahi si lo podemos agregar */

/*		pthread_mutex_lock(&mutex_popular_cola_ready);
		PCB* elem_iterado;

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
*/
	}
}
/*
void* pasar_de_exec_a_exit(){

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