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
			PCB pcb;		
			crear_pcb(&pcb, tamanio_proceso, stream, len_instrucciones, args->config.ESTIMACION_INICIAL);
			printf("Se crea PID\n");
			printf("El PID es %d\n", pcb.pid);
			printf("El tamaño es %d\n", pcb.tamanio_proceso);
			mostrar_instrucciones(pcb.stream, len_instrucciones);
			printf("El PC es %d\n", pcb.program_counter);
			printf("Tabla de paginas es %d\n", pcb.tabla_paginas);
			printf("Estimacion inicial %d\n", pcb.estimacion_rafaga);

			break;
		default:
			log_warning_sh(logger, "Operacion desconocida.");
			close(args->cliente_fd);
			break;
	}
	free(args);

	return EXIT_SUCCESS;
}

int main(void) {

	logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);
	Config config;
	cargarConfig("kernel.config", &config);

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
		pthread_create( &hilo_atender_pedido, NULL, atender_pedido, (void*) args);
		pthread_join(hilo_atender_pedido,NULL);
	}

	return 0;

}



int mostrar_instrucciones(void* stream, int len_instrucciones){

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
	free(stream);
}