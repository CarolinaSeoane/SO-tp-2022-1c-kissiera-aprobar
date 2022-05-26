#include "../include/serializacion.h"

void recv_proceso_init(int *pid, int *tamanio_proceso, int cliente) {
	recv(cliente, pid, sizeof(int), 0);
	recv(cliente, tamanio_proceso, sizeof(int), 0);
}