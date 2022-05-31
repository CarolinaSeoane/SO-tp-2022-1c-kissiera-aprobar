#include "../include/serializacion.h"

void send_proceso_a_cpu(PCB* pcb, int size_stream) {
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

int solicitar_tabla_de_paginas_a_memoria(PCB* proceso) {   
    int bytes_a_enviar = sizeof(int) * 3; //PID, OPERACION, TAM PROCESO
	void* a_enviar = malloc(bytes_a_enviar);
    int* codigo = malloc(sizeof(int));
    *codigo = INIT_PROCESO;
    int offset = 0;

    memcpy(a_enviar, &(*codigo), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &(proceso->pid), sizeof(int));
	offset += sizeof(int);
    memcpy(a_enviar + offset, &(proceso->tamanio_proceso), sizeof(int));

    send(conexion_memoria, a_enviar, bytes_a_enviar, 0);
    free(codigo);
    free(a_enviar);
    
    int tabla_primer_nivel;
    recv(conexion_memoria, &tabla_primer_nivel, sizeof(int), 0);
    return tabla_primer_nivel;
}

int solicitar_swap_out_a_memoria(PCB* proceso) { // hacer 
    int bytes_a_enviar = sizeof(int) * 3 ; 
	void* a_enviar = malloc(bytes_a_enviar);
    int* codigo = malloc(sizeof(int));
    *codigo = INIT_PROCESO;
    int offset = 0;

    memcpy(a_enviar, &(*codigo), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &(proceso->pid), sizeof(int));
	offset += sizeof(int);
    memcpy(a_enviar + offset, &(proceso->tabla_paginas), sizeof(int));


    send(conexion_memoria, a_enviar, bytes_a_enviar, 0);
    free(codigo);
    free(a_enviar);
    
    uint32_t tabla_primer_nivel;
    recv(conexion_memoria, &tabla_primer_nivel, sizeof(uint32_t), 0);
    log_info(logger, "Recibi para el proceso %d la tabla_primer_nivel con valor %d", proceso->pid, tabla_primer_nivel);
    return 7;//tabla_primer_nivel;
}

void recv_proceso_bloqueado(int* pid, int* pc, int* tiempo_bloqueo) {
    recv(conexion_dispatch, pid, sizeof(int), 0);
    recv(conexion_dispatch, pc, sizeof(int), 0);
    recv(conexion_dispatch, tiempo_bloqueo, sizeof(int), 0);
}

void recv_exit_proceso(int* pid_a_finalizar, int* program_counter) {
    recv(conexion_dispatch, pid_a_finalizar, sizeof(int), 0);
    recv(conexion_dispatch, program_counter, sizeof(int), 0);
}