#include "../include/conexiones_kernel.h"

void recv_proceso(Proceso_CPU* proceso) {

	recv(cliente_dispatch, &(proceso->pid), sizeof(int), 0);

	recv(cliente_dispatch, &(proceso->program_counter), sizeof(int), 0);

	recv(cliente_dispatch, &(proceso->tabla_paginas), sizeof(int), 0);

	int tam_stream;
	recv(cliente_dispatch, &tam_stream, sizeof(int), 0);

	proceso->stream = malloc(tam_stream);
	recv(cliente_dispatch, proceso->stream, tam_stream, 0);
	//mostrar_instrucciones(proceso->stream, tam_stream/sizeof(instruccion));
}

// Alcanza con mandar 4 int: CODIGO_OP + PID + PC + TIEMPO_BLOQUEO 
void serializar_proceso_bloqueado(Proceso_CPU* proceso, int tiempo_bloqueo, void* paquete_bloqueo) {
    int offset = 0;
    
    int* codigo = malloc(sizeof(int));
	*codigo = BLOCK_PROCESO;
	memcpy(paquete_bloqueo, &(*codigo), sizeof(int));
    
    offset += sizeof(int);
    memcpy(paquete_bloqueo + offset, &(*proceso).pid, sizeof(int));
    
    offset += sizeof(int);
    memcpy(paquete_bloqueo + offset, &(*proceso).program_counter, sizeof(int));
    
    offset += sizeof(int);
    memcpy(paquete_bloqueo + offset, &tiempo_bloqueo, sizeof(int));

    free(codigo);

}

// Alcanza con mandar 3 int: CODIGO_OP + PID + PC
void serializar_proceso_finalizado(Proceso_CPU* proceso, void* paquete_bloqueo) {
    int offset = 0;
    
    int* codigo = malloc(sizeof(int));
	*codigo = EXIT_PROCESO;
	memcpy(paquete_bloqueo, &(*codigo), sizeof(int));
    
    offset += sizeof(int);
    memcpy(paquete_bloqueo + offset, &(proceso->pid), sizeof(int));
    
    offset += sizeof(int);
    memcpy(paquete_bloqueo + offset, &(proceso->program_counter), sizeof(int));

    free(codigo);

}

void serializar_proceso_desalojado(Proceso_CPU* proceso, void* paquete_desalojado) {
    int offset = 0;
    
    int* codigo = malloc(sizeof(int));
	*codigo = DESALOJO_PROCESO;
	memcpy(paquete_desalojado, &(*codigo), sizeof(int));
    
    offset += sizeof(int);
    memcpy(paquete_desalojado + offset, &(*proceso).pid, sizeof(int));
    
    offset += sizeof(int);
    memcpy(paquete_desalojado + offset, &(*proceso).program_counter, sizeof(int));

    free(codigo);
}

void send_proceso_bloqueado(Proceso_CPU* proceso, int tiempo_bloqueo) {
    void* paquete_bloqueo = malloc(sizeof(int)*4);
    serializar_proceso_bloqueado(proceso, tiempo_bloqueo, paquete_bloqueo);
    send(cliente_dispatch, paquete_bloqueo, sizeof(int)*4, 0);
    free(paquete_bloqueo);
}

void send_proceso_finalizado(Proceso_CPU* proceso) {
    void* paquete_finalizado = malloc(sizeof(int)*3);
    serializar_proceso_finalizado(proceso, paquete_finalizado);
    log_info(logger, "por hacer el send");
    send(cliente_dispatch, paquete_finalizado, sizeof(int)*3, 0);
    log_info(logger, "hice el send");
    free(paquete_finalizado);
}

void send_proceso_desalojado(Proceso_CPU* proceso) {
    void* paquete_desalojado = malloc(sizeof(int)*3);
    serializar_proceso_desalojado(proceso, paquete_desalojado);
    send(cliente_dispatch, paquete_desalojado, sizeof(int)*3, 0);
    free(paquete_desalojado);
}

// Hay mucho código repetido, se podría mejorar