#include "include/main.h"


void* atender_pedido(void* void_args)
{
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
			// mostrar_instrucciones(stream, len_instrucciones);
			log_info(logger, "Termine de loguear instrucciones");


			//Planificador Corto Plazo  enviar a CPU por el puerto dispatch en el momento de ejecutar
			//Planificador Mediano Plazo - Este planificador se encargará de gestionar las transiciones
			//Planificador Largo
					// Armar PCB
					// Llevar el proceso al estado New;
					// Mover a Ready si el grado de multiprogramacion lo admite y en ese caso pedir a memoria iniciar las estructuras

			/* Creo PCB cuando recibo las instrucciones */
			
			/* Hay que pedir la tabla de paginas a la memoria*/

			planificador_largo_plazo(tamanio_proceso, stream, len_instrucciones, args->config, args->conexion_dispatch);

			free(stream);
			break;
		default:
			log_warning_sh(logger, "Operacion desconocida.");
			close(args->cliente_fd);
			break;
	}
	free(args);
}

void planificador_largo_plazo(int tam_proceso, void* stream, int len_instrucciones, Config config, int conexion_dispatch) {
	PCB pcb;
	crear_pcb(&pcb, tam_proceso, stream, len_instrucciones, config.ESTIMACION_INICIAL);
	log_info(logger, "PCB creado: PDI es %d - Tamaño: %d - PC: %d - Tabla de páginas: %d - Estimación Inicial: %d", pcb.pid, pcb.tamanio_proceso, pcb.program_counter , pcb.tabla_paginas, pcb.estimacion_rafaga);
	
	send_proceso_a_cpu(&pcb, len_instrucciones*sizeof(instruccion), conexion_dispatch);	// lo dejo aca para probar ahora. esto deberia ir en  el planificador de corto plazo
	log_info(logger, "Nuevo proceso recibido. Entra directo a NEW");
	list_add(cola_new, &pcb);
	log_info(logger, "Cantidad de procesos en NEW: %d", cola_new->elements_count);
	if (cola_ready->elements_count < config.GRADO_MULTIPROGRAMACION)
	{
		
		log_info(logger, "Puede entrar un nuevo proceso a READY, Cantidad de procesos en READY: %d", cola_ready->elements_count);

		t_list_iterator* iterator = list_iterator_create(cola_new);

		void* elem_iterado = malloc(sizeof(PCB));
		int count=0;
		while(count<(cola_new->elements_count)){

			if(list_iterator_has_next(iterator)){
				elem_iterado = list_iterator_next(iterator);
				if( ((PCB*)elem_iterado)->pid == pcb.pid ){
					list_iterator_remove(iterator);
					log_info(logger, "Elemento con tamanio %d ha sido removido", ((PCB*)elem_iterado)->tamanio_proceso);
					break;
				}
			}
			count++;
		}
		//free(elem_iterado);
		list_add(cola_ready, &pcb);
		log_info(logger, "Cantidad de procesos en NEW: %d", cola_new->elements_count);
		log_info(logger, "Cantidad de procesos en READY: %d", cola_ready->elements_count);
	}else{
		log_info(logger, "El proceso se queda en NEW. El grado de multiprog no se la aguanta.");
	}

}

void mostrar_instrucciones(void* stream, int len_instrucciones){

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
}

int main(void) {

	logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);
	Config config;
	cargarConfig("kernel.config", &config);


	inicializar_colas();
	inicializar_semaforos();
	log_info(logger, "Colas y semaforos inicializadas");


	int kernel_server = iniciar_servidor("127.0.0.1", config.PUERTO_ESCUCHA, SOMAXCONN);

	if(!kernel_server) {
		log_error(logger, "Error al iniciar el servidor Kernel\nCerrando el programa");
		return 1;
	}

    log_info(logger, "Kernel listo para recibir clientes");

	int conexion_dispatch = crear_conexion(config.IP_CPU, config.PUERTO_CPU_DISPATCH, logger);
	int conexion_interrupt = crear_conexion(config.IP_CPU, config.PUERTO_CPU_INTERRUPT, logger);
	int conexion_memoria = crear_conexion(config.IP_MEMORIA, config.PUERTO_MEMORIA, logger);

	pthread_t hilo_atender_pedido;

	while(1) {
		int kernel_cliente = esperar_cliente(kernel_server, logger);
		/* Parametros que necesita atender_pedido */
		args_thread *args = malloc(sizeof(args_thread));
		args->cliente_fd = kernel_cliente;
		args->conexion_memoria = conexion_memoria;
		args->config = config;
		args->conexion_dispatch = conexion_dispatch;
		args->conexion_interrupt = conexion_interrupt;
		pthread_create( &hilo_atender_pedido, NULL, atender_pedido, (void*) args);
		pthread_join(hilo_atender_pedido,NULL);
	}

	return 0;

}

void inicializar_colas()
{
	cola_new = list_create();
	cola_ready = list_create();
	cola_exec = list_create();
	cola_blck = list_create();
	cola_finish = list_create();

}

void inicializar_semaforos(){

	pthread_mutex_init(&mutexBlockSuspended, NULL);
	pthread_mutex_init(&mutexReadySuspended, NULL);
	pthread_mutex_init(&mutexNew, NULL);
	pthread_mutex_init(&mutexReady, NULL);
	pthread_mutex_init(&mutexBlock, NULL);
	pthread_mutex_init(&mutexExe, NULL);
	pthread_mutex_init(&mutexExit, NULL);

}