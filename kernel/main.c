#include "include/main.h"


int atender_pedido(int cliente_fd)
{


	int accion;
	recv(cliente_fd, &accion, sizeof(accion), 0);
	printf("Acción: %d\n", accion);

	int len_instrucciones;
	recv(cliente_fd, &len_instrucciones, sizeof(int), 0);
	printf("Cantidad Instrucciones: %d\n", len_instrucciones);

	int tamanio_proceso;
	recv(cliente_fd, &tamanio_proceso, sizeof(int), 0);
	printf("Tamanio del proceso: %d\n", tamanio_proceso);

	void* stream = malloc(len_instrucciones*sizeof(instruccion));
	printf("Cantidad reservada para stream %d\n", len_instrucciones*sizeof(instruccion));
	recv(cliente_fd, stream, len_instrucciones*sizeof(instruccion), 0);

	switch(accion)
	{
		case ENVIAR_INSTRUCCIONES:
			mostrar_instrucciones(stream, len_instrucciones);
			break;
		default:
			log_warning_sh(logger, "Operacion desconocida.");
			close(cliente_fd);
			break;
	}

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
		pthread_create( &hilo_atender_pedido, NULL,atender_pedido,kernel_cliente);
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










