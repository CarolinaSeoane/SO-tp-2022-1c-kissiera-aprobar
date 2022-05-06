#include "../include/pedidos.h"

void* atender_pedido(void* void_args) {

	args_thread_memoria* args = (args_thread_memoria*) void_args;

	int accion;
	recv(args->cliente_fd, &accion, sizeof(accion), 0);
	printf("Acción: %d\n", accion);

    switch(accion) {
        case READ_M:
            
            break;

        case WRITE_M:
            
            break;

        case INIT_PROCESO:
            
            break;

        case SUSP_PROCESO:
            
            break;

        case HANDSHAKE_MEMORIA:
        	log_info(logger, "Se recibio un handshake con CPU");
        	send_cpu_handshake((void*) args);
        	break;
        default:
            log_warning_sh(logger, "Operacion desconocida.");
			close(args->cliente_fd);
			break;
    }
	free(args);
}

void send_cpu_handshake(void* void_args) {

	args_thread_memoria* args = (args_thread_memoria*) void_args;

	int offset = 0;
	void* a_enviar = malloc(sizeof(int)*3);

	// Se envia este codigo pero CPU no lo usa realmente por ahora
	int* codigo = malloc(sizeof(int));
	*codigo = HANDSHAKE_MEMORIA;
	memcpy(a_enviar, &(*codigo), sizeof(int));
	offset += sizeof(int);

	memcpy(a_enviar+offset, &(args->config.TAM_PAGINA), sizeof(int));
	offset += sizeof(int);

	memcpy(a_enviar+offset, &(args->config.ENTRADAS_POR_TABLA), sizeof(int));

	send(args->cliente_fd, a_enviar, sizeof(int)*3, 0);
	free(a_enviar);
	free(codigo);
}
