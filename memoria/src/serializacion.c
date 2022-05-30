#include "../include/serializacion.h"

void recv_proceso_init(int *pid, int *tamanio_proceso, int cliente) {
	recv(cliente, pid, sizeof(int), 0);
	recv(cliente, tamanio_proceso, sizeof(int), 0);
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

	memcpy(a_enviar+offset, &config.TAM_PAGINA, sizeof(int));
	offset += sizeof(int);

	memcpy(a_enviar+offset, &config.ENTRADAS_POR_TABLA, sizeof(int));

	send(args->cliente_fd, a_enviar, sizeof(int)*3, 0);
	free(a_enviar);
	free(codigo);
}