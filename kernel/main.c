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
        pthread_t hilo_atender_pedido;

		args_thread *args = malloc(sizeof(args_thread));
		args->cliente_fd = kernel_cliente;
		
		pthread_create( &hilo_atender_pedido, NULL, atender_pedido, (void*) args);
        pthread_detach(hilo_atender_pedido);
        return 1;
    }
    return 0;
}

void* atender_pedido(void* void_args) {
    args_thread* args = (args_thread*) void_args;

	int accion;
	recv(args->cliente_fd, &accion, sizeof(accion), 0);
	log_info(logger, "Acción: %d", accion);

	switch(accion) {
		case ENVIAR_INSTRUCCIONES: ;
			int len_instrucciones;
			recv(args->cliente_fd, &len_instrucciones, sizeof(int), 0);

			int tamanio_proceso;
			recv(args->cliente_fd, &tamanio_proceso, sizeof(int), 0);

			void* stream = malloc(len_instrucciones*sizeof(instruccion));
			recv(args->cliente_fd, stream, len_instrucciones*sizeof(instruccion), 0);

			crear_y_poner_proceso_en_new(tamanio_proceso, stream, len_instrucciones, args->cliente_fd);
			free(stream);
			break;
		case EXIT_PROCESO: ;
			int pid_a_finalizar;
			recv(args->cliente_fd, &pid_a_finalizar, sizeof(int), 0);
			log_info(logger, "ID del proceso a finalizar: %d", pid_a_finalizar);
			//avisar_a_memoria_proceso_finalizado(pid_a_finalizar);	//Avisar a memoria que elimine las estructuras(mandar operacion + pid)
			//Movel el proceso de running a exit con mutex
			//Mandar signal de mutex_popular_cola_ready ya que el grado de multiprogramacion decrementa
		case BLOCK_PROCESO: ;
			break;
		case DESALOJO_PROCESO: ;
			break;
		default:
			log_warning_sh(logger, "Operacion desconocida.");
			close(args->cliente_fd);
			break;
	}
	free(args);
}