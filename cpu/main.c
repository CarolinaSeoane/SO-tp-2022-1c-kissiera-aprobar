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
	int dispatch = iniciar_servidor("127.0.0.1", config.PUERTO_ESCUCHA_DISPATCH, 1);
	int interrupt = iniciar_servidor("127.0.0.1", config.PUERTO_ESCUCHA_INTERRUPT, 1);

	if(!dispatch || !interrupt) {
		log_error(logger, "Error al iniciar la conexión dispatch o interrupt\nCerrando el programa");
		return EXIT_FAILURE;
	}

	log_info(logger, "CPU esperando conexión de Kernel");

	// ------ RECIBE CONEXIONES DE KERNEL EN DISPATCH O INTERRUPT

	pthread_t hilo_atender_conexiones;

	int cliente_dispatch = esperar_cliente(dispatch, logger);

	args_dispatch *args_d = malloc(sizeof(args_dispatch));
    args_d->cliente_fd = cliente_dispatch;

	pthread_create(&hilo_atender_conexiones, NULL, atender_dispatch, (void*) args_d);
	pthread_join(hilo_atender_conexiones, NULL);

	int cliente_interrupt = esperar_cliente(interrupt, logger);
	pthread_create(&hilo_atender_conexiones, NULL, atender_interrupt, NULL);
	pthread_join(hilo_atender_conexiones, NULL);


}
