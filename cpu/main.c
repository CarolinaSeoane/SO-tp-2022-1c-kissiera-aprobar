#include "include/main.h"

int main() {

	// ------ CONFIG INICIAL DE CPU ----------
	logger = log_create("cpu.log", "Cpu", 1, LOG_LEVEL_DEBUG);
	Config config;
	cargarConfig("cpu.config", &config);

	// ------ HANDSHAKE CON MEMORIA ----------
	int conexion_memoria = crear_conexion(config.IP_MEMORIA, config.PUERTO_MEMORIA, logger);
	send_handshake(conexion_memoria);
	recv_handshake(conexion_memoria);

	// ------ SERVIDORES DISPATCH E INTERRUPT PARA KERNEL
	inicializar_semaforo();
	int dispatch = iniciar_servidor("127.0.0.1", config.PUERTO_ESCUCHA_DISPATCH, 1);
	int interrupt = iniciar_servidor("127.0.0.1", config.PUERTO_ESCUCHA_INTERRUPT, 1);

	if(!dispatch || !interrupt) {
		log_error(logger, "Error al iniciar la conexión dispatch o interrupt\nCerrando el programa");
		return EXIT_FAILURE;
	}

	log_info(logger, "CPU esperando conexión de Kernel");

	// ------ RECIBE INTERRUPCIONES DE KERNEL
	pthread_t hilo_interrupt;
	int cliente_interrupt = esperar_cliente(interrupt, logger);
	args_interrupt *args_i = malloc(sizeof(args_interrupt));
	args_i->cliente_interrupt_fd = cliente_interrupt;
	args_i->config = config;
	pthread_create(&hilo_interrupt, NULL, atender_interrupt, (void*) args_i);
	
	// ------ RECIBE PROCESOS DE KERNEL
	pthread_t hilo_dispatch;
	int cliente_dispatch = esperar_cliente(dispatch, logger);
	args_dispatch *args_d = malloc(sizeof(args_dispatch));
    args_d->cliente_dispatch_fd = cliente_dispatch;
	args_d->con_memoria = conexion_memoria;
	args_d->config = config;
	pthread_create(&hilo_dispatch, NULL, atender_dispatch, (void*) args_d);

	// ------ ESPERA TERMINACION DE HILOS
	pthread_join(hilo_dispatch, NULL);
	pthread_join(hilo_interrupt, NULL);

	// ------ FINALIZACION
	sem_destroy(&mutex_flag_interrupcion);

}
