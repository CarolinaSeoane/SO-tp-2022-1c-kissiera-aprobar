#include "include/main.h"

void crear_y_poner_proceso_en_new(int tam_proceso, void* stream, int len_instrucciones) {
	PCB pcb;
	crear_pcb(&pcb, tam_proceso, stream, len_instrucciones, config.ESTIMACION_INICIAL);
	log_info(logger, "PCB creado: PDI es %d - Tamaño: %d - PC: %d - Tabla de páginas: %d - Estimación Inicial: %d", pcb.pid, pcb.tamanio_proceso, pcb.program_counter , pcb.tabla_paginas, pcb.estimacion_rafaga);
	
	//send_proceso_a_cpu(&pcb, len_instrucciones*sizeof(instruccion), conexion_dispatch);	// lo dejo aca para probar ahora. esto deberia ir en  el planificador de corto plazo
	
	pthread_mutex_lock(&mutexNew);
	list_add(cola_new, &pcb);
	PCB* elem_agregado = list_get(cola_new, cola_new->elements_count -1);
	log_info(logger, "Leido de la lista -> PID: %d - Tamaño: %d - PC: %d - Tabla de páginas: %d - Estimación Inicial: %d",elem_agregado->pid, elem_agregado->tamanio_proceso, elem_agregado->program_counter , elem_agregado->tabla_paginas, elem_agregado->estimacion_rafaga);
	pthread_mutex_unlock(&mutexNew);
	log_info(logger, "Nuevo proceso agregado a NEW, cantidad de procesos en NEW: %d", cola_new->elements_count);
	pthread_mutex_unlock(&mutex_mover_de_new_a_ready);

}


void* atender_pedido(void* void_args) {
    args_thread* args = (args_thread*) void_args;

	int accion;
	recv(args->cliente_fd, &accion, sizeof(accion), 0);
	log_info(logger, "Acción: %d", accion);


	switch(accion)
	{
		case ENVIAR_INSTRUCCIONES: ; //soluciona error de label
			int len_instrucciones;
			recv(args->cliente_fd, &len_instrucciones, sizeof(int), 0);
			log_info(logger, "Cantidad Instrucciones: %d", len_instrucciones);

			int tamanio_proceso;
			recv(args->cliente_fd, &tamanio_proceso, sizeof(int), 0);
			log_info(logger, "Tamaño del proceso: %d", tamanio_proceso);

			void* stream = malloc(len_instrucciones*sizeof(instruccion));
			log_info(logger, "Cantidad reservada para stream %d", len_instrucciones*sizeof(instruccion));
			recv(args->cliente_fd, stream, len_instrucciones*sizeof(instruccion), 0);
			log_info(logger, "Termine de loguear instrucciones");


			//Planificador Corto Plazo  enviar a CPU por el puerto dispatch en el momento de ejecutar
			//Planificador Mediano Plazo - Este planificador se encargará de gestionar las transiciones


			/* Creo PCB cuando recibo las instrucciones */
			
			/* Hay que pedir la tabla de paginas a la memoria*/

			crear_y_poner_proceso_en_new(tamanio_proceso, stream, len_instrucciones);

			free(stream);
			break;
		default:
			log_warning_sh(logger, "Operacion desconocida.");
			close(args->cliente_fd);
			break;
	}
	free(args);
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
void* mover_procesos_a_ready_desde_new() {
	
	
	while(1)
	{
		
	//Planificador Largo Plazo
		//Acciones:
			//- Mover procesos de New a Ready - Decirle a memoria que de de alta sus estructuras. Esto se hace si no hay procesos en suspended-ready (porque tiene mas prioridad los de suspended ready) y si el grado de multiprogramacion lo permite
			//- Finalizar un proceso - Dar de baja sus estructuras, sacarlo de la cola Exit y responder a la consola.
			
		// Eventos para hacer signal aca
			// Cuando un proceso entra en la cola de new
			// Cuando recibis un exit de un proceso
			// Cuando se modifique (incremente) el grado de multi desde otro lado

		pthread_mutex_lock(&mutex_mover_de_new_a_ready);
		args_thread* args = (args_thread*) args;
		int GRADO_MULTIPROGRAMACION = config.GRADO_MULTIPROGRAMACION;
		int count=0;
		PCB* elem_iterado;
		
		while( count<(cola_new->elements_count) && (cola_ready->elements_count < GRADO_MULTIPROGRAMACION) ){
			pthread_mutex_lock(&mutexNew);
			t_list_iterator* iterator = list_iterator_create(cola_new);

			if(list_iterator_has_next(iterator)){
				elem_iterado = list_remove(cola_new, count);
				elem_iterado -> tabla_paginas = solicitar_tabla_de_paginas_a_memoria(elem_iterado, conexion_memoria);			
				log_info(logger, "Proceso sacado de New, Cantidad en New: %d", cola_new->elements_count);
				pthread_mutex_lock(&mutexReady);
				list_add(cola_ready, elem_iterado);
				log_info(logger, "Cantidad en Ready: %d", cola_ready->elements_count);
				pthread_mutex_unlock(&mutexReady);
				
			}
			pthread_mutex_unlock(&mutexNew);
			count++;
		}

	}
}

int main(void) {

	inicializar_logger();
	inicializar_config();
	inicializar_servidor();
	inicializar_colas();
	inicializar_semaforos();
	inicializar_conexiones();
	
	log_info(logger, "Inicializacion de Kernel terminada");
	pthread_t hilo_largo_plazo_mover_de_new_a_ready;
	pthread_create( &hilo_largo_plazo_mover_de_new_a_ready, NULL, mover_procesos_a_ready_desde_new, NULL);
	
	while(server_escuchar(kernel_server));

	destroy_recursos();

	return 0;

}

int server_escuchar(int kernel_server) {
	int kernel_cliente = esperar_cliente(kernel_server, logger);

    if (kernel_cliente != -1) {
        pthread_t hilo_atender_pedido;

		args_thread *args = malloc(sizeof(args_thread));
		args->cliente_fd = kernel_cliente;
		
		pthread_create( &hilo_atender_pedido, NULL, atender_pedido, (void*) args);
        pthread_detach(hilo_atender_pedido);
        return 1;
    }
    return 0;
}