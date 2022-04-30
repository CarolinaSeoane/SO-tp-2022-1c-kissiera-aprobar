#include "../include/serializacion.h"

void send_proceso_a_cpu(PCB* pcb, int size_stream, int conexion_dispatch) {
    int bytes_a_enviar = (sizeof(int) * 5) + size_stream;
    void* a_enviar = serializar_proceso(pcb, bytes_a_enviar);
    send(conexion_dispatch, a_enviar, bytes_a_enviar, 0);
    free(a_enviar);
}

void* serializar_proceso(PCB* pcb, int bytes) {
    void* paquete = malloc(bytes);
    
    int* codigo = malloc(sizeof(int));
    *codigo = EXEC_PROCESO;

    int tam_stream = bytes - (sizeof(int) * 5);

    int offset = 0;
    memcpy(paquete, &(*codigo), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete + offset, &(pcb->pid), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete + offset, &(pcb->program_counter), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete + offset, &(pcb->tabla_paginas), sizeof(int));
	offset += sizeof(int);
    memcpy(paquete + offset, &(tam_stream), sizeof(int));
    offset += sizeof(int);
    memcpy(paquete + offset, pcb->stream, tam_stream);

    free(codigo);

    return paquete;
}