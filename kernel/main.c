#include "include/main.h"

int main(void) {

	inicializar_logger();
	inicializar_config();
	inicializar_servidor();
	inicializar_colas();
	inicializar_semaforos();
	inicializar_conexiones();
	inicializar_planificacion();
	
	log_info(logger, "Inicializacion de Kernel terminada");
	
	while(server_escuchar(kernel_server));

	destroy_recursos();

	return 0;
}

int server_escuchar(int kernel_server) {
	int kernel_cliente = esperar_cliente(kernel_server, logger);

    if (kernel_cliente != -1) {
        pthread_t hilo_atender_pedidos_consolas;

		args_thread *args = malloc(sizeof(args_thread));
		args->cliente_fd = kernel_cliente;
		
		pthread_create(&hilo_atender_pedidos_consolas, NULL, atender_pedidos_consolas, (void*) args);
        pthread_detach(hilo_atender_pedidos_consolas);
        return 1;
    }
    return 0;
}
