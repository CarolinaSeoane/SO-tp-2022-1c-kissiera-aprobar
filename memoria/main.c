#include "include/main.h"

int main(void) {

    logger = log_create("memoria.log", "Memoria", 1, LOG_LEVEL_DEBUG);
    Config config;
    cargarConfig("memoria.config", &config);

    int memoria_server = iniciar_servidor("127.0.0.1", config.PUERTO_ESCUCHA, 2);

    if(!memoria_server) {
		log_info(logger, "Error al iniciar el servidor Memoria\nCerrando el programa");
		return 1;
	}

    log_info(logger, "Memoria lista para recibir clientes");

    pthread_t hilo_atender_pedido_memoria;

    while(1) {
        int memoria_cliente = esperar_cliente(memoria_server, logger);
        /* Parametros que necesita atender_pedido */
        args_thread_memoria *args = malloc(sizeof(args_thread_memoria));
        args->cliente_fd = memoria_cliente;
        args->config = config;
        pthread_create(&hilo_atender_pedido_memoria, NULL, atender_pedido, (void*) args);
        pthread_join(&hilo_atender_pedido_memoria, NULL);
    }	

	return 0;

}
