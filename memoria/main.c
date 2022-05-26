#include "include/main.h"

int main(void) {


	inicializar_logger();
	inicializar_config();
	inicializar_servidor();

    log_info(logger, "Inicializacion de Memoria terminada");

    while(server_escuchar(memoria_server));

	destroy_recursos();

	return 0;
}

int server_escuchar(int memoria_server) {
	int memoria_cliente = esperar_cliente(memoria_server, logger);

    if (memoria_cliente != -1) {
        pthread_t hilo_atender_pedido_memoria;

		args_thread_memoria *args = malloc(sizeof(args_thread_memoria));
		args->cliente_fd = memoria_cliente;
		
		pthread_create(&hilo_atender_pedido_memoria, NULL, atender_pedido, (void*) args);
        pthread_join(hilo_atender_pedido_memoria, NULL);
        return 1;
    }
    return 0;
}
