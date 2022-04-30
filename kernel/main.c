#include "include/main.h"


int atender_pedido(void* void_args)
{
    args_thread* args = (args_thread*) void_args;

	int accion;
	recv(args->cliente_fd, &accion, sizeof(accion), 0);
	printf("Acción: %d\n", accion);


	switch(accion)
	{
		case ENVIAR_INSTRUCCIONES: ; //soluciona error de label
			int len_instrucciones;
			recv(args->cliente_fd, &len_instrucciones, sizeof(int), 0);
			printf("Cantidad Instrucciones: %d\n", len_instrucciones);

			int tamanio_proceso;
			recv(args->cliente_fd, &tamanio_proceso, sizeof(int), 0);
			printf("Tamaño del proceso: %d\n", tamanio_proceso);

			void* stream = malloc(len_instrucciones*sizeof(instruccion));
			printf("Cantidad reservada para stream %d\n", len_instrucciones*sizeof(instruccion));
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

	return EXIT_SUCCESS;
}

void planificador_largo_plazo(int tam_proceso, void* stream, int len_instrucciones, Config config, int conexion_dispatch) {
	PCB pcb;
	crear_pcb(&pcb, tam_proceso, stream, len_instrucciones, config.ESTIMACION_INICIAL);
	printf("PCB creado: PDI es %d - Tamaño: %d - PC: %d - Tabla de páginas: %d - Estimación Inicial: %d\n\n", pcb.pid, pcb.tamanio_proceso, pcb.program_counter , pcb.tabla_paginas, pcb.estimacion_rafaga);
	
	send_proceso_a_cpu(&pcb, len_instrucciones*sizeof(instruccion), conexion_dispatch);	// lo dejo aca para probar ahora. esto deberia ir en  el planificador de corto plazo
	
	list_add(cola_new, &pcb);
	printf("Cantidad de procesos en ready: %d", cola_new->elements_count);
	if (cola_new->elements_count < config.GRADO_MULTIPROGRAMACION)
	{
	

		
		
		/* t_list_iterator* list_iterator_create(t_list* list);  devuelve un iterator

		typedef struct {
			t_list *self;
			t_link_element *prev;
			t_link_element *element;
			int index;
		} t_list_iterator;

		Con esto vas en cada uno hasta encontrar el PDI que necesitas ubicar. Sacas el index y despues usas "void *list_get(t_list *, int index)
		Eso te devuelve un t_link_element el cual lo sacas de la lista ready y lo pasas a Ready. Despues de esto habría solicitar a memoria la estructura.

		*/
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
		printf("id_operacion: %d - operando1: %d - operando2: %d\n", id_operacion, operando1, operando2 );
	}
	printf("------------------ DONE ---------------\n\n");
}

int main(void) {

	logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);
	Config config;
	cargarConfig("kernel.config", &config);

	inicializar_colas();
	log_info(logger, "Colas inicializadas");

	int kernel_server = iniciar_servidor("127.0.0.1", config.PUERTO_ESCUCHA, SOMAXCONN);

	if(!kernel_server) {
		log_info(logger, "Error al iniciar el servidor Kernel\nCerrando el programa");
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

